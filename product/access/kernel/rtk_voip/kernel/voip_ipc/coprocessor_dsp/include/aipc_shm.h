#ifndef __AIPC_SHM_H__
#define __AIPC_SHM_H__

#include "aipc_global.h"
#include "soc_type.h"
#include "aipc_define.h"
#include "aipc_buffer.h"
#include "aipc_ioctl.h"
#ifdef AIPC_GOT
#include "aipc_got.h"
#endif

/*****************************************************************************
*   Data Plane Define
*****************************************************************************/
/*
*	Mailbox related define
*/
//	CPU -> DSP
#if 1	//this mechanism could need more memory space. use union can save more

#define MAIL_2DSP_TOTAL 	(64*4)	//jitter buffer*active sessions+1. left 1 for circle queue

#define MB_2DSP_TOTAL (MAIL_2DSP_TOTAL+1)
#define BC_2DSP_TOTAL (MAIL_2DSP_TOTAL+1)


typedef struct _mail_2dsp{
	u8_t	m[MAIL_2DSP_TOTAL][MAIL_2DSP_SIZE];
} mail_2dsp_t;

#else	//can save more space using the following union mechanism

typedef struct g711u g711u_t;
typedef struct g711a g711a_t;
typedef struct g729  g729_t;
typedef struct g7222 g7222_t;

typedef union _mail_2dsp{
	g711u_t m[MAIL_2DSP_TOTAL];
	g711a_t n[MAIL_2DSP_TOTAL];
	g729_t  o[MAIL_2DSP_TOTAL];
	g7222_t p[MAIL_2DSP_TOTAL];
} mail_2dsp_t;

#endif

typedef struct _mb_2dsp{
	void * 				mb[MB_2DSP_TOTAL];	//CPU -> DSP Mailbox Array
	volatile u32_t		ins;
	volatile u32_t		del;
#ifdef QUEUE_OPERATION_CNT
	volatile u32_t		cnt_ins;
	volatile u32_t		cnt_del;
#endif
} mb_2dsp_t;

typedef struct _bc_2dsp{
	void * 				bc[BC_2DSP_TOTAL];	//CPU -> DSP Buffer Circulation Array
	volatile u32_t		ins;
	volatile u32_t		del;
#ifdef QUEUE_OPERATION_CNT
	volatile u32_t		cnt_ins;
	volatile u32_t		cnt_del;
#endif
} bc_2dsp_t;

typedef struct _mbox_2dsp{
	mb_2dsp_t 			mb;
	bc_2dsp_t			bc;
#ifdef QUEUE_OPERATION_CNT
	volatile u32_t		cnt_ins;
	volatile u32_t		cnt_del;
#endif
} mbox_2dsp_t;

//	CPU <- DSP
#define MAIL_2CPU_TOTAL		(8)		//Active session number. left 1 for circle queue

#define MB_2CPU_TOTAL (MAIL_2CPU_TOTAL+1)
#define BC_2CPU_TOTAL (MAIL_2CPU_TOTAL+1)

typedef struct _mail_2cpu{
	u8_t	m[MAIL_2CPU_TOTAL][MAIL_2CPU_SIZE];
} mail_2cpu_t;

typedef struct _mb_2cpu{
	void * 				mb[MB_2CPU_TOTAL];//CPU <- DSP Mailbox Array
	volatile u32_t		ins;
	volatile u32_t		del;
#ifdef QUEUE_OPERATION_CNT
	volatile u32_t		cnt_ins;
	volatile u32_t		cnt_del;
#endif
} mb_2cpu_t;

typedef struct _bc_2cpu{
	void * 				bc[BC_2CPU_TOTAL];//CPU <- DSP Buffer Circulation Array
	volatile u32_t		ins;
	volatile u32_t		del;
#ifdef QUEUE_OPERATION_CNT
	volatile u32_t		cnt_ins;
	volatile u32_t		cnt_del;
#endif
}bc_2cpu_t;

typedef struct _mbox_2cpu{
	mb_2cpu_t 			mb;
	bc_2cpu_t 			bc;
#ifdef QUEUE_OPERATION_CNT
	volatile u32_t		cnt_ins;
	volatile u32_t		cnt_del;
#endif
} mbox_2cpu_t;

/*
*	AIPC interrupt related define
*/
#define SIZE_2DSP_HIQ 		64
#define SIZE_2DSP_LOWQ 		64
#define SIZE_2CPU_HIQ		64
#define SIZE_2CPU_LOWQ		64

