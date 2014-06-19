/*
    Copyright 2001, 2002 Georges Menie (www.menie.org)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> 
#include <linux/major.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/syscalls.h>
#include <asm/unistd.h>
#include <asm/uaccess.h>
#include <linux/sched.h>
#include "malta_int.h"

#if 0
#define debug(x...) printk(x)
#else
#define debug(x...)
#endif

#if 1
#define print_err(x...) printk(x)
#else
#define print_err(x...)
#endif

#define MY_FILE "/tmp/opconn.img.bak"
#define MY_FILE_UP "/dev/mtdk0"
#define CFG_FILE "/tmp/cfg.tar.gz"
#define CFG_FILE_UP "/tmp/cfg.tar.gz"

#define SOH  0x01
#define STX  0x02
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18
#define CTRLZ 0x1A

#define DLY_1S 10000000

#define MAXRETRANS 25

/* CRC16 implementation acording to CCITT standards */

unsigned char xbuff[1030]; /* 1024 for XModem 1k + 3 head chars + 2 crc + nul */
static const unsigned short crc16tab[256]= {
    0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
    0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
    0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
    0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
    0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
    0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
    0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
    0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
    0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
    0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
    0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
    0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
    0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
    0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
    0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
    0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
    0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
    0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
    0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
    0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
    0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
    0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
    0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
    0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
    0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
    0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
    0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
    0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
    0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
    0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
    0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
    0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
};
  
unsigned char _inbyte(volatile unsigned int timeout, unsigned int *ch) 
{
    //unsigned char ch;
    while (timeout--) {
        if (uart0GetChar(ch))
            return 1;
    }

    return 0;
}

void xmemcpy (unsigned char *dst, unsigned char *src, int count)
{
    while (count--)
        *dst++ = *src++;
}

void xmemset (unsigned char *dst, unsigned char ch, int count)
{
    while (count--)
        *dst++ = ch;
}

unsigned short crc16_ccitt(const void *buf, int len)
{
    int counter;
    unsigned short crc = 0;
    for( counter = 0; counter < len; counter++)
        crc = (crc<<8) ^ crc16tab[((crc>>8) ^ *(char *)buf++)&0x00FF];
    return crc;
}

static int check(int crc, const unsigned char *buf, int sz)
{
    if (crc) {
        unsigned short crc = crc16_ccitt(buf, sz);
        unsigned short tcrc = (buf[sz]<<8)+buf[sz+1];
        if (crc == tcrc)
            return 1;
    }
    else {
        int i;
        unsigned char cks = 0;
        for (i = 0; i < sz; ++i) {
            cks += buf[i];
        }
        if (cks == buf[sz])
        return 1;
    }

    return 0;
}

