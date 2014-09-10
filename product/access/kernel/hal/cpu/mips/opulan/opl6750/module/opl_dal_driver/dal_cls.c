/*
=============================================================================
     File Name: dal_cls.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 zzhu            2007/11/13   Initial Version 
----------------   ------------  ----------------------------------------------
*/
#include <linux/mutex.h>

#include "dal_lib.h"

#define DAL_CLS_LOG_TRACE()  OPL_LOG_TRACE()
#define DAL_CLS_SHOW(x)        if(OPL_DAL_DEBUG_ENABLE) printk(x)

extern CLASSIFY_ADDR_ENTRY_t *gpCaddrTbl;
extern CLASSIFY_RULE_ENTRY_t *gpCruleTbl;
extern CLASSIFY_MASK_ENTRY_t *gpCmaskTbl;
extern CLASSIFY_RULE_MALLOC_t *gpMallocCtl;
extern UINT16 gFreeCnt[CLASSIFY_RULE_TABLE_GROUP];
extern CLASSIFY_ITEM_t gClassifyItem[CLASSIFY_MAX_ITEM_NUM];
extern CLASSIFY_MASKINFO_FOR_RULE_t *gpMaskInfoForRule;

CLS_CTC_TO_DAL_MAP_t clsCtcToDalRuleMapTab[CLASSIFY_MAX_ITEM_NUM - MAX_GROUP_NUM + 1];

static opl_acl_list_t  g_dal_acl_list;
static opl_acl_list_t  g_dal_acl_list;
struct mutex		g_dalClsLock;


#define DAL_ACL_SATE_INVALID 0x0000
#define DAL_ACL_STATE_ACTIVE    0x0001
#define DAL_ACL_STATE_READY  0x0002
#define DAL_ACL_MAX_PRI 8

/*******************************************************************************
*
* dalClsTblEntryAdd - Add the specific rule and mask entry to the table.
*
* DESCRIPTION:
*   This function used t calculate the address, and then mallco the rule 
*   and mask entry and then write mask and rule into the mask and rule table
*   the rule and mask entry is generate by call funtion.
*
* INPUT:
*    pstMaskEntry - Pointer to classification Mask entry.
*    pstRuleEntry - Pointer to classification rule entry.
*
* OUTPUT:
*    pusCruleID   - Pointer to classify rule table entry ID.
*
* RETURNS:
*       OPL_OK,                 - If the operation is successful.
*       OPL_ERR_NULL_POINTER  - If the input pointer is NULL.
*   CLASSIFY_RULE_ITEM_SAME - If a same rule is existed.
*   Others                  - An appropriate specific error code defined
*                     for CLT device driver.
* SEE ALSO: 
*/

OPL_STATUS dalClsTblEntryAdd(CLASSIFY_MASK_ENTRY_t *pMaskEntry,
                             CLASSIFY_RULE_ENTRY_t *pRuleEntry,
                             UINT16 *pCruleID,
                             opl_cls_hwtbl_t *pClshwtbl)
{
  UINT8 maskID;
  UINT16 ruleID;
  UINT16 address;
  UINT16 addrOff;

  OPL_STATUS iStatus;

  if ((OPL_NULL == pMaskEntry) || (OPL_NULL == pRuleEntry)) {
    return OPL_ERR_NULL_POINTER;
  }
  /*Malloc a Mask entry from the mask table, there are maybe two situation:
     1). Malloc a new one.
     2). Referenct the old one. */
     
 if(pMaskEntry->maskPri==0)
 {
  iStatus = clsMaskMalloc(pMaskEntry, &maskID);
  if (OPL_OK != iStatus) {
    return iStatus;
  }
 }
 else
 {
 	maskID=pMaskEntry->maskPri-1;
	gpCmaskTbl[maskID].bfValid = OPL_VALID;
    gpCmaskTbl[maskID].referenceCount++;
 }
  pRuleEntry->bfMsel = 0x01F & maskID;

  /*Calculate address*/
  iStatus = clsAddrCal(pRuleEntry, &address);
  if (OPL_OK != iStatus) {
    return iStatus;
  }

  iStatus = clsRuleIDSel(pRuleEntry, address, &addrOff, &ruleID);
  if (OPL_OK == iStatus) {
    /*There a same rule in the rule table, so the current can't be written 
      into the table. Free the mask which has been malloced for this rule.*/
    clsMaskFree(maskID);
    return CLASSIFY_RULE_ITEM_SAME;
  }
  else if (CLASSIFY_RULE_NULL_ITEM != iStatus) {
    clsMaskFree(maskID);
    return iStatus;
  }

  /*Malloc a new entry from the rule table for the new rule.*/
  iStatus = clsRuleMalloc(address, &ruleID, &addrOff);
  if (OPL_OK != iStatus) {
    clsMaskFree(maskID);
    return iStatus;
  }

  *pCruleID = ruleID;
  /*Write the new entry into the table, there two table need to be
    wirtten: local table in the system SDRAM and hardware table in 
    the ASIC.*/
    
  iStatus = dalClsRuleTblWr(pRuleEntry, ruleID);
  if (OPL_OK != iStatus) {
    clsRuleFree(ruleID);
    clsMaskFree(maskID);
    return iStatus;
  }

  pMaskEntry->bfValid = OPL_VALID;
  iStatus = dalClsMaskTblWr(pMaskEntry, maskID);
  if (OPL_OK != iStatus) {
    clsRuleFree(ruleID);
    clsMaskFree(maskID);
    return iStatus;
  }
  OPL_DAL_PRINTF(("mask %d address = %d addrOff = %d ruleID = %d\n",maskID,address, addrOff, ruleID));
  iStatus = dalClsAddrTblWr(address, addrOff, ruleID);
  if (OPL_OK != iStatus) {
    clsRuleFree(ruleID);
    clsMaskFree(maskID);
    return iStatus;
  }
  pClshwtbl->clsHwMaskID= maskID;
  pClshwtbl->clsHwRuleID=ruleID;
  pClshwtbl->clsHwAddr=address;
  pClshwtbl->clsHwOff=addrOff;
  
  return OPL_OK;
}


/*******************************************************************************
*
* dalClsTblEntryDel - delete the specific rule and mask entry from the table.
*
* DESCRIPTION:
*       this function used t calculate the address, and then locate at the rule 
*       and mask entry and then delete mask and rule into the mask and rule table
*       the rule and mask entry is generate by call funtion.
*
* INPUT:
*       pstMaskEntry - classification Mask entry.
*       pstRuleEntry - classification rule entry.
*
* OUTPUT:
*        pusCruleID - classify rule table entry ID.
*
* RETURNS:
*       OPL_OK          - If the operation is successful.
*       OPL_ERR_NULL_POINTER  - If the input pointer is NULL.
*   Others                  - An appropriate specific error code defined
*                     for CLT device driver.
* SEE ALSO: 
*/
OPL_STATUS dalClsTblEntryDel(CLASSIFY_MASK_ENTRY_t *pMaskEntry,
                             CLASSIFY_RULE_ENTRY_t *pRuleEntry,
                             UINT16 *pCruleID)
{
  UINT8 maskID;
  UINT16 ruleID;
  UINT16 address;
  UINT16 addrOff;
  OPL_STATUS iStatus;

#ifdef DAL_CLS_DEBUG
  UINT32 addr1 = (UINT32) pMaskEntry;
  UINT32 addr2 = (UINT32) pRuleEntry;
#endif

#ifdef DAL_CLS_DEBUG
  OPL_DAL_PRINTF(("pMaskEntry addr= 0x%x\n", addr1));
  OPL_DAL_PRINTF(("pRuleEntry addr= 0x%x\n", addr2));
#endif  
  if ((NULL == pMaskEntry) || (NULL == pRuleEntry)) {
    return OPL_ERR_NULL_POINTER;
  }
  /*Get the same mask item in the classification mask table, it maybe made
    in early configuration process.*/
  iStatus = clsMaskSel(pMaskEntry, &maskID);
  if (OPL_OK != iStatus) {
  	
    return iStatus;
  }

  /*Calculate the hash address which used to index classification address table.*/
  iStatus = clsAddrCal(pRuleEntry, &address);
  if (OPL_OK != iStatus) {
  	
    return iStatus;
  }

  /*Check whether there are already same classification rule.*/
  pRuleEntry->bfMsel = maskID;
  iStatus = clsRuleIDSel(pRuleEntry, address, &addrOff, &ruleID);
  if (OPL_OK != iStatus) {
  	
    return iStatus;
  }

  iStatus = clsMaskFree(maskID);
  if (OPL_OK != iStatus) {
  	
    return iStatus;
  }

  iStatus = dalClsAddrTblWr(address, addrOff, 0);
  clsRuleFree(ruleID);
  *pCruleID = ruleID;

  return iStatus;
}

OPL_STATUS dalClsTblEntryRemove(opl_cls_hwtbl_t *clstblshadow)
{
	OPL_STATUS iStatus;
	UINT16 hwmaskID;
	UINT16 hwruleID;
	UINT16 address;
	UINT16 addrOff;
	
	if(clstblshadow==NULL)
	{
		return OPL_ERR_NULL_POINTER;		
	}
	 hwmaskID=clstblshadow->clsHwMaskID;
	 hwruleID=clstblshadow->clsHwRuleID;
	 address=clstblshadow->clsHwAddr;
	 addrOff=clstblshadow->clsHwOff;
	OPL_DAL_PRINTF(("delete mask %d rule %d  addr %d off %d \n",hwmaskID,hwruleID,address,addrOff));

	OPL_MEMSET(&(gpCmaskTbl[hwmaskID]),0, sizeof(CLASSIFY_MASK_ENTRY_t));
	iStatus = dalClsMaskHwWr(&(gpCmaskTbl[hwmaskID]), hwmaskID);
		
	OPL_MEMSET(&(gpCruleTbl[hwruleID]), 0,sizeof(CLASSIFY_RULE_ENTRY_t));
	dalClsRuleHwWr(&(gpCruleTbl[hwruleID]), hwruleID);
	
	OPL_MEMSET(&(gpCaddrTbl[address].ClasAddr[addrOff]), 0,sizeof(UINT16));
	iStatus = dalClsAddrHwWr(address, addrOff, 0);
	clsRuleFree(hwruleID);
	return iStatus;
}

OPL_STATUS dalClsTblEntryChange(UINT16 hwoldmaskID,UINT16 hwoldruleID,UINT16 hwnewmaskID)
{
	OPL_STATUS iStatus;
	UINT16 ruleID=hwoldruleID;


	OPL_DAL_PRINTF(("change old mask %d to new mask %d  rule %d \n",hwoldmaskID,hwnewmaskID,hwoldruleID));

  OPL_MEMCPY(&(gpCmaskTbl[hwnewmaskID]), &(gpCmaskTbl[hwoldmaskID]), sizeof(CLASSIFY_MASK_ENTRY_t));
  /*Clear both local and ASIC mask entry to 0.*/
  OPL_MEMSET(&(gpCmaskTbl[hwoldmaskID]),0, sizeof(CLASSIFY_MASK_ENTRY_t));
  dalClsMaskHwWr(&(gpCmaskTbl[hwoldmaskID]), hwoldmaskID);
  dalClsMaskHwWr(&(gpCmaskTbl[hwnewmaskID]), hwnewmaskID);

  gpCruleTbl[ruleID].bfMsel = hwnewmaskID;
  iStatus=dalClsRuleHwWr(&(gpCruleTbl[ruleID]), ruleID);
  return iStatus;
}

/*******************************************************************************
*
* dalClsMaskTblWr - wirte mask entry to the table.
*
* DESCRIPTION:
*       wirte mask entry to the table. the mask entry id is specific by call 
*       function, this funciton will write both local and AISC mask table..
*
* INPUT:
*       pMask - classification Mask entry.
*       maskID     - mask entry ID.
*
* OUTPUT:
*       NULL.
*
* RETURNS:
*       OPL_OK          - If the operation is successful.
*       OPL_ERR_NULL_POINTER  - If the input pointer is NULL.
*
* SEE ALSO: 
*/

OPL_STATUS dalClsMaskTblWr(CLASSIFY_MASK_ENTRY_t *pMask,UINT8 maskID)
{
  if (OPL_NULL == pMask) {
    return OPL_ERR_NULL_POINTER;
  }
  gpCmaskTbl[maskID].bfOffset1 = pMask->bfOffset1;
  gpCmaskTbl[maskID].bfOffset2 = pMask->bfOffset2;
  gpCmaskTbl[maskID].bfOffset3 = pMask->bfOffset3;
  gpCmaskTbl[maskID].bfOffset4 = pMask->bfOffset4;
  gpCmaskTbl[maskID].bfOffset5 = pMask->bfOffset5;
  gpCmaskTbl[maskID].bfOffset6 = pMask->bfOffset6;

  gpCmaskTbl[maskID].bfValid = pMask->bfValid;
  gpCmaskTbl[maskID].maskPri = pMask->maskPri;
  gpCmaskTbl[maskID].bfEmask = pMask->bfEmask;
  gpCmaskTbl[maskID].bfImask = pMask->bfImask;
#if 0  
  printk("%s %d 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x maskId %d  \n",__FUNCTION__,__LINE__,pMask->bfEmask,pMask->bfImask,
								  pMask->bfOffset6,
								  pMask->bfOffset5,
								  pMask->bfOffset4,
								  pMask->bfOffset3,
								  pMask->bfOffset2,
								  pMask->bfOffset1,
								  maskID);
#endif
  OPL_MEMCPY(gpCmaskTbl[maskID].Mask, pMask->Mask, CLASSIFY_KEYVALUE_LENGTH);

  return dalClsMaskHwWr(pMask, maskID);
}



/*******************************************************************************
*
* dalClsRuleTblWr - wirte rule entry to the table.
*
* DESCRIPTION:
*       wirte rule entry to the table. the rule entry id is specific by call 
*       function, this funciton will write both local and AISC rule table.
*
* INPUT:
*       ruleID     - classification rule entry ID.
*       pstRuleEntry - classification rule entry.
*
* OUTPUT:
*       NULL.
*
* RETURNS:
*       OPL_OK          - If the operation is successful.
*       OPL_ERR_NULL_POINTER  - If the input pointer is NULL.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsRuleTblWr(CLASSIFY_RULE_ENTRY_t *pRule,UINT16 ruleID)
{
  CLASSIFY_RULE_ENTRY_t *pDalClsRuleEntry;
  if (OPL_NULL == pRule) {
    return OPL_ERR_NULL_POINTER;
  }

  gpCruleTbl[ruleID].bfTranActH = pRule->bfTranActH;
  gpCruleTbl[ruleID].bfTranActL = pRule->bfTranActL;
  gpCruleTbl[ruleID].bfEgress = pRule->bfEgress;
  gpCruleTbl[ruleID].bfIngress = pRule->bfIngress;
  gpCruleTbl[ruleID].bfRmkAct = pRule->bfRmkAct;
  gpCruleTbl[ruleID].bfRmkCos = pRule->bfRmkCos;
  gpCruleTbl[ruleID].bfRmkIcos = pRule->bfRmkIcos;
  gpCruleTbl[ruleID].bfRmkVid = pRule->bfRmkVid;
  gpCruleTbl[ruleID].bfMsel = pRule->bfMsel;

  OPL_MEMCPY(gpCruleTbl[ruleID].Rule, pRule->Rule, CLASSIFY_KEYVALUE_LENGTH);

  pDalClsRuleEntry = &gpCruleTbl[ruleID];
  //OPL_DAL_PRINTF(("Local:\n"));
  /*
  OPL_DAL_PRINTF(("%-3d %-4x %-6x %-7x %-6x %-6x %-7x %-4d %-6x\n",
  ruleID,
  pDalClsRuleEntry->bfTranActH<<3|pDalClsRuleEntry->bfTranActL,
  pDalClsRuleEntry->bfEgress,
  pDalClsRuleEntry->bfIngress,
  pDalClsRuleEntry->bfRmkAct,
  pDalClsRuleEntry->bfRmkCos,
  pDalClsRuleEntry->bfRmkIcos,
  pDalClsRuleEntry->bfRmkVid,
  pDalClsRuleEntry->bfMsel));
  OPL_DAL_PRINTF(("%08x %08x %08x %08x %08x %08x\n",
  *((UINT32 *)&(pDalClsRuleEntry->Rule[0])),
  *((UINT32 *)&(pDalClsRuleEntry->Rule[4])),
  *((UINT32 *)&(pDalClsRuleEntry->Rule[8])),
  *((UINT32 *)&(pDalClsRuleEntry->Rule[12])),
  *((UINT32 *)&(pDalClsRuleEntry->Rule[16])),
  *((UINT32 *)&(pDalClsRuleEntry->Rule[20]))));
  */
  return dalClsRuleHwWr(pRule, ruleID);
}


/*******************************************************************************
*
* dalClsAddrTblWr   - Wirte classify rule ID to the classify address table.
*
* DESCRIPTION:
*       This function wirte classify rule ID to the classify address table. 
*     The rule entry id is specific by call function, this funciton will 
*     write both local and AISC rule table.
*
* INPUT:
*    ruleID   - Classification rule entry ID.
*    address    -   Entry number in classify hash table.
*        offset     -   Indicates ADDR0~7 of current hash entry. 
*
* OUTPUT:
*       NULL.
*
* RETURNS:
*       OPL_OK  - If the operation is successful.
*   Others  - An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsAddrTblWr(UINT16 address,UINT16 offset,UINT16 ruleId)
{
  gpCaddrTbl[address].ClasAddr[offset] = ruleId;
  return dalClsAddrHwWr(address, offset, ruleId);
}



/*******************************************************************************
*
* dalClsMaskHwWr  - Write classify mask to hardware table.
*
* DESCRIPTION:
*    Write classify mask to hardware table.
*
* INPUT:
*       pMask   - Classification mask entry ID.
*       maskID  - Classification mask entry.
*
* OUTPUT:
*       NULL.
*
* RETURNS:
*       OPL_OK              - If the operation is successful.
*       PARAMETER_INPUT_POINTER_NULL  - If the input pointer is NULL.
*   Others                      -   An appropriate specific error code defined
*                     for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsMaskHwWr(CLASSIFY_MASK_ENTRY_t *pMask,UINT8 maskID)
{
  OPL_STATUS iStatus;
  CLASSIFY_MASK_TBL_t MaskTblEntry;

  UINT32 *pTest;

#ifndef OPCONN_BIG_ENDIAN
  UINT8 byteCnt;
  UINT8 wordCnt;
  UINT8 srcOff;
  UINT8 destOff;
#endif

  OPL_BZERO(&MaskTblEntry, sizeof(CLASSIFY_MASK_TBL_t));
  MaskTblEntry.bfEmask = pMask->bfEmask;
  MaskTblEntry.bfImask = pMask->bfImask;
  MaskTblEntry.bfOffset1 = pMask->bfOffset1;
  MaskTblEntry.bfOffset2 = pMask->bfOffset2;
  MaskTblEntry.bfOffset3 = pMask->bfOffset3;
  MaskTblEntry.bfOffset4 = pMask->bfOffset4;
  MaskTblEntry.bfOffset5 = pMask->bfOffset5;
  MaskTblEntry.bfOffset6 = pMask->bfOffset6;

  MaskTblEntry.bfValid = pMask->bfValid;

#ifdef OPCONN_BIG_ENDIAN
  OPL_MEMCPY(MaskTblEntry.Mask, pMask->Mask, CLASSIFY_KEYVALUE_LENGTH);

#else
  for (wordCnt = 0; wordCnt < CLASSIFY_KEYVALUE_LENGTH / 4; wordCnt++) {
    for (byteCnt = 0; byteCnt < 4; byteCnt++) {
      srcOff = wordCnt * 4 + (3 - byteCnt);
      destOff = wordCnt * 4 + byteCnt;
      MaskTblEntry.Mask[destOff] = pMask->Mask[srcOff];
    }
  }
#endif

  pTest = (UINT32 *) (&MaskTblEntry);
  /*%%%%%%%TBC%%%*/
  /*Only 10 bytes in mask table entry?*/
  OPL_LOG(&gbClsDbg,
          &gbClsFileDbg,
          "Classify WRITE RULE table MaskId = %d \r\n ",
          maskID);

  OPL_LOG(&gbClsDbg,
          &gbClsFileDbg,
          " 0x%x 0x%x 0x%x 0x%x 0x%x \r\n ",
          pTest[0],
          pTest[1],
          pTest[2],
          pTest[3],
          pTest[4]);
  OPL_LOG(&gbClsDbg,
          &gbClsFileDbg,
          " 0x%x 0x%x 0x%x 0x%x 0x%x \r\n ",
          pTest[5],
          pTest[6],
          pTest[7],
          pTest[8],
          pTest[9]);
  /*    OPL_LOG(&gbClsDbg, &gbClsFileDbg, " 0x%x 0x%x 0x%x 0x%x 0x%x \r\n ",
            pTest[10] ,pTest[11], pTest[12], pTest[13], pTest[14]);
      OPL_LOG(&gbClsDbg, &gbClsFileDbg, " 0x%x 0x%x 0x%x 0x%x 0x%x \r\n ",
            pTest[15] ,pTest[16], pTest[17]);*/

  iStatus = oplTabWrite(OPCONN_TAB_CLS_MASK,
                        maskID,
                        1,
                        (UINT32 *) (&(MaskTblEntry)));

  return OPL_OK;
}


/*******************************************************************************
*
* dalClsRuleHwWr  - Write classify rule to hardware table.
*
* DESCRIPTION:
*    Write classify rule to hardware table.
*
* INPUT:
*       pstRule   - classification rule entry ID.
*       usRuleID    - classification rule entry.
*
* OUTPUT:
*       NULL.
*
* RETURNS:
*       OPL_OK              - If the operation is successful.
*       PARAMETER_INPUT_POINTER_NULL  - If the input pointer is NULL.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsRuleHwWr(CLASSIFY_RULE_ENTRY_t *pRule,UINT16 ruleID)
{
  OPL_STATUS iStatus;
  CLASSIFY_RULE_TBL_t RuleTblEntry;

  UINT32 *pTest;

#ifndef OPCONN_BIG_ENDIAN
  UINT8 byteCnt;
  UINT8 wordCnt;
  UINT8 srcOff;
  UINT8 destOff;
#endif

  OPL_BZERO(&RuleTblEntry, sizeof(CLASSIFY_RULE_TBL_t));

  RuleTblEntry.bfTranActH = pRule->bfTranActH;
  RuleTblEntry.bfTranActL = pRule->bfTranActL;
  RuleTblEntry.bfEgress = pRule->bfEgress;
  RuleTblEntry.bfIngress = pRule->bfIngress;
  RuleTblEntry.bfRmkAct = pRule->bfRmkAct;
  RuleTblEntry.bfRmkCos = pRule->bfRmkCos;
  RuleTblEntry.bfRmkIcos = pRule->bfRmkIcos;
  RuleTblEntry.bfRmkVid = pRule->bfRmkVid;
  RuleTblEntry.bfMsel = pRule->bfMsel;

#ifdef OPCONN_BIG_ENDIAN  
  OPL_MEMCPY(RuleTblEntry.Rule, pRule->Rule, CLASSIFY_KEYVALUE_LENGTH);
#else
  for (wordCnt = 0; wordCnt < CLASSIFY_KEYVALUE_LENGTH / 4; wordCnt++) {
    for (byteCnt = 0; byteCnt < 4; byteCnt++) {
      srcOff = wordCnt * 4 + (3 - byteCnt);
      destOff = wordCnt * 4 + byteCnt;
      RuleTblEntry.Rule[destOff] = pRule->Rule[srcOff];
    }
  }
#endif
  pTest = (UINT32 *) (&RuleTblEntry);
  /*%%%%%%%TBC%%%*/
  /*Only 12 bytes in classify rule table entry*/
  OPL_LOG(&gbClsDbg,
          &gbClsFileDbg,
          "Classify WRITE RULE table MaskId = %d \r\n ",
          ruleID);
  OPL_LOG(&gbClsDbg,
          &gbClsFileDbg,
          " 0x%x 0x%x 0x%x 0x%x 0x%x \r\n ",
          pTest[0],
          pTest[1],
          pTest[2],
          pTest[3],
          pTest[4]);
  OPL_LOG(&gbClsDbg,
          &gbClsFileDbg,
          " 0x%x 0x%x 0x%x 0x%x 0x%x \r\n ",
          pTest[5],
          pTest[6],
          pTest[7],
          pTest[8],
          pTest[9]);
  OPL_LOG(&gbClsDbg,
          &gbClsFileDbg,
          " 0x%x 0x%x 0x%x 0x%x 0x%x \r\n ",
          pTest[10],
          pTest[11],
          pTest[12],
          pTest[13],
          pTest[14]);
  /*    OPL_LOG(&gbClsDbg, &gbClsFileDbg, " 0x%x 0x%x 0x%x 0x%x 0x%x \r\n ",
             pTest[15] ,pTest[16], pTest[17]));*/

  iStatus = oplTabWrite(OPCONN_TAB_CLS_RULE,
                        ruleID,
                        1,
                        (UINT32 *) (&(RuleTblEntry)));

  return OPL_OK;
}


/*******************************************************************************
*
* dalClsAddrHwWr  - Write classify address to hardware table.
*
* DESCRIPTION:
*    Write classify address to hardware table.
*
* INPUT:
*    usAddress  - Classify address table entry.
*    usOffset - Classify address table offset.
*    usRuleId - Classify rule ID.
*
* OUTPUT:
*       NULL.
*
* RETURNS:
*       OPL_OK              - If the operation is successful.
*       PARAMETER_INPUT_POINTER_NULL  - If the input pointer is NULL.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsAddrHwWr(UINT16 address,UINT16 offset,UINT16 ruleId)
{
  OPL_STATUS iStatus;

  UINT32 *pTest;

  CLASSIFY_ADDR_TBL_t ClassifyAddr;

  iStatus = oplTabRead(OPCONN_TAB_CLS_ADDR,
                       address,
                       1,
                       (UINT32 *) (&ClassifyAddr));
  if (OPL_OK != iStatus) {
    return iStatus;
  }

  pTest = (UINT32 *) (&(ClassifyAddr));

  OPL_LOG(&gbClsDbg,
          &gbClsFileDbg,
          "Classify WRITE  Address = %d off = %d Rule = %d\r\n",
          address,
          offset,
          ruleId);
  OPL_LOG(&gbClsDbg,
          &gbClsFileDbg,
          "Classify WRITE before map address 0x%x   0x%x\r\n",
          pTest[0],
          pTest[1]);

  iStatus = clsAddrMapToTbl(offset, ruleId, &ClassifyAddr);
  if (OPL_OK != iStatus) {
    return iStatus;
  }

  OPL_LOG(&gbClsDbg,
          &gbClsFileDbg,
          "Classify WRITE after map address 0x%x   0x%x\r\n",
          pTest[0],
          pTest[1]);

  iStatus = oplTabWrite(OPCONN_TAB_CLS_ADDR,
                        address,
                        1,
                        (UINT32 *) (&(ClassifyAddr)));
  if (OPL_OK != iStatus) {
    return iStatus;
  }

  return OPL_OK;
}



/*******************************************************************************
*
* dalClsRuleTblRd - read a rule entry from the rule table.
*
* DESCRIPTION:
*       read a rule entry from the rule table.
*
* INPUT:
*       ruleID  - classification Rule entry ID.
*       tblType - CLASSIFY_LOCAL_TBL               1
*                   CLASSIFY_HW_TBL                  2
*
* OUTPUT:
*        pRule  - classify rule entry.
*
* RETURNS:
*       OPL_OK            - If the operation is successful.
*       OPL_ERR_NULL_POINTER  - If the input pointer is NULL.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsRuleTblRd(CLASSIFY_RULE_ENTRY_t *pRule,
                           UINT16 ruleID,
                           UINT8 tblType)
{
  OPL_STATUS iStatus;
  CLASSIFY_RULE_TBL_t RuleTblEntry;

  UINT32 *pulTest;
#ifndef OPCONN_BIG_ENDIAN
  UINT8 byteCnt;
  UINT8 wordCnt;
  UINT8 srcOff;
  UINT8 destOff;
#endif

  if (OPL_NULL == pRule) {
    return OPL_ERR_NULL_POINTER;
  }

  if (CLASSIFY_LOCAL_TBL == tblType) {
    pRule->bfTranActH = gpCruleTbl[ruleID].bfTranActH;
    pRule->bfTranActL = gpCruleTbl[ruleID].bfTranActL;
    pRule->bfEgress = gpCruleTbl[ruleID].bfEgress;
    pRule->bfIngress = gpCruleTbl[ruleID].bfIngress;
    pRule->bfRmkAct = gpCruleTbl[ruleID].bfRmkAct;
    pRule->bfRmkCos = gpCruleTbl[ruleID].bfRmkCos;
    pRule->bfRmkIcos = gpCruleTbl[ruleID].bfRmkIcos;
    pRule->bfRmkVid = gpCruleTbl[ruleID].bfRmkVid;
    pRule->bfMsel = gpCruleTbl[ruleID].bfMsel;

    OPL_MEMCPY(pRule->Rule, gpCruleTbl[ruleID].Rule, CLASSIFY_KEYVALUE_LENGTH);
  }
  else {
    iStatus = oplTabRead(OPCONN_TAB_CLS_RULE,
                         ruleID,
                         1,
                         (UINT32 *) (&(RuleTblEntry)));
    if (OPL_OK != iStatus) {
      return iStatus;
    }

    pulTest = (UINT32 *) (&RuleTblEntry);

    OPL_LOG(&gbClsDbg,
            &gbClsFileDbg,
            "Classify READ RULE table MaskId = %d \r\n ",
            ruleID);
    OPL_LOG(&gbClsDbg,
            &gbClsFileDbg,
            " 0x%x 0x%x 0x%x 0x%x 0x%x \r\n ",
            pulTest[0],
            pulTest[1],
            pulTest[2],
            pulTest[3],
            pulTest[4]);
    OPL_LOG(&gbClsDbg,
            &gbClsFileDbg,
            " 0x%x 0x%x 0x%x 0x%x 0x%x \r\n ",
            pulTest[5],
            pulTest[6],
            pulTest[7],
            pulTest[8],
            pulTest[9]);
    OPL_LOG(&gbClsDbg,
            &gbClsFileDbg,
            " 0x%x 0x%x 0x%x 0x%x 0x%x \r\n ",
            pulTest[10],
            pulTest[11],
            pulTest[12],
            pulTest[13],
            pulTest[14]);
    /*
            OPL_LOG(&gbClsDbg, &gbClsFileDbg, " 0x%x 0x%x 0x%x 0x%x 0x%x \r\n ",
                  pulTest[15] ,pulTest[16], pulTest[17]);
    */        

    pRule->bfTranActH = RuleTblEntry.bfTranActH;
    pRule->bfTranActL = RuleTblEntry.bfTranActL;
    pRule->bfEgress = RuleTblEntry.bfEgress;
    pRule->bfIngress = RuleTblEntry.bfIngress;
    pRule->bfRmkAct = RuleTblEntry.bfRmkAct;
    pRule->bfRmkCos = RuleTblEntry.bfRmkCos;
    pRule->bfRmkIcos = RuleTblEntry.bfRmkIcos;
    pRule->bfRmkVid = RuleTblEntry.bfRmkVid;
    pRule->bfMsel = RuleTblEntry.bfMsel;

