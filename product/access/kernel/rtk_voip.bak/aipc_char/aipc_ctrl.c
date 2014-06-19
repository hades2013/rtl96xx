#ifdef _AIPC_CPU_
#include <linux/module.h>
#include <linux/kernel.h>	/* printk() */
#include <linux/errno.h>	/* error codes */
#include <linux/types.h>	/* size_t */
#include <linux/version.h>
#include <linux/semaphore.h>
#include <linux/jiffies.h>

#elif defined(_AIPC_DSP_)
#include <cyg/infra/cyg_type.h>
#include <cyg/kernel/kapi.h>

#undef	printk
#define printk printf

#else	//!_AIPC_CPU_ && !//_AIPC_DSP_
#error "not supported type"
#endif 

#include "./include/aipc_api.h"
#include "./include/aipc_ctrl.h"
#include "./include/aipc_ioctl.h"
#include "./include/aipc_cbk.h"


/*
*	Control CPU->DSP
*/
#ifdef _AIPC_CPU_		//API functions
void *	aipc_ctrl_2dsp_nofbk_alloc(void);
void *	aipc_ctrl_2dsp_fbk_alloc(void);
int		aipc_ctrl_2dsp_send(void *dp, aipc_scheduable schedulable);
int		aipc_ctrl_2dsp_fbk_ret(void *dp);
#else
void *	aipc_ctrl_2dsp_recv(void);
int		aipc_ctrl_2dsp_nofbk_ret(void *dp);
int		aipc_ctrl_2dsp_fbk_fin(void *dp);
#endif

/*
*	Event CPU<-DSP
*/
#ifdef _AIPC_CPU_		//API functions
void *	aipc_ctrl_2cpu_recv(void);
int		aipc_ctrl_2cpu_ret(void *dp);
#else
void *	aipc_ctrl_2cpu_alloc(void);
int		aipc_ctrl_2cpu_send(void *dp);
#endif

/*
*	Variables
*/
#ifdef _AIPC_CPU_
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
static DECLARE_MUTEX(cpu_cm_mutex);
#else
static DEFINE_SEMAPHORE(cpu_cm_mutex);
#endif
#else
static cyg_mutex_t              dsp_cm_mutex;
static volatile event_commit_t  event_commit;
static volatile u32_t           event_alloc;
#endif

/*
*	Control
*/
/**	cmd buf **/
int
aipc_2dsp_cmd_buf_full( void )
{
	if(((CMD_2DSP.del + CMD_BUF_TOTAL - CMD_2DSP.ins)%CMD_BUF_TOTAL) == 1){
		ADEBUG(DBG_CTRL , "full\n");
		return TRUE;
		}
	else{
		ADEBUG(DBG_CTRL , "not full\n");
		return FALSE;
		}
}

//#ifdef _AIPC_CPU_
int
aipc_2dsp_cmd_buf_empty( void )
{
	if(((CMD_2DSP.ins + CMD_BUF_TOTAL - CMD_2DSP.del)%CMD_BUF_TOTAL) == 0){
		ADEBUG(DBG_CTRL , "empty\n");
		return TRUE;
		}
	else{
		ADEBUG(DBG_CTRL , "not empty\n");
		return FALSE;
		}
}
//#endif

//#ifdef _AIPC_CPU_
/**	cmd local buf **/
int
aipc_2dsp_cmd_locbuf_full( void )
{
	if(((CMD_LOCAL_2DSP.del + CMD_LOCAL_BUF_TOTAL - CMD_LOCAL_2DSP.ins)%CMD_LOCAL_BUF_TOTAL) == 1){
		ADEBUG(DBG_CTRL , "full\n");
		return TRUE;
		}
	else{
		ADEBUG(DBG_CTRL , "not full\n");
		return FALSE;
		}
}

int
aipc_2dsp_cmd_locbuf_empty( void )
{
	if(((CMD_LOCAL_2DSP.ins + CMD_LOCAL_BUF_TOTAL - CMD_LOCAL_2DSP.del)%CMD_LOCAL_BUF_TOTAL) == 0){
		ADEBUG(DBG_CTRL , "empty\n");
		return TRUE;
		}
	else{
		ADEBUG(DBG_CTRL , "not empty\n");
		return FALSE;
		}
}
//#endif

/**	cmd queue **/
//#ifdef _AIPC_CPU_
int
aipc_2dsp_cmd_queue_full( void )
{
	if(((CMD_QUEUE_2DSP.del + CMD_QUEUE_TOTAL- CMD_QUEUE_2DSP.ins)%CMD_QUEUE_TOTAL) == 1){
		ADEBUG(DBG_CTRL , "full\n");
		return TRUE;
		}
	else{
		ADEBUG(DBG_CTRL , "not full\n");
		return FALSE;
		}
}
//#endif

//#ifdef _AIPC_DSP_
int
aipc_2dsp_cmd_queue_empty( void )
{
	if(((CMD_QUEUE_2DSP.ins + CMD_QUEUE_TOTAL - CMD_QUEUE_2DSP.del)%CMD_QUEUE_TOTAL) == 0){
		ADEBUG(DBG_CTRL , "empty\n");
		return TRUE;
		}
	else{
		ADEBUG(DBG_CTRL , "not empty\n");
		return FALSE;
		}
}
//#endif

/*
*	Event
*/
/** event queue **/
#ifdef _AIPC_DSP_
static int
aipc_2cpu_event_queue_full( void )
{
	//attention!! this is count on alloc index!!
	if(((CBUF_EQ_2CPU.del + EVENT_QUEUE_TOTAL - event_alloc)%EVENT_QUEUE_TOTAL) == 1){
		ADEBUG(DBG_CTRL , "full\n");
		return TRUE;
		}
	else{
		ADEBUG(DBG_CTRL , "not full\n");
		return FALSE;
		}
}
#endif

#ifdef _AIPC_CPU_
static int
aipc_2cpu_event_queue_empty( void )
{
	if(((CBUF_EQ_2CPU.ins + EVENT_QUEUE_TOTAL - CBUF_EQ_2CPU.del)%EVENT_QUEUE_TOTAL) == 0){
		ADEBUG(DBG_CTRL , "empty\n");
		return TRUE;
		}
	else{
		ADEBUG(DBG_CTRL , "not empty\n");
		return FALSE;
		}
}
#endif

