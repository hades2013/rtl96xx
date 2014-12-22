#include <preloader.h>
#include <pblr.h>
#include "soc_nand_flash.h"

//static u32_t chunk_size;
//static u32_t num_chunk_per_block;

//#define ADDR_NOTALIGNED(byte, addr) ((addr & (byte-1)) != 0)   //czyao, 2010/0917


/**
**  _nand_read_chunk()
**  descriptions: DAM read one ecc chunk
**  parameters: chunk_buf, data buffer point
                                spare , oob buffer point
                                chunk_id, chunk number (will Calculate to real page number)
**  return:  -1 is fail, 0~N : ecc counter
**  note:  one chunk = 2k data.
**/


int
_nand_read_chunk(u8_t *chunk_buf, spare_u *spare, u32_t chunk_id) {
  int real_page = chunk_id*para_flash_info.page_per_chunk; //get real_page number
    int dma_counter = 4; //always 512byte * 4
    int dram_sa, oob_sa;
    int page_shift;
    int page_num[3];
    unsigned long flash_addr_t=0;
    unsigned long flash_addr2_t=0;
    int buf_pos=0;
    int return_value=0;
    int ecc_count;
	int i;
	u8_t temp_buf;
    int block = real_page/para_flash_info.num_page_per_block;

    // allocate dma buffer and cache align
    u8_t *oob_area;
    u8_t *data_area, data_area0[512+16+CACHELINE_SIZE+CACHELINE_SIZE-4];//512+16 and start/end cache line alignment


    pblr_bset(data_area0, 0xff, 512+16+CACHELINE_SIZE+CACHELINE_SIZE-4);
    data_area = (u8_t*) (u32_t) CACHE_ALIGN(data_area0);
    oob_area=(u8_t*) data_area+512;

    parameters._dcache_writeback_invalidate_all();

    // 1. chunk_buf and oob_buf should be translated to logical addresses
    oob_sa = ((u32_t)oob_area) & (~M_mask);

//  dram_sa = ((u32_t)chunk_buf) & (~M_mask);
    dram_sa = ((u32_t)data_area) & (~M_mask);



    rtk_writel(0xC00FFFFF, NACR);     //Enable ECC
    rtk_writel(0x0000000F, NASR);     //clear NAND flash status register
//  rtk_writel( dram_sa, NADRSAR);//dram start address register
    //set DMA RAM oob start address
//  rtk_writel( oob_sa, NADTSAR); //dma tag register  is oob address

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


    while(dma_counter>0){
    //  dram_sa = ( (u32_t)(chunk_buf+buf_pos*512) & (~M_mask));
      //set DMA RAM DATA start address
      rtk_writel(dram_sa, NADRSAR);
      //set DMA RAM oob start address , always use oob_sa buffer
      rtk_writel(oob_sa, NADTSAR);
      //set DMA flash start address,
      rtk_writel( flash_addr_t, NADFSAR); //write flash address
#if 1 //BCH
        rtk_writel( flash_addr2_t, NADFSAR2);
#endif
//      pbldr_dma_cache_inv_range((u32_t *)data_area,512+16+CACHELINE_SIZE-4);
      parameters._dcache_writeback_invalidate_all();

      //DMA read command
      rtk_writel( ((~TAG_DIS)&(DESC0|DMARE|LBC_128)),NADCRR);
      check_ready();
      //check status register
      ecc_count=nand_check_eccStatus();

    //Record ecc counter ,return the max number
      if((return_value!=-1) && (ecc_count != -1))
      {
          return_value= (ecc_count>return_value)?ecc_count:return_value;
      }else{
          return_value=-1;
      }

        //copy data
        memcpy(chunk_buf+(buf_pos*512), data_area, 512);
	//copy oob
	memcpy(spare->u8_oob+(buf_pos*6), oob_area, 6);  
 
      flash_addr_t += 528;//move next flash addr add (512+6+10)bytes
      dma_counter--;
      buf_pos++;
  }

    if(block>0){
    /*after finish read one chunk data , need swap bbi /data byte.*/
		temp_buf=chunk_buf[para_flash_info.bbi_dma_offset];
        chunk_buf[para_flash_info.bbi_dma_offset] = spare->u8_oob[para_flash_info.bbi_swap_offset];
//	  	spare->u8_oob[para_flash_info.bbi_swap_offset]=0xff;
		spare->u8_oob[para_flash_info.bbi_swap_offset]=temp_buf;

      }

#if 0//RS
     //only for test chip
    if(return_value==-1){ //ecc fail ,test chip must check data is all 0xff
          return_value=0;
          for(i=0; i<(24); i++){
              if( spare->u8_oob[i] != 0xff){
                  return_value= -1;
                }
          }
    }
#endif

  return return_value;
}

