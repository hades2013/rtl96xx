/*
 * Copyright (C) 2012 Realtek Semiconductor Corp. 
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated, 
 * modified or distributed under the authorized license from Realtek. 
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER 
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED. 
 *
 * $Revision: 39101 $
 * $Date: 2013-05-03 04:35:27 -0500 (Fri, 03 May 2013) $
 *
 * Purpose : Definition of MIB int/reset/self create APIs
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) OMCI 
 *
 */


#include "app_basic.h"
#include "omci_defs.h"

#include "omci_mib.h"



/*
*  Define local APIs
*/

static GOS_ERROR_CODE omci_AvcCallback(MIB_TABLE_INDEX tableIndex, void* pOldRow, void* pNewRow, MIB_ATTRS_SET* pAttrsSet, MIB_OPERA_TYPE  operationType)
{
    MIB_ATTR_INDEX     attrIndex;
    UINT32             i, attrSize;
    MIB_ATTRS_SET      avcAttrSet;
    PON_ME_ENTITY_ID   entityID;
    PON_OMCI_MSG_H_T   omciMsg;
    PON_OMCI_ATTRS_SET avcOmciSet;
    GOS_ERROR_CODE     ret;
    CHAR*              pData;

    if (MIB_SET != operationType)
    {
        return GOS_OK;
    }

    GOS_ASSERT(pOldRow && pNewRow && pAttrsSet);

    MIB_ClearAttrSet(&avcAttrSet);

    MIB_GetAttrFromBuf(tableIndex, MIB_ATTR_FIRST_INDEX, &entityID, pNewRow, sizeof(PON_ME_ENTITY_ID));
    omciMsg.msgId.meInstance = entityID;
    omciMsg.msgId.meClass = MIB_GetTableClassId(tableIndex);
    memset(&omciMsg.content[0], 0x00, PON_OMCI_MSG_CONTENT_LEN);

    pData = (CHAR*)(&omciMsg.content[2]);

    for (attrIndex = MIB_ATTR_FIRST_INDEX, i = 0; i < MIB_GetTableAttrNum(tableIndex);
         i++, attrIndex = MIB_ATTR_NEXT_INDEX(attrIndex))
    {
        // Not in attribute set or Not a AVC attribute
        if (!MIB_IsInAttrSet(pAttrsSet, attrIndex) || (!MIB_GetAttrAvcFlag(tableIndex, attrIndex)))
        {
            continue;
        }

        attrSize = MIB_GetAttrSize(tableIndex, attrIndex);
        ret      = GOS_FAIL;

        switch (MIB_GetAttrDataType(tableIndex, attrIndex))
        {
            case MIB_ATTR_TYPE_UINT8:
            {
                UINT8 value, oldValue;
                MIB_GetAttrFromBuf(tableIndex, attrIndex, &value, pNewRow, attrSize);
                MIB_GetAttrFromBuf(tableIndex, attrIndex, &oldValue, pOldRow, attrSize);
                // Make sure that value is changed
                if (value != oldValue)
                {
                    ret = GOS_OK;
                    memcpy(pData, &value, attrSize);
                }
                break;
            }
            case MIB_ATTR_TYPE_STR:
            {
                CHAR value[MIB_TABLE_ATTR_MAX_SIZE];
                CHAR oldValue[MIB_TABLE_ATTR_MAX_SIZE];
                MIB_GetAttrFromBuf(tableIndex, attrIndex, value, pNewRow, attrSize);
                MIB_GetAttrFromBuf(tableIndex, attrIndex, oldValue, pOldRow, attrSize);
                // Make sure that value is changed
                if (0 != strcmp(value, oldValue))
                {
                    ret = GOS_OK;
                    memcpy(pData, value, attrSize);
                }
                break;
            }
            case MIB_ATTR_TYPE_UINT16:
            {
                UINT16 value, oldValue;
                MIB_GetAttrFromBuf(tableIndex, attrIndex, &value, pNewRow, attrSize);
                MIB_GetAttrFromBuf(tableIndex, attrIndex, &oldValue, pOldRow, attrSize);
                // Make sure that value is changed
                if (value != oldValue)
                {
                    ret = GOS_OK;
                    GOS_SetUINT16((UINT16*)pData, GOS_Htons(value));
                }
                break;
            }
            case MIB_ATTR_TYPE_UINT32:
            {
                UINT32 value, oldValue;
                MIB_GetAttrFromBuf(tableIndex, attrIndex, &value, pNewRow, attrSize);
                MIB_GetAttrFromBuf(tableIndex, attrIndex, &oldValue, pOldRow, attrSize);
                // Make sure that value is changed
                if (value != oldValue)
                {
                    ret = GOS_OK;
                    GOS_SetUINT32((UINT32*)pData, GOS_Htonl(value));
                }
                break;
            }
            case MIB_ATTR_TYPE_UINT64:
            {
                UINT64 value, oldValue;
                MIB_GetAttrFromBuf(tableIndex, attrIndex, &value, pNewRow, attrSize);
                MIB_GetAttrFromBuf(tableIndex, attrIndex, &oldValue, pOldRow, attrSize);
                // Make sure that value is changed
                if ((value.high != oldValue.high) || (value.low != oldValue.low))
                {
                    ret = GOS_OK;
                    value = GOS_Htonll(value);
                    GOS_UINT64ToBuff(value, pData, MIB_GetAttrLen(tableIndex, attrIndex));
                }
                break;
            }

            case MIB_ATTR_TYPE_TABLE:
                break;
            default:
                break;
        }

        if (GOS_OK == ret)
        {
            MIB_ClearAttrSet(&avcAttrSet);
            MIB_SetAttrSet(&avcAttrSet, attrIndex);
            OMCI_AttrsMibSetToOmciSet(&avcAttrSet, &avcOmciSet);
            GOS_SetUINT16((UINT16*)&omciMsg.content[0], GOS_Htons((UINT16)avcOmciSet));
            //OMCI_SendMsg(OMCI_APPL, OMCI_AVC_MSG, OMCI_MSG_PRI_NORMAL, &omciMsg, sizeof(PON_OMCI_MSG_H_T));
            OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"%s: Class %d, Entity 0x%x, attr %d value changed indication",__FUNCTION__,
                        omciMsg.msgId.meClass, omciMsg.msgId.meInstance, avcOmciSet);
        }
    }

    return GOS_OK;
}



