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
* FILENAME:  vos_io.c
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
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/vos/linux/src/vos_io.c#1 $
* $Log:$
*
*
**************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

#include <vos_alloc.h>
#include <vos_types.h>
#include <vos_sync.h>
#include <vos_thread.h>
#include <vos_io.h>
#include <vos_macros.h>
#include <vos_match.h>


/*--------------local  variable declaration and definition------------- */

#ifdef NO_FILEIO
static void* g_pvPrintkFile = NULL;         /* Placeholder */
#else
static FILE *g_pvPrintkFile = 0;        /* Non-NULL indicates file opened */
static char *g_pvPrintkFileName = NULL;     /* Non-NULL indicates file selected */
#endif /* !NO_FILEIO */

static int g_nConsoleEnabled = 1;
static VOS_MUTEX_t g_pstConsoleMutex;
static char *g_pcHomeEnv;


/* **********************for file/dir io operations******************** */
int vosHomeDirSet(char *dir)
{
    if (dir[0] != '/') {
        return -1;
    }
    if (dir != NULL) {
      if ((g_pcHomeEnv = vosAlloc(5 + strlen(dir) + 1)) == NULL) {
          return -1;
        }
      sprintf(g_pcHomeEnv, "HOME=%s", dir);
      if (putenv(g_pcHomeEnv)) {
          return -1;
        }
    }
    return 0;
}

char *vosHomeDirGet(char *buf, size_t size)
{
    char  *s;
    if ((s = getenv("HOME")) != NULL) {
        strncpy(buf, s, size);
        buf[size - 2] = 0;
    } else {
        strcpy(buf, "/");
    }
  
    if (buf[strlen(buf) - 1] != '/') {
        strcat(buf, "/");
    }
    return buf;
}

void vosHomeDirRemove(void)
{
    if (g_pcHomeEnv) 
    {
        unsetenv("HOME");
        vosFree(g_pcHomeEnv);
    }
}


char *vosCurrentDirGet(char *buf, size_t size)
{
    if (getcwd(buf, size - 1) == NULL)
        return NULL;
    
    if (buf[strlen(buf) - 1] != '/')
        strcat(buf, "/");
    
    return buf;
}

int vosListDir(char *f, char *flags)
{
    char cmd[256];
    
    sprintf(cmd, "ls %s %s\r\n", flags ? flags : "", f);
    return(system(cmd));
}

int vosChangeDir(char *f)
{
    if (f == NULL) {
        f = getenv("HOME");
        if (f == NULL)
            f = "/";
    }
    return(chdir(f));
}

int vosClose(int fd)
{
    return close(fd);
}

/*******************************************************************************
*
* vosFOpen:	 the function as same as fopen
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	name - name of file to open
*   mode - file mode.
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	NULL or FILE * pointer.
*
* SEE ALSO: 
*/
FILE * vosFOpen(char *file, char *mode)
{
    return fopen(file, mode);
}

/*******************************************************************************
*
* vosFClose:	 Close a file opened with vosFOpen
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	fp - FILE pointer.
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	non-zero on error
*
* SEE ALSO: 
*/
int vosFClose(FILE *fp)
{
    return fclose(fp);
}

int vosRemove(char *f)
{
    return(unlink(f));
}

int vosRename(char *f_old, char *f_new)
{
    return rename(f_old, f_new);
}

int vosMakeDir(char *path)
{
    return mkdir(path, 0777);
}

int vosRemoveDir(char *path)
{
    return rmdir(path);
}

VOS_DIR_t *vosOpenDir(char *dirName)
{
    return (VOS_DIR_t *) opendir(dirName);
}

int vosCloseDir(VOS_DIR_t *pvDir)
{
    return closedir((DIR *) pvDir);
}

struct VOS_DIRENT_s * vosReadDir(VOS_DIR_t *pvDir)
{
    static struct VOS_DIRENT_s dir;
    struct dirent *d;
    
    if ((d = readdir((DIR *) pvDir)) == NULL) {
        return NULL;
    }

    strncpy(dir.dirName, d->d_name, sizeof (dir.dirName));
    dir.dirName[sizeof (dir.dirName) - 1] = 0;
    return &dir;
}

