/*
=============================================================================
     Header Name: cls.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved


Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 zzhu            2007/10/10   Initial Version
----------------   ------------  ----------------------------------------------
*/
#include "classify.h"
#include "dal_cls.h"

UINT16 gFreeCnt[CLASSIFY_RULE_TABLE_GROUP];

/*Start byte address of each offset which map to the packet. */
UINT8 gClassOffset[CLASSIFY_OFFSET_NUM] = {
  0, 4, 8, 12, 16, 20
};

OPL_BOOL classifyTagMode = CLASSIFY_ONE_TAG;

/*Start byte addres of inspect windows
*which map to the Rule or Mask table
*/
UINT8 gClassSession[CLASSIFY_OFFSET_MAX_STEP_VALUE] = {
  0, 1, 3, 6, 10, 15, 21, 28, 36, 45, 55, 66
};

CLASSIFY_ITEM_t gClassifyItem[CLASSIFY_MAX_ITEM_NUM]; /* Init this array in dalClsRst() */

static UINT8 gOffsetUsedFlag[CLASSIFY_OFFSET_NUM]; /* Offset 1-8 allocation record */

CLASSIFY_ADDR_ENTRY_t *gpCaddrTbl = OPL_NULL;   /*Classify hash table pointer*/
CLASSIFY_RULE_ENTRY_t *gpCruleTbl = OPL_NULL;     /*Classify rule table pointer*/
CLASSIFY_MASK_ENTRY_t *gpCmaskTbl = OPL_NULL;     /*Classify mask table pointer*/
CLASSIFY_RULE_MALLOC_t *gpMallocCtl = OPL_NULL;

/*Classify mask information for rule*/
CLASSIFY_MASKINFO_FOR_RULE_t *gpMaskInfoForRule = NULL;


CLS_CONFIG_INFO_t gClsRuleConfigInfo[CLASSIFY_MAX_ITEM_NUM];

CLASSIFY_FIELD_ADDRLEN_t gCEIIFieldDoubleTag[] = {
  {0,48},        /*destination Mac Address 6Bytes*/
  {48,48},        /*Source Mac Address      6Bytes*/

  {96,16},        /*first tpid              2Bytes*/
  {112,3},        /*first cos               4Bits*/
  /*{115,1}*/     /*CFI*/
  {116,12},       /*first vlan              12Bits*/

  {128,16},       /*second tpid             2Bytes*/
  {144,3},        /*second cos              4Bits*/
  /*{147,1}*/     /*CFI*/
  {148,12},       /*second vlan             12Bits*/

  {160,16},       /*type or length field    2Bytes*/

  {176,4},        /*ip version              4Bits*/
  /*{180,4},*/    /*ip head length          4Bits*/
  {184,8},        /*ip tos                  1Bytes*/
  /*{192,16},*/   /*ip pkt length           2Bytes*/
  /*{208,16},*/   /*ip identifier           2Bytes*/
  /*{224,3},*/    /*ip slice flag           3Bits*/
  /*{227,13},*/   /*ip slice offset         13Bits*/
  /*{240,8},*/    /*ip ttl                  1Bytes*/
  {248,8},        /*ip protocol type        1Bytes*/
  /*{256,16},*/   /*ip header check sum*/
  {272,32},       /*ip source address       4Bytes*/
  {304,32},       /*ip destination address  4Bytes*/

  {336,16},       /*L4 source port          2Bytes*/
  {352,16},       /*L4 destination port     2Bytes*/

  {368,32}        /*user define field       4Bytes*/
};

CLASSIFY_FIELD_ADDRLEN_t gCPPPoEFieldDoubleTag[] = {
  {0,48},        /*destination Mac Address 6Bytes*/
  {48,48},        /*Source Mac Address      6Bytes*/

  {96,16},        /*first tpid              2Bytes*/
  {112,3},        /*first cos               4Bits*/
  /*{115,1}*/     /*CFI*/
  {116,12},       /*first vlan              12Bits*/

  {128,16},       /*second tpid             2Bytes*/
  {144,3},        /*second cos              4Bits*/
  /*{147,1}*/     /*CFI*/
  {148,12},       /*second vlan             12Bits*/

  {160,16},       /*type or length field    2Bytes*/
  /*{176, 4}*/    /*version  */
  /*{180,4}*/     /*type*/
  {184,8},        /*code            1Bytes*/
  /*{192,16},*/   /*session ID        2Bytes*/
  /*{208,16}*/    /*length*/
  {224,16},       /*PPP protocol type     2Bytes*/
  {240,8},        /*lcp/ipcp code type    1Bytes*/
  {240,4},        /*ipVersion         4Bits*/

  /*{244,4},*/    /*ip head length          4Bits*/
  {248,8},        /*ip tos                  1Bytes*/
  /*{192,16},*/   /*ip pkt length           2Bytes*/
  /*{208,16},*/   /*ip identifier           2Bytes*/
  /*{224,3},*/    /*ip slice flag           3Bits*/
  /*{227,13},*/   /*ip slice offset         13Bits*/
  /*{240,8},*/    /*ip ttl                  1Bytes*/
  {312,8},        /*ip protocol type        1Bytes*/
  /*{256,16},*/   /*ip header check sum*/
  {336,32},       /*ip source address       4Bytes*/
  {368,32},       /*ip destination address  4Bytes*/

  {400,16},       /*L4 source port          2Bytes*/
  {416,16},       /*L4 destination port     2Bytes*/

  {368,32}        /*user define field       4Bytes*/
};

CLASSIFY_FIELD_ADDRLEN_t gCEIIFieldSingleTag[] = {
  {0,48},        /*destination Mac Address 6Bytes*/
  {48,48},        /*Source Mac Address      6Bytes*/

  {96,16},        /*first tpid              2Bytes*/
  {112,3},        /*first cos               4Bits*/
  /*{115,1}*/     /*CFI*/
  {116,12},       /*first vlan              12Bits*/
  #ifdef ONLY_SUPPORT_SINGLE_TAG
  #else
  {128,16},       /*second tpid             2Bytes*/
  {144,3},        /*second cos              4Bits*/
  /*{147,1}*/     /*CFI*/
  {148,12},       /*second vlan             12Bits*/
  #endif
  {128,16},       /*type or length field    2Bytes*/

  {144,4},        /*ip version              4Bits*/
  /*{148,4},*/    /*ip head length          4Bits*/
  {152,8},        /*ip tos                  1Bytes*/
  /*{160,16},*/   /*ip pkt length           2Bytes*/
  /*{176,16},*/   /*ip identifier           2Bytes*/
  /*{192,3},*/    /*ip slice flag           3Bits*/
  /*{195,13},*/   /*ip slice offset         13Bits*/
  /*{208,8},*/    /*ip ttl                  1Bytes*/
  {216,8},        /*ip protocol type        1Bytes*/
  /*{224,16},*/   /*ip header check sum*/
  {240,32},       /*ip source address       4Bytes*/
  {272,32},       /*ip destination address  4Bytes*/

  {304,16},       /*L4 source port          2Bytes*/
  {320,16},       /*L4 destination port     2Bytes*/

  {336,32}        /*user define field       4Bytes*/
};

CLASSIFY_FIELD_ADDRLEN_t gCPPPoEFieldSingleTag[] = {
  {0,48},        /*destination Mac Address 6Bytes*/
  {48,48},        /*Source Mac Address      6Bytes*/

  {96,16},        /*first tpid              2Bytes*/
  {112,3},        /*first cos               4Bits*/
  /*{115,1}*/     /*CFI*/
  {116,12},       /*first vlan              12Bits*/
  #ifdef ONLY_SUPPORT_SINGLE_TAG
  #else
  {128,16},       /*second tpid             2Bytes*/
  {144,3},        /*second cos              4Bits*/
  /*{147,1}*/     /*CFI*/
  {148,12},       /*second vlan             12Bits*/
  #endif
  {128,16},       /*type or length field    2Bytes*/
  /*{144, 4}*/    /*version  */
  /*{148,4}*/     /*type*/
  {152,8},        /*code            1Bytes*/
  /*{160,16},*/   /*session ID        2Bytes*/
  /*{176,16}*/    /*length*/
  {192,16},       /*PPP protocol type     2Bytes*/
  {208,8},        /*lcp/ipcp code type    1Bytes*/
  {208,4},        /*ipVersion         4Bits*/

  /*{212,4},*/    /*ip head length          4Bits*/
  {216,8},        /*ip tos                  1Bytes*/
  /*{192 - 32,16},*/   /*ip pkt length           2Bytes*/
  /*{208 - 32,16},*/   /*ip identifier           2Bytes*/
  /*{224 - 32,3},*/    /*ip slice flag           3Bits*/
  /*{227 - 32,13},*/   /*ip slice offset         13Bits*/
  /*{240 - 32,8},*/    /*ip ttl                  1Bytes*/
  {280,8},        /*ip protocol type        1Bytes*/
  /*{256 - 32,16},*/   /*ip header check sum*/
  {304,32},       /*ip source address       4Bytes*/
  {336,32},       /*ip destination address  4Bytes*/

  {368,16},       /*L4 source port          2Bytes*/
  {484,16},       /*L4 destination port     2Bytes*/

  {336,32}        /*user define field       4Bytes*/
};
OPL_STATUS clsWindowRangeCheck(UINT8 *pMask)
{
  UINT32 index;
  UINT32 *p = (UINT32 *) pMask;

  if (pMask == OPL_NULL) {
    return OPL_ERR_NULL_POINTER;
  }

  for (index = OPL_ZERO; index < CLASSIFY_FOFFSET_6; index++) {
    if (p[index] == 0x0000) {
      return OPL_OK;
    }
  }
  return CLASSIFY_WINDOWS_OUTOF_RANGE;
}
/**/
OPL_STATUS clsWinOffsetUsefulCheck(UINT16 prestatbit,
                                   UINT16 prelength,
                                   UINT16 startbit,
                                   UINT16 length)
{
  OPL_STATUS iStatus;

  UINT16 offsetHead5;
  UINT16 offsetTail5;
  UINT16 offsetHead6;
  UINT16 offsetTail6;
  UINT32 index;
  if ((gOffsetUsedFlag[0] == OPL_TRUE)
   && (gOffsetUsedFlag[1] == OPL_TRUE)
   && (gOffsetUsedFlag[2] == OPL_TRUE)
   && (gOffsetUsedFlag[3] == OPL_TRUE)
   && (gOffsetUsedFlag[4] == OPL_TRUE)) {
    if (gOffsetUsedFlag[5] == OPL_TRUE) {
      for (index = 0; index < CLASSIFY_OFFSET_MAX_STEP_VALUE; index++) {
        iStatus = clsOffsetHeadTailCal(CLASSIFY_FOFFSET_6,
                                       index,
                                       &offsetHead6,
                                       &offsetTail6);
        if (OPL_OK != iStatus) {
          return iStatus;
        }
        if ((offsetHead6 <= startbit) && (offsetTail6 >= (startbit + length))) {
          return OPL_OK;
        }
      }

      return CLASSIFY_WINDOWS_OUTOF_RANGE;
    }
    else {
      for (index = 0; index < CLASSIFY_OFFSET_MAX_STEP_VALUE; index++) {
        iStatus = clsOffsetHeadTailCal(CLASSIFY_FOFFSET_6,
                                       index,
                                       &offsetHead6,
                                       &offsetTail6);
        if (OPL_OK != iStatus) {
          return iStatus;
        }
        iStatus = clsOffsetHeadTailCal(CLASSIFY_FOFFSET_5,
                                       index,
                                       &offsetHead5,
                                       &offsetTail5);
        if (OPL_OK != iStatus) {
          return iStatus;
        }
        if (((offsetHead6 <= startbit) && (offsetTail6 >= (startbit + length)))
         || ((offsetHead5 <= prestatbit)
          && (offsetTail6 >= (startbit + length)))) {
          return OPL_OK;
        }
      }
      return CLASSIFY_WINDOWS_OUTOF_RANGE;
    }
  }
  return OPL_OK;
}
/*******************************************************************************
*
* clsMaskAdd - Add a specific classification mask entry.
*
* DESCRIPTION:
*       this function used to generate classify mask entry and flow mask entry
*       accordaning the input parameter, and then malloc the rule and mask entry
*       calculate the address and then write mask and rule into the mask and rule
*       table
*
* INPUT:
*        pstMaskPar - classification and flow parameter.
*
* OUTPUT:
*        MaskID - classify rule table entry ID.
*
* RETURNS:
*       OPL_OK,                       If the operation is successful.
*       PARAMETER_INPUT_POINTER_NULL, If the input pointer is NULL.
*
* SEE ALSO:
*/

OPL_STATUS clsMaskAdd(CLASSIFY_MASK_PARAMETER_t *pMaskPar,UINT16 maskID)
{
  CLASSIFY_MASK_ENTRY_t ClassMaskEntry;

  OPL_STATUS iStatus;

  if (NULL == pMaskPar) {
    return OPL_ERR_NULL_POINTER;
  }

  /*Set and mask*/
  switch (pMaskPar->pktType) {
    case CLASSIFY_EII_IP:
      iStatus = clsEIIMaskMake(pMaskPar, &ClassMaskEntry);
      break;

    case CLASSIFY_PPPOE_IP:
      iStatus = clsPPPoeMaskMake(pMaskPar, &ClassMaskEntry);
      break;

    default:
      return CLASSIFY_INVALID_PKT_TYPE;
  }

  if (OPL_OK != iStatus) {
    return iStatus;
  }

  /* Record mask information for rule */

  gpMaskInfoForRule[maskID].fieldMap = pMaskPar->FieldMap.bitMap ;
  gpMaskInfoForRule[maskID].MaskOffset.offsetValue[0] = ClassMaskEntry.bfOffset1;
  gpMaskInfoForRule[maskID].MaskOffset.offsetValue[1] = ClassMaskEntry.bfOffset2;
  gpMaskInfoForRule[maskID].MaskOffset.offsetValue[2] = ClassMaskEntry.bfOffset3;
  gpMaskInfoForRule[maskID].MaskOffset.offsetValue[3] = ClassMaskEntry.bfOffset4;
  gpMaskInfoForRule[maskID].MaskOffset.offsetValue[4] = ClassMaskEntry.bfOffset5;
  gpMaskInfoForRule[maskID].MaskOffset.offsetValue[5] = ClassMaskEntry.bfOffset6;


  gpMaskInfoForRule[maskID].ingressMask = pMaskPar->ingressMask;
  gpMaskInfoForRule[maskID].egressMask = pMaskPar->egressMask;

  switch (pMaskPar->pktType) {
    case CLASSIFY_EII_IP:
      gpMaskInfoForRule[maskID].UsrMask.EIIUsrMask = pMaskPar->UsrMask.EIIUsrMask;
      break;
    case CLASSIFY_PPPOE_IP:
      gpMaskInfoForRule[maskID].UsrMask.PPPoeUsrMask = pMaskPar->UsrMask.PPPoeUsrMask;
      break;
    default:
      return CLASSIFY_INVALID_PKT_TYPE;
  }

  OPL_MEMCPY(gpMaskInfoForRule[maskID].Mask,
             ClassMaskEntry.Mask,
             CLASSIFY_KEYVALUE_LENGTH);

  ClassMaskEntry.bfEmask = pMaskPar->egressMask;
  ClassMaskEntry.bfImask = pMaskPar->ingressMask;


  iStatus = clsMaskTblEntryAdd(&ClassMaskEntry, maskID);
  if (OPL_OK != iStatus) {
    return iStatus;
  }

  return OPL_OK;
}



/*******************************************************************************
*
* clsMaskDel - Delete classification mask entry directly.
*
* DESCRIPTION:
*       This function used to delete a specific classification mask entry. Disable
*   the valid bit and clear the data.
*
* INPUT:
*        MaskID       - Classification Mask ID.
*
* OUTPUT:
*        None.
*
* RETURNS:
*       OPL_OK,                       If the operation is successful.
*
* SEE ALSO:
*/
OPL_STATUS clsMaskDel(UINT8 maskID)
{
  CLASSIFY_MASK_ENTRY_t ClassMaskEntry;

  OPL_BZERO(&ClassMaskEntry, sizeof(CLASSIFY_MASK_ENTRY_t));
  /*ClassMaskEntry.bfValid = 0;*/

  return dalClsMaskTblWr(&ClassMaskEntry, maskID);
}


/*******************************************************************************
*
* clsRuleAdd - configure rule and mask into classify rule and classify mask
*                    table
*
* DESCRIPTION:
*       this function used to generate classify mask entry and flow mask entry
*       accordanve the input parameter, and then malloc the rule and mask entry
*       calculate the address and then write mask and rule into the mask and rule
*       table
*
* INPUT:
*        pstRulePar - classification and flow parameter.
*
* OUTPUT:
*        pusCruleID - classify rule table entry ID.
*
* RETURNS:
*       OPL_OK,                       If the operation is successful.
*       PARAMETER_INPUT_POINTER_NULL, If the input pointer is NULL.
*
* SEE ALSO:
*/

OPL_STATUS clsRuleAdd(CLASSIFY_RULE_PARAMETER_t *pRulePar,
                      UINT16 maskID,
                      UINT16 FlowMaskID,
                      UINT16 *pRuleID)
{
  CLASSIFY_RULE_ENTRY_t ClassRuleEntry;

  OPL_STATUS iStatus;

  if ((NULL == pRulePar) || (NULL == pRuleID)) {
    return OPL_ERR_NULL_POINTER;
  }

  /*Set rule and mask*/
  switch (pRulePar->pktType) {
    case CLASSIFY_EII_IP:
      iStatus = clsEIIRuleMake(pRulePar, &ClassRuleEntry, maskID);
      break;

    case CLASSIFY_PPPOE_IP:
      iStatus = clsPPPoeRuleMake(pRulePar, &ClassRuleEntry, maskID);

      break;

    default:
      return CLASSIFY_INVALID_PKT_TYPE;
  }

  if (OPL_OK != iStatus) {
    return iStatus;
  }

  ClassRuleEntry.bfTranActH = pRulePar->RuleEntry.bfTranActH;
  ClassRuleEntry.bfTranActL = pRulePar->RuleEntry.bfTranActL;
  ClassRuleEntry.bfRmkAct = pRulePar->RuleEntry.bfRmkAct;
  ClassRuleEntry.bfRmkCos = pRulePar->RuleEntry.bfRmkCos;
  ClassRuleEntry.bfRmkIcos = pRulePar->RuleEntry.bfRmkIcos;
  ClassRuleEntry.bfRmkVid = pRulePar->RuleEntry.bfRmkVid;

  ClassRuleEntry.bfMsel = maskID;

  ClassRuleEntry.bfEgress = ((pRulePar->RuleEntry.bfEgress) & (gpMaskInfoForRule[maskID].egressMask));
  ClassRuleEntry.bfIngress = ((pRulePar->RuleEntry.bfIngress) & (gpMaskInfoForRule[maskID].ingressMask));

  return clsRuleTblEntryAdd(&ClassRuleEntry, pRuleID);
}


/*******************************************************************************
*
* clsRuleDel - Delete classification rule entry directly.
*
* DESCRIPTION:
*       This function used to delete a specific classification rule entry directly.
*       Calculate the index in hash table and clear it.
*
* INPUT:
*        MaskID       - Classification Mask ID.
*
* OUTPUT:
*        None.
*
* RETURNS:
*       OPL_OK,                       If the operation is successful.
*
* SEE ALSO:
*/
OPL_STATUS clsRuleDel(UINT16 ruleID)
{
  CLASSIFY_RULE_ENTRY_t ClassRuleEntry;
  UINT16 ruleAddr;
  UINT16 addrOff;
  OPL_STATUS iStatus;


  iStatus = clsRuleAddrGet(ruleID, &ruleAddr, &addrOff);

  if (OPL_OK != iStatus) {
    return iStatus;
  }

  OPL_BZERO(&ClassRuleEntry, sizeof(CLASSIFY_RULE_ENTRY_t));

  iStatus = dalClsAddrTblWr(ruleAddr, addrOff, 0);
  if (OPL_OK != iStatus) {
    return iStatus;
  }

  iStatus = dalClsRuleTblWr(&ClassRuleEntry, ruleID);
  clsRuleFree(ruleID);

  return iStatus;
}



