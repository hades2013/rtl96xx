

#include "master.h"
#include <basetype.h>
#include "ufile_process.h"
#include "mtd_utils.h"
#include <ipc_protocol.h>
#include "sys_upgrade.h"
#include <lw_config.h>
//#include <vos_config.h>

typedef struct {
	int process;
	int *sequence;
	int current;
	int state;
	int error;
	int percentage;
	mtd_on_state_change_t status_callback;	
	uint8_t *buffer;
	uint32_t buffer_size;
}
sys_upgrade_handle_t;

#ifdef cltmon_log
#undef cltmon_log

#endif
#define cltmon_log(fmt,arg...)  


static sys_upgrade_handle_t upgrade_handle;

#define set_state(h, s, e) do { \
	(h)->state = (s); \
	(h)->error = (e); \
}while(0)
/*****************************************************************
Function:app_upgrade_start
Description:process app upgrade   
Author:liaohongjun
Date:2012/7/10
INPUT:sys_upgrade_handle_t *handle       
OUTPUT:         
Return:
    SYS_ERROR_PARA   //paras error
    SYS_ERROR          //error
    SYS_OK    //succed
Others:        
*****************************************************************/
static int app_upgrade_start(sys_upgrade_handle_t *handle)
{
    int iRet = MTD_OP_IOERR;
    char appPath[64] = {0};
    
    if(NULL == handle)
    {
        return SYS_ERROR_PARA;
    }
    
    /*begin modified by liaohongjun 2012/8/1 of QID0003*/
   	/*Begin modify by zhouguanhua 2013-4-16 for BugID0005*/
	 sprintf(appPath, "/tmp/%s", APP_UPGRADE_FILE);
	/*End modify by zhouguanhua 2013-4-16 for   BugID0005*/
	switch(handle->current){
		case UFILE_CONTENT_KERNEL:
			handle->buffer = upgrade_content_read_from_app(appPath, &handle->buffer_size, READ_CONTENT_OF_KERNEL);
			if (handle->buffer == NULL){
				set_state(handle, UFILE_STATE_FAILED, MTD_OP_NOMEM);
			}
			cltmon_log(LOG_INFO, "System is upgrading kernel");
            if(UPGRADE_APP_0 == handle->process)
            {
    			mtd_write_start(MTD_APP_0_KERNEL, handle->buffer, handle->buffer_size, handle->status_callback);
            }
            else
            {
    			mtd_write_start(MTD_APP_1_KERNEL, handle->buffer, handle->buffer_size, handle->status_callback);
            }
			break;
		case UFILE_CONTENT_ROOTFS:
			handle->buffer = upgrade_content_read_from_app(appPath, &handle->buffer_size, READ_CONTENT_OF_ROOTFS);
			if (handle->buffer == NULL){
				set_state(handle, UFILE_STATE_FAILED, MTD_OP_NOMEM);
			}
			cltmon_log(LOG_INFO, "System is upgrading root filesystem");			
            if(UPGRADE_APP_0 == handle->process)
            {
    			mtd_write_start(MTD_APP_0_ROOTFS, handle->buffer, handle->buffer_size, handle->status_callback);
            }
            else
            {
    			mtd_write_start(MTD_APP_1_ROOTFS, handle->buffer, handle->buffer_size, handle->status_callback);
            }
			break;			
		default:
			set_state(handle, UFILE_STATE_FAILED, MTD_OP_UNKNOWN);
	}

    return SYS_OK;    
}