#ifdef OPCONN_BIG_ENDIAN
    OPL_MEMCPY(pRule->Rule, RuleTblEntry.Rule, CLASSIFY_KEYVALUE_LENGTH);
#else
    for (wordCnt = 0; wordCnt < CLASSIFY_KEYVALUE_LENGTH / 4; wordCnt++) {
      for (byteCnt = 0; byteCnt < 4; byteCnt++) {
        srcOff = wordCnt * 4 + (3 - byteCnt);
        destOff = wordCnt * 4 + byteCnt;
        pRule->Rule[destOff] = RuleTblEntry.Rule[srcOff];
      }
    }
#endif
  }

  return OPL_OK;
}


/*******************************************************************************
*
* dalClsAddrTblRd   - Read classify rule ID from table.
*
* DESCRIPTION:
*       This function read classify rule ID from table.
*
* INPUT:
*    usAddress    - Index of entry of classify address table.
*    usOffset   - Offset of classify address table.
*    ucTblType    - Table type, local table or hardware table.
*
* OUTPUT:
*    pusRuleId        - Pointer to classify rule ID.
*        OPL_ERR_NULL_POINTER   - Input pointer is null.
*
* RETURNS:
*    OPL_OK     - If the operation is successful.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsAddrTblRd(UINT16 address,
                           UINT16 offset,
                           UINT16 *pRuleId,
                           UINT8 tblType)
{
  OPL_STATUS iStatus;

  CLASSIFY_ADDR_TBL_t ClassifyAddr;
  UINT32 *pTest;

  if (OPL_NULL == pRuleId) {
    return OPL_ERR_NULL_POINTER;
  }

  if (CLASSIFY_LOCAL_TBL == tblType) {
    *pRuleId = gpCaddrTbl[address].ClasAddr[offset];
  }
  else {
    iStatus = oplTabRead(OPCONN_TAB_CLS_ADDR,
                         address,
                         1,
                         (UINT32 *) (&(ClassifyAddr)));
    if (OPL_OK != iStatus) {
      return iStatus;
    }

    pTest = (UINT32 *) (&ClassifyAddr);
    OPL_LOG(&gbClsDbg,
            &gbClsFileDbg,
            "Classify READ  Address = %d off = %d \r\n",
            address,
            offset);
    OPL_LOG(&gbClsDbg,
            &gbClsFileDbg,
            "Classify READ before map address 0x%x   0x%x\r\n",
            pTest[0],
            pTest[1]);

    iStatus = clsTblMapToAddr(offset, pRuleId, &ClassifyAddr);
    if (OPL_OK != iStatus) {
      return iStatus;
    }
    OPL_LOG(&gbClsDbg,
            &gbClsFileDbg,
            "Classify READ after map address 0x%x   0x%x\r\n",
            pTest[0],
            pTest[1]);
  }
  return OPL_OK;
}




/*******************************************************************************
*
* dalClsEntrySet - configure rule and mask into classify rule and classify mask
*                    table
*
* DESCRIPTION:
*       this function used to generate classify mask entry 
*       accordanve the input parameter, and then malloc the rule and mask entry
*       calculate the address and then write mask and rule into the mask and rule
*       tables
*
* INPUT:
*        pRulePar - classification and flow parameter.
*
* OUTPUT:
*        pusCruleID - classify rule table entry ID.
*
* RETURNS:
*       OPL_OK            - If the operation is successful.
*       OPL_ERR_NULL_POINTER    - If the input pointer is NULL.
*   CLASSIFY_INVALID_PKT_TYPE - If the packet type is valid.
*   Others                    - An appropriate specific error code defined
*                     for CLT device driver.
* SEE ALSO: 
*/
OPL_STATUS dalClsEntrySet(CLASSIFY_PARAMETER_t *pMaskRulePar,UINT16 *pCruleID,opl_cls_hwtbl_t *pClshwID)
{
  OPL_STATUS iStatus = OPL_OK;

  CLASSIFY_MASK_ENTRY_t ClassMentry;
  CLASSIFY_RULE_ENTRY_t ClassRentry;

  OPL_BZERO(&ClassMentry, sizeof(CLASSIFY_MASK_ENTRY_t));
  OPL_BZERO(&ClassRentry, sizeof(CLASSIFY_RULE_ENTRY_t));

  if ((OPL_NULL == pMaskRulePar) || (OPL_NULL == pCruleID)) {
    return OPL_ERR_NULL_POINTER;
  }

  /*Set rule and mask*/
  switch (pMaskRulePar->pktType) {
    case CLASSIFY_EII_IP:
      //iStatus = clsEIIMaskRuleMake(pMaskRulePar ,&ClassMentry ,&ClassRentry);
      iStatus = clsMaskRuleMake2ONU(pMaskRulePar, &ClassMentry, &ClassRentry);
      if (OPL_OK != iStatus) {
        return iStatus;
      }
      break;
    case CLASSIFY_PPPOE_IP:
      iStatus = clsPPPoeMaskRuleMake(pMaskRulePar, &ClassMentry, &ClassRentry);
      if (OPL_OK != iStatus) {
        return iStatus;
      }

      break;
    default:
			{
      return CLASSIFY_INVALID_PKT_TYPE;
  }
  }

  /*Set action field*/
  /*  ClassRentry = pRulePar->RuleEntry;*/

  ClassRentry.bfTranActH = pMaskRulePar->RuleEntry.bfTranActH;
  ClassRentry.bfTranActL = pMaskRulePar->RuleEntry.bfTranActL;  
  ClassRentry.bfRmkAct = pMaskRulePar->RuleEntry.bfRmkAct;
  ClassRentry.bfRmkCos = pMaskRulePar->RuleEntry.bfRmkCos;
  ClassRentry.bfRmkIcos = pMaskRulePar->RuleEntry.bfRmkIcos;
  ClassRentry.bfRmkVid = pMaskRulePar->RuleEntry.bfRmkVid;
  ClassRentry.bfMsel = pMaskRulePar->RuleEntry.bfMsel;

  ClassRentry.bfEgress = ((pMaskRulePar->RuleEntry.bfEgress) & (pMaskRulePar->egressMask));
  ClassRentry.bfIngress = ((pMaskRulePar->RuleEntry.bfIngress) & (pMaskRulePar->ingressMask));

  ClassMentry.maskPri = pMaskRulePar->maskPri;
  ClassMentry.bfEmask = pMaskRulePar->egressMask;
  ClassMentry.bfImask = pMaskRulePar->ingressMask;


  /*Write classifty rulte table*/
  iStatus = dalClsTblEntryAdd(&ClassMentry, &ClassRentry, pCruleID,pClshwID); 
  if (OPL_OK != iStatus) {
    return iStatus;
  }
  
  return OPL_OK;
}


/*******************************************************************************
*
* dalClsEntryMove - remove the rule and mask entry from rule and mask table
*
* DESCRIPTION:
*       this function used to generate classify mask entry and flow mask entry 
*       accordanve the input parameter, calculate the address, and then locate 
*       at the rule and mask entry and then remove mask and rule into the mask
*       and rule table
*
* INPUT:
*        pRulePar - classification and flow parameter.
*
* OUTPUT:
*        pusCruleID - classify rule table entry ID.
*
* RETURNS:
*       OPL_OK            - If the operation is successful.
*       OPL_ERR_NULL_POINTER    - If the input pointer is NULL.
*   CLASSIFY_INVALID_PKT_TYPE - If the packet type is valid.
*   Others                    - An appropriate specific error code defined
*                     for CLT device driver.
* SEE ALSO: 
*/
OPL_STATUS dalClsEntryMove(CLASSIFY_PARAMETER_t *pRulePar,UINT16 *pCruleID)
{
  CLASSIFY_MASK_ENTRY_t ClassMentry;
  CLASSIFY_RULE_ENTRY_t ClassRentry;

  OPL_STATUS iStatus = OPL_OK;

  /********************/
  UINT32 addr1 = 0;
  UINT32 addr2 = 0;
  /********************/

  if ((OPL_NULL == pRulePar) || (OPL_NULL == pCruleID)) {
    return OPL_ERR_NULL_POINTER;
  }

  OPL_BZERO(&ClassMentry, sizeof(CLASSIFY_MASK_ENTRY_t));
  OPL_BZERO(&ClassRentry, sizeof(CLASSIFY_RULE_ENTRY_t));

  /*Set rule and mask*/
  switch (pRulePar->pktType) {
    case CLASSIFY_EII_IP:
      //iStatus = clsEIIMaskRuleMake(pRulePar, &ClassMentry, &ClassRentry);
      iStatus = clsMaskRuleMake2ONU(pRulePar, &ClassMentry, &ClassRentry);
      if (OPL_OK != iStatus) {
        return iStatus;
      }
      break;
    case CLASSIFY_PPPOE_IP:
      iStatus = clsPPPoeMaskRuleMake(pRulePar, &ClassMentry, &ClassRentry);
      if (OPL_OK != iStatus) {
        return iStatus;
      }           
      break;
    default:
      return CLASSIFY_INVALID_PKT_TYPE;
  }

  /*Set action field*/
  /*ClassRentry = pRulePar->RuleEntry;*/

  ClassRentry.bfTranActH = pRulePar->RuleEntry.bfTranActH;
  ClassRentry.bfTranActL = pRulePar->RuleEntry.bfTranActL;
  ClassRentry.bfRmkAct = pRulePar->RuleEntry.bfRmkAct;
  ClassRentry.bfRmkCos = pRulePar->RuleEntry.bfRmkCos;
  ClassRentry.bfRmkIcos = pRulePar->RuleEntry.bfRmkIcos;
  ClassRentry.bfRmkVid = pRulePar->RuleEntry.bfRmkVid;
  ClassRentry.bfMsel = pRulePar->RuleEntry.bfMsel;

  ClassRentry.bfIngress = ((pRulePar->RuleEntry.bfIngress) & (pRulePar->ingressMask));
  ClassRentry.bfEgress = ((pRulePar->RuleEntry.bfEgress) & (pRulePar->egressMask));

  ClassMentry.maskPri = pRulePar->maskPri;
  ClassMentry.bfEmask = pRulePar->egressMask;
  ClassMentry.bfImask = pRulePar->ingressMask;
  /********************/    
  addr1 = (UINT32) (&ClassMentry);
  addr2 = (UINT32) (&ClassRentry);
#ifdef DAL_CLS_DEBUG  
  OPL_DAL_PRINTF(("ClassMentry addr	= 0x%x\n", addr1));
  OPL_DAL_PRINTF(("ClassRentry  addr = 0x%x\n", addr2));
#endif  
  /********************/
  iStatus = dalClsTblEntryDel(&ClassMentry, &ClassRentry, pCruleID);
  if (OPL_OK != iStatus) {
    return iStatus;
  }

  return OPL_OK;
}

/*******************************************************************************
* dalClsRst - Reset classify engine.
*
* DESCRIPTION:
*       This function clears all classify tables, but not free the tables.
*
* INPUT:
*   None.
*
* OUTPUT:
*       None.
*
* RETURNS:
*       OPL_OK.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsRst(void)
{
  OPL_STATUS iStatus;
  UINT32 rstCnt;
  UINT32 offset;

  /*Clear classify ASCI tables when reset*/
  iStatus = oplRegFieldWrite(REG_CLE_INIT, 0, 1, 0x01);
  if (iStatus < 0) {
    OPL_LOG_TRACE();
    return iStatus;
  }

  for (rstCnt = 0; rstCnt < CLASSIFY_RULE_TABLE_GROUP; rstCnt++) {
    gFreeCnt[rstCnt] = OPL_INVALID;
  }

  /*Initialize maintain list*/
  for (rstCnt = 1; rstCnt < CLASSIFY_MAX_RULE_ENTRY; rstCnt++) {
    if (0 != (rstCnt % CLASSIFY_ENTRYS_PER_GROUP)) {
      gFreeCnt[rstCnt / CLASSIFY_ENTRYS_PER_GROUP]++;   /* Clear rule table alloc record */
    }
  }

  /* Clear classify mask table */
  OPL_BZERO(gpCmaskTbl,
            (sizeof(CLASSIFY_MASK_ENTRY_t) * CLASSIFY_MAX_MASK_ENTRY));   

  for (offset = 0; offset < CLASSIFY_RULE_TABLE_GROUP; offset++) {
    for (rstCnt = 0; rstCnt < CLASSIFY_MAX_ADDR_ENTRY; rstCnt++) {
      gpCaddrTbl[rstCnt].ClasAddr[offset] = OPL_INVALID; /* Clear hash address table */
      if (OPL_OK != iStatus) {
        return iStatus;
      }
    }
  }

  /* Clear classify item table */
  OPL_BZERO(gClassifyItem, (sizeof(CLASSIFY_ITEM_t) * CLASSIFY_MAX_ITEM_NUM));    

  /* Clear classify rule table */
  OPL_BZERO(gpCruleTbl,
            (sizeof(CLASSIFY_RULE_ENTRY_t) * CLASSIFY_MAX_RULE_ENTRY));

  /* Clear malloc control table */
  OPL_BZERO(gpMallocCtl,
            (sizeof(CLASSIFY_RULE_MALLOC_t) * CLASSIFY_MAX_RULE_ENTRY));

  OPL_BZERO(gpMaskInfoForRule,
            sizeof(CLASSIFY_MASKINFO_FOR_RULE_t) * CLASSIFY_MAX_MASK_ENTRY);
  return OPL_OK;
}



/*******************************************************************************
*
* dalClsInit - initialize the classification tables.
*
* DESCRIPTION:
*       malloc memory for local classification tables.
*
* INPUT:
*       NULL.
*
* OUTPUT:
*       NULL.
*
* RETURN:
*   An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsInit(void)
{
  OPL_STATUS iStatus = OPL_OK;
  
  iStatus = clsInit();
  if (iStatus < 0) {
    OPL_LOG_TRACE();
    return iStatus;
  }

  iStatus = oplRegFieldWrite(REG_CLE_INIT, 0, 1, 0x01);
  if (iStatus < 0) {
    OPL_LOG_TRACE();
    return iStatus;
  }

  iStatus = oplRegWrite(REG_CLE_CTRL0, 0x18180000);

  if (iStatus < 0) {
    OPL_LOG_TRACE();
    return iStatus;
  }

  if (OPL_NULL != gpCmaskTbl) {
    OPL_FREE(gpCmaskTbl);
    gpCmaskTbl = OPL_NULL;
  }
  if (OPL_NULL != gpCruleTbl) {
    OPL_FREE(gpCruleTbl);
    gpCruleTbl = OPL_NULL;
  }
  if (OPL_NULL != gpCaddrTbl) {
    OPL_FREE(gpCaddrTbl);
    gpCaddrTbl = OPL_NULL;
  }
  if (OPL_NULL != gpMaskInfoForRule) {
    OPL_FREE(gpMaskInfoForRule);
    gpMaskInfoForRule = OPL_NULL;
  }

  gpCmaskTbl = (CLASSIFY_MASK_ENTRY_t *)
               (OPL_MALLOC((sizeof(CLASSIFY_MASK_ENTRY_t) * CLASSIFY_MAX_MASK_ENTRY)));
  if (OPL_NULL == gpCmaskTbl) {
    return OPL_ERR_MEM_ALLOC_FAIL;
  }
  /* Memset gpCmaskTbl in function cltClsRst() */

  gpCruleTbl = (CLASSIFY_RULE_ENTRY_t *)
               (OPL_MALLOC((sizeof(CLASSIFY_RULE_ENTRY_t) * CLASSIFY_MAX_RULE_ENTRY)));
  if (OPL_NULL == gpCruleTbl) {
    OPL_FREE(gpCmaskTbl);
    return OPL_ERR_MEM_ALLOC_FAIL;
  }
  /* Memset gpCruleTbl in function cltClsRst() */

  gpCaddrTbl = (CLASSIFY_ADDR_ENTRY_t *)
               (OPL_MALLOC((sizeof(CLASSIFY_ADDR_ENTRY_t) * CLASSIFY_MAX_ADDR_ENTRY)));
  if (OPL_NULL == gpCaddrTbl) {
    OPL_FREE(gpCruleTbl);
    OPL_FREE(gpCmaskTbl);
    return OPL_ERR_MEM_ALLOC_FAIL;
  }
  /* Clear gpCaddrTbl in function cltClsRst() */

  gpMallocCtl = (CLASSIFY_RULE_MALLOC_t *)
                (OPL_MALLOC((sizeof(CLASSIFY_RULE_MALLOC_t) * CLASSIFY_MAX_RULE_ENTRY)));
  if (OPL_NULL == gpMallocCtl) {
    OPL_FREE(gpCruleTbl);
    OPL_FREE(gpCmaskTbl);
    OPL_FREE(gpCaddrTbl);
    return OPL_ERR_MEM_ALLOC_FAIL;
  }
  /* Memset gpMallocCtl in function cltClsRst() */

  gpMaskInfoForRule = (CLASSIFY_MASKINFO_FOR_RULE_t *)
                      (OPL_MALLOC((sizeof(CLASSIFY_MASKINFO_FOR_RULE_t) * CLASSIFY_MAX_MASK_ENTRY)));
  if (OPL_NULL == gpMaskInfoForRule) {
    OPL_FREE(gpCruleTbl);
    OPL_FREE(gpCmaskTbl);
    OPL_FREE(gpCaddrTbl);
    OPL_FREE(gpMallocCtl);
    return OPL_ERR_MEM_ALLOC_FAIL;
  }

  iStatus = dalClsRst();
  if (OPL_OK != iStatus) {
    return iStatus;
  }

  iStatus = dalClsCtcInit();
  if (OPL_OK != iStatus) {
    return iStatus;
  }

	OPL_MEMSET(&g_dal_acl_list, 0, sizeof(opl_acl_list_t));
	INIT_LIST_HEAD(&g_dal_acl_list.list);

	mutex_init(&g_dalClsLock);

  return OPL_OK;
}


/*******************************************************************************
* dalClsTranActSet - Configure the transform action field of rule entry for in-profile packet.
*
* DESCRIPTION:
*    This function reads the rule entry from rule table, changes the transform action
*    field directly and writes the rule entry to the table.
*
* INPUT:
*    classRuleID    - Classify rule entry ID.
*    actValue   - Classify transform action value.
*
* OUTPUT:
*    None.
*
* RETURNS:
*        OPL_OK             - If the operation is successful.
*        CLASSIFY_INVALID_PARAMETER_LENGTH
*                                   - If the classify rule ID out of range.
*    Others                     -   an appropriate specific error code defined
*                     for CLT device driver.
* SEE ALSO: 
*/
OPL_STATUS dalClsTranActSet(UINT16 classRuleID,UINT8 actValue)
{
  OPL_STATUS iStatus;
  CLASSIFY_RULE_ENTRY_t RuleEntry;  

  if (CLASSIFY_MAX_RULE_ENTRY <= classRuleID) {
    return CLASSIFY_INVALID_PARAMETER_LENGTH;
  } 

  OPL_BZERO(&RuleEntry, sizeof(CLASSIFY_RULE_ENTRY_t));

  /* Read rule entry from local rule table */
  iStatus = dalClsRuleTblRd(&RuleEntry, classRuleID, CLASSIFY_LOCAL_TBL);
  if (OPL_OK != iStatus) {
    return iStatus;
  }

  RuleEntry.bfTranActH = (actValue & 0X8) >> 3; /*Writes act value directly*/
  RuleEntry.bfTranActL = actValue & 0X7; /*Writes act value directly*/

  iStatus = dalClsRuleTblWr(&RuleEntry, classRuleID);
  if (OPL_OK != iStatus) {
    return iStatus;
  }

  return OPL_OK;
}


/*******************************************************************************
* dalClsRmkActSet - Configure the transform action field of rule entry for in-profile packet.
*
* DESCRIPTION:
*    This function reads the rule entry from rule table, changes the transform action
*    field directly and writes the rule entry to the table.
*
* INPUT:
*    classRuleID    - Classify rule entry ID.
*    cosValue   - Classify transform action value.
*
* OUTPUT:
*    None.
*
* RETURNS:
*        OPL_OK             - If the operation is successful.
*        CLASSIFY_INVALID_PARAMETER_LENGTH
*                                   - If the classify rule ID out of range.
*    Others                     -   an appropriate specific error code defined
*                     for CLT device driver.
* SEE ALSO: 
*/
OPL_STATUS dalClsRmkActSet(UINT16 classRuleID,UINT8 rmkactVal)
{
  OPL_STATUS iStatus;
  CLASSIFY_RULE_ENTRY_t RuleEntry;  

  if (CLASSIFY_MAX_RULE_ENTRY <= classRuleID) {
    return CLASSIFY_INVALID_PARAMETER_LENGTH;
  } 

  OPL_BZERO(&RuleEntry, sizeof(CLASSIFY_RULE_ENTRY_t));

  /* Read rule entry from local rule table */
  iStatus = dalClsRuleTblRd(&RuleEntry, classRuleID, CLASSIFY_LOCAL_TBL);
  if (OPL_OK != iStatus) {
    return iStatus;
  }

  RuleEntry.bfRmkAct = rmkactVal; /*Writes act value directly*/


  iStatus = dalClsRuleTblWr(&RuleEntry, classRuleID);
  if (OPL_OK != iStatus) {
    return iStatus;
  }

  return OPL_OK;
}


/*******************************************************************************
* dalClsRmkCosSet - Configure the transform action field of rule entry for in-profile packet.
*
* DESCRIPTION:
*    This function reads the rule entry from rule table, changes the transform action
*    field directly and writes the rule entry to the table.
*
* INPUT:
*    classRuleID    - Classify rule entry ID.
*    cosValue   - Classify transform action value.
*
* OUTPUT:
*    None.
*
* RETURNS:
*        OPL_OK             - If the operation is successful.
*        CLASSIFY_INVALID_PARAMETER_LENGTH
*                                   - If the classify rule ID out of range.
*    Others                     -   an appropriate specific error code defined
*                     for CLT device driver.
* SEE ALSO: 
*/
OPL_STATUS dalClsRmkCosSet(UINT16 classRuleID,UINT8 cosValue)
{
  OPL_STATUS iStatus;
  CLASSIFY_RULE_ENTRY_t RuleEntry;  

  if (CLASSIFY_MAX_RULE_ENTRY <= classRuleID) {
    return CLASSIFY_INVALID_PARAMETER_LENGTH;
  } 

  OPL_BZERO(&RuleEntry, sizeof(CLASSIFY_RULE_ENTRY_t));

  /* Read rule entry from local rule table */
  iStatus = dalClsRuleTblRd(&RuleEntry, classRuleID, CLASSIFY_LOCAL_TBL);
  if (OPL_OK != iStatus) {
    return iStatus;
  }

  RuleEntry.bfRmkCos = cosValue; /*Writes act value directly*/

  iStatus = dalClsRuleTblWr(&RuleEntry, classRuleID);
  if (OPL_OK != iStatus) {
    return iStatus;
  }

  return OPL_OK;
}



/*******************************************************************************
* dalClsRmkIcosSet - Configure the transform action field of rule entry for in-profile packet.
*
* DESCRIPTION:
*    This function reads the rule entry from rule table, changes the transform action
*    field directly and writes the rule entry to the table.
*
* INPUT:
*    classRuleID    - Classify rule entry ID.
*    iCosVal    - Classify transform action value.
*
* OUTPUT:
*    None.
*
* RETURNS:
*        OPL_OK             - If the operation is successful.
*        CLASSIFY_INVALID_PARAMETER_LENGTH
*                                   - If the classify rule ID out of range.
*    Others                     -   an appropriate specific error code defined
*                     for CLT device driver.
* SEE ALSO: 
*/
OPL_STATUS dalClsRmkIcosSet(UINT16 classRuleID,UINT8 iCosVal)
{
  OPL_STATUS iStatus;
  CLASSIFY_RULE_ENTRY_t RuleEntry;  

  if (CLASSIFY_MAX_RULE_ENTRY <= classRuleID) {
    return CLASSIFY_INVALID_PARAMETER_LENGTH;
  } 

  OPL_BZERO(&RuleEntry, sizeof(CLASSIFY_RULE_ENTRY_t));

  /* Read rule entry from local rule table */
  iStatus = dalClsRuleTblRd(&RuleEntry, classRuleID, CLASSIFY_LOCAL_TBL);
  if (OPL_OK != iStatus) {
    return iStatus;
  }

  RuleEntry.bfRmkIcos = iCosVal; /*Writes act value directly*/

  iStatus = dalClsRuleTblWr(&RuleEntry, classRuleID);
  if (OPL_OK != iStatus) {
    return iStatus;
  }

  return OPL_OK;
}


/*******************************************************************************
* dalClsRmkVidSet - Configure the transform action field of rule entry for in-profile packet.
*
* DESCRIPTION:
*    This function reads the rule entry from rule table, changes the transform action
*    field directly and writes the rule entry to the table.
*
* INPUT:
*    classRuleID    - Classify rule entry ID.
*    rmkVid   - Classify rmkVlan Value
*
* OUTPUT:
*    None.
*
* RETURNS:
*        OPL_OK             - If the operation is successful.
*        CLASSIFY_INVALID_PARAMETER_LENGTH
*                                   - If the classify rule ID out of range.
*    Others                     -   an appropriate specific error code defined
*                     for CLT device driver.
* SEE ALSO: 
*/
OPL_STATUS dalClsRmkVidSet(UINT16 classRuleID,UINT16 rmkVid)
{
  OPL_STATUS iStatus;
  CLASSIFY_RULE_ENTRY_t RuleEntry;  

  if (CLASSIFY_MAX_RULE_ENTRY <= classRuleID) {
    return CLASSIFY_INVALID_PARAMETER_LENGTH;
  } 

  OPL_BZERO(&RuleEntry, sizeof(CLASSIFY_RULE_ENTRY_t));

  /* Read rule entry from local rule table */
  iStatus = dalClsRuleTblRd(&RuleEntry, classRuleID, CLASSIFY_LOCAL_TBL);
  if (OPL_OK != iStatus) {
    return iStatus;
  }

  RuleEntry.bfRmkVid = rmkVid; /*Writes act value directly*/

  iStatus = dalClsRuleTblWr(&RuleEntry, classRuleID);
  if (OPL_OK != iStatus) {
    return iStatus;
  }

  return OPL_OK;
}
OPL_STATUS dalClsRmkVidActSet(UINT16 classRuleID,UINT8 rmkVidAct,UINT16 rmkVid)
{
  OPL_STATUS iStatus;
  CLASSIFY_RULE_ENTRY_t RuleEntry;  

  if (CLASSIFY_MAX_RULE_ENTRY <= classRuleID) {
    return CLASSIFY_INVALID_PARAMETER_LENGTH;
  } 

  OPL_BZERO(&RuleEntry, sizeof(CLASSIFY_RULE_ENTRY_t));

  /* Read rule entry from local rule table */
  iStatus = dalClsRuleTblRd(&RuleEntry, classRuleID, CLASSIFY_LOCAL_TBL);
  if (OPL_OK != iStatus) {
    return iStatus;
  }

  RuleEntry.bfRmkAct &= 0x3;
  RuleEntry.bfRmkAct |= ((rmkVidAct & 0x3) << 2);
  RuleEntry.bfRmkVid = rmkVid; /*Writes act value directly*/
  iStatus = dalClsRuleTblWr(&RuleEntry, classRuleID);
  if (OPL_OK != iStatus) {
    return iStatus;
  }

  return OPL_OK;
}

/*******************************************************************************
*
* dalClsEgressPortCfg - Configure egress port and port mask field for classification
*
* DESCRIPTION:
*       If the input parameter enable=1, this function will enable 
*   the egress port field mask and write egress port value into the rule entry.
*   
*   If the input parameter enable=0, this function will disable
*       the egress port field mask and clear its value from the rule entry.
*
*
* INPUT:
*   classItemID   - classify item index. 1~511 is valid.
*   pktType     - Packet protocol type. 1:PPPoE, 2:EtherNet.
*       portID      - Logic port ID.
*   eMask     - egress port mask.
*   bEnable     - 0:disable, 1:enable
*
* OUTPUT:
*       None.
*
* RETURN:
*   An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsEgressPortCfg(UINT16 classItemID,
                               UINT8 pktType,
                               UINT8 portID,
                               UINT8 eMask,
                               UINT8 bEnable)
{
  CLASSIFY_PARAMETER_t ClassTemp;

  CLASSIFY_PARAMETER_t ClassParameter;
  UINT16 classRuleID;
  OPL_STATUS iStatus = OPL_OK;
  opl_cls_hwtbl_t  classMaskID;

  OPL_BZERO(&ClassParameter, sizeof(CLASSIFY_PARAMETER_t));
  if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
    ClassParameter = gClassifyItem[classItemID - 1].Parameter;
  }

  if (OPL_ENABLE == bEnable) {
    ClassParameter.RuleEntry.bfEgress = portID;
    ClassParameter.egressMask = eMask;

    if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
      /*delete old item.*/
      ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
      iStatus = dalClsEntryMove(&ClassTemp, &classRuleID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure MOVE Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      /*add new item.*/
      ClassTemp = ClassParameter;
      iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
      if (OPL_OK != iStatus) {
        /**/
        ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
        if (OPL_OK == dalClsEntrySet(&ClassTemp, &classRuleID,&classMaskID)) {
          gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
        }
        OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }
    }
    else {
      ClassParameter.pktType = pktType;

      /*add new item.*/
      ClassTemp = ClassParameter;
      iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      gClassifyItem[classItemID - 1].valid = OPL_VALID;
      gClassifyItem[classItemID - 1].Parameter.pktType = pktType;
    }

    gClassifyItem[classItemID - 1].Parameter.RuleEntry.bfEgress = ClassParameter.RuleEntry.bfEgress;
    gClassifyItem[classItemID - 1].Parameter.egressMask = ClassParameter.egressMask;
    gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
  }
  else {
    if (OPL_INVALID == gClassifyItem[classItemID - 1].valid) {
      OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
      return iStatus;
    }

    ClassParameter.RuleEntry.bfEgress = 0;
    ClassParameter.egressMask = 0;

    if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
      /*delete old item.*/
      ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
      iStatus = dalClsEntryMove(&ClassTemp, &classRuleID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure MOVE Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      /*add new item.*/
      ClassTemp = ClassParameter;
      iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
      if (OPL_OK != iStatus) {
        /**/
        ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
        if (OPL_OK == dalClsEntrySet(&ClassTemp, &classRuleID,&classMaskID)) {
          gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
        }
        OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }
    }

    gClassifyItem[classItemID - 1].Parameter.RuleEntry.bfEgress = 0;
    gClassifyItem[classItemID - 1].Parameter.egressMask = 0;
    gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
  }
  return iStatus;
}

