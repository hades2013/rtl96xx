/* upldForm.c - GoForm to handle file upload */

/*
modification history
--------------------
01a,13feb01,lohct  written.
*/

/*
DESCRIPTION
This GoForm procedure handles http file upload.

SEE ALSO:
"GoForms - GoAhead WebServer"
*/

#include "webs.h"
#include <ufile_process.h>

#include "ipc_client.h"

extern int ipc_fd;


/*******************************************************************************
*
* upldForm - GoForm procedure to handle file uploads for upgrading device
*
* This routine handles http file uploads for upgrading device.
*
* RETURNS: OK, or ERROR if the I/O system cannot install the driver.
*/

void upldForm(webs_t wp, char_t * path, char_t * query)
{
	int delay = 0, ufile = 0, imported = 0;
	char *msg;
	char *type;
	char *go = "index.asp";
	char goto_url[128];
	int ret = UP_OK;
	ipc_system_ack_t *pack = NULL;	
	char *up_fname=NULL;
	int len=0;
	
	type = websGetVar(wp, T("TYPE"), "");	

	if (!strcmp(type, "UFILE") ){
		ufile = 1;
	}
    
    /*begin midified by liaohongjun 2012/9/28 of EPN104QID0056*/
    if(wp->flags & WEBS_POST_CLEN_OVERFLOW){
        ret = UP_ERR_LENGTH;
    } 
	else if (!strcmp(type, "UFILE")){
        /*end midified by liaohongjun 2012/9/28 of EPN104QID0056*/
	    /*Begin modify by jiangzhenhua for EPN104QID0037  2012/9/25*/
		up_fname = websGetVar(wp, T("up_file"), "");
		len = strlen(up_fname);
		if(len < 4)
		{
			ret = UP_ERR_EXT;
		}

		if(ret == UP_OK)
		{
			len = strlen(up_fname);
			if(strncmp(&up_fname[len-4], ".bin", 4))
			{
				ret = UP_ERR_EXT;
			}
		}
	    #if 1
        /*Begin add by huangmingjian 2014-04-24 for Bug 551*/	
		if(ret == UP_OK)
		{ 
			ret = fw_head_check((unsigned char *)wp->postData, wp->lenPostData);
		}  
		/*End add by huangmingjian 2014-04-24 for Bug 551*/
	    #else
		if(ret == UP_OK){
		ret = ufile_validate((unsigned char *)wp->postData, wp->lenPostData);
		}
		#endif
		/*End modify by jiangzhenhua for EPN104QID0037  2012/9/25*/
		if (ret == UP_OK){
			ret = ufile_process((unsigned char *)wp->postData, wp->lenPostData);
		}	
		if (ret == UP_OK){
			pack = ipc_system_req(ipc_fd, IPC_SYS_UPGRADE_APP, ufile_content());
		}
	} else if (!strcmp(type, "CONFIG")){
	        /*begin modify by liaohongjun 2012/8/1 of QID0003*/
	       // ret = cfgFileValidate((unsigned char *)wp->postData, wp->lenPostData);
           // if(UP_OK == ret){I
    			ret = ufile_output_config((unsigned char *)wp->postData, wp->lenPostData);
    			if (ret == UP_OK){
    				pack = ipc_system_req(ipc_fd, IPC_SYS_CONFIG_READY, NULL);
                    /*end modify by liaohongjun 2012/8/1 of QID0003*/
    				if (pack && (pack->hdr.status == IPC_STATUS_OK)){
    					imported = pack->percentage;
    				}
    			}
           // }
	}else {
		ret = UP_ERR_UNKNOWN;
	}

	if ((ret == UP_OK) && pack && (pack->hdr.status != IPC_STATUS_OK)){
		ret = UP_ERR_IPC;
	}

	if (pack) free(pack);	

	
	if (ret != UP_OK){
		msg = "fail";
		delay = 0;
	}else if (ufile){
		msg = "upgrade";
		delay = UPGRADE_TIME; 
	}else {
		msg = "cfgimport";
		delay = CFGIMPORT_TIME;
		ret = imported;
	}
	
	sprintf(goto_url, "/do_cmd.asp?DELAY=%d&ERR=%d&MSG=%s&GO=%s", delay, ret, msg, go);
	websRedirect(wp, goto_url);

	return;
}   



