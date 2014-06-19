/***************************************************************************
*Ipmux_wz_xmodem.c
*
*                   this file used for updating program with xmodem .
*
*****************************************************************************/



/* #include <String.h> */
#include <common.h>
#include <command.h>
#include <asm/arch/bspchip.h>

extern int Xdown(char *destaddr);

//#define UART0_ADDR  0xB8002000

#define COEFFICIENT 500000   //(500000*(CPU_CLOCK_RATE/REF_CPU_CLOCK_RATE))  /**this coefficient may be not right,we should check it***CPU_CLOCK_RATE)/(203125000*2)*/
//#define RESENDTIME 3

/***error type***/
//#define XDOWNSUCCESSFUL 0
#define SYNCERROR  -1
#define RETRYTIMEOUT -2
#define TRANSMISSIONABORLT  -3
// #define GETPACKETERROR  -4
// #define VERIFYCODEERROR  -5
// #define HEADERROR   -6

/* Line control codes */
#define SOH   0x01 /* start of header for Xmodem protocol  */
#define STX   0x02 /* start of header for 1K xmodem protocol*/
#define ACK   0x06 /* Acknowledge */
#define NAK   0x15 /* Negative acknowledge */
#define CAN   0x18 /* Cancel */
#define EOT   0x04 /* end of text */
#define TIMEOUT 1*COEFFICIENT
#define TIMEOUT_LONG 10*COEFFICIENT
#define XMODEM_DATA_SIZE_SOH 128  /* for Xmodem protocol */
#define XMODEM_DATA_SIZE_STX 1024 /* for 1K xmodem protocol */
#define MAXRETRANS 25
#define TRANSFINISH 1
#define TRANSABORT 2 
#define switch_State(x, y) (x = y)

#define zw_debug 0

#undef  ZWDEBUG
#ifdef  ZWDEBUG
#define ZWDEBUG
#define ZWHEAD 'h'
#define ZWCOUNT 'j'
#define ZW_CODE 'k'
#define ZWCKCODE'l'
#define ZWDATASIZE 16
#define ZWFINISH 'v'
#endif

// unsigned char xbuff[1030];
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
  
/******************************************************************************
*
* xmodemPutc - transmitting a single chacter 
*
* INPUT:  c -- character to be transmitted.
* 
* RETURNS: N/A
*/
void xmodemPutc(const char cValue) 
{
serial_putc (cValue);
}

/******************************************************************************
*
* xmodemGetc - retrieve one character from buffer 
*
* input: wait for "timeout" secones.
*
* RETURNS:  receiving character.
*/
#if 0
int xmodemGetc (volatile unsigned int timeout,char * ptmpchar)
{
    volatile struct uart *pUart = (volatile struct uart*)(UART0_ADDR);
	while((timeout)--)
           if((pUart->lsr & LSR_DR ) != 0)
    	      {
               *ptmpchar  = pUart->w2.urhr;
	         return 1;
              } 
	  return 0;
}
#else
int xmodemGetc(volatile unsigned int timeout,char * ptmpchar) 
{
	/*begin modify by shipeng*/
      //volatile struct uart *pUart = (volatile struct uart*)(UART0_ADDR); 
      while((timeout)--)
      {
	  		#if 0
          	if ((pUart->lsr & BSP_LSR_DR) != 0)
          	{
             	*ptmpchar = pUart->w0.rbr;	   
		  		return 1;
          	}
			#else
			if (serial_tstc())
          	{
             	*ptmpchar = REG8(_UART_RBR);	   
		  		return 1;
          	}
			#endif
      }
	/*end modify by shipeng*/
	  return 0;
  }
