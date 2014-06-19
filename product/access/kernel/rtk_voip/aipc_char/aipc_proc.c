#ifdef _AIPC_CPU_
#include <linux/kernel.h>	/* printk() */
#include <linux/errno.h>	/* error codes */
#include <linux/types.h>	/* size_t */
#include <linux/proc_fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>	/* copy_*_user */


#include "./include/dram_share.h"
#include "./include/aipc_dev.h"		/* local definitions */
#include "./include/aipc_reg.h"
#include "./include/soc_type.h"
#include "./include/aipc_mem.h"
#include "./include/aipc_debug.h"

#define PROC_AIPC_DEV_DIR                   DEVICE_NAME

/* switch */
#define PROC_AIPC_DEV_DIR_SWITCH            "switch"
#define PROC_AIPC_DEV_DBG_PRINT             "dbg_print"

/* status */
#define PROC_AIPC_DEV_DIR_STATUS            "status"
#define PROC_AIPC_DEV_IPC_COUNTERS          "ipc_counters"
#define PROC_AIPC_DEV_THREAD          		"thread"
#define PROC_AIPC_DEV_REGISTER          	"register"
#define PROC_AIPC_DEV_SHM                   "shm"

#ifdef CONFIG_RTL8686_IPC_RECORD_DSP_LOG
/* dsp */
#define PROC_AIPC_DEV_DIR_DSP               "dsp"

/* dsp log */
#define PROC_AIPC_DEV_DIR_DSP_LOG           "log"
#define PROC_AIPC_DEV_DSP_LOG_ENABLE        "enable"
#define PROC_AIPC_DEV_DSP_LOG_CLEAR         "clear"
#define PROC_AIPC_DEV_DSP_LOG_CONTENTS      "contents"
#define PROC_AIPC_DEV_DSP_LOG_INDEX         "index"
#endif

#ifdef CONFIG_RTL8686_IPC_DSP_CONSOLE
/* dsp console */
#define PROC_AIPC_DEV_DIR_DSP_CONSOLE           "console"

#define PROC_AIPC_DEV_DSP_CONSOLE_ENABLE        "enable"

#define PROC_AIPC_DEV_DSP_CONSOLE_READ_ENABLE   "read_enable"
#define PROC_AIPC_DEV_DSP_CONSOLE_READ_CLEAR    "read_clear"
#define PROC_AIPC_DEV_DSP_CONSOLE_READ_INDEX    "read_index"

#define PROC_AIPC_DEV_DSP_CONSOLE_WRITE_ENABLE   "write_enable"
#define PROC_AIPC_DEV_DSP_CONSOLE_WRITE_CLEAR    "write_clear"
#define PROC_AIPC_DEV_DSP_CONSOLE_WRITE_INDEX    "write_index"
#endif

/* misc */
#define PROC_AIPC_DEV_DIR_MISC              "misc"
#define PROC_AIPC_DEV_OP                    "operations"
#define PROC_AIPC_DEV_PHYADDR               "phymem_addr"
#define PROC_AIPC_DEV_LOGADDR               "logmem_addr"

static struct proc_dir_entry *proc_aipc_dev_dir ;

extern int aipc_ctrl_dump(char *buf);
extern int aipc_intq_dump(char *buf);
extern int aipc_mbox_dump(char *buf);

#ifdef CONFIG_RTL8686_IPC_RECORD_DSP_LOG
extern unsigned int *rec_dsp_log_enable;
extern unsigned int *rec_dsp_log_ins;
extern unsigned int *rec_dsp_log_del;
extern unsigned int *rec_dsp_log_touch;
extern const char   *rec_dsp_log_contents;
extern int aipc_record_dsp_log_empty(void);
extern int aipc_record_dsp_log_full(void);
extern unsigned int aipc_record_dsp_log_contents_use(void);
typedef int (*ft_aipc_dsp_log_add)(char);
extern ft_aipc_dsp_log_add   fp_aipc_dsp_log_add;
#endif

/*
*	switch related
*/
static int 
proc_switch_dbg_print_w (struct file *file, const char *buffer, unsigned long count, void *data)
{
	char tmpbuf[100] = {0};
	int flag = 0;

	if (buffer && !copy_from_user(tmpbuf, buffer, count)) {

		sscanf(tmpbuf, "%d", &flag);
		
		if (flag == 0) // disable
			ACTSW.dbg_mask = 0;
		
		else if(flag > 0)
			ACTSW.dbg_mask = flag;
		
		else
			SDEBUG("wrong number\n");
	}

	printk("\nDebug mask:\n");
	printk("\tdbg_mask=0x%x\n" , ACTSW.dbg_mask);


	return count;
}

static int 
proc_switch_dbg_print_r (char *buf, char **start, off_t off, int count, int *eof, void *data)

{
    int n = 0;

    if( off ) { /* In our case, we write out all data at once. */
        *eof = 1;
        return 0;
    }


	n += sprintf(buf , "dbg_mask=0x%x\n" , ACTSW.dbg_mask);

    *eof = 1;
	
    return n;
}


/*
*	status related
*/
static int 
proc_status_ipc_counters_w (struct file *file, const char *buffer, unsigned long count, void *data)
{

	char tmpbuf[100] = {0};
	int flag = 0;

	if (buffer && !copy_from_user(tmpbuf, buffer, count)) {

		sscanf(tmpbuf, "%d", &flag);
		
		if (flag == 0){ // reset counters
			memset(&ASTATS ,  0 , sizeof(aipc_stats_t));
			
			printk("Clear status counters\n");
		}
		else
			SDEBUG("wrong number\n");
	}

	return count;
}

