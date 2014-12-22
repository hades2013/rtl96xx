#include <preloader.h>
#include <pblr.h>
#include "soc_nand_flash.h"

#define HEALTH_TH_B0  4
#define HEALTH_TH_A0  2

// for allocating chunk/block
//static u32_t num_chunk_per_block;
//static u32_t chunk_size;
static oob_t **block_oob;
static u16_t *block_age;
static u32_t chunk_ecc_health_th;
static u32_t oldest_block, youngest_block;

#define sizeof_nand_dma_buf _sizeof_nand_dma_buf(para_chunk_size)

inline static u8_t*
uboot_addr_by_chunck_id(u32_t cid, u32_t chunk_size) {
    //	return (u8_t*)SYSTEM_STARTUP_CADDR+LPLR_DMA_SIZE+cid*chunk_size;
    //return (u8_t*)UBOOT_BIN_LZMA_BASE+cid*chunk_size;
   //   return (u8_t*)UBOOT_BIN_BASE_NAND+cid*para_chunk_size;
       return (u8_t*)DRAM_UBOOT_ADDR +cid*para_chunk_size;
}


static int 
nand_read_chunk(u8_t *chunk_buf, spare_u *spare, u32_t block_id, u32_t bchunk_id) {
    // 1. D-cache flush among dma_buf
    // 2. D-cache invalidate dma_buf
    return (parameters._nand_read_chunk)(chunk_buf, spare, block_id*para_num_chunk_per_block+bchunk_id);
}
static int
nand_write_chunk(u8_t *chunk_buf, spare_u *spare, u32_t block_id, u32_t bchunk_id) {
    // 1. D-cache flush among dma_buf
    return (parameters._nand_write_chunk)(chunk_buf, 
        spare, block_id*para_num_chunk_per_block+bchunk_id);
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
    
    for (b=para_flash_info.pblr_start_block; b<end_block; ++b) {
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
    (parameters._nand_erase_block)(block_id*para_num_chunk_per_block);
    for (bc=0;bc<para_num_chunk_per_block;++bc) oobs[bc]=new_oob;
    
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
	//printf("find_chunk_to_write bchunk_id %d\n",bchunk_id);
    if (oobs!=VZERO) {
        for (c=0;c<para_num_chunk_per_block;++c) {
            // check if any erased block 
            if (oobs[c].signature==OOB_SIG_ERASED) break;
        }
        if (c>=para_num_chunk_per_block) {
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
        //printf("youngest block is a bad block\n");
        return -1;
    }
}
// when equal, return 0
static int
compare_integrity(const nand_dma_buf_t *dma_buf, const u8_t *chunk_buf, const oob_t *oob) {
    if (pblr_memcmp(&(dma_buf->spare.oob), oob, sizeof(oob_t))!=0) return -1;
    if (pblr_memcmp(dma_buf->chunk_buf, chunk_buf, 
            para_chunk_size)!=0) return -1;
    return 0;
}

static int
append_a_chunk(u8_t *chunk_buf, u32_t chunk_id, u32_t signature, u32_t num_chunk) {
    // define a cache aligned buffer including a spare_u and a dma_buf
    u8_t *_cache_aligned_buf=alloca(sizeof(spare_u)+sizeof_nand_dma_buf+CACHELINE_SIZE-4);
    spare_u *spare=(spare_u *)CACHE_ALIGN(_cache_aligned_buf);
    nand_dma_buf_t *dma_buf=(nand_dma_buf_t *)(spare+1);

    u32_t b, bc;
    int ret, ecc, max_try;
    
    pbldr_wzero(spare, sizeof(spare_u)/sizeof(u32_t));
    spare->oob.signature=signature; //is_preloader?OOB_SIG_USED_PLR:OOB_SIG_USED_BLR;
    spare->oob.ver=parameters.curr_ver;
    spare->oob.chunk_id=chunk_id;
    spare->oob.num_chunk=num_chunk; //is_preloader?parameters.plr_num_chunk:parameters.blr_num_chunk;
	//printf("append_a_chunk chunkid %d\n",chunk_id);
    for (max_try=MAX_WRITE_TRY;max_try>0;--max_try) {
        if (find_chunk_to_write(&b, &bc)<0) return -1;
        
        spare->oob.age=block_age[b];
        *pbldr_spare_bbi(spare)=0xff;
        ret=(nand_write_chunk(chunk_buf, spare, b, bc)==0) &&
            ((ecc=nand_read_dma_buf(dma_buf, b, bc))>=0) &&
            (ecc<=(int)chunk_ecc_health_th) &&
            (compare_integrity(dma_buf, chunk_buf, &(spare->oob))==0);
        
        if (ret==0) {
			
            if (para_flash_info.enable_plr_gc) {
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
                printf("unable to find a chunk for gc\n");
                return -1;
            }
        } else {
            // write success
            (block_oob[b])[bc]=spare->oob;
            return 0;
        }
    }
    printf("unable to fix a chunk\n");
    return -1;
}

int 
load_blr(void) {
    u32_t b;
    int c, ecc, ret;
    
    // health counters and success flags for each chunk
    u8_t plr_hc_cnts[MAX_PLR_CHUNK];
    u8_t blr_hc_cnts[MAX_BLR_CHUNK];
    u32_t blr_succ_flags[NUM_WORD(MAX_BLR_CHUNK)];
    u32_t num_chunk_per_block=para_num_chunk_per_block;
    
    // constants derived from flash_info or parameters
    chunk_ecc_health_th=(parameters.soc_id==SOC_B0)?HEALTH_TH_B0:HEALTH_TH_A0;
    //num_chunk_per_block=flash_info_num_chunk_per_block;
    //chunk_size=para_flash_info.page_size*para_flash_info.page_per_chunk;

    // buffer for nand dma
    u8_t *_dma_buf=alloca(sizeof_nand_dma_buf+CACHELINE_SIZE-4);
    nand_dma_buf_t *dma_buf=(nand_dma_buf_t *)CACHE_ALIGN(_dma_buf);
    u8_t *chunk_buf=dma_buf->chunk_buf;
    
    // oob arrays
    u32_t all_oob_size=sizeof(oob_t)*num_chunk_per_block*para_flash_info.num_pblr_block;
    oob_t *all_oobs=(oob_t *)alloca(all_oob_size);
    block_oob=(oob_t **)alloca(sizeof(oob_t*)*parameters.end_pblr_block);
    block_age=(u16_t*)alloca(sizeof(u16_t)*parameters.end_pblr_block);

    // allocate space for oob
    pbldr_wzero(all_oobs, all_oob_size/sizeof(u32_t));
    for (b=0;b<parameters.end_pblr_block;b++) {
        if ((b<para_flash_info.pblr_start_block)||bbt_is_bad_block(b*num_chunk_per_block)) {
            block_oob[b]=VZERO;
        } else {
            block_oob[b]=all_oobs;
            all_oobs+=num_chunk_per_block;
        }
    }
    
    // load boot loader and scan oob
    oob_t *oob=&(dma_buf->spare.oob);
    bzero(plr_hc_cnts, MAX_PLR_CHUNK);
    bzero(blr_hc_cnts, MAX_BLR_CHUNK);
    bzero(block_age, sizeof(u16_t)*parameters.end_pblr_block);
	//printf("in load_blr function\n");
	//printf("flash_info.pblr_start_block : %d\n",para_flash_info.pblr_start_block);
	//printf("parameters.end_pblr_block : %d\n",parameters.end_pblr_block);
	//printf("parameters.curr_ver is %x\n",parameters.curr_ver);
	
    for (b=para_flash_info.pblr_start_block;b<parameters.end_pblr_block;++b) {
        if (block_oob[b]==VZERO) continue;
        
        u32_t age=OOB_ERASED_AGE;
        for (c=0;c<(int)num_chunk_per_block; ++c) {
			
//			pbldr_dma_cache_inv_range((u32_t *)dma_buf,sizeof_nand_dma_buf);
			parameters._dcache_writeback_invalidate_all();

			
            ecc=nand_read_dma_buf(dma_buf, b, c);
			//printf("load_blr b is %d cid is %d ",b,c);
            (block_oob[b])[c]=*oob;
            u32_t sig=oob->signature;
            if (ecc<0) {
				//printf("ecc fail\n");
				continue;
            }
			
			//printf("oob->sig is %x oob->ver: 0x%x\n",sig,oob->ver);
			
            if ((sig!=OOB_SIG_USED_PLR)&& 
                (sig!=OOB_SIG_USED_BLR)) {
            	    //printf("not p or b\n");
                	continue;
            }
			
            if (age > oob->age) age=oob->age;
            if (oob->ver!=parameters.curr_ver) continue;
            
            u32_t cid=oob->chunk_id;
            u32_t is_health=(ecc<=(int)chunk_ecc_health_th);
            if (sig==OOB_SIG_USED_PLR) {
                plr_hc_cnts[cid] += is_health;
            } else if (sig==OOB_SIG_USED_BLR) {
                _set_flags(blr_succ_flags, cid);
                blr_hc_cnts[cid] += is_health;
                // copy to its location
//                u8_t *dst=blr_addr_by_chunck_id(cid, para_chunk_size);
				 u8_t *dst=uboot_addr_by_chunck_id(cid, para_chunk_size);
				//printf("copy to dst  %p\n",dst);

                //pblr_wmemcpy(dst, chunk_buf, para_chunk_size/sizeof(u32_t));
                memcpy(dst, chunk_buf, para_chunk_size);
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
    if (para_flash_info.enable_plr_hcheck==0) return 0;
	if (!FIHT_IS_NORMAL(parameters.soc.header_type)) return 0;

    
    // scan healthy for preloader
    int plr_num_chunk=parameters.plr_num_chunk;
    ret=0;
    for (c=0;c<plr_num_chunk;++c) {
        if (plr_hc_cnts[c]<2) {
            u8_t *src=plr_addr_by_chunck_id(c, para_chunk_size);
            if ((ret=append_a_chunk(src, c, OOB_SIG_USED_PLR, 
                parameters.plr_num_chunk))<0) break;
        }
    }
    if (ret==0) {
        int blr_num_chunk=parameters.blr_num_chunk;
        for (c=0;c<blr_num_chunk;++c) {
            if (blr_hc_cnts[c]<2) {
                u8_t *src=blr_addr_by_chunck_id(c, para_chunk_size);
                if ((ret=append_a_chunk(src, c, OOB_SIG_USED_BLR, 
                    parameters.blr_num_chunk))<0) break;
            }
        }
    }
    if (ret<0) {
        printf("append a chunk for preloader/bootloader: fail\n");
    }
    
    return 0;
}

void flash_init(void) {
	printf("II: Flash... OK\n");
    parameters._nand_write_chunk=&_nand_write_chunk;
	parameters._nand_erase_block=&_nand_erase_block;
	parameters._nand_pio_write=&_nand_PIO_write;
	parameters.flash_init_result=INI_RES_OK;
	return;
}

/**
**  _nand_write_chunk()
**	descriptions: write one chunk to nand flash.
**	parameters:  chunk id  (must alignment of real_page/block)
**	return:  0:succes,-1 is fail
**  note: this function not check bad block table.
**/


int
_nand_write_chunk(u8_t *chunk_buf, spare_u *spare, u32_t chunk_id) {
	int real_page = chunk_id*para_flash_info.page_per_chunk; //get real_page number
	int dma_counter = 4; //always 512byte * 4
	int dram_sa, oob_sa;
	int page_shift;
	int page_num[3];
	unsigned long flash_addr_t=0;
	unsigned long flash_addr2_t=0;
	int buf_pos=0;
	int return_value=0;
	int ecc_count=0;
//	int i;
	int block = real_page/para_flash_info.num_page_per_block;

	//u8_t *oob_area, oob_area0[28]; //test chip must 16byte aligment

	u8_t *oob_area;
	u8_t *data_area, data_area0[512+16+CACHELINE_SIZE+CACHELINE_SIZE-4]; //512+16 and start/end cache line alignment

	
//	pbldr_bset(oob_area0, 0xff,28);
	pblr_bset(data_area0, 0xff, 512+16+CACHELINE_SIZE+CACHELINE_SIZE-4);

//	oob_area = (u8_t*) ((u32_t)(oob_area0 + 12) & 0xFFFFFFF0);
//	data_area = (u8_t*) ((u32_t)(data_area0 + CACHELINE_SIZE-4) & 0xFFFFFFF0);
	data_area=(u8_t*) (u32_t) CACHE_ALIGN(data_area0);

	oob_area=(u8_t*) data_area+512;
	
#if 0
	pbldr_dma_cache_inv_range((u32_t *)chunk_buf,2048);
	pbldr_dma_cache_inv_range((u32_t *)spare->u8_oob,64);
	pbldr_dma_cache_inv_range((u32_t *)data_area,512+16+CACHELINE_SIZE-4);
#endif
	parameters._dcache_writeback_invalidate_all();



	if(block>0){
		//swap bbi and data
		spare->u8_oob[para_flash_info.bbi_swap_offset]=chunk_buf[para_flash_info.bbi_dma_offset];
 	 	chunk_buf[para_flash_info.bbi_dma_offset]=0xff;
		//printf("after  chunk[%d] is %x \n",para_flash_info.bbi_dma_offset,chunk_buf[para_flash_info.bbi_dma_offset]);
		//printf("after oob_buf [%d] is %x \n",para_flash_info.bbi_swap_offset, spare->u8_oob[para_flash_info.bbi_swap_offset]); 
		
  	}
#if 0
	pbldr_dma_cache_inv_range((u32_t *)chunk_buf,2048);
	pbldr_dma_cache_inv_range((u32_t *)spare->u8_oob,64);
#endif
	parameters._dcache_writeback_invalidate_all();


	//set DMA flash start address

	for(page_shift=0;page_shift<3; page_shift++) {
		page_num[page_shift] = ((real_page>>(8*page_shift)) & 0xff);
		if(para_flash_info.page_size==2048){
			flash_addr_t |= (page_num[page_shift] << (12+8*page_shift));
		}else if(para_flash_info.page_size==4096){
			flash_addr_t |= (page_num[page_shift] << (13+8*page_shift));
		}else if(para_flash_info.page_size==512){ //512byte
			flash_addr_t |= (page_num[page_shift] << (9+8*page_shift));
		}
	}
	// If the nand flash size is larger than 2G bytes, fill the other address for DMA
	flash_addr2_t= (real_page >> 20);
		
	
 	oob_sa =  ( (u32_t)(oob_area ) & (~M_mask));

	dram_sa = ((u32_t)data_area) & (~M_mask);

	/* dma move  4*528byte */

  	while(dma_counter>0){

		//copy oob to buffer
		//pbldr_memcpy(oob_area, spare->u8_oob+(buf_pos*6), 6);
		//pbldr_memcpy(data_area, chunk_buf+(buf_pos*512), 512);
		memcpy(oob_area, spare->u8_oob+(buf_pos*6), 6);
		memcpy(data_area, chunk_buf+(buf_pos*512), 512);

#if 0
		pbldr_dma_cache_inv_range((u32_t *)data_area,512+CACHELINE_SIZE-4);
#endif
		parameters._dcache_writeback_invalidate_all();

//		pbldr_dma_cache_inv_range((u32_t *)oob_area,28);

		//get DMA RAM start address, add 512 bytes
	
	//	dram_sa = ( (u32_t)(chunk_buf+buf_pos*512) & (~M_mask));

		rtk_writel( rtk_readl(NACR) & (~RBO) & (~WBO) , NACR);
		//write data/oob address
		rtk_writel(dram_sa, NADRSAR);
		rtk_writel( oob_sa, NADTSAR);
		rtk_writel( flash_addr_t, NADFSAR);
#if 1 //BCH
		rtk_writel( flash_addr2_t, NADFSAR2);
#endif			
		//dma write cmd
		rtk_writel( ((DESC0|DMAWE|LBC_128) & (~TAG_DIS)),NADCRR);

		check_ready();


		ecc_count=parameters._nand_check_eccStatus();
	  //Record ecc counter ,return the max number
		if((return_value!=-1) && (ecc_count != -1))
		{
			return_value= (ecc_count>return_value)?ecc_count:return_value;
		}else{
			return_value=-1;
			//printk("\r\n _nand_write_chunk return -1\n");
		}

		flash_addr_t += (528); //512+16 one unit

		dma_counter--;
		buf_pos++;
	}



  
  if(block>0){
	  //swap bbi and data again. (recovery data)
	  chunk_buf[para_flash_info.bbi_dma_offset]= spare->u8_oob[para_flash_info.bbi_swap_offset];
//	  pbldr_dma_cache_inv_range((u32_t *)chunk_buf,2048);
//	  parameters._dcache_writeback_invalidate_all();

  }





  return return_value;
}

/**
**  check_ready()
**	descriptions: check nand flash busy/ready.
**	parameters:  none
**	return: none
**  note: exit loop when nand flash ready,else busy-waiting
**/
void check_ready()
{
	while(1) {
		if(  ( rtk_readl(NACR) & 0x80000000) == 0x80000000 )
			break;
	}
}



/**
**  _nand_erase_block()
**	descriptions: erase nand flash block
**	parameters:  chunk id  (must alignment of real_page/block)
**	return:  0:succes,-1 is fail
**  note: this function not check bad block table.
**/
int
_nand_erase_block(u32_t chunk_id) {

	int addr_cycle[5],page_shift;
	int real_page = chunk_id*para_flash_info.page_per_chunk; //get real_page number

	if ( real_page & (para_flash_info.num_page_per_block-1) ){
//		pblr_printf("%s: (page)chunk_id %d is not block alignment !!\n", __FUNCTION__, real_page);
		return -1;
	}

	rtk_writel( (rtk_readl(NACR) |ECC_enable), NACR);
	rtk_writel((NWER|NRER|NDRS|NDWS), NASR);
	rtk_writel(0x0, NACMR);

	rtk_writel((CECS0|CMD_BLK_ERASE_C1),NACMR); //Command register , write erase command (1 cycle)
	check_ready();

	if(para_flash_info.addr_cycles!=3){
	for(page_shift=0; page_shift<3; page_shift++){
		addr_cycle[page_shift] = (real_page>>(8*page_shift)) & 0xff;
	}

		rtk_writel( ((~enNextAD) & (AD2EN|AD1EN|AD0EN|(addr_cycle[0]<<CE_ADDR0) |(addr_cycle[1]<<CE_ADDR1)|(addr_cycle[2]<<CE_ADDR2))),NAADR); //NAND Flash Address Register1
		
	}else{
		addr_cycle[0] = 0;
		for(page_shift=0; page_shift<4; page_shift++){
			addr_cycle[page_shift+1] = (real_page>>(8*page_shift)) & 0xff;
		}		
		rtk_writel( (((~enNextAD) & AD2EN)|AD1EN|AD0EN|(addr_cycle[1]<<CE_ADDR0) |(addr_cycle[2]<<CE_ADDR1)|(addr_cycle[3]<<CE_ADDR2)),NAADR);
	}

	check_ready();

	rtk_writel((CECS0|CMD_BLK_ERASE_C2),NACMR); //write erase command cycle 2
	check_ready();

	rtk_writel((CECS0|CMD_BLK_ERASE_C3),NACMR);//read status
	check_ready();

	if ( rtk_readl(NADR) & 0x01 ){
//		pblr_printf("[%s] erase block is n	ot completed at block %d\n", __FUNCTION__, real_page/para_flash_info.num_page_per_block);
		return -1;
	}else{
		return 0;
  }

}

void nursing_uboot(void){
	load_blr();
}


/**	
**  _nand_PIO_write()
**	descriptions: PIO mode write data to flash 
**	parameters:  chunk_id (page id), length (write length)  buffer (data point)
**	return: 
**  Note: only for page size 2k 
**/

int 
_nand_PIO_write(u32_t chunk_id,u32_t length, u8_t * buffer) {
	int real_page = chunk_id*para_flash_info.page_per_chunk; //get real_page number
	int i;
	int page_size,oob_size;
	unsigned int flash_addr1=0, flash_addr2=0;
	int rlen, pio_length;
	page_size=para_flash_info.page_size;
	oob_size=64; //for 2k , always 64
	
	rtk_writel(0xc00fffff, NACR);
	pio_length = length;
	while(pio_length >0){

		if(pio_length > (page_size+oob_size)){
			rlen = (page_size+oob_size);
			pio_length -= (page_size+oob_size);
		}else{
			rlen = pio_length;
			pio_length -= rlen;
		}

		/* Command write cycle 1*/
		rtk_writel((CECS0|CMD_PG_WRITE_C1), NACMR);
		check_ready();
		
		if(para_flash_info.page_size==512){
			flash_addr1 |= ((real_page & 0xffffff) << 8);
		}else{
			flash_addr1 =  ((real_page & 0xff) << 16);
			flash_addr2 = (real_page >> 8) & 0xffffff;
		}

		//printk("flash_addr1 = 0x%08X, real_page = %d\n",flash_addr1, real_page);
		//printk("flash_addr2 = 0x%08X, \n",flash_addr2);			

		switch(para_flash_info.addr_cycles){
			case 3:
				rtk_writel( (AD2EN|AD1EN|AD0EN|flash_addr1), NAADR);
				check_ready();
				break;
			case 4:
				/* set address 1 */
				rtk_writel( (enNextAD|AD2EN|AD1EN|AD0EN|flash_addr1), NAADR);
				check_ready();				
				rtk_writel( (AD0EN|flash_addr2), NAADR);
				check_ready();
				break;
			case 5:
				rtk_writel( (enNextAD|AD2EN|AD1EN|AD0EN|flash_addr1), NAADR);
				check_ready();				
				rtk_writel( (AD1EN|AD0EN|flash_addr2), NAADR);
				check_ready();
				break;
		}


		for(i=0; i<(rlen/4); i++){
			//printk("pio write dram addr is  = 0x%08X ,data is %x \n",buffer+i*4,*(u32_t *)(buffer+i*4));
			rtk_writel( *(u32_t *)(buffer+i*4), NADR);
		}
		rtk_writel((CECS0|CMD_PG_WRITE_C2), NACMR);
		check_ready();
		rtk_writel(0, NACMR);
		real_page++;
	}
	
	return 0;

}
