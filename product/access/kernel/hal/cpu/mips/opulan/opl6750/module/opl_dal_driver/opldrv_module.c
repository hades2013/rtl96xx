#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>

#include "opconn_ioctl.h"

#define OPCONN_MAJOR 237
#define OPCONN_PROC_PMOD 0644
#define OPL_OPCONN_DEVICE_NAME "opl_dal_driver"

static u32 opconn_is_open = 0;
static u32 opconn_open_count = 0;

static int opconn_open(struct inode *inode,struct file *filp);
static int opconn_release(struct inode *inode,struct file *filp);
static ssize_t opconn_read(struct file *filp, char  *buffer, size_t length, loff_t *offset);
static ssize_t opconn_write(struct file *filp,const char *buf,size_t length,loff_t *offset);
static int opconn_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned int arg);

static const struct file_operations opconn_fops = {
	.owner = THIS_MODULE,
	.open = opconn_open,
	.read = opconn_read,
	.write = opconn_write,
	.release = opconn_release,
	.ioctl = opconn_ioctl,
};

static int opconn_open(struct inode *inode,struct file *filp)
{
	if(test_and_set_bit(0,&opconn_is_open)){
		OPL_DAL_PRINTF(("the device is opened\n")); 
	}
	opconn_open_count ++;
	return 0;
}

static int opconn_release(struct inode *inode,struct file *filp)
{
	opconn_open_count --;
	if(!opconn_open_count)
		clear_bit(0,&opconn_is_open);
	return 0;
}

static ssize_t opconn_read(struct file *filp, char  *buffer, size_t length, loff_t *offset)
{
	return 0;
}

static ssize_t opconn_write(struct file *filp,const char *buf,size_t length,loff_t *offset)
{
	return 0;
}


static int opconn_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned int arg)
{
	return opconn_dal_ioctl(cmd, arg);
}


static int __init opconn_init(void)
{
	int ret =0;
	ret = register_chrdev(OPCONN_MAJOR, OPL_OPCONN_DEVICE_NAME, &opconn_fops);
	ret += opconn_dal_init();
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

#ifdef LINUX_24
MODULE_LICENSE("GPL2");
#else
MODULE_LICENSE("Dual BSD/GPL");
#endif
MODULE_AUTHOR("Qualcomm Atheros, Inc.");
MODULE_DESCRIPTION("OPCONN PON ONU driver");