/*******************************************************************************
*
* dalClsIngressPortCfg - Configure ingress port and port mask field for classification
*
* DESCRIPTION:
*       If the input parameter enable=1, this function will enable 
*   the ingress port field mask and write ingress port value into the rule entry.
*   
*   If the input parameter enable=0, this function will disable
*       the ingress port field mask and clear its value from the rule entry.
*
*
* INPUT:
*   classItemID   - classify item index. 1~511 is valid.
*   pktType     - Packet protocol type. 1:PPPoE, 2:EtherNet.
*       portID      - Logic port ID.
*   iMask     - Ingress port mask.
*   bEnable     - 0:disable, 1:enable
*
* OUTPUT:
*       None.
*
* RETURN:
*   An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsIngressPortCfg(UINT16 classItemID,
                                UINT8 pktType,
                                UINT8 portID,
                                UINT8 iMask,
                                UINT8 bEnable)
{
  CLASSIFY_PARAMETER_t ClassTemp;

  CLASSIFY_PARAMETER_t ClassParameter;
  UINT16 classRuleID;
  OPL_STATUS iStatus = OPL_OK;
  opl_cls_hwtbl_t  classMaskID;

  OPL_BZERO(&ClassParameter, sizeof(CLASSIFY_PARAMETER_t));
  if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
    ClassParameter = gClassifyItem[classItemID - 1].Parameter;
  }

  if (OPL_ENABLE == bEnable) {
    ClassParameter.RuleEntry.bfIngress = portID;
    ClassParameter.ingressMask = iMask;

    if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
      /*delete old item.*/
      ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
      iStatus = dalClsEntryMove(&ClassTemp, &classRuleID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure MOVE Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      /*add new item.*/
      ClassTemp = ClassParameter;
      iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
      if (OPL_OK != iStatus) {
        /**/
        ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
        if (OPL_OK == dalClsEntrySet(&ClassTemp, &classRuleID,&classMaskID)) {
          gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
        }
        OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }
    }
    else {
      ClassParameter.pktType = pktType;

      /*add new item.*/
      ClassTemp = ClassParameter;
      iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      gClassifyItem[classItemID - 1].valid = OPL_VALID;
      gClassifyItem[classItemID - 1].Parameter.pktType = pktType;
    }

    gClassifyItem[classItemID - 1].Parameter.RuleEntry.bfIngress = ClassParameter.RuleEntry.bfIngress;
    gClassifyItem[classItemID - 1].Parameter.ingressMask = ClassParameter.ingressMask;
    gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
  }
  else {
    if (OPL_INVALID == gClassifyItem[classItemID - 1].valid) {
      OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
      return iStatus;
    }

    ClassParameter.RuleEntry.bfIngress = 0;
    ClassParameter.ingressMask = 0;

    if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
      /*delete old item.*/
      ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
      iStatus = dalClsEntryMove(&ClassTemp, &classRuleID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure MOVE Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      /*add new item.*/
      ClassTemp = ClassParameter;
      iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
      if (OPL_OK != iStatus) {
        /**/
        ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
        if (OPL_OK == dalClsEntrySet(&ClassTemp, &classRuleID,&classMaskID)) {
          gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
        }
        OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }
    }

    gClassifyItem[classItemID - 1].Parameter.RuleEntry.bfIngress = 0;
    gClassifyItem[classItemID - 1].Parameter.ingressMask = 0;
    gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
  }
  return iStatus;
}

/*************************************************************************
* NAME: cltClsTranAct - Configure classify transform action.
*
* DESCRIPTION:
*   This function writes transformation value into the TRANF_ACT field
*   of the rule table.
*
* INPUTS:
*   classItemID   - Classify item index.
*   pktType     - Packet protocol type. 1:PPPoE, 2:EtherNet.
*   actValue      - action value.
*
* OUTPUTS:
*   None.
*   
* RETURN:
*   An appropriate specific error code defined for CLT device driver.
*
* SIDE EFFECTS:
*
*/
OPL_STATUS dalClsTranActCfg(UINT16 classItemID,UINT8 pktType,UINT8 actValue)
{
  OPL_STATUS iStatus;
  CLASSIFY_PARAMETER_t ClassParameter;
  UINT16 classRuleID;
  opl_cls_hwtbl_t  classMaskID;


  /* TO DO: Add your handler code here */
  OPL_BZERO(&ClassParameter, sizeof(CLASSIFY_PARAMETER_t));

  if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
    ClassParameter = gClassifyItem[classItemID - 1].Parameter;
    classRuleID = gClassifyItem[classItemID - 1].classifyRuleID;

    iStatus = dalClsTranActSet(classRuleID, actValue);
    if (OPL_OK != iStatus) {
      OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
      return iStatus;
    }
  }
  else {
    ClassParameter.pktType = pktType;
    ClassParameter.RuleEntry.bfTranActH = (actValue & 0X8) >> 3;
    ClassParameter.RuleEntry.bfTranActL = actValue & 0X7;

    /*add new item.*/
    iStatus = dalClsEntrySet(&(ClassParameter), &classRuleID,&classMaskID);
    if (OPL_OK != iStatus) {
      OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
      return iStatus;
    }
    gClassifyItem[classItemID - 1].valid = OPL_VALID;
    gClassifyItem[classItemID - 1].Parameter.pktType = pktType;
  }

  gClassifyItem[classItemID - 1].Parameter.RuleEntry.bfTranActH = (actValue & 0X8)
>> 3;   
  gClassifyItem[classItemID - 1].Parameter.RuleEntry.bfTranActL = actValue & 0X7;   

  gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
  return iStatus;
}


/*************************************************************************
* NAME: dalClsRmkActCfg - Configure classify transform action.
*
* DESCRIPTION:
*   This function writes transformation value into the TRANF_ACT field
*   of the rule table.
*
* INPUTS:
*   classItemID   - Classify item index.
*   pktType     - Packet protocol type. 1:PPPoE, 2:EtherNet.
*   rmkActVal     - action value.
*
* OUTPUTS:
*   None.
*   
* RETURN:
*   An appropriate specific error code defined for CLT device driver.
*
* SIDE EFFECTS:
*
*/
OPL_STATUS dalClsRmkActCfg(UINT16 classItemID,UINT8 pktType,UINT8 rmkActVal)
{
  OPL_STATUS iStatus;
  CLASSIFY_PARAMETER_t ClassParameter;
  UINT16 classRuleID;
 opl_cls_hwtbl_t  classMaskID;


  /* TO DO: Add your handler code here */
  OPL_BZERO(&ClassParameter, sizeof(CLASSIFY_PARAMETER_t));

  if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
    ClassParameter = gClassifyItem[classItemID - 1].Parameter;
    classRuleID = gClassifyItem[classItemID - 1].classifyRuleID;

    iStatus = dalClsRmkActSet(classRuleID, rmkActVal);
    if (OPL_OK != iStatus) {
      OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
      return iStatus;
    }
  }
  else {
    ClassParameter.pktType = pktType;
    ClassParameter.RuleEntry.bfRmkAct = rmkActVal;

    /*add new item.*/
    iStatus = dalClsEntrySet(&(ClassParameter), &classRuleID,&classMaskID);
    if (OPL_OK != iStatus) {
      OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
      return iStatus;
    }
    gClassifyItem[classItemID - 1].valid = OPL_VALID;
    gClassifyItem[classItemID - 1].Parameter.pktType = pktType;
  }

  gClassifyItem[classItemID - 1].Parameter.RuleEntry.bfRmkAct = rmkActVal;
  gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
  return iStatus;
}

OPL_STATUS dalClsRmkActGet(UINT16 classItemID,UINT8 pktType,UINT8 *rmkActVal)
{

  if (OPL_NULL == rmkActVal) {
    OPL_DAL_PRINTF(("Null pointer input.\n"));
    return OPL_ERR_NULL_POINTER;
  }
  *rmkActVal = gClassifyItem[classItemID - 1].Parameter.RuleEntry.bfRmkAct;

  return OPL_OK;
}

/*************************************************************************
* NAME: dalClsRmkCosCfg - Configure classify transform action.
*
* DESCRIPTION:
*   This function writes transformation value into the TRANF_ACT field
*   of the rule table.
*
* INPUTS:
*   classItemID   - Classify item index.
*   pktType     - Packet protocol type. 1:PPPoE, 2:EtherNet.
*   cosValue      - action value.
*
* OUTPUTS:
*   None.
*   
* RETURN:
*   An appropriate specific error code defined for CLT device driver.
*
* SIDE EFFECTS:
*
*/
OPL_STATUS dalClsRmkCosCfg(UINT16 classItemID,UINT8 pktType,UINT8 cosValue)
{
  OPL_STATUS iStatus;
  CLASSIFY_PARAMETER_t ClassParameter;
  UINT16 classRuleID;
  opl_cls_hwtbl_t  classMaskID;


  /* TO DO: Add your handler code here */
  OPL_BZERO(&ClassParameter, sizeof(CLASSIFY_PARAMETER_t));

  if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
    ClassParameter = gClassifyItem[classItemID - 1].Parameter;
    classRuleID = gClassifyItem[classItemID - 1].classifyRuleID;

    iStatus = dalClsRmkCosSet(classRuleID, cosValue);
    if (OPL_OK != iStatus) {
      OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
      return iStatus;
    }
  }
  else {
    ClassParameter.pktType = pktType;
    ClassParameter.RuleEntry.bfRmkCos = cosValue;

    /*add new item.*/
    iStatus = dalClsEntrySet(&(ClassParameter), &classRuleID,&classMaskID);
    if (OPL_OK != iStatus) {
      OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
      return iStatus;
    }
    gClassifyItem[classItemID - 1].valid = OPL_VALID;
    gClassifyItem[classItemID - 1].Parameter.pktType = pktType;
  }

  gClassifyItem[classItemID - 1].Parameter.RuleEntry.bfRmkCos = cosValue;   
  gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
  return iStatus;
}

/*************************************************************************
* NAME: dalClsRmkIcosCfg - Configure classify transform action.
*
* DESCRIPTION:
*   This function writes transformation value into the TRANF_ACT field
*   of the rule table.
*
* INPUTS:
*   classItemID   - Classify item index.
*   pktType     - Packet protocol type. 1:PPPoE, 2:EtherNet.
*   IcosValue   - icos value.
*
* OUTPUTS:
*   None.
*   
* RETURN:
*   An appropriate specific error code defined for CLT device driver.
*
* SIDE EFFECTS:
*
*/
OPL_STATUS dalClsRmkIcosCfg(UINT16 classItemID,UINT8 pktType,UINT8 iCosValue)
{
  OPL_STATUS iStatus;
  CLASSIFY_PARAMETER_t ClassParameter;
  UINT16 classRuleID;
  opl_cls_hwtbl_t  classMaskID;


  /* TO DO: Add your handler code here */
  OPL_BZERO(&ClassParameter, sizeof(CLASSIFY_PARAMETER_t));

  if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
    ClassParameter = gClassifyItem[classItemID - 1].Parameter;
    classRuleID = gClassifyItem[classItemID - 1].classifyRuleID;

    iStatus = dalClsRmkIcosSet(classRuleID, iCosValue);
    if (OPL_OK != iStatus) {
      OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
      return iStatus;
    }
  }
  else {
    ClassParameter.pktType = pktType;
    ClassParameter.RuleEntry.bfRmkIcos = iCosValue;

    /*add new item.*/
    iStatus = dalClsEntrySet(&(ClassParameter), &classRuleID,&classMaskID);
    if (OPL_OK != iStatus) {
      OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
      return iStatus;
    }
    gClassifyItem[classItemID - 1].valid = OPL_VALID;
    gClassifyItem[classItemID - 1].Parameter.pktType = pktType;
  }

  gClassifyItem[classItemID - 1].Parameter.RuleEntry.bfRmkIcos = iCosValue;   
  gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
  return iStatus;
}

/*************************************************************************
* NAME: dalClsRmkVidCfg - Configure classify transform action.
*
* DESCRIPTION:
*   This function writes transformation value into the TRANF_ACT field
*   of the rule table.
*
* INPUTS:
*   classItemID   - Classify item index.
*   pktType     - Packet protocol type. 1:PPPoE, 2:EtherNet.
*   rmkVid        - vid value.
*
* OUTPUTS:
*   None.
*   
* RETURN:
*   An appropriate specific error code defined for CLT device driver.
*
* SIDE EFFECTS:
*
*/
OPL_STATUS dalClsRmkVidCfg(UINT16 classItemID,UINT8 pktType,UINT16 rmkVid)
{
  OPL_STATUS iStatus;
  CLASSIFY_PARAMETER_t ClassParameter;
  UINT16 classRuleID;
  opl_cls_hwtbl_t  classMaskID;


  /* TO DO: Add your handler code here */
  OPL_BZERO(&ClassParameter, sizeof(CLASSIFY_PARAMETER_t));

  if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
    ClassParameter = gClassifyItem[classItemID - 1].Parameter;
    classRuleID = gClassifyItem[classItemID - 1].classifyRuleID;

    iStatus = dalClsRmkVidSet(classRuleID, rmkVid);
    if (OPL_OK != iStatus) {
      OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
      return iStatus;
    }
  }
  else {
    ClassParameter.pktType = pktType;
    ClassParameter.RuleEntry.bfRmkVid = rmkVid;

    /*add new item.*/
    iStatus = dalClsEntrySet(&(ClassParameter), &classRuleID,&classMaskID);
    if (OPL_OK != iStatus) {
      OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
      return iStatus;
    }
    gClassifyItem[classItemID - 1].valid = OPL_VALID;
    gClassifyItem[classItemID - 1].Parameter.pktType = pktType;
  }

  gClassifyItem[classItemID - 1].Parameter.RuleEntry.bfRmkVid = rmkVid;   
  gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
  return iStatus;
}


/*******************************************************************************
*
* dalClsMaskPrioCfg - Set classify mask priority.
*
* DESCRIPTION:
*       This function writes classify mask priority directly.
*
* INPUT:
*   classItemID   - classify item index. 1~511 is valid.
*   pktType     - Packet protocol type. 1:PPPoE, 2:EtherNet.
*       maskPrio    - Mask priority value.
*
* OUTPUT:
*       None.
*
* RETURN:
*   An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsMaskPrioCfg(UINT16 classItemID,UINT8 pktType,UINT8 maskPrio)
{
  OPL_STATUS iStatus = OPL_OK;    

  CLASSIFY_PARAMETER_t ClassParameter;
  CLASSIFY_PARAMETER_t ClassTemp;
  UINT16 classRuleID;
  opl_cls_hwtbl_t  classMaskID;

  if ((OPL_ZERO == classItemID) || (CLASSIFY_MAX_MASK_ENTRY <= classItemID)) {
    OPL_DAL_PRINTF(("Classification ID Out of Range!\r\n"));
    return iStatus;
  }

  OPL_BZERO(&ClassParameter, sizeof(CLASSIFY_PARAMETER_t));

  if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
    ClassParameter = gClassifyItem[classItemID - 1].Parameter;
  }

  ClassParameter.maskPri = maskPrio;

  if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
    /*delete old item.*/
    ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
    iStatus = dalClsEntryMove(&ClassTemp, &classRuleID);
    if (OPL_OK != iStatus) {
      OPL_DAL_PRINTF(("Configure MOVE Error = 0x%x.\r\n", iStatus));
      return iStatus;
    }

    /*add new item.*/
    ClassTemp = ClassParameter;
    iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
    if (OPL_OK != iStatus) {
      /**/
      ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
      if (OPL_OK == dalClsEntrySet(&ClassTemp, &classRuleID,&classMaskID)) {
        gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
      }
      OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
      return iStatus;
    }
  }
  else {
    ClassParameter.pktType = pktType;

    /*add new item.*/
    ClassTemp = ClassParameter;
    iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
    if (OPL_OK != iStatus) {
      OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
      return iStatus;
    }
    gClassifyItem[classItemID - 1].valid = OPL_VALID;
    gClassifyItem[classItemID - 1].Parameter.pktType = pktType;
  }

  gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
  gClassifyItem[classItemID - 1].Parameter.maskPri = ClassParameter.maskPri;
  return iStatus;
}

/*******************************************************************************
* dalClsDestMacAddrCfg - Configure the destination MAC address field of current 
*             classification.
*
* DESCRIPTION:
*       If the input parameter enable=1, this function will enable the destination MAC 
*   address field mask and write MAC value into the rule entry.
*
*   If the input parameter enable=0, this function will disable the destination MAC
*   address field mask and clear its value from the rule entry.
*
* INPUT:
*   classItemID   - classify item index. 1~511 is valid.
*   pktType     - Packet protocol type. 1:PPPoE, 2:EtherNet.
*   pDestMac    - Destination MAC address value. Input with string format.
*   pUsrMask    - MAC address mask. Input with string format 
*   bEnable     - 0:disable, 1:enable
*
* OUTPUT:
*       None.
*
* RETURN:
*   An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsDestMacAddrCfg(UINT16 classItemID,
                                UINT8 pktType,
                                INT8 *pDestMac,
                                INT8 *pUsrMask,
                                UINT8 bEnable)
{
  CLASSIFY_PARAMETER_t ClassTemp;

  CLASSIFY_PARAMETER_t ClassParameter;
  UINT16 classRuleID;
  OPL_STATUS iStatus;
  opl_cls_hwtbl_t  classMaskID;

  UINT8 Mac[6] = {
    0, 0, 0, 0, 0, 0
  };
  UINT8 UsrMask[6] = {
    0, 0, 0, 0, 0, 0
  };

  if (OPL_ZERO == classItemID || TAB_CLS_RULE_LEN <= classItemID) {
    OPL_DAL_PRINTF(("this chip can only support %d rules,and the 0 items is not valid.\n",
                    TAB_CLS_RULE_LEN));
    return OPL_ERR_INVALID_PARAMETERS;
  } 

  if (OPL_NULL == pDestMac || OPL_NULL == pUsrMask) {
    OPL_DAL_PRINTF(("Null pointer input.\n"));
    return OPL_ERR_NULL_POINTER;
  }

  string2mac(pDestMac, Mac);
  string2mac(pUsrMask, UsrMask);

  OPL_BZERO(&ClassParameter, sizeof(CLASSIFY_PARAMETER_t));
  if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
    ClassParameter = gClassifyItem[classItemID - 1].Parameter;
  }

  if (OPL_ENABLE == bEnable) {
    switch (pktType) {
      case CLASSIFY_EII_IP:
        ClassParameter.FieldMap.EIIField.bfEnaDestMAC = OPL_ENABLE;
        ClassParameter.PktRuleValue.EIIValue.DestMac[0] = Mac[0];
        ClassParameter.PktRuleValue.EIIValue.DestMac[1] = Mac[1];
        ClassParameter.PktRuleValue.EIIValue.DestMac[2] = Mac[2];
        ClassParameter.PktRuleValue.EIIValue.DestMac[3] = Mac[3];
        ClassParameter.PktRuleValue.EIIValue.DestMac[4] = Mac[4];
        ClassParameter.PktRuleValue.EIIValue.DestMac[5] = Mac[5];

        ClassParameter.PktUsrMask.EIIValue.DestMac[0] = UsrMask[0];
        ClassParameter.PktUsrMask.EIIValue.DestMac[1] = UsrMask[1];
        ClassParameter.PktUsrMask.EIIValue.DestMac[2] = UsrMask[2];
        ClassParameter.PktUsrMask.EIIValue.DestMac[3] = UsrMask[3];
        ClassParameter.PktUsrMask.EIIValue.DestMac[4] = UsrMask[4];
        ClassParameter.PktUsrMask.EIIValue.DestMac[5] = UsrMask[5];

        break;
      case CLASSIFY_PPPOE_IP:
        ClassParameter.FieldMap.PppoEField.bfEnaDestMAC = OPL_VALID;
        ClassParameter.PktRuleValue.PPPoeValue.DestMac[0] = Mac[0];
        ClassParameter.PktRuleValue.PPPoeValue.DestMac[1] = Mac[1];
        ClassParameter.PktRuleValue.PPPoeValue.DestMac[2] = Mac[2];
        ClassParameter.PktRuleValue.PPPoeValue.DestMac[3] = Mac[3];
        ClassParameter.PktRuleValue.PPPoeValue.DestMac[4] = Mac[4];
        ClassParameter.PktRuleValue.PPPoeValue.DestMac[5] = Mac[5];

        ClassParameter.PktUsrMask.PPPoeValue.DestMac[0] = UsrMask[0];
        ClassParameter.PktUsrMask.PPPoeValue.DestMac[1] = UsrMask[1];
        ClassParameter.PktUsrMask.PPPoeValue.DestMac[2] = UsrMask[2];
        ClassParameter.PktUsrMask.PPPoeValue.DestMac[3] = UsrMask[3];
        ClassParameter.PktUsrMask.PPPoeValue.DestMac[4] = UsrMask[4];
        ClassParameter.PktUsrMask.PPPoeValue.DestMac[5] = UsrMask[5];

        break;
      default:
        OPL_DAL_PRINTF(("no such pkt type.\n"));  
        return OPL_ERR_INVALID_PARAMETERS;
        break;
    }


    if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
      /*delete old item.*/
      ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
      iStatus = dalClsEntryMove(&ClassTemp, &classRuleID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure MOVE Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      /*add new item.*/
      ClassTemp = ClassParameter;
      iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
      if (OPL_OK != iStatus) {
        /**/
        ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
        if (OPL_OK == dalClsEntrySet(&ClassTemp, &classRuleID,&classMaskID)) {
          gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
        }
        OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }
    }
    else {
      ClassParameter.pktType = pktType;

      /*add new item.*/
      ClassTemp = ClassParameter;
      iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      gClassifyItem[classItemID - 1].valid = OPL_VALID;
      gClassifyItem[classItemID - 1].Parameter.pktType = pktType;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaDestMAC = OPL_ENABLE;

        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.DestMac[0] = ClassParameter.PktRuleValue.EIIValue.DestMac[0];
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.DestMac[1] = ClassParameter.PktRuleValue.EIIValue.DestMac[1];
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.DestMac[2] = ClassParameter.PktRuleValue.EIIValue.DestMac[2];
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.DestMac[3] = ClassParameter.PktRuleValue.EIIValue.DestMac[3];
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.DestMac[4] = ClassParameter.PktRuleValue.EIIValue.DestMac[4];
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.DestMac[5] = ClassParameter.PktRuleValue.EIIValue.DestMac[5];

        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.EIIValue.DestMac[0] = ClassParameter.PktUsrMask.EIIValue.DestMac[0];
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.EIIValue.DestMac[1] = ClassParameter.PktUsrMask.EIIValue.DestMac[1];
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.EIIValue.DestMac[2] = ClassParameter.PktUsrMask.EIIValue.DestMac[2];
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.EIIValue.DestMac[3] = ClassParameter.PktUsrMask.EIIValue.DestMac[3];
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.EIIValue.DestMac[4] = ClassParameter.PktUsrMask.EIIValue.DestMac[4];
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.EIIValue.DestMac[5] = ClassParameter.PktUsrMask.EIIValue.DestMac[5];
        break;
      case CLASSIFY_PPPOE_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaDestMAC = OPL_ENABLE;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.DestMac[0] = ClassParameter.PktRuleValue.PPPoeValue.DestMac[0];
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.DestMac[1] = ClassParameter.PktRuleValue.PPPoeValue.DestMac[1];
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.DestMac[2] = ClassParameter.PktRuleValue.PPPoeValue.DestMac[2];
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.DestMac[3] = ClassParameter.PktRuleValue.PPPoeValue.DestMac[3];
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.DestMac[4] = ClassParameter.PktRuleValue.PPPoeValue.DestMac[4];
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.DestMac[5] = ClassParameter.PktRuleValue.PPPoeValue.DestMac[5];

        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.PPPoeValue.DestMac[0] = ClassParameter.PktUsrMask.PPPoeValue.DestMac[0];
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.PPPoeValue.DestMac[1] = ClassParameter.PktUsrMask.PPPoeValue.DestMac[1];
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.PPPoeValue.DestMac[2] = ClassParameter.PktUsrMask.PPPoeValue.DestMac[2];
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.PPPoeValue.DestMac[3] = ClassParameter.PktUsrMask.PPPoeValue.DestMac[3];
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.PPPoeValue.DestMac[4] = ClassParameter.PktUsrMask.PPPoeValue.DestMac[4];
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.PPPoeValue.DestMac[5] = ClassParameter.PktUsrMask.PPPoeValue.DestMac[5];
        break;
      default:
        OPL_DAL_PRINTF(("no such pkt type.\n"));  
        return OPL_ERR_INVALID_PARAMETERS;
    }
    gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
  }
  else {
    if (OPL_INVALID == gClassifyItem[classItemID - 1].valid) {
      OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
      return OPL_OK;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        if ((OPL_INVALID == gClassifyItem[classItemID - 1].valid)
         || (OPL_DISABLE
          == gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaDestMAC)) {
          OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
          return OPL_OK;
        }
        break;
      case CLASSIFY_PPPOE_IP:
        if ((OPL_INVALID == gClassifyItem[classItemID - 1].valid)
         || (OPL_DISABLE
          == gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaDestMAC)) {
          OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
          return OPL_OK;
        }
        break;
      default:
        OPL_DAL_PRINTF(("no such pkt type.\n"));  
        return OPL_ERR_INVALID_PARAMETERS;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        ClassParameter.FieldMap.EIIField.bfEnaDestMAC = OPL_DISABLE;
        ClassParameter.PktRuleValue.EIIValue.DestMac[0] = OPL_INVALID;
        ClassParameter.PktRuleValue.EIIValue.DestMac[1] = OPL_INVALID;
        ClassParameter.PktRuleValue.EIIValue.DestMac[2] = OPL_INVALID;
        ClassParameter.PktRuleValue.EIIValue.DestMac[3] = OPL_INVALID;
        ClassParameter.PktRuleValue.EIIValue.DestMac[4] = OPL_INVALID;
        ClassParameter.PktRuleValue.EIIValue.DestMac[5] = OPL_INVALID;
        break;
      case CLASSIFY_PPPOE_IP:
        ClassParameter.FieldMap.PppoEField.bfEnaDestMAC = OPL_DISABLE;
        ClassParameter.PktRuleValue.PPPoeValue.DestMac[0] = OPL_INVALID;
        ClassParameter.PktRuleValue.PPPoeValue.DestMac[1] = OPL_INVALID;
        ClassParameter.PktRuleValue.PPPoeValue.DestMac[2] = OPL_INVALID;
        ClassParameter.PktRuleValue.PPPoeValue.DestMac[3] = OPL_INVALID;
        ClassParameter.PktRuleValue.PPPoeValue.DestMac[4] = OPL_INVALID;
        ClassParameter.PktRuleValue.PPPoeValue.DestMac[5] = OPL_INVALID;

        break;
      default:
        OPL_DAL_PRINTF(("no such pkt type.\n"));  
        return OPL_ERR_INVALID_PARAMETERS;
    }

    if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
      /*delete old item.*/
      ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
      iStatus = dalClsEntryMove(&ClassTemp, &classRuleID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure MOVE Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      /*add new item.*/
      ClassTemp = ClassParameter;
      /* if(ClassTemp.FieldMap.bitMap) */ {
        iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
        if (OPL_OK != iStatus) {
          /**/
          ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
          if (OPL_OK == dalClsEntrySet(&ClassTemp, &classRuleID,&classMaskID)) {
            gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
          }
          OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
          return iStatus;
        }
      }
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaDestMAC = OPL_DISABLE;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.DestMac[0] = OPL_INVALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.DestMac[1] = OPL_INVALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.DestMac[2] = OPL_INVALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.DestMac[3] = OPL_INVALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.DestMac[4] = OPL_INVALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.DestMac[5] = OPL_INVALID;
        break;
      case CLASSIFY_PPPOE_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaDestMAC = OPL_DISABLE;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.DestMac[0] = OPL_INVALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.DestMac[1] = OPL_INVALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.DestMac[2] = OPL_INVALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.DestMac[3] = OPL_INVALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.DestMac[4] = OPL_INVALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.DestMac[5] = OPL_INVALID;
        break;
      default:
        OPL_DAL_PRINTF(("no such pkt type.\n"));  
        return OPL_ERR_INVALID_PARAMETERS;
    }
    gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
  }
  return OPL_OK;
}

