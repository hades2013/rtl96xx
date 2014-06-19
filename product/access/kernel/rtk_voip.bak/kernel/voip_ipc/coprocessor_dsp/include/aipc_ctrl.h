#ifndef __AIPC_CTRL_H__
#define __AIPC_CTRL_H__

#include "aipc_global.h"
#include "soc_type.h"
#include "aipc_shm.h"
#include "aipc_api.h"

/*****************************************************************************
*   Macro Definitions
*****************************************************************************/

/*****************************************************************************
*   Data Structure
*****************************************************************************/
#ifdef CONFIG_RTL8686_IPC_TEST_THREAD
#define CTRL_NAME_SIZE		32

//CPU
#define CPU_CTRL_SEND_NAME		"CpuCtrlSendD"
#define CPU_CTRL_POLL_NAME		"CpuCtrlPollD"
#define CPU_CTRL_SEND_PERIOD	((1)*(MUL))
#define CPU_CTRL_POLL_PERIOD	((1)*(MUL))

//DSP
#define DSP_CTRL_SEND_NAME		"DspCtrlSendD"
#define DSP_CTRL_POLL_NAME		"DspCtrlPollD"
#define DSP_CTRL_SEND_PERIOD	((1)*(MUL))
#define DSP_CTRL_POLL_PERIOD	((1)*(MUL))
#endif

#ifdef CONFIG_RTL8686_IPC_TEST_THREAD_2
#define CTRL_NAME_SIZE_2		32

//CPU
#define CPU_CTRL_SEND_NAME_2	"CpuCtrlSendD_2"
#define CPU_CTRL_POLL_NAME_2	"CpuCtrlPollD_2"
#define CPU_CTRL_SEND_PERIOD_2	((1)*(MUL)*2)
#define CPU_CTRL_POLL_PERIOD_2	((1)*(MUL)*2)

//DSP
#define DSP_CTRL_SEND_NAME_2	"DspCtrlSendD_2"
#define DSP_CTRL_POLL_NAME_2	"DspCtrlPollD_2"
#define DSP_CTRL_SEND_PERIOD_2	((1)*(MUL)*2)
#define DSP_CTRL_POLL_PERIOD_2	((1)*(MUL)*2)
#endif

#ifdef CONFIG_RTL8686_IPC_TEST_THREAD_3
#define CTRL_NAME_SIZE_3		32

//CPU
#define CPU_CTRL_SEND_NAME_3	"CpuCtrlSendD_3"
#define CPU_CTRL_POLL_NAME_3	"CpuCtrlPollD_3"
#define CPU_CTRL_SEND_PERIOD_3	((1)*(MUL)*3)
#define CPU_CTRL_POLL_PERIOD_3	((1)*(MUL)*3)

//DSP
#define DSP_CTRL_SEND_NAME_3	"DspCtrlSendD_3"
#define DSP_CTRL_POLL_NAME_3	"DspCtrlPollD_3"
#define DSP_CTRL_SEND_PERIOD_3	((1)*(MUL)*3)
#define DSP_CTRL_POLL_PERIOD_3	((1)*(MUL)*3)
#endif

#ifdef _AIPC_CPU_
#include <linux/kthread.h>
#include <linux/delay.h>

#ifdef CONFIG_RTL8686_IPC_TEST_THREAD
struct ctrl_2dsp_priv{
	char	name [CTRL_NAME_SIZE];
	struct 	mutex		mutex;
	void *  counter_ptr;
	u32_t   period;
};

struct ctrl_2cpu_priv{
	char	name [CTRL_NAME_SIZE];
	struct 	mutex		mutex;
	void *  counter_ptr;
	u32_t   period;
};
#endif

#elif defined(_AIPC_DSP_)
#include <stdio.h>

#ifdef CONFIG_RTL8686_IPC_TEST_THREAD
struct ctrl_2dsp_priv{
	char	name [CTRL_NAME_SIZE];
	cyg_mutex_t mutex;
	void *  counter_ptr;
	u32_t   period;
};

struct ctrl_2cpu_priv{
	char	name [CTRL_NAME_SIZE];
	cyg_mutex_t mutex;
	void *  counter_ptr;
	u32_t   period;
};
#endif

#else	//!_AIPC_CPU_ && !//_AIPC_DSP_
#error "not supported type"
#endif 

/*****************************************************************************
*   Export Function
*****************************************************************************/

/*****************************************************************************
*   Function
*****************************************************************************/

/*****************************************************************************
*   External Function
*****************************************************************************/

/*****************************************************************************
*   Debug Function
*****************************************************************************/

#endif