#if 0
static void sys_upgrade_start(sys_upgrade_handle_t *handle)
{
	int ret;
	
	switch(handle->current){
		case UFILE_CONTENT_BOOT:
			handle->buffer = ufile_read(UFILE_TMP_BOOT, &handle->buffer_size);
			if (handle->buffer == NULL){
				set_state(handle, UFILE_STATE_FAILED, MTD_OP_NOMEM);
			}
			cltmon_log(LOG_INFO, "System is upgrading bootloader");
			mtd_write_start(MTD_BOOT, handle->buffer, handle->buffer_size, handle->status_callback);
			break;
		case UFILE_CONTENT_KERNEL:
			handle->buffer = ufile_read(UFILE_TMP_KERNEL, &handle->buffer_size);
			if (handle->buffer == NULL){
				set_state(handle, UFILE_STATE_FAILED, MTD_OP_NOMEM);
			}
			cltmon_log(LOG_INFO, "System is upgrading kernel");
			mtd_write_start(MTD_KERNEL, handle->buffer, handle->buffer_size, handle->status_callback);
			break;
		case UFILE_CONTENT_ROOTFS:
			handle->buffer = ufile_read(UFILE_TMP_ROOTFS, &handle->buffer_size);
			if (handle->buffer == NULL){
				set_state(handle, UFILE_STATE_FAILED, MTD_OP_NOMEM);
			}
			cltmon_log(LOG_INFO, "System is upgrading root filesystem");			
			mtd_write_start(MTD_ROOTFS, handle->buffer, handle->buffer_size, handle->status_callback);
			break;			
		case UFILE_CONTENT_FILE:	
			handle->status_callback(MTD_DONE, MTD_OP_OK, 0);
			break;	
		case UFILE_CONTENT_CONFIG:
            /*begin modified by liaohongjun 2012/6/29*/
            #if 0
			if ((ret = nvram_restore(UFILE_TMP_CONFIG)) <= 0){
				handle->status_callback(MTD_DONE, MTD_OP_INVALID, 0);	
			}else {
				handle->status_callback(MTD_DONE, MTD_OP_OK, ret);// set how many imported	
			}
            #endif
			if ((ret = nvram_restore_for_file(UFILE_TMP_CONFIG)) <= 0){
				handle->status_callback(MTD_DONE, MTD_OP_INVALID, 0);	
			}else {
				handle->status_callback(MTD_DONE, MTD_OP_OK, ret);// set how many imported	
			}       
            /*end modified by liaohongjun 2012/6/29*/
			break;
		default:
			set_state(handle, UFILE_STATE_FAILED, MTD_OP_UNKNOWN);
	}
}
#endif
/*****************************************************************
Function:app_upgrade_process
Description:process app upgrade   
Author:liaohongjun
Date:2012/7/10
INPUT:sys_upgrade_handle_t *handle       
OUTPUT:         
Return:
    SYS_ERROR_PARA   //paras error
    SYS_ERROR          //error
    SYS_OK    //succed
Others:	       
*****************************************************************/
static int app_upgrade_process_next(sys_upgrade_handle_t *handle)
{        
    while(handle->sequence[0] > 0)
    {
        // we should perform this
        handle->current = handle->sequence[0];
        handle->sequence ++;
        return app_upgrade_start(handle);
    }

    if (handle->error == MTD_OP_OK)
    {
        cltmon_log(LOG_INFO, "System firmware upgrade successfully, system is going to reboot");        
        handle->state = UFILE_STATE_DONE;
        if (handle->buffer)
        {
            free(handle->buffer);
            handle->buffer = NULL;
        }
        if(UPGRADE_APP_0 == handle->process)
        {
            system("echo 0 > /proc/manufactory/bootflag");
        }
        else if(UPGRADE_APP_1 == handle->process)
        {
            system("echo 1 > /proc/manufactory/bootflag");
        }
        sys_reboot_later(4);// 4 seconds later
    }
    
    return SYS_OK;
}
#if 0
static void sys_upgrade_active_next(sys_upgrade_handle_t *handle)
{
//	DBG_PRINTF("sys upgrade active next");
	
	while(handle->sequence[0] > 0){
		if (!(handle->process & handle->sequence[0])){
			handle->sequence ++;
		}else {// we should perform this
			handle->current = handle->sequence[0];
			handle->sequence ++;
			sys_upgrade_start(handle);
			return ;
		}
	}

	if (handle->error == MTD_OP_OK){
		cltmon_log(LOG_INFO, "System firmware upgrade successfully, system is going to reboot");		
		handle->state = UFILE_STATE_DONE;
		sys_reboot_later(4);// 4 seconds later
	}
}
#endif
/*****************************************************************
Function:app_upgrade_status_callback
Description:app upgrade  
Author:liaohongjun
Date:2012/7/10
INPUT:mtd_state_t state, int error, int percentage        
OUTPUT:         
Return:
          void
Others:	       
*****************************************************************/
static void app_upgrade_status_callback(mtd_state_t state, int error, int percentage)
{
    sys_upgrade_handle_t *handle = &upgrade_handle;

//BG_PRINTF("Upgrade status change to :%d.%d.%d ", handle->current, state, percentage);

    handle->percentage= percentage;

	switch(handle->current){
		case UFILE_CONTENT_KERNEL:
			if (state == MTD_ERASING){
				set_state(handle, UFILE_STATE_KERNEL_ERASE, MTD_OP_OK);
			}else if (state == MTD_WRITING){
				set_state(handle, UFILE_STATE_KERNEL_WRITE, MTD_OP_OK);
			}else if (state == MTD_VERIFING){
				set_state(handle, UFILE_STATE_KERNEL_VERIFY, MTD_OP_OK);
			}

			break;
		case UFILE_CONTENT_ROOTFS:
			if (state == MTD_ERASING){
				set_state(handle, UFILE_STATE_ROOTFS_ERASE, MTD_OP_OK);
			}else if (state == MTD_WRITING){
				set_state(handle, UFILE_STATE_ROOTFS_WRITE, MTD_OP_OK);
			}else if (state == MTD_VERIFING){
				set_state(handle, UFILE_STATE_ROOTFS_VERIFY, MTD_OP_OK);
			}
			break;
        default:
            set_state(handle, UFILE_STATE_FAILED, MTD_OP_UNKNOWN);
            return;
    }

    if (state == MTD_DONE)
    {
        if (error != MTD_OP_OK)
        {
            if (handle->buffer)
            {
                free(handle->buffer);
                handle->buffer = NULL;
            }            
            set_state(handle, UFILE_STATE_FAILED, error);
        }
        else 
        {
            if (handle->buffer)
            {
                free(handle->buffer);
                handle->buffer = NULL;
            }            
            app_upgrade_process_next(handle);
        }
        // free the buffer
        #if 0
        if (handle->buffer)
        {
            free(handle->buffer);
            handle->buffer = NULL;
        }   
        #endif
    }

    return;
}

