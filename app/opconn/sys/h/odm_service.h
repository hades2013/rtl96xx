/*************************************************************************
*
*  COPYRIGHT (C) 2003-2008 Opulan Technologies Corp. ALL RIGHTS RESERVED.  
*
*                       Proprietary and Confidential
*
* 	This software is made available only to customers and prospective
* 	customers of Opulan Technologies Corporation under license and may be
*	used only with Opulan semi-conductor products. 
*
* FILENAME:  odm_service.h
*
* DESCRIPTION: 
*	
*
* Date Created: Sep 12, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/sys/h/odm_service.h#1 $
* $Log:$
*
*
**************************************************************************/
 
#ifndef __ODM_SERVICE_H_
#define __ODM_SERVICE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <vos.h>
 
STATUS odmServiceInit();
STATUS odmServiceShutdown();

STATUS odmOamTimerThreadInit(void);
STATUS odmOamTimerThreadShutdown(void);

void odmInterruptHandler(void *pvParam);

STATUS odmLOSLedInit(void);
STATUS odmAlarmLedControl(UINT8 ucOn);

STATUS odmEventThreadInit(void);
STATUS odmEventThreadShutdown(void);

STATUS odmUart1TestThreadInit(void);
STATUS odmUart1TestThreadShutdown(void);



#ifdef __cplusplus
}
#endif
 
#endif /* #ifndef __ODM_SERVICE_H_ */
 