static void flushinput(void)
{
    volatile int c;
    while (_inbyte(DLY_1S, &c) > 0)
        continue;
}
#if 0
#define myprintk(x...) printk(x)
#else
#define myprintk(x...)
#endif
int xmodemReceive(unsigned char *dest, int destsz)
{
    unsigned char *p;
    int bufsz, crc = 0;
    unsigned char trychar = 'C';
    unsigned char packetno = 1;
    int i, c, len = 0;
    int retry, retrans = MAXRETRANS;

    printk("start connect.\n");
    for(;;) {
        
        for( retry = 0; retry < 30; retry++) {  // approx 30 seconds allowed to make connection
            
            //schedule();
            myprintk("try connect for %d time.\n", retry);
            if (trychar) {
                myprintk("2 ");
                uart0SendChar(trychar);
            }
            if ((_inbyte(DLY_1S, &c))) {
                                
                myprintk("3 ");
                switch (c) {
                case SOH:
                    bufsz = 128;
                    
                    myprintk("4 ");
                    goto start_recv;
                case STX:
                    bufsz = 1024;
                    
                    myprintk("5 ");
                    goto start_recv;
                case EOT:           
                    /* recieve finish */        
                    flushinput();
                    uart0SendChar(ACK);
                    printk("receive findished.\n");
                    return len; /* normal end */
                case CAN:
                    
                    if (_inbyte(DLY_1S, &c)>0)
                    {
                    
                        if (c == CAN) {
                            flushinput();
                            uart0SendChar(ACK);
                            printk("canceled by remote.\n");
                            return -1; /* canceled by remote */
                        }

                    }
                    break;
                default:
                    printk("invalid char 0x%x received.\n", c);
                    //return -4; /* invalid char received */
                    break;
                }
            }else {
                myprintk("Not recieve one char for 1s.\n");
            }
        }
        //printk("6 ");
        if (trychar == 'C') { 
            trychar = NAK; 
            continue;
        }
        flushinput();
        uart0SendChar(CAN);
        uart0SendChar(CAN);
        uart0SendChar(CAN);
        printk("sync error.\n");
        return -2; /* sync error */

    start_recv:
        
        myprintk("module start recv.\n");
        if (trychar == 'C') 
            crc = 1;
        trychar = 0;
        p = xbuff;
        *p++ = c;
        for (i = 0;  i < (bufsz+(crc?1:0)+3); ++i) {
            /* xwang : never <0, but maybe = 0 */
            if (_inbyte(DLY_1S, &c) == 0) 
            {
                printk("cant't get char, i = %d.\n", i);
                goto reject;
            }
            *p++ = c;
        }
        
        myprintk("8 ");
        if (xbuff[1] == (unsigned char)(~xbuff[2]) && 
            (xbuff[1] == packetno) &&
                check(crc, &xbuff[3], bufsz)) {
            if (xbuff[1] == packetno)    {
                int count = destsz - len;
                if (count > bufsz) 
                    count = bufsz;
                if (count > 0) {
                    xmemcpy (&dest[len], &xbuff[3], count);
                    len += count;
                }
                ++packetno;
                retrans = MAXRETRANS+1;
            }
            if (--retrans <= 0) {
                
                myprintk("9 ");
                flushinput();
                uart0SendChar(CAN);
                uart0SendChar(CAN);
                uart0SendChar(CAN);
                printk("too many retry.\n");
                return -3; /* too many retry error */
            }
            
            myprintk("10 ");
            uart0SendChar(ACK);
            continue;
        }
    reject:
        myprintk("module reject\n");
        flushinput();
        uart0SendChar(NAK);
    }
}
#define mprintk(x) 
int xmodemTransmit(unsigned char *src, int srcsz)
{
    unsigned char xbuff[1030]; /* 1024 for XModem 1k + 3 head chars + 2 crc + nul */
    int bufsz, crc = -1;
    unsigned char packetno = 1;
    int i, c, len = 0;
    int retry;
    mprintk("1 ");
    for(;;) {
        
        for( retry = 0; retry < 30; ++retry) {
            mprintk("2 ");
            if (_inbyte((DLY_1S)<<1, &c) > 0) {
                switch (c) {
                case 'C':
                    mprintk("3 ");
                    crc = 1;
                    goto start_trans;
                case NAK:
                    crc = 0;
                    goto start_trans;
                case CAN:
                    if (_inbyte(DLY_1S, &c) > 0)
                    {
                        if (c == CAN) {
                            uart0SendChar(ACK);
                            flushinput();
                            return -1; /* canceled by remote */
                        }
                    }
                    break;
                default:
                    break;
                }
            }
        }
        uart0SendChar(CAN);
        uart0SendChar(CAN);
        uart0SendChar(CAN);
        flushinput();
        return -2; /* no sync */

        for(;;) {
            
        start_trans:
            mprintk("4 ");
            xbuff[0] = SOH; bufsz = 128;
            xbuff[1] = packetno;
            xbuff[2] = ~packetno;
            c = srcsz - len;
            if (c > bufsz) c = bufsz;
            if (c >= 0) {
                xmemset (&xbuff[3], 0, bufsz);
                if (c == 0) {
                    xbuff[3] = CTRLZ;
                }
                else {
                    xmemcpy (&xbuff[3], &src[len], c);
                    if (c < bufsz) 
                        xbuff[3+c] = CTRLZ;
                }
                if (crc) {
                    unsigned short ccrc = crc16_ccitt(&xbuff[3], bufsz);
                    xbuff[bufsz+3] = (ccrc>>8) & 0xFF;
                    xbuff[bufsz+4] = ccrc & 0xFF;
                }
                else {
                    unsigned char ccks = 0;
                    for (i = 3; i < bufsz+3; ++i) {
                        ccks += xbuff[i];
                    }
                    xbuff[bufsz+3] = ccks;
                }
                
                for (retry = 0; retry < 5; ++retry)
                {
                    mprintk("5 ");
                    schedule();
                    for (i = 0; i < bufsz+4+(crc?1:0); ++i) {
                        
                        uart0SendChar(xbuff[i]);
                    }
                    mprintk("6 ");
                    if ( _inbyte(DLY_1S, &c) > 0 ) {
                        switch (c) {
                        case ACK:
                            mprintk("7 ");
                            ++packetno;
                            len += bufsz;
                            goto start_trans;
                        case CAN:
                            mprintk("8 ");
                            if (_inbyte(DLY_1S, &c) >0)
                            {
                                if (c == CAN) {
                                    uart0SendChar(ACK);
                                    flushinput();
                                    return -1; /* canceled by remote */
                                }
                            }
                            break;
                        case NAK:
                        default:
                            break;
                        }
                    }
                }
                uart0SendChar(CAN);
                uart0SendChar(CAN);
                uart0SendChar(CAN);
                flushinput();
                return -4; /* xmit error */
            }
            else {
                mprintk("10 ");
                for (retry = 0; retry < 10; ++retry) {
                    mprintk("1 ");
                    uart0SendChar(EOT);
                    if (_inbyte((DLY_1S)<<1, &c)>0) {
                        if (c == ACK) 
                            break;
                    }
                    
                }
                flushinput();
                return (c == ACK)?len:-5;
            }
        }
    }
}