void vosRewindDir(VOS_DIR_t *pvDir)
{
    rewinddir((DIR *) pvDir);
}

/*******************************************************************************
*
* vosScanDirFiles:	 scan the files in the dir recursively
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	path - the path to scan
*   name - the file name to scan
*   cb   - call back function
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
void vosScanDirFiles(char *path, char *name, int(cb)(char *))
{
    struct stat   s;
    DIR           *dir;
    struct dirent *dt;
    char          fullname[256];

    if (path == NULL)
        return;

    if (path[0] == '\0')
        return;
    
    if (stat(path, &s) < 0) {
        vosPrintk("stat path(%s) error\r\n", path);
    }

    if (S_ISDIR(s.st_mode)) {
        if ((dir = opendir(path)) == NULL) {
            vosPrintk("opendir %s error\r\n", path);
            return;
        }
    
        while ((dt = readdir(dir)) != NULL) {
            if (dt->d_name[0] == '.') {
                continue;
            }
            
            fullname[0] = '\0';
            strcpy(fullname, path);
            strcpy(fullname + strlen(fullname), "/");
            strcpy(fullname + strlen(fullname), dt->d_name);
            vosScanDirFiles(fullname, name, cb);
        }

        if (closedir(dir) < 0) {
            vosPrintk("closedir error\r\n");
            return;
        }

    } else {
        /* vosPrintk("scan file: %s, pattern: %s\r\n", path, name); */
        if (cb && (TRUE == vosMatch(path, name))) {
            /* vosPrintk("scan file: %s matched\r\n", path); */
            cb(path);
        }
    }
}


char* vosFGetWholeLine(FILE *fp)
{
    int len = 0;
    int bufsize = 256;
    char *line = (char *)vosAlloc(bufsize);

    ASSERT(line != NULL, "Out of memory!!!");

    while (fgets(line + len, bufsize - len, fp)) {
        len += strlen(line + len);
        if (len == 0)
            continue;

        if (line[len - 1] == '\n')
            break;

        bufsize <<= 1;
        line = vosReAlloc(line, bufsize);
    }

    if (len == 0 || ferror(fp)) {
        vosFree(line);
        return NULL;
    }

    /*if (len + 1 < bufsize)
        line = vosReAlloc(line, len + 1);*/
    line[len - 1] = '\0';

    return line;
}


/* **************************for console and log*********************** */

int vosConsoleInit(void)
{
   vosMutexCreate(&g_pstConsoleMutex);
    return 0;
}

int vosConsoleDestroy(void)
{
    vosMutexDestroy(&g_pstConsoleMutex);
    return 0;
}

void vosConsoleLock(void)
{
    vosMutexTake(&g_pstConsoleMutex);
}

void vosConsoleUnlock(void)
{
    vosMutexGive(&g_pstConsoleMutex);
}


int vosPrintkConsEnable(int enable)
{
    int old = g_nConsoleEnabled;
    g_nConsoleEnabled = enable;
    return old;
}

int vosPrintkConsIsEnabled(void)
{
    return g_nConsoleEnabled;
}

int vosPrintkCons(const char *fmt, ...)
{
    int retv;
    if (g_nConsoleEnabled) {
        va_list varg;
        va_start(varg, fmt);    
        retv = vosVPrintf(STDOUT_FILENO, fmt, varg);
        va_end(varg);
    } else
        retv = 0;
    return retv;
}

char *vosPrintkFileName(void)
{
#ifdef NO_FILEIO
    return "<no filesystem>";
#else
    return(g_pvPrintkFileName);
#endif
}

int vosPrintkFileOpen(char *filename, int append)
{
#ifndef NO_FILEIO
    if (g_pvPrintkFileName) {
        vosPrintkFileClose();
    }
    if ((g_pvPrintkFile = vosFOpen(filename, append ? "a" : "w")) == 0) {
        perror("Error opening file");
        return -1;
    }
    g_pvPrintkFileName = strcpy((char *)vosAlloc(strlen(filename) + 1), filename);
#endif /* NO_FILEIO */
    return 0;
}