int
aipc_ctrl_dump(char *buf)
{
	int n = 0;

	n += sprintf(buf , "CTRL:\n");

	//Command buffer
	n += sprintf(buf+n , "   CMD_2DSP:");
	if(aipc_2dsp_cmd_buf_empty()){
		n += sprintf(buf+n , "empty\n");
	}else if(aipc_2dsp_cmd_buf_full()){
		n += sprintf(buf+n , "full\n");
	}else{
		n += sprintf(buf+n , "middle\n");
	}
	n += sprintf(buf+n , "\tindex ins=%d del=%d total=%d\n" , CMD_2DSP.ins , CMD_2DSP.del , CMD_BUF_TOTAL);
	n += sprintf(buf+n , "\tcount ins=%d del=%d\n"          , CMD_2DSP.cnt_ins , CMD_2DSP.cnt_del);

	//Command local buffer
	n += sprintf(buf+n , "   CMD_LOCAL_2DSP:");
	if(aipc_2dsp_cmd_locbuf_empty()){
		n += sprintf(buf+n , "empty\n");
	}else if(aipc_2dsp_cmd_locbuf_full()){
		n += sprintf(buf+n , "full\n");
	}else{
		n += sprintf(buf+n , "middle\n");
	}
	n += sprintf(buf+n , "\tindex ins=%d del=%d total=%d\n" , CMD_LOCAL_2DSP.ins , CMD_LOCAL_2DSP.del , CMD_LOCAL_BUF_TOTAL);
	n += sprintf(buf+n , "\tcount ins=%d del=%d\n"          , CMD_LOCAL_2DSP.cnt_ins , CMD_LOCAL_2DSP.cnt_del);

	//Command queue
	n += sprintf(buf+n , "   CMD_QUEUE_2DSP:");
	if(aipc_2dsp_cmd_queue_empty()){
		n += sprintf(buf+n , "empty\n");
	}else if(aipc_2dsp_cmd_queue_full()){
		n += sprintf(buf+n , "full\n");
	}else{
		n += sprintf(buf+n , "middle\n");
	}
	n += sprintf(buf+n , "\tindex ins=%d del=%d total=%d\n" , CMD_QUEUE_2DSP.ins , CMD_QUEUE_2DSP.del , CMD_QUEUE_TOTAL);
	n += sprintf(buf+n , "\tcount ins=%d del=%d\n"          , CMD_QUEUE_2DSP.cnt_ins , CMD_QUEUE_2DSP.cnt_del);

	//Event queue
	n += sprintf(buf+n , "   CBUF_EQ_2CPU:\n");
	n += sprintf(buf+n , "\tindex ins=%d del=%d total=%d\n" , CBUF_EQ_2CPU.ins , CBUF_EQ_2CPU.del , EVENT_QUEUE_TOTAL);
	n += sprintf(buf+n , "\tcount ins=%d del=%d\n"          , CBUF_EQ_2CPU.cnt_ins , CBUF_EQ_2CPU.cnt_del);

	n += sprintf(buf+n , "CBUF_EQ_2CPU event_alloc and event_commit are in DSP side. invisible.\n\n");	

	return n;
}

#ifdef _AIPC_CPU_		//Initial functions
/*
*	Init functions
*	Init is done by CPU
*/
/** command buffer **/
static int
aipc_2dsp_cmd_buf_enq_init( void *dp )
{
	int ret=NOK;

	if(aipc_2dsp_cmd_buf_full()){
		ret=NOK;		
		ADEBUG(DBG_CTRL , "enqueue %p NOK\n" , dp);
		}
	else{
		CMD_2DSP.cb[ CMD_2DSP.ins ] = dp;
		CMD_2DSP.ins = (CMD_2DSP.ins+1) % CMD_BUF_TOTAL;
		ret=OK;
		ADEBUG(DBG_CTRL , "enqueue %p OK\n" , dp);
		}
	return ret; 
}

static void
aipc_2dsp_cmd_buf_init( void )
{
	int i;
	int ret=NOK;
	CMD_2DSP.ins = 0;
	CMD_2DSP.del = 0;
	
	for( i=0 ; i<CMD_PHY_TOTAL ; i++ ){
		ret = aipc_2dsp_cmd_buf_enq_init( &(CBUF_CMD_2DSP[i]) );
		if(ret==NOK){
			SDEBUG( "init failed\n" );
			}
		}

	SDEBUG("CMD_2DSP.ins=%d i=%d\n" , CMD_2DSP.ins , i);
}

/** command local buffer **/
static int
aipc_2dsp_cmd_locbuf_enq_init( void *dp )
{
	int ret=NOK;

	if(aipc_2dsp_cmd_locbuf_full()){
		ret = NOK;		
		ADEBUG(DBG_CTRL , "enqueue %p NOK\n" , dp);
		}
	else{
		CMD_LOCAL_2DSP.clb[ CMD_LOCAL_2DSP.ins ] = dp;
		CMD_LOCAL_2DSP.ins = (CMD_LOCAL_2DSP.ins+1) % CMD_LOCAL_BUF_TOTAL;
		ret=OK;
		ADEBUG(DBG_CTRL , "enqueue %p OK\n" , dp);
		}
	
	return ret; 
}

static void
aipc_2dsp_cmd_locbuf_init( void )
{
	int i;
	int ret=NOK;
	CMD_LOCAL_2DSP.ins = 0;
	CMD_LOCAL_2DSP.del = 0;
	
	for( i=0 ; i<CMD_LOCAL_PHY_TOTAL ; i++ ){
		ret = aipc_2dsp_cmd_locbuf_enq_init( &(CBUF_CMD_LOCAL_2DSP[i]) );
		if(ret==NOK){
			SDEBUG( "init failed\n" );
			}
		}

	SDEBUG("CMD_LOCAL_2DSP.ins=%d i=%d\n" , CMD_LOCAL_2DSP.ins , i);
}

/** command queue **/
static void
aipc_2dsp_cmd_queue_init( void )
{
	CMD_QUEUE_2DSP.ins = 0;
	CMD_QUEUE_2DSP.del = 0;

}

/** event queue **/
#if 0
static int
aipc_2cpu_event_queue_enq_init( void )
{
}
#endif

static void
aipc_2cpu_event_queue_init( void )
{
	CBUF_EQ_2CPU.ins = 0;
	CBUF_EQ_2CPU.del = 0;
}