static int
aipc_dbg_thread_dump(char *buf)
{
	int n = 0;

	n += sprintf(buf   , "\nThread counters:\n");
	
	n += sprintf(buf+n , "\tctrl_2dsp_counter = %u\n" ,	ATHREAD.ctrl_2dsp_counter);
	n += sprintf(buf+n , "\tctrl_2cpu_counter = %u\n" ,	ATHREAD.ctrl_2cpu_counter);
		
	#ifdef	CONFIG_RTL8686_IPC_TEST_THREAD_2
	n += sprintf(buf+n , "\tctrl_2dsp_counter_2 = %u\n" , ATHREAD.ctrl_2dsp_counter_2);
	n += sprintf(buf+n , "\tctrl_2cpu_counter_2 = %u\n" , ATHREAD.ctrl_2cpu_counter_2);
	#endif
	#ifdef	CONFIG_RTL8686_IPC_TEST_THREAD_3
	n += sprintf(buf+n , "\tctrl_2dsp_counter_3 = %u\n" , ATHREAD.ctrl_2dsp_counter_3);
	n += sprintf(buf+n , "\tctrl_2cpu_counter_3 = %u\n" , ATHREAD.ctrl_2cpu_counter_3);
	#endif

	
	n += sprintf(buf+n , "\tmbox_2dsp_counter = %u\n" ,	ATHREAD.mbox_2dsp_counter);
	n += sprintf(buf+n , "\tmbox_2cpu_counter = %u\n" ,	ATHREAD.mbox_2cpu_counter);
	
	#ifdef	CONFIG_RTL8686_IPC_TEST_THREAD_2
	n += sprintf(buf+n , "\tmbox_2dsp_counter_2 = %u\n" , ATHREAD.mbox_2dsp_counter_2);
	n += sprintf(buf+n , "\tmbox_2cpu_counter_2 = %u\n" , ATHREAD.mbox_2cpu_counter_2);
	#endif

	#ifdef	CONFIG_RTL8686_IPC_TEST_THREAD_3
	n += sprintf(buf+n , "\tmbox_2dsp_counter_3 = %u\n" , ATHREAD.mbox_2dsp_counter_3);
	n += sprintf(buf+n , "\tmbox_2cpu_counter_3 = %u\n" , ATHREAD.mbox_2cpu_counter_3);
	#endif

	#ifdef CONFIG_RTL8686_READ_DRAM_THREAD
	n += sprintf(buf+n , "\tcpu_read_cnt = %u\n" , ATHREAD.cpu_read_cnt);
	n += sprintf(buf+n , "\tdsp_read_cnt = %u\n" , ATHREAD.dsp_read_cnt);
	n += sprintf(buf+n , "\tall_read_cnt = %u\n" , ATHREAD.all_read_cnt);
	#endif

	return n;
}


