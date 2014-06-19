#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/stddef.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <linux/poll.h>   
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/highmem.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <asm/kmap_types.h>
#include <asm/page.h>
#include <asm/io.h>
#include <linux/slab.h>
#include "../include/opl_hw_ops.h"
#include "di2c.h"
#define NUM 200

#if 0
#define OPL_I2C_DEBUG  1
#endif

#ifdef OPL_I2C_DEBUG
#define DPRINTK(args...) printk(args)
#else
#define DPRINTK(args...)
#endif

#define PRINTK(args...)

#define OPL_REGBASE		0xbf000000
int tempdata;

#define DEFINE_I2C_REG(reg, off) \
static inline unsigned int read_##reg(void) \
		{\
		tempdata=*(volatile unsigned int*)(OPL_REGBASE + 4*off);\
		PRINTK("rd  addr = 0x%04x  data = 0x%04x\n",off,tempdata);\
            	return tempdata; } \
static inline void write_##reg(int v) \
            {*(volatile unsigned int*)(OPL_REGBASE + 4*off) = v;\
		PRINTK("wr  addr = 0x%04x  data = 0x%04x\n",off,v);\
			}

DEFINE_I2C_REG(STATUS,  	0x0800)
DEFINE_I2C_REG(TIMEOUT,  	0x0800)
DEFINE_I2C_REG(DATA,  		0x0801)
DEFINE_I2C_REG(ADRRESS,  	0x0802)
DEFINE_I2C_REG(CONTROL,  	0x0803)
DEFINE_I2C_REG(ENABLE,  	0x0804)
DEFINE_I2C_REG(DEBUG,  		0x0805)



#define	CONTROL_ACK		(1<<7)
#define CONTROL_ENABLE		(1<<6 | 0x0 | 0x80)

#define CONTROL_START		1<<5
#define CONTROL_STOP		1<<4
#define ENABLE_MASTER		1<<0
#define ENABLE_RD		1<<3
#define ENABLE_WR		1<<2
#define DEBUG_TRANS		1<<1
#define TO_DISABLE 0
static void opl_i2c_hwinit(void)
{

	write_ENABLE( ENABLE_MASTER );		/* set Master mode */
	write_CONTROL( CONTROL_ENABLE );	/* set i2c module enable */
	
	write_TIMEOUT(TO_DISABLE);
}

static short i2c_poll_status(unsigned long bit)
{
	int loop_cntr = 1000;

	if( DEBUG_TRANS == bit )
	{
		do {
			udelay(10);
		}while( (read_DEBUG()&bit) && (--loop_cntr>0) );
	} 
	else
	{
		do {
			udelay(10);
		}while( (read_ENABLE()&bit) && (--loop_cntr>0) );
	}
	
	return (loop_cntr > 0);
}

static unsigned char xfer_read(void)
{
	unsigned int mword;
	/* Read data */
	mword = read_ENABLE( );	
	mword|=ENABLE_RD;
	write_ENABLE( mword );
	if( !i2c_poll_status( ENABLE_RD ) ){
			DPRINTK("RXRDY timeout\n");
			return -1;
	}
	return read_DATA();
}

static void xfer_write(unsigned char data)
{
	unsigned int mword;

	mword = read_ENABLE( );	
	mword|=ENABLE_WR;
	write_DATA( data );
	write_ENABLE( mword );

	if( !i2c_poll_status( ENABLE_WR ) ){
			DPRINTK("RXRDY timeout\n");
			return;
	}
	return;
}

void test1(void)
{
	unsigned char	data;
	unsigned int	mword;

  mword = read_CONTROL();
  mword |= CONTROL_START;
	write_CONTROL(mword);


	data = 0xa0;
	xfer_write( data );

	data = 0x10;
	xfer_write( data );

	data = 'h';
	xfer_write( data );
	
  mword = read_CONTROL();
  mword |= CONTROL_STOP;
	write_CONTROL(mword);

	mdelay(5);

  /*  		read after write		*/
  mword = read_CONTROL();
  mword |= CONTROL_START;
	write_CONTROL(mword);


	data = 0xa0;
	xfer_write(data);

	data = 0x10;
	xfer_write(data);
  
  mword = read_CONTROL();
  mword |= CONTROL_START;
  
	write_CONTROL(mword);

	data = 0xa1;
	xfer_write(data);


	data = xfer_read();

  mword = read_CONTROL();
  mword |= CONTROL_STOP;
	write_CONTROL(mword);
	
	printk("\nresult : 0x%01x\n", data);			
}
	
