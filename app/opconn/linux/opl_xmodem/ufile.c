#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> 
#include <linux/major.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#include "malta_int.h"


#define UFILE_MAJOR     220   //mknod /dev/ufile  c 220  0


#if 1
#define debug(x...) printk(x)
#else
#define debug(x...) 
#endif


//OPL_UART0_IRQ
extern void ipmux_irq_enable( unsigned int irq );
extern void ipmux_irq_disable( unsigned int irq);
extern int main_test(void);


#define THRE    0x20
#define DR      0x01


#define UART0_LSR   0xBF001814  /* Line Status Register */
#define UART0_THR   0xBF001800  /* Transmit Holding register */
#define UART0_RBR   0xBF001800  /* Receive Buffer register */


#define pUART0_LSR  ((volatile unsigned int *)UART0_LSR)
#define pUART0_THR  ((volatile unsigned int *)UART0_THR)
#define pUART0_RBR  ((volatile unsigned int *)UART0_RBR)


void uart0SendChar(unsigned char c)
{
    while (!(*pUART0_LSR & THRE));
    *pUART0_THR = c;
}

void uart0SendString(unsigned char *s)
{
    char a;
    while ((a = *s++)) {
        uart0SendChar(a);
    }
}

void uart0SendChars(unsigned char *buf, unsigned int size)
{
    while (size--) {
        uart0SendChar(*buf++);
    }
}

unsigned char uart0GetCh()
{
    while (!(*pUART0_LSR & DR));
    return *pUART0_RBR;
}

unsigned char uart0GetChar(unsigned int *a)
{
    if (!(*pUART0_LSR & DR))
        return 0;
    *a = *pUART0_RBR;
    return 1;
}

static int ufile_open ( struct inode * , struct file * ); //
static int ufile_release ( struct inode * , struct file * ); 
static int ufile_ioctl ( struct inode * , struct file * , unsigned int , unsigned long);

static struct file_operations ufile_ctl_fops = { 
open: ufile_open , 
      ioctl: ufile_ioctl ,
      release: ufile_release , 
};
static int ufile_open(struct inode *inode , struct file *file)
{

    debug("open ufile devices\n"); 

//  MOD_INC_USE_COUNT;

    return 0;

}

static int ufile_release(struct inode *inode, struct file *filp)
{
//  MOD_DEC_USE_COUNT;
    debug("device closed!!!\n");

    return 0;
}

static int ufile_ioctl(struct inode *inode, struct file *file, unsigned int command ,unsigned long arg)
{
    char val;
    int i = 9;
    int name;
    int ret = 9;

    switch( command )
    {
        case 0:
            ipmux_irq_disable( OPL_UART0_IRQ );
            debug("pUART0_LSR:%x\n",*pUART0_LSR);
            debug("OPL_UART0_IRQ:%d\n",OPL_UART0_IRQ);
            break;
        case 1:
            ipmux_irq_enable( OPL_UART0_IRQ );  
            break;
        case 2:
            ipmux_irq_disable( OPL_UART0_IRQ );
            ret = recv_firmware();      
            copy_to_user( (void*)arg, &ret  ,sizeof(int));
            ipmux_irq_enable( OPL_UART0_IRQ );  
            break;
        case 3:
            ipmux_irq_disable( OPL_UART0_IRQ );
            ret = send_firmware();
            copy_to_user( (void*)arg, &ret  ,sizeof(int));
            ipmux_irq_enable( OPL_UART0_IRQ );  
            break;
        case 4:
            ipmux_irq_disable( OPL_UART0_IRQ );
            ret = recv_config();    
            copy_to_user( (void*)arg, &ret  ,sizeof(int));
            ipmux_irq_enable( OPL_UART0_IRQ );
            break;
        case 5:
            ipmux_irq_disable( OPL_UART0_IRQ );
            ret = send_config();
            copy_to_user( (void*)arg, &ret  ,sizeof(int));
            ipmux_irq_enable( OPL_UART0_IRQ );
            break;
        case 6:
            ipmux_irq_disable( OPL_UART0_IRQ );
            ret = recv_test();          
            copy_to_user( (void*)arg, &ret  ,sizeof(int));
            ipmux_irq_enable( OPL_UART0_IRQ );      
            break;
        case 7:
            ipmux_irq_disable( OPL_UART0_IRQ );
            ret = send_test();          
            copy_to_user( (void*)arg, &ret  ,sizeof(int));
            ipmux_irq_enable( OPL_UART0_IRQ );      
            break;
        default:
            break;

    }

    return 0;
}

static int __init u_file_init(void)
{
    int err;

    debug("ufile_init\n");
    err = register_chrdev(UFILE_MAJOR, "ufile",  &ufile_ctl_fops);
    if(err<0)
    {
        debug("fail to register\n");    
        return -1;
    }
    debug("success to register!!!\n");

    return 0;
}

static void __exit u_file_exit(void)
{
    debug ( "release this device!!!\n" );
    unregister_chrdev(UFILE_MAJOR, "ufile");        
}

module_init(u_file_init);
module_exit(u_file_exit);