static int
aipc_dbg_counter_dump(char *buf)
{
	int n = 0;

	n += sprintf(buf , "Statistics:\n");

    /*    
    *    data plane
    */
	n += sprintf(buf+n , "   Data plane \n");
    //    CPU->DSP
	n += sprintf(buf+n , "\t (CPU) data CPU->DSP: \n");
	n += sprintf(buf+n , "\t aipc_data_2dsp_alloc  = %u    BC_2DSP.cnt_del = %u\n"  , ASTATS.aipc_data_2dsp_alloc , BC_2DSP.cnt_del);
	n += sprintf(buf+n , "\t aipc_data_2dsp_send   = %u    MB_2DSP.cnt_ins = %u\n"  , ASTATS.aipc_data_2dsp_send  , MB_2DSP.cnt_ins);
	n += sprintf(buf+n , "\t (DSP) data CPU->DSP: \n");
	n += sprintf(buf+n , "\t aipc_data_2dsp_recv   = %u    MB_2DSP.cnt_del = %u\n"  , ASTATS.aipc_data_2dsp_recv  , MB_2DSP.cnt_del);
	n += sprintf(buf+n , "\t aipc_data_2dsp_ret    = %u    BC_2DSP.cnt_ins = %u\n"  , ASTATS.aipc_data_2dsp_ret   , BC_2DSP.cnt_ins);

    //    CPU<-DSP
	n += sprintf(buf+n , "\n\t (CPU) data CPU<-DSP: \n");
	n += sprintf(buf+n , "\t aipc_data_2cpu_recv   = %u    MB_2CPU.cnt_del = %u\n"  , ASTATS.aipc_data_2cpu_recv  , MB_2CPU.cnt_del);
	n += sprintf(buf+n , "\t aipc_data_2cpu_ret    = %u    BC_2CPU.cnt_ins = %u\n"  , ASTATS.aipc_data_2cpu_ret   , BC_2CPU.cnt_ins);
	n += sprintf(buf+n , "\t (DSP) data CPU<-DSP: \n");
	n += sprintf(buf+n , "\t aipc_data_2cpu_alloc  = %u    BC_2CPU.cnt_del = %u\n"  , ASTATS.aipc_data_2cpu_alloc , BC_2CPU.cnt_del);
	n += sprintf(buf+n , "\t aipc_data_2cpu_send   = %u    MB_2CPU.cnt_ins = %u\n"  , ASTATS.aipc_data_2cpu_send  , MB_2CPU.cnt_ins);

    /*    
    *    control plane plane
    */
	n += sprintf(buf+n , "\n   Control plane \n");    
    //	Control
	n += sprintf(buf+n , "\t (CPU) control CPU->DSP: \n");
	n += sprintf(buf+n , "\t aipc_ctrl_2dsp_nofbk_alloc  = %u  CMD_2DSP.cnt_del = %u\n"  , 
		ASTATS.aipc_ctrl_2dsp_nofbk_alloc ,      CMD_2DSP.cnt_del);
	n += sprintf(buf+n , "\t aipc_ctrl_2dsp_fbk_alloc    = %u  CMD_LOCAL_2DSP.cnt_del = %u\n"  ,
		ASTATS.aipc_ctrl_2dsp_fbk_alloc   ,      CMD_LOCAL_2DSP.cnt_del);

	#ifdef STATS_RETRY 
	n += sprintf(buf+n , "\t aipc_ctrl_2dsp_nofbk_alloc_retry = %u\n" ,
		ASTATS.aipc_ctrl_2dsp_nofbk_alloc_retry);
	n += sprintf(buf+n , "\t aipc_ctrl_2dsp_fbk_alloc_retry = %u\n" ,
		ASTATS.aipc_ctrl_2dsp_fbk_alloc_retry);
	#endif	
	
	n += sprintf(buf+n , "\t aipc_ctrl_2dsp_send         = %u  CMD_QUEUE_2DSP.cnt_ins = %u\n"  , 
		ASTATS.aipc_ctrl_2dsp_send ,             CMD_QUEUE_2DSP.cnt_ins);
   	n += sprintf(buf+n , "\t aipc_ctrl_2dsp_fbk_ret      = %u  CMD_LOCAL_2DSP.cnt_ins = %u\n"  , 
		ASTATS.aipc_ctrl_2dsp_fbk_ret ,          CMD_LOCAL_2DSP.cnt_ins);
	
    n += sprintf(buf+n , "\t (DSP) control CPU->DSP: \n");
    n += sprintf(buf+n , "\t aipc_ctrl_2dsp_recv         = %u  CMD_QUEUE_2DSP.cnt_del = %u\n"  , 
		ASTATS.aipc_ctrl_2dsp_recv ,             CMD_QUEUE_2DSP.cnt_del);
    n += sprintf(buf+n , "\t aipc_ctrl_2dsp_nofbk_ret    = %u  CMD_2DSP.cnt_ins = %u\n" ,    
		ASTATS.aipc_ctrl_2dsp_nofbk_ret ,        CMD_2DSP.cnt_ins );
    n += sprintf(buf+n , "\t aipc_ctrl_2dsp_fbk_fin      = %u  CBUF_2DSP.done_cnt = %u\n"  , 
		ASTATS.aipc_ctrl_2dsp_fbk_fin ,          CBUF_2DSP.done_cnt);

	//	Event
    n += sprintf(buf+n , "\n\t (CPU) event CPU<-DSP: \n");    
    n += sprintf(buf+n , "\t aipc_ctrl_2cpu_recv   = %u\n"  ,    ASTATS.aipc_ctrl_2cpu_recv);
    n += sprintf(buf+n , "\t aipc_ctrl_2cpu_ret    = %u  CBUF_EQ_2CPU.cnt_del = %u\n"  , 
		ASTATS.aipc_ctrl_2cpu_ret ,              CBUF_EQ_2CPU.cnt_del);
    
    n += sprintf(buf+n , "\t (DSP) event CPU<-DSP: \n");
    n += sprintf(buf+n , "\t aipc_ctrl_2cpu_alloc  = %u\n"  ,    ASTATS.aipc_ctrl_2cpu_alloc);

	#ifdef STATS_RETRY 
    n += sprintf(buf+n , "\t aipc_ctrl_2cpu_alloc_retry  = %u\n"  , ASTATS.aipc_ctrl_2cpu_alloc_retry);
	#endif
	
	n += sprintf(buf+n , "\t aipc_ctrl_2cpu_send   = %u  CBUF_EQ_2CPU.cnt_ins = %u\n"  , 
		ASTATS.aipc_ctrl_2cpu_send ,       CBUF_EQ_2CPU.cnt_ins);

    /*
    *    interrupt counters
    */
	n += sprintf(buf+n , "\n   Interrupt counters \n");
	n += sprintf(buf+n , "\t dsp_t_cpu = %u\n" , ASTATS.dsp_t_cpu);
	n += sprintf(buf+n , "\t INT_2CPU_HIQ.cnt_ins  = %u INT_2CPU_HIQ.cnt_del =%u \n"  , 
		       INT_2CPU_HIQ.cnt_ins  ,    INT_2CPU_HIQ.cnt_del);
	n += sprintf(buf+n , "\t INT_2CPU_LOWQ.cnt_ins = %u INT_2CPU_LOWQ.cnt_del =%u \n" , 
		       INT_2CPU_LOWQ.cnt_ins ,    INT_2CPU_LOWQ.cnt_del);

	n += sprintf(buf+n , "\t cpu_t_dsp = %u\n" , ASTATS.cpu_t_dsp);
	n += sprintf(buf+n , "\t INT_2DSP_HIQ.cnt_ins  = %u INT_2DSP_HIQ.cnt_del =%u \n"  , 
		       INT_2DSP_HIQ.cnt_ins  ,    INT_2DSP_HIQ.cnt_del);
	n += sprintf(buf+n , "\t INT_2DSP_LOWQ.cnt_ins = %u INT_2DSP_LOWQ.cnt_del =%u \n" , 
		       INT_2DSP_LOWQ.cnt_ins ,    INT_2DSP_LOWQ.cnt_del);


    /*
    *    shm notify counters
    */
#ifdef CONFIG_RTL8686_SHM_NOTIFY
	n += sprintf(buf+n , "\n   shm notify counters \n");
	n += sprintf(buf+n , "\t shm_notify_cpu = %u shm_notify_dsp = %u\n" , ASTATS.shm_notify_cpu , ASTATS.shm_notify_dsp);
#endif


	/*
	*	 error case
	*/
	n += sprintf(buf+n , "\n   error counters \n");
	n += sprintf(buf+n , "\t aipc_data_error = %u aipc_ctrl_error = %u\n"  , ASTATS.aipc_data_error,ASTATS.aipc_ctrl_error);

	/*
	*	exception case
	*/
	n += sprintf(buf+n , "\n   exception counters \n");
	n += sprintf(buf+n , "\t aipc_ctrl_2cpu_exception_send = %u\n" ,    ASTATS.aipc_ctrl_2cpu_exception_send);

	return n;
}

static int
aipc_thread_switch_dump(char *buf)
{
	int n = 0;
	
	n += sprintf(buf , "\nControl Switch:\n");

//control plane
	//CPU->DSP. RX direction
	n += sprintf(buf+n , "\tctrl_2dsp_send = 0x%x\n" , ATHREAD.ctrl_2dsp_send);
	n += sprintf(buf+n , "\tctrl_2dsp_poll = 0x%x\n" , ATHREAD.ctrl_2dsp_poll);
		
	//CPU<-DSP. TX direction
	n += sprintf(buf+n , "\tctrl_2cpu_send = 0x%x\n" , ATHREAD.ctrl_2cpu_send);
	n += sprintf(buf+n , "\tctrl_2cpu_poll = 0x%x\n" , ATHREAD.ctrl_2cpu_poll);
			
//data plane
	//CPU->DSP. RX direction
	n += sprintf(buf+n , "\tmbox_2dsp_send = 0x%x\n" , ATHREAD.mbox_2dsp_send);
	n += sprintf(buf+n , "\tmbox_2dsp_poll = 0x%x\n" , ATHREAD.mbox_2dsp_poll);
			
	//CPU<-DSP. TX direction
	n += sprintf(buf+n , "\tmbox_2cpu_send = 0x%x\n" , ATHREAD.mbox_2cpu_send);
	n += sprintf(buf+n , "\tmbox_2cpu_recv = 0x%x\n" , ATHREAD.mbox_2cpu_recv);

	return n;
}