/*******************************************************************************
* dalClsSrcMacAddrCfg - Configurethe source MAC address field of current 
*             classification.
*
* DESCRIPTION:
*       If the input parameter enable=1, this function will enable the source MAC 
*   address field mask and write MAC value into the rule entry.
*
*   If the input parameter enable=0, this function will disable the source MAC
*   address field mask and clear its value from the rule entry.
*
* INPUT:
*   classItemID   - classify item index. 1~511 is valid.
*   pktType     - Packet protocol type. 1:PPPoE, 2:EtherNet.
*   pSrcMac     - Source MAC address value. Input with string format.
*   pUsrMask    - MAC address mask. Input with string format 
*   bEnable     - 0:disable, 1:enable
*
* OUTPUT:
*       None.
*
* RETURN:
*   An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsSrcMacAddrCfg(UINT16 classItemID,
                               UINT8 pktType,
                               INT8 *pSrcMac,
                               INT8 *pUsrMask,
                               UINT8 bEnable)
{
  CLASSIFY_PARAMETER_t ClassTemp;

  CLASSIFY_PARAMETER_t ClassParameter;
  UINT16 classRuleID;
  OPL_STATUS iStatus;
  opl_cls_hwtbl_t  classMaskID;

  UINT8 Mac[6] = {
    0, 0, 0, 0, 0, 0
  };
  UINT8 UsrMask[6] = {
    0, 0, 0, 0, 0, 0
  };

  if (OPL_ZERO == classItemID || TAB_CLS_RULE_LEN <= classItemID) {
    OPL_DAL_PRINTF(("this chip can only support %d rules,and the 0 items is not valid.\n",
                    TAB_CLS_RULE_LEN));
    return OPL_ERR_INVALID_PARAMETERS;
  } 

  if (OPL_NULL == pSrcMac || OPL_NULL == pUsrMask) {
    OPL_DAL_PRINTF(("Null pointer input.\n"));
    return OPL_ERR_NULL_POINTER;
  }

  string2mac(pSrcMac, Mac);
  string2mac(pUsrMask, UsrMask);

  OPL_BZERO(&ClassParameter, sizeof(CLASSIFY_PARAMETER_t));
  if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
    ClassParameter = gClassifyItem[classItemID - 1].Parameter;
  }

  if (OPL_ENABLE == bEnable) {
    switch (pktType) {
      case CLASSIFY_EII_IP:
        ClassParameter.FieldMap.EIIField.bfEnaSrcMAC = 1;
        ClassParameter.PktRuleValue.EIIValue.SrcMac[0] = Mac[0];
        ClassParameter.PktRuleValue.EIIValue.SrcMac[1] = Mac[1];
        ClassParameter.PktRuleValue.EIIValue.SrcMac[2] = Mac[2];
        ClassParameter.PktRuleValue.EIIValue.SrcMac[3] = Mac[3];
        ClassParameter.PktRuleValue.EIIValue.SrcMac[4] = Mac[4];
        ClassParameter.PktRuleValue.EIIValue.SrcMac[5] = Mac[5];

        ClassParameter.PktUsrMask.EIIValue.SrcMac[0] = UsrMask[0];
        ClassParameter.PktUsrMask.EIIValue.SrcMac[1] = UsrMask[1];
        ClassParameter.PktUsrMask.EIIValue.SrcMac[2] = UsrMask[2];
        ClassParameter.PktUsrMask.EIIValue.SrcMac[3] = UsrMask[3];
        ClassParameter.PktUsrMask.EIIValue.SrcMac[4] = UsrMask[4];
        ClassParameter.PktUsrMask.EIIValue.SrcMac[5] = UsrMask[5];

        break;
      case CLASSIFY_PPPOE_IP:
        ClassParameter.FieldMap.PppoEField.bfEnaSrcMAC = 1;
        ClassParameter.PktRuleValue.PPPoeValue.SrcMac[0] = Mac[0];
        ClassParameter.PktRuleValue.PPPoeValue.SrcMac[1] = Mac[1];
        ClassParameter.PktRuleValue.PPPoeValue.SrcMac[2] = Mac[2];
        ClassParameter.PktRuleValue.PPPoeValue.SrcMac[3] = Mac[3];
        ClassParameter.PktRuleValue.PPPoeValue.SrcMac[4] = Mac[4];
        ClassParameter.PktRuleValue.PPPoeValue.SrcMac[5] = Mac[5];

        ClassParameter.PktUsrMask.PPPoeValue.SrcMac[0] = UsrMask[0];
        ClassParameter.PktUsrMask.PPPoeValue.SrcMac[1] = UsrMask[1];
        ClassParameter.PktUsrMask.PPPoeValue.SrcMac[2] = UsrMask[2];
        ClassParameter.PktUsrMask.PPPoeValue.SrcMac[3] = UsrMask[3];
        ClassParameter.PktUsrMask.PPPoeValue.SrcMac[4] = UsrMask[4];
        ClassParameter.PktUsrMask.PPPoeValue.SrcMac[5] = UsrMask[5];

        break;
      default:
        OPL_DAL_PRINTF(("pkt type input error.\n"));
        return OPL_ERR_NULL_POINTER;
    }


    if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
      /*delete old item.*/
      ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
      iStatus = dalClsEntryMove(&ClassTemp, &classRuleID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure MOVE Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      /*add new item.*/
      ClassTemp = ClassParameter;
      iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
      if (OPL_OK != iStatus) {
        /**/
        ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
        if (OPL_OK == dalClsEntrySet(&ClassTemp, &classRuleID,&classMaskID)) {
          gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
          iStatus = OPL_OK;
        }
        OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }
    }
    else {
      ClassParameter.pktType = pktType;

      /*add new item.*/
      ClassTemp = ClassParameter;
      iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      gClassifyItem[classItemID - 1].valid = OPL_VALID;
      gClassifyItem[classItemID - 1].Parameter.pktType = pktType;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaSrcMAC = OPL_VALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.SrcMac[0] = ClassParameter.PktRuleValue.EIIValue.SrcMac[0];
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.SrcMac[1] = ClassParameter.PktRuleValue.EIIValue.SrcMac[1];
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.SrcMac[2] = ClassParameter.PktRuleValue.EIIValue.SrcMac[2];
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.SrcMac[3] = ClassParameter.PktRuleValue.EIIValue.SrcMac[3];
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.SrcMac[4] = ClassParameter.PktRuleValue.EIIValue.SrcMac[4];
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.SrcMac[5] = ClassParameter.PktRuleValue.EIIValue.SrcMac[5];

        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.EIIValue.SrcMac[0] = ClassParameter.PktUsrMask.EIIValue.SrcMac[0];
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.EIIValue.SrcMac[1] = ClassParameter.PktUsrMask.EIIValue.SrcMac[1];
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.EIIValue.SrcMac[2] = ClassParameter.PktUsrMask.EIIValue.SrcMac[2];
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.EIIValue.SrcMac[3] = ClassParameter.PktUsrMask.EIIValue.SrcMac[3];
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.EIIValue.SrcMac[4] = ClassParameter.PktUsrMask.EIIValue.SrcMac[4];
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.EIIValue.SrcMac[5] = ClassParameter.PktUsrMask.EIIValue.SrcMac[5];
        break;
      case CLASSIFY_PPPOE_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaSrcMAC = OPL_VALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.SrcMac[0] = ClassParameter.PktRuleValue.PPPoeValue.SrcMac[0];
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.SrcMac[1] = ClassParameter.PktRuleValue.PPPoeValue.SrcMac[1];
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.SrcMac[2] = ClassParameter.PktRuleValue.PPPoeValue.SrcMac[2];
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.SrcMac[3] = ClassParameter.PktRuleValue.PPPoeValue.SrcMac[3];
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.SrcMac[4] = ClassParameter.PktRuleValue.PPPoeValue.SrcMac[4];
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.SrcMac[5] = ClassParameter.PktRuleValue.PPPoeValue.SrcMac[5];

        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.PPPoeValue.SrcMac[0] = ClassParameter.PktUsrMask.PPPoeValue.SrcMac[0];
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.PPPoeValue.SrcMac[1] = ClassParameter.PktUsrMask.PPPoeValue.SrcMac[1];
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.PPPoeValue.SrcMac[2] = ClassParameter.PktUsrMask.PPPoeValue.SrcMac[2];
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.PPPoeValue.SrcMac[3] = ClassParameter.PktUsrMask.PPPoeValue.SrcMac[3];
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.PPPoeValue.SrcMac[4] = ClassParameter.PktUsrMask.PPPoeValue.SrcMac[4];
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.PPPoeValue.SrcMac[5] = ClassParameter.PktUsrMask.PPPoeValue.SrcMac[5];
        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));      
        return OPL_ERR_INVALID_PARAMETERS;
    }
    gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
  }
  else {
    if (OPL_INVALID == gClassifyItem[classItemID - 1].valid) {
      OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
      return OPL_OK;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        if ((OPL_INVALID == gClassifyItem[classItemID - 1].valid)
         || (OPL_DISABLE
          == gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaSrcMAC)) {
          OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
          return OPL_OK;
        }
        break;
      case CLASSIFY_PPPOE_IP:
        if ((OPL_INVALID == gClassifyItem[classItemID - 1].valid)
         || (OPL_DISABLE
          == gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaSrcMAC)) {
          OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
          return OPL_OK;
        }
        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        ClassParameter.FieldMap.EIIField.bfEnaSrcMAC = OPL_DISABLE;
        ClassParameter.PktRuleValue.EIIValue.SrcMac[0] = OPL_INVALID;
        ClassParameter.PktRuleValue.EIIValue.SrcMac[1] = OPL_INVALID;
        ClassParameter.PktRuleValue.EIIValue.SrcMac[2] = OPL_INVALID;
        ClassParameter.PktRuleValue.EIIValue.SrcMac[3] = OPL_INVALID;
        ClassParameter.PktRuleValue.EIIValue.SrcMac[4] = OPL_INVALID;
        ClassParameter.PktRuleValue.EIIValue.SrcMac[5] = OPL_INVALID;
        break;
      case CLASSIFY_PPPOE_IP:
        ClassParameter.FieldMap.PppoEField.bfEnaSrcMAC = OPL_DISABLE;
        ClassParameter.PktRuleValue.PPPoeValue.SrcMac[0] = OPL_INVALID;
        ClassParameter.PktRuleValue.PPPoeValue.SrcMac[1] = OPL_INVALID;
        ClassParameter.PktRuleValue.PPPoeValue.SrcMac[2] = OPL_INVALID;
        ClassParameter.PktRuleValue.PPPoeValue.SrcMac[3] = OPL_INVALID;
        ClassParameter.PktRuleValue.PPPoeValue.SrcMac[4] = OPL_INVALID;
        ClassParameter.PktRuleValue.PPPoeValue.SrcMac[5] = OPL_INVALID;

        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }

    if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
      /*delete old item.*/
      ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
      iStatus = dalClsEntryMove(&ClassTemp, &classRuleID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure MOVE Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      /*add new item.*/
      ClassTemp = ClassParameter;
      /* if(ClassTemp.FieldMap.bitMap) */ {
        iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
        if (OPL_OK != iStatus) {
          /**/
          ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
          if (OPL_OK == dalClsEntrySet(&ClassTemp, &classRuleID,&classMaskID)) {
            gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
          }
          OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
          return iStatus;
        }
      }
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaSrcMAC = OPL_DISABLE;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.SrcMac[0] = OPL_INVALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.SrcMac[1] = OPL_INVALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.SrcMac[2] = OPL_INVALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.SrcMac[3] = OPL_INVALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.SrcMac[4] = OPL_INVALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.SrcMac[5] = OPL_INVALID;
        break;
      case CLASSIFY_PPPOE_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaSrcMAC = OPL_DISABLE;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.SrcMac[0] = OPL_INVALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.SrcMac[1] = OPL_INVALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.SrcMac[2] = OPL_INVALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.SrcMac[3] = OPL_INVALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.SrcMac[4] = OPL_INVALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.SrcMac[5] = OPL_INVALID;

        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }
    gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
  }
  return OPL_OK;
}

/*******************************************************************************
* dalClsFirstCosCfg - Configure the first cos field of current classification.
*
* DESCRIPTION:
*       If the input parameter enable=1, this function will enable the first cos 
*   field mask and write cos value into the rule entry.
*
*   If the input parameter enable=0, this function will disable the first cos
*   field mask and clear its value from the rule entry.
*
* INPUT:
*   classItemID   - classify item index. 1~511 is valid.
*   pktType     - Packet protocol type. 1:PPPoE, 2:EtherNet.
*   cosVal      - VLAN cos value.
*   usrMask     - VLAN cos user mask. 
*   bEnable     - 0:disable, 1:enable
*
* OUTPUT:
*       None.
*
* RETURN:
*   An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsFirstCosCfg(UINT16 classItemID,
                             UINT8 pktType,
                             UINT8 cosVal,
                             UINT8 usrMask,
                             UINT8 bEnable)
{
  CLASSIFY_PARAMETER_t ClassTemp;
  CLASSIFY_PARAMETER_t ClassParameter;
  UINT16 classRuleID;
  OPL_STATUS iStatus;
  opl_cls_hwtbl_t  classMaskID;

  if (OPL_ZERO == classItemID || TAB_CLS_RULE_LEN <= classItemID) {
    OPL_DAL_PRINTF(("this chip can only support %d rules,and the 0 items is not valid.\n",
                    TAB_CLS_RULE_LEN));
    return OPL_ERR_INVALID_PARAMETERS;
  } 

  OPL_BZERO(&ClassParameter, sizeof(CLASSIFY_PARAMETER_t));
  if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
    ClassParameter = gClassifyItem[classItemID - 1].Parameter;
  }

  if (OPL_ENABLE == bEnable) {
    switch (pktType) {
      case CLASSIFY_EII_IP:
        ClassParameter.FieldMap.EIIField.bfEnaFirstCOS = OPL_ENABLE;
        ClassParameter.PktRuleValue.EIIValue.bfFirstCos = cosVal;
        ClassParameter.PktUsrMask.EIIValue.bfFirstCos = usrMask;

        break;
      case CLASSIFY_PPPOE_IP:
        ClassParameter.FieldMap.PppoEField.bfEnaFirstCOS = OPL_ENABLE;
        ClassParameter.PktRuleValue.PPPoeValue.bfFirstCos = cosVal;
        ClassParameter.PktUsrMask.PPPoeValue.bfFirstCos = usrMask;
        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }


    if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
      /*delete old item.*/
      ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
      iStatus = dalClsEntryMove(&ClassTemp, &classRuleID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure MOVE Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      /*add new item.*/
      ClassTemp = ClassParameter;
      iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
      if (OPL_OK != iStatus) {
        /**/
        ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
        if (OPL_OK == dalClsEntrySet(&ClassTemp, &classRuleID,&classMaskID)) {
          gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
        }
        OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }
    }
    else {
      ClassParameter.pktType = pktType;

      /*add new item.*/
      ClassTemp = ClassParameter;
      iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      gClassifyItem[classItemID - 1].valid = OPL_VALID;
      gClassifyItem[classItemID - 1].Parameter.pktType = pktType;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaFirstCOS = OPL_ENABLE;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.bfFirstCos = ClassParameter.PktRuleValue.EIIValue.bfFirstCos;
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.EIIValue.bfFirstCos = ClassParameter.PktUsrMask.EIIValue.bfFirstCos;

        break;
      case CLASSIFY_PPPOE_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaFirstCOS = OPL_ENABLE;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.bfFirstCos = ClassParameter.PktRuleValue.PPPoeValue.bfFirstCos;
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.PPPoeValue.bfFirstCos = ClassParameter.PktUsrMask.PPPoeValue.bfFirstCos;

        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }
    gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
  }
  else {
    if (OPL_INVALID == gClassifyItem[classItemID - 1].valid) {
      OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
      return OPL_OK;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        if ((OPL_INVALID == gClassifyItem[classItemID - 1].valid)
         || (OPL_DISABLE
          == gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaFirstCOS)) {
          OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
          return OPL_OK;
        }
        break;
      case CLASSIFY_PPPOE_IP:
        if ((OPL_INVALID == gClassifyItem[classItemID - 1].valid)
         || (OPL_DISABLE
          == gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaFirstCOS)) {
          OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
          return OPL_OK;
        }
        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        ClassParameter.FieldMap.EIIField.bfEnaFirstCOS = OPL_DISABLE;
        ClassParameter.PktRuleValue.EIIValue.bfFirstCos = OPL_INVALID;
        break;
      case CLASSIFY_PPPOE_IP:
        ClassParameter.FieldMap.PppoEField.bfEnaFirstCOS = OPL_DISABLE;
        ClassParameter.PktRuleValue.PPPoeValue.bfFirstCos = OPL_INVALID;
        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }

    if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
      /*delete old item.*/
      ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
      iStatus = dalClsEntryMove(&ClassTemp, &classRuleID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure MOVE Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      /*add new item.*/
      ClassTemp = ClassParameter;
      /* if(ClassTemp.FieldMap.bitMap) */ {
        iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
        if (OPL_OK != iStatus) {
          /**/
          ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
          if (OPL_OK == dalClsEntrySet(&ClassTemp, &classRuleID,&classMaskID)) {
            gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
          }
          OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
          return iStatus;
        }
      }
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaFirstCOS = OPL_DISABLE;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.bfFirstCos = OPL_INVALID;
        break;
      case CLASSIFY_PPPOE_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaFirstCOS = OPL_DISABLE;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.bfFirstCos = OPL_INVALID;

        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }
    gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
  }
  return OPL_OK;
}

/*******************************************************************************
* dalClsFirstVidCfg - Configure the first VLAN ID field of current classification.
*
* DESCRIPTION:
*       If the input parameter enable=1, this function will enable the first VLAN ID 
*   field mask and write VLAN ID value into the rule entry.
*
*   If the input parameter enable=0, this function will disable the first VLAN ID
*   field mask and clear its value from the rule entry.
*
* INPUT:
*   classItemID   - classify item index. 1~511 is valid.
*   pktType     - Packet protocol type. 1:PPPoE, 2:EtherNet.
*   vlanID      - VLAN ID value.
*   usrMask     - VLAN ID user mask. 
*   bEnable     - 0:disable, 1:enable
*
* OUTPUT:
*       None.
*
* RETURN:
*   An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsFirstVidCfg(UINT16 classItemID,
                             UINT8 pktType,
                             UINT16 vlanID,
                             UINT16 usrMask,
                             UINT8 bEnable)
{
  CLASSIFY_PARAMETER_t ClassTemp;

  CLASSIFY_PARAMETER_t ClassParameter;
  UINT16 classRuleID;
  OPL_STATUS iStatus;
  opl_cls_hwtbl_t  classMaskID;

  if (OPL_ZERO == classItemID || TAB_CLS_RULE_LEN <= classItemID) {
    OPL_DAL_PRINTF(("this chip can only support %d rules,and the 0 items is not valid.\n",
                    TAB_CLS_RULE_LEN));
    return OPL_ERR_INVALID_PARAMETERS;
  } 

  OPL_BZERO(&ClassParameter, sizeof(CLASSIFY_PARAMETER_t));
  if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
    ClassParameter = gClassifyItem[classItemID - 1].Parameter;
  }

  if (OPL_ENABLE == bEnable) {
    switch (pktType) {
      case CLASSIFY_EII_IP:
        ClassParameter.FieldMap.EIIField.bfEnaFirstTag = OPL_ENABLE;
        ClassParameter.PktRuleValue.EIIValue.bfFirstVlan = vlanID;
        ClassParameter.PktUsrMask.EIIValue.bfFirstVlan = usrMask;

        break;
      case CLASSIFY_PPPOE_IP:
        ClassParameter.FieldMap.PppoEField.bfEnaFirstTag = OPL_ENABLE;
        ClassParameter.PktRuleValue.PPPoeValue.bfFirstVlan = vlanID;
        ClassParameter.PktUsrMask.PPPoeValue.bfFirstVlan = usrMask;

        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }


    if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
      /*delete old item.*/
      ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
      iStatus = dalClsEntryMove(&ClassTemp, &classRuleID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure MOVE Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      /*add new item.*/
      ClassTemp = ClassParameter;
      iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
      if (OPL_OK != iStatus) {
        /**/
        ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
        if (OPL_OK == dalClsEntrySet(&ClassTemp, &classRuleID,&classMaskID)) {
          gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
        }
        OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }
    }
    else {
      ClassParameter.pktType = pktType;

      /*add new item.*/
      ClassTemp = ClassParameter;
      iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      gClassifyItem[classItemID - 1].valid = OPL_VALID;
      gClassifyItem[classItemID - 1].Parameter.pktType = pktType;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaFirstTag = OPL_ENABLE;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.bfFirstVlan = ClassParameter.PktRuleValue.EIIValue.bfFirstVlan;
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.EIIValue.bfFirstVlan = ClassParameter.PktUsrMask.EIIValue.bfFirstVlan;
        break;
      case CLASSIFY_PPPOE_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaFirstTag = OPL_ENABLE;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.bfFirstVlan = ClassParameter.PktRuleValue.PPPoeValue.bfFirstVlan;
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.PPPoeValue.bfFirstVlan = ClassParameter.PktUsrMask.PPPoeValue.bfFirstVlan;
        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }
    gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
  }
  else {
    if (OPL_INVALID == gClassifyItem[classItemID - 1].valid) {
      OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
      return OPL_OK;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        if ((OPL_INVALID == gClassifyItem[classItemID - 1].valid)
         || (OPL_DISABLE
          == gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaFirstTag)) {
          OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
          return OPL_OK;
        }
        break;
      case CLASSIFY_PPPOE_IP:
        if ((OPL_INVALID == gClassifyItem[classItemID - 1].valid)
         || (OPL_DISABLE
          == gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaFirstTag)) {
          OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
          return OPL_OK;
        }
        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        ClassParameter.FieldMap.EIIField.bfEnaFirstTag = OPL_DISABLE;
        ClassParameter.PktRuleValue.EIIValue.bfFirstVlan = OPL_INVALID;
        break;
      case CLASSIFY_PPPOE_IP:
        ClassParameter.FieldMap.PppoEField.bfEnaFirstTag = OPL_DISABLE;
        ClassParameter.PktRuleValue.PPPoeValue.bfFirstVlan = OPL_INVALID;
        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }

    if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
      /*delete old item.*/
      ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
      iStatus = dalClsEntryMove(&ClassTemp, &classRuleID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure MOVE Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      /*add new item.*/
      ClassTemp = ClassParameter;
      /* if(ClassTemp.FieldMap.bitMap) */ {
        iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
        if (OPL_OK != iStatus) {
          /**/
          ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
          if (OPL_OK == dalClsEntrySet(&ClassTemp, &classRuleID,&classMaskID)) {
            gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
          }
          OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
          return iStatus;
        }
      }
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaFirstTag = OPL_DISABLE;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.bfFirstVlan = OPL_INVALID;
        break;
      case CLASSIFY_PPPOE_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaFirstTag = OPL_DISABLE;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.bfFirstVlan = OPL_INVALID;

        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }
    gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
  }
  return OPL_OK;
}

/*******************************************************************************
* dalClsTypeLenCfg - Configure the type/length field of current classification.
*
* DESCRIPTION:
*       If the input parameter enable=1, this function will enable the type/length 
*   field mask and write type/length value into the rule entry.
*
*   If the input parameter enable=0, this function will disable the type/length
*   field mask and clear its value from the rule entry.
*
* INPUT:
*   classItemID   - classify item index. 1~511 is valid.
*   pktType     - Packet protocol type. 1:PPPoE, 2:EtherNet.
*   typeLen     - Type/Length value.
*   usrMask     - Layer 4 source port user mask. 
*   bEnable     - 0:disable, 1:enable
*
* OUTPUT:
*       None.
*
* RETURN:
*   An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsTypeLenCfg(UINT16 classItemID,
                            UINT8 pktType,
                            UINT16 typeLen,
                            UINT16 usrMask,
                            UINT8 bEnable)
{
  CLASSIFY_PARAMETER_t ClassTemp;
  CLASSIFY_PARAMETER_t ClassParameter;
  UINT16 classRuleID;
  OPL_STATUS iStatus;
  opl_cls_hwtbl_t  classMaskID;

  if (OPL_ZERO == classItemID || TAB_CLS_RULE_LEN <= classItemID) {
    OPL_DAL_PRINTF(("this chip can only support %d rules,and the 0 items is not valid.\n",
                    TAB_CLS_RULE_LEN));
    return OPL_ERR_INVALID_PARAMETERS;
  } 

  OPL_BZERO(&ClassParameter, sizeof(CLASSIFY_PARAMETER_t));
  if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
    ClassParameter = gClassifyItem[classItemID - 1].Parameter;
  }

  if (OPL_ENABLE == bEnable) {
    switch (pktType) {
      case CLASSIFY_EII_IP:
        ClassParameter.FieldMap.EIIField.bfEnaLenorType = OPL_ENABLE;
        ClassParameter.PktRuleValue.EIIValue.bfLenOrType = typeLen;
        ClassParameter.PktUsrMask.EIIValue.bfLenOrType = usrMask;

        break;
      case CLASSIFY_PPPOE_IP:
        ClassParameter.FieldMap.PppoEField.bfEnaLenorType = OPL_ENABLE;
        ClassParameter.PktRuleValue.PPPoeValue.lenOrType = typeLen;
        ClassParameter.PktUsrMask.PPPoeValue.lenOrType = usrMask;

        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }


    if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
      /*delete old item.*/
      ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
      iStatus = dalClsEntryMove(&ClassTemp, &classRuleID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure MOVE Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      /*add new item.*/
      ClassTemp = ClassParameter;
      iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
      if (OPL_OK != iStatus) {
        /**/
        ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
        if (OPL_OK == dalClsEntrySet(&ClassTemp, &classRuleID,&classMaskID)) {
          gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
          iStatus = OPL_OK;
        }
        OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }
    }
    else {
      ClassParameter.pktType = pktType;

      /*add new item.*/
      ClassTemp = ClassParameter;
      iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      gClassifyItem[classItemID - 1].valid = OPL_VALID;
      gClassifyItem[classItemID - 1].Parameter.pktType = pktType;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaLenorType = OPL_ENABLE;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.bfLenOrType = ClassParameter.PktRuleValue.EIIValue.bfLenOrType;
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.EIIValue.bfLenOrType = ClassParameter.PktUsrMask.EIIValue.bfLenOrType;

        break;
      case CLASSIFY_PPPOE_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaLenorType = OPL_ENABLE;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.lenOrType = ClassParameter.PktRuleValue.PPPoeValue.lenOrType;
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.PPPoeValue.lenOrType = ClassParameter.PktUsrMask.PPPoeValue.lenOrType;

        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }
    gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
  }
  else {
    if (OPL_INVALID == gClassifyItem[classItemID - 1].valid) {
      OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
      return OPL_OK;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        if ((OPL_INVALID == gClassifyItem[classItemID - 1].valid)
         || (OPL_DISABLE
          == gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaLenorType)) {
          OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
          return OPL_OK;
        }
        break;
      case CLASSIFY_PPPOE_IP:
        if ((OPL_INVALID == gClassifyItem[classItemID - 1].valid)
         || (OPL_DISABLE
          == gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaLenorType)) {
          OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
          return  OPL_OK;
        }
        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        ClassParameter.FieldMap.EIIField.bfEnaLenorType = OPL_DISABLE;
        ClassParameter.PktRuleValue.EIIValue.bfLenOrType = OPL_INVALID;
        break;
      case CLASSIFY_PPPOE_IP:
        ClassParameter.FieldMap.PppoEField.bfEnaLenorType = OPL_DISABLE;
        ClassParameter.PktRuleValue.PPPoeValue.lenOrType = OPL_INVALID;
        break;
      default:
        OPL_DAL_PRINTF(("in valid pkt type.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }

    if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
      /*delete old item.*/
      ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
      iStatus = dalClsEntryMove(&ClassTemp, &classRuleID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure MOVE Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      /*add new item.*/
      ClassTemp = ClassParameter;
      /* if(ClassTemp.FieldMap.bitMap) */ {
        iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
        if (OPL_OK != iStatus) {
          /**/
          ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
          if (OPL_OK == dalClsEntrySet(&ClassTemp, &classRuleID,&classMaskID)) {
            gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
          }
          OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
          return iStatus;
        }
      }
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaLenorType = OPL_DISABLE;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.bfLenOrType = OPL_INVALID;
        break;
      case CLASSIFY_PPPOE_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaLenorType = OPL_DISABLE;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.lenOrType = OPL_INVALID;

        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }
    gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
  }
  return  OPL_OK;
}

/*******************************************************************************
*
* dalClsDestIpCfg - Configure the destination IP address field of current classification.
*
* DESCRIPTION:
*       If the input parameter enable=1, this function will enable the destination IP 
*   address field mask and write IP value into the rule entry.
*
*   If the input parameter enable=0, this function will disable the destination IP
*   address field mask and clear its value from the rule entry.
*
* INPUT:
*   classItemID   - classify item index. 1~511 is valid.
*   pktType     - Packet protocol type. 1:PPPoE, 2:EtherNet.
*   pDestIp     - Destination IP address. Input in string format.
*   pUserMask   - Destination IP mask. Input in string format.
*   bEnable     - 0:disable, 1:enable
*
* OUTPUT:
*       None.
*
* RETURN:
*   An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsDestIpCfg(UINT16 classItemID,
                           UINT8 pktType,
                           INT8 *pDestIp,
                           INT8 *pUsrMask,
                           UINT8 bEnable)
{
  CLASSIFY_PARAMETER_t ClassTemp;
  CLASSIFY_PARAMETER_t ClassParameter;
  UINT16 classRuleID;
  OPL_STATUS iStatus;
  opl_cls_hwtbl_t  classMaskID;

  if (OPL_ZERO == classItemID || TAB_CLS_RULE_LEN <= classItemID) {
    OPL_DAL_PRINTF(("this chip can only support %d rules,and the 0 items is not valid.\n",
                    TAB_CLS_RULE_LEN));
    return OPL_ERR_INVALID_PARAMETERS;
  } 

  OPL_BZERO(&ClassParameter, sizeof(CLASSIFY_PARAMETER_t));
  if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
    ClassParameter = gClassifyItem[classItemID - 1].Parameter;
  }

  if (OPL_ENABLE == bEnable) {
    switch (pktType) {
      case CLASSIFY_EII_IP:
        ClassParameter.FieldMap.EIIField.bfEnaDestIP = OPL_ENABLE;
        ClassParameter.PktRuleValue.EIIValue.destIP = string2Ip(pDestIp);
        ClassParameter.PktUsrMask.EIIValue.destIP = string2Ip(pUsrMask);
        OPL_DAL_PRINTF(("Destination IP: 0x%x\r\n",
                        ClassParameter.PktRuleValue.EIIValue.destIP));
        OPL_DAL_PRINTF(("User mask: 0x%x\r\n",
                        ClassParameter.PktUsrMask.EIIValue.destIP));

        break;
      case CLASSIFY_PPPOE_IP:
        ClassParameter.FieldMap.PppoEField.bfEnaDestIP = OPL_ENABLE;
        ClassParameter.PktRuleValue.PPPoeValue.destIP = string2Ip(pDestIp);
        ClassParameter.PktUsrMask.PPPoeValue.destIP = string2Ip(pUsrMask);
        OPL_DAL_PRINTF(("Destination IP: 0x%x\r\n",
                        ClassParameter.PktRuleValue.EIIValue.destIP));
        OPL_DAL_PRINTF(("User mask: 0x%x\r\n",
                        ClassParameter.PktUsrMask.PPPoeValue.destIP));

        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }

    OPL_DAL_PRINTF(("0x%x\r\n", ClassParameter.PktRuleValue.EIIValue.destIP));
    if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
      /*delete old item.*/
      ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
      iStatus = dalClsEntryMove(&ClassTemp, &classRuleID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure MOVE Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      /*add new item.*/
      ClassTemp = ClassParameter;
      iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
      if (OPL_OK != iStatus) {
        /**/
        ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
        if (OPL_OK == dalClsEntrySet(&ClassTemp, &classRuleID,&classMaskID)) {
          gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
          iStatus = OPL_OK;
        }
        OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }
    }
    else {
      ClassParameter.pktType = pktType;

      /*add new item.*/
      ClassTemp = ClassParameter;
      iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      gClassifyItem[classItemID - 1].valid = OPL_VALID;
      gClassifyItem[classItemID - 1].Parameter.pktType = pktType;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaDestIP = OPL_VALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.destIP = ClassParameter.PktRuleValue.EIIValue.destIP;
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.EIIValue.destIP = ClassParameter.PktUsrMask.EIIValue.destIP;
        break;
      case CLASSIFY_PPPOE_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaDestIP = 1;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.destIP = ClassParameter.PktRuleValue.PPPoeValue.destIP;
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.PPPoeValue.destIP = ClassParameter.PktUsrMask.PPPoeValue.destIP;
        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }
    gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
  }
  else {
    if (OPL_INVALID == gClassifyItem[classItemID - 1].valid) {
      OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
      return OPL_OK;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        if ((OPL_INVALID == gClassifyItem[classItemID - 1].valid)
         || (OPL_DISABLE
          == gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaDestIP)) {
          OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
          return OPL_OK;
        }
        break;
      case CLASSIFY_PPPOE_IP:
        if ((OPL_INVALID == gClassifyItem[classItemID - 1].valid)
         || (OPL_DISABLE
          == gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaDestIP)) {
          OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
          return OPL_OK;
        }
        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        ClassParameter.FieldMap.EIIField.bfEnaDestIP = OPL_INVALID;
        ClassParameter.PktRuleValue.EIIValue.destIP = OPL_ZERO;
        break;
      case CLASSIFY_PPPOE_IP:
        ClassParameter.FieldMap.PppoEField.bfEnaDestIP = OPL_INVALID;
        ClassParameter.PktRuleValue.PPPoeValue.destIP = OPL_ZERO;
        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }

    if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
      /*delete old item.*/
      ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
      iStatus = dalClsEntryMove(&ClassTemp, &classRuleID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure MOVE Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      /*add new item.*/
      ClassTemp = ClassParameter;
      /* if(ClassTemp.FieldMap.bitMap) */ {
        iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
        if (OPL_OK != iStatus) {
          /**/
          ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
          if (OPL_OK == dalClsEntrySet(&ClassTemp, &classRuleID,&classMaskID)) {
            gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
          }
          OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
          return iStatus;
        }
      }
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaDestIP = OPL_INVALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.destIP = OPL_ZERO;
        break;
      case CLASSIFY_PPPOE_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaDestIP = OPL_INVALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.destIP = OPL_ZERO;

        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }
    gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
  }
  return OPL_OK;
}