static GOS_ERROR_CODE omci_PonResetOntOnu(void)
{
    MIB_TABLE_ONTG_T  ontg;
    MIB_TABLE_ONT2G_T ont2g;


    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Resetting ont equipment MIB");

    MIB_Default(MIB_TABLE_ONTG_INDEX,  &ontg,  sizeof(MIB_TABLE_ONTG_T));
    MIB_Default(MIB_TABLE_ONT2G_INDEX, &ont2g, sizeof(MIB_TABLE_ONT2G_T));

    // Reset ontg
    ontg.EntityID = 0;

    // 1. set vendor id
    ontg.VID[0] = 'R';
    ontg.VID[1] = 'T';
    ontg.VID[2] = 'K';
    ontg.VID[3] = 'G';

    // 2. set version id
    ontg.Version[0] = 0;
    ontg.Version[1] = 0;
    ontg.Version[2] = 0;
    ontg.Version[3] = 1;

    // 3. set serial number
    ontg.SerialNum[0] = 'R';
    ontg.SerialNum[1] = 'T';
    ontg.SerialNum[2] = 'K';
    ontg.SerialNum[3] = 'G';
    ontg.SerialNum[4] = 0x11;
    ontg.SerialNum[5] = 0x11;
    ontg.SerialNum[6] = 0x11;
    ontg.SerialNum[7] = 0x11;

    // 4. set traffic option
    ontg.TraffMgtOpt = (PON_US_TRAFF_MG_OPTION)TXC_ONTG_TRAF_OPTION;
	ontg.OpState = 0;
	
    GOS_ASSERT(GOS_OK == MIB_Set(MIB_TABLE_ONTG_INDEX, &ontg, sizeof(MIB_TABLE_ONTG_T)));


    // Reset ont2g
    ont2g.EntityID = 0;

    // 1. set downstream priority queue number
    ont2g.NumOfPriQ      = TXC_ONT2G_PRIO_Q_NUM;

    // 2. set tcont number
    ont2g.NumOfScheduler = TXC_TCONT_TCONT_NUM;

    // 3. set gem port ctp number
    ont2g.NumOfGemPort   = TXC_ONT2G_GEM_PORT_NUM;
	ont2g.OMCCVer = 0x80;

    // ...

    GOS_ASSERT(GOS_OK == MIB_Set(MIB_TABLE_ONT2G_INDEX, &ont2g, sizeof(MIB_TABLE_ONT2G_T)));


    // Reset soft image

    return GOS_OK;
}