/**
**  _nand_read_bbi()
**  descriptions: read flash bbi byte and return it
**  parameters:  byte: bbi offset byte (should be nandflash_info.bbi_offset) , chunk id: chunk number
**  return:  1 is badblock,  0 is ok block.
**  Note: only for page size 2k ,
**/
#if 1
u8_t
_nand_read_bbi(u32_t bytes, u32_t chunk_id ) {
    int real_page;
    unsigned int flash_addr1, flash_addr2;
    unsigned int data_out;
    real_page = chunk_id*para_flash_info.page_per_chunk;  //get real_page number

//  rtk_writel(0xc00fffff, NACR);

    // Calculate flash address for register.

    if(para_flash_info.page_size==512){
        flash_addr1 = (real_page << 8) & 0xffffff;
        flash_addr2 = (real_page >> 16) & 0xffffff;

    }else{
        flash_addr1 =  ((real_page & 0xff) << 16) | bytes;
        flash_addr2 = (real_page >> 8) & 0xffffff;
    }

      /* Command cycle 1*/
    if(para_flash_info.page_size!=512){
        rtk_writel((CECS0|CMD_PG_READ_C1), NACMR);
    }else{
        /*bbi should in spare-area (oob) 512+16 */
        bytes-=512;
        flash_addr1|=bytes;
        /* Command cycle 1*/
        rtk_writel((CECS0|CMD_PG_READ_512_R3), NACMR); // CMD 50h
    }
    check_ready();


#if 0 //for reference , page 512 ,if bbi not in oob.
  if(bytes<256){ /*0~255*/
      flash_addr1|=bytes;
      /* Command cycle 1*/
      rtk_writel((CECS0|CMD_PG_READ_C1), NACMR);

  }else if((255<bytes)&(bytes<512)){
      bytes-=256;
      flash_addr1|=bytes;
      /* Command cycle 1*/
      rtk_writel((CECS0|CMD_PG_READ_512_R2), NACMR);  //CMD 01h
  }else if((bytes>511)&(bytes<518)){
      bytes-=512;
      flash_addr1|=bytes;
      /* Command cycle 1*/
      rtk_writel((CECS0|CMD_PG_READ_512_R3), NACMR); // CMD 50h
  }

#endif


    // use pio read.
    switch(para_flash_info.addr_cycles){
        case 3:
            rtk_writel( (AD2EN|AD1EN|AD0EN|flash_addr1), NAADR);
            break;
        case 4:
            /* set address 1 */
            rtk_writel( (enNextAD|AD2EN|AD1EN|AD0EN|flash_addr1), NAADR);
            check_ready();
            rtk_writel( (AD0EN|flash_addr2), NAADR);
            break;
        case 5:
            rtk_writel( (enNextAD|AD2EN|AD1EN|AD0EN|flash_addr1), NAADR);
            check_ready();
            rtk_writel( (AD1EN|AD0EN|flash_addr2), NAADR);
            break;
    }

    check_ready();

    if(para_flash_info.page_size!=512){ //512 page size not need 'end' command
        /* Command cycle 2*/
        rtk_writel((CECS0|CMD_PG_READ_C2), NACMR);
        check_ready();
    }
    data_out = rtk_readl(NADR); //read 4 byte from NADR.

    if(((data_out >> 24) & 0xff) !=0xff){
        return 1;       //bad
    }

    return 0; //good

}
#endif
#if 0//old
u8_t
_nand_read_bbi(u32_t bytes, u32_t chunk_id ) {
    int real_page;
    unsigned int flash_addr1, flash_addr2;
    unsigned int data_out;
    real_page = chunk_id*para_flash_info.page_per_chunk;  //get real_page number

    rtk_writel(0xc00fffff, NACR);

    /* Command cycle 1*/
    rtk_writel((CECS0|CMD_PG_READ_C1), NACMR);
    check_ready();

    // Calculate flash address for register.

    flash_addr1 =  ((real_page & 0xff) << 16) | bytes;
    flash_addr2 = (real_page >> 8) & 0xffffff;
  // use pio read.
    //write address to flash control
    rtk_writel( (enNextAD|AD2EN|AD1EN|AD0EN|flash_addr1), NAADR);
    if(para_flash_info.addr_cycles==5){
        rtk_writel( (AD1EN|AD0EN|flash_addr2), NAADR);
    }else if(para_flash_info.addr_cycles==4){
        rtk_writel( (AD0EN|flash_addr2), NAADR);
    }

    /* Command cycle 2*/
    rtk_writel((CECS0|CMD_PG_READ_C2), NACMR);
    check_ready();

    data_out = rtk_readl(NADR); //read 4 byte from NADR.

    if(((data_out >> 24) & 0xff) !=0xff){
        return 1;       //bad
    }

    return 0; //good

}
#endif


