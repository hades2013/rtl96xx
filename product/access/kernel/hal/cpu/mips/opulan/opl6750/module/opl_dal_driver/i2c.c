/*
=============================================================================
File Name:i2c.c

  General Description:
  
    This file define all opcomm chip regisger and table address.
    ===============================================================================
    Opulan Confidential Proprietary                     
    ID and version: xxxxxxxxxxxxxx  Version 1.00
    (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
    
      
        Revision History:
        Modification
        Author          Date                Description of Changes
        --------------- --------------- ----------------------------------------------
        zzhu            2007/11/1       Initial Version
        zgan            2009/06/05      Implement codes
        --------------- --------------- ----------------------------------------------
*/

#include "i2c.h"

/* imports */

/* Global */
I2C_SYS_CONFIG_t g_stI2CCfg;

/* for debug system */
int nI2cDbgFd = OPL_ZERO; /* standard output device */
static int bNoStopIssue = OPL_FALSE;

typedef struct tag_i2cRegInfo
{
    UINT8 regAddr;
    UINT8 data;
        
}I2C_REG_INFO_S;

int ifI2cSTOStatusIssue ( void ) {return bNoStopIssue;};

/* defines */ 
#if 0
#define i2cInterruptClear()     {UINT32 u32tt; oplRegRead/*BitClear*/ (REG_I2C_CON, /*REG_BIT_I2CCON_SI*/(UINT32 *)&u32tt); if (nI2cDbgFd) printk ("Clear SI Interrupt ok.\n"); }
#else
#define i2cInterruptClear()     /*oplRegBitClear (REG_I2C_CON, REG_BIT_I2CCON_SI)*/ {volatile int ww=1000; for (; 0 < ww; ww--);} /* yzhang 20090608 */
#endif

#if 1
#ifdef OPL_DRV_PRINTF
#undef OPL_DRV_PRINTF
#endif
#define OPL_DRV_PRINTF
#endif

#define I2C_WAIT_NO_TRANSFER(ntimeout)  { \
    int iii; \
    for (iii = (ntimeout); iii; iii--) \
        { \
        /* check TIP bit, if set, try again */ \
        if (oplRegBitTest ((UINT32) REG_I2C_SR, (UINT32) REG_BIT_I2CSR_TIP)) \
            { \
            OPL_DRV_PRINTF (("A transfer is in process, try again...%d.\n", iii)); \
            /*vosUSleep (10); /* give times to SIO to operate */ \
            volatile int ww=1000; \
                for (; 0 < ww; ww--); \
            oplRegBitTest (REG_I2C_CON,OPL_ZERO); \
            continue; \
        } \
        break; \
    } \
    if (0 == iii) \
        { \
        OPL_DRV_PRINTF (("wait SIO stopping tranfser time out.\n")); \
        break; \
    } \
}


/* function declarations */

/*******************************************************************************
 **
 **         Chip configuration
 **
 *******************************************************************************/

/*******************************************************************************
*
* i2cControllerEnable
*
* DESCRIPTION:
*   this function enable the I2C Controller for transmission or receive.
*
* INPUTS:
*   n/a.
*
* OUTPUT:
*   n/a.
*
* RETURNS:
*   OPL_OK, the operation is successful.
*
* SEE ALSO: 
*/
OPL_STATUS i2cControllerEnable ( void )
{
    /* enable interrupt to generate SI */
    oplRegBitSet(REG_I2C_EN, REG_BIT_I2CEN_INT);

    /* enable I2CC by settting ENSIO bit */
    oplRegBitSet (REG_I2C_CON, REG_BIT_I2CCON_ENSIO);

    /* Disable I2C Timeout */
    oplRegWrite (REG_I2C_TO, OPL_ZERO);
    OPL_DRV_PRINTF (("enable I2C Controller ok.\n"));
    return OPL_OK;
}

/*******************************************************************************
*
* i2cControllerDisable
*
* DESCRIPTION:
*   this function disable the I2C Controller from transmission or receive.
*
* INPUTS:
*   n/a.
*
* OUTPUT:
*   n/a.
*
* RETURNS:
*   OPL_OK, the operation is successful.
*
* SEE ALSO: 
*/
OPL_STATUS i2cControllerDisable ( void )
{
    UINT32 regVal = 0;

    /* disable interrupt to generate SI */
    oplRegBitClear (REG_I2C_EN, REG_BIT_I2CEN_INT);

#if 0
    /* disable I2CC by clearing ENSIO bit */
    oplRegBitClear (REG_I2C_CON, REG_BIT_I2CCON_ENSIO);
#else
    oplRegWrite (REG_I2C_CON, regVal);

    /* stop I2CC by settting STO bit */
    oplRegBitSet (REG_I2C_CON, REG_BIT_I2CCON_STO);
#endif

    OPL_DRV_PRINTF (("disable I2C Controller ok.\n"));
    return OPL_OK;
}

/*******************************************************************************
*
* i2cControllerReset
*
* DESCRIPTION:
*   this function reset the I2C Controller by sequence of disabling followed enabling.
*
* INPUTS:
*   n/a.
*
* OUTPUT:
*   n/a.
*
* RETURNS:
*   OPL_OK, the operation is successful.
*
* SEE ALSO: 
*/
OPL_STATUS i2cControllerReset ( void )
{

    i2cControllerDisable ();
    i2cControllerEnable ();

    OPL_DRV_PRINTF (("reset I2C Controller ok.\n"));
    return OPL_OK;
}


/*******************************************************************************
*
* i2cMasterModeSet
*
* DESCRIPTION:
*   this function set the I2C Controller to work in master mode.
*
* INPUTS:
*   clockRate    - SCF value, see I2C_SPEED_e. if the clockRate is invalid, default 400K used.
*
* OUTPUT:
*   n/a.
*
* RETURNS:
*   OPL_OK, the operation is successful. 
*
* SEE ALSO: 
*/
OPL_STATUS i2cMasterModeSet (UINT32 clockRate )
{
    UINT32 regVal = 0;

    oplRegRead (REG_I2C_CON, &regVal);
    switch (clockRate)
    {
        case    I2C_SCF_330:    /* 0 */
        case    I2C_SCF_288:    /* 1 */
        case    I2C_SCF_217:    /* 2 */
        case    I2C_SCF_146:    /* 3 */
        case    I2C_SCF_88:     /* 4 */
        case    I2C_SCF_59:     /* 5 */
        case    I2C_SCF_44:     /* 6 */
        case    I2C_SCF_36:     /* 7 */
            regVal &= (~REG_BIT_I2CCON_CR);
            regVal |= (clockRate & REG_BIT_I2CCON_CR);
            break;
        default:
            OPL_DRV_PRINTF (("invalid I2C Controller Clock Rate value: %d, default speek 400K used.\n"));
    }

        /* clear AA bit */
    regVal &= (~REG_BIT_I2CCON_AA);
    oplRegWrite (REG_I2C_CON, regVal );

    /* yzhang, 20050919, add according to the registers 4-7 */
    oplRegBitSet(REG_I2C_EN, REG_BIT_I2CEN_MS);
    
    g_stI2CCfg.masterMode = 1;

    OPL_DRV_PRINTF (("I2C Controller now works in master mode.\n"));
    return OPL_OK;
}