void reverse(char s[])
{
   int c,i,j;
   for(i = 0,j = strlen(s) - 1; i < j; i++,j--){
       c = s[i];
       s[i] = s[j];
       s[j] = c;
   }
}

void itoa(int n,char s[])
{
   int i;
   i = 0;
   do{
      s[i++] = n%10 + '0';
   }while((n /= 10) > 0);
   
   reverse(s);
}

uint32_t *crc32_filltable(int endian)
{

    uint32_t *crc_table = vmalloc(256 * sizeof(uint32_t));
    uint32_t polynomial = endian ? 0x04c11db7 : 0xedb88320;
    uint32_t c;
    int i, j;

    for (i = 0; i < 256; i++) {
        c = endian ? (i << 24) : i;
        for (j = 8; j; j--) {
            if (endian)
                c = (c&0x80000000) ? ((c << 1) ^ polynomial) : (c << 1);
            else
                c = (c&1) ? ((c >> 1) ^ polynomial) : (c >> 1);
        }
        *crc_table++ = c;
    }

    return crc_table - 256;
}

int check_crc(const char *s,int size,unsigned int value)
{
    uint32_t *crc32_table = crc32_filltable(1);
    
    uint32_t crc;
    long length;
    int bytes_read;
    char *cp;

    crc = 0;
    length = 0;
    bytes_read=size;
    
    cp = s;
    length += bytes_read;
    while (bytes_read--)
        crc = (crc << 8) ^ crc32_table[((crc >> 24) ^ (*cp++)) & 0xffL];

    for (; length; length >>= 8)
        crc = (crc << 8) ^ crc32_table[((crc >> 24) ^ length) & 0xffL];
    crc ^= 0xffffffffL;

    debug("%u %d \n", crc, size);

    if(crc == value){
        debug("crc is correct\n");
        vfree(crc32_table);
        return 0;
    }
    debug("crc is wrong\n");
    vfree(crc32_table);
    return -1;
}