static GOS_ERROR_CODE omci_PonResetOntData(void)
{
    MIB_TABLE_ONTDATA_T ontData;


    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Resetting OntData MIB");

    MIB_Default(MIB_TABLE_ONTDATA_INDEX, &ontData, sizeof(MIB_TABLE_ONTDATA_T));

    ontData.EntityID    = 0;
    ontData.MIBDataSync = 0;

    GOS_ASSERT(GOS_OK == MIB_Set(MIB_TABLE_ONTDATA_INDEX, &ontData, sizeof(MIB_TABLE_ONTDATA_T)));

    return GOS_OK;
}


static GOS_ERROR_CODE omci_PonResetCardholder(void)
{
    UINT32                  slotNum;
    UINT32                  slotId;
    MIB_TABLE_CARDHOLDER_T  cardholder;
    MIB_TABLE_CIRCUITPACK_T circuitPack;


    MIB_Default(MIB_TABLE_CARDHOLDER_INDEX,  &cardholder,  sizeof(MIB_TABLE_CARDHOLDER_T));
    MIB_Default(MIB_TABLE_CIRCUITPACK_INDEX, &circuitPack, sizeof(MIB_TABLE_CIRCUITPACK_T));

    for (slotId = 0; slotId < TXC_CARDHLD_SLOT_NUM; slotId++)
    {
        OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Resetting cardholder[%d] MIB", slotId);


        slotNum = TXC_GET_SLOT_NUM_BY_SLOT_ID(slotId);

        /* 1. Reset cardholder*/
        cardholder.EntityID          = (0x01 << 8) | slotNum; // 0x01 indicate the ONT is a single integrated equipment
        cardholder.ActualType        = TXC_GET_CARDTYPE_BY_SLOT_ID(slotId);
        cardholder.ExpectedType      = TXC_GET_CARDTYPE_BY_SLOT_ID(slotId);
        cardholder.ExpectedPortCount = TXC_GET_PORT_COUNT_BY_SLOT_ID(slotId);
        GOS_ASSERT(GOS_OK == MIB_Set(MIB_TABLE_CARDHOLDER_INDEX, &cardholder, sizeof(MIB_TABLE_CARDHOLDER_T)));

        /* 2. Reset circuit pack*/
        circuitPack.EntityID   = cardholder.EntityID;
        circuitPack.Type       = TXC_GET_CARDTYPE_BY_SLOT_ID(slotId);
        circuitPack.NumOfPorts = TXC_GET_PORT_COUNT_BY_SLOT_ID(slotId);
        GOS_ASSERT(GOS_OK == MIB_Set(MIB_TABLE_CIRCUITPACK_INDEX, &circuitPack, sizeof(MIB_TABLE_CIRCUITPACK_T)));
    }

    return GOS_OK;
}