/*******************************************************************************
*
* clsAddrCal    - Calculate the classify rule entry index.
*
* DESCRIPTION:
*    Calculate the classify rule entry index.
*
* INPUT:
*    pRuleEntry - Pointer to entry of classify rule table.
*
* OUTPUT:
*    pAddr    - Pointer to entry of classify address table.
*
* RETURNS:
*        OPL_OK             - If the operation is successful.
*        OPL_ERR_NULL_POINTER     - If the input pointer is NULL.
*    Others                     -   an appropriate specific error code defined
*                     for CLT device driver.
*
* SEE ALSO:
*/
OPL_STATUS clsAddrCal(CLASSIFY_RULE_ENTRY_t *pRuleEntry,UINT16 *pAddr)
{
  OPL_STATUS iStatus;
  UINT8 Cnt;
  UINT8 xorAddr = 4;
  UINT8 Temp[CLASSIFY_CAL_VALUE_LEN];
  UINT8 ruleAddr = OPL_ZERO;

  if ((OPL_NULL == pRuleEntry) || (OPL_NULL == pAddr)) {
    return  OPL_ERR_NULL_POINTER;
  }

  /*The value format which used to calculate the */
  /*Byte0  Byte1  Byte2 Byte3   Byte4                      Byt15*/
  /*6h'0 Eport ID 6h'0 Iport ID Classify rule Value after pucker*/
  OPL_BZERO(Temp, CLASSIFY_CAL_VALUE_LEN);
  OPL_MEMCPY((Temp + xorAddr), pRuleEntry->Rule, CLASSIFY_RULE_PUCKER_LEN);

  /*32Bytes will be XOR then puckered into 16Bytes*/
  for (Cnt = 0; Cnt < CLASSIFY_RULE_PUCKER_LEN; Cnt++) {
    Temp[Cnt + xorAddr] ^= (pRuleEntry->Rule[Cnt + CLASSIFY_RULE_PUCKER_LEN]);
  }

  Temp[1] = (UINT8) pRuleEntry->bfEgress;  /* Add Eport and Iport in the head */
  Temp[3] = (UINT8) pRuleEntry->bfIngress;
  /*Calculate the hash address which used to index classification address table.*/
  iStatus = TableAddrCal8(Temp,
                          CLASSIFY_CAL_VALUE_LEN,
                          CLASSIFY_ADDRESS_LENGTH,
                          &ruleAddr);
  if (OPL_OK != iStatus) {
    return iStatus;
  }
  *pAddr = ruleAddr;
  return OPL_OK;
}


/*******************************************************************************
*
* clsMaskMalloc - Malloc memory for specified classify mask entry.
*
* DESCRIPTION:
*   This function malloc memory for specified classify mask entry. Mainly
*   divided into two situation: specified classify mask entry already
*   exist, or is a new entry. NO matter which situation, the mask table
*   need be adjusted.
*
* INPUT:
*    pstMaskEntry - Pointer to mask entry.
*
* OUTPUT:
*    pucMaskID    - Pointer to index of flow mask entry.
*    pucMaskWr    - Pointer to parameter indicating write or not write.
*
* RETURNS:
*        OPL_OK             - If the operation is successful.
*        OPL_ERR_NULL_POINTER     - If the input pointer is NULL.
*    Others                     -   an appropriate specific error code defined
*                     for CLT device driver.
*
* SEE ALSO:
*/
OPL_STATUS clsMaskMalloc(CLASSIFY_MASK_ENTRY_t *pMaskEntry,UINT8 *pMaskID)
{
  OPL_STATUS iStatus;
  UINT8 maskLastCnt;
  UINT8 ucMaskInsert;
  UINT8 maskCnt;
  UINT8 ucSrcMask;

  UINT8 selMaskID;

  if ((OPL_NULL == pMaskEntry) || (OPL_NULL == pMaskEntry)) {
    return  OPL_ERR_NULL_POINTER;
  }

  iStatus = clsMaskSel(pMaskEntry, &selMaskID);
  if (OPL_OK == iStatus) {
    gpCmaskTbl[selMaskID].referenceCount++;

    if (pMaskEntry->maskPri <= gpCmaskTbl[selMaskID].maskPri) {
      /*Needn't update the mask entry priority and location.*/
      *pMaskID = selMaskID;
      return OPL_OK;
    }
    else {
      for (ucMaskInsert =CLASSIFY_MIN_MASK_ENTRY;
           ucMaskInsert < CLASSIFY_MAX_MASK_ENTRY;
           ucMaskInsert++) {
        if (pMaskEntry->maskPri > gpCmaskTbl[ucMaskInsert].maskPri) {
          break;
        }
      }

      if (ucMaskInsert == selMaskID) {
        gpCmaskTbl[selMaskID].maskPri = pMaskEntry->maskPri;
        /*Update the mask entry priority and but not change the location.*/
        *pMaskID = selMaskID;
        return OPL_OK;
      }

      pMaskEntry->referenceCount = gpCmaskTbl[selMaskID].referenceCount;
      pMaskEntry->bfValid = 1;

      clsMselFlagSet(selMaskID, CLASSIFY_MSEL_SET_FLAG);
      for (maskCnt = selMaskID; maskCnt > ucMaskInsert; maskCnt--) {
        ucSrcMask = maskCnt - 1;
        OPL_MEMCPY(&(gpCmaskTbl[maskCnt]),
                   &(gpCmaskTbl[ucSrcMask]),
                   sizeof(gpCmaskTbl[maskCnt]));
        dalClsMaskHwWr(&(gpCmaskTbl[ucSrcMask]), maskCnt);

        /*update the MSEL field of the rule entry which associate with the
          usSrcMask.*/
        clsMselModByMask(ucSrcMask, maskCnt);
      }

      OPL_MEMCPY(&(gpCmaskTbl[ucMaskInsert]),
                 pMaskEntry,
                 sizeof(gpCmaskTbl[ucMaskInsert]));
      dalClsMaskHwWr(pMaskEntry, ucMaskInsert);
      clsMselModByFlag(ucMaskInsert);
      clsMselFlagSet(selMaskID, CLASSIFY_MSEL_CLR_FLAG);

      *pMaskID = ucMaskInsert;
      return OPL_OK;
    }
  }

  /*Search for a NULL entry.*/
  for (maskLastCnt = CLASSIFY_MIN_MASK_ENTRY;
       maskLastCnt < CLASSIFY_MAX_MASK_ENTRY;
       maskLastCnt++) {
    if (0 == gpCmaskTbl[maskLastCnt].bfValid) {
      break;
    }
  }

  if (maskLastCnt >= CLASSIFY_MAX_MASK_ENTRY) {
    return CLASSIFY_MASK_TABLE_FULL;
  }

  /*If the first entry is NULL, so there needn't to adjust the entry.*/
  if (0 == maskLastCnt) {
    gpCmaskTbl[maskLastCnt].bfValid = OPL_VALID;
    gpCmaskTbl[maskLastCnt].referenceCount++;
    *pMaskID = maskLastCnt;
    return OPL_OK;
  }

  /*Get the location where the new mask entry should be inserted into
    the Mask table, the bigger usMaskPri value the higher priority level,
    then should be inserted front(the smaller Mask ID).*/
  for (ucMaskInsert = CLASSIFY_MIN_MASK_ENTRY;
       ucMaskInsert < CLASSIFY_MAX_MASK_ENTRY;
       ucMaskInsert++) {
    if (0 == gpCmaskTbl[ucMaskInsert].bfValid) {
      /*ucMaskInsert should be equal to maskLastCnt*/
      break;
    }

    if (pMaskEntry->maskPri > gpCmaskTbl[ucMaskInsert].maskPri) {
      /*maskLastCnt should be largger than ucMaskInsert*/
      break;
    }
  }

  if (ucMaskInsert != maskLastCnt) {
    for (maskCnt = maskLastCnt; maskCnt > ucMaskInsert; maskCnt--) {
      ucSrcMask = maskCnt - 1;

      OPL_MEMCPY(&(gpCmaskTbl[maskCnt]),
                 &(gpCmaskTbl[ucSrcMask]),
                 sizeof(gpCmaskTbl[maskCnt]));

      dalClsMaskHwWr(&(gpCmaskTbl[ucSrcMask]), maskCnt);

      /*update the MSEL field of the rule entry which associate with the
          usSrcMask.*/

      clsMselModByMask(ucSrcMask, maskCnt);
    }
  }

  gpCmaskTbl[ucMaskInsert].referenceCount = OPL_TRUE;
  gpCmaskTbl[ucMaskInsert].bfValid = OPL_TRUE;
  *pMaskID = ucMaskInsert;
  return OPL_OK;
}


/*******************************************************************************
*
* clsMaskFree - Free specified entry of classify mask table.
*
* DESCRIPTION:
*   This function free specified entry of classify mask table. If only one
*   classify rule refer to the entry, adjust the entries remain after
*   free action. If several rules refer to it, just decrease the reference
*   number.
*
* INPUT:
*    maskID   - Index of entry of classify mask table.
*
* OUTPUT:
*    NULL.
*
* RETURNS:
*    OPL_OK             - If the operation is successful.
*    CLASSIFY_FREE_INVALID_MENTRY - Mask entry to be freed is invalid.
*    Others                     -   an appropriate specific error code defined
*                     for CLT device driver.
* SEE ALSO:
*/
OPL_STATUS clsMaskFree(UINT8 maskID)
{
  OPL_STATUS iStatus = OPL_OK;
  UINT8 maskCnt;
  UINT8 destMask;
  UINT8 clrMask;


  if ((0 == gpCmaskTbl[maskID].bfValid)
   || (0 == gpCmaskTbl[maskID].referenceCount)) {
    /*This judgement only used to debug code. If the table maintain is
      correct,  this situation should not take place.*/
    return CLASSIFY_FREE_INVALID_MENTRY;
  }

  if (1 == gpCmaskTbl[maskID].referenceCount) {
    if (((CLASSIFY_MAX_MASK_ENTRY - 1) == maskID)
     || (0 == (gpCmaskTbl[maskID + 1].bfValid))) {
      /*If the free mask is the last entry or the next entry isn't valid,
        It need not to be adjust the mask table, free it only.*/
      gpCmaskTbl[maskID].referenceCount = 0;
      gpCmaskTbl[maskID].bfValid = 0;
      clrMask = maskID;
    }
    else {
      /*Check whether there are valid entry below current mask entry,
        any valid mask entry below the current entry must be move up
        one by one, valid mask entry must be consecutive in the mask
        table.*/
      for (maskCnt = maskID + 1;
           maskCnt < CLASSIFY_MAX_MASK_ENTRY;
           maskCnt++) {
        OPL_LOG(&gbClsDbg,
                &gbClsFileDbg,
                "In clsMaskFree, maskCnt=%d",
                maskCnt);
        OPL_LOG(&gbClsDbg,
                &gbClsFileDbg,
                "In clsMaskFree, bfValid=%d\n",
                gpCmaskTbl[maskCnt].bfValid);
        if (0 == gpCmaskTbl[maskCnt].bfValid) {
          break;
        }

        destMask = maskCnt - 1;
        OPL_MEMCPY(&(gpCmaskTbl[destMask]),
                   &(gpCmaskTbl[maskCnt]),
                   sizeof(gpCmaskTbl[destMask]));
        dalClsMaskHwWr(&(gpCmaskTbl[maskCnt]), destMask);


        /*All MSEL field of the rule which equal to maskCnt should be
          change to destMask.*/
        clsMselModByMask(maskCnt, destMask);
      }

      /*The last entry which has been move up need to be free.*/
      clrMask = maskCnt - 1;
      gpCmaskTbl[clrMask].referenceCount = 0;
      gpCmaskTbl[clrMask].bfValid = 0;
    }

    /*Clear both local and ASIC mask entry to 0.*/
    OPL_BZERO(&(gpCmaskTbl[clrMask]), sizeof(gpCmaskTbl[clrMask]));
    iStatus = dalClsMaskHwWr(&(gpCmaskTbl[clrMask]), clrMask);
  }
  else {
    gpCmaskTbl[maskID].referenceCount--;
    /*Need to resort the entry by the old priority? but where can get the old
      information?*/
  }

  return iStatus;
}


/*******************************************************************************
*
* clsRuleFree - Free the space for specified classify rule ID.
*
* DESCRIPTION:
*   This function free the space for specified classify rule ID. Two factors,
*   free space counter for specified group, and malloc flag.
*
* INPUT:
*    ruleID   - Classify rule ID.
*
* OUTPUT:
*    NULL.
*
* RETURNS:
*        NULL.
*
* SEE ALSO:
*/
void clsRuleFree(UINT16 ruleID)
{
  UINT16 ruleGroup;

  ruleGroup = ruleID / CLASSIFY_ENTRYS_PER_GROUP;
  gFreeCnt[ruleGroup]++;
  gpMallocCtl[ruleID].mallocFlag = 0;
}


/*******************************************************************************
*
* clsRuleMalloc - Malloc the rule ID and address offset for certain
*               address entry.
*
* DESCRIPTION:
*   This function malloc the rule ID and address offset for certain address
*   entry. Firstly, check if there is any space in the hash address. Then,
*   check which group has the most free ID. Finally, calculate the rule
*   ID and address offset.
*
* INPUT:
*    address    - Index of classify address table.
*
* OUTPUT:
*    pRuleID    - Pointer to index of entry of classify rule table.
*    pAddrOff   - Pointer to offset of certain entry of classify address table.
*
* RETURNS:
*        OPL_OK             - If the operation is successful.
*        OPL_ERR_NULL_POINTER     - If the input pointer is NULL.
*    CLASSIFY_ADDR_TABLE_8COLLISION - The entry of classify address table is full.
*    CLASSIFY_RULE_TABLE_FULL   - All the classify rule table is full.
*
* SEE ALSO:
*/
OPL_STATUS clsRuleMalloc(UINT16 address,UINT16 *pRuleID,UINT16 *pAddrOff)
{
  UINT16 cruleID;
  UINT8 addrCnt;
  UINT16 ruleCnt;
  UINT8 AddrCol[CLASSIFY_RULE_TABLE_GROUP];
  UINT8 colliCnt = 0;

  UINT8 groupCnt;
  UINT8 mostFreeGroup;
  UINT16 maxFreeNum;


  if ((NULL == pRuleID) || (NULL == pAddrOff)) {
    return OPL_ERR_NULL_POINTER;
  }

  OPL_BZERO(AddrCol, CLASSIFY_RULE_TABLE_GROUP);

  for (addrCnt = 0; addrCnt < CLASSIFY_RULE_TABLE_GROUP; addrCnt++) {
    if (0 != gpCaddrTbl[address].ClasAddr[addrCnt]) {
      colliCnt++;
      AddrCol[addrCnt] = 1;
    }
  }

  if (CLASSIFY_RULE_TABLE_GROUP == colliCnt) {
    /*Stastics*/
    return CLASSIFY_ADDR_TABLE_8COLLISION;
  }

  /*Get Rule ID from the one free list of the 8 group*/
  mostFreeGroup = 0;
  maxFreeNum = 0;

  for (groupCnt = 0; groupCnt < CLASSIFY_RULE_TABLE_GROUP; groupCnt++) {
    if ((gFreeCnt[groupCnt] > maxFreeNum)
     && (0 == gpCaddrTbl[address].ClasAddr[groupCnt])) {
      maxFreeNum = gFreeCnt[groupCnt];
      mostFreeGroup = groupCnt;
    }
  }

  if (0 == maxFreeNum) {
    /*Need to adjust the table.*/
    return CLASSIFY_RULE_TABLE_FULL;
  }

  *pAddrOff = mostFreeGroup;
  gpCaddrTbl[address].ClasAddr[mostFreeGroup] = 0xFFFF;/*Malloced flag.*/

  ruleCnt = CLASSIFY_ENTRYS_PER_GROUP * mostFreeGroup + 1;

  cruleID = ruleCnt + CLASSIFY_ENTRYS_PER_GROUP - 1;
  for (; ruleCnt < cruleID; ruleCnt++) {
    if (0 == gpMallocCtl[ruleCnt].mallocFlag) {
      gpMallocCtl[ruleCnt].mallocFlag = 1;
      *pRuleID = ruleCnt;
      gFreeCnt[mostFreeGroup]--;

      return OPL_OK;
    }
  }

  return CLASSIFY_RULE_TABLE_FULL;
}




/*******************************************************************************
*
* clsEIIMaskRuleMake - Make classify mask and rule table entry.
*
* DESCRIPTION:
*       This function mainly divided into two parts. Firstly, dicide the window
*       offset through the field list of inputting parameter. Secondly, set
*       the mask and rule table entry by the data of the field. But you should
*       note that if the field of the parameter is not byte alignment, you
*       should do some shift operation first before setting.
*
* INPUT:
*        pRulePar -         Pointer to parameter inputing.
*
* OUTPUT:
*        pMaskEntry -       Pointer to classify mask table entry.
*        pRuleEntry -       Pointer to classify rule table entry.
*
* RETURNS:
*        OPL_OK                         -       Operation is successful.
*        OPL_ERR_NULL_POINTER       -       Input pointer is null.
*    Others                     -     An appropriate specific error code defined
*                       for CLT device driver.
*        CLASSIFY_NULL_FIELDMAP       -       FieldMap is null.
*        CLASSIFY_INVALID_FIELD_TYPE    -       Field type is wrong.
*        CLASSIFY_WINDOWS_OUTOF_RANGE   -       Input index of parameter exceeds
*                                               maximal defined index.
*        CLASSIFY_INVALID_FOFFSET_NUM   -       Invalid offset ID.
*
* SEE ALSO:
*/
OPL_STATUS clsEIIMaskRuleMake(CLASSIFY_PARAMETER_t *pMaskRulePar,
                              CLASSIFY_MASK_ENTRY_t *pMaskEntry,
                              CLASSIFY_RULE_ENTRY_t *pRuleEntry)
{
  OPL_STATUS iStatus = OPL_OK;
  UINT8 byteCnt = 0;

  UINT8 MaskValue[CLASSIFY_KEYVALUE_LENGTH];
  UINT8 RuleValue[CLASSIFY_KEYVALUE_LENGTH];
#ifndef OPCONN_BIG_ENDIAN
  UINT8 wordCnt;
  UINT8 srcOff;
  UINT8 destOff;
#endif
  CLASSIFY_MASK_OFFSET_FIELD_t Offset;

  if ((NULL == pMaskRulePar) || (NULL == pMaskEntry) || (NULL == pRuleEntry)) {
    return OPL_ERR_NULL_POINTER;
  }

  OPL_BZERO(&Offset, sizeof(CLASSIFY_MASK_OFFSET_FIELD_t));
  OPL_BZERO(MaskValue, CLASSIFY_KEYVALUE_LENGTH);
  OPL_BZERO(RuleValue, CLASSIFY_KEYVALUE_LENGTH);

  if (0 == pMaskRulePar->FieldMap.bitMap) {
    return OPL_OK;
  }

  iStatus = clsOffsetSet(pMaskRulePar, MaskValue, &Offset, RuleValue);
  if (OPL_OK != iStatus) {
    return iStatus;
  }

  pMaskEntry->bfOffset1 = Offset.offsetValue[0];
  pMaskEntry->bfOffset2 = Offset.offsetValue[1];
  pMaskEntry->bfOffset3 = Offset.offsetValue[2];
  pMaskEntry->bfOffset4 = Offset.offsetValue[3];
  pMaskEntry->bfOffset5 = Offset.offsetValue[4];
  pMaskEntry->bfOffset6 = Offset.offsetValue[5];


#ifdef OPCONN_BIG_ENDIAN
  for (byteCnt = 0; byteCnt < CLASSIFY_KEYVALUE_LENGTH; byteCnt++) {
    pMaskEntry->Mask[byteCnt] = MaskValue[byteCnt];
    pRuleEntry->Rule[byteCnt] = RuleValue[byteCnt];
  }
#else
  for (wordCnt = 0; wordCnt < CLASSIFY_KEYVALUE_LENGTH / 4; wordCnt++) {
    for (byteCnt = 0; byteCnt < 4; byteCnt++) {
      srcOff = wordCnt * 4 + (3 - byteCnt);
      destOff = wordCnt * 4 + byteCnt;
      pMaskEntry->Mask[destOff] = MaskValue[srcOff];
      pRuleEntry->Rule[destOff] = RuleValue[srcOff];
    }
  }
#endif

  return OPL_OK;
}

/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/