#endif	  
static void flushinput(void)
{
    volatile int c;
    while (xmodemGetc(TIMEOUT, &c) > 0)
        continue;
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

/******************************************************************************
*
* Xdown - download a  file  from sender.
*
* input:  dest address
*
* RETURNS:  status.
*/


int Xdown(char *destaddr)
{
    unsigned char *p;
    int bufsz, crc = 0;
    volatile char tmpc;
    int zw=0;
  //  volatile char checksend;
    unsigned char tmpbuff[1030];
    unsigned char trychar = 'C';
    unsigned char packetno = 1;
    int i;
    char *dest = destaddr;
    int retry, retrans = MAXRETRANS;
    int state=0;
    char debugX[100];
    char debugxc=0;
//    char *pflend;
#if 0   /** used for debug link **/
   int in[100];
   unsigned int ii=0, j; 

   while (serial_tstc() == 0)
   {
	   xmodemPutc('C'); 
	   udelay(100000);
   }
   for(j=0;j<64;j++)
   {
 xmodemGetc(TIMEOUT, &tmpc);
     in[j]=tmpc;
   	in[j]=serial_getc();
	if(in[j]==0x01)
		{
		goto rcv_out;
		}
   }
  while(1) 
  	printf("no head\t");
 rcv_out:
in[0]=in[j];
    for(ii=1;ii<64;ii++)
   {
   	in[ii]=serial_getc();
	udelay(100);
   }
   while(1)
   {
	   for(ii=0;ii<64;ii++)
	   {
	   	printf("%d : %08x.\n", ii, in[ii]);
	   }   
	    udelay(10000000);
   }   
#endif   
   for(;;) {
        for( retry = 0; retry < 600; retry++) {  // approx 30 seconds allowed to make connection

            /***** add code for Feed wtd by shipeng 2012-7-11*****/
            //GPIO_WTD_FEED();
			
            if (trychar) {
        
	       xmodemPutc('C'); 
#ifndef 	CONFIG_IPMUX_E2_C   /*zw 922 add*/
	       udelay(10000);
#else
	       udelay(10000);  
#endif  
            	}
            if ((xmodemGetc(TIMEOUT, &tmpc))) {
                                
                
                switch (tmpc) {
                case SOH:
					{
                     	bufsz = XMODEM_DATA_SIZE_SOH;
                	}     
                    goto start_recv;
                case STX:
					{
                    	bufsz = XMODEM_DATA_SIZE_STX;
                	}                    
                    goto start_recv;
                case EOT:           
                    /* recieve finish */     
                    	{
                    		flushinput();
                    		xmodemPutc(ACK);

			 				udelay(10000);
			 				switch_State(state,TRANSFINISH);
							goto result;

               			//     return (dest-destaddr); /* normal end */
                    	}
                case CAN:                   
                    if (xmodemGetc(TIMEOUT, &tmpc)>0)
                    {
    
                        if (tmpc == CAN) 
						{
                            flushinput();
                            xmodemPutc(ACK);

							udelay(10000);
			 				switch_State(state,TRANSABORT);
							goto result;				
                		//            return -1; /* canceled by remote */
                        }
                    }
                    break;
	    		case 0x03:           
                    /* recieve finish */     
                    	{
                    		flushinput();
           			//         xmodemPutc(ACK);

			 				udelay(10000);
			 				switch_State(state,TRANSABORT);			 
			 				udelay(1000000);
							goto result;

               				//     return (dest-destaddr); /* normal end */
                    	}					
                default:
						{
#if	0
                   			debugX[debugxc]=tmpc;
							debugxc++;   
							if(debugxc==0x5)
							{
								while(1)
								{
									for(debugxc=0;debugxc<=5;debugxc++)
									printf("debugX[0]=0x%x\n",debugX[debugxc]);
								}
							}
#endif			
                   			break;
                		}
                }
            }
			else 
			{
			}
        }
      
        if (trychar == 'C') { 
            trychar = NAK; 
            continue;
        }
        flushinput();
        xmodemPutc(CAN);
        xmodemPutc(CAN);
        xmodemPutc(CAN);
        
        return SYNCERROR; /* sync error */

    start_recv:
        
  
       if (trychar == 'C') 
            crc = 1;
        trychar = 0;
        p = tmpbuff;
        *p++ = tmpc;
        for (i = 0;  i < (bufsz+(crc?1:0)+3); ++i) {
 
            if (xmodemGetc(TIMEOUT, &tmpc) == 0) 
            {

                goto reject;
            }
            *p++ = tmpc;
#if 1                                               /*check packt number and complement*/			
	  if(i==1)
	      {
	       if(tmpbuff[1] != packetno )
	         goto reject;
		}
	   if(i==2)
	      {
	       if(tmpbuff[1] != (unsigned char)(~tmpbuff[2]) )
	         goto reject;
		}
#endif	
        }
        while(0) /*zw_debug*/
        {
        	for(zw=0;zw<(bufsz+(crc?1:0)+3);++zw)
			{
			printf("%d: 0x%x\n",zw,tmpbuff[zw]);

			udelay(100000);
        		}
        }
 
  /*      if (tmpbuff[1] == (unsigned char)(~tmpbuff[2]) && 
            (tmpbuff[1] == packetno || tmpbuff[1] == (unsigned char)packetno-1) &&
                check(crc, &tmpbuff[3], bufsz)) {  */
                
       if (check(crc, &tmpbuff[3], bufsz)) {               
           if (tmpbuff[1] == packetno)    {

                  memcpy (dest, &tmpbuff[3], bufsz);
		   dest+=bufsz;
    
                }
         	{
                ++packetno;
                retrans = /* MAXRETRANS*/ 25+1;
            	}
            if (--retrans <= 0) {
                
                flushinput();
                xmodemPutc(CAN);
                xmodemPutc(CAN);
                xmodemPutc(CAN);
              
                return RETRYTIMEOUT; /* too many retry error */
            }
            
          
            xmodemPutc(ACK);
            continue;
        	}
    reject:
        flushinput();
        xmodemPutc(NAK);
    }
   result:
   	switch(state){
		
		case TRANSFINISH:
		   {
                   
	//	     pflend=(unsigned char*)dest;
	//printf("finish!!\n");
	//printf("0x%x\n",dest);
	//printf("0x%x\n",dest-1);
		    while(0x1a==*(dest-1))
		    	{
		    	    
				dest--;
			//	*dest=0xff;
		    	}
		     printf("The total size of the file trasfered is %d kbytes\n",(dest-destaddr)/(1024)); 
		     return dest-destaddr;
		   }
		
		case TRANSABORT:
		   {
		   printf("\nThe sender aborted transmission,  %d kbytes transfered.\n",(dest-destaddr)/(1024));
		   return TRANSMISSIONABORLT;   
		  }
		
		default:
			break;
   		}
}