/** control plane init entry **/
void
aipc_ctrl_event_init( void )
{
	aipc_2dsp_cmd_buf_init();
	aipc_2dsp_cmd_locbuf_init();
	aipc_2dsp_cmd_queue_init();
	aipc_2cpu_event_queue_init();
}

#else

static void
aipc_dsp_event_index_init(void)
{
	/*
	*	init event queue index
	*/
	event_alloc = 0;		//this only can be done in DSP
	CBUF_EQ_2CPU.ins = 0;	//this can be done in CPU
	CBUF_EQ_2CPU.del = 0;	//this can be done in CPU

	/*
	*	init event queue buffer
	*/
	memset(&event_commit , 0 , sizeof(event_commit_t)); //this only can be done in DSP
}


void aipc_dsp_cm_init(void)
{
	// init mutex
	cyg_mutex_init(&dsp_cm_mutex);
	
	aipc_dsp_event_index_init();

}

#endif

#ifdef _AIPC_CPU_		//API internal operations. Control CPU->DSP
//aipc_ctrl_2dsp_nofbk_alloc
static void*	
aipc_2dsp_cmd_buf_deq( void )
{
	void * retp=NULL;

	#ifdef CTRL_SEND_LIMIT
	int  ctrl_send_limit = 0;
	#endif

again:
	#ifdef CTRL_LOCK
	down(&cpu_cm_mutex);
	ADEBUG(DBG_CTRL , "in mutex\n");
	#endif

	if(aipc_2dsp_cmd_buf_empty()){

		#ifdef STATS_RETRY
		if (ASTATS.aipc_ctrl_2dsp_nofbk_alloc_retry > STATS_CTRL_RETRY_MAX){
			printk( "Over retry maximum %s %d\n" , __FUNCTION__ , __LINE__);
			ASTATS.aipc_ctrl_2dsp_nofbk_alloc_retry = 0;
			}
		else
			ASTATS.aipc_ctrl_2dsp_nofbk_alloc_retry++;
		#endif

		#ifdef CTRL_SEND_LIMIT
		ctrl_send_limit++;
		#endif
		
		#ifdef CTRL_LOCK
		up(&cpu_cm_mutex);
		ADEBUG(DBG_CTRL , "out mutex\n");
		#endif

		#ifdef CTRL_SEND_LIMIT
		if (ctrl_send_limit > CTRL_SEND_RETRY_MAX){
			schedule();
			return retp;
		}
		#endif

		schedule();
		goto again;
		}
	else{
		retp = CMD_2DSP.cb[ CMD_2DSP.del ];
		CMD_2DSP.del = (CMD_2DSP.del+1) % CMD_BUF_TOTAL;

		#ifdef QUEUE_OPERATION_CNT
		CMD_2DSP.cnt_del++;
		#endif
		
		ADEBUG(DBG_CTRL , "dequeue %p OK\n" , retp);
		}
	
	ASTATS.aipc_ctrl_2dsp_nofbk_alloc++;

	#ifdef STATS_RETRY
	ASTATS.aipc_ctrl_2dsp_nofbk_alloc_retry = 0;
	#endif

	#ifdef CTRL_LOCK
	up(&cpu_cm_mutex);
	ADEBUG(DBG_CTRL , "out mutex\n");
	#endif

	return retp;
}

void*
aipc_ctrl_2dsp_nofbk_alloc(void)
{
	return aipc_2dsp_cmd_buf_deq();
}


//aipc_ctrl_2dsp_fbk_alloc
static void*
aipc_2dsp_cmd_locbuf_deq( void )
{
	void * retp=NULL;

	#ifdef CTRL_SEND_LIMIT
	int  ctrl_send_limit = 0;
	#endif

again:
	#ifdef CTRL_LOCK
	down(&cpu_cm_mutex);
	ADEBUG(DBG_CTRL , "in mutex\n");
	#endif

	if(aipc_2dsp_cmd_locbuf_empty()){
		
		#ifdef STATS_RETRY
		if (ASTATS.aipc_ctrl_2dsp_fbk_alloc_retry > STATS_CTRL_RETRY_MAX){
			printk( "Over retry maximum %s %d\n" , __FUNCTION__ , __LINE__);
			ASTATS.aipc_ctrl_2dsp_fbk_alloc_retry = 0;
			}
		else
			ASTATS.aipc_ctrl_2dsp_fbk_alloc_retry++;
		#endif

		#ifdef CTRL_SEND_LIMIT
		ctrl_send_limit++;
		#endif
		
		#ifdef CTRL_LOCK
		up(&cpu_cm_mutex);
		ADEBUG(DBG_CTRL , "out mutex\n");
		#endif

		#ifdef CTRL_SEND_LIMIT
		if (ctrl_send_limit > CTRL_SEND_RETRY_MAX){
			schedule();
			return retp;
		}
		#endif

		schedule();
		goto again;
		}
	else{
		retp = CMD_LOCAL_2DSP.clb[ CMD_LOCAL_2DSP.del ];
		CMD_LOCAL_2DSP.del = (CMD_LOCAL_2DSP.del+1) % CMD_LOCAL_BUF_TOTAL;

		#ifdef QUEUE_OPERATION_CNT
		CMD_LOCAL_2DSP.cnt_del++;
		#endif
		
		ADEBUG(DBG_CTRL , "dequeue %p OK\n" , retp);
		}

	ASTATS.aipc_ctrl_2dsp_fbk_alloc++;

	#ifdef STATS_RETRY
	ASTATS.aipc_ctrl_2dsp_fbk_alloc_retry = 0;
	#endif
	
	#ifdef CTRL_LOCK
	up(&cpu_cm_mutex);
	ADEBUG(DBG_CTRL , "out mutex\n");
	#endif

	return retp;
}

void *
aipc_ctrl_2dsp_fbk_alloc(void)
{
	return aipc_2dsp_cmd_locbuf_deq();
}