static int 
proc_status_ipc_counters_r (char *buf, char **start, off_t off, int count, int *eof, void *data)

{
	int n = 0;

	switch(off){
	case 0:
		n += aipc_mbox_dump(buf);

		n += aipc_intq_dump(buf+n);

		n += aipc_ctrl_dump(buf+n);

		break;
		
	case 1:
		n += aipc_dbg_counter_dump(buf+n);

		break;
	}

	*start = (char *)1; 

	if (n==0)
	    *eof = 1;
	
	return n;
}

static int 
proc_status_thread_w (struct file *file, const char *buffer, unsigned long count, void *data)
{
	char tmpbuf[100] = {0};
	int flag = 0;

	if (buffer && !copy_from_user(tmpbuf, buffer, count)) {

		sscanf(tmpbuf, "%d", &flag);

		if (flag == 0){
			memset(&ATHREAD , 0 , sizeof(aipc_thread_t));
			
			printk("Clear thread counters\n");
		}
		else
			printk("wrong number\n");
	}

	return count;
}

static int 
proc_status_thread_r (char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int n = 0;

    if( off ) { /* In our case, we write out all data at once. */
        *eof = 1;
        return 0;
    }

	n += aipc_thread_switch_dump(buf);
	
	n += aipc_dbg_thread_dump(buf+n);

    *eof = 1;
	
	return n;
}

static int
proc_status_register_w(struct file *file, const char *buffer, unsigned long count, void *data)
{
	return count;
}

static int
proc_status_register_r(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int n = 0;

    if( off ) { /* In our case, we write out all data at once. */
        *eof = 1;
        return 0;
    }

	n += sprintf(buf , "\nMemory Registers:\n");
	
	n += sprintf(buf+n , "\tC0DOR0  = 0x%x\n" , REG32(C0DOR0));
	n += sprintf(buf+n , "\tC0DMAR0 = 0x%x\n" , REG32(C0DMAR0));
	n += sprintf(buf+n , "\tC0DOR1  = 0x%x\n" , REG32(C0DOR1));
	n += sprintf(buf+n , "\tC0DMAR1 = 0x%x\n" , REG32(C0DMAR1));
	n += sprintf(buf+n , "\tC0DOR2  = 0x%x\n" , REG32(C0DOR2));
	n += sprintf(buf+n , "\tC0DMAR2 = 0x%x\n" , REG32(C0DMAR2));
	n += sprintf(buf+n , "\tC0RCR   = 0x%x\n" , REG32(C0RCR));
	n += sprintf(buf+n , "\tC0ILAR  = 0x%x\n" , REG32(C0ILAR));

	n += sprintf(buf+n , "\tC1DOR0  = 0x%x\n" , REG32(C1DOR0));
	n += sprintf(buf+n , "\tC1DMAR0 = 0x%x\n" , REG32(C1DMAR0));
	n += sprintf(buf+n , "\tC1DOR1  = 0x%x\n" , REG32(C1DOR1));
	n += sprintf(buf+n , "\tC1DMAR1 = 0x%x\n" , REG32(C1DMAR1));
	n += sprintf(buf+n , "\tC1DOR2  = 0x%x\n" , REG32(C1DOR2));
	n += sprintf(buf+n , "\tC1DMAR2 = 0x%x\n" , REG32(C1DMAR2));
	n += sprintf(buf+n , "\tC1RCR   = 0x%x\n" , REG32(C1RCR));
	n += sprintf(buf+n , "\tC1ILAR  = 0x%x\n" , REG32(C1ILAR));

	*eof = 1;

	return n;
}

static int
proc_status_shm_w(struct file *file, const char *buffer, unsigned long count, void *data)
{
	return count;
}

static int
proc_status_shm_r(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int n = 0;
#ifdef CONFIG_RTL8686_SHM_NOTIFY
	extern const unsigned int *aipc_shm_notify_cpu;
	extern const unsigned int *aipc_shm_notify_dsp;
#endif

    if( off ) { /* In our case, we write out all data at once. */
        *eof = 1;
        return 0;
    }

	n += sprintf(buf , "\nShared Memory Status:\n");
	
	n += sprintf(buf+n , "\nShared Memory Data Size:\n");
	n += sprintf(buf+n , "\t aipc_dram_t size  = 0x%x\n" , sizeof(aipc_dram_t));
	n += sprintf(buf+n , "\t aipc_sram_t size  = 0x%x\n" , sizeof(aipc_sram_t));

#ifdef CONFIG_RTL8686_SHM_NOTIFY
	n += sprintf(buf+n , "\nShared Memory Notification:\n");
	n += sprintf(buf+n , "\t aipc_shm_notify_cpu  = 0x%p\n" , aipc_shm_notify_cpu);
	n += sprintf(buf+n , "\t aipc_shm_notify_dsp  = 0x%p\n" , aipc_shm_notify_dsp);
#endif

	*eof = 1;

	return n;
}

#ifdef CONFIG_RTL8686_IPC_RECORD_DSP_LOG
static int
proc_dsp_log_enable_w (struct file *file, const char *buffer, unsigned long count, void *data)
{
	char tmpbuf[100];
	int flag = 0;

	if (buffer && !copy_from_user(tmpbuf, buffer, count)) {

		sscanf(tmpbuf, "%d", &flag);

		if (flag == RECORD_DSP_LOG_DISABLE){
			*rec_dsp_log_enable = RECORD_DSP_LOG_DISABLE;
			printk("Disable record dsp log\n");
		}
		else if (flag == RECORD_DSP_LOG_ENABLE_SAVE_HISTORY){
			*rec_dsp_log_enable = RECORD_DSP_LOG_ENABLE_SAVE_HISTORY;
			printk("Enable record dsp log and save history\n");			
		}
		else if (flag == RECORD_DSP_LOG_ENABLE_OVERWRITE_HISTORY){
			*rec_dsp_log_enable = RECORD_DSP_LOG_ENABLE_OVERWRITE_HISTORY;
			printk("Enable record dsp log and overwrite history\n");			
		}
		else
			printk("wrong number\n");
	}

	return count;
}

