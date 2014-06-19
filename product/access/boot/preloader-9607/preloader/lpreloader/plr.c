#include "pblr.h"


// assume DRAM is usable, and the all variables are stored in DRAM

#define HEALTH_TH_B0  4
#define HEALTH_TH_A0  2


// for allocating chunk/block
static u32_t num_chunk_per_block;
static u32_t chunk_size;
static oob_t **block_oob;
static u16_t *block_age;
static u32_t chunk_ecc_health_th;
static u32_t oldest_block, youngest_block;


#define sizeof_nand_dma_buf _sizeof_nand_dma_buf(chunk_size)
static int 
nand_read_chunk(u8_t *chunk_buf, spare_u *spare, u32_t block_id, u32_t bchunk_id) {
    // 1. D-cache flush among dma_buf
    // 2. D-cache invalidate dma_buf
    return (parameters._nand_read_chunk)(chunk_buf, spare, block_id*num_chunk_per_block+bchunk_id);
}
static int
nand_write_chunk(u8_t *chunk_buf, spare_u *spare, u32_t block_id, u32_t bchunk_id) {
    // 1. D-cache flush among dma_buf
    return (parameters._nand_write_chunk)(chunk_buf, 
        spare, block_id*num_chunk_per_block+bchunk_id);
}

static inline int
nand_read_dma_buf(nand_dma_buf_t *dma_buf, u32_t block_id, u32_t bchunk_id) {
    return nand_read_chunk(dma_buf->chunk_buf, &(dma_buf->spare), block_id, bchunk_id);
}
static inline int
nand_write_dma_buf(nand_dma_buf_t *dma_buf, u32_t block_id, u32_t bchunk_id) {
    return nand_write_chunk(dma_buf->chunk_buf, &(dma_buf->spare), block_id, bchunk_id);
}

static void
update_oldest_youngest_block(void) {
    u32_t oa=0, ya=0xffff;
    u32_t b, ob=0, yb=0;
    u32_t end_block=parameters.end_pblr_block;
    
    for (b=flash_info.pblr_start_block; b<end_block; ++b) {
        u32_t ba=block_age[b];
        if (ba==0) continue; // skip bad block
            
        if (oa<ba) {
            oa=ba;
            ob=b;
        } else if (ya>ba) {
            ya=ba;
            yb=b;
        }
    }
    oldest_block=ob;
    youngest_block=yb;
}

static void
erase_and_update_block_oob(u32_t block_id) {
    u32_t bc;
    oob_t *oobs=block_oob[block_id];
    oob_t new_oob={
        .signature=OOB_SIG_ERASED,
        .age=OOB_ERASED_AGE,
        .ver=0xffff,
        .chunk_id=0xffff,
        .num_chunk=0xffff};
    (parameters._nand_erase_block)(block_id*num_chunk_per_block);
    for (bc=0;bc<num_chunk_per_block;++bc) oobs[bc]=new_oob;
    
    u32_t youngest_age=block_age[youngest_block];
    block_age[block_id]=(youngest_age>0)?(youngest_age-1):0;
    update_oldest_youngest_block();
}

// if found it, retun 0, and *chunk and *block carry the info
// otherwise return -1
static int
find_chunk_to_write(u32_t *block_id, u32_t *bchunk_id) {
    u32_t c;
    u32_t b=youngest_block;

    oob_t *oobs=block_oob[b];
    if (oobs!=VZERO) {
        for (c=0;c<num_chunk_per_block;++c) {
            // check if any erased block 
            if (oobs[c].signature==OOB_SIG_ERASED) break;
        }
        if (c>=num_chunk_per_block) {
            // not found a earsed chunk
            // try to erase a oldest block for the allocation
            b=oldest_block;
            c=0;
            // ASSUME: the number of block is so sufficient that victim block should not contain current version
            // that is, there is no need to implement garbage collection mechanism
            if ((block_oob[b])[0].signature!=OOB_SIG_ERASED) 
                erase_and_update_block_oob(b);
        }
        *bchunk_id=c;
        *block_id=b;
        return 0;
    } else {
        pblr_printf("youngest block is a bad block\n");
        return -1;
    }
}
// when equal, return 0
static int
compare_integrity(const nand_dma_buf_t *dma_buf, const u8_t *chunk_buf, const oob_t *oob) {
    if (pbldr_memcmp(&(dma_buf->spare.oob), oob, sizeof(oob_t))!=0) return -1;
    if (pbldr_wmemcmp(dma_buf->chunk_buf, chunk_buf, 
            chunk_size/sizeof(u32_t))!=0) return -1;
    return 0;
}