static GOS_ERROR_CODE omci_PonResetPriQ(UINT32 slot, UINT32 port,UINT16 entityId, UINT32 numOfPriQ)
{
    UINT32           qId;
    UINT32           slotNum;
    MIB_TABLE_PRIQ_T priQ;
	UINT16  		 tcont_entityId;


    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Resetting priQ(slot=%d,tcont=%d,num=%d) MIB", slot, port, numOfPriQ);

    MIB_Default(MIB_TABLE_PRIQ_INDEX, &priQ, sizeof(MIB_TABLE_PRIQ_T));

    slotNum = TXC_GET_SLOT_NUM_BY_SLOT_ID(slot);


    for (qId = 0; qId < numOfPriQ; qId++)
    {
        /*
         *   slot: for upstream, it is for the ANI slot; for downstream, it is for the UNI slot
         *   port: for upstream, it is the local index of T-CONT; for downstream, it is the UNI port id
         */

        if (slot == TXC_CARDHLD_PON_SLOT)
        {
            // 0x8000 indicate Us Priority Queue
            priQ.EntityID = 0x8000 | ( port * numOfPriQ + qId);
			omci_queueMapSet(port,qId,priQ.EntityID,entityId);
			priQ.RelatedPort  = entityId << 16  | TXC_PRIO_Q_PRIOR;			
			priQ.SchedulerPtr = entityId;
			OMCI_wrapper_SetPriQueue(&priQ);
        }
        else if (slot == TXC_CARDHLD_ETH_FE_SLOT || slot == TXC_CARDHLD_ETH_GE_SLOT)
        {
            priQ.EntityID = TXC_PRIO_Q_DS_Q_BASE + port * numOfPriQ + qId;			
			priQ.RelatedPort  = (slotNum << 24) | ((port + 1) << 16) | TXC_PRIO_Q_PRIOR;
			priQ.SchedulerPtr =  0;
        }
        else
        {
            return GOS_FAIL;
        }

        priQ.MaxQSize     = TXC_PRIO_Q_MAX_SIZE;
        GOS_ASSERT(GOS_OK == MIB_Set(MIB_TABLE_PRIQ_INDEX, &priQ, sizeof(MIB_TABLE_PRIQ_T)));
    }

    return GOS_OK;
}


static GOS_ERROR_CODE omci_PonResetTrafficScheduler(UINT32 slot, UINT32 tcontId)
{
    MIB_TABLE_SCHEDULER_T  scheduler;
    UINT32                 slotNum;


    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Resetting traffic scheduler MIB");

    MIB_Default(MIB_TABLE_SCHEDULER_INDEX, &scheduler, sizeof(MIB_TABLE_SCHEDULER_T));

    slotNum = TXC_GET_SLOT_NUM_BY_SLOT_ID(slot);

    scheduler.EntityID     = (slotNum << 8) | tcontId;
    scheduler.TcontPtr     = (slotNum << 8) | tcontId;
    scheduler.SchedulerPtr = 0;
    scheduler.Policy       = TXC_TRAFF_SKED_POLICY;
    GOS_ASSERT(GOS_OK == MIB_Set(MIB_TABLE_SCHEDULER_INDEX, &scheduler, sizeof(MIB_TABLE_SCHEDULER_T)));

    return GOS_OK;
}


