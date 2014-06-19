
#ifndef __CMD_EOC_H__ 
#define __CMD_EOC_H__

#include <zebra.h>
#include "command.h"
#include "vty.h"
#include <vendor.h>


void cmd_systemconfig_init(void);
void cmd_interface_init(void);
void cmd_networkinfo_init(void);
void cmd_vlan_init(void);


#endif /*#ifndef __CMD_EOC_H__*/