/*******************************************************************************
*
* i2cSlaveModeSet
*
* DESCRIPTION:
*   this function set the I2C Controller to work in slave mode.
*
* INPUTS:
*   slvAddr  - own slave address.
*
* OUTPUT:
*   n/a.
*
* RETURNS:
*   OPL_OK, the operation is successful. 
*
* SEE ALSO: 
*/
OPL_STATUS i2cSlaveModeSet (UINT8 slvAddr )
{
    UINT32 regVal = 0;
    UINT8 regAddr = slvAddr;

    /* set I2CC own slave address */

    regAddr <<= REG_BIT_I2CADR_ADR_OFF;
    regAddr &= REG_BIT_I2CADR_ADR;
    oplRegWrite (REG_I2C_ADR, regAddr);

    /* set I2CC to slave mode by settting AA bit */
    
    oplRegRead (REG_I2C_CON, &regVal);
    
        /* set AA bit */
    oplRegBitSet (REG_I2C_CON, REG_BIT_I2CCON_AA);

    /* yzhang, 20050919, add according to the registers 4-7 */
    oplRegBitClear(REG_I2C_EN, REG_BIT_I2CEN_MS);

    g_stI2CCfg.masterMode = 0;

    OPL_DRV_PRINTF (("I2C Controller now works in slave mode.\n"));
    return OPL_OK;
}

/*******************************************************************************
*
* i2cStatusGet
*
* DESCRIPTION:
*   this function read the status code returned by the I2C Controller,
*   for interrupte mode, it just reads the status saved in g_stI2CCfg;
*   for polling mode, it reads the I2C_STA.
*
* INPUTS:
*   n/a
*
* OUTPUT:
*   n/a.
*
* RETURNS:
*   status code, or OPL_ERROR if the operation time out. 
*
* SEE ALSO: 
*/
int rrr = 1000;

int i2cStatusGet ( void )
{
    #define I2C_SIO_INTERRUPT_TIME_OUT  100
    UINT32 regVal = 0;
    UINT32 regTemp= 0;
#if 1
    int i;

#endif  
    /* get status code accordingly */
    if (g_stI2CCfg.interrupt)
    {
            /* do nothing currently */
        for (i = I2C_SIO_INTERRUPT_TIME_OUT; i; i--)
        {
            /* check SI bit, if not set, must be something wrong, try again */
            if (oplRegBitTest ((UINT32) REG_I2C_CON, (UINT32) REG_BIT_I2CCON_SI))
                break;
            else
            {
                OPL_DRV_PRINTF (("there is no SIO interrupt occurred, try again...%d.\n", i));
            }

            /*oplRtosTaskDelay (10);*/  /* give times to SIO to operate */
            {
            volatile int ww=rrr;
            for (; 0 < ww; ww--);
            }
        }
        if (0 == i)
        {
            OPL_DRV_PRINTF (("wait SIO interrupt Status Code time out.\n"));
            return OPL_ERROR;
        }
    }
    else 
    {
        /* Delay a while... */
        volatile int ww=rrr;
        for (; 0 < ww; ww--);

        oplRegRead (REG_I2C_CON, &regTemp);
    }

    oplRegRead (REG_I2C_STA, &regVal);
    g_stI2CCfg.lastStatus = g_stI2CCfg.status;
    g_stI2CCfg.status = (UINT8)regVal;

    OPL_DRV_PRINTF (("get I2C Controller Status Code ok, %x.\n", g_stI2CCfg.status));
    return (int )g_stI2CCfg.status;
}

/*******************************************************************************
*
* i2cStatusGetWhenStop
*
* DESCRIPTION:
*   this function read the status code returned by the IPMux I2C Controller,
*   for interrupte mode, it just reads the status saved in g_stI2CCfg;
*   for polling mode, it reads the I2CSTA.
*
* INPUTS:
*   n/a
*
* OUTPUT:
*   n/a.
*
* RETURNS:
*   status code, or OPL_ERROR if the operation time out. 
*
* SEE ALSO: 
*/
int i2cStatusGetWhenStop ( void )
{
    UINT32 regVal = 0;
    int i;

    /* yzhang added here, 20090608 */

        for (i = 3; i; i--)
        {
            /* Clear Interrupt and let SIO continue... */
            /*oplRegBitTest ((UINT32) REG_I2C_CON, (UINT32) REG_BIT_I2CCON_SI);*/
            
            oplRegRead (REG_I2C_STA, &regVal);
            if (0xF8 == (UINT8) regVal) break;
            OPL_DRV_PRINTF (("get I2C Controller F8 Status Code failed %d, try again....\n", i));

            /*oplRtosTaskDelay (10);*/  /* give times to SIO to operate */
            {
                volatile int ww=1000;
                for (; 0 < ww; ww--);
            }
        }

    /*oplRegRead (REG_I2C_STA, &regVal);*/
    g_stI2CCfg.lastStatus = g_stI2CCfg.status;
    g_stI2CCfg.status = (UINT8)regVal;

    OPL_DRV_PRINTF (("get I2C Controller Status Code ok, %x.\n", g_stI2CCfg.status));
    return (int )g_stI2CCfg.status;
}

/*******************************************************************************
*
* i2cInterruptEnable
*
* DESCRIPTION:
*   this function set the I2C Controller to work in interrupt or polling mode.
*
* INPUTS:
*   n/a.
*
* OUTPUT:
*   intMode - 0: polling mode, non-zero: interrupt mode.
*
* RETURNS:
*   OPL_OK, the operation is successful. 
*
* SEE ALSO: 
*/
OPL_STATUS i2cInterruptEnable ( int intMode )
{
    char * str[] = {"polling", "interrupt"};

    if (intMode)
    {
        g_stI2CCfg.interrupt = 1;

            /* yzhang, 20050919, add according to the registers 4-7 */
        oplRegBitSet(REG_I2C_EN, REG_BIT_I2CEN_INT);
    }
    else
    {
        g_stI2CCfg.interrupt = 0;

            /* yzhang, 20050919, add according to the registers 4-7 */
        oplRegBitClear(REG_I2C_EN, REG_BIT_I2CEN_INT);
    }

    OPL_DRV_PRINTF (("I2C Controller now works in %s mode.\n", str[g_stI2CCfg.interrupt]));
    return OPL_OK;
}