int vosPrintkFileClose(void)
{
#ifndef NO_FILEIO
    if (! g_pvPrintkFileName) {
        vosPrintk("File logging is not on\r\n");
        return -1;
    }
    if (g_pvPrintkFile) {
        vosFClose(g_pvPrintkFile);
        g_pvPrintkFile = 0;
    }
    vosFree(g_pvPrintkFileName);
    g_pvPrintkFileName = NULL;
#endif
        return 0;
}

int vosPrintkFileEnable(int enable)
{
    int old_enable = (g_pvPrintkFile != NULL);
#ifndef NO_FILEIO
    if (! old_enable && enable) {
        /* Enable */
        if (! g_pvPrintkFileName) {
            vosPrintk("Logging file not opened, can't enable\r\n");
            return -1;
        }
        if ((g_pvPrintkFile = vosFOpen(g_pvPrintkFileName, "a")) == 0) {
            perror("Error opening file");
            return -1;
        }
    }
    
    if (old_enable && ! enable) {
        if (g_pvPrintkFile) {
            vosFClose(g_pvPrintkFile);
            g_pvPrintkFile = 0;
        }
        /* Note: g_pvPrintkFileName remains valid; output can still be re-enabled */
    }
#endif
    return old_enable;
}

int vosPrintkFileIsEnabled(void)
{
    return (g_pvPrintkFile != NULL);
}

int vosVPrintkFile(const char *fmt, va_list ap)
{
    int     retv = 0;
#ifndef NO_FILEIO
    if (g_pvPrintkFile) {
        retv = vfprintf(g_pvPrintkFile, fmt, ap);
        fflush(g_pvPrintkFile);
    } else
        retv = 0;
#endif /* !NO_FILEIO */
    return(retv);

}

int vosPrintkFile(const char *fmt, ...)
{
    int retv;
    
    if (g_pvPrintkFile) {
        va_list varg;
        va_start(varg, fmt); 
        retv = vosVPrintkFile(fmt, varg);
        va_end(varg);
    } else
        retv = 0;
    return retv;
}

int vosVPrintk(const char *fmt, va_list ap)
{
    int retv = 0;
    /* for logging in files */
    if (g_pvPrintkFile) {
        va_list ap_copy;
        /* Avoid consuming same arg list twice. */
        va_copy(ap_copy, ap);     
        retv = vosVPrintkFile(fmt,ap_copy);
        va_end(ap_copy);
    }
    
    /* for print on console */
    if (g_nConsoleEnabled){
        retv = vprintf(fmt,ap);
    }
    return retv;
}

int vosPrintk(const char *fmt, ...)
{
    int retv;
    va_list varg;
    va_start(varg, fmt);    
    retv = vosVPrintk(fmt, varg);
    va_end(varg);
    return retv;
}

char *
vosReadLine(int readFd, int writeFd, char *prompt, char *buf, int bufsize, char *defl)
{
    char *s, *full_prompt, *cont_prompt;
    char *t;
    
    if (bufsize == 0)
        return NULL;
    
    cont_prompt = prompt[0] ? "? " : "";
    full_prompt = vosAlloc(strlen(prompt) + (defl ? strlen(defl) : 0) + 8);
    strcpy(full_prompt, prompt);
    if (defl)
        sprintf(full_prompt + strlen(full_prompt), "[%s] ", defl);
    t = vosAlloc(bufsize);        
    vosPrintf(writeFd, "%s", full_prompt);
    
    if ((s = vosFGets(t, bufsize, readFd)) == 0) {
        vosFree(t);
    } else {
        s[bufsize - 1] = 0;
        if ((t = strchr(s, '\n')) != 0)
            *t = 0;
        /* Replace garbage characters with spaces */
        for (t = s; *t; t++)
            if (*t < 32 && *t != 7 && *t != 9)
        *t = ' ';
    }
    
    if (s == 0) {                                             /* Handle Control-D */
        buf[0] = 0;
        /* EOF */
        buf = 0;
        goto done;
    } else {
        vosPrintf(writeFd, "%s\r\n", s);
    }
    
    if (s[0] == 0) {
        if (! defl)
            buf[0] = 0;
        else if (buf != defl) {
            strncpy(buf, defl, bufsize);
            buf[bufsize - 1] = 0;
        }
    } else {
        if (strlen(s) >= (size_t)(bufsize - 1))
            vosPrintf(writeFd, "WARNING: input line truncated to %d chars\r\n",
                 bufsize - 1);
        strncpy(buf, s, bufsize);
        buf[bufsize - 1] = 0;
    }
    vosFree(s);
    
    /*
     * If line ends in a backslash, perform a continuation prompt.
     */
    s = buf + strlen(buf) - 1;
    if (*s == '\\' && vosReadLine(readFd, writeFd, cont_prompt, s, bufsize - (s - buf), 0) == 0)
        buf = 0;
 done:
    vosFree(full_prompt);
    return buf;
}