int recv_test(void)
{
    int st;
    char up[256];

    printk ("Send data using the xmodem protocol from your terminal emulator now...\n");
    /* the following should be changed for your environment:
       0x30000 is the download address,
       65536 is the maximum size to be written at this address
     */
    st = xmodemReceive((char *)up, 128);
    if (st < 0) {
        printk ("Xmodem receive error: status: %d\n", st);
    }
    else  {
        printk ("Xmodem successfully received %d bytes\n", st);
    }
        
    return 0;
}
#define MAX_SIZE 0x1D0000
int recv_firmware(void)
{
    int ret;
    int st,i;
    int f_size;
    unsigned int crc;
    unsigned char size[9] = {0,0,0,0,0,0,0,0,0};
    unsigned char crc_a[9] = {0,0,0,0,0,0,0,0,0};
    char *c;
    mm_segment_t old_fs;
    struct file *file = NULL;

    char *up = vmalloc(MAX_SIZE);
    if(up == NULL){
        debug("up malloc failed!\n");
        return -1;
    }
    debug ("Send data using the xmodem protocol from your terminal emulator now...\n");
    /* the following should be changed for your environment:
       0x30000 is the download address,
       65536 is the maximum size to be written at this address
     */
    st = xmodemReceive((char *)up, MAX_SIZE);
    if (st < 0) {
        printk ("Xmodem receive error: status: %d\n", st);
        vfree(up);
        return -1;
    }
    else  {
        printk ("Xmodem successfully received %d bytes\n", st);
    }

#if 0  /* deleted by Gan Zhiheng - 2009/11/11 */
/*head*/    
    if(up[0]=='O'&&up[1]=='P'&&up[2]=='L'){
        debug("head is OPL\n");
    }
    else{
        debug("head is not OPL! exit\n");
        vfree(up);
        return -1;
    }

	/* file size field */   
    memcpy(size, up + 3, 8);
    f_size = simple_strtoul(size,NULL,16);

	/*  
    f_size = 0;
    for(i=0;i<4;i++,c++){
        size[i]=up[4+i];
        printk("size:%x ",size[i]);
        f_size += size[i] << 8*(3-i);
    }
	*/
    printk("    file size : %d\n",f_size);

    /*crc*/
    memcpy(crc_a, up+11, 8);
    crc = simple_strtoul(crc_a,NULL,16);

    printk("    crc        : 0x%x\n",crc);
    ret = check_crc(up+19, f_size, crc);
    if (ret != 0) {
        printk("CRC check failed.\n");
        return -1;
    }
#else
    f_size = st;
#endif /* #if 0 */
    
    /*begin write*/
    printk("copy to flash ... \n");
    if(file == NULL)
        file = filp_open(MY_FILE, O_RDWR | O_CREAT, 0755);
    if (IS_ERR(file)) {
        debug("error occured while opening file , exiting...\n" );
        vfree(up);
        return -1;
    }
    old_fs = get_fs();
    set_fs(KERNEL_DS);
    file->f_op->write(file,up, f_size+19, &file->f_pos);
    set_fs(old_fs);

    debug("close and free\n");
    if(file != NULL)
        filp_close(file, NULL);

    vfree(up);
    
    return 0;
}