/*******************************************************************************
*
* i2cInterruptRoutine
*
* DESCRIPTION:
*   this function can be called when SI is set in interrupt mode. It just records the SIO
*   status code. 
*
* INPUTS:
*   n/a.
*
* OUTPUT:
*   intMode - 0: polling mode, non-zero: interrupt mode.
*
* RETURNS:
*   OPL_OK, the operation is successful. 
*   OPL_ERROR, for unexpected interrupt error. 
*
* SEE ALSO: 
*/
OPL_STATUS i2cInterruptRoutine ( void )
{
    UINT32 regVal = 0;

    /* check SI bit, if not set, must be something wrong */
    if (0 == oplRegBitTest ((UINT32) REG_I2C_CON, (UINT32) REG_BIT_I2CCON_SI))
    {
        /* OPL_DRV_PRINTF (("there is no SIO interrupt occurred.\n", i)); */
        return OPL_ERROR;
    }

    /* get status code accordingly */
    if (g_stI2CCfg.interrupt)
    {
        oplRegRead (REG_I2C_STA, &regVal);
        g_stI2CCfg.lastStatus = g_stI2CCfg.status;
        g_stI2CCfg.status = (UINT8)regVal;
    }
    else 
    {
        /* do nothing currently except report a wrong call */
        return OPL_ERROR;
    }

    return OPL_OK;
}

