#ifdef _AIPC_CPU_
#include <linux/module.h>
#include <linux/kernel.h>

#elif defined(_AIPC_DSP_)
#undef	printk
#define printk diag_printf
#define printf diag_printf

#else
#error "need to define"
#endif

#include "./include/aipc_shm.h"

#if	defined(REFINE_SHM_NOTIFY) && defined(CONFIG_RTL8686_SHM_NOTIFY)
	const unsigned int *aipc_shm_notify_cpu = (unsigned int *)&(SHM_NOTIFY.notify_cpu);
	const unsigned int *aipc_shm_notify_dsp = (unsigned int *)&(SHM_NOTIFY.notify_dsp);
#elif !defined(REFINE_SHM_NOTIFY) && defined(CONFIG_RTL8686_SHM_NOTIFY)
	const unsigned int *aipc_shm_notify_cpu = (unsigned int *)AIPC_SHM_NOTIFY_CPU;
	const unsigned int *aipc_shm_notify_dsp = (unsigned int *)AIPC_SHM_NOTIFY_DSP;
#endif

#ifdef CONFIG_RTL8686_IPC_RECORD_DSP_LOG
unsigned int *rec_dsp_log_enable     = (unsigned int *)&(REC_DSP_LOG.enable);
unsigned int *rec_dsp_log_ins        = (unsigned int *)&(REC_DSP_LOG.ins);
unsigned int *rec_dsp_log_del        = (unsigned int *)&(REC_DSP_LOG.del);
unsigned int *rec_dsp_log_touch      = (unsigned int *)&(REC_DSP_LOG.touch);
char         *rec_dsp_log_contents   = (char *)        &(REC_DSP_LOG.contents);

typedef int (*ft_aipc_dsp_log_add)(char);
ft_aipc_dsp_log_add   fp_aipc_dsp_log_add = NULL;


int aipc_record_dsp_log_full(void)
{
	if ((*rec_dsp_log_del + RECORD_DSP_LOG_SIZE - *rec_dsp_log_ins) % RECORD_DSP_LOG_SIZE == 1)
		return TRUE;
	else 
		return FALSE;

}

int aipc_record_dsp_log_empty(void)
{
	if ((*rec_dsp_log_ins + RECORD_DSP_LOG_SIZE - *rec_dsp_log_del) % RECORD_DSP_LOG_SIZE == 0)
		return TRUE;
	else 
		return FALSE;
}

unsigned int aipc_record_dsp_log_contents_use(void)
{
	return ((*rec_dsp_log_ins + RECORD_DSP_LOG_SIZE - *rec_dsp_log_del) % RECORD_DSP_LOG_SIZE);
}
#endif

#ifdef _AIPC_CPU_

unsigned short CheckDspIfAllSoftwareReady(void)
{
	unsigned short status = (unsigned short)ACTSW.init;
	return status;
}

EXPORT_SYMBOL(CheckDspIfAllSoftwareReady);

#else

unsigned short SetDspAllSoftwareReady(void)
{
	unsigned short status;
	ACTSW.init = INIT_OK;
	status = (unsigned short)ACTSW.init;
	return status;
}

#ifdef CONFIG_RTL8686_IPC_RECORD_DSP_LOG

int aipc_dsp_log_add(char buf)
{
	if ((*rec_dsp_log_enable) && (!aipc_record_dsp_log_full()) && (rec_dsp_log_contents)){
		rec_dsp_log_contents[ *rec_dsp_log_ins ] = buf;
		*rec_dsp_log_ins = (*rec_dsp_log_ins+1) % RECORD_DSP_LOG_SIZE;
		return OK;
	}
	else{
		return NOK;	
	}
}

int aipc_record_dsp_log_init(void)
{
	int status = NOK;
	REC_DSP_LOG.enable = (unsigned int)RECORD_DSP_LOG_ENABLE;
	
	if( !fp_aipc_dsp_log_add ){
		fp_aipc_dsp_log_add = aipc_dsp_log_add;
		//diag_printf("%p %p %s(%d)\n" , fp_aipc_dsp_log_add , aipc_dsp_log_add , __FUNCTION__, __LINE__);
	}

	status = OK;
	return status;
}
#endif

void
aipc_dsp_ipc_init(void)
{
	extern void	aipc_dsp_cm_init(void);
	extern void aipc_dsp_mb_init(void);
#ifdef CONFIG_RTL8686_IPC_RECORD_DSP_LOG
	extern int aipc_record_dsp_log_init( void );
#endif

#ifdef AIPC_GOT
	extern void aipc_dsp_intq_init( void );
	aipc_dsp_intq_init();
#endif

	aipc_dsp_cm_init();
	aipc_dsp_mb_init();
	
#ifdef CONFIG_RTL8686_IPC_RECORD_DSP_LOG
	aipc_record_dsp_log_init();
#endif
}

#endif