typedef struct _int_2dsp_hiq{
	volatile u32_t 	hiq[SIZE_2DSP_HIQ]; 	//Interrupt High Priority Queue
	volatile u32_t	ins;
	volatile u32_t	del;
#ifdef QUEUE_OPERATION_CNT
	volatile u32_t	cnt_ins;
	volatile u32_t	cnt_del;
#endif
} int_2dsp_hiq_t;

typedef struct _int_2dsp_lowq{
	volatile u32_t 	lowq[SIZE_2DSP_LOWQ];	//Interrupt Low Priority Queue
	volatile u32_t	ins;
	volatile u32_t	del;
#ifdef QUEUE_OPERATION_CNT
	volatile u32_t	cnt_ins;
	volatile u32_t	cnt_del;
#endif
}  int_2dsp_lowq_t;

//	AIPC Interrupt Queue CPU -> DSP
typedef struct _intq_2dsp{
	int_2dsp_hiq_t 	hiq;
	int_2dsp_lowq_t	lowq;
} intq_2dsp_t;


typedef struct _int_2cpu_hiq{
	volatile u32_t 	hiq[SIZE_2CPU_HIQ]; 	//Interrupt High Priority Queue
	volatile u32_t	ins;
	volatile u32_t	del;
#ifdef QUEUE_OPERATION_CNT
	volatile u32_t	cnt_ins;
	volatile u32_t	cnt_del;
#endif
} int_2cpu_hiq_t;

typedef struct _int_2cpu_lowq{
	volatile u32_t 	lowq[SIZE_2CPU_LOWQ];	//Interrupt Low Priority Queue
	volatile u32_t	ins;
	volatile u32_t	del;
#ifdef QUEUE_OPERATION_CNT
	volatile u32_t	cnt_ins;
	volatile u32_t	cnt_del;
#endif
}  int_2cpu_lowq_t;

//	AIPC Interrupt Queue CPU <- DSP
typedef struct _intq_2cpu{
	int_2cpu_hiq_t 	hiq;
	int_2cpu_lowq_t	lowq;
} intq_2cpu_t;

/*
*	Data plane related structure
*/
//add boot instruction
typedef struct _aipc_boot{
	u32_t cmd[4];	//for boot
} aipc_boot_t; 

//	Data plane related shared memory structure
typedef struct _aipc_mbox{
	mbox_2dsp_t	mbox_2dsp;			//CPU -> DSP mailbox
	mbox_2cpu_t	mbox_2cpu;			//CPU <- DSP mailbox
} aipc_mbox_t; 

typedef struct _aipc_mail{
	mail_2dsp_t	mail_2dsp;			//CPU ->  DSP mail
	mail_2cpu_t	mail_2cpu;			//CPU <-  DSP mail
} aipc_mail_t; 

typedef struct _aipc_intq{
	intq_2dsp_t	intq_2dsp; 			//CPU -> DSP interrupt queue
	intq_2cpu_t	intq_2cpu; 			//CPU <- DSP interrupt queue
} aipc_intq_t; 

/*****************************************************************************
*   Control Plane Define
*****************************************************************************/
/*
*	CPU -> DSP
*/
#define CMD_PHY_TOTAL			8
#define CMD_LOCAL_PHY_TOTAL		4

#define CMD_BUF_TOTAL			(CMD_PHY_TOTAL+1)
#define CMD_LOCAL_BUF_TOTAL		(CMD_LOCAL_PHY_TOTAL+1)

#define CMD_QUEUE_TOTAL			(CMD_PHY_TOTAL+CMD_LOCAL_PHY_TOTAL+1)


// Command element
typedef struct{
	u8_t					ce[CMD_SIZE];
} cmd_element_t;

typedef struct{
	u8_t					cle[CMD_SIZE];
	volatile u32_t			done;
} cmd_local_element_t;


// Command buffer queue
typedef struct{
	void *					cb[CMD_BUF_TOTAL];
	volatile u32_t			ins;
	volatile u32_t			del;
#ifdef QUEUE_OPERATION_CNT
	volatile u32_t			cnt_ins;
	volatile u32_t			cnt_del;
#endif
} cmd_buf_t;

// Command local buffer queue
typedef struct{
	void *					clb[CMD_LOCAL_BUF_TOTAL];
	volatile u32_t			ins;
	volatile u32_t			del;
#ifdef QUEUE_OPERATION_CNT
	volatile u32_t			cnt_ins;
	volatile u32_t			cnt_del;
#endif
} cmd_local_buf_t;