/*******************************************************************************
*
* i2cWrite
*
* DESCRIPTION:
*   this function transmits data to I2C bus via the I2C Controller. the function first detect
*   I2CC work mode, and initialize the transfer data accordingly.
*
*   Note: for a special case, it is needed that a read operation is followed the write, the MSB of
*   parameter slcAddr is used to be such indicator:
*   slvAddr & 0x80 == 0x80      no stop is issued after data is written;
*   slvAddr & 0x80 == 0x00      normal write operation, stop is issued;
*
* INPUTS:
*   slvAddr  - slave device address, only valid for master.
*   nByte    - biffer size in bytes.
*   pDataBuf     - pointer to data byte array.
*
* OUTPUT:
*   n/a.
*
* RETURNS:
*   # of data byte written successfully, or ERROR CODE:
*   OPL_ERROR
*   OPL_I2C_TIME_OUT
*   OPL_I2C_ARBITRATION_LOST
*   OPL_I2C_SLV_WRITE_MORE_DATA_NEED
*
* SEE ALSO: 
*/
int i2cWrite (UINT8 slvAddr, UINT32 nByte, UINT8 *pDataBuf )
{
    UINT8 regAddr = (slvAddr << REG_BIT_I2CADR_ADR_OFF) | I2C_WRITE;
    INT32 statusCode;
    int i = OPL_ZERO;
    UINT32 regVal = OPL_ZERO;
    int bErrorState = OPL_ZERO;

    /* check master or slave mode */
    if (g_stI2CCfg.masterMode)
    {
        /* for master mode */
        if (oplRegBitTest((UINT32) REG_I2C_SR, (UINT32) REG_BIT_I2CSR_BUSY))
        {
            OPL_DRV_PRINTF (("Ieee, I2C data bus is busy, you cannot Tx data before it is normal.\n"));
            return OPL_ERROR;
        }

        /* step1: set STA */
        oplRegBitSet (REG_I2C_CON, REG_BIT_I2CCON_STA);
        
        /* wait SIO status */
        statusCode = i2cStatusGet();
        switch (statusCode)
        {
            case OPL_ERROR:
                OPL_DRV_PRINTF (("wait SIO Interrupt time out after STA, write to I2C controller failed.\n"));
                return OPL_I2C_TIME_OUT;
            case 0x08:  /* A START condition has been transmitted */
            case 0x10:  /* A repeated START condition has been transmitted */
                /* step2: write SLA+W */
                /* oplRegWrite (REG_I2C_ADR, regAddr); */
                oplRegWrite (REG_I2C_DAT, regAddr);

                /* yzhang, 20050919, add according to the registers 4-7 */
                oplRegRead(REG_I2C_EN, &regVal);
                regVal &= ~(REG_BIT_I2CEN_RD);  /* clear RD bit */
                regVal |= REG_BIT_I2CEN_WR;     /* set WR bit */
                oplRegWrite(REG_I2C_EN, regVal);

                i2cInterruptClear();    /* clear SIO interrupt and let SIO continue */
                break;
            case 0x00:  /* Bus error due to illegal START or STOP condition */
            case 0x70:  /* Bus error SDA stuck LOW */
            case 0x90:  /* Bus error SCL stuck LOW */
            default:
                OPL_DRV_PRINTF (("inaptitude Status Code received: %x#, write to I2C controller failed.\n", statusCode));
                return OPL_ERROR;
        }

        /* wait SIO status */
        statusCode = i2cStatusGet();
        switch (statusCode)
        {
            case OPL_ERROR:
                OPL_DRV_PRINTF (("wait SIO Interrupt time out after SLV+W, write to I2C controller failed.\n"));
                return OPL_I2C_TIME_OUT;
            case 0x18:  /* SLA+W has been transmitted; ACK has been received */
            case 0x20:  /* SLA+W has been transmitted; NO ACK has been received */
                /* step3: write data byte by byte */
                for (i = 0; !bErrorState && (i < nByte); i++)
                {
                    /*I2C_WAIT_NO_TRANSFER(5);*/
                    oplRegWrite (REG_I2C_DAT, pDataBuf[i]);

                    /* yzhang, 20050919, add according to the registers 4-7 */
                    oplRegRead(REG_I2C_EN, &regVal);
                    regVal &= ~(REG_BIT_I2CEN_RD);  /* clear RD bit */
                    regVal |= REG_BIT_I2CEN_WR;     /* set WR bit */
                    oplRegWrite(REG_I2C_EN, regVal);

                    i2cInterruptClear();    /* clear SIO interrupt and let SIO continue */

                    /* wait SIO status */
                    statusCode = i2cStatusGet();
                    switch (statusCode)
                    {
                        case OPL_ERROR:
                            OPL_DRV_PRINTF (("wait SIO Interrupt time out after DATA[%d], write to I2C controller failed.\n", i));
                            return OPL_I2C_TIME_OUT;
                        case 0x28:  /* Data byte in I2CDAT has been transmitted;ACK has been received */
                        case 0x30:  /* Data byte in I2CDAT has been transmitted;NO ACK has been received */
                            break;
                        case 0x38:  /* Arbitration lost in NOT ACK bit */
                            OPL_DRV_PRINTF (("arbitration lost in data byte[%d].\n", i));
                            /*return OPL_I2C_ARBITRATION_LOST;*/
                            bErrorState = statusCode; 
                        default:
                            OPL_DRV_PRINTF (("Bad Status Code received %x when in Master Tx mode, data[%d], stopped.\n", statusCode, i));
                            bErrorState = statusCode; 

                    }
                }
                break;
            default:
                OPL_DRV_PRINTF (("inaptitude Status Code received: %x#, write to I2C controller failed.\n", statusCode));
                /*return OPL_ERROR;*/
                bErrorState = statusCode; 
        }

        if (ifI2cSTOStatusIssue() /* OPWAYCNU_REG_I2C_READ_FLAG */)/*(0x80 & slvAddr)*/ /* yzhang added, 20070213 */
        {
            OPL_DRV_PRINTF (("No STO issued after %d data transmitted, and returned immediately.\n", i));
            i2cInterruptClear();    /* clear SIO interrupt and let SIO continue */
            return i;
        }
        else
        {
            /* step4: write STO */
            oplRegRead(REG_I2C_CON, &regVal);
            regVal &= ~(REG_BIT_I2CCON_STA);    /* clear STA bit */
            regVal |= REG_BIT_I2CCON_STO;           /* set STO bit, STO bit is read-cleard */
            oplRegWrite(REG_I2C_CON, regVal);
        }
        i2cInterruptClear();    /* clear SIO interrupt and let SIO continue */

        /* wait SIO status */
        statusCode = i2cStatusGetWhenStop(); /* 20090608 */
        switch (statusCode)
        {
            case OPL_ERROR:
                OPL_DRV_PRINTF (("wait SIO Interrupt time out after STO, write to I2C controller failed.\n"));
                return OPL_I2C_TIME_OUT;
            case 0xF8:  /* A STOP condition has been transmitted */
                /* write finished */
                OPL_DRV_PRINTF (("write to slave device %x# ok, %d of %d bytes transmitted.\n", slvAddr, i, nByte));
                return i;
            default:
                OPL_DRV_PRINTF (("inaptitude Status Code received: %x#, write to I2C controller failed.\n", statusCode));
                return OPL_ERROR;
        }

    }
    else
    {
        /* for slave mode */

        /* step1: wait SIO status */
        statusCode = i2cStatusGet();
        switch (statusCode)
        {
            case OPL_ERROR:
                OPL_DRV_PRINTF (("get SIO Status for slave transmition time out, write to I2C controller failed.\n"));
                return OPL_I2C_TIME_OUT;
            case 0xA8:  /* Own SLA+R has been received; ACK has been returned */
            case 0xB0:  /* Own SLA+R has been received; ACK has been returned */
            case 0xb8:  /* add by xwang : Data byte in I2CDAT has been transmitted; ACK has been received */
                /* step2: write data byte by byte */
                for (i = 0; i < nByte; i++)
                {
                    oplRegWrite (REG_I2C_DAT, pDataBuf[i]);
                    OPL_DRV_PRINTF (("I2C Controller sent DATA[%d]=0x%x, \'%c\'.\n", i, pDataBuf[i], pDataBuf[i]));

                    /* yzhang, 20050919, add according to the registers 4-7 */
                    oplRegRead(REG_I2C_EN, &regVal);
                    regVal &= ~(REG_BIT_I2CEN_RD);  /* clear RD bit */
                    regVal |= REG_BIT_I2CEN_WR;             /* set WR bit */
                    oplRegWrite(REG_I2C_EN, regVal);

                    i2cInterruptClear();    /* clear SIO interrupt and let SIO continue */

                    /* wait SIO status */
                    statusCode = i2cStatusGet();
                    switch (statusCode)
                    {
                        case OPL_ERROR:
                            OPL_DRV_PRINTF (("wait SIO Interrupt time out after DATA[%d], write to I2C controller failed.\n", i));
                            return OPL_I2C_TIME_OUT;
                        case 0xB8:  /* Data byte in I2CDAT has been transmitted;ACK has been received */
                        case 0xC0:  /* Data byte in I2CDAT has been transmitted;NO ACK has been received */
                            break;
                        case 0xC8:  /* Last data byte in I2CDAT has been transmitted (AA = 0); ACK has been received */
                        case 0xF8:  /* Last data byte in I2CDAT has been transmitted (AA = 0); ACK has been received */
                            OPL_DRV_PRINTF (("remote master stop data receive after DATA[%d].\n", i));

                            /* write finished, yet not completedly */
                            OPL_DRV_PRINTF (("slave write ok, %d of %d bytes transmitted.\n", i + 1, nByte));
                            return i + 1;
                    }
                }

                /* check if the more data still in want by remote master */
                /*if ((0xB8 == statusCode) || (0xC0 == statusCode))*/
                if (0xC0 == statusCode)
                {
                    /* write finished */
                    OPL_DRV_PRINTF (("slave write stopped while more data are still in want, %d of %d bytes transmitted.\n", i, nByte));
                    return OPL_I2C_SLV_WRITE_MORE_DATA_NEED;
                }

                /* write finished */
                OPL_DRV_PRINTF (("slave write ok, %d of %d bytes transmitted.\n", i, nByte));
                return i;
            default:
                OPL_DRV_PRINTF (("inaptitude Status Code received: %x#, write to I2C controller failed.\n", statusCode));
                return OPL_ERROR;
        }
    }
}
/*****************************************************************
    Function:i2cWriteRegByteData
    Description:向i2c 从设备寄存器写入一字节数据
    Author:liaohongjun
    Date:2012/8/28
    Input: UINT8 slvAddr, UINT8 regAddr, UINT8 byteData
    Output:         
    Return:
        DRV_OK
=================================================
    Note:
*****************************************************************/
int i2cWriteRegByteData (UINT8 slvAddr, UINT8 regAddr, UINT8 byteData)
{
    I2C_REG_INFO_S stRegInfo;

    memset(&stRegInfo, 0 , sizeof(I2C_REG_INFO_S));
    stRegInfo.regAddr = regAddr;
    stRegInfo.data = byteData;
    
    return i2cWrite(slvAddr, sizeof(I2C_REG_INFO_S), &stRegInfo);
}

