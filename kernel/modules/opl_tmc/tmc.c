#include <linux/kernel.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/proc_fs.h>

#define ONU_REGBASE		0xBF000000

static unsigned int tempdata;
unsigned int TIMER_DEBUG = 0;

#define PRINTK(args...)\
	if(TIMER_DEBUG)	\
		printk(args);	\


#define DEFINE_TIMER0_REG(reg, idx) \
static unsigned int read_TIMER0_##reg(void) \
            {                                       \
                tempdata = *(volatile unsigned int*) (ONU_REGBASE + idx*4);\
                PRINTK("read idx : 0x%04x  data: 0x%08x\n",idx,tempdata);\
                return tempdata; } \
static void write_TIMER0_##reg(unsigned int v) \
            {                                           \
                PRINTK("write idx: 0x%04x  data: 0x%08x\n",idx,v);\
                *(volatile unsigned int*)(ONU_REGBASE + idx*4) = v; } \
                
DEFINE_TIMER0_REG( CFG,0x0B40 )
DEFINE_TIMER0_REG( MATCH,0x0B44 )
DEFINE_TIMER0_REG( STATUS,0x0B48 )


#define DEFINE_TM_REG(reg, idx) \
static unsigned int read_TM_##reg(void) \
            {                                       \
                tempdata = *(volatile unsigned int*) (ONU_REGBASE + idx*4);\
                PRINTK("read idx : 0x%04x  data: 0x%08x\n",idx,tempdata);\
                return tempdata; } \
static void write_TM_##reg(unsigned int v) \
            {                                           \
                PRINTK("write idx: 0x%04x  data: 0x%08x\n",idx,v);\
                *(volatile unsigned int*)(ONU_REGBASE + idx*4) = v; } \

DEFINE_TM_REG( CTL,0x2000 )
DEFINE_TM_REG( IND,0x20BC )
DEFINE_TM_REG( SHD,0x20BE )

#define IRQ_TIMER0		28

static int init_ok = 0;
static int s_cnt = 0;
static int cfg_value = 0;
static int no_enable = 0;
static unsigned int s_cellnum;
static unsigned int s_pktnum;

static int wait_busy( void )
{
	int temp=1;
	unsigned int timeout=50000;
	unsigned int value;
		
	while (timeout--) {
		value = read_TM_IND();
		if( !(value&(1<<31)) )
		{
			temp = 0;
			break;
		}
	}
	return temp;
}


static void rd_tm_table(void)
{
	unsigned int busy;
	unsigned int value;
	
	value = (1<<31)|8;
	write_TM_IND( value );
	
	busy = wait_busy();

	if( busy )
	{
		printk("ERROR!,  table busy bit always set\n");
	}
	
	value = read_TM_SHD( )&(0x7ffffff);
	s_cellnum = value>>13;
	s_pktnum = value&0x1fff;
}
static void tm_ctl(void)
{
		rd_tm_table();
		if( (s_cellnum >= 1000) || (no_enable >= 5)  )
		{
			write_TM_CTL( 0x103 );
			no_enable = 0;
		}
		else
		if( s_cellnum <= 500 )
		{
			no_enable++;
			write_TM_CTL( 0x102 );
		}
}
static void timer0_proc(void)
{
	s_cnt++;
	tm_ctl();
	return;
}
static irqreturn_t timer0_isr(int irq, void *dev_id)
{
	unsigned int value;

	value = read_TIMER0_CFG();
	cfg_value &= ~(1<<7);
	write_TIMER0_CFG( cfg_value );
	
	timer0_proc();

	cfg_value |= (1<<7);
	write_TIMER0_CFG( cfg_value );
	return IRQ_HANDLED;
}
static int init_timer_interrupts(void)
{
	if(request_irq(IRQ_TIMER0, timer0_isr,
                       SA_INTERRUPT, "timer0 rx", NULL) != 0) {
                return -EBUSY;
        }

	return 0;
}

int timer0_proc_read(char *buf, char **start, off_t offset, 
		    int count, int *eof, void *data)
{
	int len;

	len = sprintf(buf, 
		      "counter.....: %d\n"
		      "cellnum.....: %d\n",
		      s_cnt,s_cellnum);


	*eof=1;
	return len;
}
int timer0_init(void)
{
	
	cfg_value = (1<<6)|1|(2<<1);
	write_TIMER0_CFG( cfg_value );
	write_TIMER0_MATCH( 125000 );
	
	if (!init_timer_interrupts())
		init_ok = 0;
	else
		init_ok = 1;
    
	create_proc_read_entry("Timer0", 0, NULL, timer0_proc_read, NULL);

	/* enable timer interrupt */
	cfg_value |= (1<<7);
	write_TIMER0_CFG( cfg_value );
	return init_ok;
}

void timer0_close(void)
{
	//disable timer0
	write_TIMER0_CFG( 0 );
	
	free_irq(IRQ_TIMER0, NULL);
	remove_proc_entry("Timer0", NULL);
}

//EXPORT_SYMBOL(timer_init);
//EXPORT_SYMBOL(timer_close);

int init_module(void)
{

	timer0_init();
	return 0;
}

void cleanup_module(void)
{
	timer0_close();
}

/**
wait_just_a_bit(HZ/10);

static void wait_just_a_bit(int foo)
{
	set_current_state(TASK_INTERRUPTIBLE);
	schedule_timeout(foo);
}
**/