static int
proc_dsp_log_enable_r (char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int n = 0;

    if( off ) { /* In our case, we write out all data at once. */
        *eof = 1;
        return 0;
    }

	n += sprintf(buf , "record dsp log = %d\n" , *rec_dsp_log_enable);	
	
    *eof = 1;

	return n;
}

static int
proc_dsp_log_clear_w (struct file *file, const char *buffer, unsigned long count, void *data)
{
	char tmpbuf[100];
	int flag = 0;

	if (buffer && !copy_from_user(tmpbuf, buffer, count)) {

		sscanf(tmpbuf, "%d", &flag);

		if (flag == 0){
			*rec_dsp_log_del   = *rec_dsp_log_ins;
			*rec_dsp_log_touch = 0;
			
			printk("Clear record dsp log\n");
		}
		else
			printk("wrong number\n");
	}
	
	return count;
}

static int
proc_dsp_log_clear_r (char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int n = 0;

    *eof = 1;

	return n;
}

static int
proc_dsp_log_contents_w (struct file *file, const char *buffer, unsigned long count, void *data)
{
	return count;
}

static int
proc_dsp_log_contents_r (char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int n = 0;
	static unsigned int dsp_log_del = 0;
	static int remain = 0;

	if (*rec_dsp_log_enable == RECORD_DSP_LOG_DISABLE) {
        *eof = 1;
        n += sprintf( buf , "Record DSP log is disabled\n" );
        return n;
    }

	if (aipc_record_dsp_log_empty()) {
        *eof = 1;
        n += sprintf( buf , "Record DSP log is empty\n" );
        return n;
    }
    
    if (off==0){
    	dsp_log_del = *rec_dsp_log_del;
		remain = aipc_record_dsp_log_contents_use();
    }

	while( remain > 0 ){
		buf[n] = rec_dsp_log_contents[ dsp_log_del ];
		n++;
		dsp_log_del = (dsp_log_del+1) % RECORD_DSP_LOG_SIZE;
		remain--;

		if (n >= count)
			break;
	}

	*start = (char *)1;

	if (n==0)
	    *eof = 1;	

	//printk("count = %d remain = %u n = %d\n" , count , remain , n);

	return n;
}

static int
proc_dsp_log_index_w (struct file *file, const char *buffer, unsigned long count, void *data)
{
	return count;
}

static int
proc_dsp_log_index_r (char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int n = 0;
	int full  = 0 ;
	int empty = 0;
	unsigned int use  = 0;
	unsigned int enable  = 0;	

    if( off ) { /* In our case, we write out all data at once. */
        *eof = 1;
        return 0;
    }
	
	use    = aipc_record_dsp_log_contents_use();
	full   = aipc_record_dsp_log_full();
	empty  = aipc_record_dsp_log_empty();
	enable = *rec_dsp_log_enable;
	
	n += sprintf(buf , "\nIndex:\n");
	
	n += sprintf(buf+n , "\t enable      = %u "       , enable);
	if (enable == RECORD_DSP_LOG_DISABLE)
		n += sprintf(buf+n , "   %s\n"      , "disable");
	else if (enable == RECORD_DSP_LOG_ENABLE_SAVE_HISTORY)
		n += sprintf(buf+n , "   %s\n"      , "save history");
	else if (enable == RECORD_DSP_LOG_ENABLE_OVERWRITE_HISTORY)
		n += sprintf(buf+n , "   %s\n"      , "overwrite history");
	else 
		n += sprintf(buf+n , "   %s\n"      , "others");
	
	n += sprintf(buf+n , "\t touch       = %u\n"       , *rec_dsp_log_touch);
	n += sprintf(buf+n , "\t ins         = %u\n"       , *rec_dsp_log_ins);
	n += sprintf(buf+n , "\t del         = %u\n"       , *rec_dsp_log_del);
	n += sprintf(buf+n , "\t use         = %u\n"       ,  use);
	
	if (full) {
		n += sprintf(buf+n , "\t full        = 1\n");
	}
	else if (empty) {
		n += sprintf(buf+n , "\t empty       = 1\n");
	}
	else if (use) {
		n += sprintf(buf+n , "\t middle      = 1\n");
	}
	else {
		n += sprintf(buf+n , "\t unexpected queue status \n");
	}
	
	n += sprintf(buf+n , "\t conts addr  = %p\n"       ,  rec_dsp_log_contents);
	n += sprintf(buf+n , "\t log size    = %u\n"       ,  RECORD_DSP_LOG_SIZE);

	*eof = 1;
	return n;
}

#endif

#ifdef CONFIG_RTL8686_IPC_DSP_CONSOLE
extern          int aipc_dsp_console_read_buf_full(void);
extern          int aipc_dsp_console_read_buf_empty(void);
extern unsigned int aipc_dsp_console_read_buf_use(void);

extern          int aipc_dsp_console_write_buf_full(void);
extern          int aipc_dsp_console_write_buf_empty(void);
extern unsigned int aipc_dsp_console_write_buf_use(void);  


static int
proc_dsp_console_enable_w (struct file *file, const char *buffer, unsigned long count, void *data)
{
	char tmpbuf[100];
	int flag = 0;

	if (buffer && !copy_from_user(tmpbuf, buffer, count)) {

		sscanf(tmpbuf, "%d", &flag);

		if (flag == 0){
			ADSP_CONSOLE.enable = 0;
			printk("disable dsp console\n");
		}
		else if (flag == 1){
			ADSP_CONSOLE.enable = 1;
			printk("ensable dsp console\n");			
		}
		else
			printk("wrong number\n");
	}

	return count;
}

static int
proc_dsp_console_enable_r (char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int n = 0;

    if( off ) { /* In our case, we write out all data at once. */
        *eof = 1;
        return 0;
    }

	n += sprintf(buf , "dsp console enable = %d\n" , ADSP_CONSOLE.enable);	
	
    *eof = 1;

	return n;
}