/*******************************************************************************
* dalClsSrcIpCfg - Configure the source IP address field of current classification.
*
* DESCRIPTION:
*       If the input parameter enable=1, this function will enable the source IP 
*   address field mask and write IP value into the rule entry.
*
*   If the input parameter enable=0, this function will disable the source IP
*   address field mask and clear its value from the rule entry.
*
* INPUT:
*   classItemID   - classify item index. 1~511 is valid.
*   pktType     - Packet protocol type. 1:PPPoE, 2:EtherNet.
*   pSrcIp      - Source IP address. Input in string format.
*   pUserMask   - Destination IP mask. Input in string format.
*   bEnable     - 0:disable, 1:enable
*
* OUTPUT:
*       None.
*
* RETURN:
*   An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsSrcIpCfg(UINT16 classItemID,
                          UINT8 pktType,
                          INT8 *pSrcIp,
                          INT8 *pUsrMask,
                          UINT8 bEnable)
{
  CLASSIFY_PARAMETER_t ClassTemp;
  CLASSIFY_PARAMETER_t ClassParameter;
  UINT16 classRuleID;
  OPL_STATUS iStatus;
 opl_cls_hwtbl_t  classMaskID;

  if (OPL_ZERO == classItemID || TAB_CLS_RULE_LEN <= classItemID) {
    OPL_DAL_PRINTF(("this chip can only support %d rules,and the 0 items is not valid.\n",
                    TAB_CLS_RULE_LEN));
    return OPL_ERR_INVALID_PARAMETERS;
  } 

  OPL_BZERO(&ClassParameter, sizeof(CLASSIFY_PARAMETER_t));
  if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
    ClassParameter = gClassifyItem[classItemID - 1].Parameter;
  }

  if (OPL_ENABLE == bEnable) {
    switch (pktType) {
      case CLASSIFY_EII_IP:
        ClassParameter.FieldMap.EIIField.bfEnaSrcIP = OPL_ENABLE;
        ClassParameter.PktRuleValue.EIIValue.srcIP = string2Ip(pSrcIp);
        ClassParameter.PktUsrMask.EIIValue.srcIP = string2Ip(pUsrMask);
        OPL_DAL_PRINTF(("Source IP: 0x%x\r\n",
                        ClassParameter.PktRuleValue.EIIValue.srcIP));
        OPL_DAL_PRINTF(("User mask: 0x%x\r\n",
                        ClassParameter.PktUsrMask.EIIValue.srcIP));

        break;
      case CLASSIFY_PPPOE_IP:
        ClassParameter.FieldMap.PppoEField.bfEnaSrcIP = OPL_ENABLE;
        ClassParameter.PktRuleValue.PPPoeValue.srcIP = string2Ip(pSrcIp);
        ClassParameter.PktUsrMask.PPPoeValue.srcIP = string2Ip(pUsrMask);
        OPL_DAL_PRINTF(("Source IP: 0x%x\r\n",
                        ClassParameter.PktRuleValue.EIIValue.srcIP));
        OPL_DAL_PRINTF(("User mask: 0x%x\r\n",
                        ClassParameter.PktUsrMask.PPPoeValue.srcIP));

        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }

    OPL_DAL_PRINTF(("0x%x\r\n", ClassParameter.PktRuleValue.EIIValue.srcIP));
    if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
      /*delete old item.*/
      ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
      iStatus = dalClsEntryMove(&ClassTemp, &classRuleID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure MOVE Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      /*add new item.*/
      ClassTemp = ClassParameter;
      iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
      if (OPL_OK != iStatus) {
        /**/
        ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
        if (OPL_OK == dalClsEntrySet(&ClassTemp, &classRuleID,&classMaskID)) {
          gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
          iStatus = OPL_OK;
        }
        OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }
    }
    else {
      ClassParameter.pktType = pktType;

      /*add new item.*/
      ClassTemp = ClassParameter;
      iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      gClassifyItem[classItemID - 1].valid = OPL_VALID;
      gClassifyItem[classItemID - 1].Parameter.pktType = pktType;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaSrcIP = OPL_VALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.srcIP = ClassParameter.PktRuleValue.EIIValue.srcIP;
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.EIIValue.srcIP = ClassParameter.PktUsrMask.EIIValue.srcIP;
        break;
      case CLASSIFY_PPPOE_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaSrcIP = OPL_VALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.srcIP = ClassParameter.PktRuleValue.PPPoeValue.srcIP;
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.PPPoeValue.srcIP = ClassParameter.PktUsrMask.PPPoeValue.srcIP;
        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }
    gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
  }
  else {
    if (OPL_INVALID == gClassifyItem[classItemID - 1].valid) {
      OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
      return OPL_OK;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        if ((OPL_INVALID == gClassifyItem[classItemID - 1].valid)
         || (OPL_DISABLE
          == gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaSrcIP)) {
          OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
          return OPL_OK;
        }
        break;
      case CLASSIFY_PPPOE_IP:
        if ((OPL_INVALID == gClassifyItem[classItemID - 1].valid)
         || (OPL_DISABLE
          == gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaSrcIP)) {
          OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
          return OPL_OK;
        }
        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        ClassParameter.FieldMap.EIIField.bfEnaSrcIP = OPL_INVALID;
        ClassParameter.PktRuleValue.EIIValue.srcIP = OPL_ZERO;
        break;
      case CLASSIFY_PPPOE_IP:
        ClassParameter.FieldMap.PppoEField.bfEnaSrcIP = OPL_INVALID;
        ClassParameter.PktRuleValue.PPPoeValue.srcIP = OPL_ZERO;
        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }

    if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
      /*delete old item.*/
      ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
      iStatus = dalClsEntryMove(&ClassTemp, &classRuleID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure MOVE Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      /*add new item.*/
      ClassTemp = ClassParameter;
      /* if(ClassTemp.FieldMap.bitMap) */ {
        iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
        if (OPL_OK != iStatus) {
          /**/
          ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
          if (OPL_OK == dalClsEntrySet(&ClassTemp, &classRuleID,&classMaskID)) {
            gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
          }
          OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
          return iStatus;
        }
      }
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaSrcIP = OPL_INVALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.srcIP = OPL_ZERO;
        break;
      case CLASSIFY_PPPOE_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaSrcIP = OPL_INVALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.srcIP = OPL_ZERO;

        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }
    gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
  }
  return OPL_OK;
}

/*******************************************************************************
* dalClsIpTypeCfg - Configure the IP type field of current classification.
*
* DESCRIPTION:
*       If the input parameter enable=1, this function will enable the IP type
*   field mask and write IP type value into the rule entry.
*
*   If the input parameter enable=0, this function will disable the IP type
*   field mask and clear its value from the rule entry.
*
* INPUT:
*   classItemID   - classify item index. 1~511 is valid.
*   pktType     - Packet protocol type. 1:PPPoE, 2:EtherNet.
*   ipType      - IP type value. 
*   usrMask     - IP type user mask. 
*   bEnable     - 0:disable, 1:enable
*
* OUTPUT:
*       None.
*
* RETURN:
*   An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsIpTypeCfg(UINT16 classItemID,
                           UINT8 pktType,
                           UINT8 ipType,
                           UINT8 usrMask,
                           UINT8 bEnable)
{
  CLASSIFY_PARAMETER_t ClassTemp;
  CLASSIFY_PARAMETER_t ClassParameter;
  UINT16 classRuleID;
  OPL_STATUS iStatus;
  opl_cls_hwtbl_t  classMaskID;

  if (OPL_ZERO == classItemID || TAB_CLS_RULE_LEN <= classItemID) {
    OPL_DAL_PRINTF(("this chip can only support %d rules,and the 0 items is not valid.\n",
                    TAB_CLS_RULE_LEN));
    return OPL_ERR_INVALID_PARAMETERS;
  } 

  OPL_BZERO(&ClassParameter, sizeof(CLASSIFY_PARAMETER_t));
  if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
    ClassParameter = gClassifyItem[classItemID - 1].Parameter;
  }

  if (OPL_ENABLE == bEnable) {
    switch (pktType) {
      case CLASSIFY_EII_IP:
        ClassParameter.FieldMap.EIIField.bfEnaIpProType = OPL_VALID;
        ClassParameter.PktRuleValue.EIIValue.ipProType = ipType;
        ClassParameter.PktUsrMask.EIIValue.ipProType = usrMask;
#ifdef DAL_CLS_DEBUG              
        OPL_DAL_PRINTF(("0x%x\r\n",
                        ClassParameter.PktRuleValue.EIIValue.ipProType));
        OPL_DAL_PRINTF(("0x%x\r\n",
                        ClassParameter.PktUsrMask.EIIValue.ipProType));
#endif         
        break;
      case CLASSIFY_PPPOE_IP:
        ClassParameter.FieldMap.PppoEField.bfEnaIpProType = OPL_VALID;
        ClassParameter.PktRuleValue.PPPoeValue.ipProType = ipType;
        ClassParameter.PktUsrMask.PPPoeValue.ipProType = usrMask;
#ifdef DAL_CLS_DEBUG                  
        OPL_DAL_PRINTF(("0x%x\r\n",
                        ClassParameter.PktRuleValue.PPPoeValue.ipProType));
        OPL_DAL_PRINTF(("0x%x\r\n",
                        ClassParameter.PktUsrMask.PPPoeValue.ipProType));
#endif        
        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }


    if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
      /*delete old item.*/
      ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
      iStatus = dalClsEntryMove(&ClassTemp, &classRuleID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure MOVE Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      /*add new item.*/
      ClassTemp = ClassParameter;
      iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
      if (OPL_OK != iStatus) {
        /**/
        ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
        if (OPL_OK == dalClsEntrySet(&ClassTemp, &classRuleID,&classMaskID)) {
          gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
          iStatus = OPL_OK;
        }
        OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }
    }
    else {
      ClassParameter.pktType = pktType;

      /*add new item.*/
      ClassTemp = ClassParameter;
      iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      gClassifyItem[classItemID - 1].valid = OPL_VALID;
      gClassifyItem[classItemID - 1].Parameter.pktType = pktType;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaIpProType = OPL_VALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.ipProType = ClassParameter.PktRuleValue.EIIValue.ipProType;
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.EIIValue.ipProType = ClassParameter.PktUsrMask.EIIValue.ipProType;

        break;
      case CLASSIFY_PPPOE_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaIpProType = OPL_VALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.ipProType = ClassParameter.PktRuleValue.PPPoeValue.ipProType;
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.PPPoeValue.ipProType = ClassParameter.PktUsrMask.PPPoeValue.ipProType;

        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }
    gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
  }
  else {
    if (OPL_INVALID == gClassifyItem[classItemID - 1].valid) {
      OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
      return OPL_OK;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        if ((OPL_INVALID == gClassifyItem[classItemID - 1].valid)
         || (OPL_DISABLE
          == gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaIpProType)) {
          OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
          return OPL_OK;
        }
        break;
      case CLASSIFY_PPPOE_IP:
        if ((OPL_INVALID == gClassifyItem[classItemID - 1].valid)
         || (OPL_DISABLE
          == gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaIpProType)) {
          OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
          return OPL_OK;
        }
        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        ClassParameter.FieldMap.EIIField.bfEnaIpProType = 0;
        ClassParameter.PktRuleValue.EIIValue.ipProType = 0;
        break;
      case CLASSIFY_PPPOE_IP:
        ClassParameter.FieldMap.PppoEField.bfEnaIpProType = 0;
        ClassParameter.PktRuleValue.PPPoeValue.ipProType = 0;
        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }

    if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
      /*delete old item.*/
      ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
      iStatus = dalClsEntryMove(&ClassTemp, &classRuleID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure MOVE Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      /*add new item.*/
      ClassTemp = ClassParameter;
      /* if(ClassTemp.FieldMap.bitMap) */ {
        iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
        if (OPL_OK != iStatus) {
          /**/
          ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
          if (OPL_OK == dalClsEntrySet(&ClassTemp, &classRuleID,&classMaskID)) {
            gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
          }
          OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
          return iStatus;
        }
      }
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaIpProType = OPL_INVALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.ipProType = OPL_INVALID;
        break;
      case CLASSIFY_PPPOE_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaIpProType = OPL_INVALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.ipProType = OPL_INVALID;
        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }
    gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
  }
  return OPL_OK;
}


/*---add by ltang ------2010-10-15-----start----*/
#if 1
/*******************************************************************************
* dalClsIpTypeCfg - Configure the IP type field of current classification.
*
* DESCRIPTION:
*       If the input parameter enable=1, this function will enable the IP type
*   field mask and write IP type value into the rule entry.
*
*   If the input parameter enable=0, this function will disable the IP type
*   field mask and clear its value from the rule entry.
*
* INPUT:
*   classItemID   - classify item index. 1~511 is valid.
*   pktType     - Packet protocol type. 1:PPPoE, 2:EtherNet.
*   ipType      - IP type value. 
*   usrMask     - IP type user mask. 
*   bEnable     - 0:disable, 1:enable
*
* OUTPUT:
*       None.
*
* RETURN:
*   An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsIpVersionCfg(UINT16 classItemID,
                           UINT8 pktType,
                           UINT8 ipVersion,
                           UINT8 usrMask,
                           UINT8 bEnable)
{
  CLASSIFY_PARAMETER_t ClassTemp;
  CLASSIFY_PARAMETER_t ClassParameter;
  UINT16 classRuleID;
  OPL_STATUS iStatus;
  opl_cls_hwtbl_t  classMaskID;

  if (OPL_ZERO == classItemID || TAB_CLS_RULE_LEN <= classItemID) {
    OPL_DAL_PRINTF(("this chip can only support %d rules,and the 0 items is not valid.\n",
                    TAB_CLS_RULE_LEN));
    return OPL_ERR_INVALID_PARAMETERS;
  } 

  OPL_BZERO(&ClassParameter, sizeof(CLASSIFY_PARAMETER_t));
  if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
    ClassParameter = gClassifyItem[classItemID - 1].Parameter;
  }

  if (OPL_ENABLE == bEnable) {
    switch (pktType) {
      case CLASSIFY_EII_IP:
        ClassParameter.FieldMap.EIIField.bfEnaIpVer = OPL_VALID;
        ClassParameter.PktRuleValue.EIIValue.bfIpVersion = ipVersion;
        ClassParameter.PktUsrMask.EIIValue.bfIpVersion = usrMask;
#ifdef DAL_CLS_DEBUG              
        OPL_DAL_PRINTF(("0x%x\r\n",
                        ClassParameter.PktRuleValue.EIIValue.bfIpVersion));
        OPL_DAL_PRINTF(("0x%x\r\n",
                        ClassParameter.PktUsrMask.EIIValue.bfIpVersion));
#endif         
        break;
      case CLASSIFY_PPPOE_IP:
        ClassParameter.FieldMap.PppoEField.bfEnaIpVer = OPL_VALID;
        ClassParameter.PktRuleValue.PPPoeValue.bfIpVer_lcpIpcpCodeL = ipVersion;
        ClassParameter.PktUsrMask.PPPoeValue.bfIpVer_lcpIpcpCodeL = usrMask;
#ifdef DAL_CLS_DEBUG                  
        OPL_DAL_PRINTF(("0x%x\r\n",
                        ClassParameter.PktRuleValue.PPPoeValue.bfIpVersion));
        OPL_DAL_PRINTF(("0x%x\r\n",
                        ClassParameter.PktUsrMask.PPPoeValue.bfIpVersion));
#endif        
        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }


    if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
      /*delete old item.*/
      ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
      iStatus = dalClsEntryMove(&ClassTemp, &classRuleID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure MOVE Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      /*add new item.*/
      ClassTemp = ClassParameter;
      iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
      if (OPL_OK != iStatus) {
        /**/
        ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
        if (OPL_OK == dalClsEntrySet(&ClassTemp, &classRuleID,&classMaskID)) {
          gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
          iStatus = OPL_OK;
        }
        OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }
    }
    else {
      ClassParameter.pktType = pktType;

      /*add new item.*/
      ClassTemp = ClassParameter;
      iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      gClassifyItem[classItemID - 1].valid = OPL_VALID;
      gClassifyItem[classItemID - 1].Parameter.pktType = pktType;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaIpVer = OPL_VALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.bfIpVersion = ClassParameter.PktRuleValue.EIIValue.bfIpVersion;
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.EIIValue.bfIpVersion = ClassParameter.PktUsrMask.EIIValue.bfIpVersion;

        break;
      case CLASSIFY_PPPOE_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaIpVer = OPL_VALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.bfIpVer_lcpIpcpCodeL = ClassParameter.PktRuleValue.PPPoeValue.bfIpVer_lcpIpcpCodeL;
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.PPPoeValue.bfIpVer_lcpIpcpCodeL = ClassParameter.PktUsrMask.PPPoeValue.bfIpVer_lcpIpcpCodeL;

        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }
    gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
  }
  else {
    if (OPL_INVALID == gClassifyItem[classItemID - 1].valid) {
      OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
      return OPL_OK;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        if ((OPL_INVALID == gClassifyItem[classItemID - 1].valid)
         || (OPL_DISABLE
          == gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaIpVer)) {
          OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
          return OPL_OK;
        }
        break;
      case CLASSIFY_PPPOE_IP:
        if ((OPL_INVALID == gClassifyItem[classItemID - 1].valid)
         || (OPL_DISABLE
          == gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaIpVer)) {
          OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
          return OPL_OK;
        }
        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        ClassParameter.FieldMap.EIIField.bfEnaIpVer = 0;
        ClassParameter.PktRuleValue.EIIValue.bfIpVersion = 0;
        break;
      case CLASSIFY_PPPOE_IP:
        ClassParameter.FieldMap.PppoEField.bfEnaIpVer = 0;
        ClassParameter.PktRuleValue.PPPoeValue.bfIpVer_lcpIpcpCodeL = 0;
        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }

    if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
      /*delete old item.*/
      ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
      iStatus = dalClsEntryMove(&ClassTemp, &classRuleID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure MOVE Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      /*add new item.*/
      ClassTemp = ClassParameter;
      /* if(ClassTemp.FieldMap.bitMap) */ {
        iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
        if (OPL_OK != iStatus) {
          /**/
          ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
          if (OPL_OK == dalClsEntrySet(&ClassTemp, &classRuleID,&classMaskID)) {
            gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
          }
          OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
          return iStatus;
        }
      }
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaIpVer = OPL_INVALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.bfIpVersion = OPL_INVALID;
        break;
      case CLASSIFY_PPPOE_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaIpVer = OPL_INVALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.bfIpVer_lcpIpcpCodeL = OPL_INVALID;
        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }
    gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
  }
  return OPL_OK;
}




/*******************************************************************************
* dalClsIpTypeCfg - Configure the IP type field of current classification.
*
* DESCRIPTION:
*       If the input parameter enable=1, this function will enable the IP type
*   field mask and write IP type value into the rule entry.
*
*   If the input parameter enable=0, this function will disable the IP type
*   field mask and clear its value from the rule entry.
*
* INPUT:
*   classItemID   - classify item index. 1~511 is valid.
*   pktType     - Packet protocol type. 1:PPPoE, 2:EtherNet.
*   ipType      - IP type value. 
*   usrMask     - IP type user mask. 
*   bEnable     - 0:disable, 1:enable
*
* OUTPUT:
*       None.
*
* RETURN:
*   An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsFirstTpidCfg(UINT16 classItemID,
                           UINT8 pktType,
                           UINT16 firsttpid,
                           UINT16 usrMask,
                           UINT8 bEnable)
{
  CLASSIFY_PARAMETER_t ClassTemp;
  CLASSIFY_PARAMETER_t ClassParameter;
  UINT16 classRuleID;
  OPL_STATUS iStatus;
  opl_cls_hwtbl_t  classMaskID;

  if (OPL_ZERO == classItemID || TAB_CLS_RULE_LEN <= classItemID) {
    OPL_DAL_PRINTF(("this chip can only support %d rules,and the 0 items is not valid.\n",
                    TAB_CLS_RULE_LEN));
    return OPL_ERR_INVALID_PARAMETERS;
  } 

  OPL_BZERO(&ClassParameter, sizeof(CLASSIFY_PARAMETER_t));
  if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
    ClassParameter = gClassifyItem[classItemID - 1].Parameter;
  }

  if (OPL_ENABLE == bEnable) {
    switch (pktType) {
      case CLASSIFY_EII_IP:
        ClassParameter.FieldMap.EIIField.bfEnaFirstTPID = OPL_VALID;
        ClassParameter.PktRuleValue.EIIValue.bfFirstTpid = firsttpid;
        ClassParameter.PktUsrMask.EIIValue.bfFirstTpid = usrMask;
#ifdef DAL_CLS_DEBUG              
        OPL_DAL_PRINTF(("0x%x\r\n",
                        ClassParameter.PktRuleValue.EIIValue.bfFirstTpid));
        OPL_DAL_PRINTF(("0x%x\r\n",
                        ClassParameter.PktUsrMask.EIIValue.bfFirstTpid));
#endif         
        break;
      case CLASSIFY_PPPOE_IP:
        ClassParameter.FieldMap.PppoEField.bfEnaFirstTPID = OPL_VALID;
        ClassParameter.PktRuleValue.PPPoeValue.bfFirstTpid = firsttpid;
        ClassParameter.PktUsrMask.PPPoeValue.bfFirstTpid = usrMask;
#ifdef DAL_CLS_DEBUG                  
        OPL_DAL_PRINTF(("0x%x\r\n",
                        ClassParameter.PktRuleValue.PPPoeValue.bfFirstTpid));
        OPL_DAL_PRINTF(("0x%x\r\n",
                        ClassParameter.PktUsrMask.PPPoeValue.bfFirstTpid));
#endif        
        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }



    if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
      /*delete old item.*/
      ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
      iStatus = dalClsEntryMove(&ClassTemp, &classRuleID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure MOVE Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      /*add new item.*/
      ClassTemp = ClassParameter;
      iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
      if (OPL_OK != iStatus) {
        /**/
        ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
        if (OPL_OK == dalClsEntrySet(&ClassTemp, &classRuleID,&classMaskID)) {
          gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
          iStatus = OPL_OK;
        }
        OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }
    }
    else {
      ClassParameter.pktType = pktType;


	
      /*add new item.*/
      ClassTemp = ClassParameter;
      iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      gClassifyItem[classItemID - 1].valid = OPL_VALID;
      gClassifyItem[classItemID - 1].Parameter.pktType = pktType;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaFirstTPID = OPL_VALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.bfFirstTpid = ClassParameter.PktRuleValue.EIIValue.bfFirstTpid;
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.EIIValue.bfFirstTpid = ClassParameter.PktUsrMask.EIIValue.bfFirstTpid;

        break;
      case CLASSIFY_PPPOE_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaFirstTPID = OPL_VALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.bfFirstTpid = ClassParameter.PktRuleValue.PPPoeValue.bfFirstTpid;
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.PPPoeValue.bfFirstTpid = ClassParameter.PktUsrMask.PPPoeValue.bfFirstTpid;

        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }
    gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
  }
  else {
    if (OPL_INVALID == gClassifyItem[classItemID - 1].valid) {
      OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
      return OPL_OK;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        if ((OPL_INVALID == gClassifyItem[classItemID - 1].valid)
         || (OPL_DISABLE
          == gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaFirstTPID)) {
          OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
          return OPL_OK;
        }
        break;
      case CLASSIFY_PPPOE_IP:
        if ((OPL_INVALID == gClassifyItem[classItemID - 1].valid)
         || (OPL_DISABLE
          == gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaFirstTPID)) {
          OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
          return OPL_OK;
        }
        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        ClassParameter.FieldMap.EIIField.bfEnaFirstTPID = 0;
        ClassParameter.PktRuleValue.EIIValue.bfFirstTpid = 0;
        break;
      case CLASSIFY_PPPOE_IP:
        ClassParameter.FieldMap.PppoEField.bfEnaFirstTPID = 0;
        ClassParameter.PktRuleValue.PPPoeValue.bfFirstTpid = 0;
        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }

    if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
      /*delete old item.*/
      ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
      iStatus = dalClsEntryMove(&ClassTemp, &classRuleID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure MOVE Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      /*add new item.*/
      ClassTemp = ClassParameter;
      /* if(ClassTemp.FieldMap.bitMap) */ {
        iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
        if (OPL_OK != iStatus) {
          /**/
          ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
          if (OPL_OK == dalClsEntrySet(&ClassTemp, &classRuleID,&classMaskID)) {
            gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
          }
          OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
          return iStatus;
        }
      }
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaFirstTPID = OPL_INVALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.bfFirstTpid = OPL_INVALID;
        break;
      case CLASSIFY_PPPOE_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaFirstTPID = OPL_INVALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.bfFirstTpid = OPL_INVALID;
        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }
    gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
  }
  return OPL_OK;
}





#endif 

