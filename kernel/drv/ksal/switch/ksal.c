/*****************************************************************************
                                                                             
*****************************************************************************/
#ifdef  __cplusplus
    extern "C"{
#endif

#include <linux/autoconf.h>
#include <linux/compiler.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/socket.h>
#include <linux/miscdevice.h>

#include <lw_type.h>
#include "lw_drv_pub.h"
#include "lw_drv_req.h"
#include "hal_common.h"

/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/
extern DRV_RET_E Drv_L2SwtichIoctl(DRV_REQ_S *pstDrvReq);
/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/

//l2_driver_t* g_l2_switch_drv = NULL;

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/
/*
why define to 134, please reference to file:linux/miscdevice.h,
if you change the kernel and find some error, 
please modify it, and re-mknode device /dev/l2switch in root-fs
*/
#define L2SWITCH_MINOR		222	/* l2 switch */
/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/


static ssize_t l2_switch_write(struct file *file, const char __user *data,
				 size_t len, loff_t *ppos)
{
    file = file;
    data = data;
    len = len;
    ppos = ppos;
    return 0;
}

static ssize_t l2_switch_read(struct file *file, char __user *buf,
				size_t len, loff_t *ppos)
{
    file = file;
    buf = buf;
    len = len;
    ppos = ppos;
    return 0;
}

static int l2_switch_open(struct inode *inode, struct file *file)
{
    inode = inode;
    file =file;
    return 0;
}

/*lint -save -e529 */
static int Dev_L2SwtichIoctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
    void __user *argp = (void __user *)arg;
    DRV_REQ_S drv_req;
    DRV_RET_E ret = DRV_OK;

    inode = inode;
    file = file;

    if ( copy_from_user(&drv_req, argp, sizeof(DRV_REQ_S)) )
    {
        printk("copy_from_user err\n");
        return -EFAULT;
    }

    if(SIOCDRVPRIVATE != cmd){
        return -EOPNOTSUPP;
    }
    ret = Drv_L2SwtichIoctl(&drv_req);
    drv_req.err = ret;

    if ( copy_to_user(argp, &drv_req, sizeof(DRV_REQ_S)) )
    {
        printk("copy_to_user err\n");
        return -EFAULT;
    }

    return 0;
}
/*lint -restore */

/*lint -save -e785 */
static const struct file_operations l2_switch_fops = {
	.owner = THIS_MODULE,
	.write = l2_switch_write,
	.read  = l2_switch_read,
	.open  = l2_switch_open,
	.ioctl = Dev_L2SwtichIoctl
};

static struct miscdevice l2_switch_miscdev = {
  .minor = L2SWITCH_MINOR,
  .name  = DEV_SWTICH,
  .fops  = &l2_switch_fops,
};
/*lint -restore */

__init void find_current_switch(void)
{
    return;
}

__init int  regist_l2_switch_device(void)
{
	int	ret;

	ret = misc_register(&l2_switch_miscdev);
	if (ret) {
		printk (KERN_CRIT "Cannot regist_l2_switch miscdev\n");
		return ret;
	}

	return 0;
}

__init int switch_init(void)
{
    //struct task_struct * phy_task_th;
    
	printk("Module switch init\n");
	find_current_switch();

    Hal_SwitchInit();

#if 0
	if(NULL == g_l2_switch_drv){
	    printk("Not find l2 switch chip\n");
	    return -1;
    }

	if(g_l2_switch_drv->l2d_switch_name){
        printk("Find l2_switch: %s\n", g_l2_switch_drv->l2d_switch_name);
	}

    if(g_l2_switch_drv->l2d_init){
    	g_l2_switch_drv->l2d_init();
    }
#endif

	if(regist_l2_switch_device()){
	    printk("Fail to register l2_switch device\n");
    }
	    
	return 0;
}

module_init(switch_init);

#ifdef  __cplusplus
}
#endif