//aipc_ctrl_2dsp_send(void *dp, aipc_scheduable schedulable)
static int
aipc_2dsp_cmd_queue_enq( void *dp , aipc_scheduable schedulable )
{
	int ret=NOK;
	int is_nofbk=0;
	int is_fbk = 0;
	int idx=0;

	#ifdef CTRL_LOCK
	ul32_t flags;
	#endif

again_restore_int:

	#ifdef CTRL_LOCK
	local_irq_save(flags);
	ADEBUG(DBG_CTRL , "disable interrupt\n");
	#endif

	is_nofbk = IS_NOFBK(dp);
	is_fbk =   IS_FBK(dp);

	if(is_nofbk){

nofbk_restore_int: 
		if(aipc_2dsp_cmd_queue_full()){
			if(schedulable == AIPC_SCHEDULABLE){
				#ifdef CTRL_LOCK
				local_irq_restore(flags);
				ADEBUG(DBG_CTRL , "restore interrupt\n");
				#endif
		
				schedule();
				goto again_restore_int;
				}
			else{
				goto nofbk_restore_int;
				}
			}
		else{
			CMD_QUEUE_2DSP.cq[ CMD_QUEUE_2DSP.ins ] = dp;
			CMD_QUEUE_2DSP.ins = (CMD_QUEUE_2DSP.ins+1) % CMD_QUEUE_TOTAL;

			#ifdef QUEUE_OPERATION_CNT
			CMD_QUEUE_2DSP.cnt_ins++;
			#endif
			
			ret=OK;
			ADEBUG(DBG_CTRL , "enqueue %p OK\n" , dp);
			}
	}
	else if(is_fbk){
		idx    = CMD_LOCBUF_IDX(dp);
	
		if(idx == NOK){
			printk("%d %s wrong case. dp=%p\n" , __LINE__, __FUNCTION__ , dp);
			ASTATS.aipc_ctrl_error++;
		
			#ifdef CTRL_LOCK
			local_irq_restore(flags);
			#endif
			return ret;
		
		}
		else{
			//SDEBUG("FBK control 2DSP idx=%d\n" , idx);
			ADEBUG(DBG_CTRL, "FBK control 2DSP idx=%d\n" , idx);
		}

fbk_restore_int: 

		if(aipc_2dsp_cmd_queue_full()){
			if(schedulable == AIPC_SCHEDULABLE){
				#ifdef CTRL_LOCK
				local_irq_restore(flags);
				ADEBUG(DBG_CTRL , "restore interrupt\n");
				#endif

				schedule();
				goto again_restore_int;
				}
			else{
				goto fbk_restore_int;
				}
			}
		else{
			CMD_QUEUE_2DSP.cq[ CMD_QUEUE_2DSP.ins ] = dp;
			CMD_QUEUE_2DSP.ins = (CMD_QUEUE_2DSP.ins+1) % CMD_QUEUE_TOTAL;

			#ifdef QUEUE_OPERATION_CNT
			CMD_QUEUE_2DSP.cnt_ins++;
			#endif
			
			ret=OK;
			ADEBUG(DBG_CTRL , "enqueue %p OK\n" , dp);
			}

			while(CBUF_CMD_LOCAL_2DSP[idx].done == 0){
				if(schedulable == AIPC_SCHEDULABLE)
					schedule();
				else
					continue;
			}
	}
	else{
		printk("%d %s Wrong case. %p\n" , __LINE__ , __FUNCTION__, dp);
		ASTATS.aipc_ctrl_error++;
	}
	

	ASTATS.aipc_ctrl_2dsp_send++;

	#ifdef CTRL_LOCK
	local_irq_restore(flags);
	ADEBUG(DBG_CTRL , "restore interrupt\n");
	#endif

	return ret;
}

int 
aipc_ctrl_2dsp_send(void *dp, aipc_scheduable schedulable)
{
	return aipc_2dsp_cmd_queue_enq(dp, schedulable);
}


//aipc_ctrl_2dsp_fbk_ret
static int	 
aipc_2dsp_cmd_locbuf_enq( void *dp )
{
	int ret=NOK;

	#ifdef CTRL_LOCK
	ul32_t flags;
	local_irq_save(flags);
	ADEBUG(DBG_CTRL , "disable interrupt\n");
	#endif
	
	if(aipc_2dsp_cmd_locbuf_full()){
		ret=NOK;
		ADEBUG(DBG_CTRL , "enqueue %p NOK\n" , dp);
		}
	else{
		CMD_LOCAL_2DSP.clb[ CMD_LOCAL_2DSP.ins ] = dp;
		CMD_LOCAL_2DSP.ins = (CMD_LOCAL_2DSP.ins+1) % CMD_LOCAL_BUF_TOTAL;

		#ifdef QUEUE_OPERATION_CNT
		CMD_LOCAL_2DSP.cnt_ins++;
		#endif
		
		ret=OK;
		ADEBUG(DBG_CTRL , "enqueue %p OK\n" , dp);
		}

	ASTATS.aipc_ctrl_2dsp_fbk_ret++;

	#ifdef CTRL_LOCK
	local_irq_restore(flags);
	ADEBUG(DBG_CTRL , "restore interrupt\n");
	#endif
	
	return ret; 
}

int
aipc_ctrl_2dsp_fbk_ret(void * dp)
{
	return aipc_2dsp_cmd_locbuf_enq(dp);
}

EXPORT_SYMBOL(aipc_ctrl_2dsp_nofbk_alloc);
EXPORT_SYMBOL(aipc_ctrl_2dsp_fbk_alloc);
EXPORT_SYMBOL(aipc_ctrl_2dsp_send);
EXPORT_SYMBOL(aipc_ctrl_2dsp_fbk_ret);

#else

//aipc_ctrl_2dsp_recv
static void*  
aipc_2dsp_cmd_queue_deq( void )
{
	void *retp;

	#ifdef CTRL_LOCK
	cyg_uint32 _old;
	HAL_DISABLE_INTERRUPTS(_old);
	ADEBUG(DBG_CTRL , "disable interrupt\n");
	#endif

	if(aipc_2dsp_cmd_queue_empty()){
		retp = NULL;
		ADEBUG(DBG_CTRL , "dequeue NOK\n");
		}
	else{
		retp = CMD_QUEUE_2DSP.cq[ CMD_QUEUE_2DSP.del ];
		CMD_QUEUE_2DSP.del = (CMD_QUEUE_2DSP.del+1) % CMD_QUEUE_TOTAL;

		#ifdef QUEUE_OPERATION_CNT
		CMD_QUEUE_2DSP.cnt_del++;
		#endif
		
		ADEBUG(DBG_CTRL , "dequeue %p OK\n" , retp);
		}

	ASTATS.aipc_ctrl_2dsp_recv++;

	#ifdef CTRL_LOCK
	HAL_RESTORE_INTERRUPTS(_old);
	ADEBUG(DBG_CTRL , "restore interrupt\n");
	#endif

	return retp;
}