// Command queue
typedef struct{
	void *					cq[CMD_QUEUE_TOTAL];
	volatile u32_t			ins;
	volatile u32_t			del;
#ifdef QUEUE_OPERATION_CNT
	volatile u32_t			cnt_ins;
	volatile u32_t			cnt_del;
#endif
} cmd_queue_t;

/*
*	CPU <- DSP
*/
#define EVENT_QUEUE_TOTAL		8

typedef struct{
	u8_t					ee[EVENT_SIZE];
} event_element_t;

typedef struct{
	event_element_t 		event[EVENT_QUEUE_TOTAL];
	volatile u32_t			ins;
	volatile u32_t			del;
//	volatile u32_t			alloc;		//move to DSP cache area
#ifdef QUEUE_OPERATION_CNT
	volatile u32_t			cnt_ins;
	volatile u32_t			cnt_del;
#endif
} event_queue_t;


/*
*	Control plane related structure in shared memory
*/
/*
*	CPU -> DSP
*/
// Combine command element and command local elements. Real buffers.
typedef struct{
	cmd_element_t			cmd_e [CMD_PHY_TOTAL];
	cmd_local_element_t		cmd_le[CMD_LOCAL_PHY_TOTAL];
#ifdef QUEUE_OPERATION_CNT
	volatile u32_t			done_cnt;
#endif
} cbuf_2dsp_t;

// Combine command queue and buffers. Control index. 
typedef struct{
	cmd_buf_t				cmd_b;
	cmd_local_buf_t			cmd_lb;
	cmd_queue_t				cmd_q;
} ctrl_2dsp_t;

/*
*	CPU <- DSP
*/
// Put this only on DSP side. Cache area is OK.
typedef struct{
	volatile u8_t			commit[EVENT_QUEUE_TOTAL];
} event_commit_t;

// Event queue
typedef struct{
	event_queue_t			eq;
} cbuf_2cpu_t;


typedef struct {
	ctrl_2dsp_t 			ctrl_2dsp;			//CPU -> DSP ctrl
} aipc_ctrl_t; 


typedef struct {
	cbuf_2dsp_t 			cbuf_2dsp;			//CPU ->  DSP cbuf
	cbuf_2cpu_t 			cbuf_2cpu;			//CPU <-  DSP cbuf
} aipc_cbuf_t; 


/*****************************************************************************
*   Statistics information
*****************************************************************************/

typedef struct{
	//	CPU data plane
	u32_t aipc_data_2dsp_alloc;
	u32_t aipc_data_2dsp_send;
	u32_t aipc_data_2cpu_ret;
	u32_t aipc_data_2cpu_recv;

	//	DSP data plane
	u32_t aipc_data_2cpu_alloc;
	u32_t aipc_data_2cpu_send;
	u32_t aipc_data_2dsp_ret;
	u32_t aipc_data_2dsp_recv;

	/*
	*	Control CPU->DSP
	*/
	/** CPU **/
	u32_t aipc_ctrl_2dsp_nofbk_alloc;
	u32_t aipc_ctrl_2dsp_fbk_alloc;
	#ifdef STATS_RETRY
	u32_t aipc_ctrl_2dsp_nofbk_alloc_retry;
	u32_t aipc_ctrl_2dsp_fbk_alloc_retry;
	#endif
	u32_t aipc_ctrl_2dsp_send;
	u32_t aipc_ctrl_2dsp_fbk_ret;
	
	/** DSP **/
	u32_t aipc_ctrl_2dsp_recv;
	u32_t aipc_ctrl_2dsp_nofbk_ret;
	u32_t aipc_ctrl_2dsp_fbk_fin;

	/*
	*	Event CPU<-DSP
	*/
	/** CPU **/
	u32_t aipc_ctrl_2cpu_recv;
	u32_t aipc_ctrl_2cpu_ret;
	
	/** DSP **/
	u32_t aipc_ctrl_2cpu_alloc;
	#ifdef STATS_RETRY
	u32_t aipc_ctrl_2cpu_alloc_retry;
	#endif
	u32_t aipc_ctrl_2cpu_send;

	//interrupt statistics
	u32_t cpu_t_dsp;
	u32_t dsp_t_cpu;

	#ifdef CONFIG_RTL8686_SHM_NOTIFY
	//shm notify
	u32_t shm_notify_cpu;
	u32_t shm_notify_dsp;
	#endif

	//error case counter
	u32_t aipc_data_error;
	u32_t aipc_ctrl_error;

	//exception case
	u32_t aipc_ctrl_2cpu_exception_send;

} aipc_stats_t;


/*****************************************************************************
*   Test Threads
*****************************************************************************/

