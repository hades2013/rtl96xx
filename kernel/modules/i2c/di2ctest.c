/*				gfntes.c 
For testing GFN driver
*/

#include <stdio.h>
#include <fcntl.h>
#include "di2c.h"



#define		DEV_ADR_WR  0xa0           // slave address + write_bit 
#define		DEV_ADR_RD  0xa1           // slave address +  read_bit 


int	handle;

static void i2c_start(void)
{
	char data;

	if(ioctl(handle, OPL_I2C_START, &data) < 0)
		printf("Error doing IOCTL i2c_start\n");;
}
static void i2c_stop(void)
{
	char data;

	if(ioctl(handle, OPL_I2C_STOP, &data) < 0)
		printf("Error doing IOCTL i2c_start\n");;
}

static void i2c_ack(char flag)
{
	char data;
	
	data = flag;
	if( ioctl(handle, OPL_I2C_ACK, &data) < 0 )
		printf("Error doing IOCTL i2c_start\n");
}


static unsigned char i2c_rd()
{
	char	data;
	
	if(ioctl(handle, OPL_I2C_READ, &data) < 0)
		printf("Error doing IOCTL i2c_start\n");;
	return data;
}

void i2c_rndwr(unsigned int eep_adr, unsigned char eep_data) 
{ 
	char data;

	i2c_start();    // START command 
        data = DEV_ADR_WR; 
        ioctl( handle, OPL_I2C_SEND, &data );         
        data = eep_adr&0xff;
        ioctl( handle, OPL_I2C_SEND, &data );
	data = eep_data&0xff; 
        ioctl( handle, OPL_I2C_SEND, &data );
	i2c_stop();    // finally , STOP command 
	
} 

void test2(void)
{
	unsigned char data;

/* 	write first */
	i2c_start();
	data = DEV_ADR_WR; 
        ioctl( handle, OPL_I2C_SEND, &data );	
	data = 0x10; 
        ioctl( handle, OPL_I2C_SEND, &data );	
	data = 't'; 
        ioctl( handle, OPL_I2C_SEND, &data );	

	i2c_stop();
/*      read then */
	i2c_start();
	
	data = DEV_ADR_WR; 
        ioctl( handle, OPL_I2C_SEND, &data );	

	data = 0x10; 
        ioctl( handle, OPL_I2C_SEND, &data );	

	i2c_start();

	data = 0xa1; 
        ioctl( handle, OPL_I2C_SEND, &data );	

	data = i2c_rd()            ;  // store the result in "dst"(<-eep_buf)
	i2c_stop()                 ;  // finally , STOP command              	

	printf("\nthe final === result:0x%04x\n",data);			
        
}
void i2c_rndrd(unsigned int eep_adr,unsigned char *dst) 
{  
	char data;
	
	i2c_start()                ;  // START command 
	data = DEV_ADR_WR       ;      
        ioctl( handle, OPL_I2C_SEND, &data );     // write slave address + write_bit 
	data = eep_adr&0xff     ;      
	ioctl( handle, OPL_I2C_SEND, &data );  // write the  low_byte of the adr 
	i2c_start()                ;  // REPEATED START condition 
	data = DEV_ADR_RD       ;       
	ioctl( handle, OPL_I2C_SEND, &data );  // change the direction of the trsf 
	*dst = i2c_rd()            ;  // store the result in "dst"(<-eep_buf)
	i2c_stop()                 ;  // finally , STOP command              
} 


#define		ADDR 0x0040
#define		STRSZ      16          // size of the string 

unsigned char *src_str  = "C_I2C_BB_VFLEDTX" ; 
unsigned int adr_cnt        ;  // address counter  
unsigned char ch_cnt        ;  // character counter inside  strings  
unsigned char dst_str[STRSZ];  // destination string,read from the eep  

OPL_i2c_status  i_status;


int main()
{

	handle = open("/dev/i2c", O_RDWR);
	if(handle <= 0)
	{
		printf("Error opening i2c\n");
		return;
	}

	
	adr_cnt = ADDR; ch_cnt=0;	
	
	while(ch_cnt<STRSZ)              // repeat till the end of the string 
	{
	 	i2c_rndwr(adr_cnt,src_str[ch_cnt]); // write a random byte 
	        ch_cnt++  ;  adr_cnt++   ;
	}	

	ch_cnt=0    ;   adr_cnt=ADDR      ; // init both counters : char & address 
	while(ch_cnt<STRSZ)                 // repeat till the end of the string 
	{i2c_rndrd(adr_cnt,dst_str+ch_cnt) ; // fill the destination string with chrs 
	   ch_cnt++  ;  adr_cnt++  ;  }     

	printf("\n");
	printf("the output string:%s\n", dst_str);
	printf("\n");
#if 0
ioctl(handle,OPL_I2C_STATUS,&i_status);
printf("STATUS:0x%04x\n",i_status.state);
printf("ENABLE:0x%04x\n",i_status.enable);
printf("CONTROL:0x%04x\n",i_status.control);
printf("DEBUG:0x%04x\n",i_status.debug);
#endif

//test2();
//ioctl(handle,OPL_I2C_TEST, &i_status);

	return 0;
}

