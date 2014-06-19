#ifndef _PONMONITOR_H_
#define _PONMONITOR_H_

#ifdef  __cplusplus
extern "C"{
#endif
#include "dal_lib.h"

#if	!defined(FALSE) || (FALSE!=0)
#define FALSE		0
#endif

#if	!defined(TRUE) || (TRUE!=1)
#define TRUE		1
#endif

#define I2C_DEBUG_ON 1
#if (I2C_DEBUG_ON)
#define I2C_DEBUG(args...) printk(args)
#else
#define I2C_DEBUG(arg...)
#endif

#define MAX_I2CPONINFO_LEN
#define BEN_ON 1

#define I2C_SLAVE_ADDR 0x54
#define STATUS0        0xB4
#define TXSET0         0x60
#define TXSET1         0x61
#define IMODSET        0x62
#define TCSTART        0x63
#define TSLOPE         0x64
#define APCSET         0x66
#define BIASINIT       0x67
#define APCCTRL0       0x68
#define MDMAX          0x69
#define BIASMAX        0x6A
#define RXSET0         0x6B
#define RXSET1         0x6C
#define LOSSET         0x6D

#define TEMP_MSB       0xA0
#define TX_POWER_MSB   0xA2
#define TX_BIAS_MSB    0xA4
#define TX_MOD_MSB     0xA6
#define RX_POWER_MSB   0xA8
#define VDD_TX_MSB     0xAA
#define VDD_RX_MSB     0xAC
#define VDD_DIG_MSB    0xAE

#define CONTROL0    0xB0

union f2uchar
{
	unsigned char uc[4];
	float f;
};
typedef struct tag_i2c_info
{
    int iTemp;    //temperature
}I2C_INFO_S;

int dal_I2cPonMonitorInit(void);
//extern DRV_RET_E Hal_I2cSetImod(unsigned int iModVal);
OPL_STATUS dal_I2c25l90ImodSet(unsigned int iModVal);
OPL_STATUS dal_I2c25l90ApcSet(unsigned int uiApcSet);


#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */

#endif