int vosGetChar(int fd)
{
    char c = 0;
    int ret;
    ret = read(fd, &c, 1);
    if (ret < 0) {
        return -1;
    }
    return (int)c;
}

char* vosGets(int fd, char *s)
{
    int c;
    char *p = s;
    if (s) {
        c = vosGetChar(fd);
        while(c != EOF && c != '\n') 
        { 
            *p++ = c;
            c = vosGetChar(fd);
        }
        if (c == '\n') {
            *p++ = '\n';
        }
        *p = '\0';
        return s;
    }

    return NULL;
}

char* vosFGets(char *s, int size, int fd)
{
    int cnt = 0;
    int c;
    char *p = s;
    if (s) {
        c = vosGetChar(fd);
        while(c != EOF && c != '\n') 
        { 
            *p++ = c;
            c = vosGetChar(fd);
            cnt++;
            if (cnt >= size)
                break;
        }
        if (c == '\n' && cnt < size) {
            *p++ = '\n';
        }
        if (cnt < size) {
            *p = '\0';
        }
        return s;
    }

    return NULL;
}

int vosReadChar(int readFd, int writeFd, const char *prompt)
{
    char    buf[64];
    vosPrintf(writeFd, "%s", prompt);
    if (NULL == (vosFGets(buf, 64, readFd))) {
        return(EOF);
    } else {
        return(buf[0]);
    }
}

long vosSafeRead(int fd, void *buf, uint32 count)
{
    long n;

    do {
        n = read(fd, buf, count);
    } while (n < 0 && errno == EINTR);

    return n;
}

long vosSafeWrite(int fd, const void *buf, uint32 count)
{
    long n;
    
    do {
        n = write(fd, buf, count);
    } while (n < 0 && errno == EINTR);
    
    return n;
}

int vosPutChar(int fd, char c)
{
    return write(fd, &c, 1);
}

int vosPuts(int fd, const char *s)
{
    int ret = -1;
    char CR = '\r';
    char LF = '\n';
    if (s) {
        ret = write(fd, s, vosStrLen(s));
        ret += write(fd, &CR, 1);
        ret += write(fd, &LF, 1);
        return ret;
    }

    return ret;
}

int vosFPuts(int fd, const char *s)
{
    if (s) {
        return write(fd, s, vosStrLen(s));
    }

    return -1;
}

/*******************************************************************************
*
* vosVPrintf:	 vprintf
*
* DESCRIPTION:
* 	the function is same as printf.
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
int vosVPrintf(int fd, const char *fmt, va_list varg)
{
    int     retv;
    static char *buf = NULL;
    int     iBufLen = 0;

    /*buf = vosVAsprintf(fmt, &varg);
    retv = vosFPuts(fd, buf);
    vosFree(buf);*/

    vosConsoleLock();

    if (NULL == buf)
        buf = (char *)vosAlloc(4096);

    vsnprintf(buf, 4096, fmt, varg);

    /* 014544 */
    #if 0
    extern char g_szPrintBuf[4096 * 2 + 1];
    extern int g_iIsPrintf;

    if (1 == g_iIsPrintf )
    {
        iBufLen = vosStrLen(g_szPrintBuf);

        if(0 == iBufLen)
        {
            vosStrCpy(g_szPrintBuf, buf);
        }
        else if(iBufLen <= 4096 * 2)
        {
            vosStrCat(g_szPrintBuf, buf);
        }
    }
    #endif
    /* 014544 */
    retv = vosFPuts(fd, buf);

    vosConsoleUnlock();

    return retv;
}