/*---add by ltang ------2010-10-15-----start----*/
/*******************************************************************************
* dalClsIpTosCfg - Configure the IP tos field of current classification.
*
* DESCRIPTION:
*       If the input parameter enable=1, this function will enable the IP tos
*   field mask and write IP tos value into the rule entry.
*
*   If the input parameter enable=0, this function will disable the IP tos
*   field mask and clear its value from the rule entry.
*
* INPUT:
*   classItemID   - classify item index. 1~511 is valid.
*   pktType     - Packet protocol type. 1:PPPoE, 2:EtherNet.
*   ipTos     - IP tos value. 
*   usrMask     - IP tos user mask. 
*   bEnable     - 0:disable, 1:enable
*
* OUTPUT:
*       None.
*
* RETURN:
*   An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsIpTosCfg(UINT16 classItemID,
                          UINT8 pktType,
                          UINT8 ipTos,
                          UINT8 usrMask,
                          UINT8 bEnable)
{
  CLASSIFY_PARAMETER_t ClassTemp;
  CLASSIFY_PARAMETER_t ClassParameter;
  UINT16 classRuleID;
  OPL_STATUS iStatus;
  opl_cls_hwtbl_t  classMaskID;

  if (OPL_ZERO == classItemID || TAB_CLS_RULE_LEN <= classItemID) {
    OPL_DAL_PRINTF(("this chip can only support %d rules,and the 0 items is not valid.\n",
                    TAB_CLS_RULE_LEN));
    return OPL_ERR_INVALID_PARAMETERS;
  } 

  OPL_BZERO(&ClassParameter, sizeof(CLASSIFY_PARAMETER_t));
  if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
    ClassParameter = gClassifyItem[classItemID - 1].Parameter;
  }

  if (OPL_ENABLE == bEnable) {
    switch (pktType) {
      case CLASSIFY_EII_IP:
        ClassParameter.FieldMap.EIIField.bfEnaIpTos = OPL_VALID;
        ClassParameter.PktRuleValue.EIIValue.bfIpTos = ipTos;
        ClassParameter.PktUsrMask.EIIValue.bfIpTos = usrMask;

        break;
      case CLASSIFY_PPPOE_IP:
        ClassParameter.FieldMap.PppoEField.bfEnaIpTos = OPL_VALID;
        ClassParameter.PktRuleValue.PPPoeValue.ipTos = ipTos;
        ClassParameter.PktUsrMask.PPPoeValue.ipTos = usrMask;

        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }

    if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
      /*delete old item.*/
      ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
      iStatus = dalClsEntryMove(&ClassTemp, &classRuleID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure MOVE Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      /*add new item.*/
      ClassTemp = ClassParameter;
      iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
      if (OPL_OK != iStatus) {
        /**/
        ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
        if (OPL_OK == dalClsEntrySet(&ClassTemp, &classRuleID,&classMaskID)) {
          gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
          iStatus = OPL_OK;
        }
        OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }
    }
    else {
      ClassParameter.pktType = pktType;

      /*add new item.*/
      ClassTemp = ClassParameter;
      iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      gClassifyItem[classItemID - 1].valid = OPL_VALID;
      gClassifyItem[classItemID - 1].Parameter.pktType = pktType;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaIpTos = OPL_VALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.bfIpTos = ClassParameter.PktRuleValue.EIIValue.bfIpTos;
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.EIIValue.bfIpTos = ClassParameter.PktUsrMask.EIIValue.bfIpTos;

        break;
      case CLASSIFY_PPPOE_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaIpTos = OPL_VALID;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.ipTos = ClassParameter.PktRuleValue.PPPoeValue.ipTos;
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.PPPoeValue.ipTos = ClassParameter.PktUsrMask.PPPoeValue.ipTos;

        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }
    gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
  }
  else {
    if (OPL_INVALID == gClassifyItem[classItemID - 1].valid) {
      OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
      return OPL_OK;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        if ((OPL_INVALID == gClassifyItem[classItemID - 1].valid)
         || (OPL_DISABLE
          == gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaIpTos)) {
          OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
          return OPL_OK;
        }
        break;
      case CLASSIFY_PPPOE_IP:
        if ((OPL_INVALID == gClassifyItem[classItemID - 1].valid)
         || (OPL_DISABLE
          == gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaIpTos)) {
          OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
          return OPL_OK;
        }
        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        ClassParameter.FieldMap.EIIField.bfEnaIpTos = OPL_DISABLE;
        ClassParameter.PktRuleValue.EIIValue.bfIpTos = OPL_INVALID;
        break;
      case CLASSIFY_PPPOE_IP:
        ClassParameter.FieldMap.PppoEField.bfEnaIpTos = OPL_DISABLE;
        ClassParameter.PktRuleValue.PPPoeValue.ipTos = OPL_INVALID;
        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }

    if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
      /*delete old item.*/
      ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
      iStatus = dalClsEntryMove(&ClassTemp, &classRuleID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure MOVE Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      /*add new item.*/
      ClassTemp = ClassParameter;
      /* if(ClassTemp.FieldMap.bitMap) */ {
        iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
        if (OPL_OK != iStatus) {
          /**/
          ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
          if (OPL_OK == dalClsEntrySet(&ClassTemp, &classRuleID,&classMaskID)) {
            gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
          }
          OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
          return iStatus;
        }
      }
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaIpTos = OPL_DISABLE;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.bfIpTos = OPL_INVALID;
        break;
      case CLASSIFY_PPPOE_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaIpTos = OPL_DISABLE;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.ipTos = OPL_INVALID;

        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }
    gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
  }
  return OPL_OK;
}

/*******************************************************************************
* dalClsL4DestPortCfg - Configure the layer 4 destination port field of current 
*             classification.
*
* DESCRIPTION:
*       If the input parameter enable=1, this function will enable the layer 4 destination port 
*   field mask and write port value into the rule entry.
*
*   If the input parameter enable=0, this function will disable the layer 4 destination port
*   field mask and clear its value from the rule entry.
*
* INPUT:
*   classItemID   - classify item index. 1~511 is valid.
*   pktType     - Packet protocol type. 1:PPPoE, 2:EtherNet.
*   L4DestPortID  - Layer 4 destination port ID.
*   usrMask     - Layer 4 destination port user mask. 
*   bEnable     - 0:disable, 1:enable
*
* OUTPUT:
*       None.
*
* RETURN:
*   An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsL4DestPortCfg(UINT16 classItemID,
                               UINT8 pktType,
                               UINT16 L4DestPortID,
                               UINT16 usrMask,
                               UINT8 bEnable)
{
  CLASSIFY_PARAMETER_t ClassTemp;
  CLASSIFY_PARAMETER_t ClassParameter;
  UINT16 classRuleID;
  OPL_STATUS iStatus;
  opl_cls_hwtbl_t  classMaskID;

  if (OPL_ZERO == classItemID || TAB_CLS_RULE_LEN <= classItemID) {
    OPL_DAL_PRINTF(("this chip can only support %d rules,and the 0 items is not valid.\n",
                    TAB_CLS_RULE_LEN));
    return OPL_ERR_INVALID_PARAMETERS;
  } 

  OPL_BZERO(&ClassParameter, sizeof(CLASSIFY_PARAMETER_t));
  if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
    ClassParameter = gClassifyItem[classItemID - 1].Parameter;
  }

  if (OPL_ENABLE == bEnable) {
    switch (pktType) {
      case CLASSIFY_EII_IP:
        ClassParameter.FieldMap.EIIField.bfEnaDestPort = OPL_ENABLE;
        ClassParameter.PktRuleValue.EIIValue.destPort = L4DestPortID;
        ClassParameter.PktUsrMask.EIIValue.destPort = usrMask;

        break;
      case CLASSIFY_PPPOE_IP:
        ClassParameter.FieldMap.PppoEField.bfEnaDestPort = OPL_ENABLE;
        ClassParameter.PktRuleValue.PPPoeValue.destPort = L4DestPortID;
        ClassParameter.PktUsrMask.PPPoeValue.destPort = usrMask;

        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }


    if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
      /*delete old item.*/
      ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
      iStatus = dalClsEntryMove(&ClassTemp, &classRuleID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure MOVE Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      /*add new item.*/
      ClassTemp = ClassParameter;
      iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
      if (OPL_OK != iStatus) {
        /**/
        ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
        if (OPL_OK == dalClsEntrySet(&ClassTemp, &classRuleID,&classMaskID)) {
          gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
          iStatus = OPL_OK;
        }
        OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }
    }
    else {
      ClassParameter.pktType = pktType;

      /*add new item.*/
      ClassTemp = ClassParameter;
      iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      gClassifyItem[classItemID - 1].valid = OPL_VALID;
      gClassifyItem[classItemID - 1].Parameter.pktType = pktType;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaDestPort = OPL_ENABLE;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.destPort = ClassParameter.PktRuleValue.EIIValue.destPort;
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.EIIValue.destPort = ClassParameter.PktUsrMask.EIIValue.destPort;

        break;
      case CLASSIFY_PPPOE_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaDestPort = OPL_ENABLE;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.destPort = ClassParameter.PktRuleValue.PPPoeValue.destPort;
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.PPPoeValue.destPort = ClassParameter.PktUsrMask.PPPoeValue.destPort;

        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }
    gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
  }
  else {
    if (OPL_INVALID == gClassifyItem[classItemID - 1].valid) {
      OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
      return OPL_OK;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        if ((OPL_INVALID == gClassifyItem[classItemID - 1].valid)
         || (OPL_DISABLE
          == gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaDestPort)) {
          OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
          return OPL_OK;
        }
        break;
      case CLASSIFY_PPPOE_IP:
        if ((OPL_INVALID == gClassifyItem[classItemID - 1].valid)
         || (OPL_DISABLE
          == gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaDestPort)) {
          OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
          return OPL_OK;
        }
        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        ClassParameter.FieldMap.EIIField.bfEnaDestPort = OPL_DISABLE;
        ClassParameter.PktRuleValue.EIIValue.destPort = OPL_INVALID;

        break;
      case CLASSIFY_PPPOE_IP:
        ClassParameter.FieldMap.PppoEField.bfEnaDestPort = OPL_DISABLE;
        ClassParameter.PktRuleValue.PPPoeValue.destPort = OPL_INVALID;
        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }

    if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
      /*delete old item.*/
      ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
      iStatus = dalClsEntryMove(&ClassTemp, &classRuleID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure MOVE Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      /*add new item.*/
      ClassTemp = ClassParameter;
      /* if(ClassTemp.FieldMap.bitMap) */ {
        iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
        if (OPL_OK != iStatus) {
          /**/
          ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
          if (OPL_OK == dalClsEntrySet(&ClassTemp, &classRuleID,&classMaskID)) {
            gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
          }
          OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
          return iStatus;
        }
      }
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaDestPort = OPL_DISABLE;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.destPort = OPL_INVALID;

        break;
      case CLASSIFY_PPPOE_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaDestPort = OPL_DISABLE;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.destPort = OPL_INVALID;

        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }
    gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
  }
  return OPL_OK;
}

/*******************************************************************************
* dalClsL4SrcPortCfg - Configure the layer 4 source port field of current 
*             classification.
*
* DESCRIPTION:
*       If the input parameter enable=1, this function will enable the layer 4 source port 
*   field mask and write port value into the rule entry.
*
*   If the input parameter enable=0, this function will disable the layer 4 source port
*   field mask and clear its value from the rule entry.
*
* INPUT:
*   classItemID   - classify item index. 1~511 is valid.
*   pktType     - Packet protocol type. 1:PPPoE, 2:EtherNet.
*   L4SrcPortID   - Layer 4 source port ID.
*   usrMask     - Layer 4 source port user mask. 
*   bEnable     - 0:disable, 1:enable
*
* OUTPUT:
*       None.
*
* RETURN:
*   An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsL4SrcPortCfg(UINT16 classItemID,
                              UINT8 pktType,
                              UINT16 L4SrcPortID,
                              UINT16 usrMask,
                              UINT8 bEnable)
{
  CLASSIFY_PARAMETER_t ClassTemp;
  CLASSIFY_PARAMETER_t ClassParameter;
  UINT16 classRuleID;
  OPL_STATUS iStatus;
  opl_cls_hwtbl_t  classMaskID;

  if (OPL_ZERO == classItemID || TAB_CLS_RULE_LEN <= classItemID) {
    OPL_DAL_PRINTF(("this chip can only support %d rules,and the 0 items is not valid.\n",
                    TAB_CLS_RULE_LEN));
    return OPL_ERR_INVALID_PARAMETERS;
  } 

  OPL_BZERO(&ClassParameter, sizeof(CLASSIFY_PARAMETER_t));
  if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
    ClassParameter = gClassifyItem[classItemID - 1].Parameter;
  }

  if (OPL_ENABLE == bEnable) {
    switch (pktType) {
      case CLASSIFY_EII_IP:
        ClassParameter.FieldMap.EIIField.bfEnaSrcPort = OPL_ENABLE;
        ClassParameter.PktRuleValue.EIIValue.srcPort = L4SrcPortID;
        ClassParameter.PktUsrMask.EIIValue.srcPort = usrMask;

        break;
      case CLASSIFY_PPPOE_IP:
        ClassParameter.FieldMap.PppoEField.bfEnaSrcPort = OPL_ENABLE;
        ClassParameter.PktRuleValue.PPPoeValue.srcPort = L4SrcPortID;
        ClassParameter.PktUsrMask.PPPoeValue.srcPort = usrMask;

        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }


    if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
      /*delete old item.*/
      ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
      iStatus = dalClsEntryMove(&ClassTemp, &classRuleID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure MOVE Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      /*add new item.*/
      ClassTemp = ClassParameter;
      iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
      if (OPL_OK != iStatus) {
        /**/
        ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
        if (OPL_OK == dalClsEntrySet(&ClassTemp, &classRuleID,&classMaskID)) {
          gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
        }
        OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }
    }
    else {
      ClassParameter.pktType = pktType;

      /*add new item.*/
      ClassTemp = ClassParameter;
      iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      gClassifyItem[classItemID - 1].valid = OPL_VALID;
      gClassifyItem[classItemID - 1].Parameter.pktType = pktType;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaSrcPort = OPL_ENABLE;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.srcPort = ClassParameter.PktRuleValue.EIIValue.srcPort;
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.EIIValue.srcPort = ClassParameter.PktUsrMask.EIIValue.srcPort;
        break;
      case CLASSIFY_PPPOE_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaSrcPort = OPL_ENABLE;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.srcPort = ClassParameter.PktRuleValue.PPPoeValue.srcPort;
        gClassifyItem[classItemID - 1].Parameter.PktUsrMask.PPPoeValue.srcPort = ClassParameter.PktUsrMask.PPPoeValue.srcPort;

        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }
    gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
  }
  else {
    if (OPL_INVALID == gClassifyItem[classItemID - 1].valid) {
      OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
      return OPL_OK;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        if ((OPL_INVALID == gClassifyItem[classItemID - 1].valid)
         || (OPL_DISABLE
          == gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaSrcPort)) {
          OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
          return OPL_OK;
        }
        break;
      case CLASSIFY_PPPOE_IP:
        if ((OPL_INVALID == gClassifyItem[classItemID - 1].valid)
         || (OPL_DISABLE
          == gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaSrcPort)) {
          OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
          return OPL_OK;
        }
        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        ClassParameter.FieldMap.EIIField.bfEnaSrcPort = OPL_DISABLE;
        ClassParameter.PktRuleValue.EIIValue.srcPort = OPL_INVALID;

        break;
      case CLASSIFY_PPPOE_IP:
        ClassParameter.FieldMap.PppoEField.bfEnaSrcPort = OPL_DISABLE;
        ClassParameter.PktRuleValue.PPPoeValue.srcPort = OPL_INVALID;
        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }

    if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
      /*delete old item.*/
      ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
      iStatus = dalClsEntryMove(&ClassTemp, &classRuleID);
      if (OPL_OK != iStatus) {
        OPL_DAL_PRINTF(("Configure MOVE Error = 0x%x.\r\n", iStatus));
        return iStatus;
      }

      /*add new item.*/
      ClassTemp = ClassParameter;
      /* if(ClassTemp.FieldMap.bitMap) */ {
        iStatus = dalClsEntrySet(&(ClassTemp), &classRuleID,&classMaskID);
        if (OPL_OK != iStatus) {
          /**/
          ClassTemp = (gClassifyItem[classItemID - 1].Parameter);
          if (OPL_OK == dalClsEntrySet(&ClassTemp, &classRuleID,&classMaskID)) {
            gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
          }
          OPL_DAL_PRINTF(("Configure SET Error = 0x%x.\r\n", iStatus));
          return iStatus;
        }
      }
    }

    switch (pktType) {
      case CLASSIFY_EII_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.EIIField.bfEnaSrcPort = OPL_DISABLE;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.EIIValue.srcPort = OPL_INVALID;

        break;
      case CLASSIFY_PPPOE_IP:
        gClassifyItem[classItemID - 1].Parameter.FieldMap.PppoEField.bfEnaSrcPort = OPL_DISABLE;
        gClassifyItem[classItemID - 1].Parameter.PktRuleValue.PPPoeValue.srcPort = OPL_INVALID;

        break;
      default:
        OPL_DAL_PRINTF(("pkt type error.\n"));
        return OPL_ERR_INVALID_PARAMETERS;
    }
    gClassifyItem[classItemID - 1].classifyRuleID = classRuleID;
  }
  return OPL_OK;
}

#if 1
/*******************************************************************************
*
* dalClsRuleEntryShow - show rule table of classify
*
* DESCRIPTION:
*       show rule table of classify
*
* INPUT:
*       startId:
*   endId:
*   mode:0 for dal,1 for asic,2 for both dal and asic
*
* OUTPUT:
*       NULL.
*
* RETURN:
*   An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsRuleEntryShow(UINT16 startId,UINT16 endId,UINT8 mode)
{
  OPL_STATUS retVal = OPL_OK; 
  CLASSIFY_RULE_ENTRY_t *pDalClsRuleEntry = OPL_NULL;
  CLASSIFY_RULE_TBL_t asicClsRuleEntry;
  UINT16 entryId;
  UINT16 offset;

  UINT16 ruleid;
  UINT32 l_count = 0;
  UINT32 c_count = 0;

  if (startId >= TAB_CLS_RULE_LEN || endId >= TAB_CLS_RULE_LEN) {
    printk(("entryId out of rule table range.\r\n"));
    return OPL_ERR_INVALID_PARAMETERS;
  }

  printk("%-3s %-4s %-6s %-7s %-6s %-6s %-7s %-4s %-6s %s\r\n",
               "No",
               "Tact",
               "egress",
               "ingress",
               "rmkAct",
               "rmkCos",
               "rmkIcos",
               "vid",
               "MaskId",
               "ruleVal");
  for (entryId = startId; entryId <= endId; entryId++) {
    if ((mode == CLASSIFY_DAL_TABLE) || (mode == CLASSIFY_DAL_AND_ASIC_TABLE)) {
      for (offset = 0; offset < CLASSIFY_RULE_TABLE_GROUP; offset++) {
        if (gpCaddrTbl[entryId].ClasAddr[offset] > 0) {
          ruleid = (gpCaddrTbl[entryId].ClasAddr[offset]);
          pDalClsRuleEntry = &gpCruleTbl[ruleid];
          //OPL_DAL_PRINTF(("Local:\n");
          printk("%-3d %-4x %-6x %-7x %-6x %-6x %-7x %-4d %-6x\r\n",
                       ruleid,
                       pDalClsRuleEntry->bfTranActH
                    << 3
                     | pDalClsRuleEntry->bfTranActL,
                       pDalClsRuleEntry->bfEgress,
                       pDalClsRuleEntry->bfIngress,
                       pDalClsRuleEntry->bfRmkAct,
                       pDalClsRuleEntry->bfRmkCos,
                       pDalClsRuleEntry->bfRmkIcos,
                       pDalClsRuleEntry->bfRmkVid,
                       pDalClsRuleEntry->bfMsel);
          printk("%08x %08x %08x %08x %08x %08x\r\n",
                       *((UINT32 *) &(pDalClsRuleEntry->Rule[0])),
                       *((UINT32 *) &(pDalClsRuleEntry->Rule[4])),
                       *((UINT32 *) &(pDalClsRuleEntry->Rule[8])),
                       *((UINT32 *) &(pDalClsRuleEntry->Rule[12])),
                       *((UINT32 *) &(pDalClsRuleEntry->Rule[16])),
                       *((UINT32 *) &(pDalClsRuleEntry->Rule[20])));
          l_count++;
        }
      }
    }

    if ((mode == CLASSIFY_ASIC_TABLE) || (mode == CLASSIFY_DAL_AND_ASIC_TABLE)) {
      for (offset = 0; offset < CLASSIFY_RULE_TABLE_GROUP; offset++) {
        if (gpCaddrTbl[entryId].ClasAddr[offset] > 0) {
          ruleid = (gpCaddrTbl[entryId].ClasAddr[offset]);
          retVal = clsRuleEntryHwRead(ruleid, &asicClsRuleEntry);
          if (OPL_OK != retVal) {
            break;
          }      

          printk("%-3d %-4x %-6x %-7x %-6x %-6x %-7x %-4d %-6x\r\n",
                       ruleid,
                       asicClsRuleEntry.bfTranActH
                    << 3
                     | asicClsRuleEntry.bfTranActL,
                       asicClsRuleEntry.bfEgress,
                       asicClsRuleEntry.bfIngress,
                       asicClsRuleEntry.bfRmkAct,
                       asicClsRuleEntry.bfRmkCos,
                       asicClsRuleEntry.bfRmkIcos,
                       asicClsRuleEntry.bfRmkVid,
                       asicClsRuleEntry.bfMsel);
          printk("%08x %08x %08x %08x %08x %08x\r\n",
                       *((UINT32 *) &(asicClsRuleEntry.Rule[0])),
                       *((UINT32 *) &(asicClsRuleEntry.Rule[4])),
                       *((UINT32 *) &(asicClsRuleEntry.Rule[8])),
                       *((UINT32 *) &(asicClsRuleEntry.Rule[12])),
                       *((UINT32 *) &(asicClsRuleEntry.Rule[16])),
                       *((UINT32 *) &(asicClsRuleEntry.Rule[20])));
          c_count++;
        }
      }
    }
  }
  if ((mode == CLASSIFY_DAL_TABLE) || (mode == CLASSIFY_DAL_AND_ASIC_TABLE)) {
    printk("------------------------------------\r\n");
    printk("Asic rule entry count: %d/128\n", l_count);
  }
  if ((mode == CLASSIFY_ASIC_TABLE) || (mode == CLASSIFY_DAL_AND_ASIC_TABLE)) {
    printk("------------------------------------\r\n");
    printk("Asic rule entry count: %d/128\r\n", c_count);
  }
  return retVal;
}

/*******************************************************************************
*
* dalClsMaskEntryShow - show mask table of classify
*
* DESCRIPTION:
*       show mask table of classify
*
* INPUT:
*       startId:
*   endId:
*   mode:0 for dal,1 for asic,2 for both dal and asic
*
* OUTPUT:
*       NULL.
*
* RETURN:
*   An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsMaskEntryShow(UINT16 startId,UINT16 endId,UINT8 mode)
{
  OPL_STATUS retVal = OPL_OK;
  UINT32 entryId;
  CLASSIFY_MASK_ENTRY_t *pDalClsMaskEntry;
  CLASSIFY_MASK_TBL_t asiclClsMaskEntry;

  UINT32 l_count = 0;
  UINT32 c_count = 0;

  if (startId >= TAB_CLS_MASK_LEN || endId >= TAB_CLS_MASK_LEN) {
    printk(("entryId out of mask table range.\r\n"));
    return OPL_ERR_INVALID_PARAMETERS;
  }

  printk("%-3s %-5s %-5s %-5s %-3s %-3s %-3s %-3s %-3s %-3s %-3s %-3s %s\r\n",
               "No",
               "valid",
               "imask",
               "emask",
               "of6",
               "of5",
               "of4",
               "of3",
               "of2",
               "of1",
               "pri",
               "cnt",
               "maskVal");
  for (entryId = startId; entryId <= endId; entryId++) {
    if ((mode == CLASSIFY_DAL_TABLE) || (mode == CLASSIFY_DAL_AND_ASIC_TABLE)) {
      pDalClsMaskEntry = &gpCmaskTbl[entryId];
      if (pDalClsMaskEntry->bfValid == 1) {
        //OPL_DAL_PRINTF(("Local:\n"));
        printk("%-3d %-5x %-5x %-5x %-3d %-3d %-3d %-3d %-3d %-3d %-3d %-3d\r\n",
                     entryId,
                     pDalClsMaskEntry->bfValid,
                     pDalClsMaskEntry->bfImask,
                     pDalClsMaskEntry->bfEmask,
                     pDalClsMaskEntry->bfOffset6,
                     pDalClsMaskEntry->bfOffset5,
                     pDalClsMaskEntry->bfOffset4,
                     pDalClsMaskEntry->bfOffset3,
                     pDalClsMaskEntry->bfOffset2,
                     pDalClsMaskEntry->bfOffset1,
                     pDalClsMaskEntry->maskPri,
                     pDalClsMaskEntry->referenceCount);

        printk("%08x %08x %08x %08x %08x %08x\r\n",
                     *((UINT32 *) &(pDalClsMaskEntry->Mask[0])),
                     *((UINT32 *) &(pDalClsMaskEntry->Mask[4])),
                     *((UINT32 *) &(pDalClsMaskEntry->Mask[8])),
                     *((UINT32 *) &(pDalClsMaskEntry->Mask[12])),
                     *((UINT32 *) &(pDalClsMaskEntry->Mask[16])),
                     *((UINT32 *) &(pDalClsMaskEntry->Mask[20])));

        l_count++;
      }
    }

    if ((mode == CLASSIFY_ASIC_TABLE) || (mode == CLASSIFY_DAL_AND_ASIC_TABLE)) {
      retVal = clsMaskEntryHwRead(entryId, &asiclClsMaskEntry);
      if (OPL_OK != retVal) {
        break;
      }
      if (asiclClsMaskEntry.bfValid == 1) {
        //OPL_DAL_PRINTF(("Asic:\n"));
       printk("%-3d %-5x %-5x %-5x %-3d %-3d %-3d %-3d %-3d %-3d\r\n",
                     entryId,
                     asiclClsMaskEntry.bfValid,
                     asiclClsMaskEntry.bfImask,
                     asiclClsMaskEntry.bfEmask,
                     asiclClsMaskEntry.bfOffset6,
                     asiclClsMaskEntry.bfOffset5,
                     asiclClsMaskEntry.bfOffset4,
                     asiclClsMaskEntry.bfOffset3,
                     asiclClsMaskEntry.bfOffset2,
                     asiclClsMaskEntry.bfOffset1);

       printk("%08x %08x %08x %08x %08x %08x\r\n",
                     *((UINT32 *) &(asiclClsMaskEntry.Mask[0])),
                     *((UINT32 *) &(asiclClsMaskEntry.Mask[4])),
                     *((UINT32 *) &(asiclClsMaskEntry.Mask[8])),
                     *((UINT32 *) &(asiclClsMaskEntry.Mask[12])),
                     *((UINT32 *) &(asiclClsMaskEntry.Mask[16])),
                     *((UINT32 *) &(asiclClsMaskEntry.Mask[20])));
        c_count++;
      }
    }
  }
  if ((mode == CLASSIFY_DAL_TABLE) || (mode == CLASSIFY_DAL_AND_ASIC_TABLE)) {
    printk("------------------------------------\r\n");
    printk("Asic mask entry count: %d/23\n", l_count);
  }
  if ((mode == CLASSIFY_ASIC_TABLE) || (mode == CLASSIFY_DAL_AND_ASIC_TABLE)) {
    printk( "------------------------------------\r\n");
    printk("Asic mask entry count: %d/23\n", c_count);
  }
  return retVal;
}
/*******************************************************************************
*
* dalClsAddrEntryShow - show addr table of classify
*
* DESCRIPTION:
*       show addr table of classify
*
* INPUT:
*       startId:
*   endId:
*   mode:0 for dal,1 for asic,2 for both dal and asic
*
* OUTPUT:
*       NULL.
*
* RETURN:
*   An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsAddrEntryShow(UINT16 startId,UINT16 endId,UINT8 mode)
{
  OPL_STATUS retVal = OPL_OK;
  UINT32 entryId;
  CLASSIFY_ADDR_ENTRY_t *pDalClsAddrEntry;
  CLASSIFY_ADDR_TBL_t asiclClsAddrEntry;

  if (startId >= TAB_CLS_ADDR_LEN || endId >= TAB_CLS_ADDR_LEN) {
    printk(("entryId out of addr table range.\r\n"));
    return OPL_ERR_INVALID_PARAMETERS;
  }

  printk("%-3s %-4s %-4s %-4s %-4s\r\n",
               "No",
               "addr0",
               "addr1",
               "addr2",
               "addr3");

  for (entryId = startId; entryId <= endId; entryId++) {
    if ((mode == CLASSIFY_DAL_TABLE) || (mode == CLASSIFY_DAL_AND_ASIC_TABLE)) {
      pDalClsAddrEntry = &gpCaddrTbl[entryId];
      if ((pDalClsAddrEntry->ClasAddr[0] > 0)
       || (pDalClsAddrEntry->ClasAddr[1] > 0)
       || (pDalClsAddrEntry->ClasAddr[2] > 0)
       || (pDalClsAddrEntry->ClasAddr[3] > 0)) {
        printk("%-3d %-4d %-4d %-4d %-4d\r\n",
                     entryId,
                     pDalClsAddrEntry->ClasAddr[0],
                     pDalClsAddrEntry->ClasAddr[1],
                     pDalClsAddrEntry->ClasAddr[2],
                     pDalClsAddrEntry->ClasAddr[3]);
      }
    }

    if ((mode == CLASSIFY_ASIC_TABLE) || (mode == CLASSIFY_DAL_AND_ASIC_TABLE)) {
      retVal = clsAddrEntryHwRead(entryId, &asiclClsAddrEntry);
      if (OPL_OK != retVal) {
        break;
      }
      if ((asiclClsAddrEntry.bfAddr0 > 0)
       || (asiclClsAddrEntry.bfAddr1 > 0)
       || (asiclClsAddrEntry.bfAddr2 > 0)
       || (asiclClsAddrEntry.bfAddr3 > 0)) {
       printk("%-3d %-4d %-4d %-4d %-4d\r\n",
                     entryId,
                     asiclClsAddrEntry.bfAddr0,
                     asiclClsAddrEntry.bfAddr1,
                     asiclClsAddrEntry.bfAddr2,
                     asiclClsAddrEntry.bfAddr3);
      }
    }
  }
  return retVal;
}
#endif

/*******************************************************************************
* dalClsRuleDel - this function is used to remove the specified rule
*
* DESCRIPTION:
*
* INPUT:
*
*
* OUTPUT:
*       None.
*
* RETURN:
*   An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsRuleDel(UINT16 classItemID)
{
  CLASSIFY_PARAMETER_t ClassTemp;
  CLASSIFY_PARAMETER_t ClassParameter;
  UINT16 classRuleID;
  OPL_STATUS iStatus;

  if (OPL_ZERO == classItemID || TAB_CLS_RULE_LEN <= classItemID) {
    OPL_DAL_PRINTF(("this chip can only support %d rules,and the 0 items is not valid.\n",
                    TAB_CLS_RULE_LEN));
    return OPL_ERR_INVALID_PARAMETERS;
  } 

  OPL_BZERO(&ClassTemp, sizeof(CLASSIFY_PARAMETER_t));
  OPL_BZERO(&ClassParameter, sizeof(CLASSIFY_PARAMETER_t));
  if (OPL_VALID == gClassifyItem[classItemID - 1].valid) {
    ClassParameter = gClassifyItem[classItemID - 1].Parameter;
  }


  if (OPL_INVALID == gClassifyItem[classItemID - 1].valid) {
    OPL_DAL_PRINTF(("Delete NULL field.\r\n"));
    return OPL_OK;
  }

  iStatus = dalClsEntryMove(&ClassParameter, &classRuleID);
  if (OPL_OK != iStatus) {
    OPL_DAL_PRINTF(("remove this rule failed.\n"));
    return OPL_ERROR;
  }

  OPL_DAL_PRINTF(("remove successful.\n"));

  OPL_BZERO(&gClassifyItem[classItemID - 1], sizeof(CLASSIFY_ITEM_t));

  return OPL_OK;
}

/*******************************************************************************
* dalClsConvertPara - this function is used to fix the classs parameter from ctc parameter
*
* DESCRIPTION:
*
* INPUT:
*   pClassItemInfo:  classifyItem
*   stClassCfg:      ctcConfig Info.
*   pktType:
*
* OUTPUT:
*       None.
*
* RETURN:
*   An appropriate specific error code defined for CLT device driver.
*
* SEE ALSO: 
*/
OPL_STATUS dalClsConvertPara(CLASSIFY_ITEM_t *pClassItemInfo,
                             CLS_CONFIG_INFO_t *stClassCfg,
                             UINT8 pktType)
{
  CLASSIFY_PARAMETER_t *pParameter;

 int i;
 
  if (pClassItemInfo == OPL_NULL || stClassCfg == OPL_NULL) {
    OPL_LOG_TRACE();
    return OPL_ERR_NULL_POINTER;
  }

  OPL_BZERO(pClassItemInfo, sizeof(CLASSIFY_ITEM_t));
  pParameter = &(pClassItemInfo->Parameter);

  /*set type and priority*/
  pParameter->pktType = pktType;
  pParameter->maskPri = stClassCfg->rulePri;
  pParameter->ingressMask = stClassCfg->ingressmask;
  pParameter->RuleEntry.bfIngress = stClassCfg->bingress;
  /*set transfaction*/
  pParameter->RuleEntry.bfTranActH = ((stClassCfg->t_act) & 0x8) >> 3; 
  pParameter->RuleEntry.bfTranActL = ((stClassCfg->t_act) & 0x7);
  /*set remark action*/
  if (stClassCfg->cosMapVal <= CTC_ETHER_PRI_7) {
		/*pParameter->RuleEntry.bfRmkAct |= CHANGE_COS_FIELD;*/
		/*modified by ltang,for bug3222 ---start*/
		if(1==stClassCfg->rmkcoschange)
		{
			pParameter->RuleEntry.bfRmkAct |= CHANGE_COS_FIELD;
		}
		/*modified by ltang,for bug3222 ---end*/
		pParameter->RuleEntry.bfRmkCos = stClassCfg->cosMapVal;
	}
	else if (stClassCfg->cosMapVal == CTC_ETHER_PRI_END) {
	}
	else {
	OPL_LOG_TRACE();
	return OPL_ERR_INVALID_PARAMETERS;
	}

  /* 
	  modify by zttan, for mapped queueid , can't set CTC_QUEUE_MAP_7 , because of queue 7 is oam queue 
   */
	if (stClassCfg->queueMapId < CTC_QUEUE_MAP_7) {
		/*pParameter->RuleEntry.bfRmkAct |= CHANGE_ICOS_FIELD;*/
		/*modified by ltang,for bug3222 ---start*/
		if(1==stClassCfg->rmkicoschange)
		{
//			printk("-ltang debug-! stClassCfg->rmkicoschange is %d",stClassCfg->rmkicoschange);
			pParameter->RuleEntry.bfRmkAct |= CHANGE_ICOS_FIELD;
		}
		/*modified by ltang,for bug3222 ---end*/
		pParameter->RuleEntry.bfRmkIcos = stClassCfg->queueMapId;	
	}
	else if (stClassCfg->queueMapId == CTC_QUEUE_MAP_END) {
	}
	else {
		OPL_LOG_TRACE();
		return OPL_ERR_INVALID_PARAMETERS;
	}
#if 0
  if (stClassCfg->queueMapId < CTC_QUEUE_MAP_7) {
    pParameter->RuleEntry.bfRmkAct |= CHANGE_ICOS_FIELD;
    pParameter->RuleEntry.bfRmkIcos = stClassCfg->queueMapId;
  }
  else if (stClassCfg->queueMapId == CTC_QUEUE_MAP_END) {
  }
  else {
    OPL_LOG_TRACE();
    return OPL_ERR_INVALID_PARAMETERS;
  }
#endif

  /*set up stream ingress mask
  * GE PORT: 0, PON PORT 1.
  */
  /*
  pParameter->RuleEntry.bfIngress= 1;
  pParameter->ingressMask = 1;
  */
  /*set care field*/
  if (stClassCfg->dstMacFlag) {
    pParameter->FieldMap.EIIField.bfEnaDestMAC = OPL_ENABLE;
	
	for (i = 0; i < 6; i++) {
		stClassCfg->dstMac.lowRange[i] &= stClassCfg->dstMac.highRange[i];
	}
    OPL_MEMCPY(pParameter->PktRuleValue.EIIValue.DestMac,stClassCfg->dstMac.lowRange,MAC_LENGTH);
    OPL_MEMCPY(pParameter->PktUsrMask.EIIValue.DestMac,stClassCfg->dstMac.highRange,MAC_LENGTH);
  }

  if (stClassCfg->srcMacFlag) {
    pParameter->FieldMap.EIIField.bfEnaSrcMAC = OPL_ENABLE;
	for (i = 0; i < 6; i++) {
		stClassCfg->srcMac.lowRange[i] &= stClassCfg->srcMac.highRange[i];
	}
    OPL_MEMCPY(pParameter->PktRuleValue.EIIValue.SrcMac,stClassCfg->srcMac.lowRange,MAC_LENGTH);
    OPL_MEMCPY(pParameter->PktUsrMask.EIIValue.SrcMac,stClassCfg->srcMac.highRange,MAC_LENGTH);
  }

  if (stClassCfg->etherPriFlag) {
    pParameter->FieldMap.EIIField.bfEnaFirstCOS = OPL_ENABLE;
	stClassCfg->etherPri.lowRange &= stClassCfg->etherPri.highRange;
    pParameter->PktRuleValue.EIIValue.bfFirstCos = stClassCfg->etherPri.lowRange;
	pParameter->PktUsrMask.EIIValue.bfFirstCos=stClassCfg->etherPri.highRange;
  }

  if (stClassCfg->vlanIdFlag) {
    pParameter->FieldMap.EIIField.bfEnaFirstTag = OPL_ENABLE;
	stClassCfg->vlanId.lowRange &= stClassCfg->vlanId.highRange;
    pParameter->PktRuleValue.EIIValue.bfFirstVlan = stClassCfg->vlanId.lowRange;
    pParameter->PktUsrMask.EIIValue.bfFirstVlan = stClassCfg->vlanId.highRange;
  }

  if (stClassCfg->lenOrTypeFlag) {
    pParameter->FieldMap.EIIField.bfEnaLenorType = OPL_ENABLE;
	stClassCfg->lenOrType.lowRange &= stClassCfg->lenOrType.highRange;
    pParameter->PktRuleValue.EIIValue.bfLenOrType = stClassCfg->lenOrType.lowRange;
    pParameter->PktUsrMask.EIIValue.bfLenOrType = stClassCfg->lenOrType.highRange;
  }

  if (stClassCfg->dstIpFlag) {
    pParameter->FieldMap.EIIField.bfEnaDestIP = OPL_ENABLE;
	stClassCfg->dstIp.lowRange &= stClassCfg->dstIp.highRange;
    pParameter->PktRuleValue.EIIValue.destIP = stClassCfg->dstIp.lowRange;
    pParameter->PktUsrMask.EIIValue.destIP = stClassCfg->dstIp.highRange;
  }

  if (stClassCfg->srcIpFlag) {
    pParameter->FieldMap.EIIField.bfEnaSrcIP = OPL_ENABLE;
	stClassCfg->srcIp.lowRange &= stClassCfg->srcIp.highRange;
    pParameter->PktRuleValue.EIIValue.srcIP = stClassCfg->srcIp.lowRange;
    pParameter->PktUsrMask.EIIValue.srcIP = stClassCfg->srcIp.highRange;
  }

  if (stClassCfg->ipTypeFlag) {
    pParameter->FieldMap.EIIField.bfEnaIpProType = OPL_ENABLE;
	stClassCfg->ipType.lowRange &= stClassCfg->ipType.highRange;
    pParameter->PktRuleValue.EIIValue.ipProType = stClassCfg->ipType.lowRange;
    pParameter->PktUsrMask.EIIValue.ipProType = stClassCfg->ipType.highRange;
  }

  if (stClassCfg->ipV4DscpFlag) {
    pParameter->FieldMap.EIIField.bfEnaIpTos = OPL_ENABLE;
	stClassCfg->ipV4Dscp.lowRange &=stClassCfg->ipV4Dscp.highRange;
    pParameter->PktRuleValue.EIIValue.bfIpTos = stClassCfg->ipV4Dscp.lowRange;
    pParameter->PktUsrMask.EIIValue.bfIpTos = stClassCfg->ipV4Dscp.highRange;
  }

  if (stClassCfg->srcL4PortFlag) {
    pParameter->FieldMap.EIIField.bfEnaSrcPort = OPL_ENABLE;
	stClassCfg->srcL4Port.lowRange &= stClassCfg->srcL4Port.highRange;
    pParameter->PktRuleValue.EIIValue.srcPort = stClassCfg->srcL4Port.lowRange;
    pParameter->PktUsrMask.EIIValue.srcPort = stClassCfg->srcL4Port.highRange;
  }

  if (stClassCfg->dstL4PortFlag) {
    pParameter->FieldMap.EIIField.bfEnaDestPort = OPL_ENABLE;
	stClassCfg->dstL4Port.lowRange &= stClassCfg->dstL4Port.highRange;
    pParameter->PktRuleValue.EIIValue.destPort = stClassCfg->dstL4Port.lowRange;
    pParameter->PktUsrMask.EIIValue.destPort = stClassCfg->dstL4Port.highRange;
  }
  if (stClassCfg->rmkvidOp == 0x2
  		|| stClassCfg->rmkvidOp == 0x3) {
		pParameter->RuleEntry.bfRmkAct |= (stClassCfg->rmkvidOp<<2);
		pParameter->RuleEntry.bfRmkVid= stClassCfg->rmkvid;
  }
  return OPL_OK;
}