/*
*	read queue
*/
static int
proc_dsp_console_read_queue_enable_w (struct file *file, const char *buffer, unsigned long count, void *data)
{
	char tmpbuf[100];
	int flag = 0;

	if (buffer && !copy_from_user(tmpbuf, buffer, count)) {

		sscanf(tmpbuf, "%d", &flag);

		if (flag == 0){
			ADSP_CONSOLE_RD.enable = 0;
			printk("disable dsp read queue\n");
		}
		else if (flag == 1){
			ADSP_CONSOLE_RD.enable = 1;
			printk("ensable dsp read queue\n");			
		}
		else
			printk("wrong number\n");
	}

	return count;
}

static int
proc_dsp_console_read_queue_enable_r (char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int n = 0;

    if( off ) { /* In our case, we write out all data at once. */
        *eof = 1;
        return 0;
    }

	n += sprintf(buf , "dsp read queue enable = %d\n" , ADSP_CONSOLE_RD.enable);	
	
    *eof = 1;

	return n;
}

static int
proc_dsp_console_read_queue_index_w (struct file *file, const char *buffer, unsigned long count, void *data)
{
	return count;
}

static int
proc_dsp_console_read_queue_index_r (char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int n = 0;
	unsigned int use  = 0;
	int full  = 0 ;
	int empty = 0;

    if( off ) { /* In our case, we write out all data at once. */
        *eof = 1;
        return 0;
    }
	
	use   = aipc_dsp_console_read_buf_use();
	full  = aipc_dsp_console_read_buf_full();
	empty = aipc_dsp_console_read_buf_empty();
	
	n += sprintf(buf , "\nIndex:\n");
	
	n += sprintf(buf+n , "\t enable      = %u\n"       ,  ADSP_CONSOLE_RD.enable);
	n += sprintf(buf+n , "\t eq_try      = %u\n"       ,  ADSP_CONSOLE_RD.eq_try);
	n += sprintf(buf+n , "\t eq_ok       = %u\n"       ,  ADSP_CONSOLE_RD.eq_ok);
	n += sprintf(buf+n , "\t dq_try      = %u\n"       ,  ADSP_CONSOLE_RD.dq_try);
	n += sprintf(buf+n , "\t dq_ok       = %u\n"       ,  ADSP_CONSOLE_RD.dq_ok);
	n += sprintf(buf+n , "\t ins         = %u\n"       ,  ADSP_CONSOLE_RD.ins);
	n += sprintf(buf+n , "\t del         = %u\n"       ,  ADSP_CONSOLE_RD.del);
	n += sprintf(buf+n , "\t use         = %u\n"       ,  use);
	
	if (full) {
		n += sprintf(buf+n , "\t full        = 1\n");
	}
	else if (empty) {
		n += sprintf(buf+n , "\t empty       = 1\n");
	}
	else if (use) {
		n += sprintf(buf+n , "\t middle      = 1\n");
	}
	else {
		n += sprintf(buf+n , "\t unexpected queue status \n");
	}
	
	n += sprintf(buf+n , "\t buf addr   = %p\n"       ,  &ADSP_CONSOLE_RD.read_buf);
	n += sprintf(buf+n , "\t queue size = %u\n"       ,  DSP_CONSOLE_READ_BUF_SIZE);

	*eof = 1;
	return n;
}

static int
proc_dsp_console_read_queue_clear_w (struct file *file, const char *buffer, unsigned long count, void *data)
{
	char tmpbuf[100];
	int flag = 0;

	if (buffer && !copy_from_user(tmpbuf, buffer, count)) {

		sscanf(tmpbuf, "%d", &flag);

		if (flag == 0){
			ADSP_CONSOLE_RD.del   = ADSP_CONSOLE_RD.ins;
			ADSP_CONSOLE_RD.eq_try  = 0;
			ADSP_CONSOLE_RD.eq_ok   = 0;
			ADSP_CONSOLE_RD.dq_try  = 0;
			ADSP_CONSOLE_RD.dq_ok   = 0;
			
			printk("clear dsp console read queue\n");
		}
		else
			printk("wrong number\n");
	}
	
	return count;
}

static int
proc_dsp_console_read_queue_clear_r (char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int n = 0;

    *eof = 1;

	return n;
}

/*
*	write queue
*/
static int
proc_dsp_console_write_queue_enable_w (struct file *file, const char *buffer, unsigned long count, void *data)
{
	char tmpbuf[100];
	int flag = 0;

	if (buffer && !copy_from_user(tmpbuf, buffer, count)) {

		sscanf(tmpbuf, "%d", &flag);

		if (flag == 0){
			ADSP_CONSOLE_WR.enable = 0;
			printk("disable dsp write queue\n");
		}
		else if (flag == 1){
			ADSP_CONSOLE_WR.enable = 1;
			printk("ensable dsp write queue\n");			
		}
		else
			printk("wrong number\n");
	}

	return count;
}

static int
proc_dsp_console_write_queue_enable_r (char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int n = 0;

    if( off ) { /* In our case, we write out all data at once. */
        *eof = 1;
        return 0;
    }

	n += sprintf(buf , "dsp write queue enable = %d\n" , ADSP_CONSOLE_WR.enable);	
	
    *eof = 1;

	return n;
}

static int
proc_dsp_console_write_queue_index_w (struct file *file, const char *buffer, unsigned long count, void *data)
{
	return count;
}

