/* oam_dispatch.c - 
    gcc -o oam_dispatch oam_dispatch.c
*/
 
/* Copyright 2010 Atheros Communications, Inc. */
 
/* 
Modification history 
____________________
1.00, Sep 29, 2010, Gan Zhiheng created
*/ 

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <signal.h> 

#define OAM_MAX_BUF_LEN 2048

#define MSQ_BUF_APPEND_SIZE (sizeof(long))

#define OAM_MSQ_PATH "/tmp/oam_msq"
#define OAM_MSQ_FILE "/tmp/oam_msq/msq"

#define OAM_MSQ_PROJ_ID 'O'

static int g_nOamMsqSeqNo = 0;
static int g_nOamMsgId = -1;

typedef struct MSQ_BUF_s
{
    long    		seqNo; /* message queue mtype */
    unsigned char 	buffer[OAM_MAX_BUF_LEN];
}MSQ_BUF_t;

static int msqSeqNoGet()
{
    g_nOamMsqSeqNo++;
    /* range in 1 ~ 128 */
    if (g_nOamMsqSeqNo > 128)
        g_nOamMsqSeqNo = 1;
    return g_nOamMsqSeqNo;
}

int oamMsqInit(void)
{
    DIR * pDir;
    key_t key;
    FILE *pFile;

    pDir = opendir(OAM_MSQ_PATH);
    if (pDir == NULL)
    {
        mkdir(OAM_MSQ_PATH, 0777);
    }
    else
    {
        closedir(pDir);
    }

    pFile = fopen(OAM_MSQ_FILE, "w");
    if (pFile == NULL)
    {
    	fprintf(stderr, "fopen failed : [%s]\n", strerror(errno)); 
        return -1;
    }
    fclose(pFile);

    key = ftok(OAM_MSQ_FILE, (char)OAM_MSQ_PROJ_ID);

    g_nOamMsgId = msgget(key, IPC_CREAT|00666);
    if(-1 == g_nOamMsgId)
    {
    	fprintf(stderr, "msgget failed : [%s]\n", strerror(errno)); 
        return -1;
    }

    return 0;
}

void oamMsqDestroy(void)
{
    if (-1 != g_nOamMsgId)
    {
        msgctl(g_nOamMsgId, IPC_RMID, NULL);
        /* remove msq file */
        unlink(OAM_MSQ_FILE);
    }
    /* remove msq path */
    rmdir(OAM_MSQ_PATH);
}

int oamMsqSend(void *msgBuf, unsigned int msgsz)
{
	MSQ_BUF_t       stMsqBuf;

	if (-1 == g_nOamMsgId)
    {
    	fprintf(stderr, "oamMsqSend : msq is invalid\n"); 
    	return -1;
    }

    if (NULL == msgBuf)
    {
    	fprintf(stderr, "oamMsqSend : msgBuf is null pointer\n"); 
    	return -1;
    }

    stMsqBuf.seqNo = msqSeqNoGet();
    if (msgsz > OAM_MAX_BUF_LEN)
    {
        memcpy(stMsqBuf.buffer, msgBuf, OAM_MAX_BUF_LEN);
    }
    else
    {
        memcpy(stMsqBuf.buffer, msgBuf, msgsz);
    }
    return msgsnd(g_nOamMsgId, &stMsqBuf, msgsz+MSQ_BUF_APPEND_SIZE, MSG_NOERROR);
}

unsigned int oamMsqReceive(void *msgBuf, unsigned int msgsz)
{
	unsigned int recv;
    MSQ_BUF_t       stMsqBuf;

	if (-1 == g_nOamMsgId)
    {
    	fprintf(stderr, "oamMsqReceive : msq is invalid\n"); 
    	return (unsigned int)-1;
    }

    if (NULL == msgBuf)
    {
    	fprintf(stderr, "oamMsqReceive : msgBuf is null pointer\n"); 
    	return (unsigned int)-1;
    }

    stMsqBuf.seqNo = 0;
    
    if (msgsz > OAM_MAX_BUF_LEN)
    {
        recv = msgrcv(g_nOamMsgId, &stMsqBuf, OAM_MAX_BUF_LEN+MSQ_BUF_APPEND_SIZE, 0, MSG_NOERROR);
    }
    else
    {
        recv = msgrcv(g_nOamMsgId, &stMsqBuf, msgsz+MSQ_BUF_APPEND_SIZE, 0, MSG_NOERROR);
    }
    memcpy(msgBuf, stMsqBuf.buffer, recv);

	return recv;
}

/* test code */
#if 0  /* deleted by Gan Zhiheng - 2010/09/29 */
int main(int argc, char **argv) 
{ 
	int i;
	unsigned char buf[2048] = {0}; 
	
	for (i = 0; i < 6; i++)
		buf[i] = 0xFF;
	for (i = 7; i < 12; i++)
		buf[i] = i;
	buf[12] = 0x88;
	buf[13] = 0x09;
	buf[14] = 0x03;
	for (i = 15; i < 64; i++)
		buf[i] = i;

    oamMsqInit(); 
    fprintf(stdout, "msq id %d\n", g_nOamMsgId); 
    oamMsqSend(buf, 64); 
    //oamMsqDestroy();
    return 0; 
}
#endif /* #if 0 */
