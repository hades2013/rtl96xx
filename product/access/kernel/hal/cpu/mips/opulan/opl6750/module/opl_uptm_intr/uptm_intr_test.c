#include <linux/kernel.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/proc_fs.h>
#include <linux/kthread.h>

#define ONU_REGBASE		0xBF000000

static unsigned int tempdata;
unsigned int TIMER_DEBUG = 0;

#define PRINTK(args...)\
	if(TIMER_DEBUG)	\
		printk(args);	\



#define IRQ_UPTM		21

static int init_ok = 0;
static int cfg_value = 0;
/* 错误码 */ 
#define OPEN_FILE_ERROR                -1                /* 打开文件错误 */ 
#define INPUT_BUFFER_NULL        -2                /* 输入缓冲区指针为空 */ 
/****************************************************************************** 
* 名称：ker_fread 
* 功能：读文件 
* 参数：filename: 文件名 
*       buffer:   缓冲区，存放读出的数据 
*             len:      读取的最大长度 
* 返回值：>0 :    实际读取的字节数 
*         <0:     错误，参见错误码 
******************************************************************************/ 
int ker_fread(const char *filename, char *buffer, unsigned long len) 
{ 
	struct file *fp = NULL; 
	struct inode *pinode = NULL; 
	mm_segment_t old_fs; 
	off_t fsize; 
	int readlen = 0; 

	if(buffer == NULL) 
	{ 
		printk("The input buffer points to NULL!\n"); 
		return INPUT_BUFFER_NULL; 
	} 

	fp = filp_open(filename, O_RDONLY, 0); 
	if(IS_ERR(fp)) 
	{ 
		printk("Open file error!\n"); 
		return OPEN_FILE_ERROR; 
	} 
	pinode = fp->f_dentry->d_inode; 
	fsize = pinode->i_size; 
	old_fs = get_fs(); 
	set_fs(KERNEL_DS); 
	if(len < fsize) 
	{ 
		readlen = fp->f_op->read(fp, buffer, len, &(fp->f_pos)); 
		buffer[len] = '\0'; 
	} 
	else 
	{ 
		readlen = fp->f_op->read(fp, buffer, fsize, &(fp->f_pos)); 
		buffer[fsize] = '\0'; 
	} 
	set_fs(old_fs); 

	filp_close(fp, NULL); 
	return readlen; 
} 

/****************************************************************************** 
* 名称：ker_fwrite 
* 功能：写文件 
* 参数：filename: 文件名 
*       buffer:   缓冲区，存放要写入的数据 
*             len:      写入的最大长度 
* 返回值：>0 :    实际写入的字节数 
*         <0:     错误，参见错误码 
******************************************************************************/ 
int ker_fwrite(const char *filename, char *buffer, unsigned long len) 
{ 
	struct file *fp = NULL; 
	mm_segment_t fs; 
	int writelen = 0; 
	unsigned long buflen; 

	fp = filp_open(filename, O_RDWR | O_TRUNC | O_CREAT, 0644); 
	if(IS_ERR(fp)) 
	{ 
		printk("Open file error\n"); 
		return OPEN_FILE_ERROR; 
	} 
	buflen = strlen(buffer); 
	fs = get_fs(); 
	set_fs(KERNEL_DS); 
	if(buflen <= len) 
	{ 
		writelen = fp->f_op->write(fp, buffer, buflen, &(fp->f_pos)); 
	} 
	else 
	{ 
		writelen = fp->f_op->write(fp, buffer, len, &(fp->f_pos)); 
	} 
	set_fs(fs); 
	filp_close(fp, NULL); 
	return writelen; 
} 

static unsigned int start_write_file = 0; 
extern unsigned long msleep_interruptible(unsigned int); 
char buf[4096] = {0}; 

