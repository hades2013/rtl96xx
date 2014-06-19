/*************************************************************************
*
*  COPYRIGHT (C) 2003-2008 Opulan Technologies Corp. ALL RIGHTS RESERVED.  
*
*                       Proprietary and Confidential
*
*   This software is made available only to customers and prospective
*   customers of Opulan Technologies Corporation under license and may be
*   used only with Opulan semi-conductor products. 
*
* FILENAME:  vos_io.h
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
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/vos/linux/h/vos_io.h#1 $
* $Log:$
*
*
**************************************************************************/
 
#ifndef __VOS_IO_H_
#define __VOS_IO_H_

#ifdef __cplusplus
extern "C" {
#endif

//#include <vos_types.h>
//#include <vos_compiler.h>
//#include <vos_libc.h>
#ifdef NO_FILE_IO
typedef void* FILE;
#else
#include <stdio.h>
#endif
#include <unistd.h>
#include <fcntl.h> 
#include <stdarg.h>
#include <lw_type.h> 

/*--------------------------Macro definition------------------------- */

#define OP_VOS_NAME_MAX     256

#define OP_STDIN_FILENO     0
#define OP_STDOUT_FILENO    1
#define OP_STDERR_FILENO    2


/*--------------------------type definition------------------------- */

/* directory IO */
struct VOS_DIRENT_s {
    char  dirName[OP_VOS_NAME_MAX];
};

typedef struct VOS_DIR_s {
    int    isNetIO;
    void   *pvDir;        /* NETIO_DIR or DIR pointer */
    struct VOS_DIRENT_s de;
} VOS_DIR_t;
 
/* for console and log */
int vosIOInit(void);
int vosIODestroy(void);
void vosConsoleLock(void);
void vosConsoleUnlock(void);

int vosPrintk(const char *, ...) ;              /* To console and/or file */
//     COMPILER_ATTRIBUTE ((format (printf, 1, 2)));
int vosVPrintk(const char *, va_list ap);

int vosPrintkConsEnable(int enable);     /* Set console output on/off */
int vosPrintkConsIsEnabled(void);
int vosPrintkCons(const char *, ...)  ;    /* To console only */
     //COMPILER_ATTRIBUTE ((format (printf, 1, 2)));
     
int vosPrintkFileEnable(int enable);     /* Set file output on/off */
int vosPrintkFileIsEnabled(void);
int vosPrintkFileOpen(char *filename, int append);
int vosPrintkFileClose(void);                /* Terminate output to file */
int vosPrintkFile(const char *, ...)  ;    /* To log file only */
   //  COMPILER_ATTRIBUTE ((format (printf, 1, 2)));
int vosVPrintkFile(const char *, va_list ap);
char *vosPrintkFileName(void);           /* Return current file name */

/* for directory operations  */
VOS_DIR_t *vosOpenDir(char *dirName);
int vosCloseDir(VOS_DIR_t *pDir);
struct VOS_DIRENT_s *vosReadDir(VOS_DIR_t *pDir);
void vosRewindDir(VOS_DIR_t *pDir);

/* for file operations */
int vosOpen(char *name, int oflag, ...);
int vosClose(int fd);
FILE *vosFOpen(char *name, char *mode);
int vosFClose(FILE *fp);
int vosHomeDirSet(char *dir);
char *vosHomeDirGet(char *buf, size_t size);
void vosHomeDirRemove(void);
char *vosCurrentDirGet(char *buf, size_t size);
int  vosListDir(char *filename, char *flags);
int  vosChangeDir(char *filename);
int  vosRemove(char *filename);
int  vosRename(char *file_old, char *file_new);
int  vosMakeDir(char *path);
int  vosRemoveDir(char *path);
void vosScanDirFiles(char *path, char *name, int(cb)(char *));
char* vosFGetWholeLine(FILE *fp);


/* */
char *vosReadLine(int readFd, int writeFd, char *prompt, char *buf, int bufsize, char *defl);
int vosReadChar(int readFd, int writeFd, const char *prompt);

int vosGetChar(int fd);
char* vosGets(int fd, char *s);
char* vosFGets(char *s, int size, int fd);
long vosSafeRead(int fd, void *buf, UINT32 count);
long vosSafeWrite(int fd, const void *buf, UINT32 count);
int vosPutChar(int fd, char c);
int vosPuts(int fd, const char *s);
int vosFPuts(int fd, const char *s);

int vosPrintf(int fd, const char *fmt, ...);
  //   COMPILER_ATTRIBUTE ((format (printf, 2, 3)));
int vosVPrintf(int fd, const char *fmt, va_list varg);

int  vosNonBlockingOn(int fd);
int  vosNonBlockingOff(int fd);

long vosFileRead( char *filename, char **buffer );
long vosFileWrite( char *filename, char *buffer, size_t nbytes );
long vosFileSize( char *filename);

 
#ifdef __cplusplus
}
#endif
 
#endif /* #ifndef __VOS_IO_H_ */
 
