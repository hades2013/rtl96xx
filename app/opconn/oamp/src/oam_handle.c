/* oam_handle.c - 
    gcc -o oam_handle oam_handle.c
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

int PacketCount  = 0; 

void oamHandleLoop(void) 
{ 
    int n; 
    int t;
    unsigned char buf[2048]; 
   
    while( 1 ) 
    { 
        t=0;
        n = oamMsqReceive(buf, 2048); 
        if (n == -1) 
        { 
            fprintf(stderr, "oamMsqReceive error : [%s]\n", strerror(errno)); 
            break; 
        }
    	while((n-t)>=16)
        {
            int i;
            printf("[");
            for(i=0;i<16;i++)
                printf("%02x ",buf[t+i]);
            printf("]\t[");
            for(i=0;i<16;i++)
            {
                char ch=buf[t+i];
                if(isalnum(ch))
                    printf("%c",ch);
                else
                    printf(".");
            }
            printf("]\n");
            t+=16;
        }

        if(n>t)
        {
            int i=t;
            printf("[");
            while(i<n)
                printf("%02x ",buf[i++]);
            printf("]");
            i=n-t;
            i=16-i;
            while(i--)
                printf("   ");
            printf("\t[");
            i=t;
            while(i<n)
            {
                char ch=buf[i++];
                if(isalnum(ch))
                    printf("%c",ch);
                else
                    printf(".");
            }
            printf("]\n");
        }
        printf("\n\n");
        PacketCount++; 
    }

    oamMsqDestroy(); 
} 

void Shutdown(int signum) 
{ 
    oamMsqDestroy();
    fprintf(stdout, "Received %d packets\n", PacketCount); 
    exit(0); 
} 
   
void AppInit(void) 
{ 
    oamMsqInit(); 
    fprintf(stdout, "msq id %d\n", g_nOamMsgId); 
    signal(SIGINT, Shutdown); 
    signal(SIGTERM, Shutdown); 
} 

int main(int argc, char **argv) 
{ 
    AppInit(); 
    oamHandleLoop(); 
    return 0; 
}