void write_file(void) 
{ 
	int len = 0; 
	memset(buf,0,sizeof(buf)); 
	unsigned short addr_hi = 0; 
	unsigned char  blk_addr = 0; 
	unsigned char  col_addr = 0; 
	unsigned int reg_addr = 0; 
        
	int i = 0; 
	int j = 0; 
        
	tempdata = *(volatile unsigned int*) (ONU_REGBASE + 0x0A1A * 4); 
	len += sprintf(buf + len,"%x,%x\n",0x0A1A,tempdata); 
	tempdata = *(volatile unsigned int*) (ONU_REGBASE + 0x2003 * 4); 
	len += sprintf(buf + len,"%x,%x\n",0x2003,tempdata); 
	tempdata = *(volatile unsigned int*) (ONU_REGBASE + 0x2004 * 4); 
	len += sprintf(buf + len,"%x,%x\n",0x2003,tempdata); 
	tempdata = *(volatile unsigned int*) (ONU_REGBASE + 0x4001 * 4); 
	len += sprintf(buf + len,"%x,%x\n",0x4001,tempdata); 
	tempdata = *(volatile unsigned int*) (ONU_REGBASE + 0x5042 * 4); 
	len += sprintf(buf + len,"%x,%x\n",0x5042,tempdata); 
        
        
	addr_hi = 0x8001; 
	len += sprintf(buf + len,"mpcp_test\n"); 
	for(i = 1; i < 16; i++) 
	{ 
		col_addr = i; 
		reg_addr = (addr_hi << 16) |  (col_addr); 
            
		*(volatile unsigned int*)(ONU_REGBASE + 0x9 * 4) = reg_addr; 
		len += sprintf(buf + len,"write %x,%x\n",0x9,reg_addr);     
            
		tempdata = *(volatile unsigned int*) (ONU_REGBASE + 0x000A * 4); 
		len += sprintf(buf + len,"%x,%x\n",0x000A,tempdata); 
		tempdata = *(volatile unsigned int*) (ONU_REGBASE + 0x000A * 4); 
		len += sprintf(buf + len,"%x,%x\n",0x000A,tempdata); 
	} 
        
	addr_hi = 0x8002; 
	len += sprintf(buf + len,"tm_test\n"); 
	for(i = 0; i < 7; i++) 
	{ 
		for(j = 0; j < 7; j++) 
		{ 
			blk_addr = i; 
			col_addr = j; 
			reg_addr = (addr_hi << 16) | (blk_addr << 8) | (col_addr); 
                
			*(volatile unsigned int*)(ONU_REGBASE + 0x9 * 4) = reg_addr; 
			len += sprintf(buf + len,"write %x,%x\n",0x9,reg_addr);     
                
			tempdata = *(volatile unsigned int*) (ONU_REGBASE + 0x000A * 4); 
			len += sprintf(buf + len,"%x,%x\n",0x000A,tempdata); 
			tempdata = *(volatile unsigned int*) (ONU_REGBASE + 0x000A * 4); 
			len += sprintf(buf + len,"%x,%x\n",0x000A,tempdata); 
		} 
	} 

	printk("len = %d, strlen(buf) = %d\n",len,strlen(buf)); 

	ker_fwrite("/tmp/dumplog",buf,strlen(buf)); 
} 