void *
aipc_ctrl_2dsp_recv(void)
{
	return aipc_2dsp_cmd_queue_deq();	
}


//aipc_ctrl_2dsp_nofbk_ret
static int 
aipc_2dsp_cmd_buf_enq( void *dp )
{
	int ret=NOK;
	
	#ifdef CTRL_LOCK
	cyg_uint32 _old;
	HAL_DISABLE_INTERRUPTS(_old);
	ADEBUG(DBG_CTRL , "disable interrupt\n");
	#endif

	if(aipc_2dsp_cmd_buf_full()){
		ret = NOK;		
		ADEBUG(DBG_CTRL , "enqueue %p NOK\n" , dp);
		}
	else{
		CMD_2DSP.cb[ CMD_2DSP.ins ] = dp;
		CMD_2DSP.ins = (CMD_2DSP.ins+1) % CMD_BUF_TOTAL;

		#ifdef QUEUE_OPERATION_CNT
		CMD_2DSP.cnt_ins++;
		#endif
		
		ret=OK;
		ADEBUG(DBG_CTRL , "enqueue %p OK\n" , dp);
		}

	ASTATS.aipc_ctrl_2dsp_nofbk_ret++;

	#ifdef CTRL_LOCK
	HAL_RESTORE_INTERRUPTS(_old);
	ADEBUG(DBG_CTRL , "restore interrupt\n");
	#endif
	
	return ret; 
}

int
aipc_ctrl_2dsp_nofbk_ret(void * dp)
{
	return aipc_2dsp_cmd_buf_enq(dp);
}


//aipc_ctrl_2dsp_fbk_fin
static int
_aipc_ctrl_2dsp_fbk_fin( void *dp )
{
	int ret=NOK;
	int idx=NOK;

	#ifdef CTRL_LOCK
	cyg_uint32 _old;
	HAL_DISABLE_INTERRUPTS(_old);
	ADEBUG(DBG_CTRL , "disable interrupt\n");
	#endif

	idx = CMD_LOCBUF_IDX(dp);

	if (idx!=NOK){
		CBUF_CMD_LOCAL_2DSP[idx].done = 1;

		#ifdef QUEUE_OPERATION_CNT
		CBUF_2DSP.done_cnt++;
		#endif		
	}
	else{
		SDEBUG("wrong buffer %p\n" , dp);
		ASTATS.aipc_ctrl_error++;
	}

	ASTATS.aipc_ctrl_2dsp_fbk_fin++;

	#ifdef CTRL_LOCK
	HAL_RESTORE_INTERRUPTS(_old);
	ADEBUG(DBG_CTRL , "restore interrupt\n");
	#endif

	return ret;
}

int
aipc_ctrl_2dsp_fbk_fin(void * dp)
{
	return _aipc_ctrl_2dsp_fbk_fin(dp);
}
#endif


#ifdef _AIPC_CPU_		//API internal operations. Event CPU<-DSP

//aipc_ctrl_2cpu_recv
static void*
aipc_2cpu_event_queue_deq( void )
{
	void *retp=NULL;

	#ifdef CTRL_LOCK
	ul32_t flags;
	local_irq_save(flags);
	ADEBUG(DBG_CTRL , "disable interrupt\n");
	#endif

	ASTATS.aipc_ctrl_2cpu_recv++;
	
	if (aipc_2cpu_event_queue_empty()){
		retp=NULL;
		ADEBUG(DBG_CTRL , "event euque is empty\n");
	}
	else{
		retp = &(CBUF_EVENT_2CPU[ CBUF_EQ_2CPU.del ]);
		ADEBUG(DBG_CTRL, "event pointer=%p\n" , retp);
	}

	#ifdef CTRL_LOCK
	local_irq_restore(flags);
	ADEBUG(DBG_CTRL , "restore interrupt\n");
	#endif
	
	return retp;
}

void *
aipc_ctrl_2cpu_recv(void)
{
	return aipc_2cpu_event_queue_deq();
}

//aipc_ctrl_2cpu_ret
static int
_aipc_ctrl_2cpu_ret(void * dp)
{
	int ret=NOK;

	#ifdef CTRL_LOCK
	ul32_t flags;
	local_irq_save(flags);
	ADEBUG(DBG_CTRL , "disable interrupt\n");
	#endif

	CBUF_EQ_2CPU.del = (CBUF_EQ_2CPU.del+1)%EVENT_QUEUE_TOTAL;

	#ifdef QUEUE_OPERATION_CNT
	CBUF_EQ_2CPU.cnt_del++;
	#endif

	ret=OK;

	ASTATS.aipc_ctrl_2cpu_ret++;
	
	#ifdef CTRL_LOCK
	local_irq_restore(flags);
	ADEBUG(DBG_CTRL , "restore interrupt\n");
	#endif

	return ret;
}

int
aipc_ctrl_2cpu_ret(void * dp)
{
	return _aipc_ctrl_2cpu_ret(dp);
}

EXPORT_SYMBOL(aipc_ctrl_2cpu_recv);
EXPORT_SYMBOL(aipc_ctrl_2cpu_ret);

#else

//aipc_ctrl_2cpu_alloc
static void *	
_aipc_ctrl_2cpu_alloc( void )
{
	void *retp = NULL;
	cyg_uint32 _old;

	#ifdef CTRL_SEND_LIMIT
	int  ctrl_send_limit = 0;
	#endif

again:
	#ifdef CTRL_LOCK
	HAL_DISABLE_INTERRUPTS(_old);
	#endif
	if (aipc_2cpu_event_queue_full()){

		#ifdef STATS_RETRY
		if (ASTATS.aipc_ctrl_2cpu_alloc_retry > STATS_CTRL_RETRY_MAX){
			printk( "Over retry maximum %s %d\n" , __FUNCTION__ , __LINE__);
			ASTATS.aipc_ctrl_2cpu_alloc_retry = 0;
			}
		else
			ASTATS.aipc_ctrl_2cpu_alloc_retry++;
		#endif

		#ifdef CTRL_SEND_LIMIT
		ctrl_send_limit++;
		#endif
			
		#ifdef CTRL_LOCK
		HAL_RESTORE_INTERRUPTS(_old);
		#endif

		#ifdef CTRL_SEND_LIMIT
		if (ctrl_send_limit > CTRL_SEND_RETRY_MAX){
			cyg_thread_yield();
			return retp;
		}
		#endif		
		
		cyg_thread_yield();
		goto again;
	}

	retp = &(CBUF_EVENT_2CPU[ event_alloc ]);
	event_commit.commit[ event_alloc ] = 0;
	event_alloc = (event_alloc+1) % EVENT_QUEUE_TOTAL;

	ASTATS.aipc_ctrl_2cpu_alloc++;

	#ifdef STATS_RETRY
	ASTATS.aipc_ctrl_2cpu_alloc_retry = 0;
	#endif

	ADEBUG(DBG_CTRL , "alloc event buffer %p\n" , retp);
	#ifdef CTRL_LOCK
	HAL_RESTORE_INTERRUPTS(_old);
	#endif
	return retp;
}