static int
append_a_chunk(u8_t *chunk_buf, u32_t chunk_id, u32_t signature, u32_t num_chunk) {
    // define a cache aligned buffer including a spare_u and a dma_buf
    u8_t *_cache_aligned_buf=alloca(sizeof(spare_u)+sizeof_nand_dma_buf+CACHE_SIZE-4);
    spare_u *spare=(spare_u *)CACHE_ALIGN(_cache_aligned_buf);
    nand_dma_buf_t *dma_buf=(nand_dma_buf_t *)(spare+1);

    u32_t b, bc;
    int ret, ecc, max_try;
    
    pbldr_wzero(spare, sizeof(spare_u)/sizeof(u32_t));
    spare->oob.signature=signature; //is_preloader?OOB_SIG_USED_PLR:OOB_SIG_USED_BLR;
    spare->oob.ver=parameters.curr_ver;
    spare->oob.chunk_id=chunk_id;
    spare->oob.num_chunk=num_chunk; //is_preloader?parameters.plr_num_chunk:parameters.blr_num_chunk;

    for (max_try=MAX_WRITE_TRY;max_try>0;--max_try) {
        if (find_chunk_to_write(&b, &bc)<0) return -1;
        
        spare->oob.age=block_age[b];
        *pbldr_spare_bbi(spare)=0xff;
        ret=(nand_write_chunk(chunk_buf, spare, b, bc)==0) &&
            ((ecc=nand_read_dma_buf(dma_buf, b, bc))>=0) &&
            (ecc<=chunk_ecc_health_th) &&
            (compare_integrity(dma_buf, chunk_buf, &(spare->oob))==0);
        
        if (ret==0) {
            if (parameters.flash_info.enable_plr_gc) {
                // write fail, or read fail
                if (bc==0) {
                    // if the failure is in chunk 0, erase again and mark reserved
                    erase_and_update_block_oob(b);
                    pbldr_wzero(dma_buf, sizeof_nand_dma_buf/sizeof(u32_t)); // bbi and age are cleaned, too
                    dma_buf->spare.oob.signature=OOB_SIG_RESERVED;
                    nand_write_dma_buf(dma_buf, b, bc);
                }
                (block_oob[b])[bc]=dma_buf->spare.oob;
            } else {
                pblr_printf("unable to find a chunk for gc\n");
                return -1;
            }
        } else {
            // write success
            (block_oob[b])[bc]=spare->oob;
            return 0;
        }
    }
    pblr_printf("unable to fix a chunk\n");
    return -1;
}

