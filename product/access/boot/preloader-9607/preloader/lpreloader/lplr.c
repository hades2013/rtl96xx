#include "pblr.h"

// nand flash control without cache operations (cache flush and invlidate)
// return number ecc; -1 for fail to read
int 
_nand_read_chunk(u8_t *chunk_buf, spare_u *spare, u32_t chunk_id) {
    // 1. chunk_buf and oob_buf should be translated to logical addresses
    // 2. read from nand by a loop
    // 3. reassemble oob
    return 0;
}

u8_t
_nand_read_bbi(u32_t chunk_id) {
    // using raw read
    return 0;
}

int
_nand_write_chunk(u8_t *chunk_buf, spare_u *spare, u32_t chunk_id) {
    // 1. chunk_buf and oob_buf should be translated to logical addresses
    // 2. reassemble oob
    // 3. write to nand by a loop
    return 0;
}

int
_nand_erase_block(u32_t chunk_id) {
    return 0;
}

static u32_t chunk_size;
static u32_t num_chunk_per_block;
static int
load_plr(nand_dma_buf_t *dma_buf) {
    u32_t plr_succ_flags[NUM_WORD(MAX_PLR_CHUNK)]; // a bit stream recording, it indicats loaded successfully for each chunk of plr
    u32_t blr_succ_flags[NUM_WORD(MAX_BLR_CHUNK)]; // a bit stream recording, it indicats loaded successfully for each chunk of blr    
    
    // uncached address
    u8_t *chunk_buf=(u8_t*)UADDR(dma_buf->chunk_buf);   
    spare_u *spare=(spare_u *)UADDR(&(dma_buf->spare));         
    
    int c, ecc;
    u16_t ver_cap=0xffff;

    do {
        int n=flash_info.num_pblr_block;
        int b=flash_info.pblr_start_block-1;
        parameters.curr_ver=0;
        while((++b,n>0)) {
            if (bbt_is_bad_block(b*num_chunk_per_block)) continue;
            --n;
            for (c=0;c<num_chunk_per_block; ++c) {
                if ((ecc=_nand_read_chunk(chunk_buf, spare, b*num_chunk_per_block+c))<0) continue;
                if ((spare->oob.signature!=OOB_SIG_USED_PLR)&& 
                    (spare->oob.signature!=OOB_SIG_USED_BLR)) continue;
                
                u16_t ver=spare->oob.ver;
                if ((ver<parameters.curr_ver)||(ver>=ver_cap)) continue;
                if (ver>parameters.curr_ver) {
                    parameters.curr_ver=ver;
                    parameters.plr_num_chunk=0;
                    parameters.blr_num_chunk=0;
                    pbldr_wzero(plr_succ_flags, NUM_WORD(MAX_PLR_CHUNK));
                    pbldr_wzero(blr_succ_flags, NUM_WORD(MAX_BLR_CHUNK));
                }
                u16_t cid=spare->oob.chunk_id;
                if (spare->oob.signature==OOB_SIG_USED_PLR) {
                    if (cid==0) parameters.plr_num_chunk=spare->oob.num_chunk;
                    if (_get_flags(plr_succ_flags, cid)==0) {
                        // copy chunk_buf to the right location
                        u8_t *dst=plr_addr_by_chunck_id(cid, chunk_size);
                        pbldr_wmemcpy(dst, chunk_buf, chunk_size/sizeof(u32_t));
                        _set_flags(plr_succ_flags, cid);
                    }
                } else if (spare->oob.signature==OOB_SIG_USED_BLR) {
                    if (cid==0) parameters.blr_num_chunk=spare->oob.num_chunk;
                    _set_flags(blr_succ_flags, cid);
                } else {
                    // unreachable
                }
            }
        }
        parameters.end_pblr_block=b;
        // check if preload and bootloader are both successfully read
        if ((check_if_all_one(plr_succ_flags, parameters.plr_num_chunk)==0)&&
            (check_if_all_one(blr_succ_flags, parameters.blr_num_chunk)==0))
            return 0;
        
        // need more scans
    } while ((ver_cap=parameters.curr_ver)>0);
    return -1;
}

static void
create_bbt(nand_dma_buf_t *dma_buf) {
    u32_t b, ecc;
    u8_t bbi;
    
    // when FTx, all blocks are assumed well
    if (!FIHT_IS_NORMAL(flash_info.header_type)) return;

    pbldr_wzero(parameters.bbt, NUM_WORD(MAX_BLOCKS));
    for (b=1;b<flash_info.num_block;++b) {
        bbi=_nand_read_bbi(b*num_chunk_per_block);
        if (bbi==0xFF) continue;
        if ((ecc=_nand_read_chunk(dma_buf->chunk_buf, &dma_buf->spare, b*num_chunk_per_block))<0) {
            _set_flags(parameters.bbt, b);
            continue;
        }
        u8_t oob_bbi=*pbldr_spare_bbi(&(dma_buf->spare));
        if (oob_bbi==0xFF) continue;
        if ((oob_bbi==0x00) && (dma_buf->spare.oob.signature==OOB_SIG_RESERVED)) continue;
        
        // mark bad block
        _set_flags(parameters.bbt, b);
    }
    parameters.bbt_valid=1;
}

static int 
nand_operations(void) {
    // allocate dma buffer and cache align
    u8_t *_dma_buf=alloca(_sizeof_nand_dma_buf(chunk_size)+CACHE_SIZE-4);
    nand_dma_buf_t *dma_buf=(nand_dma_buf_t *)CACHE_ALIGN(_dma_buf);

    // create bad block table
    create_bbt(dma_buf);
    
    // load pre-loader
    return load_plr(dma_buf);
}

// before running C_start, stack point should be (parameters - 4), 
//  I/D-cache should be init
void
C_start(void) {
    // check soc_configuration.signature1 and soc_configuration.signature2
    if ((soc_configuration.signature1!=FC_SIG1) || 
        (soc_configuration.signature2!=FC_SIG2)) {
        while(1);
    }

    // copy parameters from soc_configuration to parameter_to_bootloader
    pbldr_wzero(&parameters, sizeof(parameter_to_bootloader_t)/sizeof(u32_t));
    pbldr_wmemcpy((plr_flash_info_t*)&flash_info, 
        &(soc_configuration.info), sizeof(plr_flash_info_t)/sizeof(u32_t));

    // set up function points
    parameters._nand_read_bbi=_nand_read_bbi;
    parameters._nand_read_chunk=_nand_read_chunk;
    parameters._nand_write_chunk=_nand_write_chunk;
    parameters._nand_erase_block=_nand_erase_block;

    // create nand dynamic information and load pre-loader
    chunk_size=flash_info.page_size*flash_info.page_per_chunk;
    num_chunk_per_block=flash_info_num_chunk_per_block;
    if (nand_operations()!=0) while(1);
    
    // jump to preloader (=SYSTEM_STARTUP_CADDR+LPLR_DMA_SIZE)
    void (*preloader)(void)=(void*)SYSTEM_STARTUP_CADDR+LPLR_DMA_SIZE;
    preloader();
}