static void do_uptm_proc(unsigned long unused) 
{ 
	int i = 0; 

	while(1) 
	{ 
		if(start_write_file) 
		{ 
			write_file(); 
			/* reset PON chip */ 
			*(volatile unsigned int*)(ONU_REGBASE ) = 0x80000000;  
			break; 
		} 
		else 
		{   
			msleep_interruptible(100); 
		} 
	} 
}
static void uptm_proc(void)
{
#if 0
	start_write_file = 1;  /*触发线程写log*/  
#else
	int len = 0; 
	memset(buf,0,sizeof(buf)); 
	unsigned short addr_hi = 0; 
	unsigned char  blk_addr = 0; 
	unsigned char  col_addr = 0; 
	unsigned int reg_addr = 0; 
        
	int i = 0; 
	int j = 0; 
        
	tempdata = *(volatile unsigned int*) (ONU_REGBASE + 0x0A1A * 4); 
	len += sprintf(buf + len,"%x,%x\n",0x0A1A,tempdata); 
	tempdata = *(volatile unsigned int*) (ONU_REGBASE + 0x2003 * 4); 
	len += sprintf(buf + len,"%x,%x\n",0x2003,tempdata); 
	tempdata = *(volatile unsigned int*) (ONU_REGBASE + 0x2004 * 4); 
	len += sprintf(buf + len,"%x,%x\n",0x2003,tempdata); 
	tempdata = *(volatile unsigned int*) (ONU_REGBASE + 0x4001 * 4); 
	len += sprintf(buf + len,"%x,%x\n",0x4001,tempdata); 
	tempdata = *(volatile unsigned int*) (ONU_REGBASE + 0x5042 * 4); 
	len += sprintf(buf + len,"%x,%x\n",0x5042,tempdata); 
        
        
	addr_hi = 0x8001; 
	len += sprintf(buf + len,"mpcp_test\n"); 
	for(i = 1; i < 16; i++) 
	{ 
		col_addr = i; 
		reg_addr = (addr_hi << 16) |  (col_addr); 
            
		*(volatile unsigned int*)(ONU_REGBASE + 0x9 * 4) = reg_addr; 
		len += sprintf(buf + len,"write %x,%x\n",0x9,reg_addr);     
            
		tempdata = *(volatile unsigned int*) (ONU_REGBASE + 0x000A * 4); 
		len += sprintf(buf + len,"%x,%x\n",0x000A,tempdata); 
		tempdata = *(volatile unsigned int*) (ONU_REGBASE + 0x000A * 4); 
		len += sprintf(buf + len,"%x,%x\n",0x000A,tempdata); 
	} 
        
	addr_hi = 0x8002; 
	len += sprintf(buf + len,"tm_test\n"); 
	for(i = 0; i < 7; i++) 
	{ 
		for(j = 0; j < 7; j++) 
		{ 
			blk_addr = i; 
			col_addr = j; 
			reg_addr = (addr_hi << 16) | (blk_addr << 8) | (col_addr); 
                
			*(volatile unsigned int*)(ONU_REGBASE + 0x9 * 4) = reg_addr; 
			len += sprintf(buf + len,"write %x,%x\n",0x9,reg_addr);     
                
			tempdata = *(volatile unsigned int*) (ONU_REGBASE + 0x000A * 4); 
			len += sprintf(buf + len,"%x,%x\n",0x000A,tempdata); 
			tempdata = *(volatile unsigned int*) (ONU_REGBASE + 0x000A * 4); 
			len += sprintf(buf + len,"%x,%x\n",0x000A,tempdata); 
		} 
	} 

	printk("len = %d, strlen(buf) = %d\n",len,strlen(buf)); 

	printk(buf); 
#endif
	return;
}
static irqreturn_t uptm_isr(int irq, void *dev_id)
{
	local_irq_disable(); 

	/* disable uptm interrupt */
	*(volatile unsigned int*)(0xBF008014) = 0; 

#if 1 
	/* turn off laser */ 
	*(volatile unsigned int*)(0xBF002864) = 0x8; 
	/* disable level 1 interrupt */
	*(volatile unsigned int*)(0xbf002e18) = 0x0;
	/* reset GPIO to default */ 
	*(volatile unsigned int*)(0xBF002CA4) = 0x0; 
	*(volatile unsigned int*)(0xBF002C84) = 0x0; 
	/* disable CPDMA */
	*(volatile unsigned int*)(0xbf018014) = 0x0;
	/* disable PMAC */
	*(volatile unsigned int*)(0xbf002800) = 0x0;
	/* disable GMAC */
	*(volatile unsigned int*)(0xbf002400) = 0x0;
	/* disable FEMAC */
	*(volatile unsigned int*)(0xbf003400) = 0x0;
	uptm_proc(); 
#else 
	/* turn off laser */ 
	*(volatile unsigned int*)(0xBF002864) = 0x8; 
	/* reset GPIO to default */ 
	*(volatile unsigned int*)(0xBF002CA4) = 0x0; 
	*(volatile unsigned int*)(0xBF002C84) = 0x0; 
	/* reset PON chip */ 
	*(volatile unsigned int*)(ONU_REGBASE ) = 0x80000000; 
#endif 

	local_irq_enable(); 
	return IRQ_HANDLED; 
} 
static int init_uptm_interrupts(void) 
{ 
	if(request_irq(IRQ_UPTM, uptm_isr, 
		SA_INTERRUPT, "UP TM", NULL) != 0) { 
		return -EBUSY; 
	} 
	return 0; 
}


int uptm_intr_init(void)
{
	if (!init_uptm_interrupts())
		init_ok = 0;
	else
		init_ok = 1;

	kthread_run(do_uptm_proc, NULL, "UptmTask");
    
	/* enable uptm interrupt */
	*(volatile unsigned int*)(0xBF008014) = 0xF38; 

	return init_ok;
}

void uptm_intr_close(void)
{
	//disable uptm interrupt
	*(volatile unsigned int*)(0xBF008014) = 0; 
	free_irq(IRQ_UPTM, NULL);
}
int init_module(void)
{
	uptm_intr_init();
	return 0;
}

void cleanup_module(void)
{
	uptm_intr_close();
}