/*******************************************************************************
*
* vosPrintf:	 printf
*
* DESCRIPTION:
* 	the function is same as printf
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*   printf
*/
int vosPrintf(int fd, const char *fmt, ...)
{
    int retv;
    va_list varg;
    va_start(varg, fmt);
    retv = vosVPrintf(fd, fmt, varg);
    va_end(varg);
    return retv;
}

// Turn on nonblocking I/O on a fd
int vosNonBlockingOn(int fd)
{
    return fcntl(fd, F_SETFL, fcntl(fd,F_GETFL,0) | O_NONBLOCK);
}

int vosNonBlockingOff(int fd)
{
    return fcntl(fd, F_SETFL, fcntl(fd,F_GETFL,0) & ~O_NONBLOCK);
}

/*******************************************************************************
*
* vosFileRead:	 File read
*
* DESCRIPTION:
* 	
*
* INPUTS:
*   filename    : full file name
*   buffer      : the caller is the owner, free the buffer must use vosFree
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	The number of items successfully read 
*
* SEE ALSO: 
*/
long vosFileRead( char *filename, char **buffer )
{
    FILE *pFile;
    long CurPos1, CurPos2, ReadBytes, FileSize;
    int ch;
    char *pData=NULL;

    pFile = fopen(filename, "rb");
    if (pFile == NULL)
    {
        *buffer = NULL;
        return 0;
    }
    
    FileSize = 0;    

    while ((ch = fgetc(pFile)) != EOF)
    {
        FileSize++;
    }

    if (FileSize <= 0)
    {
        fclose(pFile);
        *buffer = NULL;
        return 0;
    }
   
    pData = (char*) vosAlloc(FileSize);
    if(pData == NULL)
    {
        /* printf( "vosFileRead: Out of memory.\n"); */
        fclose(pFile);
        *buffer = NULL;
        return 0;
    }

    /* set the file position indicator to the beginning */
    fclose(pFile);
    
    pFile = fopen(filename, "rb");
    if (pFile == NULL)
    {
        *buffer = NULL;
        vosFree(pData);
        return 0;
    }

    ReadBytes = 0;
    while ((ch = fgetc(pFile)) != EOF)
    {
        pData[ReadBytes++] = (char)ch;
    }

    if( ReadBytes != FileSize )
    {
        /* printf( "vosFileRead: cannot read %d bytes for file %s, read(%d)\n", 
                FileSize, filename, ReadBytes); */
        fclose(pFile);
        vosFree(pData);
        *buffer = NULL;
        return 0;
    }
    
    *buffer = pData;
    fclose(pFile);
    return FileSize;
}

/*
 */
/*******************************************************************************
*
* vosFileWrite:	 File write
*
* DESCRIPTION:
* 	
*
* INPUTS:
*   filename : full file name
*   buffer   : buffer containing bytes to be written  
*   size     : number of bytes to write 
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	The number of items successfully written 
*
* SEE ALSO: 
*/
long vosFileWrite( char *filename, char *buffer, size_t nbytes )
{
    FILE *pFile;
    long nWroteBytes;
    
    pFile = fopen(filename, "wb");
    if (pFile == NULL)
    {
        return 0;
    }

    nWroteBytes = fwrite(buffer, 1, nbytes, pFile);

    fclose(pFile);
    
    return nWroteBytes;
}

/*******************************************************************************
*
* vosFileSize:	 Get file size
*
* DESCRIPTION:
* 	
*
* INPUTS:
*   filename    : full file name
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	The number of items successfully read 
*
* SEE ALSO: 
*/
long vosFileSize( char *filename)
{
    FILE *pFile;
    long CurPos1, CurPos2, ReadBytes, FileSize;
    int ch;
    char *pData=NULL;

    pFile = fopen(filename, "rb");
    if (pFile == NULL)
    {
        return 0;
    }
    
    FileSize = 0;    

    while ((ch = fgetc(pFile)) != EOF)
    {
        FileSize++;
    }

    if (FileSize <= 0)
    {
        fclose(pFile);
        return 0;
    }

    fclose(pFile);
    return FileSize;
}