void *
aipc_ctrl_2cpu_alloc(void)
{
	return _aipc_ctrl_2cpu_alloc();
}

//aipc_ctrl_2cpu_send
static int	 
aipc_2cpu_event_queue_enq( void *dp )
{
	extern int aipc_int_send_2cpu( u32_t int_id );

	int ret=NOK;
	int int_id=AIPC_MGR_2CPU_EVENT_RECV;
	int cur=0;
	int idx=NOK;

	#ifdef CTRL_LOCK
	cyg_uint32 _old;
	HAL_DISABLE_INTERRUPTS(_old);
	ADEBUG(DBG_CTRL , "disable interrupt\n");
	#endif

	idx = EVENT_IDX(dp);

	if(idx!=NOK){
		event_commit.commit[idx] = 1;
	}
	else{
		#ifdef CTRL_LOCK
		HAL_RESTORE_INTERRUPTS(_old);
		ADEBUG(DBG_CTRL , "restore interrupt\n");
		#endif
		return ret;
	}

	cur = CBUF_EQ_2CPU.ins;

	while(cur != event_alloc){
		if(event_commit.commit[cur]==0)
			break;

		cur = (cur+1)%EVENT_QUEUE_TOTAL;
	}

	if(CBUF_EQ_2CPU.ins != event_alloc){
		CBUF_EQ_2CPU.ins = cur;

		#ifdef QUEUE_OPERATION_CNT
		CBUF_EQ_2CPU.cnt_ins++;
		#endif

		ret = aipc_int_send_2cpu( int_id );
		if (ret==OK){
			ADEBUG(DBG_CTRL, "enqueue %p OK\n\n" , dp);
			//SDEBUG("enqueue %p OK\n\n" , dp);
			}
		else{
			ADEBUG(DBG_CTRL, "enqueue %p failed!!!!!\n\n" , dp);
			//SDEBUG("enqueue %p failed!!!!!\n\n" , dp);
			}
	}else{
		ASTATS.aipc_ctrl_2cpu_exception_send++;
	}

	ASTATS.aipc_ctrl_2cpu_send++;

	#ifdef CTRL_LOCK
	HAL_RESTORE_INTERRUPTS(_old);
	ADEBUG(DBG_CTRL , "restore interrupt\n");
	#endif

	return ret;	
}

int
aipc_ctrl_2cpu_send(void * dp)
{
	return aipc_2cpu_event_queue_enq(dp);
}


#endif


#ifdef CONFIG_RTL8686_IPC_TEST_THREAD
#ifdef _AIPC_CPU_		//Control Test
static struct task_struct *			ctrl_2dsp_send_task=NULL;
static struct ctrl_2dsp_priv		ctrl_2dsp_send_dsc;

#ifdef CONFIG_RTL8686_IPC_TEST_THREAD_2
static struct task_struct *			ctrl_2dsp_send_task_2=NULL;
static struct ctrl_2dsp_priv		ctrl_2dsp_send_dsc_2;
#endif

#ifdef CONFIG_RTL8686_IPC_TEST_THREAD_3
static struct task_struct *			ctrl_2dsp_send_task_3=NULL;
static struct ctrl_2dsp_priv		ctrl_2dsp_send_dsc_3;
#endif

static int 
ctrl_2dsp_send_thread(void *p)
{
	struct ctrl_2dsp_priv * tp = (struct ctrl_2dsp_priv *)p;

	int i=0;

	void *mp;  

    set_current_state(TASK_INTERRUPTIBLE);

	do {                    
		msleep_interruptible(tp->period);

		if(ATHREAD.ctrl_2dsp_send & CTRL_2DSP_SEND_START){
			ADEBUG(DBG_CTRL , "\n");

#ifdef CPU_SEND_NOFBK
			for(i=0 ; i<5; i++){	
				msleep_interruptible(tp->period);

				mp = aipc_ctrl_2dsp_nofbk_alloc();

				if(mp){
					memset(mp , 0 , CMD_SIZE);
					snprintf(mp , CMD_SIZE , "%s counter=%u\n" , tp->name , (*(u32_t*)(tp->counter_ptr))++);
					ADEBUG(DBG_CONT , "%s\n" , (char*)mp);
					aipc_ctrl_2dsp_send(mp , AIPC_SCHEDULABLE);
				}
			}

			for(i=0 ; i<5; i++){	
				msleep_interruptible(tp->period);

				mp = aipc_ctrl_2dsp_nofbk_alloc();

				if(mp){
					memset(mp , 0 , CMD_SIZE);
					snprintf(mp , CMD_SIZE , "%s counter=%u\n" , tp->name , (*(u32_t*)(tp->counter_ptr))++);
					ADEBUG(DBG_CONT , "%s\n" , (char*)mp);
					aipc_ctrl_2dsp_send(mp , AIPC_NON_SCHEDULABLE);
				}
			}
#endif

#ifdef CPU_SEND_FBK	
			for(i=0 ; i<2; i++){	

				msleep_interruptible(tp->period);

				mp = aipc_ctrl_2dsp_fbk_alloc();
				
				if(mp){
					memset(mp , 0 , CMD_SIZE);
					snprintf(mp , CMD_SIZE , "%s counter=%u\n" , tp->name , (*(u32_t*)(tp->counter_ptr))++);

					ADEBUG(DBG_CONT , "%s\n" , (char*)mp);

					aipc_ctrl_2dsp_send(mp , AIPC_SCHEDULABLE);

					ADEBUG(DBG_CONT , "%s\n" , (char*)mp);
					
					aipc_ctrl_2dsp_fbk_ret(mp);
				}
			}

			for(i=0 ; i<2; i++){	

				msleep_interruptible(tp->period);

				mp = aipc_ctrl_2dsp_fbk_alloc();
				
				if(mp){
					memset(mp , 0 , CMD_SIZE);
					snprintf(mp , CMD_SIZE , "%s counter=%u\n" , tp->name , (*(u32_t*)(tp->counter_ptr))++);

					ADEBUG(DBG_CONT , "%s\n" , (char*)mp);

					aipc_ctrl_2dsp_send(mp , AIPC_NON_SCHEDULABLE);

					ADEBUG(DBG_CONT , "%s\n" , (char*)mp);
					
					aipc_ctrl_2dsp_fbk_ret(mp);
				}
			}
#endif
		}
	} while (!kthread_should_stop());

	return OK;
}



