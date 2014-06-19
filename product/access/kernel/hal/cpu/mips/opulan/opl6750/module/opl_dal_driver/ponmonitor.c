/*****************************************************************************

      File name:ponmonitor.c
      Description:provide funcs of pon monitor
      Author:liaohongjun
      Date:
              2012/8/27
==========================================================
      Note:
*****************************************************************************/
#ifdef  __cplusplus
        extern "C"{
#endif

#include <linux/kthread.h>
#include <linux/errno.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/fs.h>   //add by dengjian 2012-11-28 for EPN204QID0015
#include <asm/uaccess.h> //add by dengjian 2012-11-28 for EPN204QID0015
#include "dal_lib.h"

#ifdef CONFIG_BOSA
static struct task_struct *kponmonitor_task;
#if 0
unsigned char main_info[MAX_I2CPONINFO_LEN] = {0};
unsigned char c_main_info[512] = {0};
unsigned char c_auxi_info[512] = {0};
unsigned char c_defi_info[512] = {0};
#endif
/*Begin add by dengjian 2012-11-28 for EPN204QID0015*/
#define UFILE_MODSETVAL "/proc/manufactory/modsetval"
#define UFILE_APCSETVAL "/proc/manufactory/apcsetval"
#define BUFSIZE5      5
//I2C_INFO_S stI2cInfo;
unsigned char modSetVal;
unsigned char apcSetVal;

static BOOL_T g_bI2cPonInitDone = FALSE;

void get_imodvalue(void)
{
    struct file *filp;    
    mm_segment_t fs;    
    off_t fsize;    
    char *buf;    
    unsigned int uiModSetVal;
    
    filp=filp_open(UFILE_MODSETVAL,O_RDONLY,0);  
    if(IS_ERR(filp))
    {
        return;
    }  
    fsize=BUFSIZE5;    
    buf=(char *) kmalloc(fsize+1,GFP_ATOMIC);  
    
    fs=get_fs();    
    set_fs(KERNEL_DS);    
    filp->f_op->read(filp,buf,fsize,&(filp->f_pos));    
    set_fs(fs);     
    
    buf[fsize]='\0';    
    filp_close(filp,NULL); 

    sscanf(buf,"%x",&uiModSetVal);

    modSetVal = (UINT8)uiModSetVal;

    kfree(buf);
    return ;
	
}

void get_apcvalue(void)
{
    struct file *filp;    
    mm_segment_t fs;    
    off_t fsize;    
    char *buf;    
    unsigned int uiapcSetVal;
    
    filp=filp_open(UFILE_APCSETVAL,O_RDONLY,0);  
    if(IS_ERR(filp))
    {
        return;
    }  
    fsize=BUFSIZE5;    
    buf=(char *) kmalloc(fsize+1,GFP_ATOMIC);  
    
    fs=get_fs();    
    set_fs(KERNEL_DS);    
    filp->f_op->read(filp,buf,fsize,&(filp->f_pos));    
    set_fs(fs);     
    
    buf[fsize]='\0';    
    filp_close(filp,NULL); 

    sscanf(buf,"%x",&uiapcSetVal);

    apcSetVal = (UINT8)uiapcSetVal;

    kfree(buf);

    return ;
	
}
/*End add by dengjian 2012-11-28 for EPN204QID0015*/

void Init25L90(void)
{   
	UINT8 readByte = 0;
	UINT8 counter  = 0;
    int iRet = OPL_ERROR;

REPEAT_POS:

    g_bI2cPonInitDone = TRUE;
    /*Being add by dengjian 2012-11-28 for EPN204QID0015*/
    get_imodvalue();
    get_apcvalue();

    I2C_DEBUG("modSetVal=%x,apcSetVal=%x\n",modSetVal,apcSetVal);
	/*End add by dengjian 2012-11-28 for EPN204QID0015*/
    
	do 
	{
		iRet = i2cReadRandom(I2C_SLAVE_ADDR, STATUS0, 1, &readByte);
        if(iRet < OPL_OK)
        {
            I2C_DEBUG("[%s,%d]:i2c read reg val failed!\n",__FUNCTION__,__LINE__);
            g_bI2cPonInitDone = FALSE;
        }
		counter++;
        //I2C_DEBUG("iRet = %d  readByte = 0x%02x counter = %d\n", iRet, readByte, counter);
	}while (((readByte & 0x10) != 0x00) && (counter < 4));
    
    iRet = i2cWriteRegByteData(I2C_SLAVE_ADDR, TCSTART, 0x00);
	if (iRet < OPL_OK)
	{
		g_bI2cPonInitDone = FALSE;
	}
    
    iRet = i2cWriteRegByteData(I2C_SLAVE_ADDR, TSLOPE, 0x00);
	if (iRet < OPL_OK)
	{
		g_bI2cPonInitDone = FALSE;
	}    
    
    iRet = i2cWriteRegByteData(I2C_SLAVE_ADDR, BIASINIT, 0x64);
	if (iRet < OPL_OK)
	{
		g_bI2cPonInitDone = FALSE;
	}   
    
    iRet = i2cWriteRegByteData(I2C_SLAVE_ADDR, APCCTRL0, 0x15);
	if (iRet < OPL_OK)
	{
		g_bI2cPonInitDone = FALSE;
	}   
    
    iRet = i2cWriteRegByteData(I2C_SLAVE_ADDR, MDMAX, 0xFC);
	if (iRet < OPL_OK)
	{
		g_bI2cPonInitDone = FALSE;
	}  
    
    iRet = i2cWriteRegByteData(I2C_SLAVE_ADDR, BIASMAX, 0xFF);
    if (iRet < OPL_OK)
    {
        g_bI2cPonInitDone = FALSE;
    }
    
    /*to do : set LOSSET correctly*/
    iRet = i2cWriteRegByteData(I2C_SLAVE_ADDR, LOSSET, 0x3C);
	if (iRet < OPL_OK)
	{
		g_bI2cPonInitDone = FALSE;
	}    
    
    //iRet = i2cWriteRegByteData(I2C_SLAVE_ADDR, RXSET0, 0xF0);
    iRet = i2cWriteRegByteData(I2C_SLAVE_ADDR, RXSET0, 0xF8);
	if (iRet < OPL_OK)
	{
		g_bI2cPonInitDone = FALSE;
	}     
    
    iRet = i2cWriteRegByteData(I2C_SLAVE_ADDR, RXSET1, 0x06);
	if (iRet < OPL_OK)
	{
        g_bI2cPonInitDone = FALSE;
	}  
    
    /*to do : set APCSET correctly*/
    iRet = i2cWriteRegByteData(I2C_SLAVE_ADDR, APCSET, apcSetVal);
	if (iRet < OPL_OK)
	{
        g_bI2cPonInitDone = FALSE;
	}  

    /*to do : set IMODSET correctly*/
    iRet = i2cWriteRegByteData(I2C_SLAVE_ADDR, IMODSET, modSetVal);
	if (iRet < OPL_OK)
	{
        g_bI2cPonInitDone = FALSE;
	}  
	#if (BEN_ON)
    iRet = i2cWriteRegByteData(I2C_SLAVE_ADDR, TXSET0, 0x76);
	if (iRet < OPL_OK)
	{
        g_bI2cPonInitDone = FALSE;
	}      
    #else    
    iRet = i2cWriteRegByteData(I2C_SLAVE_ADDR, TXSET0, 0x70);
	if (iRet < OPL_OK)
	{
        g_bI2cPonInitDone = FALSE;
	}  
    #endif
    
    iRet = i2cWriteRegByteData(I2C_SLAVE_ADDR, TXSET1, 0x00);
	if (iRet < OPL_OK)
	{
        g_bI2cPonInitDone = FALSE;
	}    
   
	if ((counter < 4) || (FALSE == g_bI2cPonInitDone))
	{
		goto REPEAT_POS;
	} 

    return;
}
#if 0
/***------------------------------------------------------------------
* Call mode:  void MonitorTemp(void)
* Function :  Monitor module Tx Bias.
--------------------------------------------------------------------*/
static void MonitorTemp(void)
{
	unsigned int   uival = 0;
	unsigned char  temp_adc[2] = {0};

	if (i2cReadRandom(I2C_SLAVE_ADDR, TEMP_MSB, 2, &temp_adc) < OPL_OK) //Bias ADC
	{
        I2C_DEBUG("[%s,%d]:i2c read reg value failed!\n",__FUNCTION__,__LINE__);
		return;
	}
    //I2C_DEBUG("[%s,%d]:temp_adc[0]=0x%02x,temp_adc[1]=0x%02x\n",__FUNCTION__,__LINE__,temp_adc[0],temp_adc[1]);
    uival = (unsigned int)temp_adc[0];
    uival <<= 8;
    uival +=(unsigned int)temp_adc[1];
    stI2cInfo.iTemp = (int)(((uival*192)>>16) - 54);
    //I2C_DEBUG("[%s,%d]:stI2cInfo.iTemp = %d\n",__FUNCTION__,__LINE__,stI2cInfo.iTemp);
}

/***------------------------------------------------------------------
* Call mode:  void MonitorBias(void)
* Function :  Monitor module Tx Bias.
--------------------------------------------------------------------*/
static void MonitorBias(void)
{
	float          fval = 0;
	unsigned int   uival = 0;
	unsigned char  bias_adc[2] = {0};

	if (i2cReadRandom(I2C_SLAVE_ADDR, TX_BIAS_MSB, 2, &bias_adc) < OPL_OK) //Bias ADC
	{
        I2C_DEBUG("[%s,%d]:i2c read reg value failed!\n",__FUNCTION__,__LINE__);
		return;
	}
    I2C_DEBUG("[%s,%d]:bias_adc[0]=0x%02x,bias_adc[1]=0x%02x\n",__FUNCTION__,__LINE__,bias_adc[0],bias_adc[1]);    
}

/***------------------------------------------------------------------
* Call mode:  void MonitorTxPower(void)
* Function :  Monitor module Tx power.
--------------------------------------------------------------------*/
static void MonitorTxPower(void)
{
	static unsigned int  tx_adc_old = 0;
	static unsigned char i = 0;
	signed int  ival = 0;
	float         fval = 0;
	unsigned int  uival = 0;
    unsigned int  k = 0;
	unsigned char txpwr_adc[2] = {0};

	if (i2cReadRandom(I2C_SLAVE_ADDR, TX_POWER_MSB, 2, &txpwr_adc) < OPL_OK) // TX_POWER ADC
	{
        I2C_DEBUG("[%s,%d]:i2c read reg value failed!\n",__FUNCTION__,__LINE__);
		return;
	}
    I2C_DEBUG("[%s,%d]:txpwr_adc[0]=0x%02x,txpwr_adc[1]=0x%02x\n",__FUNCTION__,__LINE__,txpwr_adc[0],txpwr_adc[1]);
}


/***------------------------------------------------------------------
* Call mode:  void MonitorRxPower(void)
* Function :  Monitor module rx power.
--------------------------------------------------------------------*/
static void MonitorRxPower(void)
{
	static unsigned int  rx_adc_old = 0;
	static unsigned char i          = 0;

	union f2uchar a;
	float C[5]; 
	float fval;
    signed   int  ival;
	unsigned int  uival;
	unsigned int  a_w_hi,a_w_lo;
	unsigned char m, n;
	unsigned char rxpwr_adc[2];

	if (i2cReadRandom(I2C_SLAVE_ADDR, RX_POWER_MSB, 2, &rxpwr_adc) < OPL_OK) // Rx_power ADC
	{
        I2C_DEBUG("[%s,%d]:i2c read reg value failed!\n",__FUNCTION__,__LINE__);
		return;
	}
    I2C_DEBUG("[%s,%d]:rxpwr_adc[0]=0x%02x,rxpwr_adc[1]=0x%02x\n",__FUNCTION__,__LINE__,rxpwr_adc[0],rxpwr_adc[1]);
}
/***------------------------------------------------------------------
* Call mode:  void StatusControl(void)
* Function :  Control Status.
--------------------------------------------------------------------*/

static void StatusControl(void)
{
    unsigned char status_monitor = 0;
    
	if (i2cReadRandom(I2C_SLAVE_ADDR, STATUS0, 1, &status_monitor) < OPL_OK) // Rx_power ADC
	{
        I2C_DEBUG("[%s,%d]:i2c read reg value failed!\n",__FUNCTION__,__LINE__);
		return;
	}
    I2C_DEBUG("[%s,%d]:status_monitor=0x%02x\n",__FUNCTION__,__LINE__,status_monitor);
    
	if (0x00 == (status_monitor & 0x20))//los告警,没有检测到los信号
	{
		I2C_DEBUG("[%s,%d]:RX_SD == 0\n",__FUNCTION__,__LINE__);	
	}  
    #if 0
	if (i2cReadRandom(I2C_SLAVE_ADDR, 0xD0, 1, &status_monitor) < OPL_OK)
	{
        I2C_DEBUG("[%s,%d]:i2c read reg value failed!\n",__FUNCTION__,__LINE__);
		return;
	}
    I2C_DEBUG("[%s,%d]:D0h=0x%02x\n",__FUNCTION__,__LINE__,status_monitor);
	if (i2cReadRandom(I2C_SLAVE_ADDR, 0xD1, 1, &status_monitor) < OPL_OK)
	{
        I2C_DEBUG("[%s,%d]:i2c read reg value failed!\n",__FUNCTION__,__LINE__);
		return;
	}
    I2C_DEBUG("[%s,%d]:D1h=0x%02x\n",__FUNCTION__,__LINE__,status_monitor);  
    #endif
}

static int ponminitor_thread_exit(void)
{
    kthread_stop(kponmonitor_task);
    return 0;
}
void i2cParasSet(void)
{
    int iRet = OPL_ERROR;
    
    modSetVal = 0xBE;
    
    iRet = i2cWriteRegByteData(I2C_SLAVE_ADDR, IMODSET, modSetVal);
	if (iRet < OPL_OK)
	{
        I2C_DEBUG("[%s,%d]:i2c write reg data error!\n",__FUNCTION__,__LINE__);
	}
}

static int ponminitor_thread(void)
{
	UINT8 readByte = 0;
    int iRet = OPL_ERROR;    
    while(1)
    {       
        msleep(90); 
        //MonitorTemp();
        //MonitorBias();
        //MonitorTxPower();
        //MonitorRxPower();
        //StatusControl();
        i2cParasSet();
    }
    return 0;
}
#endif
/*****************************************************************
    Function:dal_I2cPonMonitorInit
    Description:通过i2c 初始化光模块
    Author:liaohongjun
    Date:2012/8/23
    Input: 
    Output:         
    Return:
        DRV_OK
=================================================
    Note:
*****************************************************************/
int dal_I2cPonMonitorInit(void)
{
    
    I2C_DEBUG("call dal_I2cPonMonitorInit...\n");

    Init25L90();
#if 0
	kponmonitor_task = kthread_run(ponminitor_thread, NULL, "kponmonitor");
	if (!IS_ERR(kponmonitor_task))
	{
		return OPL_ERROR;  
	}
#endif
    return OPL_OK;
}
OPL_STATUS dal_I2c25l90ImodSet(unsigned int iModVal)
{
    int iRet = OPL_ERROR;
    
    if((iModVal < 0 )||(iModVal > 255))
    {
        I2C_DEBUG("[%s,%d]:invalid register val of IMODSET\n",__FUNCTION__,__LINE__);
        return OPL_ERROR;
    }
    modSetVal = (unsigned char)iModVal;
    iRet = i2cWriteRegByteData(I2C_SLAVE_ADDR, IMODSET, modSetVal);
	if (iRet < OPL_OK)
	{
        I2C_DEBUG("[%s,%d]:i2c write reg data error!\n",__FUNCTION__,__LINE__);
	} 
    
    I2C_DEBUG("[%s,%d]DBG_INFO:set 25L90 register IMODSET to 0x%2.2x = %d\n",__FUNCTION__,__LINE__,modSetVal,(unsigned int)iModVal);    
    return iRet;
}

OPL_STATUS dal_I2c25l90ApcSet(unsigned int uiApcSet)
{
    int iRet = OPL_ERROR;
    unsigned char ApcSetVal = 0;
        
    if((uiApcSet < 0 )||(uiApcSet > 255))
    {
        I2C_DEBUG("[%s,%d]:invalid register val of APCSET\n",__FUNCTION__,__LINE__);
        return OPL_ERROR;
    }
    
    ApcSetVal = (unsigned char)uiApcSet;
    apcSetVal = ApcSetVal;
    iRet = i2cWriteRegByteData(I2C_SLAVE_ADDR, APCSET, ApcSetVal);
	if (iRet < OPL_OK)
	{
        I2C_DEBUG("[%s,%d]:i2c write reg data error!\n",__FUNCTION__,__LINE__);
	}   
    
    I2C_DEBUG("[%s,%d]DBG_INFO:set 25L90 register APCSET to 0x%2.2x = %d\n",__FUNCTION__,__LINE__,ApcSetVal,(unsigned int)ApcSetVal);    
    return iRet;
}

#endif /*end CONFIG_BOSA*/

#ifdef  __cplusplus
}
#endif