/*******************************************************************************
*
* i2cWriteRepeat
*
* DESCRIPTION:
*   this function transmit data to I2C bus via the I2C Controller. It should be called
*   only when OPL_I2C_SLV_WRITE_MORE_DATA_NEED returned in slave mode.
*
* INPUTS:
*   nByte    - biffer size in bytes.
*   pDataBuf     - pointer to data byte array.
*
* OUTPUT:
*   pDataBuf     - pointer to data byte received.
*
* RETURNS:
*   # of data byte read successfully, or ERROR CODE:
*   OPL_ERROR
*   OPL_I2C_TIME_OUT
*   OPL_I2C_SLV_WRITE_MORE_DATA_NEED
*
* SEE ALSO: 
*/
int i2cWriteRepeat (UINT32 nByte, UINT8 *pDataBuf )
{
    INT32 statusCode;
    int i;
    UINT32 regVal = OPL_ZERO;

    /* check master or slave mode */
    if (g_stI2CCfg.masterMode)
    {
        OPL_DRV_PRINTF (("this function should be called in master mode.\n"));
        return OPL_ERROR;
    }
    else
    {
        /* for slave mode */

        /* step1: wait SIO status */
        statusCode = i2cStatusGet();
        switch (statusCode)
        {
            case OPL_ERROR:
                OPL_DRV_PRINTF (("get SIO Status for slave transmition time out, repeat write to I2C controller failed.\n"));
                return OPL_I2C_TIME_OUT;
            case 0xB8:  /* Data byte in I2CDAT has been transmitted;ACK has been received */
            case 0xC0:  /* Data byte in I2CDAT has been transmitted;NO ACK has been received */
                    /* step2: write data byte by byte */
                for (i = 0; i < nByte; i++)
                {
                    oplRegWrite (REG_I2C_DAT, pDataBuf[i]);
                    OPL_DRV_PRINTF (("I2C Controller sent DATA[%d]=0x%x, \'%c\'.\n", i, pDataBuf[i], pDataBuf[i]));
                                                                                                        
                    /* yzhang, 20050919, add according to the registers 4-7 */
                    oplRegRead(REG_I2C_EN, &regVal);
                    regVal &= ~(REG_BIT_I2CEN_RD);  /* clear RD bit */
                    regVal |= REG_BIT_I2CEN_WR;             /* set WR bit */
                    oplRegWrite(REG_I2C_EN, regVal);

                    i2cInterruptClear();    /* clear SIO interrupt and let SIO continue */

                    /* wait SIO status */
                    statusCode = i2cStatusGet();
                    switch (statusCode)
                    {
                        case OPL_ERROR:
                            OPL_DRV_PRINTF (("wait SIO Interrupt time out after DATA[%d], write to I2C controller failed.\n", i));
                            return OPL_I2C_TIME_OUT;
                        case 0xB8:  /* Data byte in I2CDAT has been transmitted;ACK has been received */
                        case 0xC0:  /* Data byte in I2CDAT has been transmitted;NO ACK has been received */
                            break;
                        case 0xC8:  /* Last data byte in I2CDAT has been transmitted (AA = 0); ACK has been received */
                        case 0xF8:  /* Last data byte in I2CDAT has been transmitted (AA = 0); ACK has been received */
                            OPL_DRV_PRINTF (("remote master stop data receive after DATA[%d].\n", i));

                            /* write finished, yet not completedly */
                            OPL_DRV_PRINTF (("slave write ok, %d of %d bytes transmitted.\n", i + 1, nByte));
                            return i + 1;
                    }
                }

                /* check if the more data still in want by remote master */
                /*if ((0xB8 == statusCode) || (0xC0 == statusCode))*/
                if (0xC0 == statusCode)
                {
                    /* write finished */
                    OPL_DRV_PRINTF (("slave repeat write stopped while more data are still in want, %d of %d bytes transmitted.\n", i, nByte));
                    return OPL_I2C_SLV_WRITE_MORE_DATA_NEED;
                }
                
                /* repeat write finished, might never come here. */
                OPL_DRV_PRINTF (("slave write ok, %d of %d bytes transmitted.\n", i, nByte));
                return i;
            default:
                OPL_DRV_PRINTF (("inaptitude Status Code received: %x#, write to I2C controller failed.\n", statusCode));
                return OPL_ERROR;
        }
    }
}


