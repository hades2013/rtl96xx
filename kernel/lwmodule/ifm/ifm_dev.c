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
#include "lw_if_pub.h"
#include "lw_drv_req.h"
//#include "hal_common.h"

#define IFM_MINOR		230	/* ifm */


static ssize_t ifm_write(struct file *file, const char __user *data,
				 size_t len, loff_t *ppos)
{
    file = file;
    data = data;
    len = len;
    ppos = ppos;
    return 0;
}

static ssize_t ifm_read(struct file *file, char __user *buf,
				size_t len, loff_t *ppos)
{
    file = file;
    buf = buf;
    len = len;
    ppos = ppos;
    return 0;
}

static int ifm_open(struct inode *inode, struct file *file)
{
    inode = inode;
    file =file;
    return 0;
}

extern IF_RET_E Ifm_Ioctl(IFM_REQ_S *pUsrreq);

/*lint -save -e529 */
static int Dev_ifmIoctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
    void __user *argp = (void __user *)arg;
    IFM_REQ_S ifm_req;
    IF_RET_E ret = DRV_OK;

    inode = inode;
    file = file;

    if ( copy_from_user(&ifm_req, argp, sizeof(IFM_REQ_S)) )
    {
        printk("copy_from_user err\n");
        return -EFAULT;
    }

    if(SIOCDRVPRIVATE != cmd){
        return -EOPNOTSUPP;
    }    

    ret=Ifm_Ioctl(&ifm_req);
  //  drv_req.err = ret;
    if ( copy_to_user(argp, &ifm_req, sizeof(IFM_REQ_S)) )
    {
        printk("copy_to_user err\n");
        return -EFAULT;
    }

    return 0;
}
/*lint -restore */

/*lint -save -e785 */
static const struct file_operations ifm_fops = {
	.owner = THIS_MODULE,
	.write = ifm_write,
	.read  = ifm_read,
	.open  = ifm_open,
	.ioctl = Dev_ifmIoctl
};

static struct miscdevice ifm_miscdev = {
  .minor = IFM_MINOR,
  .name  = "ifm",
  .fops  = &ifm_fops,
};
/*lint -restore */


int  regist_ifm_device(void)
{
	int	ret;

	ret = misc_register(&ifm_miscdev);
	if (ret) {
		printk (KERN_CRIT "Cannot regist_l2_switch miscdev\n");
		return ret;
	}

	return 0;
}
extern int  IF_ModuleInit(VOID);

__init int ifm_init(void)
{
    
	printk("Module ifm init\n");
    IF_ModuleInit();
	if(regist_ifm_device())
	{
	    printk("Fail to register ifm device\n");
   	}
    printk("Module ifm init ok\n");
	return 0;
}

module_init(ifm_init);

#ifdef  __cplusplus
}
#endif