static GOS_ERROR_CODE omci_PonResetTcont(UINT32 slot)
{
    UINT32            tcontId;
    UINT32            slotNum;
    MIB_TABLE_TCONT_T tcont;


    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Resetting tcont MIB");

    MIB_Default(MIB_TABLE_TCONT_INDEX, &tcont, sizeof(MIB_TABLE_TCONT_T));

    slotNum = TXC_GET_SLOT_NUM_BY_SLOT_ID(slot);

    for (tcontId = 0; tcontId < TXC_TCONT_TCONT_NUM; tcontId++)
    {
        // Reset Tcont
        tcont.EntityID = (UINT32)((slotNum << 8) | tcontId);
        tcont.Policy   = TXC_TRAFF_SKED_POLICY;

        GOS_ASSERT(GOS_OK == MIB_Set(MIB_TABLE_TCONT_INDEX, &tcont, sizeof(MIB_TABLE_TCONT_T)));

        // Reset Traffic Scheduler
        omci_PonResetTrafficScheduler(slot, tcontId);

        // Reset Upstream Priority Queue
        omci_PonResetPriQ(slot, tcontId,tcont.EntityID, TXC_TCONT_PRIO_Q_PER_TCONT);

    }

    return GOS_OK;
}


static GOS_ERROR_CODE omci_PonResetAnig(UINT32 slot, UINT32 port)
{
    MIB_TABLE_ANIG_T anig;
    UINT32           slotNum;

    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Resetting anig MIB");

    MIB_Default(MIB_TABLE_ANIG_INDEX, &anig, sizeof(MIB_TABLE_ANIG_T));

    slotNum = TXC_GET_SLOT_NUM_BY_SLOT_ID(slot);

    anig.EntityID   = (UINT16)((slotNum << 8) | (port + 1));
    anig.NumOfTcont = TXC_TCONT_TCONT_NUM;

    GOS_ASSERT(GOS_OK == MIB_Set(MIB_TABLE_ANIG_INDEX, &anig, sizeof(MIB_TABLE_ANIG_T)));

    return GOS_OK;
}


static GOS_ERROR_CODE omci_PonResetEthPptp(UINT32 slot, UINT32 port)
{
    MIB_TABLE_ETHUNI_T  ethUni,ret;
    UINT32              slotNum;

    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Resetting Eth PPTP(slot=%d,port=%d) MIB", slot, port);

    MIB_Default(MIB_TABLE_ETHUNI_INDEX, &ethUni, sizeof(MIB_TABLE_ETHUNI_T));

    slotNum = TXC_GET_SLOT_NUM_BY_SLOT_ID(slot);

    ethUni.EntityID     = (UINT16)((slotNum << 8) | (port + 1));
    ethUni.SensedType   = TXC_GET_CARDTYPE_BY_SLOT_ID(slot);
    ethUni.ExpectedType = TXC_GET_CARDTYPE_BY_SLOT_ID(slot);

    GOS_ASSERT(GOS_OK == MIB_Set(MIB_TABLE_ETHUNI_INDEX, &ethUni, sizeof(MIB_TABLE_ETHUNI_T)));

	MIB_GetFirst(MIB_TABLE_ETHUNI_INDEX,(void*) &ret,sizeof(MIB_TABLE_ETHUNI_T));

    // Reset Downstream Priority Queue of Eth
    omci_PonResetPriQ(slot, port,ethUni.EntityID, TXC_PRIO_Q_Q_NUM_PER_PORT);

    return GOS_OK;
}


static GOS_ERROR_CODE omci_PonResetVeIP(UINT32 slot,UINT32 port)
{
	MIB_TABLE_VEIP_T  veip,ret;
    UINT32              slotNum;

    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Resetting VEIP (slot=%d,port=%d) MIB", slot, port);

    MIB_Default(MIB_TABLE_VEIP_INDEX, &veip, sizeof(MIB_TABLE_VEIP_T));

    slotNum = TXC_GET_SLOT_NUM_BY_SLOT_ID(slot);

    veip.EntityId     = (UINT16)((slotNum << 8) | (port + 1));
	veip.OperState    = 1;

    GOS_ASSERT(GOS_OK == MIB_Set(MIB_TABLE_VEIP_INDEX, &veip, sizeof(MIB_TABLE_VEIP_T)));

	MIB_GetFirst(MIB_TABLE_VEIP_INDEX,(void*) &ret,sizeof(MIB_TABLE_VEIP_T));

    // Reset Downstream Priority Queue of Eth
    omci_PonResetPriQ(slot, port,veip.EntityId, TXC_PRIO_Q_Q_NUM_PER_PORT);

    return GOS_OK;
}