/*******************************************************************************
*
* i2cRead - i2c current address read
*
* DESCRIPTION:
*   this function receives data from I2C bus via the I2C Controller. the function first detect
*   I2CC work mode, and initialize the receiver accordingly.
*
* INPUTS:
*   slvAddr  - slave device address, only valid for master.
*   nByte    - biffer size in bytes.
*   pDataBuf     - pointer to data byte array.
*
* OUTPUT:
*   pDataBuf     - pointer to data byte received.
*
* RETURNS:
*   # of data byte read successfully, or ERROR CODE:
*   OPL_ERROR
*   OPL_I2C_TIME_OUT
*   OPL_I2C_ARBITRATION_LOST
*   OPL_I2C_SLV_READ_BUFFER_TOO_SMALL
*
* SEE ALSO: 
*/
int i2cRead (UINT8 slvAddr, UINT32 nByte, UINT8 *pDataBuf )
{
    UINT8 regAddr = (slvAddr << REG_BIT_I2CADR_ADR_OFF) | I2C_READ;
    INT32 statusCode;
    UINT32 regData;
    int i;
    UINT32 regVal = OPL_ZERO;
    int bErrorState = OPL_ZERO;

    /* check master or slave mode */
    if (g_stI2CCfg.masterMode)
    {
        /* for master mode */
    
        /* step1: set STA */
        oplRegBitSet (REG_I2C_CON, REG_BIT_I2CCON_STA | REG_BIT_I2CCON_AA);
        
        /* wait SIO status */
        statusCode = i2cStatusGet();
        switch (statusCode)
        {
            case OPL_ERROR:
                OPL_DRV_PRINTF (("wait SIO Interrupt time out after STA, read from I2C controller failed.\n"));
                return OPL_I2C_TIME_OUT;
            case 0x08:  /* A START condition has been transmitted */
            case 0x10:  /* A repeated START condition has been transmitted */
                /* step2: write SLA+R */
                oplRegWrite (REG_I2C_DAT, regAddr);

                /* yzhang, 20050919, add according to the registers 4-7 */
                oplRegRead(REG_I2C_EN, &regVal);
                regVal &= ~(REG_BIT_I2CEN_RD);      /* clear RD bit */
                regVal |= REG_BIT_I2CEN_WR;             /* set WR bit */
                oplRegWrite(REG_I2C_EN, regVal);
                i2cInterruptClear();    /* clear SIO interrupt and let SIO continue */
                break;
            default:
                OPL_DRV_PRINTF (("inaptitude Status Code received: %x#, read from I2C controller failed.\n", statusCode));
                return OPL_ERROR;
        }

        /* wait SIO status */
        statusCode = i2cStatusGet();
        switch (statusCode)
        {
            case OPL_ERROR:
                OPL_DRV_PRINTF (("wait SIO Interrupt time out after SLV+R, read from I2C controller failed.\n"));
                return OPL_I2C_TIME_OUT;

            case 0x40:  /* SLA+R has been transmitted; ACK has been received */
            case 0x48:  /* SLA+R has been transmitted; NO ACK has been received */
                /* step3: read data byte by byte */

                for (i = 0; !bErrorState && (i < nByte); i++)
                {
                    #define ACCESS_DATA_PRIOR_STATUS
                    /*
                     *  Note: this MACRO is defined to switch between two method
                     *  CNU version 20070213 dose not support ACCESS_DATA_PRIOR_STATUS
                     *  please undefined the MACRO
                     */
                    #undef ACCESS_DATA_PRIOR_STATUS

                    /* the last byte does not need ACK, 20070531 */
                    if (i == (nByte - 1))
                    {
                        oplRegBitClear (REG_I2C_CON, REG_BIT_I2CCON_AA);
                    }
                    
                    /* yzhang, 20050919, add according to the registers 4-7 */
                    oplRegRead(REG_I2C_EN, &regVal);
                    regVal &= ~(REG_BIT_I2CEN_WR);          /* clear WR bit */
                    regVal |= REG_BIT_I2CEN_RD;             /* set RD bit */
                    oplRegWrite(REG_I2C_EN, regVal);
#ifdef ACCESS_DATA_PRIOR_STATUS
                    oplRegRead (REG_I2C_DAT, &regData);
                    pDataBuf[i] = (UINT8)regData;
                    OPL_DRV_PRINTF (("Get byte[%d]=0x%x,\'%c\'.\n", i, pDataBuf[i], (char)pDataBuf[i]));
#endif
                    i2cInterruptClear();    /* clear SIO interrupt and let SIO continue */

                    /* wait SIO status */
                    statusCode = i2cStatusGet();
                    switch (statusCode)
                    {
                        case OPL_ERROR:
                            OPL_DRV_PRINTF (("wait SIO Interrupt time out after DATA[%d], read from I2C controller failed.\n", i));
                            return OPL_I2C_TIME_OUT;
                        case 0x50:  /* Data byte in I2CDAT has been received;ACK has been received */
                        case 0x58:  /* Data byte in I2CDAT has been received;NO ACK has been received */
#ifndef ACCESS_DATA_PRIOR_STATUS
                            oplRegRead (REG_I2C_DAT, &regData);
                            pDataBuf[i] = (UINT8)regData;
                            OPL_DRV_PRINTF (("Get byte[%d]=0x%x,\'%c\'.\n", i, pDataBuf[i], (char)pDataBuf[i]));
#endif
                            break;
                        case 0x38:  /* Arbitration lost in NOT ACK bit  */
                            OPL_DRV_PRINTF (("arbitration lost after data byte[%d] read.\n", i));
                            return OPL_I2C_ARBITRATION_LOST;
                        default:
                            OPL_DRV_PRINTF (("Bad status code %x got while Master Rx, data byte[%d], stopped.\n", statusCode, i));
                            bErrorState = statusCode;
                    }
                }
                break;
            case 0x38:  /* Arbitration lost in SLA+R */
                OPL_DRV_PRINTF (("arbitration lost in SLA+R.\n"));
                return OPL_I2C_ARBITRATION_LOST;
            default:
                OPL_DRV_PRINTF (("inaptitude Status Code received: %x#, write to I2C controller failed.\n", statusCode));
                return OPL_ERROR;
        }

        /* step4: write STO */
        oplRegRead(REG_I2C_CON, &regVal);
        regVal &= ~(REG_BIT_I2CCON_STA | REG_BIT_I2CCON_AA);        /* clear STA bit */
        regVal |= REG_BIT_I2CCON_STO;           /* set STO bit, STO bit is read-cleard */
        oplRegWrite(REG_I2C_CON, regVal);
        i2cInterruptClear();    /* clear SIO interrupt and let SIO continue */

        /* wait SIO status */
        statusCode = i2cStatusGetWhenStop(); /* 20090608 */
        switch (statusCode)
        {
            case OPL_ERROR:
                OPL_DRV_PRINTF (("wait SIO Interrupt time out after STO, read from I2C controller failed.\n"));
                return OPL_I2C_TIME_OUT;
            case 0xF8:  /* A STOP condition has been transmitted */
                    /* write finished */
                OPL_DRV_PRINTF (("read from slave device %x# ok, %d bytes received.\n", slvAddr, i));
                return i;
            default:
                OPL_DRV_PRINTF (("inaptitude Status Code received: %x#, read from I2C controller failed.\n", statusCode));
                return OPL_ERROR;
        }

    }
    else
    {
        /* for slave mode */

        /* yzhang, 20050919, add according to the registers 4-7 */
        oplRegRead(REG_I2C_EN, &regVal);
        regVal &= ~(REG_BIT_I2CEN_WR);      /* clear WR bit */
        regVal |= REG_BIT_I2CEN_RD;             /* set RD bit */
        oplRegWrite(REG_I2C_EN, regVal);

        /* step1: wait SIO status */
        statusCode = i2cStatusGet();
        switch (statusCode)
        {
            case OPL_ERROR:
                OPL_DRV_PRINTF (("get SIO Status for slave transmition time out, read from I2C controller failed.\n"));
                return OPL_I2C_TIME_OUT;
            case 0x68:  /* Own SLA+W has been received; ACK has been returned */
            case 0x60:  /* Own SLA+W has been received; ACK has been returned */
                    /* step2: read data byte by byte */
                for (i = 0; i < (nByte + 1); i++)
                {
                    oplRegRead (REG_I2C_DAT, &regData);
                    pDataBuf[i] = (UINT8)regData;
                    OPL_DRV_PRINTF (("Get byte=0x%x,\'%c\'.\n", pDataBuf[i], (char)pDataBuf[i]));
                    i2cInterruptClear();    /* clear SIO interrupt and let SIO continue */

                    /* wait SIO status */
                    statusCode = i2cStatusGet();
                    switch (statusCode)
                    {
                        case OPL_ERROR:
                            OPL_DRV_PRINTF (("wait SIO Interrupt time out after DATA[%d], read from I2C controller failed.\n", i));
                            return OPL_I2C_TIME_OUT;
                        case 0x88:  /* Data byte in I2CDAT has been received;NO ACK has been received */
                        case 0x80:  /* Data byte in I2CDAT has been received;ACK has been received */
                            break;
                        case 0xA0:  /* A STOP condition or repeated START condition has been received while still addressed as SLV/REC */
                        case 0xF8:  /* A STOP condition or reset condition has been received*/
                            i2cInterruptClear();    /* clear SIO interrupt and let SIO continue */
                            OPL_DRV_PRINTF (("remote master stop data transfer after DATA[%d].\n", i));

                            /* read finished */
                            OPL_DRV_PRINTF (("slave read ok, %d bytes received.\n", i + 1));
                            return i;
                    }
                }

                /* check if the buffer overflow */
                if ((0x80 == statusCode) || (0x88 == statusCode))
                {
                    /* read finished */
                    OPL_DRV_PRINTF (("slave read stopped for overflow of receive buffer, %d of %d bytes received.\n", i, nByte));
                    return OPL_I2C_SLV_READ_BUFFER_TOO_SMALL;
                }
                
                /* read finished, might never come here. */
                OPL_DRV_PRINTF (("slave read ok, %d bytes received.\n", i));
                return i;
            /* add by xwang : in polling mode, just one byte can be received because their is no fifo in I2C */
            case 0x80:  /* Data byte in I2CDAT has been received;ACK has been received */

                oplRegRead (REG_I2C_DAT, &regData);
                pDataBuf[0] = (UINT8)regData;
                i2cInterruptClear();    /* clear SIO interrupt and let SIO continue */
                return 1;
                break;
            default:
                OPL_DRV_PRINTF (("inaptitude Status Code received: %x#, read from I2C controller failed.\n", statusCode));
                return OPL_ERROR;
        }
    }
}

