/*
 * Copyright (C) 2011 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 *
 *
 * $Revision: 40672 $
 * $Date: 2013-07-02 10:10:54 +0800 (Tue, 02 Jul 2013) $
 *
 * Purpose : EPON polling kernel thread
 *
 * Feature : Use kernel thread to perodically polling the LOS state
 *
 */

#ifdef CONFIG_EPON_FEATURE
/*
 * Include Files
 */
#include <linux/module.h> 
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/init.h> 
#include <linux/proc_fs.h>

/* For RTK APIs */
#include <common/rt_error.h>
#include <rtk/epon.h>

#include <pkt_redirect.h>

/*
 * Symbol Definition
 */
#define EPON_POLLING_FLAG_STOPPOLLING       0x00000001UL

/*
 * Data Declaration
 */
__DRAM unsigned int polling_flag = 0;
struct proc_dir_entry *epon_proc_dir = NULL;
struct proc_dir_entry *polling_flag_entry = NULL;
struct task_struct *pEponPollTask;


unsigned int _epon_polling_los_on_cnt = 0;
unsigned int _epon_polling_los_off_cnt = 0;


/*
 * Macro Definition
 */

/*
 * Function Declaration
 */

int epon_polling_thread(void *data)
{
    int ret;
    unsigned char losMsg[] = {
        0x15, 0x68, /* Magic key */
        0xde, 0xad  /* Message body */
    };
    rtk_enable_t state, currState = ENABLED;
    rtk_epon_llid_entry_t llidEntry;
    rtk_epon_regReq_t regEntry;

    while(!kthread_should_stop())
    {
        /* No need to wake up earlier */
        set_current_state(TASK_UNINTERRUPTIBLE);
        schedule_timeout(2 * HZ / 100);

        if(polling_flag & EPON_POLLING_FLAG_STOPPOLLING)
        {
            /* Stop polling, just wati until next round */
            continue;
        }

        ret = rtk_epon_losState_get(&state);
        do {
            if((RT_ERR_OK == ret) && (currState != state))
            {
                if(DISABLED == state)
                {
                    //printk("\n fiber-on\n");
                    _epon_polling_los_off_cnt++;
					#if 0
                    /* Trigger register */
                    llidEntry.llidIdx = 0;
                    ret = rtk_epon_llid_entry_get(&llidEntry);
                    if(RT_ERR_OK != ret)
                    {
                        break;
                    }

                    llidEntry.valid = DISABLED;
                    llidEntry.llid = 0x7fff;
                    ret = rtk_epon_llid_entry_set(&llidEntry);
                    if(RT_ERR_OK != ret)
                    {
                        break;
                    }

                    ret = rtk_epon_registerReq_get(&regEntry);
                    if(RT_ERR_OK != ret)
                    {
                        break;
                    }

                    regEntry.llidIdx = 0;
                    regEntry.doRequest = ENABLED;
                    ret = rtk_epon_registerReq_set(&regEntry);
                    if(RT_ERR_OK != ret)
                    {
                        break;
                    }
					#endif
                }
                else
                {
                    //printk("\n fiber-off\n");
                    _epon_polling_los_on_cnt++;
                    /* Trigger lost of link */
                    llidEntry.llidIdx = 0;
                    ret = rtk_epon_llid_entry_get(&llidEntry);
                    if(RT_ERR_OK != ret)
                    {
                        break;
                    }

                    llidEntry.valid = DISABLED;
                    llidEntry.llid = 0x7fff;
                    ret = rtk_epon_llid_entry_set(&llidEntry);
                    if(RT_ERR_OK != ret)
                    {
                        break;
                    }
                    pkt_redirect_kernelApp_sendPkt(PR_USER_UID_EPONOAM, 1, sizeof(losMsg), losMsg);
                }
                currState = state;
            }
        }while(0);
    }

    return 0;
}

static int polling_flag_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int len;

	len = sprintf(page, "[polling_flag = 0x%08x]\n", polling_flag);
	len += sprintf(page + len, "EPON_POLLING_FLAG_STOPPOLLING\t0x%08x\n", EPON_POLLING_FLAG_STOPPOLLING);

	len += sprintf(page + len, "EPON_LASER_ON_CNT\t %d\n", _epon_polling_los_on_cnt);
	len += sprintf(page + len, "EPON_LASER_OFF_CNT\t %d\n", _epon_polling_los_off_cnt);



	return len;
}

static int polling_flag_write(struct file *file, const char *buffer, unsigned long count, void *data)
{
	unsigned char tmpBuf[16] = {0};
	int len = (count > 15) ? 15 : count;

	if (buffer && !copy_from_user(tmpBuf, buffer, len))
	{
		polling_flag = simple_strtoul(tmpBuf, NULL, 16);
		printk("write polling_flag to 0x%08x\n", polling_flag);
		return count;
	}
	return -EFAULT;
}

static void epon_polling_dbg_init(void)
{
    /* Create proc debug commands */
    if(NULL == epon_proc_dir)
    {
        epon_proc_dir = proc_mkdir("epon", NULL);
    }
    if(epon_proc_dir)
    {
    	polling_flag_entry = create_proc_entry("polling_flag", 0, epon_proc_dir);
    	if(polling_flag_entry){
    		polling_flag_entry->read_proc = polling_flag_read;
    		polling_flag_entry->write_proc = polling_flag_write;
    	}
    }
}

static void epon_polling_dbg_exit(void)
{
    /* Remove proc debug commands */
    if(polling_flag_entry)
    {
    	remove_proc_entry("polling_flag", epon_proc_dir);
        polling_flag_entry = NULL;
    }
    if(epon_proc_dir)
    {
        remove_proc_entry("epon", NULL);
        epon_proc_dir = NULL;
    }
}

int __init epon_poling_init(void)
{
    pEponPollTask = kthread_create(epon_polling_thread, NULL, "epon_polling");
    if(IS_ERR(pEponPollTask))
    {
        printk("%s:%d epon_poling_init failed %ld!\n", __FILE__, __LINE__, PTR_ERR(pEponPollTask));
    }
    else
    {
        wake_up_process(pEponPollTask);
        printk("%s:%d epon_poling_init complete!\n", __FILE__, __LINE__);
    }
    epon_polling_dbg_init();

    return 0;
}

void __exit epon_polling_exit(void)
{
    kthread_stop(pEponPollTask);
    epon_polling_dbg_exit();
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("RealTek EPON polling module");
MODULE_AUTHOR("Elliot Cheng <elliot.cheng@realtek.com>");
module_init(epon_poling_init);
module_exit(epon_polling_exit);
#endif