// stack should be moved to DRAM
int 
load_blr(void) {
    int b, c, ecc, ret;
    
    // health counters and success flags for each chunk
    u8_t plr_hc_cnts[MAX_PLR_CHUNK];
    u8_t blr_hc_cnts[MAX_BLR_CHUNK];
    u32_t blr_succ_flags[NUM_WORD(MAX_BLR_CHUNK)];
    
    // constants derived from flash_info or parameters
    chunk_ecc_health_th=(parameters.soc_id==SOC_B0)?HEALTH_TH_B0:HEALTH_TH_A0;
    num_chunk_per_block=flash_info_num_chunk_per_block;
    chunk_size=flash_info.page_size*flash_info.page_per_chunk;

    // buffer for nand dma
    u8_t *_dma_buf=alloca(sizeof_nand_dma_buf+CACHE_SIZE-4);
    nand_dma_buf_t *dma_buf=(nand_dma_buf_t *)CACHE_ALIGN(_dma_buf);
    u8_t *chunk_buf=dma_buf->chunk_buf;
    
    // oob arrays
    u32_t all_oob_size=sizeof(oob_t)*num_chunk_per_block*flash_info.num_pblr_block;
    oob_t *all_oobs=(oob_t *)alloca(all_oob_size);
    block_oob=(oob_t **)alloca(sizeof(oob_t*)*parameters.end_pblr_block);
    block_age=(u16_t*)alloca(sizeof(u16_t)*parameters.end_pblr_block);

    // allocate space for oob
    pbldr_wzero(all_oobs, all_oob_size/sizeof(u32_t));
    for (b=0;b<parameters.end_pblr_block;b++) {
        if ((b<flash_info.pblr_start_block)||bbt_is_bad_block(b*num_chunk_per_block)) {
            block_oob[b]=VZERO;
        } else {
            block_oob[b]=all_oobs;
            all_oobs+=num_chunk_per_block;
        }
    }
    
    // load boot loader and scan oob
    oob_t *oob=&(dma_buf->spare.oob);
    pbldr_bzero(plr_hc_cnts, MAX_PLR_CHUNK);
    pbldr_bzero(blr_hc_cnts, MAX_BLR_CHUNK);
    pbldr_bzero(block_age, sizeof(u16_t)*parameters.end_pblr_block);
    for (b=flash_info.pblr_start_block;b<parameters.end_pblr_block;++b) {
        if (block_oob[b]==VZERO) continue;
        
        u32_t age=OOB_ERASED_AGE;
        for (c=0;c<num_chunk_per_block; ++c) {
            ecc=nand_read_dma_buf(dma_buf, b, c);
            (block_oob[b])[c]=*oob;
            u32_t sig=oob->signature;
            if (ecc<0) continue;
            if ((sig!=OOB_SIG_USED_PLR)&& 
                (sig!=OOB_SIG_USED_BLR)) continue;
            if (age > oob->age) age=oob->age;
            if (oob->ver!=parameters.curr_ver) continue;
            
            u32_t cid=oob->chunk_id;
            u32_t is_health=(ecc<=chunk_ecc_health_th);
            if (sig==OOB_SIG_USED_PLR) {
                plr_hc_cnts[cid] += is_health;
            } else if (sig==OOB_SIG_USED_BLR) {
                _set_flags(blr_succ_flags, cid);
                blr_hc_cnts[cid] += is_health;
                // copy to its location
                u8_t *dst=blr_addr_by_chunck_id(cid, chunk_size);
                pbldr_wmemcpy(dst, chunk_buf, chunk_size/sizeof(u32_t));
            } else {
                // unreachable
            }
        }
        block_age[b]=age;
    }
    
    // ajdust oldest/youngest block and allocating block/chunk
    update_oldest_youngest_block();
    
    // check if bootloader is read incompletely, it unlike happens
    if (check_if_all_one(blr_succ_flags, parameters.blr_num_chunk)!=0) return -1;
    
    // FTx doesn't fix health chunk
    if (parameters.flash_info.enable_plr_hcheck==0) return 0;
    if (!FIHT_IS_NORMAL(flash_info.header_type)) return 0;
    
    // scan healthy for preloader
    int plr_num_chunk=parameters.plr_num_chunk;
    ret=0;
    for (c=0;c<plr_num_chunk;++c) {
        if (plr_hc_cnts[c]<2) {
            u8_t *src=plr_addr_by_chunck_id(c, chunk_size);
            if ((ret=append_a_chunk(src, c, OOB_SIG_USED_PLR, 
                parameters.plr_num_chunk))<0) break;
        }
    }
    if (ret==0) {
        int blr_num_chunk=parameters.blr_num_chunk;
        for (c=0;c<blr_num_chunk;++c) {
            if (blr_hc_cnts[c]<2) {
                u8_t *src=blr_addr_by_chunck_id(c, chunk_size);
                if ((ret=append_a_chunk(src, c, OOB_SIG_USED_BLR, 
                    parameters.blr_num_chunk))<0) break;
            }
        }
    }
    if (ret<0) {
        pblr_printf("append a chunk for preloader/bootloader: fail\n");
    }
    
    return 0;
}

