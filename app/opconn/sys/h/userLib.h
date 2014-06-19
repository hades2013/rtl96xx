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
* FILENAME:  userLib.h
*
* DESCRIPTION: 
*	
*
* Date Created: Apr 29, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/sys/h/userLib.h#1 $
* $Log:$
*
*
**************************************************************************/
 
#ifndef __USERLIB_H_
#define __USERLIB_H_

#ifdef __cplusplus
extern "C" {
#endif
 
#include "defs.h"
#include "cli.h"

UINT32 odmUserCfgInit(void);
UINT32 odmUserEntryCountGet(void);
char * odmUserNameGet(UINT32 index);
char * odmUserPasswordGet(char *pName);
UINT8 odmUserStateGet(const char *pName);
ACCESS_LEVEL_e odmUserAccessLevelGet(char *pName);
ACCESS_TYPE_e odmUserAccessTypeGet(char *pName);
UINT32 odmUserAccessHostGet(char *pName, char *pHost);
LANGUAGE_e odmUserLanguageGet(char *pName);

UINT32 odmUserAdd(
    const char *pName, 
    OnChangedCallBack pCallBack
    );

UINT32 odmUserAddWithAllParams(
    const char *pName, 
    const char *pPasswd, 
    UINT8 ucState, 
    ACCESS_LEVEL_e tAccessLevel,
    LANGUAGE_e tLanguage,
    OnChangedCallBack pCallBack
    );

UINT32 odmUserPasswordSet(
    const char *pName, 
    const char *pPasswd, 
    OnChangedCallBack pCallBack
    );

UINT32 odmUserStateSet(
    const char *pName, 
    UINT8 ucState, 
    OnChangedCallBack pCallBack
    );

UINT32 odmUserAccessLevelSet(
    const char *pName, 
    ACCESS_LEVEL_e tAccessLevel, 
    OnChangedCallBack pCallBack
    );

UINT32 odmUserLanguageSet(
    const char *pName, 
    LANGUAGE_e tLanguage, 
    OnChangedCallBack pCallBack
    );

UINT32 odmUserDelete(
    const char *pName, 
    OnChangedCallBack pCallBack
    );

int odmUserEncryptString(char * pcTextString);
UINT32 odmUserValidCheck(const char *pName, const char *pPasswd);
UINT32 odmUserExistCheck(const char *pName);

UINT32 odmUserLogin(ENV_t *pstEnv);

void odmUserShow(int fd);
void odmUserDetailShow(int fd);

 
#ifdef __cplusplus
}
#endif
 
#endif /* #ifndef __USERLIB_H_ */
 