void print_sts(void)
{
		int val, val1;
		val = read_STATUS();
		val1 = read_CONTROL();
		
		#ifdef OPL_I2C_DEBUG
		printk(" >> stats 0x%x, int %x.\n", val, val1 & 0x8);
		#endif
}

	
int opli2c_ioctl(struct inode *inode, struct file *filp,
          unsigned int cmd, unsigned long arg)
{
	unsigned char	data[100];
	unsigned int	mword;
	unsigned int    value;
	OPL_i2c_status  i_status;
	
	
	switch(cmd) {

		case	OPL_I2C_READ:
			data[0] = xfer_read();
			if( copy_to_user( (unsigned char*)arg,data, 1 ) )
				return -EFAULT;
	              	break;
	              	
		case	OPL_I2C_SEND:
	       		if(copy_from_user(data,(unsigned char*)arg,1)) return -EFAULT;
			xfer_write( data[0] );
	              	break;
		
	       	case	OPL_I2C_STOP:
	       		mword = read_CONTROL();
	       		mword |= CONTROL_STOP;
			write_CONTROL(mword);
			mdelay(5);
			break;

	       	case	OPL_I2C_START:
	       		mword = read_CONTROL();
	       		mword |= CONTROL_START;
			write_CONTROL(mword);
			break;
	        
	       	case	OPL_I2C_ACK:
	       		if(copy_from_user(data,(unsigned char*)arg,1)) return -EFAULT;
	       		mword = read_CONTROL();
	       		value = CONTROL_ACK;
	       		if( data[0] == OPL_ACK_SET )
	       			mword |= value;
	       		else
	       		{
				mword &= (~value);
			}
			write_CONTROL(mword);
	       		mword = read_CONTROL();
			break;
		case	OPL_I2C_STATUS:
			i_status.state  = read_STATUS();
			i_status.enable = read_ENABLE();
			i_status.control = read_CONTROL();
			i_status.debug = read_DEBUG();
			if( copy_to_user( (unsigned char*)arg,&i_status, sizeof(i_status) ) )
				return -EFAULT;
			break;
		case	OPL_I2C_TEST:
			test1();
			break;
	}
	return 0;
}
void inline i2c_start(void)
{
	int mword;
	DPRINTK("==== reg[%d], start\n", slave_addr);
	mword = read_CONTROL();
	mword |= CONTROL_START;
	write_CONTROL(mword);	
}

void inline i2c_nack(void)
{
	int mword;
	DPRINTK("==== nack\n");
	mword = read_CONTROL();				
	
	mword &= (~CONTROL_ACK);				
	write_CONTROL(mword);
	mword = read_CONTROL();			
	
}


void inline i2c_stop(void)
{
	int mword;
	DPRINTK("==== stop\n");
	mword = read_CONTROL();
	mword |= CONTROL_STOP;
	write_CONTROL(mword);
}
		
static int opli2c_open(struct inode * inode, struct file * file)
{
	opl_i2c_hwinit();
	DPRINTK("opl i2c driver installed \n");
	return 0;
}

static int opli2c_release(struct inode * inode, struct file * file)
{
	return 0;
}

void inline delay(int n)
{
		
#if 1
	udelay(n*100);
#else
	int i;
	for(i=0;i<n;i++)
	{
		print_sts();
	}
#endif	
}


/* 通过i2c 读取一个寄存器 */
void i2c_rr(int slave_addr, int regid)
{
	unsigned char	data;

	/*================= start ==================*/
	i2c_start();
	
	/*=========== write slave_addr to slave =========*/
	xfer_write(slave_addr<<1 | 0); /*write sla+w*/
	
	xfer_write(regid);					/*write address*/
		
	//delay(100);
	
	i2c_start();
	
	/*=========== read slave data ==============*/
	xfer_write(slave_addr<<1 | 1); /*write sla+r*/

	i2c_nack();
			
	data= xfer_read();
	
	/*================= stop ==================*/
	i2c_stop();

	printk("I2C SLAVE %d REG %d : 0x%1x = %u\n", slave_addr, regid, data & 0xff , data & 0xff);	
	
}

/* 通过i2c 读取多个寄存器 */
void i2c_rrs(int slave_addr, int regid, int cnt)
{
	int i;
	for(i=regid; i < regid + cnt; i++)
	{
		i2c_rr(slave_addr, i);
	}
}
struct file_operations opli2c_fops = {
	owner: THIS_MODULE,
	open:    opli2c_open,
	release: opli2c_release,
	ioctl:  opli2c_ioctl
      };


int init_i2c_module(void)
{
	int i;
	#if 0
		if (register_chrdev(I2C_MAJOR,"opli2c",&opli2c_fops)) {
			DPRINTK("i2c: Failed to get major %d\n", I2C_MAJOR);
			return -EIO;
			}
		DPRINTK("Registered device opli2c: major %d\n",I2C_MAJOR);
		
		
	#else
		opl_i2c_hwinit();
		DPRINTK("==== read serdis data\n");
		
		/*80, 81*/
		/*
		for (i=81;i<=81;i++)
		{
			i2c_rrs(i, 104, 4);	
		}
		*/
		i2c_rrs(81, 104, 2);	
		i2c_rrs(81, 100, 2);	
		i2c_rrs(81, 110, 2);	
	
	#endif
	
	return 0;
}

void cleanup_i2c_module(void)
{
	#if 0
	DPRINTK("Freed resources\n"); 
	unregister_chrdev(I2C_MAJOR,"opli2c");
	DPRINTK("Unregistered device gfn: major %d\n",I2C_MAJOR);
	#endif
}
module_init(init_i2c_module);
module_exit(cleanup_i2c_module);
#ifdef LINUX_24
MODULE_LICENSE("GPL2");
#else
MODULE_LICENSE("Dual BSD/GPL");
#endif
MODULE_AUTHOR("opulan inc");
MODULE_DESCRIPTION("dedicated the module for mmap the opulan switch chip register.");

