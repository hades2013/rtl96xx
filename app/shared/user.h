/*****************************************************************************

      File name:user.h
      Description:provide funcs of user interface
      Author:liaohongjun
      Date:
              2012/8/17
==========================================================
      Note:
*****************************************************************************/
#ifndef _CMD_USER_H_
#define _CMD_USER_H_
    
#ifdef  __cplusplus
    extern "C"{
#endif

#include "lw_type.h"
#include "pdt_config.h"

#define MAX_VTY_COUNT 3
#define MIN_VTY_USER_NO 0
#define VTY_USER_NO_OF_CONSOLE 0
#define MIN_VTY_USER_NO_OF_TELNET 1
#define MAX_VTY_USERNAME_LEN 16
#define MAX_VTY_PASSWD_LEN 32
#define MAX_AUTHMODE_TXT_LEN 16
#define MAX_VTY_PASSWD_TYPE_LEN 16

#define VTY_AUTH_INPUT_TIMEOUT 60

#define DEFAULT_USERNAME    "admin"
#define DEFAULT_VTY_PASSWD      "admin"
#define VTY_PASSWD_NULL         ""
#define DEFAULT_CONSOL_AUTH_MODE   "none"
#define DEFAULT_TELNET_AUTH_MODE   "password"
#define DEFAULT_PASSWD_TYPE     "cleartext"
#define DEFAULT_VTY_TIMEOUT     "300"

#define AUTH_MODE_NONE      "none"
#define AUTH_MODE_PASSWORD    "password"
#define PASSWD_TYPE_CLEARTEXT   "cleartext"
#define PASSWD_TYPE_CIPHERTEXT  "ciphertext"

#define PARTS_VTY_USERNAME_ITEM    "user_vty%d_name"
#define PARTS_VTY_AUTHMODE_ITEM    "user_vty%d_authmode"
#define PARTS_VTY_AUTHPASSWD_ITEM  "user_vty%d_password"
#define PARTS_VTY_PASSWD_TYPE_ITEM "user_vty%d_password_type"
#define PARTS_VTY_TIMEOUT_ITEM     "user_vty%d_timeout"
#define DEFAULT_LOOP_ENABLE 0

typedef struct tag_vty_user_info
{
    int active;
    int vty_sockfd;
    //int login;
    char username[MAX_VTY_USERNAME_LEN+1];
    char authmode[MAX_AUTHMODE_TXT_LEN+1];
    char passwd[MAX_VTY_PASSWD_LEN+1];
    char passwd_type[MAX_VTY_PASSWD_TYPE_LEN+1];
    unsigned long timeout;
    /*Begin modified by feihuaxin  for bug ID 78 2013/5/28*/
    char login_flag;
    /*End modified by feihuaxin  for bug ID 78 2013/5/28*/

}VTY_USER_INFO_S;

extern int vty_user_init(void);
void cmd_vty_user_init(void);

#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */

#endif

