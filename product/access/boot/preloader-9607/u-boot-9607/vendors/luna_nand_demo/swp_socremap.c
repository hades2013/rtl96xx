#include "common.h"
#include <asm/arch/bspchip.h>
#include <asm/arch/sramctl.h>
#include "soc_remap.h"



/*soc 6266  address map table*/
const struct soc_reg_remap_t soc_reg_remap_6266 = {
	.SOC_ID=0x6266,
	.SOC_SUB_TYPE=0,
	.SOC_IO_MODE_EN=0xBB023018,
	.SOC_I2C_IND_CMD=0xBB000030,
	.SOC_I2C_IND_RD=0xBB000038,
	.SOC_GPHY_IND_WD=0xBB00000C,
	.SOC_GPHY_IND_CMD=0xBB000010,
	.SOC_GPHY_IND_RD=0xBB000014,
	.SOC_GPIO_CTRL_0=0xBB0000D8,
	.SOC_GPIO_CTRL_1=0xBB0000E4,
	.SOC_GPIO_CTRL_2=0xBB0000F0,
	.SOC_GPIO_CTRL_4=0xBB0000FC,
	.SOC_IO_LED_EN=0xBB023014,
	.SOC_LED_EN=0xBB01E0A0,
	.SOC_LED_BLINK_RATE_CFG=0xBB01E094,
	.SOC_LOW_RATE_BLINK_CFG=0xBB01E098,
	.SOC_SYS_PKT_BUF_CTRL=0xBB000178,
	/*add remap register at here*/
};

struct soc_reg_remap_t soc_reg_remap;

int rtl8686_soc_remap(void)
{

	//printf("software patch rtl8686_soc_remap\n");
	soc_reg_remap = soc_reg_remap_6266; 
	WRITE_MEM32(BOND_CHIP_MODE,0xb0000000); //enable bound id display.
	soc_reg_remap.SOC_SUB_TYPE=(REG32(BOND_CHIP_MODE)& 0xff);
	//printf("soc_reg_remap.SOC_SUB_TYPE is %x\n",soc_reg_remap.SOC_SUB_TYPE);
}

PATCH_REG(rtl8686_soc_remap, 2);