static GOS_ERROR_CODE omci_PonResetPptp(void)
{
    UINT32 slotId, portId;


    for (slotId = 0; slotId < TXC_CARDHLD_SLOT_NUM; slotId++)
    {
        if (slotId == TXC_CARDHLD_PON_SLOT)
        {
            omci_PonResetTcont(slotId);
        }

        for (portId = 0; portId < TXC_GET_PORT_COUNT_BY_SLOT_ID(slotId); portId++)
        {
            if (slotId == TXC_CARDHLD_PON_SLOT)
            {
                omci_PonResetAnig(slotId, portId);
            }

            else if (slotId == TXC_CARDHLD_ETH_FE_SLOT)
            {
                omci_PonResetEthPptp(slotId, portId);
            }
            else if (slotId == TXC_CARDHLD_ETH_GE_SLOT)
            {
                //omci_PonResetEthPptp(slotId, 4);
                omci_PonResetEthPptp(slotId, portId);
            }
	    	else if (slotId == TXC_CARDHLD_VEIP_SLOT)
	    	{
	    		omci_PonResetVeIP(slotId,portId);
            }
            else
            {
                return GOS_FAIL;
            }
        }
    }

    return GOS_OK;
}


static GOS_ERROR_CODE omci_PonResetSwImage(void)
{
	MIB_TABLE_SWIMAGE_T swImage;
    UINT32              slotNum;

    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Resetting SWIMAGE MIB");

    MIB_Default(MIB_TABLE_SWIMAGE_INDEX, &swImage, sizeof(MIB_TABLE_SWIMAGE_T));

	swImage.EntityID = (UINT16) 0;
	swImage.Valid = 1;
	sprintf(swImage.Version,"RTL9600V1.00");
	swImage.Committed = 1;
	swImage.Active = 1;

    GOS_ASSERT(GOS_OK == MIB_Set(MIB_TABLE_SWIMAGE_INDEX, &swImage, sizeof(MIB_TABLE_SWIMAGE_T)));


	MIB_Default(MIB_TABLE_SWIMAGE_INDEX, &swImage, sizeof(MIB_TABLE_SWIMAGE_T));
	
	swImage.EntityID = (UINT16) 1;
	swImage.Valid = 1;
	sprintf(swImage.Version,TXC_DEFAULT_SW_VERSION);
	swImage.Committed = 0;
	swImage.Active = 0;

	GOS_ASSERT(GOS_OK == MIB_Set(MIB_TABLE_SWIMAGE_INDEX, &swImage, sizeof(MIB_TABLE_SWIMAGE_T)));



    return GOS_OK;
}


static GOS_ERROR_CODE omci_PonResetLoId(void)
{
	MIB_TABLE_LOIDAUTH_T loid;
    UINT32              slotNum;

    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Resetting LOID MIB");

    MIB_Default(MIB_TABLE_LOIDAUTH_INDEX, &loid, sizeof(MIB_TABLE_LOIDAUTH_T));

	loid.EntityId = (UINT16) 0;
	loid.AuthStatus = 0;

	loid.OperationId[0] = 'C';
	loid.OperationId[1] = 'T';
	loid.OperationId[2] = 'C';
	loid.OperationId[3] = ' ';

	memset(&loid.LoID[0],0,MIB_TABLE_LOID_LEN+1);	
	memset(&loid.Password[0],0,MIB_TABLE_PASSWORD_LEN+1);

    GOS_ASSERT(GOS_OK == MIB_Set(MIB_TABLE_LOIDAUTH_INDEX, &loid, sizeof(MIB_TABLE_LOIDAUTH_T)));

    return GOS_OK;
}