/**
**  _nand_read_512_bbi()
**  descriptions: read flash bbi byte and return it
**  parameters:  byte: bbi offset byte (should be nandflash_info.bbi_offset) , chunk id: chunk number
**  return:  1 is badblock,  0 is ok block.
**  Note: only for page size 512 ,
**/
#if 0
u8_t
_nand_read_512_bbi(u32_t bytes, u32_t chunk_id ) {
    unsigned int real_page;
    unsigned int flash_addr1, flash_addr2;
    unsigned int data_out;
    real_page = chunk_id*para_flash_info.page_per_chunk;  //get real_page number

    rtk_writel(0xc00fffff, NACR);

    /* Command cycle 1*/
    rtk_writel((CECS0|CMD_PG_READ_C1), NACMR);
    check_ready();

    // Calculate flash address for register.

    flash_addr1 = (real_page << 8) & 0xffffff;
    flash_addr2 = (real_page >> 16) & 0xffffff;

    if(bytes<256){ /*0~255*/
        flash_addr1|=bytes;
        /* Command cycle 1*/
        rtk_writel((CECS0|CMD_PG_READ_C1), NACMR);
    }else if((255<bytes)&(bytes<512)){
        bytes-=256;
        flash_addr1|=bytes;
        /* Command cycle 1*/
        rtk_writel((CECS0|CMD_PG_READ_512_R2), NACMR);  //CMD 01h
    }else if((bytes>511)&(bytes<518)){
        bytes-=512;
        flash_addr1|=bytes;
        /* Command cycle 1*/
        rtk_writel((CECS0|CMD_PG_READ_512_R3), NACMR); // CMD 50h
    }
    check_ready();

  // use pio read.

    //write address to flash control
//  rtk_writel( (enNextAD|AD2EN|AD1EN|AD0EN|flash_addr1), NAADR);
//  if(flash_info.addr_cycles==5){
//      rtk_writel( (AD1EN|AD0EN|flash_addr2), NAADR);
//  }else if(flash_info.addr_cycles==4){
//      rtk_writel( (AD0EN|flash_addr2), NAADR);
//  }



    switch(para_flash_info.addr_cycles){
        case 3:
            rtk_writel( (AD2EN|AD1EN|AD0EN|flash_addr1), NAADR);
            break;
        case 4:
            /* set address 1 */
            rtk_writel( (enNextAD|AD2EN|AD1EN|AD0EN|flash_addr1), NAADR);
            check_ready();
            rtk_writel( (AD0EN|flash_addr2), NAADR);
            break;
        case 5:
            rtk_writel( (enNextAD|AD2EN|AD1EN|AD0EN|flash_addr1), NAADR);
            check_ready();
            rtk_writel( (AD1EN|AD0EN|flash_addr2), NAADR);
            break;
    }
    check_ready();

    //512 page size not need 'end' command

    data_out = rtk_readl(NADR); //read 4 byte from NADR.

    if(((data_out >> 24) & 0xff) !=0xff){
        return 1;       //bad
    }

    return 0; //good

}
#endif