/*******************************************************************************
* dalClsCtcInit - this function is used to init ctc classification.
*
* DESCRIPTION:
*
* INPUT:
* void
*
* OUTPUT:
*       None.
*
* RETURN:
*   
*
* SEE ALSO: 
*/
OPL_STATUS dalClsCtcInit(void)
{
  UINT32 index = OPL_ZERO;

  OPL_MEMSET((UINT8 *) &clsCtcToDalRuleMapTab[0],
             0x00,
             (CLASSIFY_MAX_ITEM_NUM - MAX_GROUP_NUM ) * sizeof(CLS_CTC_TO_DAL_MAP_t));

  for (index = OPL_ZERO;
       index < CLASSIFY_MAX_ITEM_NUM - MAX_GROUP_NUM ;
       index++) {
    OPL_MEMSET(&gClassifyItem[index], 0x00, sizeof(CLASSIFY_ITEM_t));
  }

  return OPL_OK;
}

/*******************************************************************************
* dalClsRuleCtcAdd - this function is used to add a ctc rule.
*
* DESCRIPTION:
*
* INPUT:
*   stClassCfg:
*   ruleId:
*
* OUTPUT:
*       None.
*
* RETURN:
*   
*
* SEE ALSO: 
*/
OPL_STATUS dalClsRuleCtcAdd(CLS_CONFIG_INFO_t *stClassCfg,UINT16 *ruleId)
{
  OPL_STATUS iStatus = OPL_OK;
  UINT32 ruleIndex;
  UINT32 firstEmptyRuleId = 0xffffffff;
  CLASSIFY_ITEM_t *pClassItemInfo = OPL_NULL;

  if (stClassCfg == OPL_NULL || ruleId == OPL_NULL) {
    OPL_LOG_TRACE();
    return OPL_ERR_NULL_POINTER;
  }

  stClassCfg->valid = OPL_ENABLE;

  for (ruleIndex = OPL_ZERO;
       ruleIndex < CLASSIFY_MAX_ITEM_NUM - MAX_GROUP_NUM ;
       ruleIndex++) {
    if ((!clsCtcToDalRuleMapTab[ruleIndex].valid)
     && (!gClassifyItem[ruleIndex].valid)) {
      firstEmptyRuleId = ruleIndex;
      continue;
    }
    else if (!OPL_MEMCMP((UINT8 *)
                         stClassCfg,
                         (UINT8 *) &(clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo),
                         sizeof(CLS_CONFIG_INFO_t))) {
      //  *ruleId = clsCtcToDalRuleMapTab[ruleIndex].dalRuleMapIndex;
      *ruleId = gClassifyItem[ruleIndex].classifyRuleID;
      return OPL_OK;
    }
  }

  if (firstEmptyRuleId != 0xffffffff) {
    pClassItemInfo = &(gClassifyItem[firstEmptyRuleId]);
  }
  else {
    OPL_LOG_TRACE();
    return OPL_ERROR;
  }

  iStatus = dalClsConvertPara(pClassItemInfo, stClassCfg, CLASSIFY_EII_IP);
  if (OPL_OK != iStatus) {
    OPL_LOG_TRACE();
    return iStatus;
  }
  /*
  * we need to config two rule one for no tag ,another for single tag packets
  */
  iStatus = dalClsEntrySet(&(pClassItemInfo->Parameter), ruleId,&(pClassItemInfo->shadowtbl));
  if (OPL_OK != iStatus) {
    OPL_LOG_TRACE();
    /*clear class item data*/
    OPL_MEMSET(pClassItemInfo, 0, sizeof(CLASSIFY_ITEM_t));
    return iStatus;
  }

  /*set rule success*/
  OPL_MEMCPY(&(clsCtcToDalRuleMapTab[firstEmptyRuleId].clsCtcConfigInfo),
             stClassCfg,
             sizeof(CLS_CONFIG_INFO_t));
  clsCtcToDalRuleMapTab[firstEmptyRuleId].valid = OPL_VALID;
  clsCtcToDalRuleMapTab[firstEmptyRuleId].dalRuleMapIndex = firstEmptyRuleId;

  pClassItemInfo->classifyRuleID = *ruleId;
  pClassItemInfo->valid = OPL_VALID;

  return OPL_OK;
}

OPL_STATUS dalVoipClsRuleCtcAdd(CLS_CONFIG_INFO_t *stClassCfg,UINT16 *ruleId)
{
  OPL_STATUS iStatus = OPL_OK;
  UINT32 ruleIndex;
  UINT32 firstEmptyRuleId = 0xffffffff;
  CLASSIFY_ITEM_t *pClassItemInfo = OPL_NULL;


  if (stClassCfg == OPL_NULL || ruleId == OPL_NULL) {
    OPL_LOG_TRACE();
    return OPL_ERR_NULL_POINTER;
  }

  stClassCfg->valid = OPL_ENABLE;

  for (ruleIndex = OPL_ZERO;
       ruleIndex < CLASSIFY_MAX_ITEM_NUM - MAX_GROUP_NUM ;
       ruleIndex++) {
    if ((!gClassifyItem[ruleIndex].valid)) {
      firstEmptyRuleId = ruleIndex;
      break;
    } 
  }

  if (firstEmptyRuleId != 0xffffffff) {
    pClassItemInfo = &(gClassifyItem[firstEmptyRuleId]);
  }
  else {
    OPL_LOG_TRACE();
    return OPL_ERROR;
  }

  iStatus = dalClsConvertPara(pClassItemInfo, stClassCfg, CLASSIFY_EII_IP);
  if (OPL_OK != iStatus) {
    OPL_LOG_TRACE();
    return iStatus;
  }
  /*deny multi action for same rule */

	    for (ruleIndex = OPL_ZERO;
	       ruleIndex < CLASSIFY_MAX_ITEM_NUM- MAX_GROUP_NUM  ;
	       ruleIndex++) {
	    	if((gClassifyItem[ruleIndex].valid)       && 
               (!OPL_MEMCMP((UINT8 *)&(pClassItemInfo->Parameter.PktRuleValue),
	                        (UINT8 *)&(gClassifyItem[ruleIndex].Parameter.PktRuleValue),
	                        sizeof(ClsDataValue_t)))  &&
	           (gClassifyItem[ruleIndex].Parameter.RuleEntry.bfIngress == pClassItemInfo->Parameter.RuleEntry.bfIngress))
	           {
                    OPL_DAL_PRINTF(("rule data same %d  empty %d  ,return \n",ruleIndex,firstEmptyRuleId));
				    //OPL_LOG_TRACE();
					return OPL_ERROR;
			   }
	  		}
  /*
  * we need to config two rule one for no tag ,another for single tag packets
  */
  
  iStatus = dalClsEntrySet(&(pClassItemInfo->Parameter),ruleId,&(pClassItemInfo->shadowtbl));
  if (OPL_OK != iStatus) {
    OPL_LOG_TRACE();
    /*clear class item data*/
    OPL_MEMSET(pClassItemInfo, 0, sizeof(CLASSIFY_ITEM_t));
    return iStatus;
  }

  /*set rule success*/
  	OPL_MEMCPY(&(gClassifyItem[firstEmptyRuleId].Parameter),
             &(pClassItemInfo->Parameter),
             sizeof(CLASSIFY_PARAMETER_t));
  pClassItemInfo->classifyRuleID = *ruleId;
  pClassItemInfo->valid = OPL_VALID;

  return OPL_OK;
}

OPL_STATUS dalOnuClsRuleCtcAdd(opl_acl_list_t *oamcls)
{
  OPL_STATUS iStatus = OPL_OK;
  UINT32 ruleIndex;
  UINT32 firstEmptyRuleId = 0xffffffff;
  CLASSIFY_ITEM_t *pClassItemInfo ;
  UINT16 ruleId=0;
  CLS_CONFIG_INFO_t *stClassCfg=NULL;

  if (oamcls == OPL_NULL) {
    OPL_LOG_TRACE();
    return OPL_ERR_NULL_POINTER;
  }
  
  stClassCfg=&(oamcls->stClassCfg);

  stClassCfg->valid = OPL_ENABLE;

  for (ruleIndex = OPL_ZERO;
       ruleIndex < CLASSIFY_MAX_ITEM_NUM- MAX_GROUP_NUM  ;
       ruleIndex++) {
    if ((!gClassifyItem[ruleIndex].valid)) {
      firstEmptyRuleId = ruleIndex;
      break;
    } 
  }
	//printk("add index %d for precedence %d \n",firstEmptyRuleId 
		//,stClassCfg->rulePri);
  if (firstEmptyRuleId != 0xffffffff) {
    pClassItemInfo = &(gClassifyItem[firstEmptyRuleId]);
  }
  else {
    OPL_LOG_TRACE();
    return OPL_ERROR;
  }

  iStatus = dalClsConvertPara(pClassItemInfo, stClassCfg, CLASSIFY_EII_IP);
  if (OPL_OK != iStatus) {
    OPL_LOG_TRACE();
    return iStatus;
  }
  /*deny multi action for same rule */
  
/* Bug2931 F13 ,modified by cxji,2010/06/18 18:00:00 .begin*/
    for (ruleIndex = OPL_ZERO;
       ruleIndex < CLASSIFY_MAX_ITEM_NUM- MAX_GROUP_NUM  ;
       ruleIndex++) {
    	if(gClassifyItem[ruleIndex].valid && !OPL_MEMCMP((UINT8 *)
                         &(pClassItemInfo->Parameter.PktRuleValue),
                         (UINT8 *) &(gClassifyItem[ruleIndex].Parameter.PktRuleValue),
                         sizeof(ClsDataValue_t))
                         && !OPL_MEMCMP((UINT8 *)
                         &(pClassItemInfo->Parameter.FieldMap),
                         (UINT8 *) &(gClassifyItem[ruleIndex].Parameter.FieldMap),
                         sizeof(CLASSIFY_FIELD_MAP_t))){
                        // printk("rule data same %d  empty %d  ,return \n",ruleIndex,firstEmptyRuleId);
	    OPL_LOG_TRACE();
		return OPL_ERROR;
		}
  } 
/* Bug2931 F13 ,modified by cxji,2010/06/18 18:00:00 .end*/

  oamcls->dalindex=firstEmptyRuleId;
  /*
  * we need to config two rule one for no tag ,another for single tag packets
  */
  iStatus = dalClsEntrySet(&(pClassItemInfo->Parameter), &ruleId,&(pClassItemInfo->shadowtbl));
  if (OPL_OK != iStatus) {
  	OPL_DAL_PRINTF(("set classify error status %d \n",iStatus));
    OPL_LOG_TRACE();
    /*clear class item data*/
    OPL_MEMSET(pClassItemInfo, 0, sizeof(CLASSIFY_ITEM_t));
    return iStatus;
  }

  /*set rule success*/

  OPL_MEMCPY(&(gClassifyItem[firstEmptyRuleId].Parameter),
             &(pClassItemInfo->Parameter),
             sizeof(CLASSIFY_PARAMETER_t));
  pClassItemInfo->classifyRuleID = ruleId;
  pClassItemInfo->valid = OPL_VALID;
  OPL_MEMCPY(&(oamcls->hwshadow),&(pClassItemInfo->shadowtbl),sizeof(opl_cls_hwtbl_t));
  return OPL_OK;
}

OPL_STATUS dalOnuClsRuleCtcDel(opl_acl_list_t *oamcls)
{

  UINT16 ruleIndex;
  opl_cls_hwtbl_t clstblshadow;

	if (oamcls == OPL_NULL) {
	OPL_LOG_TRACE();
	return OPL_ERR_NULL_POINTER;
  	}
	ruleIndex=oamcls->dalindex;
	OPL_DAL_PRINTF(("delete index %d\n",ruleIndex));
	memcpy(&clstblshadow,&(gClassifyItem[ruleIndex].shadowtbl),sizeof(opl_cls_hwtbl_t));
   dalClsTblEntryRemove(&clstblshadow); 
   OPL_MEMSET(&gClassifyItem[ruleIndex], 0, sizeof(CLASSIFY_ITEM_t));
  return OPL_OK;
}

OPL_STATUS dalOnuClsRuleCtcChange(opl_acl_list_t *oamcls,UINT16 newmask)
{
  UINT32 ruleIndex;
  UINT16 oldmask;
  UINT16 ruleId;
  
	if (oamcls == OPL_NULL) {
	OPL_LOG_TRACE();
	return OPL_ERR_NULL_POINTER;
  	}
	ruleIndex=oamcls->dalindex;
	oldmask=gClassifyItem[ruleIndex].shadowtbl.clsHwMaskID;
	ruleId=gClassifyItem[ruleIndex].shadowtbl.clsHwRuleID;
	dalClsTblEntryChange(oldmask,ruleId,newmask); 
	gClassifyItem[ruleIndex].shadowtbl.clsHwMaskID=newmask;
  return OPL_OK;
}

/*******************************************************************************
* dalClsRuleCtcDel - this function is used to delete a ctc classify rule
*
* DESCRIPTION:
*
* INPUT:
*   stClassCfg:   ctc config info
*   ruleId:     
* OUTPUT:
*       None.
*
* RETURN:
*   
*
* SEE ALSO: 
*/
OPL_STATUS dalClsRuleCtcDel(CLS_CONFIG_INFO_t *stClassCfg,UINT16 *ruleId)
{
  OPL_STATUS iStatus = OPL_OK;
  UINT32 ruleIndex;
  UINT32 findRuleId = 0xffffffff;
  CLASSIFY_ITEM_t *pClassItemInfo = OPL_NULL;

  if (stClassCfg == OPL_NULL || ruleId == OPL_NULL) {
    OPL_LOG_TRACE();
    return OPL_ERR_NULL_POINTER;
  }

  for (ruleIndex = OPL_ZERO;
       ruleIndex < CLASSIFY_MAX_ITEM_NUM - MAX_GROUP_NUM;
       ruleIndex++) {
    if (!clsCtcToDalRuleMapTab[ruleIndex].valid) {
      continue;
    }
    else if (!OPL_MEMCMP((UINT8 *)
                         stClassCfg,
                         (UINT8 *) &(clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo),
                         sizeof(CLS_CONFIG_INFO_t))) {
      //  *ruleId = clsCtcToDalRuleMapTab[ruleIndex].dalRuleMapIndex;
      *ruleId = gClassifyItem[ruleIndex].classifyRuleID;
      findRuleId = ruleIndex;
      break;
    }
  }

  if (findRuleId == 0xffffffff) {
    OPL_LOG_TRACE();
    return OPL_OK;
  }

  pClassItemInfo = &(gClassifyItem[findRuleId]);
  iStatus = dalClsEntryMove(&(pClassItemInfo->Parameter), ruleId);
  if (OPL_OK != iStatus) {
    OPL_LOG_TRACE();
    return iStatus;
  }

  return iStatus;
}

/*******************************************************************************
* dalClsRuleCtcClear - this function is used to clear all ctc classify rule
*
* DESCRIPTION:
*
* INPUT:
*
*
* OUTPUT:
*       None.
*
* RETURN:
*   
*
* SEE ALSO: 
*/
OPL_STATUS dalClsRuleCtcClear(void)
{
  OPL_STATUS iStatus;
  UINT32 ruleIndex;
  UINT16 ruleId;

  for (ruleIndex = OPL_ZERO;
       ruleIndex < CLASSIFY_MAX_ITEM_NUM - MAX_GROUP_NUM;
       ruleIndex++) {
    if (!clsCtcToDalRuleMapTab[ruleIndex].valid) {
      continue;
    }

    iStatus = dalClsEntryMove(&(gClassifyItem[ruleIndex].Parameter), &ruleId);
    if (OPL_OK != iStatus) {
      OPL_LOG_TRACE();
      return iStatus;
    }
  }

  dalClsCtcInit();

  return OPL_OK;
}

/*-------------------------------------*/
int dalClsRuleToDalIndex(UINT16 ruleid)
{
  UINT32 ruleIndex;

  for (ruleIndex = OPL_ZERO;
       ruleIndex < CLASSIFY_MAX_ITEM_NUM - MAX_GROUP_NUM ;
       ruleIndex++) {
    if (gClassifyItem[ruleIndex].classifyRuleID == ruleid) {
      return ruleIndex;
    }
  }
  return -1;
}
OPL_STATUS dalClsDelEntry(UINT32 ruleid)
{
	return dalVoipClsDelEntry(ruleid);
}

OPL_STATUS dalVoipClsDelEntry(UINT32 ruleid)
{
	UINT16 rule;
	INT32 ruleIndex;
	
	int iStatus;
	
	CLASSIFY_ITEM_t *pClassItemInfo = OPL_NULL;
	ruleIndex = dalClsRuleToDalIndex(ruleid);
	if (ruleIndex == -1) {
		
	  return OPL_ERROR;
	}
	
	pClassItemInfo = (&gClassifyItem[ruleIndex]);
	iStatus = dalClsEntryMove(&(pClassItemInfo->Parameter), &rule);
	if (OPL_OK != iStatus) {
		
	  OPL_LOG_TRACE();
	  return iStatus;
	}
	
	OPL_MEMSET(&gClassifyItem[ruleIndex], 0, sizeof(CLASSIFY_ITEM_t));
	return OPL_OK;

}

OPL_STATUS dalExClsDelEntry(UINT32 ruleIndex)
{
         CLASSIFY_ITEM_t *pClassItemInfo = OPL_NULL;
         int iStatus=0;
         UINT16 rule=0;

         pClassItemInfo = (&gClassifyItem[ruleIndex]);
         iStatus = dalClsEntryMove(&(pClassItemInfo->Parameter), &rule);
         if (OPL_OK != iStatus) {
           OPL_LOG_TRACE();
           return iStatus;
         }
         
         OPL_MEMSET(&gClassifyItem[ruleIndex], 0, sizeof(CLASSIFY_ITEM_t));
         return OPL_OK;

}

OPL_STATUS dalClaCfgCheck(CLS_CONFIG_INFO_t *stClassCfg,UINT16 *ruleId)
{

  UINT32 ruleIndex;

  stClassCfg->valid = OPL_ENABLE;

  if (stClassCfg == OPL_NULL) {
    OPL_LOG_TRACE();
    return OPL_ERR_NULL_POINTER;
  }

  for (ruleIndex = OPL_ZERO;
       ruleIndex < CLASSIFY_MAX_ITEM_NUM - MAX_GROUP_NUM ;
       ruleIndex++) {
    if (!OPL_MEMCMP((UINT8 *)
                    stClassCfg,
                    (UINT8 *) &(clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo),
                    sizeof(CLS_CONFIG_INFO_t))) {
      //  *ruleId = clsCtcToDalRuleMapTab[ruleIndex].dalRuleMapIndex;
      *ruleId = gClassifyItem[ruleIndex].classifyRuleID;
      return OPL_OK;
    }
  }

  return OPL_ERROR;
}