typedef struct{
//data plane
	//CPU->DSP. RX direction
	u32_t	mbox_2dsp_send; 
	u32_t	mbox_2dsp_poll;
	
	u32_t	mbox_2dsp_counter;
	#ifdef	CONFIG_RTL8686_IPC_TEST_THREAD_2
	u32_t	mbox_2dsp_counter_2;
	#endif
	#ifdef	CONFIG_RTL8686_IPC_TEST_THREAD_3
	u32_t	mbox_2dsp_counter_3;
	#endif


	//CPU<-DSP. TX direction
	u32_t	mbox_2cpu_send;
	u32_t	mbox_2cpu_recv;
	
	u32_t	mbox_2cpu_counter;
	#ifdef	CONFIG_RTL8686_IPC_TEST_THREAD_2
	u32_t	mbox_2cpu_counter_2;
	#endif
	#ifdef	CONFIG_RTL8686_IPC_TEST_THREAD_3
	u32_t	mbox_2cpu_counter_3;
	#endif

//control plane
	//CPU->DSP. RX direction
	u32_t	ctrl_2dsp_send; 
	u32_t	ctrl_2dsp_poll;
	
	u32_t	ctrl_2dsp_counter;
	#ifdef	CONFIG_RTL8686_IPC_TEST_THREAD_2
	u32_t	ctrl_2dsp_counter_2;
	#endif
	#ifdef	CONFIG_RTL8686_IPC_TEST_THREAD_3
	u32_t	ctrl_2dsp_counter_3;
	#endif

	//CPU<-DSP. TX direction
	u32_t	ctrl_2cpu_send;
	u32_t	ctrl_2cpu_poll;
	
	u32_t	ctrl_2cpu_counter;
	#ifdef	CONFIG_RTL8686_IPC_TEST_THREAD_2
	u32_t	ctrl_2cpu_counter_2;
	#endif
	#ifdef	CONFIG_RTL8686_IPC_TEST_THREAD_3
	u32_t	ctrl_2cpu_counter_3;
	#endif

	#ifdef	CONFIG_RTL8686_READ_DRAM_THREAD
	u32_t   cpu_read_cnt;
	u32_t   dsp_read_cnt;
	u32_t   all_read_cnt;
	#endif
	
} aipc_thread_t;

/*****************************************************************************
*   Control switch variable for both CPU/DSP
*****************************************************************************/
//Init status
#define		INIT_NR				(1<<0)
#define		INIT_OK				(1<<1)

typedef struct{
	u32_t				init;				//IPC init done
	u32_t   			dbg_mask;			//debug print mask
	aipc_stats_t 		stats;
	aipc_thread_t		thread;
}aipc_ctsw_t;


/*****************************************************************************
*   SRAM/DRAM data structure define
*****************************************************************************/
typedef struct _aipc_sram{
#ifdef AIPC_BOOT
	aipc_boot_t  boot;
#endif
#ifdef AIPC_GOT
	aipc_got_t	 got;
#endif
	aipc_ctrl_t  ctrl;
	aipc_mbox_t	 mbox;
	aipc_intq_t	 intq;
} aipc_sram_t;

typedef struct _aipc_dram{
	aipc_cbuf_t  cbuf;
	aipc_mail_t	 mail;
	aipc_ctsw_t	 ctsw;
} aipc_dram_t;

#endif


/*****************************************************************************
*   Abbreviation Define
*****************************************************************************/

#if	defined(_AIPC_CPU_)||defined(_AIPC_DSP_)
//#define AIPC_DRAM_BASE		0x90000000
/* DRAM */
#define AIPC_DRAM_BASE		(0xB0000000)
#define AIPC_DRAM_LENGTH	(0x300000)

/* SRAM */
#define AIPC_SRAM_BASE		(AIPC_DRAM_BASE+AIPC_DRAM_LENGTH)
#define AIPC_SRAM_LENGTH	(0x100000)

/* SHM_NOTIFY */
#ifdef CONFIG_RTL8686_SHM_NOTIFY
#define AIPC_SHM_NOTIFY_BASE 	(AIPC_SRAM_BASE+AIPC_SRAM_LENGTH)
#define AIPC_SHM_NOTIFY_CPU		(AIPC_SHM_NOTIFY_BASE+0x0)
#define AIPC_SHM_NOTIFY_DSP		(AIPC_SHM_NOTIFY_BASE+0x4)
#endif


/*	DRAM variable define*/
#define ADRAM 			((aipc_dram_t *)AIPC_DRAM_BASE)
/*	SRAM variable define*/
#define ASRAM 			((aipc_sram_t *)AIPC_SRAM_BASE)

