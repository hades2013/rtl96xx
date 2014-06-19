/*************************************************************************
*
*  COPYRIGHT (C) 2003-2009 Opulan Technologies Corp. ALL RIGHTS RESERVED.  
*
*                       Proprietary and Confidential
*
* 	This software is made available only to customers and prospective
* 	customers of Opulan Technologies Corporation under license and may be
*	used only with Opulan semi-conductor products. 
*
* FILENAME:  hal_global.c
*
* DESCRIPTION: 
*	
*
* Date Created: Mar 18, 2009
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/hal/src/hal_global.c#1 $
* $Log:$
*
*
**************************************************************************/

#include <vos.h>
#include <hal.h>
#include "opconn_usr_ioctrl.h"


extern OPCONN_TAB_INFO_t opconn_tab_info[OPCONN_TAB_END+1];


/*******************************************************************************
*
* halGBLRegRead:	 read register
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	pParam -    pointer to the parameter of reading register 
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halGBLRegRead(VOID *pParam)
{
    HAL_API_GBL_REG_t *pRegParam = (HAL_API_GBL_REG_t *)pParam;

    if (NULL == pRegParam)
    {
        return;
    }

    pRegParam->ulResult = oplRegRead(pRegParam->ulAddr*4,&pRegParam->ulValue);
}

/*******************************************************************************
*
* halGBLRegWrite:	 write register
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	pParam -    pointer to the parameter of writing register
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halGBLRegWrite(VOID *pParam)
{
    HAL_API_GBL_REG_t *pRegParam = (HAL_API_GBL_REG_t *)pParam;

    if (NULL == pRegParam)
    {
        return;
    }

    pRegParam->ulResult = oplRegWrite(pRegParam->ulAddr*4,pRegParam->ulValue);
}

/*******************************************************************************
*
* halGBLRegFieldRead:	 read register fields
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	pParam -    pointer to the parameter of reading register field
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halGBLRegFieldRead(VOID *pParam)
{
    HAL_API_GBL_REG_FIELDS_t *pRegParam = (HAL_API_GBL_REG_FIELDS_t *)pParam;

    if (NULL == pRegParam)
    {
        return;
    }

    pRegParam->ulResult = oplRegFieldRead(
        pRegParam->ulAddr*4,
        pRegParam->usFieldOffset,
        pRegParam->usFieldWidth,
        &pRegParam->ulFieldData);
}

/*******************************************************************************
*
* halGBLRegFieldWrite:	 write register fields
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	pParam -    pointer to the parameter of writing register fields
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halGBLRegFieldWrite(VOID *pParam)
{
    HAL_API_GBL_REG_FIELDS_t *pRegParam = (HAL_API_GBL_REG_FIELDS_t *)pParam;

    if (NULL == pRegParam)
    {
        return;
    }

    pRegParam->ulResult = oplRegFieldWrite(
        pRegParam->ulAddr*4,
        pRegParam->usFieldOffset,
        pRegParam->usFieldWidth,
        pRegParam->ulFieldData);
}

/*******************************************************************************
*
* halGBLTabRead:	 read table
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	pParam -    pointer to the parameter of reading table
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halGBLTabRead(VOID *pParam)
{
    HAL_API_GBL_TAB_t *pTabParam = (HAL_API_GBL_TAB_t *)pParam;

    if (NULL == pTabParam)
    {
        return;
    }

    if (NULL == pTabParam->pulRecordData)
    {
        pTabParam->ulResult = OPL_ERR_NULL_POINTER;
        return;
    }

    pTabParam->ulResult = oplTabRead(pTabParam->ulRegion,
        (UINT32)pTabParam->usRecordStart,
        (UINT32)pTabParam->usRecordNum,
        (UINT32 *)&pTabParam->pulRecordData);
}

/*******************************************************************************
*
* halGBLTabWrite:	 write table
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	pParam -    pointer to the parameter of writing table
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halGBLTabWrite(VOID *pParam)
{
    HAL_API_GBL_TAB_t *pTabParam = (HAL_API_GBL_TAB_t *)pParam;

    if (NULL == pTabParam)
    {
        return;
    }

    if (NULL == pTabParam->pulRecordData)
    {
        pTabParam->ulResult = OPL_ERR_NULL_POINTER;
        return;
    }

    pTabParam->ulResult = oplTabWrite(pTabParam->ulRegion,
        pTabParam->usRecordStart,
        pTabParam->usRecordNum,
        pTabParam->pulRecordData);
}

/*******************************************************************************
*
* halGBLRegBitTest:	 test register bits
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	pParam -    pointer to the parameter of testing register bits
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halGBLRegBitTest(VOID *pParam)
{
    HAL_API_GBL_REG_BITS_t *pRegBits = (HAL_API_GBL_REG_BITS_t *)pParam;

    if (NULL == pRegBits)
    {
        return;
    }

    pRegBits->ulResult = oplRegBitTest(pRegBits->ulRegAddr*4, pRegBits->ulBits);
}

/*******************************************************************************
*
* halGBLRegBitSet:  set register bits
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	pParam -    pointer to the parameter of setting register bits
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halGBLRegBitSet(VOID *pParam)
{
    HAL_API_GBL_REG_BITS_t *pRegBits = (HAL_API_GBL_REG_BITS_t *)pParam;

    if (NULL == pRegBits)
    {
        return;
    }

    oplRegBitSet(pRegBits->ulRegAddr*4, pRegBits->ulBits);
    pRegBits->ulResult = NO_ERROR;
}


/*******************************************************************************
*
* halGBLRegBitClear:    clear register bits
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	pParam -    pointer to the parameter of clearing register bits
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halGBLRegBitClear(VOID *pParam)
{
    HAL_API_GBL_REG_BITS_t *pRegBits = (HAL_API_GBL_REG_BITS_t *)pParam;

    if (NULL == pRegBits)
    {
        return;
    }

    oplRegBitClear(pRegBits->ulRegAddr*4, pRegBits->ulBits);
    pRegBits->ulResult = NO_ERROR;
}

/*******************************************************************************
*
* halGBLRegBitToggle:   toggle register bits
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	pParam -    pointer to the parameter of toggling register bits
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halGBLRegBitToggle(VOID *pParam)
{
    HAL_API_GBL_REG_BITS_t *pRegBits = (HAL_API_GBL_REG_BITS_t *)pParam;

    if (NULL == pRegBits)
    {
        return;
    }

    oplRegBitToggle(pRegBits->ulRegAddr*4, pRegBits->ulBits);
    pRegBits->ulResult = NO_ERROR;
}


/*******************************************************************************
*
* halGBLOrr:    read register for debugging
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	pParam -    pointer to the parameter of reading register for debugging
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halGBLOrr(VOID *pParam)
{
    UINT32 regVal;
    HAL_API_GBL_ORR_t *pRegParam = (HAL_API_GBL_ORR_t *)pParam;

    if (NULL == pRegParam)
    {
        return;
    }
    
    if (0 == pRegParam->ulFd)
    {
        pRegParam->ulFd = OP_STDOUT_FILENO;
    }

    if(OK == oplRegRead(pRegParam->ulRegAddr*4,&regVal))
    {
        vosPrintf(pRegParam->ulFd, "Register 0x%08X, value: 0x%08X\r\n",pRegParam->ulRegAddr,regVal);
    }
}

/*******************************************************************************
*
* halGBLOwr:    write register for debugging
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	pParam -    pointer to the parameter of writing register for debugging
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halGBLOwr(VOID *pParam)
{
    HAL_API_GBL_OWR_t *pRegParam = (HAL_API_GBL_OWR_t *)pParam;

    if (NULL == pRegParam)
    {
        return;
    }
    #if 0
    if (0 == pRegParam->ulFd)
    {
        pRegParam->ulFd = OP_STDOUT_FILENO;
    }
	#endif

    if(OK == oplRegWrite(pRegParam->ulRegAddr*4,pRegParam->ulRegVal))
    {
    	if(pRegParam->ulFd != 0)
        	vosPrintf(pRegParam->ulFd, "Register 0x%08X, value: 0x%08X\r\n",pRegParam->ulRegAddr,pRegParam->ulRegVal);
    }
}


/*******************************************************************************
*
* halGBLMrr:	 read multiple register for debugging
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	pParam -    pointer to the parameter of reading multiple register for debugging
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halGBLMrr(VOID *pParam)
{
    UINT32 regVal;
    UINT32 index;
    HAL_API_GBL_MRR_t *pRegParam = (HAL_API_GBL_MRR_t *)pParam;

    if (NULL == pRegParam)
    {
        return;
    }
    
    if (0 == pRegParam->ulFd)
    {
        pRegParam->ulFd = OP_STDOUT_FILENO;
    }

    vosPrintf(pRegParam->ulFd, " Address     Value\r\n");
    vosPrintf(pRegParam->ulFd, "----------  ----------\r\n");
    for(index = 0; index < pRegParam->ulRegNum; index++,regVal = OPL_ZERO)
    {
        oplRegRead((pRegParam->ulRegAddr+index)*4,&regVal);
        vosPrintf(pRegParam->ulFd, "0x%08X  0x%08X\r\n",(pRegParam->ulRegAddr+index),regVal);
    }
}

/*******************************************************************************
*
* halGBLMwr:	 write multiple register for debugging
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	pParam -    pointer to the parameter of writing multiple register for debugging
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halGBLMwr(VOID *pParam)
{
    UINT32 regVal;
    UINT32 index;
    HAL_API_GBL_MWR_t *pRegParam = (HAL_API_GBL_MWR_t *)pParam;

    if (NULL == pRegParam)
    {
        return;
    }
    
    if (0 == pRegParam->ulFd)
    {
        pRegParam->ulFd = OP_STDOUT_FILENO;
    }

    vosPrintf(pRegParam->ulFd, " Address     Value\r\n");
    vosPrintf(pRegParam->ulFd, "----------  ----------\r\n");
    regVal = pRegParam->ulNewVal;
    for(index = 0; index < pRegParam->ulRegNum; index++,regVal++)
    {
        if (pRegParam->ulMode) /* fixed value */
        {
            oplRegWrite((pRegParam->ulRegAddr+index)*4, regVal);
            vosPrintf(pRegParam->ulFd, "0x%08X  0x%08X\r\n",pRegParam->ulRegAddr+index,regVal);
        }
        else /* fixed register */
        {
            oplRegWrite(pRegParam->ulRegAddr*4, regVal+index);
            vosPrintf(pRegParam->ulFd, "0x%08X  0x%08X\r\n",pRegParam->ulRegAddr,regVal+index);
        }
    }
}

