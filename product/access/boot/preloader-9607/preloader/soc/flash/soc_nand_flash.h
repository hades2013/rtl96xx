#ifndef SOC_NAND_FLASH_H
#define SOC_NAND_FLASH_H
#include <pblr.h>
//define nand control register address
//RS code register:
#if 0
#define NAND_CTRL_BASE  0xB801A000
#define NACFR  (NAND_CTRL_BASE + 0x0)
				#define NAFC_RC (1<<31)
#define NACR    (NAND_CTRL_BASE + 0x04)
        #define flash_READY  (1<<31)
        #define ECC_enable    (1<<30)
        #define RBO                  (1<<29)
        #define WBO                  (1<<28)
#define NACMR   (NAND_CTRL_BASE + 0x08)
        #define CECS1   (1<<31)
        #define CECS0   (1<<30)
        #define Chip_Seletc_Base        30
#define NAADR   (NAND_CTRL_BASE + 0x0C)
        #define enNextAD                (1<<27)
        #define AD2EN           (1<<26)
        #define AD1EN           (1<<25)
        #define AD0EN           (1<<24)
        #define CE_ADDR2                16
        #define CE_ADDR1                8
        #define CE_ADDR0                0
#define NADCRR  (NAND_CTRL_BASE + 0x10)
        #define TAG_DIS         (1<<6)
        #define DESC1           (1<<5)
        #define DESC0           (1<<4)
        #define DMARE           (1<<3)
        #define DMAWE           (1<<2)
        #define LBC_128         3
        #define LBC_64          2
        #define LBC_32          1
        #define LBC_16          0
#define NADR    (NAND_CTRL_BASE + 0x14)
#define NADFSAR (NAND_CTRL_BASE + 0x18)
#define NADRSAR (NAND_CTRL_BASE + 0x1C)
#define NASR    (NAND_CTRL_BASE + 0x20)
        #define NECN                    (1<<4)
        #define NRER                    (1<<3)
        #define NWER                    (1<<2)
        #define NDRS                    (1<<1)
        #define NDWS            (1<<0)
#define NADTSAR (NAND_CTRL_BASE + 0x54)


#else

#define NAND_CTRL_BASE  0xB801A000
#define NACFR  (NAND_CTRL_BASE + 0x0)
/*bit 31:30	read command strap pin, 00: 512Byte, 01: 2048Byte,10:2048,11:4096 */
				#define NAFC_RC1 (1<<31)			
				#define NAFC_RC0 (1<<30)
/*bit 29:28 Address Cycle strap pin, 00: 3 address cycle 01: 4 address cycle 10: 5 address cycle 11: reserved */
				#define NAFC_AC1 (1<<29)
				#define NAFC_AC0 (1<<28)
				
#define NACR    (NAND_CTRL_BASE + 0x04)
        #define flash_READY  (1<<31)
        #define ECC_enable    (1<<30)
        #define RBO                  (1<<29)
        #define WBO                  (1<<28)
#define NACMR   (NAND_CTRL_BASE + 0x08)
        #define CECS1   (1<<31)
        #define CECS0   (1<<30)
        #define Chip_Seletc_Base        30
#define NAADR   (NAND_CTRL_BASE + 0x0C)
        #define enNextAD                (1<<27)
        #define AD2EN           (1<<26)
        #define AD1EN           (1<<25)
        #define AD0EN           (1<<24)
        #define CE_ADDR2                16
        #define CE_ADDR1                8
        #define CE_ADDR0                0
#define NADCRR  (NAND_CTRL_BASE + 0x10)
        #define TAG_DIS         (1<<6)
        #define DESC1           (1<<5)
        #define DESC0           (1<<4)
        #define DMARE           (1<<3)
        #define DMAWE           (1<<2)
        #define LBC_128         3
        #define LBC_64          2
        #define LBC_32          1
        #define LBC_16          0
#define NADR    (NAND_CTRL_BASE + 0x14)
#define NADFSAR (NAND_CTRL_BASE + 0x18)	 //DMA Flash Start Address Register 1
#define NADFSAR2 (NAND_CTRL_BASE + 0x1C) //DMA Flash Start Address Register 2 ,zero when nand flash address smaller 2G

#define NADRSAR (NAND_CTRL_BASE + 0x20)
#define NADTSAR (NAND_CTRL_BASE + 0x24)  

#define NASR    (NAND_CTRL_BASE + 0x28)
        #define NECN                    (1<<4)
        #define NRER                    (1<<3)
        #define NWER                    (1<<2)
        #define NDRS                    (1<<1)
        #define NDWS            (1<<0)



#endif // #if 0
#define M_mask                  0xe0000000


#define rtk_writel(val, offset)    (*(volatile unsigned long *)(offset) = val)
#define rtk_readl(offset)         (*(volatile unsigned long *)(offset))



/* NAND Flash Command Sets */
#define CMD_READ_ID				0x90
#define CMD_READ_STATUS		0x70

#define CMD_PG_READ_C1		0x00

#define CMD_PG_READ_512_R2		0x01
#define CMD_PG_READ_512_R3		0x50

#define CMD_PG_READ_C2		0x30

#define CMD_PG_READ_C3		CMD_READ_STATUS

#define CMD_PG_WRITE_C1		0x80
#define CMD_PG_WRITE_C2		0x10
#define CMD_PG_WRITE_C3		CMD_READ_STATUS

#define CMD_BLK_ERASE_C1		0x60	//Auto Block Erase Setup command
#define CMD_BLK_ERASE_C2		0xd0	//CMD_ERASE_START
#define CMD_BLK_ERASE_C3		CMD_READ_STATUS	//CMD_STATUS_READ

int _nand_erase_block(u32_t chunk_id);
int _nand_read_chunk(u8_t *chunk_buf, spare_u *spare, u32_t chunk_id);
int _nand_write_chunk(u8_t *chunk_buf, spare_u *spare, u32_t chunk_id);
u8_t _nand_read_bbi(u32_t bytes, u32_t chunk_id );
int _nand_PIO_write(u32_t chunk_id,u32_t length, unsigned char * buffer);

int nand_check_eccStatus();

void check_ready();


#define para_chunk_size             (parameters.chunk_size)
#define para_num_chunk_per_block    (parameters.num_chunk_per_block)
#define MAX_WRITE_TRY           6   // when writing a chunk, the maximal number to try
inline static int
bbt_is_bad_block(u32_t chunk_id) {
	// assert(parameters.bbt_valid)
	u32_t *bbt=parameters.bbt;
	u32_t b=chunk_id/para_num_chunk_per_block;
	return _get_flags(bbt, b);
}

inline static u8_t*
pbldr_spare_bbi(spare_u *spare) {
	return &(spare->u8_oob[para_flash_info.bbi_swap_offset]);
}


#endif //SOC_NAND_FLASH_H