//Interrupt Queue
#define INT_2CPU_HIQ 	((ASRAM)->intq.intq_2cpu.hiq)
#define INT_2CPU_LOWQ 	((ASRAM)->intq.intq_2cpu.lowq)
#define INT_2DSP_HIQ 	((ASRAM)->intq.intq_2dsp.hiq)
#define INT_2DSP_LOWQ 	((ASRAM)->intq.intq_2dsp.lowq)

//Mail box
#define MB_2DSP  		((ASRAM)->mbox.mbox_2dsp.mb)
#define BC_2DSP  		((ASRAM)->mbox.mbox_2dsp.bc)
#define MB_2CPU  		((ASRAM)->mbox.mbox_2cpu.mb)
#define BC_2CPU	 		((ASRAM)->mbox.mbox_2cpu.bc)

#define MAIL_2CPU		((ADRAM)->mail.mail_2cpu)
#define MAIL_2DSP		((ADRAM)->mail.mail_2dsp)

//boot instruction
#ifdef AIPC_BOOT
#define ABOOT			((ASRAM)->boot)
#endif

//GOT function
#ifdef AIPC_GOT
#define AGOT  			((ASRAM)->got)
#endif


//Control plane
//Command CPU->DSP
#define CMD_2DSP				((ASRAM)->ctrl.ctrl_2dsp.cmd_b)
#define CMD_LOCAL_2DSP			((ASRAM)->ctrl.ctrl_2dsp.cmd_lb)
#define CMD_QUEUE_2DSP			((ASRAM)->ctrl.ctrl_2dsp.cmd_q)

#define CBUF_2DSP				((ADRAM)->cbuf.cbuf_2dsp)
#define CBUF_CMD_2DSP			((ADRAM)->cbuf.cbuf_2dsp.cmd_e)
#define CBUF_CMD_LOCAL_2DSP		((ADRAM)->cbuf.cbuf_2dsp.cmd_le)

//Event CPU<-DSP
#define CBUF_EQ_2CPU			((ADRAM)->cbuf.cbuf_2cpu.eq)
#define CBUF_EVENT_2CPU			((ADRAM)->cbuf.cbuf_2cpu.eq.event)

//Control switch for both CPU/DSP
#define ACTSW			((ADRAM)->ctsw)

//Statistics
#define ASTATS			((ADRAM)->ctsw.stats)

//Thread control
#define ATHREAD			((ADRAM)->ctsw.thread)

#else	//!_AIPC_CPU_ && !//_AIPC_DSP_
#error "not supported type"
#endif


#undef  ADEBUG

#ifdef CONFIG_RTL8686_IPC_DEBUG_MESSAGE
#define AIPC_DEBUG
#endif

#ifdef AIPC_DEBUG
#define AIPC_DEBUG_SWITCH
#endif

#if defined(_AIPC_CPU_)

#ifdef AIPC_DEBUG
#  ifdef __KERNEL__
     /* This one if debugging is on, and kernel space */
#    ifdef AIPC_DEBUG_SWITCH
#        define ADEBUG(flg , fmt, args...)  if(flg & ACTSW.dbg_mask) \
				printk("%4d %s  " fmt, __LINE__ , __FUNCTION__ , ## args)
#    else
#        define ADEBUG(fmt, args...)  printk("%4d %s  " fmt , __LINE__ , __FUNCTION__ , ## args)
#    endif
#        define SDEBUG(fmt, args...)  printk("%4d %s  " fmt , __LINE__ , __FUNCTION__ , ## args)
#  endif
#else
#         define ADEBUG(fmt, args...) /* not debugging: nothing */
#         define SDEBUG(fmt, args...) /* not debugging: nothing */
#endif

#elif defined(_AIPC_DSP_)

#ifdef AIPC_DEBUG
#    ifdef AIPC_DEBUG_SWITCH
#        define ADEBUG(flg , fmt, args...)  if(flg & ACTSW.dbg_mask) \
				printf("%4d %s  " fmt , __LINE__ , __FUNCTION__ , ## args)
#    else
#        define ADEBUG(fmt, args...)  printf("%4d %s  " fmt , __LINE__ , __FUNCTION__ , ## args)
#    endif
#        define SDEBUG(fmt, args...)  printf("%4d %s  " fmt , __LINE__ , __FUNCTION__ , ## args)
#else
#        define ADEBUG(fmt, args...) /* not debugging: nothing */
#        define SDEBUG(fmt, args...) /* not debugging: nothing */
#endif

#else
#error "not supported type"
#endif