/*******************************************************************************
*
* i2cReadRandom - i2c Random Read
*
* DESCRIPTION:
*   this function receives data from I2C bus via the I2C Controller. the function first detect
*   I2CC work mode, and initialize the receiver accordingly.
*
* INPUTS:
*   slvAddr  - slave device address, only valid for master.
*   staRegAddr  - start register address, only valid for master.
*   nByte    - biffer size in bytes.
*   pDataBuf     - pointer to data byte array.
*
* OUTPUT:
*   pDataBuf     - pointer to data byte received.
*
* RETURNS:
*   # of data byte read successfully, or ERROR CODE:
*   OPL_ERROR
*   OPL_I2C_TIME_OUT
*   OPL_I2C_ARBITRATION_LOST
*   OPL_I2C_SLV_READ_BUFFER_TOO_SMALL
*
* SEE ALSO: 
*/
int i2cReadRandom (UINT8 slvAddr, UINT8 staRegAddr, UINT32 nByte, UINT8 *pDataBuf )
{
    int retVal = 0;
	/* check master or slave mode */
	if (g_stI2CCfg.masterMode)
	{
		/* for master mode */
        retVal = i2cWrite(slvAddr, 1, &staRegAddr);
        if (0 > retVal)
        {
            OPL_DRV_PRINTF (("I2C write start register address failed. (%d)\n", retVal));
            return retVal;
        }
	}
    return i2cRead(slvAddr, nByte, pDataBuf);
}


/*******************************************************************************
*
* i2cReadRepeat
*
* DESCRIPTION:
*   this function receives data from I2C bus via the I2C Controller. It should be called
*   only when OPL_I2C_SLV_READ_BUFFER_TOO_SMALL returned in slave mode.
*
* INPUTS:
*   nByte    - biffer size in bytes.
*   pDataBuf     - pointer to data byte array.
*
* OUTPUT:
*   pDataBuf     - pointer to data byte received.
*
* RETURNS:
*   # of data byte read successfully, or ERROR CODE:
*   OPL_ERROR
*   OPL_I2C_TIME_OUT
*   OPL_I2C_SLV_READ_BUFFER_TOO_SMALL
*
* SEE ALSO: 
*/
int i2cReadRepeat (UINT32 nByte, UINT8 *pDataBuf )
{
    INT32 statusCode;
    UINT32 regData;
    int i;

    /* check master or slave mode */
    if (g_stI2CCfg.masterMode)
    {
        OPL_DRV_PRINTF (("this function should be called in master mode.\n"));
        return OPL_ERROR;
    }
    else
    {
        /* for slave mode */

        /* step1: wait SIO status */
        statusCode = i2cStatusGet();
        switch (statusCode)
        {
            case OPL_ERROR:
                OPL_DRV_PRINTF (("get SIO Status for slave transmition time out, repeat read from I2C controller failed.\n"));
                return OPL_I2C_TIME_OUT;
            case 0x88:  /* Data byte in I2CDAT has been received;NO ACK has been received */
            case 0x80:  /* Data byte in I2CDAT has been received;ACK has been received */
                    /* step2: read data byte by byte */
                for (i = 0; i < nByte; i++)
                {
                    oplRegRead (REG_I2C_DAT, &regData);
                    pDataBuf[i] = (UINT8)regData;
                    i2cInterruptClear();    /* clear SIO interrupt and let SIO continue */

                    /* wait SIO status */
                    statusCode = i2cStatusGet();
                    switch (statusCode)
                    {
                        case OPL_ERROR:
                            OPL_DRV_PRINTF (("wait SIO Interrupt time out after DATA[%d], read from I2C controller failed.\n", i));
                            return OPL_I2C_TIME_OUT;
                        case 0x88:  /* Data byte in I2CDAT has been received;NO ACK has been received */
                        case 0x80:  /* Data byte in I2CDAT has been received;ACK has been received */
                            break;
                        case 0xA0:  /* A STOP condition or repeated START condition has been received while still addressed as SLV/REC */
                        case 0xF8:  /* A STOP condition or reset condition has been received*/
                            i2cInterruptClear();    /* clear SIO interrupt and let SIO continue */
                            OPL_DRV_PRINTF (("remote master stop data transfer after DATA[%d].\n", i));

                            /* read finished */
                            OPL_DRV_PRINTF (("slave read ok, %d bytes received.\n", i + 1));
                            return i;
                    }
                }

                /* check if the buffer overflow */
                if ((0x80 == statusCode) || (0x88 == statusCode))
                {
                    /* read finished */
                    OPL_DRV_PRINTF (("slave repeat read stopped for overflow of receive buffer, %d of %d bytes received.\n", i, nByte));
                    return OPL_I2C_SLV_READ_BUFFER_TOO_SMALL;
                }
                
                /* repeat read finished, might never come here. */
                OPL_DRV_PRINTF (("slave repeat read ok, %d bytes received.\n", i));
                return i;
            default:
                OPL_DRV_PRINTF (("inaptitude Status Code received: %x#, repeat read from I2C controller failed.\n", statusCode));
                return OPL_ERROR;
        }
    }
}

/*******************************************************************************
*
* i2cInit  -initial routine for I2C
*
* DESCRIPTION:
*   Call this function to start I2C device
*   in this function, 
*
* INPUTS:
*   i2cMode - zero: slave mode; non-zero: master mode. 
*   intMode - 0: polling mode; 1: interrupt mode.
*   slvAddr - if slave mode, this is the slave address.
*   nRate   - see below:
*       I2C_SCF_330:     0
*       I2C_SCF_288:     1
*       I2C_SCF_217:     2
*       I2C_SCF_146:     3
*       I2C_SCF_88:      4
*       I2C_SCF_59:      5
*       I2C_SCF_44:      6
*       I2C_SCF_36:      7
*
*
* OUTPUT:
*   n/a.
*
* RETURNS:
*   OPL_OK.
*
* SEE ALSO: 
*/
int i2cInit (int i2cMode, int intMode, UINT8 slvAddr, int nRate)
{

    /* reset the I2C first */
    i2cControllerReset ();
    i2cControllerReset ();

    i2cControllerEnable ();

    /* master or slave */
    if (i2cMode)
    {
        OPL_DRV_PRINTF (("I2C is in master mode\n"));
        i2cMasterModeSet(nRate /* I2C_SCF_330 */); 
    }
    else
    {
        OPL_DRV_PRINTF (("I2C is in slave mode\n"));
        i2cSlaveModeSet(slvAddr);
    }

    /* interrupt Mode */
    i2cInterruptEnable(intMode);

    OPL_DRV_PRINTF (("I2C initialized ok.\n"));
    return OPL_OK;
}