static int 
ctrl_2dsp_send_task_init(
	struct task_struct    * task_ptr,
	struct ctrl_2dsp_priv * thread_priv,
	void * thread_fn,
	char * thread_name,
	void * counter_ptr , 
	u32_t  period
	)
{
	if (!thread_priv || !thread_fn || !thread_name || !counter_ptr || !period){
		printk("Wrong thread init\n");
		SDEBUG("\n");
		return NOK;
	}

	memset(thread_priv , 0 , sizeof(struct ctrl_2dsp_priv));
	
	thread_priv->counter_ptr = counter_ptr;
	thread_priv->period      = period;

	strncpy(thread_priv->name, thread_name , sizeof(thread_priv->name));
	mutex_init(&thread_priv->mutex);


	task_ptr = kthread_run(
				thread_fn,
				thread_priv,
				thread_priv->name);

	if (!IS_ERR(task_ptr)){
		//printk("%s create successfully!\n" , thread_priv->name);
		return OK;
	}

	printk("%s create failed!\n" , thread_priv->name);
	return NOK;
}



static int 
cpu_ctrl_task_start(void)
{
	ctrl_2dsp_send_task_init(
		ctrl_2dsp_send_task , 
		&ctrl_2dsp_send_dsc ,
		ctrl_2dsp_send_thread,
		CPU_CTRL_SEND_NAME,
		&(ATHREAD.ctrl_2dsp_counter),
		CPU_CTRL_SEND_PERIOD
	);

#ifdef CONFIG_RTL8686_IPC_TEST_THREAD_2
	ctrl_2dsp_send_task_init(
		ctrl_2dsp_send_task_2, 
		&ctrl_2dsp_send_dsc_2,
		ctrl_2dsp_send_thread,
		CPU_CTRL_SEND_NAME_2,
		&(ATHREAD.ctrl_2dsp_counter_2),
		CPU_CTRL_SEND_PERIOD_2
	);
#endif

#ifdef CONFIG_RTL8686_IPC_TEST_THREAD_3
	ctrl_2dsp_send_task_init(
		ctrl_2dsp_send_task_3, 
		&ctrl_2dsp_send_dsc_3,
		ctrl_2dsp_send_thread,
		CPU_CTRL_SEND_NAME_3,
		&(ATHREAD.ctrl_2dsp_counter_3),
		CPU_CTRL_SEND_PERIOD_3
	);
#endif

	return OK;
}

int 
cpu_ctrl_task_init(void)
{
	cpu_ctrl_task_start();
	return OK;
}

#else

#define CTRL_2DSP_POLL_PRIORITY  	29
#define CTRL_2DSP_POLL_STACK_SIZE  	2048 

static int ctrl_2dsp_poll_thread_stack[ CTRL_2DSP_POLL_STACK_SIZE ];
static cyg_handle_t ctrl_2dsp_poll_thread_handle;
static cyg_thread   ctrl_2dsp_poll_thread_obj;

static void 
ctrl_2dsp_poll_thread(cyg_addrword_t index)
{
	void *mp;

	while(1)
	{
		cyg_thread_delay( DSP_CTRL_POLL_PERIOD );
		
		if(ATHREAD.ctrl_2dsp_poll & CTRL_2DSP_POLL_START){
			ADEBUG(DBG_CTRL , "\n");
			
			mp = aipc_ctrl_2dsp_recv();	
			if(mp){				
				ADEBUG(DBG_CONT , "%s\n" , (char*)mp);

				if(IS_FBK(mp)){
						snprintf(mp , CMD_SIZE , "this need fbk\n");
					}
				
				RETURN_CTRL(mp);				
				}	
			else{
				ADEBUG(DBG_CONT , "no contrl CPU->DSP\n");
			}
		}
	}
}

static int 
ctrl_2dsp_poll_task_init(void)
{
	cyg_thread_create(
		CTRL_2DSP_POLL_PRIORITY,
		ctrl_2dsp_poll_thread,
		0,
		DSP_CTRL_POLL_NAME,
		&ctrl_2dsp_poll_thread_stack,
		CTRL_2DSP_POLL_STACK_SIZE,
		&ctrl_2dsp_poll_thread_handle,
		&ctrl_2dsp_poll_thread_obj );

		// Let the thread run when the scheduler starts.
		cyg_thread_resume( ctrl_2dsp_poll_thread_handle );

	return OK;
}


static int ctrl_2cpu_send_task_init(void);
#ifdef CONFIG_RTL8686_IPC_TEST_THREAD_2
static int ctrl_2cpu_send_task_init_2(void);
#endif
#ifdef CONFIG_RTL8686_IPC_TEST_THREAD_3
static int ctrl_2cpu_send_task_init_3(void);
#endif



static int 
dsp_ctrl_task_start(void)
{
	ctrl_2cpu_send_task_init();
#ifdef CONFIG_RTL8686_IPC_TEST_THREAD_2
	ctrl_2cpu_send_task_init_2();
#endif
#ifdef CONFIG_RTL8686_IPC_TEST_THREAD_3
	ctrl_2cpu_send_task_init_3();
#endif
	ctrl_2dsp_poll_task_init();

	return OK;
}

int 
dsp_ctrl_task_init(void)
{
	dsp_ctrl_task_start();
	return OK;
}

#endif

#endif


#ifdef CONFIG_RTL8686_IPC_TEST_THREAD
#ifdef _AIPC_DSP_		//Event Test

