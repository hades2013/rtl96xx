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

#define PRINTK(args...) \
	if(TIMER_DEBUG)	\
		printk(args);


#define DEFINE_TIMER1_REG(reg, idx) \
static unsigned int read_TIMER1_##reg(void) \
{ \
	tempdata = *(volatile unsigned int*)(ONU_REGBASE + idx*4);\
	PRINTK("read idx : 0x%04x  data: 0x%08x\n",idx,tempdata);\
	return tempdata; \
} \
static void write_TIMER1_##reg(unsigned int v) \
{ \
	PRINTK("write idx: 0x%04x  data: 0x%08x\n",idx,v);\
	*(volatile unsigned int*)(ONU_REGBASE + idx*4) = v; \
} 


DEFINE_TIMER1_REG( CFG,0x0B41 )
DEFINE_TIMER1_REG( MATCH,0x0B45 )
DEFINE_TIMER1_REG( STATUS,0x0B49 )

#define IRQ_TIMER1		27

#define DEFINE_GPIO_REG(reg, off) \
static inline unsigned int read_GPIO_##reg(void) \
{ \
    tempdata=*(volatile unsigned int*)(ONU_REGBASE + off*4); \
  	PRINTK("read  addr = 0x%04x  data = 0x%04x\n",off, tempdata); \
	return tempdata; \
} \
static inline void write_GPIO_##reg(unsigned int v) \
{ \
	*(volatile unsigned int*)(ONU_REGBASE + off*4) = v; \
	PRINTK("write  addr = 0x%04x  data = 0x%04x\n",off,v); \
}

DEFINE_GPIO_REG(MUX, 0x0b29)        /* GPIO Multiplex Register */
DEFINE_GPIO_REG(VAL, 0x0b20)        /* GPIO Value Register, read only */
DEFINE_GPIO_REG(DIR, 0x0b21)        /* GPIO Direction Register */
DEFINE_GPIO_REG(SET, 0x0b22)        /* GPIO Set Register */
DEFINE_GPIO_REG(CLR, 0x0b23)        /* GPIO Clear Register */


static int init_ok = 0;
static int cfg_value = 0;

static void hw_watchdog_reset()
{
	unsigned int regVal;
	PRINTK("hw_watchdog_reset\n");
	regVal = read_GPIO_SET();
	if(regVal & 0x8)
	{
		regVal &= 0xFFFFFFF7;
		write_GPIO_SET(regVal);
		regVal = read_GPIO_CLR();
		regVal |= 0x8;
		write_GPIO_CLR(regVal);
	}
	else
	{
		regVal |= 0x8;
		write_GPIO_SET(regVal);
	}
	return;

}

static irqreturn_t timer1_isr(int irq, void *dev_id)
{
	unsigned int value;

	value = read_TIMER1_CFG();
	cfg_value &= ~(1<<7);
	write_TIMER1_CFG( cfg_value );
	
	hw_watchdog_reset();

	cfg_value |= (1<<7);
	write_TIMER1_CFG( cfg_value );
	return IRQ_HANDLED;
}
static int init_timer_interrupts(void)
{
	if(request_irq(IRQ_TIMER1, timer1_isr,
                       SA_INTERRUPT, "timer1 rx", NULL) != 0) {
                return -EBUSY;
        }

	return 0;
}

int timer1_init(void)
{
	cfg_value = (1<<6)|1|(2<<1);
	write_TIMER1_CFG( cfg_value );
	/* configure timer1 match count to 400ms */
	write_TIMER1_MATCH( 50000000 );
	
	if (!init_timer_interrupts())
		init_ok = 0;
	else
		init_ok = 1;

	/* enable timer interrupt */
	cfg_value |= (1<<7);
	write_TIMER1_CFG( cfg_value );
	return init_ok;
}

void timer1_close(void)
{
	//disable timer1
	write_TIMER1_CFG( 0 );
	
	free_irq(IRQ_TIMER1, NULL);
}

//EXPORT_SYMBOL(timer_init);
//EXPORT_SYMBOL(timer_close);

int init_module(void)
{

	timer1_init();
	return 0;
}

void cleanup_module(void)
{
	timer1_close();
}

/**
wait_just_a_bit(HZ/10);

static void wait_just_a_bit(int foo)
{
	set_current_state(TASK_INTERRUPTIBLE);
	schedule_timeout(foo);
}
**/
