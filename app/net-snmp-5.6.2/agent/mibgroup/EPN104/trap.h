
#ifndef TRAP_H
#define TRAP_H

config_add_mib(EPN104-MIB)
config_require(util_funcs/header_generic)
config_require(EPN104/trap)

/*
 * function declarations 
 */
void init_trap(void);
void send_portlinkUplinkDown();
void send_iptrap(char * ipdate);
void send_linkUpDown_trap(int port,int status);
void send_RemoteUpgrade_Error_trap(char * info);
#endif                          /* MISCIDENT_H */