OPL_STATUS dalClscollisionActGet(CLS_CONFIG_INFO_t *stClassCfg,
                                 UINT8 *act,
                                 UINT16 *rule)
{

  UINT32 ruleIndex;

  UINT8 action = 0;
  UINT16 tmp = 0;

  UINT32 srcflag = 0;
  UINT32 dstflag = 0;



  if ((stClassCfg == OPL_NULL) || (act == OPL_NULL) || (rule == OPL_NULL)) {
    OPL_LOG_TRACE();
    return OPL_ERR_NULL_POINTER;
  }

  for (ruleIndex = OPL_ZERO;
       ruleIndex < CLASSIFY_MAX_ITEM_NUM - MAX_GROUP_NUM ;
       ruleIndex++) {
    if (1 == clsCtcToDalRuleMapTab[ruleIndex].valid) {
      srcflag |= stClassCfg->dstMacFlag << 1;
      srcflag |= stClassCfg->srcMacFlag << 2;
      srcflag |= stClassCfg->etherPriFlag << 3;
      srcflag |= stClassCfg->vlanIdFlag << 4;
      srcflag |= stClassCfg->lenOrTypeFlag << 5;
      srcflag |= stClassCfg->dstIpFlag << 6;
      srcflag |= stClassCfg->srcIpFlag << 7;
      srcflag |= stClassCfg->ipTypeFlag << 8;
      srcflag |= stClassCfg->ipV4DscpFlag << 9;
      srcflag |= stClassCfg->ipV6PreFlag << 10;
      srcflag |= stClassCfg->srcL4PortFlag << 11;
      srcflag |= stClassCfg->dstL4PortFlag << 12;

      dstflag |= clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.dstMacFlag
              << 1;
      dstflag |= clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.srcMacFlag
              << 2;
      dstflag |= clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.etherPriFlag
              << 3;
      dstflag |= clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.vlanIdFlag
              << 4;
      dstflag |= clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.lenOrTypeFlag
              << 5;
      dstflag |= clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.dstIpFlag
              << 6;
      dstflag |= clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.srcIpFlag
              << 7;
      dstflag |= clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.ipTypeFlag
              << 8;
      dstflag |= clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.ipV4DscpFlag
              << 9;
      dstflag |= clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.ipV6PreFlag
              << 10;
      dstflag |= clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.srcL4PortFlag
              << 11;
      dstflag |= clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.dstL4PortFlag
              << 12;


      if (srcflag == dstflag) {
        continue;
      }
      /*find small field*/
      else if ((srcflag | dstflag) == srcflag) {
        if (clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.dstMacFlag) {
          if (OPL_MEMCMP(stClassCfg->dstMac.lowRange,
                         clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.dstMac.lowRange,
                         MAC_LENGTH)
           != 0) {
            continue;
          }
        }

        if (clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.srcMacFlag) {
          if (OPL_MEMCMP(stClassCfg->srcMac.lowRange,
                         clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.srcMac.lowRange,
                         MAC_LENGTH)
           != 0) {
            continue;
          }
        }

        if (clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.etherPriFlag) {
          if (stClassCfg->etherPri.lowRange
           != clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.etherPri.lowRange) {
            continue;
          }
        }

        if (clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.vlanIdFlag) {
          if (stClassCfg->vlanId.lowRange
           != clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.vlanId.lowRange) {
            continue;
          }
        }

        if (clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.lenOrTypeFlag) {
          if (stClassCfg->lenOrType.lowRange
           != clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.lenOrType.lowRange) {
            continue;
          }
        }

        if (clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.dstIpFlag) {
          if (stClassCfg->dstIp.lowRange
           != clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.dstIp.lowRange) {
            continue;
          }
        }

        if (clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.srcIpFlag) {
          if (stClassCfg->srcIp.lowRange
           != clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.srcIp.lowRange) {
            continue;
          }
        }

        if (clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.ipTypeFlag) {
          if (stClassCfg->ipType.lowRange
           != clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.ipType.lowRange) {
            continue;
          }
        }

        if (clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.ipV4DscpFlag) {
          if (stClassCfg->ipV4Dscp.lowRange
           != clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.ipV4Dscp.lowRange) {
            continue;
          }
        }

        if (clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.srcL4PortFlag) {
          if (stClassCfg->srcL4Port.lowRange
           != clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.srcL4Port.lowRange) {
            continue;
          }
        }

        if (clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.dstL4PortFlag) {
          if (stClassCfg->dstL4Port.lowRange
           != clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.dstL4Port.lowRange) {
            continue;
          }
        }

        action |= clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.t_act;
      }


      /*find big field*/
      else if ((srcflag | dstflag) == dstflag) {
        if (stClassCfg->dstMacFlag) {
          if (OPL_MEMCMP(stClassCfg->dstMac.lowRange,
                         clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.dstMac.lowRange,
                         MAC_LENGTH)
           != 0) {
            continue;
          }
        }

        if (stClassCfg->srcMacFlag) {
          if (OPL_MEMCMP(stClassCfg->srcMac.lowRange,
                         clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.srcMac.lowRange,
                         MAC_LENGTH)
           != 0) {
            continue;
          }
        }

        if (stClassCfg->etherPriFlag) {
          if (stClassCfg->etherPri.lowRange
           != clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.etherPri.lowRange) {
            continue;
          }
        }

        if (stClassCfg->vlanIdFlag) {
          if (stClassCfg->vlanId.lowRange
           != clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.vlanId.lowRange) {
            continue;
          }
        }

        if (stClassCfg->lenOrTypeFlag) {
          if (stClassCfg->lenOrType.lowRange
           != clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.lenOrType.lowRange) {
            continue;
          }
        }

        if (stClassCfg->dstIpFlag) {
          if (stClassCfg->dstIp.lowRange
           != clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.dstIp.lowRange) {
            continue;
          }
        }

        if (stClassCfg->srcIpFlag) {
          if (stClassCfg->srcIp.lowRange
           != clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.srcIp.lowRange) {
            continue;
          }
        }

        if (stClassCfg->ipTypeFlag) {
          if (stClassCfg->ipType.lowRange
           != clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.ipType.lowRange) {
            continue;
          }
        }

        if (stClassCfg->ipV4DscpFlag) {
          if (stClassCfg->ipV4Dscp.lowRange
           != clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.ipV4Dscp.lowRange) {
            continue;
          }
        }

        if (stClassCfg->srcL4PortFlag) {
          if (stClassCfg->srcL4Port.lowRange
           != clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.srcL4Port.lowRange) {
            continue;
          }
        }

        if (stClassCfg->dstL4PortFlag) {
          if (stClassCfg->dstL4Port.lowRange
           != clsCtcToDalRuleMapTab[ruleIndex].clsCtcConfigInfo.dstL4Port.lowRange) {
            continue;
          }
        }

        rule[tmp++] = gClassifyItem[ruleIndex].classifyRuleID;
      }
    }
  }

  *act = action;

  return OPL_OK;
}

/*****************************************************************************
    Func Name: dalCpuClsInit
  Description: Add some classifiers to copy some kind of frame to cpu.
        Input: void  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
void dalCpuClsInit(void)
{
    UINT16 rule=0;
    CLS_CONFIG_INFO_t ClassItemInfoSin;
    UINT8 aucBroadMac[MAC_LENGTH] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
   
    /* Copy broadcast arp to cpu. */
    memset(&ClassItemInfoSin, 0, sizeof(ClassItemInfoSin));
    ClassItemInfoSin.dstMacFlag = 1;
    ClassItemInfoSin.dstMac.lowRange[0] = 0xFF;
    ClassItemInfoSin.dstMac.lowRange[1] = 0xFF;
    ClassItemInfoSin.dstMac.lowRange[2] = 0xFF;
    ClassItemInfoSin.dstMac.lowRange[3] = 0xFF;
    ClassItemInfoSin.dstMac.lowRange[4] = 0xFF;
    ClassItemInfoSin.dstMac.lowRange[5] = 0xFF;
    ClassItemInfoSin.lenOrTypeFlag = 1;
    ClassItemInfoSin.lenOrType.lowRange = 0x0806;
    memcpy(&(ClassItemInfoSin.dstMac.highRange),aucBroadMac,MAC_LENGTH);
    ClassItemInfoSin.lenOrType.highRange = 0xffff;
    #ifndef OPL_NO_DMA1_ETH
    ClassItemInfoSin.bingress=0x00;
	ClassItemInfoSin.ingressmask=0x00;
	#endif
    ClassItemInfoSin.t_act = (DONOT_DROP|COPY_PKTS_TO_CPU);
    (void)dalVoipClsRuleCtcAdd(&ClassItemInfoSin,&rule);
    
    /*begin added by liaohongjun 2012/11/30 of EPN104QID0084*/
    /* Copy ethtype = 9001 frames to cpu.for loop detection */
    memset(&ClassItemInfoSin, 0, sizeof(ClassItemInfoSin));
    ClassItemInfoSin.dstMacFlag = 1;
    ClassItemInfoSin.dstMac.lowRange[0] = 0xFF;
    ClassItemInfoSin.dstMac.lowRange[1] = 0xFF;
    ClassItemInfoSin.dstMac.lowRange[2] = 0xFF;
    ClassItemInfoSin.dstMac.lowRange[3] = 0xFF;
    ClassItemInfoSin.dstMac.lowRange[4] = 0xFF;
    ClassItemInfoSin.dstMac.lowRange[5] = 0xFF;
    ClassItemInfoSin.lenOrTypeFlag = 1;
    ClassItemInfoSin.lenOrType.lowRange = 0x9001;
    memcpy(&(ClassItemInfoSin.dstMac.highRange),aucBroadMac,MAC_LENGTH);
    ClassItemInfoSin.lenOrType.highRange = 0xffff;
#ifndef OPL_NO_DMA1_ETH
    ClassItemInfoSin.bingress=0x00;
	ClassItemInfoSin.ingressmask=0x00;
#endif
    ClassItemInfoSin.t_act = (DONOT_DROP|COPY_PKTS_TO_CPU);
    (void)dalVoipClsRuleCtcAdd(&ClassItemInfoSin,&rule);
    /*end added by liaohongjun 2012/11/30 of EPN104QID0084*/

    /* Copy dhcp ack to cpu. */
    vosMemSet((char *)&ClassItemInfoSin, 0, sizeof(ClassItemInfoSin));
    ClassItemInfoSin.lenOrTypeFlag = 1;
    ClassItemInfoSin.lenOrType.lowRange = 0x800;
    ClassItemInfoSin.lenOrType.highRange = 0xffff;
    ClassItemInfoSin.ipTypeFlag = 1;
    ClassItemInfoSin.ipType.lowRange = 0x11;
    ClassItemInfoSin.ipType.highRange = 0xff;
    ClassItemInfoSin.srcL4PortFlag = 1;
    ClassItemInfoSin.srcL4Port.lowRange = 67;
    ClassItemInfoSin.srcL4Port.highRange = 0xffff;
    ClassItemInfoSin.dstL4PortFlag = 1;
    ClassItemInfoSin.dstL4Port.lowRange = 68;
    ClassItemInfoSin.dstL4Port.highRange = 0xffff;
  //  ClassItemInfoSin.bingress=0x01;
   // ClassItemInfoSin.ingressmask=0x01;
    ClassItemInfoSin.t_act = (DONOT_DROP|COPY_PKTS_TO_CPU);
    dalVoipClsRuleCtcAdd(&ClassItemInfoSin,&rule);

    /* Copy dhcp req to cpu. */
    vosMemSet((char *)&ClassItemInfoSin, 0, sizeof(ClassItemInfoSin));
    ClassItemInfoSin.lenOrTypeFlag = 1;
    ClassItemInfoSin.lenOrType.lowRange = 0x800;
    ClassItemInfoSin.lenOrType.highRange = 0xffff;
    ClassItemInfoSin.ipTypeFlag = 1;
    ClassItemInfoSin.ipType.lowRange = 0x11;
    ClassItemInfoSin.ipType.highRange = 0xff;
    ClassItemInfoSin.srcL4PortFlag = 1;
    ClassItemInfoSin.srcL4Port.lowRange = 68;
    ClassItemInfoSin.srcL4Port.highRange = 0xffff;
    ClassItemInfoSin.dstL4PortFlag = 1;
    ClassItemInfoSin.dstL4Port.lowRange = 67;
    ClassItemInfoSin.dstL4Port.highRange = 0xffff;
   // ClassItemInfoSin.bingress=0x01;
   // ClassItemInfoSin.ingressmask=0x01;
    ClassItemInfoSin.t_act = (DONOT_DROP|COPY_PKTS_TO_CPU);
    dalVoipClsRuleCtcAdd(&ClassItemInfoSin,&rule);
#if 0
    /* Redirect realtek header type to cpu. */
    memset(&ClassItemInfoSin, 0, sizeof(ClassItemInfoSin));
    ClassItemInfoSin.lenOrTypeFlag = 1;
    ClassItemInfoSin.lenOrType.lowRange = 0x8899;
    //memcpy(&(ClassItemInfoSin.dstMac.highRange),aucBroadMac,MAC_LENGTH);
    ClassItemInfoSin.lenOrType.highRange = 0xffff;
    #ifndef OPL_NO_DMA1_ETH
    ClassItemInfoSin.bingress=0x00;
	ClassItemInfoSin.ingressmask=0x00;
	#endif
    ClassItemInfoSin.t_act = (DROP_PKTS|COPY_PKTS_TO_CPU);
    (void)dalVoipClsRuleCtcAdd(&ClassItemInfoSin,&rule);
#else
    /* Redirect igmp frame to cpu. */
    vosMemSet((char *)&ClassItemInfoSin, 0, sizeof(ClassItemInfoSin));
    ClassItemInfoSin.lenOrTypeFlag = 1;
    ClassItemInfoSin.lenOrType.lowRange = 0x800;
    ClassItemInfoSin.lenOrType.highRange = 0xffff;
    ClassItemInfoSin.ipTypeFlag = 1;
    ClassItemInfoSin.ipType.lowRange = 0x02;
    ClassItemInfoSin.ipType.highRange = 0xff;
   // ClassItemInfoSin.bingress=0x01;
   // ClassItemInfoSin.ingressmask=0x01;
    ClassItemInfoSin.t_act = (DROP_PKTS|COPY_PKTS_TO_CPU);
    (void)dalVoipClsRuleCtcAdd(&ClassItemInfoSin,&rule);

#endif

/* Added by Einsn, copy broadcast mme to cpu 20130418 */
#ifdef CONFIG_EOC_EXTEND
    /* Copy Realtek Header packets to cpu. */
    memset(&ClassItemInfoSin, 0, sizeof(ClassItemInfoSin));
    ClassItemInfoSin.lenOrTypeFlag = 1;
    ClassItemInfoSin.lenOrType.lowRange = 0x8899;
    ClassItemInfoSin.lenOrType.highRange = 0xffff;
#ifndef OPL_NO_DMA1_ETH
    ClassItemInfoSin.bingress=0x00;
	ClassItemInfoSin.ingressmask=0x00;
#endif
    ClassItemInfoSin.t_act = (DROP_PKTS|COPY_PKTS_TO_CPU);
    (void)dalVoipClsRuleCtcAdd(&ClassItemInfoSin,&rule);

    /* Copy broadcast MME to cpu. */
    memset(&ClassItemInfoSin, 0, sizeof(ClassItemInfoSin));
    ClassItemInfoSin.dstMacFlag = 1;
    ClassItemInfoSin.dstMac.lowRange[0] = 0xFF;
    ClassItemInfoSin.dstMac.lowRange[1] = 0xFF;
    ClassItemInfoSin.dstMac.lowRange[2] = 0xFF;
    ClassItemInfoSin.dstMac.lowRange[3] = 0xFF;
    ClassItemInfoSin.dstMac.lowRange[4] = 0xFF;
    ClassItemInfoSin.dstMac.lowRange[5] = 0xFF;
    ClassItemInfoSin.lenOrTypeFlag = 1;
    ClassItemInfoSin.lenOrType.lowRange = 0x88E1;
    memcpy(&(ClassItemInfoSin.dstMac.highRange),aucBroadMac,MAC_LENGTH);
    ClassItemInfoSin.lenOrType.highRange = 0xffff;
#ifndef OPL_NO_DMA1_ETH
    ClassItemInfoSin.bingress=0x00;
	ClassItemInfoSin.ingressmask=0x00;
#endif
    ClassItemInfoSin.t_act = (DONOT_DROP|COPY_PKTS_TO_CPU);
    (void)dalVoipClsRuleCtcAdd(&ClassItemInfoSin,&rule);        
#endif 
/* End */


#if 0
    UINT8	userMac[6] = {0x01,0x00,0x5E,0x00,0x00,0x00};
	brgUsrDefineRsvMacHwWrite(0, userMac);
	brgUsrDefineRsvMacCtrlHwWrite(0, 0, 1);
#endif    
    return ;
}
#if 1
OPL_STATUS dalClsCfgShow(UINT16 usFirstItem,UINT16 usLastItem)
{
  UINT16 usCbitMapCnt;
  UINT16 usCfgCnt;
  UINT16 usShowCnt = 0;
  UINT8 *pucIpAddr = NULL;
  UINT8 aucAddr[40];
  UINT8 ucFlag = 0;
  OPL_STATUS retVal = OPL_OK; 
  
  if (0 == usFirstItem) {
    printk("Classification ID cant be set zero!\r\n");
    return OPL_ERR_INVALID_PARAMETERS;
  }

  printk("\r\n_____________________________________________________________________________\r\n");

  for (usCfgCnt = usFirstItem - 1; usCfgCnt < usLastItem; usCfgCnt++) {
    if (1 == gClassifyItem[usCfgCnt].valid) {
      ucFlag = 1;    
      usShowCnt = 0;
      printk("ClassifyNum : %-5d", usCfgCnt + 1);
      printk("Rule Precendence : %-2d", gClassifyItem[usCfgCnt].Parameter.maskPri);
      printk("RuleEntry   : %-5d(%d)",
                   gClassifyItem[usCfgCnt].classifyRuleID,gClassifyItem[usCfgCnt].shadowtbl.clsHwRuleID);
	  
      printk("MaskEntry   : %-5d",
                   gClassifyItem[usCfgCnt].shadowtbl.clsHwMaskID);
      printk("AddrEntry   : %-5d",
                   gClassifyItem[usCfgCnt].shadowtbl.clsHwAddr);
      printk("OffsetEntry   : %-5d",
                   gClassifyItem[usCfgCnt].shadowtbl.clsHwOff);

      printk("PacketType  : ");
      switch (gClassifyItem[usCfgCnt].Parameter.pktType) {
        case CLASSIFY_EII_IP:
          printk("Ethernet.\r\n");

          for (usCbitMapCnt = 0;
               usCbitMapCnt < CLASSIFY_MAX_FIELD;
               usCbitMapCnt++) {
            if (0
             != (gClassifyItem[usCfgCnt].Parameter.FieldMap.bitMap & (CLASSIFY_FIELD_MS_BIT
                                                                   >> usCbitMapCnt))) {
              if ((0 == (usShowCnt % 2)) && (0 != usShowCnt)) {
                printk("\r\n");
              }

              if (1 == (usShowCnt % 2)) {
                printk(" ");
              }

              switch (usCbitMapCnt) {
                case CLASSIFY_SOURCEMAC:
                  printk("SourceMac      : %02x:%02x:%02x:%02x:%02x:%-17x",
                               gClassifyItem[usCfgCnt].Parameter.PktRuleValue.EIIValue.SrcMac[0],
                               gClassifyItem[usCfgCnt].Parameter.PktRuleValue.EIIValue.SrcMac[1],
                               gClassifyItem[usCfgCnt].Parameter.PktRuleValue.EIIValue.SrcMac[2],
                               gClassifyItem[usCfgCnt].Parameter.PktRuleValue.EIIValue.SrcMac[3],
                               gClassifyItem[usCfgCnt].Parameter.PktRuleValue.EIIValue.SrcMac[4],
                               gClassifyItem[usCfgCnt].Parameter.PktRuleValue.EIIValue.SrcMac[5]);
                  usShowCnt++;
                  break;
                case CLASSIFY_DESTINATIONMAC:
                 printk("DestinationMac : %02x:%02x:%02x:%02x:%02x:%-17x",
                               gClassifyItem[usCfgCnt].Parameter.PktRuleValue.EIIValue.DestMac[0],
                               gClassifyItem[usCfgCnt].Parameter.PktRuleValue.EIIValue.DestMac[1],
                               gClassifyItem[usCfgCnt].Parameter.PktRuleValue.EIIValue.DestMac[2],
                               gClassifyItem[usCfgCnt].Parameter.PktRuleValue.EIIValue.DestMac[3],
                               gClassifyItem[usCfgCnt].Parameter.PktRuleValue.EIIValue.DestMac[4],
                               gClassifyItem[usCfgCnt].Parameter.PktRuleValue.EIIValue.DestMac[5]);

                  usShowCnt++;
                  break;
                case CLASSIFY_FIRST_TPID:
                  printk("FirstTpid      : 0x%-20x",
                               gClassifyItem[usCfgCnt].Parameter.PktRuleValue.EIIValue.bfFirstTpid);
                  usShowCnt++;
                  break;
                case CLASSIFY_FIRST_COS:
                  printk("FirstCos       : %-22d",
                               gClassifyItem[usCfgCnt].Parameter.PktRuleValue.EIIValue.bfFirstCos);
                  usShowCnt++;
                  break;
                case CLASSIFY_FIRST_VLAN:
                  printk("FirstVlanId    : %-22d",
                               gClassifyItem[usCfgCnt].Parameter.PktRuleValue.EIIValue.bfFirstVlan);
                  usShowCnt++;
                  break;
                case CLASSIFY_TYPE_LENGTH:
                  printk("TypeOrLength   : 0x%-20x",
                               gClassifyItem[usCfgCnt].Parameter.PktRuleValue.EIIValue.bfLenOrType);
                  usShowCnt++;
                  break;
                case CLASSIFY_VERSION:
                  printk("IpVersion      : %-22d",
                               gClassifyItem[usCfgCnt].Parameter.PktRuleValue.EIIValue.bfIpVersion);
                  usShowCnt++;
                  break;
                case CLASSIFY_TOS:
                  printk("IpTos          : %-22d",
                               gClassifyItem[usCfgCnt].Parameter.PktRuleValue.EIIValue.bfIpTos);
                  usShowCnt++;
                  break;

                case CLASSIFY_PRO_TYPE:
                  printk("IpProtocolType : %-22d",
                               gClassifyItem[usCfgCnt].Parameter.PktRuleValue.EIIValue.ipProType);
                  usShowCnt++;
                  break;
                case CLASSIFY_SOURCE_IP:
                  pucIpAddr = (UINT8 *)
                              (&(gClassifyItem[usCfgCnt].Parameter.PktRuleValue.EIIValue.srcIP));

                  printk("IpSourceIpAddr : %d.%d.%d.%d", pucIpAddr[0],
                          pucIpAddr[1],
                          pucIpAddr[2],
                          pucIpAddr[3]);
                  usShowCnt++;
                  break;
                case CLASSIFY_DESTINATION_IP:
                  pucIpAddr = (UINT8 *)
                              (&(gClassifyItem[usCfgCnt].Parameter.PktRuleValue.EIIValue.destIP));

                  printk("IpDestIpAddr   : %d.%d.%d.%d", pucIpAddr[0],
                          pucIpAddr[1],
                          pucIpAddr[2],
                          pucIpAddr[3]);
                  usShowCnt++;
                  break;
                case CLASSIFY_SOURCE_PORT:
                  printk("L4SourcePort   : %-22d",
                               gClassifyItem[usCfgCnt].Parameter.PktRuleValue.EIIValue.srcPort);
                  usShowCnt++;
                  break;
                case CLASSIFY_DESTINATION_PORT:
                  printk("L4DestPort     : %-22d",
                               gClassifyItem[usCfgCnt].Parameter.PktRuleValue.EIIValue.destPort);
                  usShowCnt++;
                  break;                         
                default:

                                        ;
              }
            }
          }

          printk("\r\n");

          break; 

        default:
          printk("%d\r\n", gClassifyItem[usCfgCnt].Parameter.pktType);
          break;
      }
      printk("\r\n_____________________________________________________________________________\r\n");
    }
  }

  if (0 == ucFlag) {
    printk("_____________________________________________________________________________\r\n");
  }
  return retVal;
}
#endif

void oplDalClsAccessLock(void)
{
	mutex_lock(&g_dalClsLock);
}

void oplDalClsAccessUnLock(void)
{
	mutex_unlock(&g_dalClsLock);
}


int dalAclListDelLowestPre()
{
    struct list_head *tmp;
    opl_acl_list_t *pClsinfo=NULL,*beg;
    struct list_head *head=&g_dal_acl_list.list;

    if(head==NULL)
    {
      return OPL_ERROR;
    }
    beg=list_entry(head,opl_acl_list_t,list);
    tmp=head->prev;	
    pClsinfo=list_entry(tmp,opl_acl_list_t,list);
    if(pClsinfo->rulepri>0)
    {
      dalOnuClsRuleCtcDel(pClsinfo);
      list_del(tmp);
      OPL_FREE(pClsinfo);	
      beg->prinum--;
      beg->tcnum--;	
      return OPL_OK;	
    }
    return OPL_ERROR;
}

opl_acl_list_t * dalAclListFindPre(UINT8 precedence)
{
    struct list_head *pos, *n;
    opl_acl_list_t *pClsinfo=NULL;
    UINT8 rulepri=precedence;
    struct list_head *head=&g_dal_acl_list.list;


    if(head==NULL)
    {
        return NULL;
    }
	for (pos = (head)->prev, n= pos->prev;pos != (head); pos = n, n = pos->prev)
	{
		pClsinfo=list_entry(pos,opl_acl_list_t,list);
	    if(rulepri==pClsinfo->rulepri)
	    {
	        return pClsinfo;
	    }
	}
    return NULL;
}

int dalAclListDel(UINT8 precedence)
{
    struct list_head *pos, *n,*del;
    opl_acl_list_t *pClsinfo=NULL;
    opl_acl_list_t *tmp=NULL,*beg=NULL;
    UINT8 rulepri=precedence;
    struct list_head *head=&g_dal_acl_list.list;

    if(head==NULL || precedence >DAL_ACL_MAX_PRI)
    {
        return OPL_ERROR;
    }

    beg=list_entry(head,opl_acl_list_t,list);
    pClsinfo=dalAclListFindPre(precedence);
    if(pClsinfo==NULL)
    {
        return OPL_ERROR;
    }
    else
    {
        del = pClsinfo->list.next;
        if (pClsinfo->dalindex != 0xFFFF)
        {
            dalOnuClsRuleCtcDel(pClsinfo);
        }

        list_del(&pClsinfo->list);
        OPL_FREE(pClsinfo);
        for (; del != head;del = del->next)
        {
            tmp=list_entry(del,opl_acl_list_t,list);
            if(tmp->rulepri > 0)
            {
                tmp->rulepri--;
                dalOnuClsRuleCtcChange(tmp,tmp->rulepri-1);
                tmp->hwshadow.clsHwMaskID=tmp->rulepri-1;
            }
        }
        beg->prinum--;
        beg->tcnum--;
    }
    return OPL_OK;
}

int dalAclListAdd(CLS_CONFIG_INFO_t *stClassCfg)
{
    struct list_head *pos, *prev, *n;
    opl_acl_list_t *pClsinfo=NULL;
    opl_acl_list_t *tmp=NULL,*beg=NULL,*pre=NULL;
    UINT8 rulepri;
    UINT8 match=0;
    int status=OPL_OK;
    struct list_head *head=&g_dal_acl_list.list;

    if(head==NULL || stClassCfg==NULL)
    {
        return OPL_ERROR;
    }

    rulepri=stClassCfg->rulePri;
    if(rulepri>DAL_ACL_MAX_PRI )
    {
        printk("precedence less than 8....\r\n");
        return OPL_ERROR;
    }
	
    pClsinfo=OPL_MALLOC(sizeof(opl_acl_list_t));
    if(pClsinfo==NULL)
    {
        printk("system out of memory....\r\n");
        return OPL_ERROR;
    }
    OPL_MEMSET(pClsinfo,0,sizeof(opl_acl_list_t));
    pClsinfo->status=DAL_ACL_STATE_ACTIVE;
    pClsinfo->rulepri=rulepri;
    OPL_MEMCPY(&(pClsinfo->stClassCfg),stClassCfg,sizeof(CLS_CONFIG_INFO_t));
    beg=list_entry(head,opl_acl_list_t,list);
    if(rulepri==0)
    {
        list_add(&pClsinfo->list,head);
        dalOnuClsRuleCtcAdd(pClsinfo);
        beg->tcnum++;
        return OPL_OK;
    }
	
    if(dalAclListFindPre(rulepri)!=NULL)
    {
        match=1;
    }
	for (pos = (head)->prev, n= pos->prev;pos != (head); pos = n, n = pos->prev)
	{
    tmp=list_entry(pos,opl_acl_list_t,list);
    //pre=list_entry(prev,opl_acl_list_t,list);
    if(match==1 && tmp->rulepri==DAL_ACL_MAX_PRI)
    {
        dalAclListDelLowestPre();
    }
    else if(match==1 && tmp->rulepri>=rulepri)
    {
        tmp->rulepri++;
        status=dalOnuClsRuleCtcChange(tmp,tmp->rulepri-1);
        tmp->hwshadow.clsHwMaskID=tmp->rulepri-1;
    }
    else if(tmp->rulepri<rulepri)
    {
        break;
    }
    }
    beg->prinum++;
    beg->tcnum++;	
    list_add(&pClsinfo->list,pos); 

    /* assign invalid dal index ,if add ctc error, don't need del ctc record */
    pClsinfo->dalindex = 0xFFFF;
    status+=dalOnuClsRuleCtcAdd(pClsinfo);
    if(status!=OPL_OK)
    {
        pClsinfo->status=DAL_ACL_STATE_READY;
        printk("add error \n");
        dalAclListDel(rulepri);
        return status;
    }
	
    return OPL_OK;	
}

/* Begin Added by 邢在美 06333, 2011-4-25 For qinq. */
OPL_STATUS dalClsValidEntryGet(UINT16 *puiClsId)
{
    UINT16 uiClsId;

    for (uiClsId = 0; uiClsId < CLASSIFY_MAX_ITEM_NUM; uiClsId++)
    {
        if (OPL_VALID != gClassifyItem[uiClsId].valid)
        {
            break;
        }
    }

    *puiClsId = uiClsId;
    
    return OPL_OK;
}

OPL_STATUS dalClsInsertVlanForIngressPort(UINT8 ucPortId, UINT16 usVid)
{
    UINT8 usMask = 1;
    UINT8 bEnable = OPL_ENABLE;
    UINT16 usClsId;
    OPL_STATUS retVal;

    retVal = dalClsValidEntryGet(&usClsId);
    if(OPL_OK != retVal)
	{
		OPL_DAL_PRINTF(("\nfunc:%s, line:%d. \n",__FUNCTION__,__LINE__));
		return retVal;
	}

    if (CLASSIFY_MAX_ITEM_NUM <= usClsId)
    {
		OPL_DAL_PRINTF(("\nfunc:%s, line:%d. \n",__FUNCTION__,__LINE__));
		return OPL_ERROR;
	}
    
    retVal = dalClsTranActCfg(usClsId, CLASSIFY_EII_IP, DONOT_DROP);
	if(OPL_OK != retVal)
	{
        OPL_DAL_PRINTF(("\nfunc:%s, line:%d. \n",__FUNCTION__,__LINE__));
		return retVal;
	}
    
    retVal = dalClsRmkVidCfg(usClsId, CLASSIFY_EII_IP, usVid);
    if(OPL_OK != retVal)
	{
        OPL_DAL_PRINTF(("\nfunc:%s, line:%d. \n",__FUNCTION__,__LINE__));
		return retVal;
	}

	retVal = dalClsIngressPortCfg(usClsId, CLASSIFY_EII_IP,ucPortId, usMask, bEnable);
	if(OPL_OK != retVal)
	{
        OPL_DAL_PRINTF(("\nfunc:%s, line:%d. \n",__FUNCTION__,__LINE__));
		return retVal;
	}

    return OPL_OK;
}
/* End   Added by 邢在美 06333, 2011-4-25 For qinq. */