/*******************************************************************************
*
* halGBLOst:	 read table description for debugging
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	pParam -    pointer to the parameter of reading table description for debugging
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halGBLOst(VOID *pParam)
{
    HAL_API_GBL_OST_t *pOstParam = (HAL_API_GBL_OST_t *)pParam;
    UINT32 tabIndex;

    if (NULL == pOstParam)
    {
        return;
    }
    
    if (0 == pOstParam->ulFd)
    {
        pOstParam->ulFd = OP_STDOUT_FILENO;
    }

    vosPrintf(pOstParam->ulFd, " Table ID     Record Num   Description\r\n");
    vosPrintf(pOstParam->ulFd, "------------ ------------ ----------------\r\n");
    for(tabIndex = OPCONN_TAB_CLS_MASK; tabIndex < OPCONN_TAB_END; tabIndex++)
    {
        vosPrintf(pOstParam->ulFd, "%-12d %-12d %-16s\r\n",  
            opconn_tab_info[tabIndex].tabId,
            opconn_tab_info[tabIndex].numOfRec,
            opconn_tab_info[tabIndex].desc);
    }
}

/*******************************************************************************
*
* halGBLOrt:	 read table for debugging
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	pParam -    pointer to the parameter of reading table for debugging
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halGBLOrt(VOID *pParam)
{
    UINT32 val[TAB_MAX_WIDTH];
    OPL_STATUS retVal;
    UINT32 index;
    UINT32 cnt;
    HAL_API_GBL_ORT_t *pOrtParam = (HAL_API_GBL_ORT_t *)pParam;
   
    if (NULL == pOrtParam)
    {
        return;
    }
    
    if (0 == pOrtParam->ulFd)
    {
        pOrtParam->ulFd = OP_STDOUT_FILENO;
    }

    for(index = 0;index < pOrtParam->ulRecordNum;index++)
    {
        retVal = oplTabRead(pOrtParam->ulRegion, pOrtParam->ulRecordStart+index, 1,&val[0]);
        if(OK != retVal)
        {
            vosPrintf(pOrtParam->ulFd, "Read table %d, record %d failed.\r\n ",
                (UINT16)pOrtParam->ulRegion,(pOrtParam->ulRecordStart+ index));
            return;
        }
        vosPrintf(pOrtParam->ulFd, "Record ID %d\r\n",(pOrtParam->ulRecordStart+index));
        for(cnt = 0; cnt< opconn_tab_info[pOrtParam->ulRegion].widthOfRec; cnt++)
        {
            if((cnt+1)%4 == 0)
            {
                vosPrintf(pOrtParam->ulFd, "%08X\r\n",val[cnt]);
            }else
            {
                vosPrintf(pOrtParam->ulFd, "%08X ",val[cnt]);
            }
        }
        vosPrintf(pOrtParam->ulFd, "\r\n");
    }
}

/*******************************************************************************
*
* halGBLOwt:	 write table for debugging
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	pParam -    pointer to the parameter of writing table for debugging
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halGBLOwt(VOID *pParam)
{
    UINT32 tmpRecVal[TAB_MAX_WIDTH];
	UINT32 index = OPL_ZERO;
	OPL_STATUS retVal;
	UINT32 startId;
    HAL_API_GBL_OWT_t *pOwtParam = (HAL_API_GBL_OWT_t *)pParam;
   
    if (NULL == pOwtParam)
    {
        return;
    }

	for(index = pOwtParam->ulRecordStart; index < (pOwtParam->ulRecordStart + pOwtParam->ulRecordNum); index++)
	{
		retVal = oplTabRead(pOwtParam->ulRegion,index,1,tmpRecVal);
		if(OK != retVal)
		{
			return;
		}
		tmpRecVal[pOwtParam->ulOffset%TAB_MAX_WIDTH] = pOwtParam->ulValue;

		retVal = oplTabWrite(pOwtParam->ulRegion,index,1,tmpRecVal);
		if(OK != retVal)
		{
			return;
		}
	}
}

/*******************************************************************************
*
* halGBLChipReset:	 chip reset
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	pParam -    pointer to the parameter of chip reset
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halGBLChipReset(VOID *pParam)
{
    chipReset();
}

/*******************************************************************************
*
* halGBLMdioRegRead:	 read mdio register
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	pParam -    pointer to the parameter of reading mdio register
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halGBLMdioRegRead(VOID *pParam)
{
    HAL_API_GBL_MDIO_REG_t *pRegParam = (HAL_API_GBL_MDIO_REG_t *)pParam;

    if (NULL == pRegParam)
    {
        return;
    }

    pRegParam->ulResult = mdioRegisterRead(pRegParam->ucDevAddr,pRegParam->ucRegAddr,&pRegParam->usData);
}

/*******************************************************************************
*
* halGBLMdioRegWrite:	 write mdio register
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	pParam -    pointer to the parameter of writing mdio register
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halGBLMdioRegWrite(VOID *pParam)
{
    HAL_API_GBL_MDIO_REG_t *pRegParam = (HAL_API_GBL_MDIO_REG_t *)pParam;

    if (NULL == pRegParam)
    {
        return;
    }

    pRegParam->ulResult = mdioRegisterWrite(pRegParam->ucDevAddr,pRegParam->ucRegAddr,pRegParam->usData);
}


/*******************************************************************************
*
* halGBLMdioRegFieldRead:	 read mdio register fields
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	pParam -    pointer to the parameter of reading mdio register fields
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halGBLMdioRegFieldRead(VOID *pParam)
{
    HAL_API_GBL_MDIO_REG_FIELDS_t *pRegParam = (HAL_API_GBL_MDIO_REG_FIELDS_t *)pParam;

    if (NULL == pRegParam)
    {
        return;
    }

    pRegParam->ulResult = mdioRegisterFieldRead(pRegParam->ucDevAddr,
        pRegParam->ucRegAddr,
        pRegParam->ucOffset,
        pRegParam->ucWidth,
        &pRegParam->usData);
}

/*******************************************************************************
*
* halGBLMdioRegFieldWrite:	 write mdio register fields
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	pParam -    pointer to the parameter of writing mdio register fields
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halGBLMdioRegFieldWrite(VOID *pParam)
{
    HAL_API_GBL_MDIO_REG_FIELDS_t *pRegParam = (HAL_API_GBL_MDIO_REG_FIELDS_t *)pParam;

    if (NULL == pRegParam)
    {
        return;
    }

    pRegParam->ulResult = mdioRegisterFieldWrite(pRegParam->ucDevAddr,
        pRegParam->ucRegAddr,
        pRegParam->ucOffset,
        pRegParam->ucWidth,
        pRegParam->usData);
}





/*******************************************************************************
*
* halGBLInit:	 to initialize global hal module
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	n/a.
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	0      - initialize successfully.
*   other  - initialize failed.
*
* SEE ALSO: 
*/
OPL_RESULT halGBLInit(void)
{
    OPL_API_UNIT_t stApiUnit;
    OPL_RESULT ulRet;


    /* register global API */
    stApiUnit.apiId = HAL_API_GBL_REG_READ;
    stApiUnit.apiFunc = halGBLRegRead;
    ulRet = halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_REG_WRITE;
    stApiUnit.apiFunc = halGBLRegWrite;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_REG_FIELD_READ;
    stApiUnit.apiFunc = halGBLRegFieldRead;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_REG_FIELD_WRITE;
    stApiUnit.apiFunc = halGBLRegFieldWrite;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_TAB_READ;
    stApiUnit.apiFunc = halGBLTabRead;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_TAB_WRITE;
    stApiUnit.apiFunc = halGBLTabWrite;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_REG_BIT_TEST;
    stApiUnit.apiFunc = halGBLRegBitTest;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_REG_BIT_SET;
    stApiUnit.apiFunc = halGBLRegBitSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_REG_BIT_CLEAR;
    stApiUnit.apiFunc = halGBLRegBitClear;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_REG_BIT_TOGGLE;
    stApiUnit.apiFunc = halGBLRegBitToggle;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_ORR;
    stApiUnit.apiFunc = halGBLOrr;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_OWR;
    stApiUnit.apiFunc = halGBLOwr;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_MRR;
    stApiUnit.apiFunc = halGBLMrr;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_MWR;
    stApiUnit.apiFunc = halGBLMwr;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_OST;
    stApiUnit.apiFunc = halGBLOst;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_ORT;
    stApiUnit.apiFunc = halGBLOrt;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_OWT;
    stApiUnit.apiFunc = halGBLOwt;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_CHIP_RESET;
    stApiUnit.apiFunc = halGBLChipReset;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_MDIO_REG_READ;
    stApiUnit.apiFunc = halGBLMdioRegRead;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_MDIO_REG_WRITE;
    stApiUnit.apiFunc = halGBLMdioRegWrite;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_MDIO_REG_FIELD_READ;
    stApiUnit.apiFunc = halGBLMdioRegFieldRead;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_MDIO_REG_FIELD_WRITE;
    stApiUnit.apiFunc = halGBLMdioRegFieldWrite;
    ulRet += halDrvApiRegister(&stApiUnit);

    return ulRet;

}

/*******************************************************************************
*
* halGBLDestroy:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	0      - initialize successfully.
*   other  - initialize failed.
*
* SEE ALSO: 
*/
OPL_RESULT halGBLDestroy()
{
    OPL_API_UNIT_t stApiUnit;
    OPL_RESULT ulRet;

    stApiUnit.apiFunc = NULL;

    /* deregister global API */
    stApiUnit.apiId = HAL_API_GBL_REG_READ;
    ulRet = halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_REG_WRITE;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_REG_FIELD_READ;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_REG_FIELD_WRITE;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_TAB_READ;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_TAB_WRITE;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_REG_BIT_TEST;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_REG_BIT_SET;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_REG_BIT_CLEAR;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_REG_BIT_TOGGLE;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_ORR;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_OWR;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_MRR;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_MWR;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_OST;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_ORT;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_OWT;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_CHIP_RESET;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_MDIO_REG_READ;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_MDIO_REG_WRITE;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_MDIO_REG_FIELD_READ;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_GBL_MDIO_REG_FIELD_WRITE;
    ulRet += halDrvApiRegister(&stApiUnit);

    return ulRet;
    
}