/**
**  _nand_init()
**  descriptions: nand control init.
**  parameters:  none
**  return: 0:succes , -1:fail
**/
int
_nand_init(){
    //int id_chain=0;
    //int i;
    soc_t *rw_soc=parameter_soc_rwp;

    /* initialize hardware */

    rtk_writel(0x0000000F, NASR);     //clear NAND flash status register
    rtk_writel( (rtk_readl(NACR) |ECC_enable|RBO|WBO), NACR); //Enable ECC
#if 0
    //read ID command , chip select always use chip 0
    rtk_writel( ((1<<(Chip_Seletc_Base+0))|CMD_READ_ID) , NACMR);
    check_ready();

    rtk_writel( 0x0, NAADR);
    rtk_writel( (0x0 |AD2EN|AD1EN|AD0EN) , NAADR);  //dummy address cycle
    check_ready();

    id_chain = rtk_readl(NADR);

    if(id_chain==0){
        //read flash fail.
        return -1;
    }

    //clear command/address register
    rtk_writel( 0x0, NACMR);
    rtk_writel( 0x0, NAADR);
#endif
    //read from strap pin
    u32_t nand_strap_pin= rtk_readl(NACFR);

    //READ page size strap ping
    int lg_pgsz=((nand_strap_pin & 0xC0000000) >>30);
    int base=lg_pgsz?1024:512;
    rw_soc->flash_info.page_size=base<<lg_pgsz;

    #if 0
    switch( (nand_strap_pin & 0xC0000000) >>30){
        case 0:
            //  *(u16_t*)&(para_flash_info.page_size) = 512;
            rw_soc->flash_info.page_size=512;
            break;
        case 1:
            //*(u16_t*)&(para_flash_info.page_size)= 2048;
            rw_soc->flash_info.page_size=2048;
            break;
        case 2:
            //*(u16_t*)&(para_flash_info.page_size) = 4096;
            rw_soc->flash_info.page_size=4096;
            break;
        case 3:
            //*(u16_t*)&(para_flash_info.page_size) = 8192;
            rw_soc->flash_info.page_size=8192;
            break;
    }
    #endif
    //read address cycle pin
    //*(u8_t*)&(para_flash_info.addr_cycles)= ((nand_strap_pin & 0x30000000) >>28)+3;  //3,4,5 address cycle
    rw_soc->flash_info.addr_cycles= ((nand_strap_pin & 0x30000000) >>28)+3;  //3,4,5 address cycle


    /*this hard-copy , for MXIC MX30LF1208AA and toshiba TC58NVM9S3ETA00,
     64Mbyte flash, page size 2k. 64pages * 512blocks , address 4 cycle */
    //Assign nandflash_info ,number of block
#if 0
    *(u16_t*)&(para_flash_info.num_block)  = 512;
    //page number of one block
    *(u16_t*)&(para_flash_info.num_page_per_block) = 64;
    *(u8_t*)&(para_flash_info.page_per_chunk)=1;
    *(u16_t*)&(para_flash_info.bbi_dma_offset)=2000;
    //for 2048+64 pagesize, if spare_area byte[0] is bbi, in dma data_buffer is byte 2000, because must ignore 16byte oob *3,
    // oob buffer not inside in data buffer.
    *(u16_t*)&(para_flash_info.bbi_raw_offset)=2048; //bbi in nand flash raw address, for pio read

    *(u16_t*)&(para_flash_info.bbi_swap_offset)=23; // 512+16 nand control read, use 4th 512's oob byte[5] for data swap
                                   // after dma move to oob buffer, data byte is in oob_buf[23]  (6+6+6+6) (no ecc byte)
   *(u8_t*)&(para_flash_info.pblr_start_block)=1; //8 bit

    *(u16_t*)&(para_flash_info.num_pblr_block)=3; //16bit
#endif
 //end hard-copy

    // create nand dynamic information and load pre-loader
    para_chunk_size=para_flash_info.page_size*para_flash_info.page_per_chunk;
    para_num_chunk_per_block=((para_flash_info.num_page_per_block)/(para_flash_info.page_per_chunk));

    return 0;
}