static int
proc_dsp_console_write_queue_index_r (char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int n = 0;
	unsigned int use  = 0;
	int full  = 0 ;
	int empty = 0;

    if( off ) { /* In our case, we write out all data at once. */
        *eof = 1;
        return 0;
    }
	
	use   = aipc_dsp_console_write_buf_use();
	full  = aipc_dsp_console_write_buf_full();
	empty = aipc_dsp_console_write_buf_empty();
	
	n += sprintf(buf , "\nIndex:\n");
	
	n += sprintf(buf+n , "\t enable      = %u\n"       ,  ADSP_CONSOLE_WR.enable);
	n += sprintf(buf+n , "\t eq_try      = %u\n"       ,  ADSP_CONSOLE_WR.eq_try);
	n += sprintf(buf+n , "\t eq_ok       = %u\n"       ,  ADSP_CONSOLE_WR.eq_ok);
	n += sprintf(buf+n , "\t dq_try      = %u\n"       ,  ADSP_CONSOLE_WR.dq_try);
	n += sprintf(buf+n , "\t dq_ok       = %u\n"       ,  ADSP_CONSOLE_WR.dq_ok);
	n += sprintf(buf+n , "\t ins         = %u\n"       ,  ADSP_CONSOLE_WR.ins);
	n += sprintf(buf+n , "\t del         = %u\n"       ,  ADSP_CONSOLE_WR.del);
	n += sprintf(buf+n , "\t use         = %u\n"       ,  use);
	
	if (full) {
		n += sprintf(buf+n , "\t full        = 1\n");
	}
	else if (empty) {
		n += sprintf(buf+n , "\t empty       = 1\n");
	}
	else if (use) {
		n += sprintf(buf+n , "\t middle      = 1\n");
	}
	else {
		n += sprintf(buf+n , "\t unexpected queue status \n");
	}
	
	n += sprintf(buf+n , "\t buf addr   = %p\n"       ,  &ADSP_CONSOLE_WR.write_buf);
	n += sprintf(buf+n , "\t queue size = %u\n"       ,  DSP_CONSOLE_WRITE_BUF_SIZE);

	*eof = 1;
	return n;

}

static int
proc_dsp_console_write_queue_clear_w (struct file *file, const char *buffer, unsigned long count, void *data)
{
	char tmpbuf[100];
	int flag = 0;

	if (buffer && !copy_from_user(tmpbuf, buffer, count)) {

		sscanf(tmpbuf, "%d", &flag);

		if (flag == 0){
			ADSP_CONSOLE_WR.del   = ADSP_CONSOLE_WR.ins;
			ADSP_CONSOLE_WR.eq_try  = 0;
			ADSP_CONSOLE_WR.eq_ok   = 0;
			ADSP_CONSOLE_WR.dq_try  = 0;
			ADSP_CONSOLE_WR.dq_ok   = 0;
			
			printk("clear dsp console write queue\n");
		}
		else
			printk("wrong number\n");
	}
	
	return count;
}

static int
proc_dsp_console_write_queue_clear_r (char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int n = 0;

    *eof = 1;

	return n;
}
#endif

#if 0
static int 
proc_read_phymem_addr(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	return sprintf(buf, "%08lx\n", __pa(dst_addr));
}

static int 
proc_read_logmem_addr(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	return sprintf(buf, "%08lx\n", (ul32_t)dst_addr);
}

static struct file_operations aipc_dev_proc_ops = {
    .owner   = THIS_MODULE,
    .open    = NULL,
    .read    = NULL,
	.write	 = NULL,
    .llseek  = NULL,
    .release = NULL
};
#endif


