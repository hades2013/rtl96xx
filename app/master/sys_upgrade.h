#ifndef __SYS_UPGRADE_H__
#define __SYS_UPGRADE_H__

typedef enum{
	SYS_OK = 0,	
	SYS_ERROR,
	SYS_ERROR_PARA
}SYS_RETURN_E;

int sys_perform_upgrade(int process);
int sys_is_performing_upgrade(void);
int sys_upgrade_state(int reset);
int sys_upgrade_error(void);
int sys_upgrade_percentage(void);
int sys_upgrade_app(int process);
int sys_import_config();





#endif /*__SYS_UPGRADE_H__*/