/**
**  check_ready()
**  descriptions: check nand flash busy/ready.
**  parameters:  none
**  return: none
**  note: exit loop when nand flash ready,else busy-waiting
**/
void check_ready()
{
    #if 0
    while(1) {
        if(  ( rtk_readl(NACR) & 0x80000000) == 0x80000000 )
            break;
    }
    #endif
    while ((rtk_readl(NACR) & 0x80000000)==0);
}


/**
**  nand_check_eccStatus()
**  descriptions: check ecc counter,status.
**  parameters:  none
**  return: 0: ecc ok, 1~N ,ecc can fix it, return ecc counter,  -1: ecc can't fix it.
**  note:
**/
int nand_check_eccStatus()
{
    int error_count,status;
    int rc=0;

    status = rtk_readl(NASR);

    if( (status & NDRS)== NDRS){     //read status
        if( status & NRER) { //ecc result = 1 , ecc read fail.
            error_count = (status & 0xf0) >> 4;
            /*ecc correction counter must change */
            if(error_count <=4 && error_count > 0 ) { //some bit fail, but ecc can fix it.
                //pblr_printf("[%s] R: Correctable HW ECC Error at page=%u, status=0x%08X\n\r", __FUNCTION__, page,status);
                status &= 0x0f; //clear NECN
                rtk_writel(status, NASR);
                rc=error_count;
            }else{
                //counter = 0 and ecc fail,  it mean ecc can' fix it
                rc = -1;
            }
        }
    }else if( (status & NDWS)== NDWS){ //write status
            if( status & NWER) {
//              pblr_printf("Un-Correctable HW ECC Error in Write \n");
                rc=-1;
            }
    }
    status &= 0x0f; //clear status.
    rtk_writel(status, NASR);
    return rc;

}


//implement from lplr.c

/**
**  create_bbt()
**  descriptions: Create bbt table
**  parameters: nand_dma_buf_t
**  return:
**  note:
**/