/*******************************************************************************
*
* i2cpr         -Multiple Read I2C device
*
* DESCRIPTION:
*        read registers and print the value of specified register to STANDARD OUTPUT device.
*
* INPUTS:
*        regid         - register ID.
*        num          - number of registers.
*
* OUTPUT:
*        n/a.
*
* RETURNS:
*       OPL_OK, if the read successful.
*
* SEE ALSO: 
*/
OPL_STATUS i2cpr16 (UINT8 slave,UINT16 regisd)
{
    UINT8 pVal;
    int nRet;
    UINT32 i = 2000;
    UINT8 u8Buf[3];
    u8Buf[0]= (UINT8)(regisd/0x100);
    u8Buf[1]= (UINT8)(regisd%0x100);
    
    

    OT_REG_I2C_READ_FLAG_SET;
    nRet = i2cWrite( (OT_EPROM_I2C_ADDR | slave), (UINT32) 2, (UINT8 *) u8Buf);
    OT_REG_I2C_READ_FLAG_CLEAR;
    if (OPL_ZERO >= nRet)
    {
        printk ("I2C write failed, %d.\n",  nRet);
        return OPL_ERROR;
    }
    while(i-->0);
    nRet = i2cReadRandom( OT_EPROM_I2C_ADDR, 0, (UINT32) 1, (UINT8 *) &pVal);
    if (OPL_ZERO >= nRet)
    {
        printk ("I2C read failed, %d.\n",  nRet);
        return OPL_ERROR;
    }
    printk ("Read [0x%02x]EPROM address at 0x%02x%02x, value= 0x%02x.\n",(OT_EPROM_I2C_ADDR | slave), u8Buf[0],u8Buf[1], pVal);
    return OPL_OK;
}
 
/*******************************************************************************
*
* i2cpw         -Multiple Read I2C device
*
* DESCRIPTION:
*        read registers and print the value of specified register to STANDARD OUTPUT device.
*
* INPUTS:
*        regid         - register ID.
*        num          - number of registers.
*
* OUTPUT:
*        n/a.
*
* RETURNS:
*       OPL_OK, if the read successful.
*
* SEE ALSO: 
*/
OPL_STATUS i2cpw16 (UINT8 slave,UINT16 regisd, UINT8 val)
{
    int nRet;
    UINT8 u8Buf[3];

    /* put data to buffer */
    u8Buf[0] = (UINT8)(regisd/0x100);
    u8Buf[1] = (UINT8)(regisd%0x100);
    u8Buf[2] = val;

    /* write the buffer to I2C device */
    nRet = i2cWrite( (OT_EPROM_I2C_ADDR | slave), 3 , (UINT8 *) u8Buf);
    if (OPL_ZERO >= nRet)
    {
        printk ("I2C write failed, %d.\n",  nRet);
        return OPL_ERROR;
    }

    printk ("Write [0x%02x]EPROM address at 0x%02x%02x, value= 0x%02x.\n",(OT_EPROM_I2C_ADDR | slave), u8Buf[0],u8Buf[1], (UINT32)val);
    return OPL_OK;
}


void ziic(int nLoop, int nByte, UINT8 * pDataBuf)
{
    int k = 0;
    int nRet;
    
    for (; k < nLoop; k++)
    {
        nRet = i2cRead (6, (UINT32) nByte, (UINT8 *) pDataBuf);
        if (nRet != nByte) 
        {
            printk ("Chu Cuo La......., :-o\n");
            break; 
        }
        printk ("%04d\r", k);
    }
    printk ("Finished at %d/%d times, status=0x%02x.\n", k, nLoop, nRet);
}


int micro_read (int nBlock, int nOffset, int nByte, char * pDataBuf)
{
    UINT8 slvAddr = (0xa << 3) | ((UINT8)nBlock & 0x7);
    
    int nRet = i2cWrite(slvAddr, 1, (UINT8 *) &nOffset);
    if (nRet != 1) 
    {
        printk ("Error occured when write the offset to EEPROM chip......., :-(\n");
        return OPL_ERROR;
    }
    return i2cRead(slvAddr, nByte, (UINT8 *) pDataBuf);
}

int micro_write (int nBlock, int nOffset, int nByte, char * pDataBuf)
{
    UINT8 slvAddr = (0xa << 3) | ((UINT8)nBlock & 0x7);
    UINT8 pDataOffBuf[258];

    pDataOffBuf[0] = (UINT8)nOffset;
    memcpy ((void *)((unsigned int)pDataOffBuf + 1), (const void *)pDataBuf, (unsigned int)nByte);
    return  i2cWrite(slvAddr, 1 + nByte, (UINT8 *) &pDataOffBuf);
}

int zmic(int nLoop, int nByte, UINT8 * pDataBuf)
{
    int k = 0;
    int nRet;
    
    for (; k < nLoop; k++)
    {
        nRet = micro_read (6, 0, (UINT32) nByte, (UINT8 *) pDataBuf);
        if (nRet != nByte) 
        {
            printk ("Chu Cuo La......., :-o\n");
            break; 
        }
        printk ("%04d\r", k);
    }
    printk ("Finished at %d/%d times, status=0x%02x.\n", k, nLoop, nRet);
    return nRet;
}

void zmod(int nLoop, int nByte, UINT8 * pDataBuf)
{
    int k = 0;
    int nRet;
    int nMod[9] = {0x40, 0x41,0x42,0x43,0x44,0x45,0x46,0x47,0};
    UINT32 regVal;
    
    i2cInit (1, 1, 0, 0);
    for (; nMod[k]; k++)
    {
        oplRegWrite(REG_I2C_CON, nMod[k]);
        oplRegRead(REG_I2C_CON, &regVal);
        printk ("test clock mode of 0x%02x, interrupt mode.\n", regVal);
        nRet = zmic (nLoop, nByte, pDataBuf);
        if (nRet != nByte) 
        {
            printk ("Chu Cuo La......., at mode 0x%02x:-o\n", nMod[k]);
            i2cInit (1, 1, 0, k);
            break; 
        }
    }

    i2cInit (1, 0, 0, 0);
    for (k=0; nMod[k]; k++)
    {
        oplRegWrite(REG_I2C_CON, nMod[k]);
        oplRegRead(REG_I2C_CON, &regVal);
        printk ("test clock mode of 0x%02x, Polling mode.\n", regVal);
        nRet = zmic (nLoop, nByte, pDataBuf);
        if (nRet != nByte) 
        {
            printk ("Chu Cuo La......., at mode 0x%02x:-o\n", nMod[k]);
            i2cInit (1, 0, 0, k);
            break; 
        }
    }

}

