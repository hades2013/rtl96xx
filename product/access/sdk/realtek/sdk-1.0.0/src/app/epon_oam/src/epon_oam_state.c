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
 * $Date: 2013-05-03 17:35:27 +0800 (Fri, 03 May 2013) $
 *
 * Purpose : 
 *
 * Feature : 
 *
 */

/*
 * Include Files
 */
#include <stdio.h>
#include <string.h> 
#include <malloc.h>
#include <errno.h>
#include <semaphore.h>

#include "epon_oam_config.h"
#include "epon_oam_db.h"
#include "epon_oam_err.h"
#include "epon_oam_rx.h"
#include "epon_oam_dbg.h"
#include "epon_oam_msgq.h"

/* 
 * Symbol Definition 
 */
#define EPON_OAM_FSM_STATE_WAIT_REMOTE          1
#define EPON_OAM_FSM_STATE_WAIT_REMOTE_OK       2
#define EPON_OAM_FSM_STATE_COMPLETE             3

#define EPON_OAM_LOCALPDU_LF_INFO       0
#define EPON_OAM_LOCALPDU_RX_INFO       1
#define EPON_OAM_LOCALPDU_INFO          2
#define EPON_OAM_LOCALPDU_ANY           3

/*  
 * Data Declaration  
 */
static unsigned char oam_fsm_state[EPON_OAM_SUPPORT_LLID_NUM];
static unsigned char local_pdu[EPON_OAM_SUPPORT_LLID_NUM];
static unsigned short oampdu_flag[EPON_OAM_SUPPORT_LLID_NUM];
/* Semaphore for state access */
static sem_t oamStateAccessSem;

/* 
 * Macro Definition 
 */

/*  
 * Function Declaration  
 */
int epon_oam_discovery_init(void)
{
    int ret;

    /* Initial semaphore,
     * The semaphore might be used by multple source, including
     *   - packet rx thread
     *   - State keep thread
     */
    ret = sem_init(&oamStateAccessSem, 0, 1);
    if(0 != ret)
    {
        EPON_OAM_PRINT(EPON_OAM_DBGFLAG_ERROR,
            "[OAM:%s:%d] sem_init failed %d\n", __FILE__, __LINE__, errno);
        return EPON_OAM_ERR_UNKNOWN;
    }

    return EPON_OAM_ERR_OK;
}

int epon_oam_discoveryLlid_init(
    unsigned char llidIdx)
{
    /* Parameter check */
    if(llidIdx >= EPON_OAM_SUPPORT_LLID_NUM)
    {
        return EPON_OAM_ERR_PARAM;
    }

    /* Start FSM wiht EPON_OAM_FSM_STATE_PASSIVE_WAIT state */
    sem_wait(&oamStateAccessSem);
    oam_fsm_state[llidIdx] = EPON_OAM_FSM_STATE_WAIT_REMOTE;
    local_pdu[llidIdx]     = EPON_OAM_LOCALPDU_RX_INFO;
    oampdu_flag[llidIdx]  = EPON_OAM_FLAG_LOCAL_EVAL;
    sem_post(&oamStateAccessSem);
    
    return EPON_OAM_ERR_OK;
}

int epon_oam_discoveryIdleLlid_get(
    unsigned char llidIdx,
    unsigned char *isIdle)
{
    /* Parameter check */
    if(llidIdx >= EPON_OAM_SUPPORT_LLID_NUM)
    {
        return EPON_OAM_ERR_PARAM;
    }

    /* Start FSM wiht EPON_OAM_FSM_STATE_PASSIVE_WAIT state */
    *isIdle = 0;
    sem_wait(&oamStateAccessSem);
    if(EPON_OAM_FSM_STATE_WAIT_REMOTE == oam_fsm_state[llidIdx])
    {
        *isIdle = 1;
    }
    sem_post(&oamStateAccessSem);
    
    return EPON_OAM_ERR_OK;
}

int epon_oam_discovery_proc(
    unsigned char llidIdx,
    oam_oamPdu_t *pOamPdu,
    oam_oamInfo_t *pOamInfo)
{
    oam_oamInfo_t localInfo;

    /* Parameter check */
    if(llidIdx >= EPON_OAM_SUPPORT_LLID_NUM)
    {
        return EPON_OAM_ERR_PARAM;
    }

    if((NULL == pOamPdu) || (NULL == pOamInfo))
    {
        return EPON_OAM_ERR_PARAM; 
    }

    sem_wait(&oamStateAccessSem);
    switch(oam_fsm_state[llidIdx])
    {
    case EPON_OAM_FSM_STATE_WAIT_REMOTE:
        oam_fsm_state[llidIdx] = EPON_OAM_FSM_STATE_WAIT_REMOTE_OK;
        /* Save the remote's "LOCAL" TLV */
        pOamInfo->valid = 1;
        epon_oam_remoteInfo_set(pOamPdu->llidIdx, pOamInfo);
        oampdu_flag[llidIdx] = EPON_OAM_FLAG_LOCAL_STABLE;
        break;
    case EPON_OAM_FSM_STATE_WAIT_REMOTE_OK:
        /* Update the remote's "LOCAL" TLV */
        pOamInfo->valid = 1;
        epon_oam_remoteInfo_set(pOamPdu->llidIdx, pOamInfo);
        if(0 != (pOamPdu->flag & EPON_OAM_FLAG_LOCAL_STABLE))
        {
            oam_fsm_state[llidIdx] = EPON_OAM_FSM_STATE_COMPLETE;
            local_pdu[llidIdx] = EPON_OAM_LOCALPDU_ANY;
        }
        oampdu_flag[llidIdx] = EPON_OAM_FLAG_LOCAL_STABLE;
        /* Send event to state keeper to start timer for OAM keepalive */
        epon_oam_event_send(llidIdx, EPON_OAM_EVENT_DISCOVERY_COMPLETE);
        break;
    case EPON_OAM_FSM_STATE_COMPLETE:
        /* Update the remote's "LOCAL" TLV */
        pOamInfo->valid = 1;
        epon_oam_remoteInfo_set(pOamPdu->llidIdx, pOamInfo);
        if(0 == (pOamPdu->flag & EPON_OAM_FLAG_REMOTE_STABLE))
        {
            oam_fsm_state[llidIdx] = EPON_OAM_FSM_STATE_WAIT_REMOTE_OK;
            local_pdu[llidIdx] = EPON_OAM_LOCALPDU_INFO;
        }
        oampdu_flag[llidIdx] = EPON_OAM_FLAG_LOCAL_STABLE;
        break;
    default:
        /* Not supported states */
        break;
    }
    /* Update remote flag */
    oampdu_flag[llidIdx] |=
        (pOamPdu->flag & EPON_OAM_FLAG_LOCAL_EVAL) ? EPON_OAM_FLAG_REMOTE_EVAL : 0;
    oampdu_flag[llidIdx] |=
        (pOamPdu->flag & EPON_OAM_FLAG_LOCAL_STABLE) ? EPON_OAM_FLAG_REMOTE_STABLE : 0;
    sem_post(&oamStateAccessSem);

    return EPON_OAM_ERR_OK;
}

unsigned short epon_oam_oamPduFlag_get(
    unsigned char llidIdx)
{
    unsigned short retValue;
    
    sem_wait(&oamStateAccessSem);
    retValue = oampdu_flag[llidIdx];
    sem_post(&oamStateAccessSem);

    return retValue;
}

