#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>

#include "opconn_ioctl.h"

#define OPCONN_MAJOR 237
#define OPCONN_PROC_PMOD 0644
#define OPL_OPCONN_DEVICE_NAME "/dev/opl_dal_driver"




int opconn_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned int arg)
{
        return opconn_dal_ioctl(cmd, arg);
}



static const struct file_operations opconn_fops = {
        .owner = THIS_MODULE,
        .ioctl = opconn_ioctl,
};

static int __init opconn_init(void)
{
        int ret =0;
        ret = register_chrdev(OPCONN_MAJOR, OPL_OPCONN_DEVICE_NAME, &opconn_fops);
        return ret;
}

static void __exit opconn_exit(void)
{
  int ret;
  ret = unregister_chrdev(OPCONN_MAJOR,OPL_OPCONN_DEVICE_NAME);
  if(ret < 0){
    /*TBD:add debug message  */
  }
  return;
}


module_init(opconn_init);
module_exit(opconn_exit);

MODULE_DESCRIPTION("OPCONN PON ONU driver");
