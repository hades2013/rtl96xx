#include <linux/module.h>       // for all modules
#include <linux/init.h>         // for entry/exit macros
#include <linux/kernel.h>       // for printk priority macros
#include <asm/current.h>        // process information, just for fun
#include <linux/sched.h>        // for "struct task_struct"


MODULE_LICENSE("Dual BSD/GPL"); 

static int hello_init(void)
{
printk(KERN_ALERT "Hello, Linux world!\n");
return 0;
}

static void hello_exit(void)
{
printk(KERN_ALERT "Goodbye, Linux world!\n");
}

module_init(hello_init);
module_exit(hello_exit);