/*****************************************************************
    Function:config_import_status_callback
    Description: import config file status callback
    Author:liaohongjun
    Date:2012/8/1
    Input:mtd_state_t state, int error, int percentage    
    Output:         
    Return:
        void  
=================================================
    Note: added by liaohongjun 2012/8/1 of QID0003
*****************************************************************/
static void config_import_status_callback(mtd_state_t state, int error, int percentage)
{
    sys_upgrade_handle_t *handle = &upgrade_handle;
    handle->percentage= percentage;

    if (state == MTD_DONE)
    {
        if (error != MTD_OP_OK)
        {
            set_state(handle, UFILE_STATE_FAILED, error);
        }
        else 
        {
            cltmon_log(LOG_INFO, "System configuration file has been imported successfully, system is going to reboot");
            handle->state = UFILE_STATE_DONE;
            sys_reboot_later(4);// 4 seconds later
        }
        // free the buffer
        if (handle->buffer)
        {
            free(handle->buffer);
            handle->buffer = NULL;
        }        
    }

    return;
}

#if 0
static void sys_upgrade_status_callback(mtd_state_t state, int error, int percentage)
{
	sys_upgrade_handle_t *handle = &upgrade_handle;

//	DBG_PRINTF("Upgrade status change to :%d.%d.%d ", handle->current, state, percentage);
	
	handle->percentage= percentage;
	
	switch(handle->current){
		case UFILE_CONTENT_BOOT:
			if (state == MTD_ERASING){
				set_state(handle, UFILE_STATE_BOOT_ERASE, MTD_OP_OK);
			}else if (state == MTD_WRITING){
				set_state(handle, UFILE_STATE_BOOT_WRITE, MTD_OP_OK);
			}else if (state == MTD_VERIFING){
				set_state(handle, UFILE_STATE_BOOT_VERIFY, MTD_OP_OK);
			}
			break;
		case UFILE_CONTENT_KERNEL:
			if (state == MTD_ERASING){
				set_state(handle, UFILE_STATE_KERNEL_ERASE, MTD_OP_OK);
			}else if (state == MTD_WRITING){
				set_state(handle, UFILE_STATE_KERNEL_WRITE, MTD_OP_OK);
			}else if (state == MTD_VERIFING){
				set_state(handle, UFILE_STATE_KERNEL_VERIFY, MTD_OP_OK);
			}

			break;
		case UFILE_CONTENT_ROOTFS:
			if (state == MTD_ERASING){
				set_state(handle, UFILE_STATE_ROOTFS_ERASE, MTD_OP_OK);
			}else if (state == MTD_WRITING){
				set_state(handle, UFILE_STATE_ROOTFS_WRITE, MTD_OP_OK);
			}else if (state == MTD_VERIFING){
				set_state(handle, UFILE_STATE_ROOTFS_VERIFY, MTD_OP_OK);
			}
			break;	
		case UFILE_CONTENT_FILE: // do nothing, just for receiving MTD_DONE
			break;
		case UFILE_CONTENT_CONFIG:
			break;
		default:
			set_state(handle, UFILE_STATE_FAILED, MTD_OP_UNKNOWN);
			return;
			
	}	

// Active next state 

	if (state == MTD_DONE){
		if (error != MTD_OP_OK){
			set_state(handle, UFILE_STATE_FAILED, error);
		}else {
			sys_upgrade_active_next(handle);
		}	
		// free the buffer
		if (handle->buffer){
			free(handle->buffer);
			handle->buffer = NULL;
		}
	}	
}
#endif

int sys_is_performing_upgrade(void)
{
	//fprintf(stderr, "upgrade state get\n");
	switch(upgrade_handle.state){
		case UFILE_STATE_IDLE:
		case UFILE_STATE_DONE:
		case UFILE_STATE_FAILED:
			return 0;
	}
	return 1;
//	return (upgrade_handle.state == UFILE_STATE_IDLE) ? 0 : 1;
}