typedef struct CREQUEST_INFO_s {
  UINT8 index;
  UINT8 foffset;
  UINT8 foffset_off;
  UINT8 mask;
  UINT8 rule;
} CREQUEST_INFO_t;

typedef struct FOFFSET_SEARCH_TBL_ENTRY_s {
  UINT8 mIndex;
  UINT8 mOffset;
  UINT8 mOffset_off;
} FOFFSET_SEARCH_TBL_ENTRY_t;

typedef struct FOFFSET_USED_TBL_ENTRY_s {
  UINT8 uOffset;
  UINT8 uOffset_off;
} FOFFSET_USED_TBL_ENTRY_t;

typedef struct FOFFSET_SEARCH_TBL_s {
  FOFFSET_SEARCH_TBL_ENTRY_t entry[7];
} FOFFSET_SEARCH_TBL_t;

#define FOFFSET_LEN   4
#define CREQUEST_LEN  80
#define FOFFSET_ROW_NUM   6
#define FOFFSET_COLUMN_NUM    12
#define CREQUEST_MATCH_LEN    24

static CREQUEST_INFO_t crinfo[CREQUEST_LEN];

static FOFFSET_USED_TBL_ENTRY_t foffsetUsedTbl[FOFFSET_ROW_NUM];

static FOFFSET_SEARCH_TBL_t foffsetSearchTbl[CREQUEST_LEN] = {
  {1,0,0, 4,11,2, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 0 */
  {1,0,1, 1,1,0, 4,11,3, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 1 */
  {1,0,2, 1,1,1, 5,11,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 2 */
  {1,0,3, 1,1,2, 1,2,0, 5,11,1, 0,0,0, 0,0,0, 0,0,0}, /* byte 3 */
  {1,1,3, 1,2,1, 2,0,0, 5,11,2, 0,0,0, 0,0,0, 0,0,0}, /* byte 4 */
  {1,2,2, 2,0,1, 2,1,0, 5,11,3, 0,0,0, 0,0,0, 0,0,0}, /* byte 5 */
  {1,2,3, 1,3,0, 2,0,2, 2,1,1, 6,11,0, 0,0,0, 0,0,0}, /* byte 6 */
  {1,3,1, 2,0,3, 2,1,2, 2,2,0, 6,11,1, 0,0,0, 0,0,0}, /* byte 7 */
  {1,3,2, 2,1,3, 2,2,1, 3,0,0, 6,11,2, 0,0,0, 0,0,0}, /* byte 8 */
  {1,3,3, 2,2,2, 3,0,1, 3,1,0, 6,11,3, 0,0,0, 0,0,0}, /* byte 9 */

  {1,4,0, 2,2,3, 2,3,0, 3,0,2, 3,1,1, 0,0,0, 0,0,0}, /* byte 10 */
  {1,4,1, 2,3,1, 3,0,3, 3,1,2, 3,2,0, 0,0,0, 0,0,0}, /* byte 11 */
  {1,4,2, 2,3,2, 3,1,3, 3,2,1, 4,0,0, 0,0,0, 0,0,0}, /* byte 12 */
  {1,4,3, 2,3,3, 3,2,2, 4,0,1, 4,1,0, 0,0,0, 0,0,0}, /* byte 13 */
  {2,4,0, 3,2,3, 3,3,0, 4,0,2, 4,1,1, 0,0,0, 0,0,0}, /* byte 14 */
  {1,5,0, 2,4,1, 3,3,1, 4,0,3, 4,1,2, 4,2,0, 0,0,0}, /* byte 15 */
  {1,5,1, 2,4,2, 3,3,2, 4,1,3, 4,2,1, 5,0,0, 0,0,0}, /* byte 16 */
  {1,5,2, 2,4,3, 3,3,3, 4,2,2, 5,0,1, 5,1,0, 0,0,0}, /* byte 17 */
  {1,5,3, 3,4,0, 4,2,3, 4,3,0, 5,0,2, 5,1,1, 0,0,0}, /* byte 18 */
  {2,5,0, 3,4,1, 4,3,1, 5,0,3, 5,1,2, 5,2,0, 0,0,0}, /* byte 19 */

  {2,5,1, 3,4,2, 4,3,2, 5,1,3, 5,2,1, 6,0,0, 0,0,0}, /* byte 20 */
  {1,6,0, 2,5,2, 3,4,3, 4,3,3, 5,2,2, 6,0,1, 6,1,0}, /* byte 21 */
  {1,6,1, 2,5,3, 4,4,0, 5,2,3, 5,3,0, 6,0,2, 6,1,1}, /* byte 22 */
  {1,6,2, 3,5,0, 4,4,1, 5,3,1, 6,0,3, 6,1,2, 6,2,0}, /* byte 23 */
  {1,6,3, 3,5,1, 4,4,2, 5,3,2, 6,1,3, 6,2,1, 0,0,0}, /* byte 24 */
  {2,6,0, 3,5,2, 4,4,3, 5,3,3, 6,2,2, 0,0,0, 0,0,0}, /* byte 25 */
  {2,6,1, 3,5,3, 5,4,0, 6,2,3, 6,3,0, 0,0,0, 0,0,0}, /* byte 26 */
  {2,6,2, 4,5,0, 5,4,1, 6,3,1, 0,0,0, 0,0,0, 0,0,0}, /* byte 27 */
  {1,7,0, 2,6,3, 4,5,1, 5,4,2, 6,3,2, 0,0,0, 0,0,0}, /* byte 28 */
  {1,7,1, 3,6,0, 4,5,2, 5,4,3, 6,3,3, 0,0,0, 0,0,0}, /* byte 29 */

  {1,7,2, 3,6,1, 4,5,3, 6,4,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 30 */
  {1,7,3, 3,6,2, 5,5,0, 6,4,1, 0,0,0, 0,0,0, 0,0,0}, /* byte 31 */
  {2,7,0, 3,6,3, 5,5,1, 6,4,2, 0,0,0, 0,0,0, 0,0,0}, /* byte 32 */
  {2,7,1, 4,6,0, 5,5,2, 6,4,3, 0,0,0, 0,0,0, 0,0,0}, /* byte 33 */
  {2,7,2, 4,6,1, 5,5,3, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 34 */
  {2,7,3, 4,6,2, 6,5,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 35 */
  {1,8,0, 3,7,0, 4,6,3, 6,5,1, 0,0,0, 0,0,0, 0,0,0}, /* byte 36 */
  {1,8,1, 3,7,1, 5,6,0, 6,5,2, 0,0,0, 0,0,0, 0,0,0}, /* byte 37 */
  {1,8,2, 3,7,2, 5,6,1, 6,5,3, 0,0,0, 0,0,0, 0,0,0}, /* byte 38 */
  {1,8,3, 3,7,3, 5,6,2, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 39 */

  {2,8,0, 4,7,0, 5,6,3, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 40 */
  {2,8,1, 4,7,1, 6,6,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 41 */
  {2,8,2, 4,7,2, 6,6,1, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 42 */
  {2,8,3, 4,7,3, 6,6,2, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 43 */
  {3,8,0, 5,7,0, 6,6,3, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 44 */
  {1,9,0, 3,8,1, 5,7,1, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 45 */
  {1,9,1, 3,8,2, 5,7,2, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 46 */
  {1,9,2, 3,8,3, 5,7,3, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 47 */
  {1,9,3, 4,8,0, 6,7,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 48 */
  {2,9,0, 4,8,1, 6,7,1, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 49 */

  {2,9,1, 4,8,2, 6,7,2, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 50 */
  {2,9,2, 4,8,3, 6,7,3, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 51 */
  {2,9,3, 5,8,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 52 */
  {3,9,0, 5,8,1, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 53 */
  {3,9,1, 5,8,2, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 54 */
  {1,10,0, 3,9,2, 5,8,3, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 55 */
  {1,10,1, 3,9,3, 6,8,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 56 */
  {1,10,2, 4,9,0, 6,8,1, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 57 */
  {1,10,3, 4,9,1, 6,8,2, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 58 */
  {2,10,0, 4,9,2, 6,8,3, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 59 */

  {2,10,1, 4,9,3, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 60 */
  {2,10,2, 5,9,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 61 */
  {2,10,3, 5,9,1, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 62 */
  {3,10,0, 5,9,2, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 63 */
  {3,10,1, 5,9,3, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 64 */
  {3,10,2, 6,9,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 65 */
  {1,11,0, 3,10,3, 6,9,1, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 66 */
  {1,11,1, 4,10,0, 6,9,2, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 67 */
  {1,11,2, 4,10,1, 6,9,3, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 68 */
  {1,11,3, 4,10,2, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 69 */

  {2,11,0, 4,10,3, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 70 */
  {2,11,1, 5,10,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 71 */
  {2,11,2, 5,10,1, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 72 */
  {2,11,3, 5,10,2, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 73 */
  {3,11,0, 5,10,3, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 74 */
  {3,11,1, 6,10,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 75 */
  {3,11,2, 6,10,1, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 76 */
  {3,11,3, 6,10,2, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 77 */
  {4,11,0, 6,10,3, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 78 */
  {4,11,1, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0}, /* byte 79 */
};

int clsOffsetAvailableCheck(UINT8 index,UINT8 offset,UINT8 offset_off)
{
  //printk("clsOffsetAvailableCheck,index=%d,offset=%d,offset_off=%d\n", index,offset,offset_off);

  if ((foffsetUsedTbl[index - 1].uOffset == 0)
   && (foffsetUsedTbl[index - 1].uOffset_off == 0)) /* all of the offsets is not used */ {
    return 0;
  }

  if (foffsetUsedTbl[index - 1].uOffset == offset) {
    /* the offset is used */
    if ((foffsetUsedTbl[index - 1].uOffset_off >> offset_off) & 0x1) {
      return -1;
    }
    else {
      /* the target byte is not used */
      return 0;
    }
  }

  return -1;
}

int clsOffsetCalc(UINT8 *requestValid)
{
  int i;
  int ret;
  UINT8 index, offset, offset_off;
  UINT8 requestByte, requestNextByte;

  //printk("requestValid=%x\n", requestValid);

  if (requestValid == NULL) {
    return -1;
  }

  requestByte = *requestValid;
  requestNextByte = *(requestValid + 1);

  for (i = 0; i < 7; i++) {
    if (foffsetSearchTbl[requestByte].entry[i].mIndex != 0) {
      index = foffsetSearchTbl[requestByte].entry[i].mIndex;
      offset = foffsetSearchTbl[requestByte].entry[i].mOffset;
      offset_off = foffsetSearchTbl[requestByte].entry[i].mOffset_off;

      ret = clsOffsetAvailableCheck(index, offset, offset_off);
      if (ret != 0) {
        /* the offset is used */
        continue;
      }

      crinfo[requestByte].index = index;
      crinfo[requestByte].foffset = offset;
      crinfo[requestByte].foffset_off = offset_off;

      foffsetUsedTbl[index - 1].uOffset = offset;
      foffsetUsedTbl[index - 1].uOffset_off |= 1 << offset_off;

      /* analyze the next byte */
      if (requestNextByte != 0) {
        ret = clsOffsetCalc(requestValid + 1);
        if (ret == 0) {
          return 0;
        }
        else {
          /* analyze failed */
          crinfo[requestByte].index = 0;
          crinfo[requestByte].foffset = 0;
          crinfo[requestByte].foffset_off = 0;

          foffsetUsedTbl[index - 1].uOffset_off &= ~(1 << offset_off);
          if (foffsetUsedTbl[index - 1].uOffset_off == 0) {
            foffsetUsedTbl[index - 1].uOffset = 0;
          }
        }
      }
      else {
        return 0; /* all of the 80 bytes are analyzed */
      }
    }
    else {
      break;
    }
  }

  return -1;
}

void clsCRInfoPrt(void)
{
  int i;
  printk("in clsCRInfoPrt\n");
  for (i = 0; i < CREQUEST_LEN; i++) {
    if (crinfo[i].index != 0) {
      printk("no.%d:index=%d,offset=%d,offset_off=%d,mask=%x,rule=%x\n",
             i,
             crinfo[i].index,
             crinfo[i].foffset,
             crinfo[i].foffset_off,
             crinfo[i].mask,
             crinfo[i].rule);
    }
  }
}

OPL_STATUS clsMaskRuleMake2ONU(CLASSIFY_PARAMETER_t *pMaskRulePar,
                               CLASSIFY_MASK_ENTRY_t *pMaskEntry,
                               CLASSIFY_RULE_ENTRY_t *pRuleEntry)
{
  int i, j;
  int ret;

  UINT8 maskValue[CLASSIFY_PACKET_HEADER_LEN];
  UINT8 ruleValue[CLASSIFY_PACKET_HEADER_LEN];
  UINT8 crequestValid[CLASSIFY_KEYVALUE_LENGTH + 1];

  if ((pMaskRulePar == NULL) || (pMaskEntry == NULL) || (pRuleEntry == NULL)) {
    return OPL_ERROR;
  }

  memset(maskValue, 0, CLASSIFY_PACKET_HEADER_LEN);
  memset(ruleValue, 0, CLASSIFY_PACKET_HEADER_LEN);
  memset(crequestValid, 0, CLASSIFY_KEYVALUE_LENGTH + 1);

  memcpy(maskValue,
         (UINT8 *) (&pMaskRulePar->PktUsrMask.EIIValue),
         sizeof(CLASSIFY_EII_t));
  memcpy(ruleValue,
         (UINT8 *) (&pMaskRulePar->PktRuleValue.EIIValue),
         sizeof(CLASSIFY_EII_t));

  j = 0;
  for (i = 0; i < CLASSIFY_PACKET_HEADER_LEN; i++) {
    if (maskValue[i]) {
      crequestValid[j++] = i;
    }
  }

  if (j == 0) {
    /* need not to match */
    return 0;
  }

  memset(crinfo, 0, sizeof(CREQUEST_INFO_t) * CREQUEST_LEN);
  memset(foffsetUsedTbl, 0, FOFFSET_ROW_NUM * sizeof(FOFFSET_USED_TBL_ENTRY_t));

  //printk("Begin to match...\n");
  //printk("crequestValid=%x\n", crequestValid);
  ret = clsOffsetCalc(crequestValid);
  if (ret != 0) {
    printk("Failed: mismatch!!!\n");
    return -1;
  }
  //printk("End to match...\n");

  //clsCRInfoPrt();

  for (i = 0; i < CLASSIFY_PACKET_HEADER_LEN; i++) {
    if (crinfo[i].index == 1) {
      pMaskEntry->bfOffset1 = crinfo[i].foffset;
      pMaskEntry->Mask[crinfo[i].foffset_off] = maskValue[i];
      pRuleEntry->Rule[crinfo[i].foffset_off] = ruleValue[i];
    }
    if (crinfo[i].index == 2) {
      pMaskEntry->bfOffset2 = crinfo[i].foffset;
      pMaskEntry->Mask[4 + crinfo[i].foffset_off] = maskValue[i];
      pRuleEntry->Rule[4 + crinfo[i].foffset_off] = ruleValue[i];
    }
    if (crinfo[i].index == 3) {
      pMaskEntry->bfOffset3 = crinfo[i].foffset;
      pMaskEntry->Mask[8 + crinfo[i].foffset_off] = maskValue[i];
      pRuleEntry->Rule[8 + crinfo[i].foffset_off] = ruleValue[i];
    }
    if (crinfo[i].index == 4) {
      pMaskEntry->bfOffset4 = crinfo[i].foffset;
      pMaskEntry->Mask[12 + crinfo[i].foffset_off] = maskValue[i];
      pRuleEntry->Rule[12 + crinfo[i].foffset_off] = ruleValue[i];
    }
    if (crinfo[i].index == 5) {
      pMaskEntry->bfOffset5 = crinfo[i].foffset;
      pMaskEntry->Mask[16 + crinfo[i].foffset_off] = maskValue[i];
      pRuleEntry->Rule[16 + crinfo[i].foffset_off] = ruleValue[i];
    }
    if (crinfo[i].index == 6) {
      pMaskEntry->bfOffset6 = crinfo[i].foffset;
      pMaskEntry->Mask[20 + crinfo[i].foffset_off] = maskValue[i];
      pRuleEntry->Rule[20 + crinfo[i].foffset_off] = ruleValue[i];
    }
  }

  return 0;
}


/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
/*******************************************************************************
*
* clsPPPoeMaskRuleMake -       Make classify mask and rule table entry.
*
* DESCRIPTION:
*                This function mainly divided into two parts. Firstly, dicide the window
*                       offset through the field list of inputting parameter. Secondly, set
*                       the mask and rule table entry by the data of the field. But you should
*                       note that if the field of the parameter is not byte alignment, you
*                       should do some shift operation first before setting.
*
* INPUT:
*        pRulePar               -       Pointer to parameter inputing.
*
* OUTPUT:
*        pstMaskEntry           -       Pointer to classify mask table entry.
*        pstRuleEntry           -       Pointer to classify rule table entry.
*
* RETURNS:
*        OPL_OK                                                 -       Operation is successful.
*        PARAMETER_INPUT_POINTER_NULL   -       Input pointer is null.
*                CLASSIFY_NULL_FIELDMAP                 -       FieldMap is null.
*                CLASSIFY_INVALID_FIELD_TYPE    -       Field type is wrong.
*                CLASSIFY_WINDOWS_OUTOF_RANGE   -       Input index of parameter exceeds
*                                                                                               maximal defined index.
*                CLASSIFY_INVALID_FOFFSET_NUM   -       Invalid offset ID.
*
* SEE ALSO:
*/
OPL_STATUS clsPPPoeMaskRuleMake(CLASSIFY_PARAMETER_t *pMaskRulePar,
                                CLASSIFY_MASK_ENTRY_t *pMaskEntry,
                                CLASSIFY_RULE_ENTRY_t *pRuleEntry)
{
  OPL_STATUS iStatus = OPL_OK;
  UINT8 byteCnt = 0;

  UINT8 MaskValue[CLASSIFY_KEYVALUE_LENGTH];
  UINT8 RuleValue[CLASSIFY_KEYVALUE_LENGTH];

  CLASSIFY_MASK_OFFSET_FIELD_t Offset;
#ifndef OPCONN_BIG_ENDIAN
  UINT8 wordCnt;
  UINT8 srcOff;
  UINT8 destOff;
#endif
  if ((NULL == pMaskRulePar) || (NULL == pMaskEntry) || (NULL == pRuleEntry)) {
    return OPL_ERR_NULL_POINTER;
  }

  OPL_BZERO(pMaskEntry, sizeof(CLASSIFY_MASK_ENTRY_t));
  OPL_BZERO(pRuleEntry, sizeof(CLASSIFY_RULE_ENTRY_t));
  OPL_BZERO(&Offset, sizeof(CLASSIFY_MASK_OFFSET_FIELD_t));
  OPL_BZERO(MaskValue, CLASSIFY_KEYVALUE_LENGTH);
  OPL_BZERO(RuleValue, CLASSIFY_KEYVALUE_LENGTH);

  if (0 == pMaskRulePar->FieldMap.bitMap) {
    OPL_BZERO(pRuleEntry->Rule, CLASSIFY_KEYVALUE_LENGTH);
    OPL_BZERO(pMaskEntry->Mask, CLASSIFY_KEYVALUE_LENGTH);
    pMaskEntry->bfOffset1 = 0;
    pMaskEntry->bfOffset2 = 0;
    pMaskEntry->bfOffset3 = 0;
    pMaskEntry->bfOffset4 = 0;
    pMaskEntry->bfOffset5 = 0;
    pMaskEntry->bfOffset6 = 0;

    return OPL_OK;
  }

  iStatus = clsOffsetSet(pMaskRulePar, MaskValue, &Offset, RuleValue);
  if (OPL_OK != iStatus) {
    return iStatus;
  }

  pMaskEntry->bfOffset1 = Offset.offsetValue[0];
  pMaskEntry->bfOffset2 = Offset.offsetValue[1];
  pMaskEntry->bfOffset3 = Offset.offsetValue[2];
  pMaskEntry->bfOffset4 = Offset.offsetValue[3];
  pMaskEntry->bfOffset5 = Offset.offsetValue[4];
  pMaskEntry->bfOffset6 = Offset.offsetValue[5];

#ifdef OPCONN_BIG_ENDIAN
  for (byteCnt = 0; byteCnt < CLASSIFY_KEYVALUE_LENGTH; byteCnt++) {
    pMaskEntry->Mask[byteCnt] = MaskValue[byteCnt];
    pRuleEntry->Rule[byteCnt] = RuleValue[byteCnt];
  }
#else
  for (wordCnt = 0; wordCnt < CLASSIFY_KEYVALUE_LENGTH / 4; wordCnt++) {
    for (byteCnt = 0; byteCnt < 4; byteCnt++) {
      srcOff = wordCnt * 4 + (3 - byteCnt);
      destOff = wordCnt * 4 + byteCnt;
      pMaskEntry->Mask[destOff] = MaskValue[srcOff];
      pRuleEntry->Rule[destOff] = RuleValue[srcOff];
    }
  }
#endif
  return OPL_OK;
}


/*******************************************************************************
*
* clsMaskRuleSet - Set classify mask and rule entry.
*
* DESCRIPTION:
*        Set classify mask and rule entry. There are mainly three steps. Firstly,
*        get the start bit and total bits of specified field. Secondly, change
*        the start bit and total bits of field into start byte and total bytes
*        of window. For field has 128 bytes totally, and window has 64 bytes
*        instead. Thirdly, fill in mask and rule entry with 0xff and data
*        by several instances: no tail bits neither header bits, has tail bits
*        but no header bits, no tail bits but has header bits, both tail bits
*        and header bits.
*
* INPUT:
*        pData                - Value to set specified classify entry.
*        fieldMapCount        - Index of the field correspond to the value.
*        pOffset              - Pointer to window offset array.
*
* OUTPUT:
*        pClassMask         - Pointer to classify mask table.
*        pClassRule         - Pointer to classify rule table.
*
* RETURNS:
*        OPL_OK                 - Operation is successful.
*        OPL_ERR_NULL_POINTER   - Input pointer is null.
*
* SEE ALSO:
*/
OPL_STATUS clsMaskRuleSet(UINT8 offsetSeq,
                          UINT8 offsetValue,
                          UINT16 startBit,
                          UINT16 fieldLen,
                          UINT8 *pRuleData,
                          UINT8 *pUsrMask,
                          UINT8 *pMaskValue,
                          UINT8 *pRuleValue)
{
  OPL_STATUS iStatus;
  UINT8 maskTemp = 0;
  UINT8 ruleTemp = 0;
  UINT8 bitCnt = 0;
  UINT8 byteCnt = 0;
  UINT8 ruleDataCnt = 0;
  UINT8 startByte = 0;
  UINT16 offsetHead = 0;
  UINT16 offsetTail = 0;
  UINT16 localStartBit = 0;
  INT16 localFieldLen = 0;


  UINT8 aucLocalRuleData[CLASSIFY_PACKET_HEADER_LEN];
  UINT8 aucLocalUserMask[CLASSIFY_PACKET_HEADER_LEN];

#ifndef OPCONN_BIG_ENDIAN
  UINT8 endianByteCnt;
  UINT8 wordCnt;
  UINT8 srcOff;
  UINT8 destOff;
#endif

  if ((OPL_NULL == pRuleData)
   || (OPL_NULL == pUsrMask)
   || (OPL_NULL == pMaskValue)
   || (OPL_NULL == pRuleValue)) {
    return OPL_ERR_NULL_POINTER;
  }

  localFieldLen = fieldLen;
  localStartBit = startBit;
  startByte = startBit / BITS_PER_BYTE;
  ruleDataCnt = startByte;

  iStatus = clsOffsetHeadTailCal(offsetSeq,
                                 offsetValue,
                                 &offsetHead,
                                 &offsetTail);
  if (OPL_OK != iStatus) {
    return iStatus;
  }

  byteCnt = startByte
          + gClassOffset[offsetSeq - 1]
          - offsetHead
          / BITS_PER_BYTE;
  OPL_MEMCPY(aucLocalRuleData, pRuleData, CLASSIFY_PACKET_HEADER_LEN);
  OPL_MEMCPY(aucLocalUserMask, pUsrMask, CLASSIFY_PACKET_HEADER_LEN);

#ifndef OPCONN_BIG_ENDIAN
  for (wordCnt = 0; wordCnt < CLASSIFY_PACKET_HEADER_LEN / 4; wordCnt++) {
    for (endianByteCnt = 0; endianByteCnt < 4; endianByteCnt++) {
      srcOff = wordCnt * 4 + (3 - endianByteCnt);
      destOff = wordCnt * 4 + endianByteCnt;
      aucLocalRuleData[destOff] = pRuleData[srcOff];
      aucLocalUserMask[destOff] = pUsrMask[srcOff];
    }
  }
#endif
  while (localFieldLen > 0) {
    bitCnt = 0;
    maskTemp = 0;

    while ((bitCnt < localFieldLen)
        && (bitCnt <= (localStartBit + BITS_PER_BYTE - 1) % BITS_PER_BYTE)) {
      maskTemp |= 0x80 >> bitCnt;
      bitCnt++;
    }

    if (0 != localStartBit % BITS_PER_BYTE) {
      maskTemp = maskTemp
              >> (BITS_PER_BYTE - (localStartBit % BITS_PER_BYTE));
    }

    maskTemp &= aucLocalUserMask[ruleDataCnt];
    ruleTemp = aucLocalRuleData[ruleDataCnt];
    ruleTemp &= maskTemp;

    pMaskValue[byteCnt] |= maskTemp;
    pRuleValue[byteCnt] |= ruleTemp;

    localStartBit += bitCnt;
    localFieldLen -= bitCnt;

    byteCnt++;
    ruleDataCnt++;
  }

  return OPL_OK;
}



/*******************************************************************************
*
* clsOffsetSet - Set all 8 window offsets according to FieldMap.
*
* DESCRIPTION:
*       This function looks up the FieldMap. It the specific bit is valid, this
*   function get the start bit and length of the field, the search the offset
*   table.
* INPUT:
*        pRulePar               -       Pointer to parameter inputing.
*
* OUTPUT:
*        pOffset                -       Pointer to window offset array.
*        pMaskValue           -       Pointer to mask value.
*        pRuleValue           -       Pointer to rule value.
*
* RETURNS:
*        OPL_OK                 -       Operation is successful.
*        OPL_ERR_NULL_POINTER   -       Input pointer is null.
*
*
* SEE ALSO:
*/
OPL_STATUS clsOffsetSet(CLASSIFY_PARAMETER_t *pMaskRulePar,
                        UINT8 *pMaskValue,
                        CLASSIFY_MASK_OFFSET_FIELD_t *pOffset,
                        UINT8 *pRuleValue)
{
  OPL_STATUS iStatus;
  UINT8 cnt = 0;
  UINT8 pktType = 0;
  UINT32 localFieldMap = 0;

  UINT16 prestarbit = 0;
  UINT16 prelength = 0;
  CLASSIFY_OFFSET_SET_PAR_t OffsetSetPar;

  if ((OPL_NULL == pOffset)
   || (OPL_NULL == pMaskRulePar)
   || (OPL_NULL == pMaskValue)) {
    return OPL_ERR_NULL_POINTER;
  }

  localFieldMap = pMaskRulePar->FieldMap.bitMap;
  pktType = pMaskRulePar->pktType;

  OPL_BZERO(&OffsetSetPar, sizeof(CLASSIFY_OFFSET_SET_PAR_t));
  OPL_BZERO(gOffsetUsedFlag, CLASSIFY_OFFSET_NUM);        /* It must be clear */


  OffsetSetPar.offsetSeq = CLASSIFY_FOFFSET_1;
  OffsetSetPar.searchLoop = OFFSET_WINDOWS_SEARCH_LOOP_1;

  while (0 < localFieldMap) {
    if (0 != (localFieldMap & CLASSIFY_FIELD_MS_BIT)) {
      iStatus = candFfieldAddrLenGet(cnt,
                                     &(OffsetSetPar.startBit),
                                     &(OffsetSetPar.fieldLength),
                                     pktType);
      if (OPL_OK != iStatus) {
        return iStatus;
      }
      iStatus = clsWinOffsetUsefulCheck(prestarbit,
                                        prelength,
                                        OffsetSetPar.startBit,
                                        OffsetSetPar.fieldLength);
      if (OPL_OK != iStatus) {
        return iStatus;
      }
      iStatus = clsWindowRangeCheck(pMaskValue);
      if (OPL_OK != iStatus) {
        return iStatus;
      }
      iStatus = clsOffsetTblSearch(&OffsetSetPar,
                                   pOffset,
                                   pMaskRulePar,
                                   pMaskValue,
                                   pRuleValue);
      if (OPL_OK != iStatus) {
        return iStatus;
      }


      prestarbit = OffsetSetPar.startBit;
      prelength = OffsetSetPar.fieldLength;
    }
    cnt++;  /* The index move to the next field */
    localFieldMap = pMaskRulePar->FieldMap.bitMap << cnt; /* Ready to set the next field*/
  }

  OPL_BZERO(gOffsetUsedFlag, CLASSIFY_OFFSET_NUM);

  return OPL_OK;
}



/*******************************************************************************
*
* clsOffsetTblSearch - Caculate classify offset value.
*
* DESCRIPTION:
*       This function search the offset table and find a location according to the input
*   start bit and length. Then get the mask value and rule value via call sub func.
*
* INPUT:
*     pOffsetSetPar  - offset set parameter, include offset sequence, loop
*                                 time, start bit and field length
*           pOffset          - each offset value
*           pRulePar         - rule parameter
*
* OUTPUT:
*           pMaskValue       - get current mask value after set offset
*           pRuleValue       - get current rule value after set offset
*
* RETURNS:
*           OPL_OK                - Operation is successful.
*         OPL_ERR_NULL_POINTER    - Input pointer is null.
*
* SEE ALSO:
*/
OPL_STATUS clsOffsetTblSearch(CLASSIFY_OFFSET_SET_PAR_t *pOffsetSetPar,
                              CLASSIFY_MASK_OFFSET_FIELD_t *pOffset,
                              CLASSIFY_PARAMETER_t *pMaskRulePar,
                              UINT8 *pMaskValue,
                              UINT8 *pRuleValue)
{
  OPL_STATUS iStatus;
  OPL_STATUS iSearchStatus = OFFSET_FIND_NONE;
  UINT8 pktType = 0;
  UINT8 offsetSeqCnt;
  UINT8 offsetValue = 0;
  UINT16 offsetHead = 0;
  UINT16 offsetTail = 0;
  UINT8 *pRuleData = OPL_NULL;
  UINT8 *pUsrMaskData = OPL_NULL;

  if ((OPL_NULL == pOffsetSetPar)
   || (OPL_NULL == pOffset)
   || (OPL_NULL == pMaskRulePar)
   || (OPL_NULL == pMaskValue)
   || (OPL_NULL == pRuleValue)) {
    return OPL_ERR_NULL_POINTER;
  }

  if (CLASSIFY_PACKET_HEADER_LEN * BITS_PER_BYTE
    < (pOffsetSetPar->startBit + pOffsetSetPar->fieldLength)) {
    return CLASSIFY_INVALID_PARAMETER_LENGTH;
  }

  pktType = pMaskRulePar->pktType;
  switch (pktType) {
    case CLASSIFY_EII_IP:
      pRuleData = (UINT8 *) (&(pMaskRulePar->PktRuleValue.EIIValue));
      pUsrMaskData = (UINT8 *) (&(pMaskRulePar->PktUsrMask.EIIValue));
      break;

    case CLASSIFY_PPPOE_IP:
      pRuleData = (UINT8 *) (&(pMaskRulePar->PktRuleValue.PPPoeValue));
      pUsrMaskData = (UINT8 *) (&(pMaskRulePar->PktUsrMask.PPPoeValue));
      break;
    default:
      return CLASSIFY_INVALID_PKT_TYPE;
      break;
  }

  offsetSeqCnt = pOffsetSetPar->offsetSeq;

  for (; offsetSeqCnt <= CLASSIFY_FOFFSET_6; offsetSeqCnt++) {
    iSearchStatus = OFFSET_FIND_NONE;

    /* Look for a match start bit */
    for (offsetValue = 0;
         offsetValue < CLASSIFY_OFFSET_MAX_STEP_VALUE;
         offsetValue++) {
      iStatus = clsOffsetHeadTailCal(pOffsetSetPar->offsetSeq,
                                     offsetValue,
                                     &offsetHead,
                                     &offsetTail);
      if (OPL_OK != iStatus) {
        return iStatus;
      }

      if (pOffsetSetPar->startBit == offsetHead) {
        iSearchStatus = OFFSET_FIND_MATCH_STARTBIT;

        if (gOffsetUsedFlag[pOffsetSetPar->offsetSeq - 1]) {
          if (OFFSET_WINDOWS_SEARCH_LOOP_1 == pOffsetSetPar->searchLoop) {
            if (offsetValue
             != pOffset->offsetValue[pOffsetSetPar->offsetSeq - 1]) {
              iSearchStatus = OFFSET_FIND_NONE;
              break;
            }
          }
          else if (OFFSET_WINDOWS_SEARCH_LOOP_2 == pOffsetSetPar->searchLoop) {
            iSearchStatus = OFFSET_FIND_NONE;
            break;
          }
        }

        if (OFFSET_WINDOWS_SEARCH_LOOP_2 == pOffsetSetPar->searchLoop) {
          if ((pOffsetSetPar->startBit + pOffsetSetPar->fieldLength - 1)
            > offsetTail) {
            if (CLASSIFY_FOFFSET_6 == pOffsetSetPar->offsetSeq) {
              return CLASSIFY_WINDOWS_OUTOF_RANGE;
            }
            else if (OPL_TRUE == gOffsetUsedFlag[pOffsetSetPar->offsetSeq]) {
              return CLASSIFY_WINDOWS_OUTOF_RANGE;
            }
          }
        }

        pOffset->offsetValue[pOffsetSetPar->offsetSeq - 1] = offsetValue;
        gOffsetUsedFlag[pOffsetSetPar->offsetSeq - 1] = OPL_TRUE;

        iStatus = clsMaskRuleSet(pOffsetSetPar->offsetSeq,
                                 offsetValue,
                                 pOffsetSetPar->startBit,
                                 pOffsetSetPar->fieldLength,
                                 pRuleData,
                                 pUsrMaskData,
                                 pMaskValue,
                                 pRuleValue);
        if (OPL_OK != iStatus) {
          return iStatus;
        }

        if ((pOffsetSetPar->startBit + pOffsetSetPar->fieldLength - 1)
          > offsetTail) {
          /* Calculate current field remanent length and the new start bit. */
          pOffsetSetPar->fieldLength = pOffsetSetPar->fieldLength
                                     - (offsetTail
                                      + 1 - pOffsetSetPar->startBit);
          pOffsetSetPar->startBit = offsetTail + 1;


          if (OFFSET_WINDOWS_SEARCH_LOOP_1 == pOffsetSetPar->searchLoop) {
            if (CLASSIFY_FOFFSET_6 == pOffsetSetPar->offsetSeq) {
              pOffsetSetPar->searchLoop = OFFSET_WINDOWS_SEARCH_LOOP_2;
              pOffsetSetPar->offsetSeq = CLASSIFY_FOFFSET_1;
              break;
            }
            else {
              (pOffsetSetPar->offsetSeq)++;
              break;
            }
          }
          else if (OFFSET_WINDOWS_SEARCH_LOOP_2 == pOffsetSetPar->searchLoop) {
            if (CLASSIFY_FOFFSET_6 == pOffsetSetPar->offsetSeq) {
              return CLASSIFY_WINDOWS_OUTOF_RANGE;                                /*schen 061221*/
            }
            else {
              (pOffsetSetPar->offsetSeq)++;
              break;
            }
          }
        }
        else if ((pOffsetSetPar->startBit + pOffsetSetPar->fieldLength - 1)
              == offsetTail) {
          if (OFFSET_WINDOWS_SEARCH_LOOP_1 == pOffsetSetPar->searchLoop) {
            if (CLASSIFY_FOFFSET_6 == pOffsetSetPar->offsetSeq) {
              pOffsetSetPar->searchLoop = OFFSET_WINDOWS_SEARCH_LOOP_2;
              pOffsetSetPar->offsetSeq = CLASSIFY_FOFFSET_1;
              break;
            }
            else {
              (pOffsetSetPar->offsetSeq)++;
              return OPL_OK;
            }
          }
          else if (OFFSET_WINDOWS_SEARCH_LOOP_2 == pOffsetSetPar->searchLoop) {
            if (CLASSIFY_FOFFSET_6 == pOffsetSetPar->offsetSeq) {
              if (CLASSIFY_FIELD_MS_BIT == pMaskRulePar->FieldMap.bitMap) {
                /* Current field is the last one */
                return OPL_OK;
              }
              else {
                /* Not any more offset can be used */
                return CLASSIFY_WINDOWS_OUTOF_RANGE;
              }
            }
          }
        }
        else {
          /* Complete current filed configuration */
          return OPL_OK;
        }
      }
    }

    if (OFFSET_FIND_MATCH_STARTBIT == iSearchStatus) {
      continue;   /*If find a match start bit, don't look for a closest start bit*/
    }

    /*Look for a closest start bit*/
    for (offsetValue = 0;
         offsetValue < CLASSIFY_OFFSET_MAX_STEP_VALUE;
         offsetValue++) {
      iStatus = clsOffsetHeadTailCal(pOffsetSetPar->offsetSeq,
                                     offsetValue,
                                     &offsetHead,
                                     &offsetTail);
      if (OPL_OK != iStatus) {
        return iStatus;
      }
      /* Special offset which occupy packet byte 79 and 0 */
      if ((CLASSIFY_FOFFSET_4 == pOffsetSetPar->offsetSeq)
       && (CLASSIFY_FOFFSET_VALUE_11 == offsetValue)) {
        if (offsetHead <= pOffsetSetPar->startBit) {
          offsetTail = (CLASSIFY_PACKET_HEADER_LEN - 1) * BITS_PER_BYTE - 1;
        }
        else if (offsetTail >= pOffsetSetPar->startBit) {
          offsetHead = 0;
        }
      }

      if ((offsetHead <= pOffsetSetPar->startBit)
       && (offsetTail >= pOffsetSetPar->startBit)) {
        iSearchStatus = OFFSET_FIND_CLOSEST_STARTBIT;

        if (gOffsetUsedFlag[pOffsetSetPar->offsetSeq - 1]) {
          iStatus = clsOffsetHeadTailCal(pOffsetSetPar->offsetSeq,
                                         pOffset->offsetValue[pOffsetSetPar->offsetSeq - 1],
                                         &offsetHead,
                                         &offsetTail);
          if (OPL_OK != iStatus) {
            return iStatus;
          }

          if (OFFSET_WINDOWS_SEARCH_LOOP_1 == pOffsetSetPar->searchLoop) {
            /*Offset with current offsetvalue contain current startBit*/
            if ((offsetHead <= pOffsetSetPar->startBit)
             && (offsetTail >= pOffsetSetPar->startBit)) {
              offsetValue = pOffset->offsetValue[pOffsetSetPar->offsetSeq - 1];
            }

            if (offsetValue
             != pOffset->offsetValue[pOffsetSetPar->offsetSeq - 1]) {
              if (CLASSIFY_FOFFSET_6 == pOffsetSetPar->offsetSeq) {
                pOffsetSetPar->searchLoop = OFFSET_WINDOWS_SEARCH_LOOP_2;
                pOffsetSetPar->offsetSeq = CLASSIFY_FOFFSET_1;
                break;
              }
              else {
                (pOffsetSetPar->offsetSeq)++;
                break;
              }
            }
          }
          else if (OFFSET_WINDOWS_SEARCH_LOOP_2 == pOffsetSetPar->searchLoop) {
            if (CLASSIFY_FOFFSET_6 == pOffsetSetPar->offsetSeq) {
              return OPL_ERROR;
            }
            else {
              (pOffsetSetPar->offsetSeq)++;
              break;
            }
          }
        }

        if (OFFSET_WINDOWS_SEARCH_LOOP_2 == pOffsetSetPar->searchLoop) {
          if ((pOffsetSetPar->startBit + pOffsetSetPar->fieldLength - 1)
            > offsetTail) {
            if (CLASSIFY_FOFFSET_6 == pOffsetSetPar->offsetSeq) {
              return CLASSIFY_WINDOWS_OUTOF_RANGE;
            }
            else if (OPL_TRUE == gOffsetUsedFlag[pOffsetSetPar->offsetSeq]) {
              return CLASSIFY_WINDOWS_OUTOF_RANGE;
            }
          }
        }

        pOffset->offsetValue[pOffsetSetPar->offsetSeq - 1] = offsetValue;
        gOffsetUsedFlag[pOffsetSetPar->offsetSeq - 1] = OPL_TRUE;
        iStatus = clsMaskRuleSet(pOffsetSetPar->offsetSeq,
                                 offsetValue,
                                 pOffsetSetPar->startBit,
                                 pOffsetSetPar->fieldLength,
                                 pRuleData,
                                 pUsrMaskData,
                                 pMaskValue,
                                 pRuleValue);
        if (OPL_OK != iStatus) {
          return iStatus;
        }

        if ((pOffsetSetPar->startBit + pOffsetSetPar->fieldLength - 1)
          > offsetTail) {
          pOffsetSetPar->fieldLength = pOffsetSetPar->fieldLength
                                     - (offsetTail
                                      + 1 - pOffsetSetPar->startBit);
          pOffsetSetPar->startBit = offsetTail + 1;

          if (OFFSET_WINDOWS_SEARCH_LOOP_1 == pOffsetSetPar->searchLoop) {
            if (CLASSIFY_FOFFSET_6 == pOffsetSetPar->offsetSeq) {
              pOffsetSetPar->searchLoop = OFFSET_WINDOWS_SEARCH_LOOP_2;
              pOffsetSetPar->offsetSeq = CLASSIFY_FOFFSET_1;
              break;
            }
            else {
              (pOffsetSetPar->offsetSeq)++;
              break;
            }
          }
          else if (OFFSET_WINDOWS_SEARCH_LOOP_2 == pOffsetSetPar->searchLoop) {
            if (CLASSIFY_FOFFSET_6 == pOffsetSetPar->offsetSeq) {
              return OPL_ERROR;
            }
            else {
              (pOffsetSetPar->offsetSeq)++;
              break;
            }
          }
        }
        else if ((pOffsetSetPar->startBit + pOffsetSetPar->fieldLength - 1)
              == offsetTail) {
          if (OFFSET_WINDOWS_SEARCH_LOOP_1 == pOffsetSetPar->searchLoop) {
            if (CLASSIFY_FOFFSET_6 == pOffsetSetPar->offsetSeq) {
              pOffsetSetPar->searchLoop = OFFSET_WINDOWS_SEARCH_LOOP_2;
              pOffsetSetPar->offsetSeq = CLASSIFY_FOFFSET_1;
              break;
            }
            else {
              (pOffsetSetPar->offsetSeq)++;
              return OPL_OK;
            }
          }
          else if (OFFSET_WINDOWS_SEARCH_LOOP_2 == pOffsetSetPar->searchLoop) {
            if (CLASSIFY_FOFFSET_6 == pOffsetSetPar->offsetSeq) {
              if (CLASSIFY_FIELD_MS_BIT == pMaskRulePar->FieldMap.bitMap) {
                return OPL_OK;
              }
              else {
                return OPL_ERROR;
              }
            }
          }
        }
        else {
          return OPL_OK;
        }
      }
    }


    if (OFFSET_FIND_NONE != iSearchStatus) {
      continue;
    }

    /*If not find a closest start bit*/
    iSearchStatus = OFFSET_FIND_NONE;

    if (CLASSIFY_FOFFSET_6 == offsetSeqCnt) {
      if (OFFSET_WINDOWS_SEARCH_LOOP_1 == pOffsetSetPar->searchLoop) {
        /* Clear loop log and sequence cnt for the next loop search */
        pOffsetSetPar->searchLoop = OFFSET_WINDOWS_SEARCH_LOOP_2;
        pOffsetSetPar->offsetSeq = CLASSIFY_FOFFSET_1;
      }
      else {
        return OPL_ERROR;
      }
    }
    else {
      (pOffsetSetPar->offsetSeq)++;
    }
  }
  return OPL_OK;
}


/*******************************************************************************
*
* clsOffsetHeadTailCal  - Specify the sequence and value of a offset, then get its' head
*                     and tail byte.
*
* DESCRIPTION:
*       This function calculates the start and tail bits of a specific offset.
*   Especially calculates the offset which ocuppies the byte 79 and byte 0.
*
* INPUT:
*           offsetSeq   - Offset sequence, from 1 - 8
*     offsetValue - Offset step value, from 1 - 12
*
* OUTPUT:
*     pOffsetHead - Byte number of offset head
*       pOffsetTail - Byte number of offset tail
*
* RETURNS:
*           OPL_OK                - Operation is successful.
*         OPL_ERR_NULL_POINTER    - Input pointer is null.
*
* SEE ALSO:
*/
OPL_STATUS clsOffsetHeadTailCal(UINT8 offsetSeq,
                                UINT8 offsetValue,
                                UINT16 *pOffsetHead,
                                UINT16 *pOffsetTail)
{
  if ((NULL == pOffsetHead) || (NULL == pOffsetTail)) {
    return OPL_ERR_NULL_POINTER;
  }

  *pOffsetHead = (gClassOffset[offsetSeq - 1] + gClassSession[offsetValue]) * BITS_PER_BYTE;
  *pOffsetTail = (gClassOffset[offsetSeq - 1]
                + gClassSession[offsetValue]
                + CLASSIFY_OFFSET_LEN) * BITS_PER_BYTE
               - 1;

  if (CLASSIFY_PACKET_HEADER_LEN * BITS_PER_BYTE <= *pOffsetHead) {
    *pOffsetHead -= CLASSIFY_PACKET_HEADER_LEN * BITS_PER_BYTE;
  }
  if (CLASSIFY_PACKET_HEADER_LEN * BITS_PER_BYTE <= *pOffsetTail) {
    *pOffsetTail -= CLASSIFY_PACKET_HEADER_LEN * BITS_PER_BYTE;
  }
  return  OPL_OK;
}


/*******************************************************************************
*
* clsRuleIDSel    - Select rule ID corresponding to specified rule entry.
*
* DESCRIPTION:
*    This function select rule ID in classify address table corresponding to
*     specified rule entry inputting through parameter.
*
* INPUT:
*    pstRuleEntry - Pointer to entry of classify rule table.
*    usAddr     - Index of classify address table.
*
* OUTPUT:
*    pAddrOff   - Pointer to offset of certain entry of classify address table.
*    pRuleID    - Pointer to index of entry of classify rule table.
*
* RETURNS:
*        OPL_OK           - If the operation is successful.
*        OPL_ERR_NULL_POINTER   - If the input pointer is NULL.
*    CLASSIFY_RULE_NULL_ITEM  - No such rule item is find in address table.
*
* SEE ALSO:
*/
OPL_STATUS clsRuleIDSel(CLASSIFY_RULE_ENTRY_t *pRuleEntry,
                        UINT16 usAddr,
                        UINT16 *pAddrOff,
                        UINT16 *pRuleID)
{
  UINT16 cruleID;
  UINT8 addrCnt;

  if (OPL_NULL == pRuleEntry) {
    return OPL_ERR_NULL_POINTER;
  }

  for (addrCnt = 0; addrCnt < CLASSIFY_RULE_TABLE_GROUP; addrCnt++) {
    if ((0 == gpCaddrTbl[usAddr].ClasAddr[addrCnt])
     || (0xFFFF == gpCaddrTbl[usAddr].ClasAddr[addrCnt])) {
      continue;
    }

    cruleID = gpCaddrTbl[usAddr].ClasAddr[addrCnt];
    if ((pRuleEntry->bfMsel == gpCruleTbl[cruleID].bfMsel)
     && (pRuleEntry->bfIngress == gpCruleTbl[cruleID].bfIngress)
     && (pRuleEntry->bfEgress == gpCruleTbl[cruleID].bfEgress)
     && (0 == OPL_MEMCMP(pRuleEntry->Rule,
                         gpCruleTbl[cruleID].Rule,
                         CLASSIFY_KEYVALUE_LENGTH))) {
      *pAddrOff = addrCnt;
      *pRuleID = cruleID;
      return OPL_OK;
    }
  }

  return CLASSIFY_RULE_NULL_ITEM;
}



/*******************************************************************************
*
* clsMaskSel  - Select the mask entry ID according to parameter.
*
* DESCRIPTION:
*    Select the mask entry ID according to parameter. If there's not entry match
*     parameter, return message to indicate no entry selected.
*
* INPUT:
*    pMaskEntry - Pointer to mask entry.
*
* OUTPUT:
*    pMask    - Pointer to mask ID.
*
* RETURNS:
*        OPL_OK           - If the operation is successful.
*        OPL_ERR_NULL_POINTER   - If the input pointer is NULL.
*    CLASSIFY_MASKF_SELECT_NULL - If the specified mask entry is not exist.
*
* SEE ALSO:
*/
OPL_STATUS clsMaskSel(CLASSIFY_MASK_ENTRY_t *pMaskEntry,UINT8 *pMask)
{
  UINT8 maskCnt;

  if ((NULL == pMask) || (NULL == pMaskEntry)) {
    return OPL_ERR_NULL_POINTER;
  }

  for (maskCnt = CLASSIFY_MIN_MASK_ENTRY; maskCnt < CLASSIFY_MAX_MASK_ENTRY; maskCnt++) {
    if ((OPL_VALID == gpCmaskTbl[maskCnt].bfValid)
     && (pMaskEntry->bfEmask == gpCmaskTbl[maskCnt].bfEmask)
     && (pMaskEntry->bfImask == gpCmaskTbl[maskCnt].bfImask)
     && (pMaskEntry->bfOffset6 == gpCmaskTbl[maskCnt].bfOffset6)
     && (pMaskEntry->bfOffset5 == gpCmaskTbl[maskCnt].bfOffset5)
     && (pMaskEntry->bfOffset4 == gpCmaskTbl[maskCnt].bfOffset4)
     && (pMaskEntry->bfOffset3 == gpCmaskTbl[maskCnt].bfOffset3)
     && (pMaskEntry->bfOffset2 == gpCmaskTbl[maskCnt].bfOffset2)
     && (pMaskEntry->bfOffset1 == gpCmaskTbl[maskCnt].bfOffset1)
     && (0 == OPL_MEMCMP(pMaskEntry->Mask,
                         gpCmaskTbl[maskCnt].Mask,
                         CLASSIFY_KEYVALUE_LENGTH))) {
      *pMask = maskCnt; return OPL_OK;
    }
  }

  return CLASSIFY_MASKF_SELECT_NULL;
}



/*******************************************************************************
*
* clsMaskTblRd - read a mask entry from the mask table.
*
* DESCRIPTION:
*       read a mask entry from the mask table.
*
* INPUT:
*       maskID  - classification Mask entry ID.
*       tblType - CLASSIFY_LOCAL_TBL               1
*                   CLASSIFY_HW_TBL                  2
*
* OUTPUT:
*        pMask  - classify mask entry.
*
* RETURNS:
*       OPL_OK,                       If the operation is successful.
*       PARAMETER_INPUT_POINTER_NULL, If the input pointer is NULL.
*
* SEE ALSO:
*/

OPL_STATUS clsMaskTblRd(CLASSIFY_MASK_ENTRY_t *pMask,
                        UINT16 maskID,
                        UINT8 tblType)
{
  OPL_STATUS iStatus;
  CLASSIFY_MASK_TBL_t MaskTblEntry;
  UINT32 *pulTest;

#ifndef OPCONN_BIG_ENDIAN
  UINT8 byteCnt;
  UINT8 wordCnt;
  UINT8 srcOff;
  UINT8 destOff;
#endif

  if (CLASSIFY_LOCAL_TBL == tblType) {
    pMask->bfEmask = gpCmaskTbl[maskID].bfEmask;
    pMask->bfImask = gpCmaskTbl[maskID].bfImask;
    pMask->bfOffset1 = gpCmaskTbl[maskID].bfOffset1;
    pMask->bfOffset2 = gpCmaskTbl[maskID].bfOffset2;
    pMask->bfOffset3 = gpCmaskTbl[maskID].bfOffset3;
    pMask->bfOffset4 = gpCmaskTbl[maskID].bfOffset4;
    pMask->bfOffset5 = gpCmaskTbl[maskID].bfOffset5;
    pMask->bfOffset6 = gpCmaskTbl[maskID].bfOffset6;
    pMask->bfValid = gpCmaskTbl[maskID].bfValid;
    pMask->maskPri = gpCmaskTbl[maskID].maskPri;
    pMask->referenceCount = gpCmaskTbl[maskID].referenceCount;

    OPL_MEMCPY(pMask->Mask, gpCmaskTbl[maskID].Mask, CLASSIFY_KEYVALUE_LENGTH);
  }
  else {
    iStatus = oplTabRead(TAB_CLS_MASK_LEN,
                         maskID,
                         1,
                         (UINT32 *) (&(MaskTblEntry)));
    if (OPL_OK != iStatus) {
      return iStatus;
    }

    pulTest = (UINT32 *) (&MaskTblEntry);

    OPL_LOG(&gbClsDbg,
            &gbClsFileDbg,
            "Classify READ MASK table MaskId = %d \r\n ",
            maskID);
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

    pMask->bfEmask = MaskTblEntry.bfEmask;
    pMask->bfImask = MaskTblEntry.bfImask;
    pMask->bfOffset1 = MaskTblEntry.bfOffset1;
    pMask->bfOffset2 = MaskTblEntry.bfOffset2;
    pMask->bfOffset3 = MaskTblEntry.bfOffset3;
    pMask->bfOffset4 = MaskTblEntry.bfOffset4;
    pMask->bfOffset5 = MaskTblEntry.bfOffset5;
    pMask->bfOffset6 = MaskTblEntry.bfOffset6;
    pMask->bfValid = MaskTblEntry.bfValid;
    pMask->maskPri = gpCmaskTbl[maskID].maskPri;
    pMask->referenceCount = gpCmaskTbl[maskID].referenceCount;

#ifdef OPCONN_BIG_ENDIAN
    OPL_MEMCPY(pMask->Mask, MaskTblEntry.Mask, CLASSIFY_KEYVALUE_LENGTH);
#else
    for (wordCnt = 0; wordCnt < CLASSIFY_KEYVALUE_LENGTH / 4; wordCnt++) {
      for (byteCnt = 0; byteCnt < 4; byteCnt++) {
        srcOff = wordCnt * 4 + (3 - byteCnt);
        destOff = wordCnt * 4 + byteCnt;
        pMask->Mask[destOff] = MaskTblEntry.Mask[srcOff];
      }
    }
#endif
  }
  return OPL_OK;
}



/*******************************************************************************
*
* clsMaskTblEntryAdd  - Add the specific mask entry to the table.
*
* DESCRIPTION:
*       This function used to calculate the address, and then allocate the rule
*     and mask entry and then write mask and rule into the mask and rule table
*     the rule and mask entry is generated by the calling funtion.
*
* INPUT:
*    pstMaskEntry - Pointer to classification Mask entry.
*    usMaskID     - classification mask ID.
*
* OUTPUT:
*    None.
*
* RETURNS:
*       OPL_OK,                       If the operation is successful.
*       PARAMETER_INPUT_POINTER_NULL, If the input pointer is NULL.
*
* SEE ALSO:
*/
OPL_STATUS clsMaskTblEntryAdd(CLASSIFY_MASK_ENTRY_t *pMaskEntry,UINT16 maskID)
{
  OPL_STATUS iStatus;

  pMaskEntry->bfValid = 1;

  iStatus = dalClsMaskTblWr(pMaskEntry, (UINT8) maskID);
  if (OPL_OK != iStatus) {
    return iStatus;
  }

  return OPL_OK;
}




/*******************************************************************************
*
* clsRuleTblEntryAdd  - Add the specific rule and mask entry to the table.
*
* DESCRIPTION:
*    This function used to calculate the address, and then allocate the rule
*     and mask entry and then write mask and rule into the mask and rule table
*     the rule and mask entry is generated by the calling funtion.
*
* INPUT:
*    RuleEntry          - Pointer to classification rule entry.
*
* OUTPUT:
*    ClassifyRuleID   - Pointer to classify rule table entry ID.
*
* RETURNS:
*       OPL_OK,                       If the operation is successful.
*       PARAMETER_INPUT_POINTER_NULL, If the input pointer is NULL.
*
* SEE ALSO:
*/
OPL_STATUS clsRuleTblEntryAdd(CLASSIFY_RULE_ENTRY_t *pRuleEntry,
                              UINT16 *pClassifyRuleID)
{
  UINT16 ruleID;
  UINT16 address;
  UINT16 addrOff;
  OPL_STATUS iStatus;

  if ((NULL == pRuleEntry) || (NULL == pClassifyRuleID)) {
    return OPL_ERR_NULL_POINTER;
  }


  /*Calculate address*/
  iStatus = clsAddrCal(pRuleEntry, &address);
  if (OPL_OK != iStatus) {
    return iStatus;
  }

  iStatus = clsRuleIDSel(pRuleEntry, address, &addrOff, &ruleID);
  if (OPL_OK == iStatus) {
    /*There a same rule in the rule table, so the current can't be written
      into the table. Free the mask which has been malloced for this rule.*/
    return CLASSIFY_RULE_ITEM_SAME;
  }
  else if (CLASSIFY_RULE_NULL_ITEM != iStatus) {
    return iStatus;
  }

  /*Malloc a new entry from the rule table for the new rule.*/
  iStatus = clsRuleMalloc(address, &ruleID, &addrOff);
  if (OPL_OK != iStatus) {
    return iStatus;
  }

  *pClassifyRuleID = ruleID;
  /*Write the new entry into the table, there two table need to be
    wirtten: local table in the system SDRAM and hardware table in
    the ASIC.*/
  iStatus = dalClsRuleTblWr(pRuleEntry, ruleID);
  if (OPL_OK != iStatus) {
    clsRuleFree(ruleID);
    return iStatus;
  }


  iStatus = dalClsAddrTblWr(address, addrOff, ruleID);
  if (OPL_OK != iStatus) {
    clsRuleFree(ruleID);
    return iStatus;
  }

  return OPL_OK;
}



/*******************************************************************************
* clsEIIMaskMake  - Make classify mask and entry.
*
* DESCRIPTION:
*    This function mainly divided into two parts. Firstly, dicide the window
*     offset through the field list of inputting parameter. Secondly, set
*     the mask and rule table entry by the data of the field. But you should
*     note that if the field of the parameter is not byte alignment, you
*     should do some shift operation first before setting.
*
* INPUT:
*        pstMaskPar     - Pointer to parameter inputing.
*
* OUTPUT:
*        pstMaskEntry   - Pointer to classify mask table entry.
*        pstRuleEntry   - Pointer to classify rule table entry.
*
* RETURNS:
*        OPL_OK             - Operation is successful.
*        PARAMETER_INPUT_POINTER_NULL - Input pointer is null.
*    CLASSIFY_NULL_FIELDMAP     - FieldMap is null.
*    CLASSIFY_INVALID_FIELD_TYPE  - Field type is wrong.
*    CLASSIFY_WINDOWS_OUTOF_RANGE - Input index of parameter exceeds
*                       maximal defined index.
*    CLASSIFY_INVALID_FOFFSET_NUM - Invalid offset ID.
*
* SEE ALSO:
*/
OPL_STATUS clsEIIMaskMake(CLASSIFY_MASK_PARAMETER_t *pMaskPar,
                          CLASSIFY_MASK_ENTRY_t *pMaskEntry)
{
  UINT8 cnt;

  UINT8 MaskValue[CLASSIFY_KEYVALUE_LENGTH];
  UINT8 RuleValue[CLASSIFY_KEYVALUE_LENGTH];

  CLASSIFY_MASK_OFFSET_FIELD_t offset;
  CLASSIFY_PARAMETER_t MaskRulePar;

  OPL_STATUS iStatus;

  if ((NULL == pMaskPar) || (NULL == pMaskEntry)) {
    return OPL_ERR_NULL_POINTER;
  }

  OPL_BZERO(pMaskEntry, sizeof(CLASSIFY_MASK_ENTRY_t));
  OPL_BZERO(&offset, sizeof(CLASSIFY_MASK_OFFSET_FIELD_t));
  OPL_BZERO(&MaskRulePar, sizeof(CLASSIFY_PARAMETER_t));
  OPL_BZERO(MaskValue, CLASSIFY_KEYVALUE_LENGTH);
  OPL_BZERO(RuleValue, CLASSIFY_KEYVALUE_LENGTH);

  if (0 == pMaskPar->FieldMap.bitMap) {
    return OPL_OK;
  }

  MaskRulePar.pktType = pMaskPar->pktType;
  MaskRulePar.egressMask = pMaskPar->egressMask;
  MaskRulePar.ingressMask = pMaskPar->ingressMask;
  MaskRulePar.FieldMap.EIIField = pMaskPar->FieldMap.EIIField;
  MaskRulePar.PktRuleValue.EIIValue = pMaskPar->RuleValue.EIIValue;
  MaskRulePar.PktUsrMask.EIIValue = pMaskPar->UsrMask.EIIUsrMask;

  iStatus = clsOffsetSet(&MaskRulePar, MaskValue, &offset, RuleValue);
  if (OPL_OK != iStatus) {
    return iStatus;
  }

  pMaskEntry->bfOffset1 = offset.offsetValue[0];
  pMaskEntry->bfOffset2 = offset.offsetValue[1];
  pMaskEntry->bfOffset3 = offset.offsetValue[2];
  pMaskEntry->bfOffset4 = offset.offsetValue[3];
  pMaskEntry->bfOffset5 = offset.offsetValue[4];
  pMaskEntry->bfOffset6 = offset.offsetValue[5];

  for (cnt = 0; cnt < CLASSIFY_KEYVALUE_LENGTH; cnt++) {
    pMaskEntry->Mask[cnt] = MaskValue[cnt];
  }

  return OPL_OK;
}



/*******************************************************************************
*
* clsEIIRuleMake  - Make classify mask and rule table entry.
*
* DESCRIPTION:
*    This function mainly divided into two parts. Firstly, dicide the window
*     offset through the field list of inputting parameter. Secondly, set
*     the mask and rule table entry by the data of the field. But you should
*     note that if the field of the parameter is not byte alignment, you
*     should do some shift operation first before setting.
*
* INPUT:
*        pstRulePar     - Pointer to parameter inputing.
*
* OUTPUT:
*        pstMaskEntry   - Pointer to classify mask table entry.
*        pstRuleEntry   - Pointer to classify rule table entry.
*
* RETURNS:
*        OPL_OK             - Operation is successful.
*        PARAMETER_INPUT_POINTER_NULL - Input pointer is null.
*    CLASSIFY_NULL_FIELDMAP     - FieldMap is null.
*    CLASSIFY_INVALID_FIELD_TYPE  - Field type is wrong.
*    CLASSIFY_WINDOWS_OUTOF_RANGE - Input index of parameter exceeds
*                       maximal defined index.
*    CLASSIFY_INVALID_FOFFSET_NUM - Invalid offset ID.
*
* SEE ALSO:
*/
OPL_STATUS clsEIIRuleMake(CLASSIFY_RULE_PARAMETER_t *pRulePar,
                          CLASSIFY_RULE_ENTRY_t *pRuleEntry,
                          UINT16 maskID)
{
  UINT8 cnt;

  UINT8 MaskValue[CLASSIFY_KEYVALUE_LENGTH];
  UINT8 RuleValue[CLASSIFY_KEYVALUE_LENGTH];

  CLASSIFY_MASK_OFFSET_FIELD_t offset;
  CLASSIFY_PARAMETER_t MaskRulePar;

  OPL_STATUS iStatus;

  if ((NULL == pRulePar) || (NULL == pRuleEntry)) {
    return OPL_ERR_NULL_POINTER;
  }

  OPL_BZERO(pRuleEntry, sizeof(CLASSIFY_RULE_ENTRY_t));
  OPL_BZERO(&offset, sizeof(CLASSIFY_MASK_OFFSET_FIELD_t));
  OPL_BZERO(&MaskRulePar, sizeof(CLASSIFY_PARAMETER_t));
  OPL_BZERO(MaskValue, CLASSIFY_KEYVALUE_LENGTH);
  OPL_BZERO(RuleValue, CLASSIFY_KEYVALUE_LENGTH);

  if (0 == gpMaskInfoForRule[maskID].fieldMap) {
    return OPL_OK;
  }

  MaskRulePar.pktType = pRulePar->pktType;
#if 0
  MaskRulePar.cbs                     = pRulePar->cbs;
  MaskRulePar.cir                       = pRulePar->cir;
#endif
  MaskRulePar.RuleEntry = pRulePar->RuleEntry;
  MaskRulePar.PktRuleValue.EIIValue = pRulePar->RuleValue.EIIValue;

  OPL_MEMCPY((&(MaskRulePar.FieldMap.EIIField)),
             (&(gpMaskInfoForRule[maskID].fieldMap)),
             sizeof(gpMaskInfoForRule[maskID].fieldMap));

  MaskRulePar.PktUsrMask.EIIValue = gpMaskInfoForRule[maskID].UsrMask.EIIUsrMask;

  iStatus = clsOffsetSet(&MaskRulePar, MaskValue, &offset, RuleValue);
  if (OPL_OK != iStatus) {
    return iStatus;
  }

  for (cnt = 0; cnt < CLASSIFY_KEYVALUE_LENGTH; cnt++) {
    pRuleEntry->Rule[cnt] = RuleValue[cnt];
  }
  return OPL_OK;
}



/*******************************************************************************
*
* clsPPPoeMaskMake  - Make classify mask and rule table entry.
*
* DESCRIPTION:
*    This function mainly divided into two parts. Firstly, dicide the window
*     offset through the field list of inputting parameter. Secondly, set
*     the mask and rule table entry by the data of the field. But you should
*     note that if the field of the parameter is not byte alignment, you
*     should do some shift operation first before setting.
*
* INPUT:
*        pstRulePar     - Pointer to parameter inputing.
*
* OUTPUT:
*        pstMaskEntry   - Pointer to classify mask table entry.
*        pstRuleEntry   - Pointer to classify rule table entry.
*
* RETURNS:
*        OPL_OK             - Operation is successful.
*        PARAMETER_INPUT_POINTER_NULL - Input pointer is null.
*    CLASSIFY_NULL_FIELDMAP     - FieldMap is null.
*    CLASSIFY_INVALID_FIELD_TYPE  - Field type is wrong.
*    CLASSIFY_WINDOWS_OUTOF_RANGE - Input index of parameter exceeds
*                       maximal defined index.
*    CLASSIFY_INVALID_FOFFSET_NUM - Invalid offset ID.
*        CLASSIFY_INVALID_FIELD_NUM     -   Invalid field number
*
* SEE ALSO:
*/
OPL_STATUS clsPPPoeMaskMake(CLASSIFY_MASK_PARAMETER_t *pMaskPar,
                            CLASSIFY_MASK_ENTRY_t *pMaskEntry)
{
  UINT8 cnt;
  UINT8 MaskValue[CLASSIFY_KEYVALUE_LENGTH];
  UINT8 RuleValue[CLASSIFY_KEYVALUE_LENGTH];

  CLASSIFY_MASK_OFFSET_FIELD_t offset;
  CLASSIFY_PARAMETER_t MaskRulePar;

  OPL_STATUS iStatus;

  if ((NULL == pMaskPar) || (NULL == pMaskEntry)) {
    return OPL_ERR_NULL_POINTER;
  }

  OPL_BZERO(pMaskEntry, sizeof(CLASSIFY_MASK_ENTRY_t));
  OPL_BZERO(&offset, sizeof(CLASSIFY_MASK_OFFSET_FIELD_t));
  OPL_BZERO(&MaskRulePar, sizeof(CLASSIFY_PARAMETER_t));
  OPL_BZERO(MaskValue, CLASSIFY_KEYVALUE_LENGTH);
  OPL_BZERO(RuleValue, CLASSIFY_KEYVALUE_LENGTH);

  MaskRulePar.pktType = pMaskPar->pktType;
  MaskRulePar.egressMask = pMaskPar->egressMask;
  MaskRulePar.ingressMask = pMaskPar->ingressMask;
  MaskRulePar.FieldMap.PppoEField = pMaskPar->FieldMap.PppoEField;
  MaskRulePar.PktRuleValue.PPPoeValue = pMaskPar->RuleValue.PPPoeValue;
  MaskRulePar.PktUsrMask.PPPoeValue = pMaskPar->UsrMask.PPPoeUsrMask;

  iStatus = clsOffsetSet(&MaskRulePar, MaskValue, &offset, RuleValue);
  if (OPL_OK != iStatus) {
    return iStatus;
  }

  pMaskEntry->bfOffset1 = offset.offsetValue[0];
  pMaskEntry->bfOffset2 = offset.offsetValue[1];
  pMaskEntry->bfOffset3 = offset.offsetValue[2];
  pMaskEntry->bfOffset4 = offset.offsetValue[3];
  pMaskEntry->bfOffset5 = offset.offsetValue[4];
  pMaskEntry->bfOffset6 = offset.offsetValue[5];

  for (cnt = 0; cnt < CLASSIFY_KEYVALUE_LENGTH; cnt++) {
    pMaskEntry->Mask[cnt] = MaskValue[cnt];
  }

  return OPL_OK;
}




/*******************************************************************************
*
* clsPPPoeRuleMake  - Make classify mask and rule table entry.
*
* DESCRIPTION:
*    This function mainly divided into two parts. Firstly, dicide the window
*     offset through the field list of inputting parameter. Secondly, set
*     the mask and rule table entry by the data of the field. But you should
*     note that if the field of the parameter is not byte alignment, you
*     should do some shift operation first before setting.
*
* INPUT:
*        pstRulePar     - Pointer to parameter inputing.
*
* OUTPUT:
*        pstMaskEntry   - Pointer to classify mask table entry.
*        pstRuleEntry   - Pointer to classify rule table entry.
*
* RETURNS:
*        OPL_OK             - Operation is successful.
*        PARAMETER_INPUT_POINTER_NULL - Input pointer is null.
*    CLASSIFY_NULL_FIELDMAP     - FieldMap is null.
*    CLASSIFY_INVALID_FIELD_TYPE  - Field type is wrong.
*    CLASSIFY_WINDOWS_OUTOF_RANGE - Input index of parameter exceeds
*                       maximal defined index.
*    CLASSIFY_INVALID_FOFFSET_NUM - Invalid offset ID.
*        CLASSIFY_INVALID_FIELD_NUM     -   Invalid field number
*
* SEE ALSO:
*/
OPL_STATUS clsPPPoeRuleMake(CLASSIFY_RULE_PARAMETER_t *pRulePar,
                            CLASSIFY_RULE_ENTRY_t *pRuleEntry,
                            UINT16 maskID)
{
  UINT8 cnt;

  UINT8 MaskValue[CLASSIFY_KEYVALUE_LENGTH];
  UINT8 RuleValue[CLASSIFY_KEYVALUE_LENGTH];

  CLASSIFY_MASK_OFFSET_FIELD_t offset;
  CLASSIFY_PARAMETER_t MaskRulePar;

  OPL_STATUS iStatus;

  if ((NULL == pRulePar) || (NULL == pRuleEntry)) {
    return OPL_ERR_NULL_POINTER;
  }

  OPL_BZERO(pRuleEntry, sizeof(CLASSIFY_RULE_ENTRY_t));
  OPL_BZERO(&offset, sizeof(CLASSIFY_MASK_OFFSET_FIELD_t));
  OPL_BZERO(&MaskRulePar, sizeof(CLASSIFY_PARAMETER_t));
  OPL_BZERO(MaskValue, CLASSIFY_KEYVALUE_LENGTH);
  OPL_BZERO(RuleValue, CLASSIFY_KEYVALUE_LENGTH);

  if (0 == gpMaskInfoForRule[maskID].fieldMap) {
    return OPL_OK;
  }

  MaskRulePar.pktType = pRulePar->pktType;
#if 0
  MaskRulePar.cbs                         = pRulePar->cbs;
  MaskRulePar.cir                         = pRulePar->cir;
#endif
  MaskRulePar.RuleEntry = pRulePar->RuleEntry;
  MaskRulePar.PktRuleValue.PPPoeValue = pRulePar->RuleValue.PPPoeValue;

  OPL_MEMCPY((&(MaskRulePar.FieldMap.PppoEField)),
             (&(gpMaskInfoForRule[maskID].fieldMap)),
             sizeof(gpMaskInfoForRule[maskID].fieldMap));

  MaskRulePar.PktUsrMask.PPPoeValue = gpMaskInfoForRule[maskID].UsrMask.PPPoeUsrMask;

  iStatus = clsOffsetSet(&MaskRulePar, MaskValue, &offset, RuleValue);
  if (OPL_OK != iStatus) {
    return iStatus;
  }

  for (cnt = 0; cnt < CLASSIFY_KEYVALUE_LENGTH; cnt++) {
    pRuleEntry->Rule[cnt] = RuleValue[cnt];
  }
  return OPL_OK;
}



/*******************************************************************************
*
* clsRuleAddrGet  - Allocate the rule ID and address offset for certain
*               address entry.
*
* DESCRIPTION:
*    This function allocates the rule ID and address offset for certain address
*     entry. Firstly, check if there is any space in the hash address. Then,
*     check which group has the most free ID. Finally, calculate the rule
*     ID and address offset.
*
* INPUT:
*    RuleID       - classify rule ID.
*
* OUTPUT:
*    RuleAddr   - Pointer to index of entry of classify hash address table.
*    AddrOff    - Pointer to offset of certain entry of classify address table.
*
* RETURNS:
*        OPL_OK             - If the operation is successful.
*        PARAMETER_INPUT_POINTER_NULL - If the input pointer is NULL.
*    CLASSIFY_ADDR_TABLE_8COLLISION - The entry of classify address table is full.
*    CLASSIFY_RULE_TABLE_FULL   - All the classify rule table is full.
*
* SEE ALSO:
*/
OPL_STATUS clsRuleAddrGet(UINT16 RuleID,UINT16 *RuleAddr,UINT16 *AddrOff)
{
  UINT8 OffsetCnt;
  UINT16 AddrCnt;

  if ((NULL == RuleAddr) || (NULL == AddrOff)) {
    return OPL_ERR_NULL_POINTER;
  }


  for (AddrCnt = 0; AddrCnt < CLASSIFY_MAX_ADDR_ENTRY; AddrCnt++) {
    for (OffsetCnt = 0; OffsetCnt < CLASSIFY_RULE_TABLE_GROUP; OffsetCnt++) {
      if (RuleID == gpCaddrTbl[AddrCnt].ClasAddr[OffsetCnt]) {
        *RuleAddr = AddrCnt;
        *AddrOff = OffsetCnt;
        return OPL_OK;
      }
    }
  }

  return CLASSIFY_RULE_NULL_ITEM;
}



/*******************************************************************************
*
* candFfieldAddrLenGet - Get the start bit and total bits of specified parameter.
*
* DESCRIPTION:
*        First distinguish the type of received packet, then get the start bit
*        and total bits of specified parameter directly from defined global array.
*
* INPUT:
*        fieldNum             -       Index of parameter in FieldMap.
*        ucPktType            -       Type of the packet received.
*
* OUTPUT:
*        pFieldStartAddr      -       Start bit of specified parameter in packet.
*        pFieldLen            -       Total bits of specified parameter in packet.
*
* RETURNS:
*        OPL_OK                        - Operation is successful.
*        OPL_ERR_NULL_POINTER       - Input pointer is null.
*        CLASSIFY_WINDOWS_OUTOF_RANGE  - Input index of parameter exceeds                                                                            maximal defined index.
*        CLASSIFY_INVALID_PKT_TYPE     - Not defined packet type.
*
* SEE ALSO:
*/
OPL_STATUS candFfieldAddrLenGet(UINT8 fieldNum,
                                UINT16 *pFieldStartAddr,
                                UINT16 *pFieldLen,
                                UINT8 pktType)
{
  if ((NULL == pFieldStartAddr) || (NULL == pFieldLen)) {
    return OPL_ERR_NULL_POINTER;
  }


  switch (pktType) {
    case CLASSIFY_EII_IP:
      if (fieldNum >= CLASSIFY_LAST_NULL_FIELD) {
        return CLASSIFY_WINDOWS_OUTOF_RANGE;
      }

      switch (classifyTagMode) {
        case CLASSIFY_ONE_TAG:
          *pFieldStartAddr = gCEIIFieldSingleTag[fieldNum].fieldOffset;
          *pFieldLen = gCEIIFieldSingleTag[fieldNum].fieldLength;
          break;
        case CLASSIFY_TWO_TAG:
          *pFieldStartAddr = gCEIIFieldDoubleTag[fieldNum].fieldOffset;
          *pFieldLen = gCEIIFieldDoubleTag[fieldNum].fieldLength;
          break;
        default:
          OPL_DRV_PRINTF(("no such tag mode.\n"));
          return OPL_ERROR;
      }
      break;

    case CLASSIFY_PPPOE_IP:
      if (fieldNum >= CLASSIFY_PPPOE_LAST_NULL_FIELD) {
        return CLASSIFY_WINDOWS_OUTOF_RANGE;
      }

      switch (classifyTagMode) {
        case CLASSIFY_ONE_TAG:
          *pFieldStartAddr = gCPPPoEFieldSingleTag[fieldNum].fieldOffset;
          *pFieldLen = gCPPPoEFieldSingleTag[fieldNum].fieldLength;
          break;
        case CLASSIFY_TWO_TAG:
          *pFieldStartAddr = gCPPPoEFieldDoubleTag[fieldNum].fieldOffset;
          *pFieldLen = gCPPPoEFieldDoubleTag[fieldNum].fieldLength;
          break;
        default:
          OPL_DRV_PRINTF(("no such tag mode.\n"));
          return OPL_ERROR;
      }
      break;
    default:
      return CLASSIFY_INVALID_PKT_TYPE;
      break;
  }
  /*If there many different packet type, .e.g. one tag/two tag, so
    can add code below to adjust the field and the length.*/
  return OPL_OK;
}


/*******************************************************************************
*
* clsAddrMapToTbl -   Map classify rule ID to address table.
*
* DESCRIPTION:
*        This function map classify rule ID to address table.
*
* INPUT:
*    offset   - Offset of classify address table.
*    ruleID     - Classify rule ID.
*
* OUTPUT:
*    pulBuf     - Pointer to address buffer.
*
* RETURNS:
*        OPL_OK     - Operation is successful.
*
* SEE ALSO:
*/
OPL_STATUS clsAddrMapToTbl(UINT16 offset,
                           UINT16 ruleID,
                           CLASSIFY_ADDR_TBL_t *pBuf)
{
  switch (offset) {
    case 0:
      pBuf->bfAddr0 = ruleID;
      break;
    case 1:
      pBuf->bfAddr1 = ruleID - CLASSIFY_ENTRYS_PER_GROUP * offset;
      break;
    case 2:
      pBuf->bfAddr2 = ruleID - CLASSIFY_ENTRYS_PER_GROUP * offset;
      break;
    case 3:
      pBuf->bfAddr3 = ruleID - CLASSIFY_ENTRYS_PER_GROUP * offset;
      break;
    default:
      break;
  }

  return OPL_OK;
}


/*******************************************************************************
*
* clsTblMapToAddr -   Read classify rule ID from address table.
*
* DESCRIPTION:
*        This function read classify rule ID from address table.
*
* INPUT:
*    offset   - Offset of classify address table.
*    pBuf     - Pointer to address buffer.
*
* OUTPUT:
*    pRule    - Pointer to classify rule ID.
*
* RETURNS:
*        OPL_OK   - Operation is successful.
*
* SEE ALSO:
*/
OPL_STATUS clsTblMapToAddr(UINT16 offset,
                           UINT16 *pRule,
                           CLASSIFY_ADDR_TBL_t *pBuf)
{
  switch (offset) {
    case 0:
      *pRule = (UINT16) (pBuf->bfAddr0);
      break;
    case 1:
      *pRule = (UINT16) (pBuf->bfAddr1);
      *pRule = *pRule + CLASSIFY_ENTRYS_PER_GROUP;
      break;
    case 2:
      *pRule = (UINT16) (pBuf->bfAddr2);
      *pRule = *pRule + CLASSIFY_ENTRYS_PER_GROUP * offset;
      break;
    case 3:
      *pRule = (UINT16) (pBuf->bfAddr3);
      *pRule = *pRule + CLASSIFY_ENTRYS_PER_GROUP * offset;
      break;
    default:
      break;
  }

  return OPL_OK;
}



/*******************************************************************************
*
* clsMselModByMask  - Move the mask entry from source to destination.
*
* DESCRIPTION:
*    Move the mask entry from source to destination.
*
* INPUT:
*    ucSrcMsel      - Source mask index.
*    ucDestMsel   - Destination mask index.
*
* OUTPUT:
*    NULL.
*
* RETURNS:
*        OPL_OK           - If the operation is successful.
*        PARAMETER_INPUT_POINTER_NULL - If the input pointer is NULL.
*
* SEE ALSO:
*/
OPL_STATUS clsMselModByMask(UINT8 srcMsel,UINT8 destMsel)
{
  UINT16 ruleID;

  for (ruleID = 1; ruleID < CLASSIFY_MAX_RULE_ENTRY; ruleID++) {
    /*The first entry of every group is NULL, should not be written.
    reference the malloc process pls.*/
    if (0 == ruleID % CLASSIFY_ENTRYS_PER_GROUP) {
      continue;
    }

    if ((gpCruleTbl[ruleID].bfMsel == srcMsel)
     && (OPL_TRUE == gpMallocCtl[ruleID].mallocFlag)) {
      gpCruleTbl[ruleID].bfMsel = destMsel;
      dalClsRuleHwWr(&(gpCruleTbl[ruleID]), ruleID);
    }
  }

  return OPL_OK;
}


/*******************************************************************************
*
* clsMselFlagSet  - Make flag to selected mask entry.
*
* DESCRIPTION:
*    Make flag to selected mask entry. Flag indicates the entry is malloced
*    or cleared.
*
* INPUT:
*    msel   - Selected mask ID.
*    opType   - Action type which is select or clear.
*
* OUTPUT:
*    NULL.
*
* RETURNS:
*    NULL.
*
* SEE ALSO:
*/
void clsMselFlagSet(UINT8 msel,UINT8 opType)
{
  UINT16 ruleID;

  for (ruleID = 1; ruleID < CLASSIFY_MAX_RULE_ENTRY; ruleID++) {
    /*The first entry of every group is NULL, should not be written.
      reference the malloc process pls.*/
    if (0 == ruleID % CLASSIFY_ENTRYS_PER_GROUP) {
      continue;
    }

    if (CLASSIFY_MSEL_SET_FLAG == opType) {
      if ((msel == gpCruleTbl[ruleID].bfMsel)
       && (OPL_TRUE == gpMallocCtl[ruleID].mallocFlag)) {
        gpMallocCtl[ruleID].modMselFlag = OPL_TRUE;
      }
    }

    if (CLASSIFY_MSEL_CLR_FLAG == opType) {
      gpMallocCtl[ruleID].modMselFlag = OPL_FALSE;
    }
  }
}


/*******************************************************************************
*
* clsMselModByFlag  - Change the reference mask of the rule.
*
* DESCRIPTION:
*    Change the reference mask of the rule.
*
* INPUT:
*    msel     - Selected mask ID.
*
* OUTPUT:
*    NULL.
*
* RETURNS:
*    OPL_OK             - If the operation is successful.
*        PARAMETER_INPUT_POINTER_NULL - If the input pointer is NULL.
*    Others                     -   an appropriate specific error code defined
*                     for CLT device driver.
* SEE ALSO:
*/
OPL_STATUS clsMselModByFlag(UINT8 msel)
{
  UINT16 ruleID;
  OPL_STATUS iStatus;

  for (ruleID = 1; ruleID < CLASSIFY_MAX_RULE_ENTRY; ruleID++) {
    /*The first entry of every group is NULL, should not be written.
      reference the malloc process pls.*/
    if (0 == ruleID % CLASSIFY_ENTRYS_PER_GROUP) {
      continue;
    }

    if ((OPL_VALID == gpMallocCtl[ruleID].mallocFlag)
     && (OPL_VALID == gpMallocCtl[ruleID].modMselFlag)) {
      gpCruleTbl[ruleID].bfMsel = msel;
      iStatus = dalClsRuleHwWr(&(gpCruleTbl[ruleID]), ruleID);
      if (OPL_OK != iStatus) {
        return iStatus;
      }
    }
  }

  return OPL_OK;
}

/*******************************************************************************
*
* clsInit - Change the reference mask of the rule.
*
*
* SEE ALSO:
*/
OPL_STATUS clsInit(void)
{
  OPL_STATUS retVal = OPL_OK;
  CLASSIFY_RULE_TBL_t clsRuleEntry;
  CLASSIFY_MASK_TBL_t clsMaskEntry;
  CLASSIFY_ADDR_TBL_t clsAddrEntry;
  UINT32 entryId;

  return oplRegFieldWrite(REG_CLE_INIT, 0, 1, 1);

  OPL_MEMSET(&clsRuleEntry, 0X00, sizeof(CLASSIFY_RULE_TBL_t));
  OPL_MEMSET(&clsMaskEntry, 0X00, sizeof(CLASSIFY_MASK_TBL_t));
  OPL_MEMSET(&clsAddrEntry, 0X00, sizeof(CLASSIFY_ADDR_TBL_t));

  for (entryId = OPL_ZERO; entryId < CLASSIFY_MAX_RULE_ENTRY; entryId++) {
    retVal = clsRuleEntryHwWrite(entryId, &clsRuleEntry);
    if (OPL_OK != retVal) {
      OPL_DRV_PRINTF(("init rule table %d failed.\n", entryId));
      return retVal;
    }
  }

  for (entryId = OPL_ZERO; entryId < CLASSIFY_MAX_MASK_ENTRY; entryId++) {
    retVal = clsMaskEntryHwWrite(entryId, &clsMaskEntry);
    if (OPL_OK != retVal) {
      OPL_DRV_PRINTF(("init mask table %d failed.\n", entryId));
      return retVal;
    }
  }

  for (entryId = OPL_ZERO; entryId < CLASSIFY_MAX_ADDR_ENTRY; entryId++) {
    retVal = clsAddrEntryHwWrite(entryId, &clsAddrEntry);
    if (OPL_OK != retVal) {
      OPL_DRV_PRINTF(("init addr table %d failed.\n", entryId));
      return retVal;
    }
  }
  return retVal;
}
/*******************************************************************************
*
* clsRuleEntryHwWrite - write a rule to the classify rule table.
*
* DESCRIPTION:
*    write a rule to the classify rule table.
*
* INPUT:
*    entryId: rule entryId
*        pClsRuleEntry: value
*
* OUTPUT:
*    NULL.
*
* RETURNS:
*    OPL_OK             - If the operation is successful.
*        PARAMETER_INPUT_POINTER_NULL - If the input pointer is NULL.
*    Others                     -   an appropriate specific error code defined
*                     for CLT device driver.
* SEE ALSO:
*/
OPL_STATUS clsRuleEntryHwWrite(UINT32 entryId,
                               CLASSIFY_RULE_TBL_t *pClsRuleEntry)
{
  if (entryId >= TAB_CLS_RULE_LEN) {
    OPL_PRINTF("rule entryId out of range.\n");
    return OPL_ERR_INVALID_PARAMETERS;
  }
  if (OPL_NULL == pClsRuleEntry) {
    return OPL_ERR_NULL_POINTER;
  }

  return oplTabWrite(OPCONN_TAB_CLS_RULE, entryId, 1, (UINT32 *) pClsRuleEntry);
}
/*******************************************************************************
*
* clsRuleEntryHwRead  - read a rule from the classify rule table.
*
* DESCRIPTION:
*    write a rule to the classify rule table.
*
* INPUT:
*    entryId: rule entryId
*
* OUTPUT:
*      pClsRuleEntry: value
*
* RETURNS:
*    OPL_OK             - If the operation is successful.
*        PARAMETER_INPUT_POINTER_NULL - If the input pointer is NULL.
*    Others                     -   an appropriate specific error code defined
*                     for CLT device driver.
* SEE ALSO:
*/
OPL_STATUS clsRuleEntryHwRead(UINT32 entryId,
                              CLASSIFY_RULE_TBL_t *pClsRuleEntry)
{
  if (entryId >= TAB_CLS_RULE_LEN) {
    OPL_PRINTF("rule entryId out of range.\n");
    return OPL_ERR_INVALID_PARAMETERS;
  }
  if (OPL_NULL == pClsRuleEntry) {
    return OPL_ERR_NULL_POINTER;
  }

  return oplTabRead(OPCONN_TAB_CLS_RULE, entryId, 1, (UINT32 *) pClsRuleEntry);
}
/*******************************************************************************
*
* clsAddrEntryHwWrite - write a addr to the classify addr table.
*
* DESCRIPTION:
*    write a addr to the addr table.
*
* INPUT:
*    entryId: rule entryId
*        pClsAddrEntry: value
*
* OUTPUT:
*    NULL.
*
* RETURNS:
*    OPL_OK             - If the operation is successful.
*        PARAMETER_INPUT_POINTER_NULL - If the input pointer is NULL.
*    Others                     -   an appropriate specific error code defined
*                     for CLT device driver.
* SEE ALSO:
*/
OPL_STATUS clsAddrEntryHwWrite(UINT32 entryId,
                               CLASSIFY_ADDR_TBL_t *pClsAddrEntry)
{
  if (entryId >= TAB_CLS_ADDR_LEN) {
    OPL_PRINTF("addr entryId out of range.\n");
    return OPL_ERR_INVALID_PARAMETERS;
  }
  if (OPL_NULL == pClsAddrEntry) {
    return OPL_ERR_NULL_POINTER;
  }

  return oplTabWrite(OPCONN_TAB_CLS_ADDR, entryId, 1, (UINT32 *) pClsAddrEntry);
}
/*******************************************************************************
*
* clsAddrEntryHwRead  - read a addr from the classify addr table.
*
* DESCRIPTION:
*    write a addr to the addr table.
*
* INPUT:
*    entryId: rule entryId
*        pClsAddrEntry: value
*
* OUTPUT:
*    NULL.
*
* RETURNS:
*    OPL_OK             - If the operation is successful.
*        PARAMETER_INPUT_POINTER_NULL - If the input pointer is NULL.
*    Others                     -   an appropriate specific error code defined
*                     for CLT device driver.
* SEE ALSO:
*/
OPL_STATUS clsAddrEntryHwRead(UINT32 entryId,
                              CLASSIFY_ADDR_TBL_t *pClsAddrEntry)
{
  if (entryId >= TAB_CLS_ADDR_LEN) {
    OPL_PRINTF("addr entryId out of range.\n");
    return OPL_ERR_INVALID_PARAMETERS;
  }
  if (OPL_NULL == pClsAddrEntry) {
    return OPL_ERR_NULL_POINTER;
  }

  return oplTabRead(OPCONN_TAB_CLS_ADDR, entryId, 1, (UINT32 *) pClsAddrEntry);
}
/*******************************************************************************
*
* clsMaskEntryHwWrite - write a mask to the classify mask table.
*
* DESCRIPTION:
*    write a addr to the addr table.
*
* INPUT:
*    entryId: rule entryId
*        pClsMaskEntry: value
*
* OUTPUT:
*    NULL.
*
* RETURNS:
*    OPL_OK             - If the operation is successful.
*        PARAMETER_INPUT_POINTER_NULL - If the input pointer is NULL.
*    Others                     -   an appropriate specific error code defined
*                     for CLT device driver.
* SEE ALSO:
*/
OPL_STATUS clsMaskEntryHwWrite(UINT32 entryId,
                               CLASSIFY_MASK_TBL_t *pClsMaskEntry)
{
  if (entryId >= TAB_CLS_MASK_LEN) {
    OPL_PRINTF("mask entryId out of range.\n");
    return OPL_ERR_INVALID_PARAMETERS;
  }
  if (OPL_NULL == pClsMaskEntry) {
    return OPL_ERR_NULL_POINTER;
  }

  return oplTabWrite(OPCONN_TAB_CLS_MASK, entryId, 1, (UINT32 *) pClsMaskEntry);
}
/*******************************************************************************
*
* clsMaskEntryHwRead  - read a mask from the classify mask table.
*
* DESCRIPTION:
*    write a mask to the mask table.
*
* INPUT:
*    entryId: rule entryId
*
*
* OUTPUT:
*    pClsMaskEntry: value
*
* RETURNS:
*    OPL_OK             - If the operation is successful.
*        PARAMETER_INPUT_POINTER_NULL - If the input pointer is NULL.
*    Others                     -   an appropriate specific error code defined
*                     for CLT device driver.
* SEE ALSO:
*/
OPL_STATUS clsMaskEntryHwRead(UINT32 entryId,
                              CLASSIFY_MASK_TBL_t *pClsMaskEntry)
{
  if (entryId >= TAB_CLS_MASK_LEN) {
    OPL_PRINTF("mask entryId out of range.\n");
    return OPL_ERR_INVALID_PARAMETERS;
  }
  if (OPL_NULL == pClsMaskEntry) {
    return OPL_ERR_NULL_POINTER;
  }

  return oplTabRead(OPCONN_TAB_CLS_MASK, entryId, 1, (UINT32 *) pClsMaskEntry);
}
/*******************************************************************************
*
* clsRuleEntryShow  - show classify rule table in asic.
*
* DESCRIPTION:
*    show classify rule table in asic.
*
* INPUT:
*   startId:
*   endId:
*
* OUTPUT:
*
*
* RETURNS:
*    OPL_OK             - If the operation is successful.
*        PARAMETER_INPUT_POINTER_NULL - If the input pointer is NULL.
*    Others                     -   an appropriate specific error code defined
*                     for CLT device driver.
* SEE ALSO:
*/
OPL_STATUS clsRuleEntryShow(UINT32 startId,UINT32 endId)
{
  OPL_STATUS retVal = OPL_OK;
  UINT32 entryId;
  CLASSIFY_RULE_TBL_t clsRuleEntry;

  if (startId >= TAB_CLS_RULE_LEN || endId >= TAB_CLS_RULE_LEN) {
    OPL_DRV_SHOW_PRINTF(("entryId out of rule table range.\n"));
    return OPL_ERR_INVALID_PARAMETERS;
  }

  OPL_DRV_SHOW_PRINTF(("%-3s %-4s %-6s %-7s %-6s %-6s %-7s %-4s %-6s %s\n",
                       "No",
                       "Tact",
                       "egress",
                       "ingress",
                       "rmkAct",
                       "rmkCos",
                       "rmkIcos",
                       "vid",
                       "MaskId",
                       "ruleVal"));

  for (entryId = startId; entryId <= endId; entryId++) {
    retVal = clsRuleEntryHwRead(entryId, &clsRuleEntry);
    if (OPL_OK != retVal) {
      break;
    }
    OPL_DRV_SHOW_PRINTF(("%-3d %-4x %-6x %-7x %-6x %-6x %-7x %-4d %-6x\n",
                         entryId,
                         clsRuleEntry.bfTranActH
                      << 3
                       | clsRuleEntry.bfTranActL,
                         clsRuleEntry.bfEgress,
                         clsRuleEntry.bfIngress,
                         clsRuleEntry.bfRmkAct,
                         clsRuleEntry.bfRmkCos,
                         clsRuleEntry.bfRmkIcos,
                         clsRuleEntry.bfRmkVid,
                         clsRuleEntry.bfMsel));
    OPL_DRV_SHOW_PRINTF(("%08x %08x %08x %08x %08x %08x\n",
                         *((UINT32 *) &(clsRuleEntry.Rule[0])),
                         *((UINT32 *) &(clsRuleEntry.Rule[4])),
                         *((UINT32 *) &(clsRuleEntry.Rule[8])),
                         *((UINT32 *) &(clsRuleEntry.Rule[12])),
                         *((UINT32 *) &(clsRuleEntry.Rule[16])),
                         *((UINT32 *) &(clsRuleEntry.Rule[20]))));
  }
  return retVal;
}
/*******************************************************************************
*
* clsAddrEntryShow  - show classify address table in asic.
*
* DESCRIPTION:
*    show classify address table in asic.
*
* INPUT:
*   startId:
*   endId:
*
* OUTPUT:
*
*
* RETURNS:
*    OPL_OK             - If the operation is successful.
*        PARAMETER_INPUT_POINTER_NULL - If the input pointer is NULL.
*    Others                     -   an appropriate specific error code defined
*                     for CLT device driver.
* SEE ALSO:
*/
OPL_STATUS clsAddrEntryShow(UINT32 startId,UINT32 endId)
{
  OPL_STATUS retVal = OPL_OK;
  UINT32 entryId;
  CLASSIFY_ADDR_TBL_t clsAddrEntry;

  if (startId >= TAB_CLS_ADDR_LEN || endId >= TAB_CLS_ADDR_LEN) {
    OPL_DRV_SHOW_PRINTF(("entryId out of addr table range.\n"));
    return OPL_ERR_INVALID_PARAMETERS;
  }

  OPL_DRV_SHOW_PRINTF(("%-3s %-4s %-4s %-4s %-4s\n",
                       "No",
                       "addr0",
                       "addr1",
                       "addr2",
                       "addr3"));

  for (entryId = startId; entryId <= endId; entryId++) {
    retVal = clsAddrEntryHwRead(entryId, &clsAddrEntry);
    if (OPL_OK != retVal) {
      break;
    }
    OPL_DRV_SHOW_PRINTF(("%-3d %-4d %-4d %-4d %-4d\n",
                         entryId,
                         clsAddrEntry.bfAddr0,
                         clsAddrEntry.bfAddr1,
                         clsAddrEntry.bfAddr2,
                         clsAddrEntry.bfAddr3));
  }
  return retVal;
}
/*******************************************************************************
*
* clsMaskEntryShow  - show classify mask table in asic.
*
* DESCRIPTION:
*    show classify mask table in asic.
*
* INPUT:
*   startId:
*   endId:
*
* OUTPUT:
*
*
* RETURNS:
*    OPL_OK             - If the operation is successful.
*        PARAMETER_INPUT_POINTER_NULL - If the input pointer is NULL.
*    Others                     -   an appropriate specific error code defined
*                     for CLT device driver.
* SEE ALSO:
*/
OPL_STATUS clsMaskEntryShow(UINT32 startId,UINT32 endId)
{
  OPL_STATUS retVal = OPL_OK;
  UINT32 entryId;
  CLASSIFY_MASK_TBL_t clsMaskEntry;

  if (startId >= TAB_CLS_MASK_LEN || endId >= TAB_CLS_MASK_LEN) {
    OPL_DRV_SHOW_PRINTF(("entryId out of mask table range.\n"));
    return OPL_ERR_INVALID_PARAMETERS;
  }

  OPL_DRV_SHOW_PRINTF(("%-3s %-5s %-5s %-5s %-3s %-3s %-3s %-3s %-3s %-3s %s\n",
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
                       "maskVal"));

  for (entryId = startId; entryId <= endId; entryId++) {
    retVal = clsMaskEntryHwRead(entryId, &clsMaskEntry);
    if (OPL_OK != retVal) {
      break;
    }
    OPL_DRV_SHOW_PRINTF(("%-3d %-5x %-5x %-5x %-3d %-3d %-3d %-3d %-3d %-3d\n",
                         entryId,
                         clsMaskEntry.bfValid,
                         clsMaskEntry.bfImask,
                         clsMaskEntry.bfEmask,
                         clsMaskEntry.bfOffset6,
                         clsMaskEntry.bfOffset5,
                         clsMaskEntry.bfOffset4,
                         clsMaskEntry.bfOffset3,
                         clsMaskEntry.bfOffset2,
                         clsMaskEntry.bfOffset1));

    OPL_DRV_SHOW_PRINTF(("%08x %08x %08x %08x %08x %08x\n",
                         *((UINT32 *) &(clsMaskEntry.Mask[0])),
                         *((UINT32 *) &(clsMaskEntry.Mask[4])),
                         *((UINT32 *) &(clsMaskEntry.Mask[8])),
                         *((UINT32 *) &(clsMaskEntry.Mask[12])),
                         *((UINT32 *) &(clsMaskEntry.Mask[16])),
                         *((UINT32 *) &(clsMaskEntry.Mask[20]))));
  }
  return retVal;
}
/*******************************************************************************
*
* clsCounterEntryShow - show classify counter table in asic.
*
* DESCRIPTION:
*    show classify counter table in asic.
*
* INPUT:
*   startId:
*   endId:
*
* OUTPUT:
*
*
* RETURNS:
*    OPL_OK             - If the operation is successful.
*        PARAMETER_INPUT_POINTER_NULL - If the input pointer is NULL.
*    Others                     -   an appropriate specific error code defined
*                     for CLT device driver.
* SEE ALSO:
*/
OPL_STATUS clsCounterEntryShow(UINT32 startId,UINT32 endId)
{
  OPL_STATUS retVal = OPL_OK;
  UINT32 entryId;
  UINT32 clsCounter;

  if (startId >= TAB_CLS_COUNTER_LEN || endId >= TAB_CLS_COUNTER_LEN) {
    OPL_DRV_SHOW_PRINTF(("entryId out of counter table range.\n"));
    return OPL_ERR_INVALID_PARAMETERS;
  }
  OPL_DRV_SHOW_PRINTF(("%-3s %-8s\n", "No", "counter"));
  for (entryId = startId; entryId < endId; entryId++) {
    retVal = oplTabRead(OPCONN_TAB_CLS_COUNTER, entryId, 1, &clsCounter);
    if (OPL_OK != retVal) {
      break;
    }
    OPL_DRV_SHOW_PRINTF(("%-3d %-8d\n", entryId, clsCounter));
  }
  return retVal;
}

OPL_STATUS clsLastWindowCreate(UINT32 comVal,
                               UINT32 mask,
                               UINT8 imask,
                               UINT8 emask,
                               UINT8 tranAct,
                               UINT8 egressPort,
                               UINT8 ingressPort,
                               UINT8 remarkAct,
                               UINT8 remarkCos,
                               UINT8 remarkIcos,
                               UINT8 remarkVid,
                               UINT8 enable)
{
  CLASSIFY_MASK_TBL_t lastWindowTestMask = {
    0x0, 0x1, 0x0, 0x0, 0, 0, 11, 11, 0, 0, {
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0xff,0xff,
    0xff,0xff,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00
    }
  };

  CLASSIFY_RULE_TBL_t lastWindowTestRule;
  CLASSIFY_ADDR_TBL_t lastWindowTestAddr;
  UINT16 hashAddr;
  OPL_STATUS retVal = OPL_OK;

  OPL_MEMSET(&lastWindowTestRule, 0X0, sizeof(CLASSIFY_RULE_TBL_t));
  OPL_MEMSET(&lastWindowTestAddr, 0X0, sizeof(CLASSIFY_ADDR_TBL_t));

  lastWindowTestMask.bfEmask = emask;
  lastWindowTestMask.bfImask = imask;
  lastWindowTestMask.Mask[10] = ((mask >> 24) & 0xff) ;
  lastWindowTestMask.Mask[11] = ((mask >> 16) & 0xff);
  lastWindowTestMask.Mask[12] = ((mask >> 8) & 0xff);
  lastWindowTestMask.Mask[13] = (mask & 0xff);

  lastWindowTestRule.bfTranActH = tranAct >> 3;
  lastWindowTestRule.bfTranActL = tranAct & 0x7;
  lastWindowTestRule.bfEgress = egressPort & emask;
  lastWindowTestRule.bfIngress = ingressPort & imask;
  lastWindowTestRule.bfRmkAct = remarkAct;
  lastWindowTestRule.bfRmkCos = remarkCos;
  lastWindowTestRule.bfRmkIcos = remarkIcos;
  lastWindowTestRule.bfRmkVid = remarkVid;
  lastWindowTestRule.bfMsel = 0;

  lastWindowTestRule.Rule[10] = lastWindowTestMask.Mask[10] & ((comVal >> 24) & 0xff);
  lastWindowTestRule.Rule[11] = lastWindowTestMask.Mask[11] & ((comVal >> 16) & 0xff);
  lastWindowTestRule.Rule[12] = lastWindowTestMask.Mask[12] & ((comVal >> 8) & 0xff);
  lastWindowTestRule.Rule[13] = lastWindowTestMask.Mask[13] & (comVal & 0xff);

  retVal = clsAddrCal((CLASSIFY_RULE_ENTRY_t *) (&lastWindowTestRule),
                      &hashAddr);
  if (OPL_OK != retVal) {
    OPL_DRV_PRINTF(("cal hashAddress failed.\n"));
    return retVal;
  }

  OPL_DRV_PRINTF(("hasAddr = %d\n", hashAddr));

  if (enable) {
    lastWindowTestAddr.bfAddr0 = 1;

    retVal = clsMaskEntryHwWrite(0, &lastWindowTestMask);
    if (OPL_OK != retVal) {
      OPL_DRV_PRINTF(("write mask failed.\n"));
      return retVal;
    }
    retVal = clsRuleEntryHwWrite(1, &lastWindowTestRule);
    if (OPL_OK != retVal) {
      OPL_DRV_PRINTF(("write rule failed.\n"));
      return retVal;
    }

    retVal = clsAddrEntryHwWrite(hashAddr, &lastWindowTestAddr);
    if (OPL_OK != retVal) {
      OPL_DRV_PRINTF(("write hash address failed.\n"));
      return retVal;
    }
  }
  else {
    OPL_MEMSET(&lastWindowTestMask, 0X0, sizeof(CLASSIFY_RULE_TBL_t));
    OPL_MEMSET(&lastWindowTestRule, 0X0, sizeof(CLASSIFY_RULE_TBL_t));
    OPL_MEMSET(&lastWindowTestAddr, 0X0, sizeof(CLASSIFY_ADDR_TBL_t));

    retVal = clsMaskEntryHwWrite(0, &lastWindowTestMask);
    if (OPL_OK != retVal) {
      OPL_DRV_PRINTF(("write mask failed.\n"));
      return retVal;
    }
    retVal = clsRuleEntryHwWrite(1, &lastWindowTestRule);
    if (OPL_OK != retVal) {
      OPL_DRV_PRINTF(("write rule failed.\n"));
      return retVal;
    }

    retVal = clsAddrEntryHwWrite(hashAddr, &lastWindowTestAddr);
    if (OPL_OK != retVal) {
      OPL_DRV_PRINTF(("write hash address failed.\n"));
      return retVal;
    }
  }

  return retVal;
}

OPL_STATUS clsFirstWindowCreate(UINT32 comVal,
                                UINT32 mask,
                                UINT8 imask,
                                UINT8 emask,
                                UINT8 tranAct,
                                UINT8 egressPort,
                                UINT8 ingressPort,
                                UINT8 remarkAct,
                                UINT8 remarkCos,
                                UINT8 remarkIcos,
                                UINT8 remarkVid,
                                UINT8 enable)
{
  CLASSIFY_MASK_TBL_t lastWindowTestMask = {
    0x0, 0x1, 0x0, 0x0, 0, 0, 0, 0, 0, 0, {
    0x00,0x00,0xff,0xff,
    0xff,0xff,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00
    }
  };

  CLASSIFY_RULE_TBL_t lastWindowTestRule;
  CLASSIFY_ADDR_TBL_t lastWindowTestAddr;
  UINT16 hashAddr;
  OPL_STATUS retVal = OPL_OK;

  OPL_MEMSET(&lastWindowTestRule, 0X0, sizeof(CLASSIFY_RULE_TBL_t));
  OPL_MEMSET(&lastWindowTestAddr, 0X0, sizeof(CLASSIFY_ADDR_TBL_t));

  lastWindowTestMask.bfEmask = emask;
  lastWindowTestMask.bfImask = imask;
  lastWindowTestMask.Mask[2] = ((mask >> 24) & 0xff) ;
  lastWindowTestMask.Mask[3] = ((mask >> 16) & 0xff);
  lastWindowTestMask.Mask[4] = ((mask >> 8) & 0xff);
  lastWindowTestMask.Mask[5] = (mask & 0xff);

  lastWindowTestRule.bfTranActH = tranAct >> 3;
  lastWindowTestRule.bfTranActL = tranAct & 0x7;
  lastWindowTestRule.bfEgress = egressPort & emask;
  lastWindowTestRule.bfIngress = ingressPort & imask;
  lastWindowTestRule.bfRmkAct = remarkAct;
  lastWindowTestRule.bfRmkCos = remarkCos;
  lastWindowTestRule.bfRmkIcos = remarkIcos;
  lastWindowTestRule.bfRmkVid = remarkVid;
  lastWindowTestRule.bfMsel = 0;

  lastWindowTestRule.Rule[2] = lastWindowTestMask.Mask[2] & ((comVal >> 24) & 0xff);
  lastWindowTestRule.Rule[3] = lastWindowTestMask.Mask[3] & ((comVal >> 16) & 0xff);
  lastWindowTestRule.Rule[4] = lastWindowTestMask.Mask[4] & ((comVal >> 8) & 0xff);
  lastWindowTestRule.Rule[5] = lastWindowTestMask.Mask[5] & (comVal & 0xff);

  retVal = clsAddrCal((CLASSIFY_RULE_ENTRY_t *) (&lastWindowTestRule),
                      &hashAddr);
  if (OPL_OK != retVal) {
    OPL_DRV_PRINTF(("cal hashAddress failed.\n"));
    return retVal;
  }

  OPL_DRV_PRINTF(("hasAddr = %d\n", hashAddr));

  if (enable) {
    lastWindowTestAddr.bfAddr0 = 1;

    retVal = clsMaskEntryHwWrite(0, &lastWindowTestMask);
    if (OPL_OK != retVal) {
      OPL_DRV_PRINTF(("write mask failed.\n"));
      return retVal;
    }
    retVal = clsRuleEntryHwWrite(1, &lastWindowTestRule);
    if (OPL_OK != retVal) {
      OPL_DRV_PRINTF(("write rule failed.\n"));
      return retVal;
    }

    retVal = clsAddrEntryHwWrite(hashAddr, &lastWindowTestAddr);
    if (OPL_OK != retVal) {
      OPL_DRV_PRINTF(("write hash address failed.\n"));
      return retVal;
    }
  }
  else {
    OPL_MEMSET(&lastWindowTestMask, 0X0, sizeof(CLASSIFY_RULE_TBL_t));
    OPL_MEMSET(&lastWindowTestRule, 0X0, sizeof(CLASSIFY_RULE_TBL_t));
    OPL_MEMSET(&lastWindowTestAddr, 0X0, sizeof(CLASSIFY_ADDR_TBL_t));

    retVal = clsMaskEntryHwWrite(0, &lastWindowTestMask);
    if (OPL_OK != retVal) {
      OPL_DRV_PRINTF(("write mask failed.\n"));
      return retVal;
    }
    retVal = clsRuleEntryHwWrite(1, &lastWindowTestRule);
    if (OPL_OK != retVal) {
      OPL_DRV_PRINTF(("write rule failed.\n"));
      return retVal;
    }

    retVal = clsAddrEntryHwWrite(hashAddr, &lastWindowTestAddr);
    if (OPL_OK != retVal) {
      OPL_DRV_PRINTF(("write hash address failed.\n"));
      return retVal;
    }
  }

  return retVal;
}

OPL_STATUS cls72Start4ByteCreate(UINT32 comVal,
                                 UINT32 mask,
                                 UINT8 imask,
                                 UINT8 emask,
                                 UINT8 tranAct,
                                 UINT8 egressPort,
                                 UINT8 ingressPort,
                                 UINT8 remarkAct,
                                 UINT8 remarkCos,
                                 UINT8 remarkIcos,
                                 UINT8 remarkVid,
                                 UINT8 enable)
{
  CLASSIFY_MASK_TBL_t lastWindowTestMask = {
    0x0, 0x1, 0x0, 0x0, 0, 0, 0, 11, 11, 0, {
    0x00,0x00,0x00,0x00,
    0x00,0x00,0xff,0xff,
    0xff,0xff,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00
    }
  };

  CLASSIFY_RULE_TBL_t lastWindowTestRule;
  CLASSIFY_ADDR_TBL_t lastWindowTestAddr;
  UINT16 hashAddr;
  OPL_STATUS retVal = OPL_OK;

  OPL_MEMSET(&lastWindowTestRule, 0X0, sizeof(CLASSIFY_RULE_TBL_t));
  OPL_MEMSET(&lastWindowTestAddr, 0X0, sizeof(CLASSIFY_ADDR_TBL_t));

  lastWindowTestMask.bfEmask = emask;
  lastWindowTestMask.bfImask = imask;
  lastWindowTestMask.Mask[6] = ((mask >> 24) & 0xff) ;
  lastWindowTestMask.Mask[7] = ((mask >> 16) & 0xff);
  lastWindowTestMask.Mask[8] = ((mask >> 8) & 0xff);
  lastWindowTestMask.Mask[9] = (mask & 0xff);

  lastWindowTestRule.bfTranActH = tranAct >> 3;
  lastWindowTestRule.bfTranActL = tranAct & 0x7;
  lastWindowTestRule.bfEgress = egressPort & emask;
  lastWindowTestRule.bfIngress = ingressPort & imask;
  lastWindowTestRule.bfRmkAct = remarkAct;
  lastWindowTestRule.bfRmkCos = remarkCos;
  lastWindowTestRule.bfRmkIcos = remarkIcos;
  lastWindowTestRule.bfRmkVid = remarkVid;
  lastWindowTestRule.bfMsel = 0;

  lastWindowTestRule.Rule[6] = lastWindowTestMask.Mask[6] & ((comVal >> 24) & 0xff);
  lastWindowTestRule.Rule[7] = lastWindowTestMask.Mask[7] & ((comVal >> 16) & 0xff);
  lastWindowTestRule.Rule[8] = lastWindowTestMask.Mask[8] & ((comVal >> 8) & 0xff);
  lastWindowTestRule.Rule[9] = lastWindowTestMask.Mask[9] & (comVal & 0xff);

  retVal = clsAddrCal((CLASSIFY_RULE_ENTRY_t *) (&lastWindowTestRule),
                      &hashAddr);
  if (OPL_OK != retVal) {
    OPL_DRV_PRINTF(("cal hashAddress failed.\n"));
    return retVal;
  }

  OPL_DRV_PRINTF(("hasAddr = %d\n", hashAddr));

  if (enable) {
    lastWindowTestAddr.bfAddr0 = 1;

    retVal = clsMaskEntryHwWrite(0, &lastWindowTestMask);
    if (OPL_OK != retVal) {
      OPL_DRV_PRINTF(("write mask failed.\n"));
      return retVal;
    }
    retVal = clsRuleEntryHwWrite(1, &lastWindowTestRule);
    if (OPL_OK != retVal) {
      OPL_DRV_PRINTF(("write rule failed.\n"));
      return retVal;
    }

    retVal = clsAddrEntryHwWrite(hashAddr, &lastWindowTestAddr);
    if (OPL_OK != retVal) {
      OPL_DRV_PRINTF(("write hash address failed.\n"));
      return retVal;
    }
  }
  else {
    OPL_MEMSET(&lastWindowTestMask, 0X0, sizeof(CLASSIFY_RULE_TBL_t));
    OPL_MEMSET(&lastWindowTestRule, 0X0, sizeof(CLASSIFY_RULE_TBL_t));
    OPL_MEMSET(&lastWindowTestAddr, 0X0, sizeof(CLASSIFY_ADDR_TBL_t));

    retVal = clsMaskEntryHwWrite(0, &lastWindowTestMask);
    if (OPL_OK != retVal) {
      OPL_DRV_PRINTF(("write mask failed.\n"));
      return retVal;
    }
    retVal = clsRuleEntryHwWrite(1, &lastWindowTestRule);
    if (OPL_OK != retVal) {
      OPL_DRV_PRINTF(("write rule failed.\n"));
      return retVal;
    }

    retVal = clsAddrEntryHwWrite(hashAddr, &lastWindowTestAddr);
    if (OPL_OK != retVal) {
      OPL_DRV_PRINTF(("write hash address failed.\n"));
      return retVal;
    }
  }

  return retVal;
}

OPL_STATUS cls74To77Create(UINT32 comVal,
                           UINT32 mask,
                           UINT8 imask,
                           UINT8 emask,
                           UINT8 tranAct,
                           UINT8 egressPort,
                           UINT8 ingressPort,
                           UINT8 remarkAct,
                           UINT8 remarkCos,
                           UINT8 remarkIcos,
                           UINT8 remarkVid,
                           UINT8 enable)
{
  CLASSIFY_MASK_TBL_t lastWindowTestMask = {
    0x0, 0x1, 0x0, 0x0, 0, 0, 0, 11, 0, 0, {
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0xff,0xff,0xff,0xff,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00
    }
  };

  CLASSIFY_RULE_TBL_t lastWindowTestRule;
  CLASSIFY_ADDR_TBL_t lastWindowTestAddr;
  UINT16 hashAddr;
  OPL_STATUS retVal = OPL_OK;

  OPL_MEMSET(&lastWindowTestRule, 0X0, sizeof(CLASSIFY_RULE_TBL_t));
  OPL_MEMSET(&lastWindowTestAddr, 0X0, sizeof(CLASSIFY_ADDR_TBL_t));

  lastWindowTestMask.bfEmask = emask;
  lastWindowTestMask.bfImask = imask;
  lastWindowTestMask.Mask[8] = ((mask >> 24) & 0xff) ;
  lastWindowTestMask.Mask[9] = ((mask >> 16) & 0xff);
  lastWindowTestMask.Mask[10] = ((mask >> 8) & 0xff);
  lastWindowTestMask.Mask[11] = (mask & 0xff);

  lastWindowTestRule.bfTranActH = tranAct >> 3;
  lastWindowTestRule.bfTranActL = tranAct & 0x7;
  lastWindowTestRule.bfEgress = egressPort & emask;
  lastWindowTestRule.bfIngress = ingressPort & imask;
  lastWindowTestRule.bfRmkAct = remarkAct;
  lastWindowTestRule.bfRmkCos = remarkCos;
  lastWindowTestRule.bfRmkIcos = remarkIcos;
  lastWindowTestRule.bfRmkVid = remarkVid;
  lastWindowTestRule.bfMsel = 0;

  lastWindowTestRule.Rule[8] = lastWindowTestMask.Mask[8] & ((comVal >> 24) & 0xff);
  lastWindowTestRule.Rule[9] = lastWindowTestMask.Mask[9] & ((comVal >> 16) & 0xff);
  lastWindowTestRule.Rule[10] = lastWindowTestMask.Mask[10] & ((comVal >> 8) & 0xff);
  lastWindowTestRule.Rule[11] = lastWindowTestMask.Mask[11] & (comVal & 0xff);

  retVal = clsAddrCal((CLASSIFY_RULE_ENTRY_t *) (&lastWindowTestRule),
                      &hashAddr);
  if (OPL_OK != retVal) {
    OPL_DRV_PRINTF(("cal hashAddress failed.\n"));
    return retVal;
  }

  OPL_DRV_PRINTF(("hasAddr = %d\n", hashAddr));

  if (enable) {
    lastWindowTestAddr.bfAddr0 = 1;

    retVal = clsMaskEntryHwWrite(0, &lastWindowTestMask);
    if (OPL_OK != retVal) {
      OPL_DRV_PRINTF(("write mask failed.\n"));
      return retVal;
    }
    retVal = clsRuleEntryHwWrite(1, &lastWindowTestRule);
    if (OPL_OK != retVal) {
      OPL_DRV_PRINTF(("write rule failed.\n"));
      return retVal;
    }

    retVal = clsAddrEntryHwWrite(hashAddr, &lastWindowTestAddr);
    if (OPL_OK != retVal) {
      OPL_DRV_PRINTF(("write hash address failed.\n"));
      return retVal;
    }
  }
  else {
    OPL_MEMSET(&lastWindowTestMask, 0X0, sizeof(CLASSIFY_RULE_TBL_t));
    OPL_MEMSET(&lastWindowTestRule, 0X0, sizeof(CLASSIFY_RULE_TBL_t));
    OPL_MEMSET(&lastWindowTestAddr, 0X0, sizeof(CLASSIFY_ADDR_TBL_t));

    retVal = clsMaskEntryHwWrite(0, &lastWindowTestMask);
    if (OPL_OK != retVal) {
      OPL_DRV_PRINTF(("write mask failed.\n"));
      return retVal;
    }
    retVal = clsRuleEntryHwWrite(1, &lastWindowTestRule);
    if (OPL_OK != retVal) {
      OPL_DRV_PRINTF(("write rule failed.\n"));
      return retVal;
    }

    retVal = clsAddrEntryHwWrite(hashAddr, &lastWindowTestAddr);
    if (OPL_OK != retVal) {
      OPL_DRV_PRINTF(("write hash address failed.\n"));
      return retVal;
    }
  }

  return retVal;
}