int recv_config(void)
{
    int st,i;
    int f_size;
    char *c;
    mm_segment_t old_fs;
    struct file *file = NULL;

    char *up = vmalloc(MAX_SIZE);
    debug ("Send data using the xmodem protocol from your terminal emulator now...\n");
    /* the following should be changed for your environment:
       0x30000 is the download address,
       65536 is the maximum size to be written at this address
     */
    st = xmodemReceive((char *)up, MAX_SIZE);
    if (st < 0) {
        printk ("Xmodem receive error: status: %d\n", st);
        return -1;
    }
    else  {
        printk ("Xmodem successfully received %d bytes\n", st);
    }

    while(up[st-1] == 0x1a){
        st--;
    }
    
    f_size = st;
    debug("\nf_size:%d\n",f_size);

    /*begin write*/
    if(file == NULL)
        file = filp_open(CFG_FILE, O_RDWR | O_CREAT, 0755);
    if (IS_ERR(file)) {
        debug("error occured while opening file , exiting...\n" );
        return -1;
    }
    old_fs = get_fs();
    set_fs(KERNEL_DS);
    file->f_op->write(file,up, f_size, &file->f_pos);
    set_fs(old_fs);

    if(file != NULL)
        filp_close(file, NULL);

    vfree(up);
    
    return 0;
}


int send_firmware()
{
    int st,i;
    unsigned int f_size = 0;
    char * bfs;
    
    mm_segment_t old_fs;
    struct file * file = NULL;
    file = filp_open(MY_FILE_UP,O_RDONLY,0);

    old_fs = get_fs();
    set_fs(KERNEL_DS);
    f_size = file->f_op->llseek(file, 0, SEEK_END);
    file->f_op->llseek(file, 0, SEEK_SET);
    set_fs(old_fs);

    debug("f_size:%d\n",f_size);
    
    bfs = (char *)vmalloc(f_size);
    
    old_fs = get_fs();
    set_fs(KERNEL_DS);
    file->f_op->read(file, bfs, f_size,&file->f_pos);
    set_fs(old_fs);

    //printk("bfs:%s\n",bfs);

    debug ("Prepare your terminal emulator to receive data now...\n");
    

    st = xmodemTransmit((char *)bfs, f_size);
    if (st < 0) {
        debug ("Xmodem transmit error: status: %d\n", st);
        vfree(bfs);
        return -1;
    }
    else  {
        debug ("Xmodem successfully transmitted %d bytes\n", st);
    }

    vfree(bfs);
    return 0;
}

int send_config(void)
{
    int st,i,err;
    unsigned int f_size = 0;
    char * bfs;
    
    mm_segment_t old_fs;
    struct file * file = NULL;
    file = filp_open(CFG_FILE_UP,O_RDONLY,0);

    err = PTR_ERR(file);
    if (IS_ERR(file))
    {
        printk("open cfg file error\n");
        return -1;
    }	
    old_fs = get_fs();
    set_fs(KERNEL_DS);
    f_size = file->f_op->llseek(file, 0, SEEK_END);
    file->f_op->llseek(file, 0, SEEK_SET);
    set_fs(old_fs);

    debug("f_size:%d\n",f_size);
    
    bfs = (char *)vmalloc(f_size);
    
    old_fs = get_fs();
    set_fs(KERNEL_DS);
    file->f_op->read(file, bfs, f_size,&file->f_pos);
    set_fs(old_fs);

    //printk("bfs:%s\n",bfs);

    debug ("Prepare your terminal emulator to receive data now...\n");
    

    st = xmodemTransmit((char *)bfs, f_size);
    if (st < 0) {
        debug ("Xmodem transmit error: status: %d\n", st);
        vfree(bfs);
        return -1;
    }
    else  {
        debug ("Xmodem successfully transmitted %d bytes\n", st);
    }

    vfree(bfs);
    return 0;
}

int send_test(void)
{
    int st;
    static char bfs[24000];
    printk ("Prepare your terminal emulator to receive data now...\n");
    
     bfs[0] = 'a';
     bfs[1] = 'b';
     bfs[2] = 'c';
    st = xmodemTransmit((char *)bfs, 12000);
    if (st < 0) {
        printk ("Xmodem transmit error: status: %d\n", st);
        return -1;
    }
    else  {
        printk ("Xmodem successfully transmitted %d bytes\n", st);
    }

    return 0;
}