#define CTRL_2CPU_SEND_PRIORITY 	29
#define CTRL_2CPU_SEND_STACK_SIZE 	2048 
static int ctrl_2cpu_send_thread_stack[ CTRL_2CPU_SEND_STACK_SIZE ];
static cyg_handle_t ctrl_2cpu_send_thread_handle;
static cyg_thread   ctrl_2cpu_send_thread_obj;

static void 
ctrl_2cpu_send_thread(cyg_addrword_t index)
{
	void *mp;
	while(1)
	{
		cyg_thread_delay( DSP_CTRL_SEND_PERIOD );

		if(ATHREAD.ctrl_2cpu_send & CTRL_2CPU_SEND_START){

			ADEBUG(DBG_CTRL , "\n");

			mp = aipc_ctrl_2cpu_alloc();

			if(mp){
				memset(mp , 0 , EVENT_SIZE);

				snprintf(mp , EVENT_SIZE, 
					"%s ctrl_2cpu_counter=%u\n" , "CTRL CPU<-DSP Thread 1" , (ATHREAD.ctrl_2cpu_counter)++);

				ADEBUG(DBG_CONT , "%s\n" , (char*)mp);

				aipc_ctrl_2cpu_send( mp );			
			}
			else{
				ADEBUG(DBG_CONT , "CC_2CPU no buffers\n");
			}
		}
	}
}

static int 
ctrl_2cpu_send_task_init(void)
{
	cyg_thread_create(
		CTRL_2CPU_SEND_PRIORITY,
		ctrl_2cpu_send_thread,
		0,
		DSP_CTRL_SEND_NAME,
		&ctrl_2cpu_send_thread_stack,
		CTRL_2CPU_SEND_STACK_SIZE,
		&ctrl_2cpu_send_thread_handle,
		&ctrl_2cpu_send_thread_obj );

		// Let the thread run when the scheduler starts.
		cyg_thread_resume( ctrl_2cpu_send_thread_handle );

	return OK;
}

#ifdef CONFIG_RTL8686_IPC_TEST_THREAD_2

#define CTRL_2CPU_SEND_PRIORITY_2 		29
#define CTRL_2CPU_SEND_STACK_SIZE_2 	2048 

static int ctrl_2cpu_send_thread_stack_2[ CTRL_2CPU_SEND_STACK_SIZE_2 ];
static cyg_handle_t ctrl_2cpu_send_thread_handle_2;
static cyg_thread   ctrl_2cpu_send_thread_obj_2;

static void 
ctrl_2cpu_send_thread_2(cyg_addrword_t index)
{
	void *mp;
	while(1)
	{
		cyg_thread_delay( DSP_CTRL_SEND_PERIOD_2 );

		if(ATHREAD.ctrl_2cpu_send & CTRL_2CPU_SEND_START){

			ADEBUG(DBG_CTRL , "\n");

			mp = aipc_ctrl_2cpu_alloc();
			
			if(mp){
				memset(mp , 0 , EVENT_SIZE);
			
				snprintf(mp , EVENT_SIZE, 
					"%s ctrl_2cpu_counter_2=%u\n" , "CTRL CPU<-DSP Thread 2" , (ATHREAD.ctrl_2cpu_counter_2)++);
			
				ADEBUG(DBG_CONT , "%s\n" , (char*)mp);
			
				aipc_ctrl_2cpu_send( mp );
				}			
			else{
				ADEBUG(DBG_CONT , "CC_2CPU no buffers\n");
			}
		}
	}
}

static int 
ctrl_2cpu_send_task_init_2(void)
{
	cyg_thread_create(
		CTRL_2CPU_SEND_PRIORITY_2,
		ctrl_2cpu_send_thread_2,
		0,
		DSP_CTRL_SEND_NAME_2,
		&ctrl_2cpu_send_thread_stack_2,
		CTRL_2CPU_SEND_STACK_SIZE_2,
		&ctrl_2cpu_send_thread_handle_2,
		&ctrl_2cpu_send_thread_obj_2 );

		// Let the thread run when the scheduler starts.
		cyg_thread_resume( ctrl_2cpu_send_thread_handle_2 );

	return OK;
}

#endif

#ifdef CONFIG_RTL8686_IPC_TEST_THREAD_3

#define CTRL_2CPU_SEND_PRIORITY_3 		29
#define CTRL_2CPU_SEND_STACK_SIZE_3 	2048 

static int ctrl_2cpu_send_thread_stack_3[ CTRL_2CPU_SEND_STACK_SIZE_3 ];
static cyg_handle_t ctrl_2cpu_send_thread_handle_3;
static cyg_thread   ctrl_2cpu_send_thread_obj_3;

static void 
ctrl_2cpu_send_thread_3(cyg_addrword_t index)
{
	void *mp;
	while(1)
	{
		cyg_thread_delay( DSP_CTRL_SEND_PERIOD_3 );

		if(ATHREAD.ctrl_2cpu_send & CTRL_2CPU_SEND_START){

			ADEBUG(DBG_CTRL , "\n");

			mp = aipc_ctrl_2cpu_alloc();
			
			if(mp){
				memset(mp , 0 , EVENT_SIZE);
			
				snprintf(mp , EVENT_SIZE, 
					"%s ctrl_2cpu_counter_3=%u\n" , "CTRL CPU<-DSP Thread 2" , (ATHREAD.ctrl_2cpu_counter_3)++);
			
				ADEBUG(DBG_CONT , "%s\n" , (char*)mp);
			
				aipc_ctrl_2cpu_send( mp );			
				}			
			else{
				ADEBUG(DBG_CONT , "CC_2CPU no buffers\n");
			}
		}
	}
}

static int 
ctrl_2cpu_send_task_init_3(void)
{
	cyg_thread_create(
		CTRL_2CPU_SEND_PRIORITY_3,
		ctrl_2cpu_send_thread_3,
		0,
		DSP_CTRL_SEND_NAME_3,
		&ctrl_2cpu_send_thread_stack_3,
		CTRL_2CPU_SEND_STACK_SIZE_3,
		&ctrl_2cpu_send_thread_handle_3,
		&ctrl_2cpu_send_thread_obj_3 );

		// Let the thread run when the scheduler starts.
		cyg_thread_resume( ctrl_2cpu_send_thread_handle_3 );

	return OK;
}

#endif
#endif
#endif


