/*************************************************************************
*
*  COPYRIGHT (C) 2003-2008 Opulan Technologies Corp. ALL RIGHTS RESERVED.  
*
*                       Proprietary and Confidential
*
* 	This software is made available only to customers and prospective
* 	customers of Opulan Technologies Corporation under license and may be
*	used only with Opulan semi-conductor products. 
*
* FILENAME:  vos_msq.c
*
* DESCRIPTION: 
*	
*
* Date Created: Oct 31, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/vos/linux/src/vos_msq.c#1 $
* $Log:$
*
*
**************************************************************************/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#include <vos_msq.h>
#include <vos_hash.h>
#include <vos_io.h>
#include <vos_alloc.h>
#include <vos_libc.h>


#ifndef MSGMNB
#define MSGMNB 16384
#endif

#pragma pack(1)		/* pragma pack definition */

typedef struct OP_MSQID_s
{
    int     seqNo;
    int     msqid;
} OP_MSQID_t;

#pragma pack()

#define MSQ_KEY_STR_LEN 32

#define HASH_TABLE_SIZE 32

#define MSQ_PATH_LEN 64
#define MSQ_PROJ_ID 5

static HASH_TABLE_t *g_pstMsqHashTb;
static char g_acMsqPath[MSQ_PATH_LEN];
static int g_nMsqSeqNo = 0;

static int msqSeqNoGet()
{
    g_nMsqSeqNo++;
    /* range in 1 ~ HASH_TABLE_SIZE */
    if (g_nMsqSeqNo > HASH_TABLE_SIZE)
        g_nMsqSeqNo = 1;
    return g_nMsqSeqNo;
}

static void msqDelete(void *key, void *value)
{
    char acMsqFile[MSQ_PATH_LEN];
    OP_MSQID_t *pMsq = value;

    if (pMsq)
    {
        msgctl(pMsq->msqid, IPC_RMID, NULL);
        vosSprintf(acMsqFile, "%s/msq%d", g_acMsqPath, pMsq->seqNo);
       /* remove msq file */
        vosRemove(acMsqFile);
    }
    vosFree(key);
    vosFree(value);

    return;
}

static int msqRemoveExisted(char *file)
{
    char *pMsqKeyStr = NULL;
	long len = 0;
	int mid;

    if (file)
    {
    	len = vosFileRead(file, &pMsqKeyStr);
		if (len > 0 && pMsqKeyStr)
		{
			mid = vosStrToInt(pMsqKeyStr, NULL);
        	msgctl(mid, IPC_RMID, NULL);
		}
		vosFree(pMsqKeyStr);
       /* remove msq file */
        vosRemove(file);
    }

    return 0;
}

int vosMsqInit(char *pcPath)
{
    VOS_DIR_t * pDir;

    if (NULL == pcPath)
        return ERROR;
    
    vosSnprintf(g_acMsqPath, MSQ_PATH_LEN, pcPath);
    pDir = vosOpenDir(g_acMsqPath);
    if (pDir == NULL)
    {
        vosMakeDir(g_acMsqPath);
    }
    else
    {
        vosCloseDir(pDir);
    }

	vosScanDirFiles(g_acMsqPath, "msq*", msqRemoveExisted);

    g_pstMsqHashTb = vosHashCreate(vosStringHash, 
            vosEqualString,
            msqDelete, 
            HASH_TABLE_SIZE);

    return OK;
}

void vosMsqDestroy(void)
{
    vosHashFree(g_pstMsqHashTb);
    /* remove msq path */
    vosRemoveDir(g_acMsqPath);
}


int vosMsqCreate(int maxMsgLength)
{
    int mid;
    key_t key;
    struct msqid_ds msgqidDs;
    OP_MSQID_t *pMsQID;
    char msqKeyStr[MSQ_KEY_STR_LEN] = {0};
    char acMsqFile[MSQ_PATH_LEN];
    int seqNo = msqSeqNoGet();
    FILE *pFile;

    vosSprintf(acMsqFile, "%s/msq%d", g_acMsqPath, seqNo);
    pFile = vosFOpen(acMsqFile, "w");
    if (pFile == NULL)
    {
        return ERROR;
    }
    vosFClose(pFile);

    key = ftok(acMsqFile, (char)MSQ_PROJ_ID);

    mid = msgget(key, IPC_CREAT|00666);
    if(-1 == mid)
    {
        return -1;
    }

    pMsQID = (OP_MSQID_t *)vosAlloc(sizeof(struct OP_MSQID_s));
    if (pMsQID == NULL)
    {
        return -1;
    }
    pMsQID->seqNo = seqNo;
    pMsQID->msqid = mid;

    vosSnprintf(msqKeyStr,MSQ_KEY_STR_LEN,"0x%08X", (uint32)mid);
    vosHashInsert((void *)vosStrDup(msqKeyStr), (void *)pMsQID, g_pstMsqHashTb);

    if (maxMsgLength > MSGMNB)
    {
        vosMemSet(&msgqidDs, 0, sizeof(struct msqid_ds));
        msgqidDs.msg_perm.uid = getuid();
        msgqidDs.msg_perm.gid = getgid();
        msgqidDs.msg_qbytes = maxMsgLength;
        msgctl(mid, IPC_SET, &msgqidDs); 
    }

	vosFileWrite(acMsqFile, msqKeyStr, vosStrLen(msqKeyStr));

    return mid;
}

int vosMsqSend(int msqid, void *msgBuf, uint32 msgsz)
{
    return msgsnd(msqid, msgBuf, msgsz, MSG_NOERROR);
}

uint32 vosMsqReceive(int msqid, void *msgBuf, uint32 msgsz)
{
    return (uint32)msgrcv(msqid, msgBuf, msgsz, 0, MSG_NOERROR);
}

int vosMsqDelete(int msqid)
{
    char msqKeyStr[MSQ_KEY_STR_LEN] = {0};
    
    vosSnprintf(msqKeyStr,MSQ_KEY_STR_LEN,"0x%08X", msqid);
    vosHashRemove((void *)msqKeyStr, g_pstMsqHashTb);
    
    return 0; 
}