static void
create_bbt(nand_dma_buf_t *dma_buf) {
    u32_t b;
    int ecc;
    u8_t bbi;
    u32_t num_chunk_per_block=para_num_chunk_per_block;

    // when FTx, all blocks are assumed well
    if (!FIHT_IS_NORMAL(parameters.soc.header_type)) return;

    pbldr_wzero(parameters.bbt, NUM_WORD(MAX_BLOCKS));
    for (b=1;b<para_flash_info.num_block;++b) {

        bbi=_nand_read_bbi(para_flash_info.bbi_raw_offset,b*num_chunk_per_block);
        if (bbi==0) continue;

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

/**
**  load_plr()
**  descriptions: move preloader from nand to DRAM (SYSTEM_STARTUP_CADDR)
**  parameters: nand_dma_buf_t  (temp usage)
**  return:
**  note:
**/

static int
load_plr(nand_dma_buf_t *dma_buf) {
    #define NUM_WORD_XLR_CHUNK (NUM_WORD(MAX_PLR_CHUNK)+NUM_WORD(MAX_BLR_CHUNK))
    u32_t _succ_flags[NUM_WORD_XLR_CHUNK];
    #define plr_succ_flags _succ_flags
    #define blr_succ_flags (_succ_flags+NUM_WORD(MAX_PLR_CHUNK))

    // uncached address
    u8_t *chunk_buf=(u8_t*)UADDR(dma_buf->chunk_buf);
    spare_u *spare=(spare_u *)UADDR(&(dma_buf->spare));
    int b,n;
    int c, ecc;
    u32_t ver_cap=0xffff;
    u32_t num_chunk_per_block=para_num_chunk_per_block;
    parameters.plr_num_chunk=0;
    parameters.blr_num_chunk=0;
    pbldr_wzero(_succ_flags, NUM_WORD_XLR_CHUNK);

    do {
        n=para_flash_info.pblr_start_block+para_flash_info.num_pblr_block;
        parameters.curr_ver=0;
        for (b=para_flash_info.pblr_start_block;b<n;b++) {

            if (bbt_is_bad_block(b*num_chunk_per_block)){
		n++;
                continue;
		}

            for (c=0;c<(int)num_chunk_per_block; ++c) {
                if ((ecc=_nand_read_chunk(chunk_buf, spare, b*num_chunk_per_block+c))<0) continue;
                if ((spare->oob.signature!=OOB_SIG_USED_PLR)&&
                    (spare->oob.signature!=OOB_SIG_USED_BLR)) continue;

                u32_t ver=spare->oob.ver;
                if ((ver<parameters.curr_ver)||(ver>=ver_cap)) continue;
                if (ver>parameters.curr_ver) {
                    parameters.curr_ver=ver;
                    parameters.plr_num_chunk=0;
                    parameters.blr_num_chunk=0;
                    #if 0
                    pbldr_wzero(plr_succ_flags, NUM_WORD(MAX_PLR_CHUNK));
                    pbldr_wzero(blr_succ_flags, NUM_WORD(MAX_BLR_CHUNK));
                    #else
                        pbldr_wzero(_succ_flags, NUM_WORD_XLR_CHUNK);
                    #endif
                }
                u32_t cid=spare->oob.chunk_id;
                if (spare->oob.signature==OOB_SIG_USED_PLR) {
                    if (cid==0) parameters.plr_num_chunk=spare->oob.num_chunk;
                    if (_get_flags(plr_succ_flags, cid)==0) {
                        // copy chunk_buf to the right location
                        u8_t *dst=plr_addr_by_chunck_id(cid, para_chunk_size);
                        //pblr_wmemcpy(dst, chunk_buf, chunk_size/sizeof(u32_t));
                        memcpy(dst, chunk_buf, para_chunk_size);
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
        if ((check_if_all_one(plr_succ_flags, parameters.plr_num_chunk-1)==0)&&
            (check_if_all_one(blr_succ_flags, parameters.blr_num_chunk-1)==0))
            return 0;

        // need more scans
    } while ((ver_cap=parameters.curr_ver)>0);
    return -1;
}


static int
nand_operations(void) {
    // allocate dma buffer and cache align
    u8_t *_dma_buf=alloca(_sizeof_nand_dma_buf(para_chunk_size)+CACHELINE_SIZE-4);
    nand_dma_buf_t *dma_buf=(nand_dma_buf_t *)CACHE_ALIGN(_dma_buf);

    // create bad block table
    create_bbt(dma_buf);

    // load pre-loader
     return load_plr(dma_buf);
}


/* When running bad_block_management(), console is not
   available yet, so use NOT printf()! */
void nand_loader_init(void) {

    // set up function points
    parameters._nand_read_bbi=_nand_read_bbi;
    parameters._nand_read_chunk=_nand_read_chunk;
    parameters._nand_check_eccStatus=nand_check_eccStatus;
    //parameters._nand_write_chunk=_nand_write_chunk;
    //parameters._nand_erase_block=_nand_erase_block;

    _nand_init(); //assign flish_info

    if (nand_operations()!=0) while(1);
    return;
}