int sys_upgrade_state(int reset)
{
	if (reset){
		memset(&upgrade_handle, 0, sizeof(upgrade_handle));
	}
	return upgrade_handle.state;
}

int sys_upgrade_error(void)
{
	return upgrade_handle.error;
}

int sys_upgrade_percentage(void)
{
	return upgrade_handle.percentage;
}

/*****************************************************************
Function:sys_upgrade_app
Description:app upgrade  
Author:liaohongjun
Date:2012/7/10
INPUT:int process        
OUTPUT:         
Return:
    SYS_ERROR_PARA   //paras error
    SYS_ERROR          //error
    SYS_OK    //succed
Others:	       
*****************************************************************/
int sys_upgrade_app(int process)
{
	static int sequences[] = {UFILE_CONTENT_KERNEL, UFILE_CONTENT_ROOTFS, 0}; 
    
    if((UPGRADE_APP_0 != process) && (UPGRADE_APP_1 != process))
    {
        return SYS_ERROR_PARA;
    }
    
    if (sys_is_performing_upgrade())
    {
        cltmon_log(LOG_ERR, "System is performing firmware upgrade");
        return SYS_ERROR;
    }
    /*init the upgrade_handle*/
    (void)sys_upgrade_state(1);
    upgrade_handle.process = process;
    upgrade_handle.sequence = sequences;
    upgrade_handle.status_callback = app_upgrade_status_callback;
    if(SYS_OK != app_upgrade_process_next(&upgrade_handle))
    {
        return SYS_ERROR;
    }

    return SYS_OK;
}

/*****************************************************************
    Function:sys_import_config
    Description: import config file
    Author:liaohongjun
    Date:2012/8/1
    Input:   
    Output:         
    Return:
        SYS_ERROR          //error
        SYS_OK    //succed    
=================================================
    Note: added by liaohongjun 2012/8/1 of QID0003
*****************************************************************/
#define UFILE_TMP_CONFIG "/tmp/config.bin"

int sys_import_config()
{
    int ret = 0;
    
    
    if (sys_is_performing_upgrade())
    {
        cltmon_log(LOG_ERR, "System is performing firmware upgrade");
        return SYS_ERROR;
    }
    /*init the upgrade_handle*/
    (void)sys_upgrade_state(1);
    upgrade_handle.status_callback = config_import_status_callback;
    //if ((ret = nvram_restore_for_file(UFILE_TMP_CONFIG)) < 0)
    if ((ret = cfg_restore_for_file(UFILE_TMP_CONFIG)) < 0)
    {
        DBG_PRINTF("import config file error!");
        upgrade_handle.status_callback(MTD_DONE, MTD_OP_INVALID, 0);
        return SYS_ERROR;
    }
    else 
    {
        upgrade_handle.status_callback(MTD_DONE, MTD_OP_OK, ret);// set how many imported
    }       
   
    return SYS_OK;
}
#if 0
int sys_perform_upgrade(int process)
{
	static int sequences[] = {UFILE_CONTENT_BOOT, UFILE_CONTENT_KERNEL, UFILE_CONTENT_ROOTFS, UFILE_CONTENT_FILE, UFILE_CONTENT_CONFIG, 0}; 	

	if (sys_is_performing_upgrade()){
		cltmon_log(LOG_ERR, "System is performing firmware upgrade");
		return 0;
	}

	if ((process & UFILE_CONTENT_BOOT) && !ufile_check(UFILE_TMP_BOOT)){
		cltmon_log(LOG_ERR, "Could not find upgrade file for bootloader");
		return 0;
	}

	if ((process & UFILE_CONTENT_KERNEL) && !ufile_check(UFILE_TMP_KERNEL)){
		cltmon_log(LOG_ERR, "Could not find upgrade file for kernel");
		return 0;
	}

	if ((process & UFILE_CONTENT_ROOTFS) && !ufile_check(UFILE_TMP_ROOTFS)){
		cltmon_log(LOG_ERR, "Could not find upgrade file for root filesystem");
		return 0;
	}

	if ((process & UFILE_CONTENT_CONFIG) && !ufile_check(UFILE_TMP_CONFIG)){
		cltmon_log(LOG_ERR, "Could not find upgrade file for configuration");
		return 0;
	}	

	// initial
	sys_upgrade_state(1);// reset
	upgrade_handle.sequence = sequences;
	upgrade_handle.process = process;
	upgrade_handle.status_callback = sys_upgrade_status_callback;
	sys_upgrade_active_next(&upgrade_handle);
	return 1;
}
#endif