static GOS_ERROR_CODE omci_PonResetOltG(void)
{
    MIB_TABLE_OLTG_T oltG;

    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Resetting OLT-G MIB");

    MIB_Default(MIB_TABLE_OLTG_INDEX, &oltG, sizeof(MIB_TABLE_OLTG_T));
    oltG.EntityId = (UINT16) 0;

    GOS_ASSERT(GOS_OK == MIB_Set(MIB_TABLE_OLTG_INDEX, &oltG, sizeof(MIB_TABLE_OLTG_T)));

    return GOS_OK;
}

static GOS_ERROR_CODE omci_PonResetMib(void)
{
    GOS_ERROR_CODE ret;


    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"PON Starting reset MIB");

    ret = omci_PonResetOntOnu();
    GOS_ASSERT(ret == GOS_OK);

    ret = omci_PonResetOntData();
    GOS_ASSERT(ret == GOS_OK);

    ret = omci_PonResetCardholder();
    GOS_ASSERT(ret == GOS_OK);

    ret = omci_PonResetPptp();
    GOS_ASSERT(ret == GOS_OK);

    ret = omci_PonResetSwImage();
    GOS_ASSERT(ret == GOS_OK);

    ret = omci_PonResetLoId();
    GOS_ASSERT(ret == GOS_OK);

    ret = omci_PonResetOltG();
    GOS_ASSERT(ret == GOS_OK);

	/*Reset all tree and mib*/
	//MIB_ClearAll();
	//MIB_AvlTreeRemoveAll();
	
    return GOS_OK;
}


static GOS_ERROR_CODE omci_InitOntMe(void)
{
    GOS_ERROR_CODE   ret;
    MIB_TABLE_ONTG_T ontg;


    MIB_ClearPublic();

    /* RESET PON MIB */
    ret = omci_PonResetMib();
    GOS_ASSERT(ret == GOS_OK);

    ontg.EntityID = 0;
    if (GOS_OK == MIB_Get(MIB_TABLE_ONTG_INDEX, &ontg, sizeof(MIB_TABLE_ONTG_T)))
    {
        ontg.OntState = PON_ONU_STATE_INITIAL;
        MIB_Set(MIB_TABLE_ONTG_INDEX, &ontg, sizeof(MIB_TABLE_ONTG_T));
    }

    return GOS_OK;
}






/*
*  Define Global APIs for MIB init and self-create 
*/

GOS_ERROR_CODE OMCI_ResetMib(void)
{
    GOS_ERROR_CODE   ret;
    MIB_TABLE_ONTG_T ontg;
    PON_ONU_STATE    ponState;

    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Reseting MIB by OLT");

    // backup ONT PON state
    ontg.EntityID = 0;
    if (GOS_OK == MIB_Get(MIB_TABLE_ONTG_INDEX, &ontg, sizeof(MIB_TABLE_ONTG_T)))
    {
        ponState = ontg.OntState;
    }

    /*clear MIB*/
    MIB_ClearPublic();

    /*reset MIB default values*/
    ret = omci_PonResetMib();
    GOS_ASSERT(ret == GOS_OK);

    /* update ONT PON state*/
    ontg.EntityID = 0;
    if (GOS_OK == MIB_Get(MIB_TABLE_ONTG_INDEX, &ontg, sizeof(MIB_TABLE_ONTG_T)))
    {
        ontg.OntState = ponState;
        MIB_Set(MIB_TABLE_ONTG_INDEX, &ontg, sizeof(MIB_TABLE_ONTG_T));
    }

    return GOS_OK;
}


GOS_ERROR_CODE OMCI_InitMib(void)
{
    /* initialize MIB table for MEs */
    MIB_Init(FALSE, TRUE, NULL, NULL);

    /* initialize MEs created by ONT in the MIB */
    omci_InitOntMe();

    /* register Callback function to MIB */
    if (GOS_OK != MIB_RegisterCallbackToAll(NULL, omci_AvcCallback))
    {
        return GOS_FAIL;
    }

    return GOS_OK;
}