void 
aipc_dev_create_proc(void)
{
	struct proc_dir_entry *entry;
	
	/*
	*	create root directory
	*/
	proc_aipc_dev_dir = proc_mkdir(PROC_AIPC_DEV_DIR, NULL);
	if (proc_aipc_dev_dir == NULL) {
		printk("create proc root failed!\n");
		return;
	}

	/*
	*	create switch directory
	*/
	proc_aipc_dev_dir = proc_mkdir(PROC_AIPC_DEV_DIR "/" PROC_AIPC_DEV_DIR_SWITCH , NULL);
	if (proc_aipc_dev_dir == NULL) {
		printk("create proc aipc_dev/switch failed!\n");
		return;
	}

	if((entry = create_proc_entry(PROC_AIPC_DEV_DBG_PRINT, 0644, proc_aipc_dev_dir)) == NULL) {
		printk("create proc aipc_dev/switch/dbg_print failed!\n");
		return;
	}
	entry->write_proc = proc_switch_dbg_print_w;
	entry->read_proc  = proc_switch_dbg_print_r;


	/*
	*	create status directory
	*/
	proc_aipc_dev_dir = proc_mkdir(PROC_AIPC_DEV_DIR "/" PROC_AIPC_DEV_DIR_STATUS, NULL);
	if (proc_aipc_dev_dir == NULL) {
		printk("create proc aipc_dev/status failed!\n");
		return;
	}

	if((entry = create_proc_entry(PROC_AIPC_DEV_IPC_COUNTERS, 0644, proc_aipc_dev_dir)) == NULL) {
		printk("create proc aipc_dev/status/ipc_counters failed!\n");
		return;
	}
	entry->write_proc = proc_status_ipc_counters_w;
	entry->read_proc  = proc_status_ipc_counters_r;

	if((entry = create_proc_entry(PROC_AIPC_DEV_THREAD, 0644, proc_aipc_dev_dir)) == NULL) {
		printk("create proc aipc_dev/status/thread failed!\n");
		return;
	}
	entry->write_proc = proc_status_thread_w;
	entry->read_proc  = proc_status_thread_r;

	if((entry = create_proc_entry(PROC_AIPC_DEV_REGISTER, 0644, proc_aipc_dev_dir)) == NULL) {
		printk("create proc aipc_dev/status/register failed!\n");
		return;
	}
	entry->write_proc = proc_status_register_w;
	entry->read_proc  = proc_status_register_r;

	if((entry = create_proc_entry(PROC_AIPC_DEV_SHM, 0644, proc_aipc_dev_dir)) == NULL) {
		printk("create proc aipc_dev/status/shm failed!\n");
		return;
	}
	entry->write_proc = proc_status_shm_w;
	entry->read_proc  = proc_status_shm_r;

#ifdef CONFIG_RTL8686_IPC_RECORD_DSP_LOG
	/*
	*	create dsp/log directory
	*/
	proc_aipc_dev_dir = proc_mkdir(PROC_AIPC_DEV_DIR "/" PROC_AIPC_DEV_DIR_DSP , NULL);
	if (proc_aipc_dev_dir == NULL) {
		printk("create proc aipc_dev/dsp failed!\n");
		return;
	}
	
	proc_aipc_dev_dir = proc_mkdir(PROC_AIPC_DEV_DIR "/" PROC_AIPC_DEV_DIR_DSP "/" PROC_AIPC_DEV_DIR_DSP_LOG , NULL);
	if (proc_aipc_dev_dir == NULL) {
		printk("create proc aipc_dev/dsp/log failed!\n");
		return;
	}
	
	if((entry = create_proc_entry(PROC_AIPC_DEV_DSP_LOG_ENABLE, 0644, proc_aipc_dev_dir)) == NULL) {
		printk("create proc aipc_dev/dsp/log/enable failed!\n");
		return;
	}
	entry->write_proc = proc_dsp_log_enable_w;
	entry->read_proc  = proc_dsp_log_enable_r;

	if((entry = create_proc_entry(PROC_AIPC_DEV_DSP_LOG_CLEAR, 0644, proc_aipc_dev_dir)) == NULL) {
		printk("create proc aipc_dev/dsp/log/clear failed!\n");
		return;
	}
	entry->write_proc = proc_dsp_log_clear_w;
	entry->read_proc  = proc_dsp_log_clear_r;

	if((entry = create_proc_entry(PROC_AIPC_DEV_DSP_LOG_CONTENTS, 0644, proc_aipc_dev_dir)) == NULL) {
		printk("create proc aipc_dev/dsp/log/contents failed!\n");
		return;
	}
	entry->write_proc = proc_dsp_log_contents_w;
	entry->read_proc  = proc_dsp_log_contents_r;

	if((entry = create_proc_entry(PROC_AIPC_DEV_DSP_LOG_INDEX, 0644, proc_aipc_dev_dir)) == NULL) {
		printk("create proc aipc_dev/dsp/log/index failed!\n");
		return;
	}
	entry->write_proc = proc_dsp_log_index_w;
	entry->read_proc  = proc_dsp_log_index_r;
#endif

#ifdef CONFIG_RTL8686_IPC_DSP_CONSOLE
	/*
	*	create dsp/console directory
	*/
	proc_aipc_dev_dir = proc_mkdir(PROC_AIPC_DEV_DIR "/" PROC_AIPC_DEV_DIR_DSP "/" PROC_AIPC_DEV_DIR_DSP_CONSOLE , NULL);
	if (proc_aipc_dev_dir == NULL) {
		printk("create proc aipc_dev/dsp/console failed!\n");
		return;
	}

	if((entry = create_proc_entry(PROC_AIPC_DEV_DSP_CONSOLE_ENABLE, 0644, proc_aipc_dev_dir)) == NULL) {
		printk("create proc aipc_dev/dsp/enable failed!\n");
		return;
	}
	entry->write_proc = proc_dsp_console_enable_w;
	entry->read_proc  = proc_dsp_console_enable_r;	

	/*
	*	read queue
	*/	
	if((entry = create_proc_entry(PROC_AIPC_DEV_DSP_CONSOLE_READ_ENABLE, 0644, proc_aipc_dev_dir)) == NULL) {
		printk("create proc aipc_dev/dsp/log/read_enable failed!\n");
		return;
	}
	entry->write_proc = proc_dsp_console_read_queue_enable_w;
	entry->read_proc  = proc_dsp_console_read_queue_enable_r;

	if((entry = create_proc_entry(PROC_AIPC_DEV_DSP_CONSOLE_READ_CLEAR, 0644, proc_aipc_dev_dir)) == NULL) {
		printk("create proc aipc_dev/dsp/log/read_clear failed!\n");
		return;
	}
	entry->write_proc = proc_dsp_console_read_queue_clear_w;
	entry->read_proc  = proc_dsp_console_read_queue_clear_r;

	if((entry = create_proc_entry(PROC_AIPC_DEV_DSP_CONSOLE_READ_INDEX, 0644, proc_aipc_dev_dir)) == NULL) {
		printk("create proc aipc_dev/dsp/log/index failed!\n");
		return;
	}
	entry->write_proc = proc_dsp_console_read_queue_index_w;
	entry->read_proc  = proc_dsp_console_read_queue_index_r;

	/*
	*	write queue
	*/	
	if((entry = create_proc_entry(PROC_AIPC_DEV_DSP_CONSOLE_WRITE_ENABLE, 0644, proc_aipc_dev_dir)) == NULL) {
		printk("create proc aipc_dev/dsp/log/write_enable failed!\n");
		return;
	}
	entry->write_proc = proc_dsp_console_write_queue_enable_w;
	entry->read_proc  = proc_dsp_console_write_queue_enable_r;

	if((entry = create_proc_entry(PROC_AIPC_DEV_DSP_CONSOLE_WRITE_CLEAR, 0644, proc_aipc_dev_dir)) == NULL) {
		printk("create proc aipc_dev/dsp/log/write_clear failed!\n");
		return;
	}
	entry->write_proc = proc_dsp_console_write_queue_clear_w;
	entry->read_proc  = proc_dsp_console_write_queue_clear_r;

	if((entry = create_proc_entry(PROC_AIPC_DEV_DSP_CONSOLE_WRITE_INDEX, 0644, proc_aipc_dev_dir)) == NULL) {
		printk("create proc aipc_dev/dsp/log/write_index failed!\n");
		return;
	}
	entry->write_proc = proc_dsp_console_write_queue_index_w;
	entry->read_proc  = proc_dsp_console_write_queue_index_r;
#endif


	/*
	*	create misc directory
	*/
#if 0
	proc_aipc_dev_dir = proc_mkdir(PROC_AIPC_DEV_DIR "/" PROC_AIPC_DEV_DIR_MISC , NULL);
	if (proc_aipc_dev_dir == NULL) {
		printk("create proc aipc_dev/misc failed!\n");
		return;
	}

	if((entry = create_proc_entry(PROC_AIPC_DEV_OP, 0644, proc_aipc_dev_dir)) == NULL){
		printk("create proc aipc_dev/misc/operations failed!\n");
		return;
	}
	entry->proc_fops = &aipc_dev_proc_ops;

	create_proc_read_entry( PROC_AIPC_DEV_PHYADDR , 0644 , 
		proc_aipc_dev_dir , proc_read_phymem_addr , NULL );
	create_proc_read_entry( PROC_AIPC_DEV_LOGADDR , 0644 , 
		proc_aipc_dev_dir , proc_read_logmem_addr , NULL );
#endif
}

#endif

