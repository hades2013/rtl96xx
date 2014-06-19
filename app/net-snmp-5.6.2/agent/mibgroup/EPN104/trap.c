/*
 
 File name:trap.c
 Description:
 Author:zhouguanhua
 Date:
         2013/01/14

 */

/*
 * start be including the appropriate header files 
 */
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

/*
 * contains prototypes 
 */
#include "trap.h"
#include "lw_type.h"
#include "lw_drv_pub.h"
#include "lw_drv_req.h"

#define LINKDOWN_TRAP (0)
#define LINKUP_TRAP (1)
#define NO_ERROR (-1)
/*
 * our initialization routine
 * (to get called, the function name must match init_FILENAME() 
 */
void
init_trap(void)
{
    DEBUGMSGTL(("init_trap",
                "initializing (trap)\n"));
    snmp_alarm_register(1,     /* seconds */
                        SA_REPEAT,      /* repeat (every 30 seconds). */
                        send_portlinkUplinkDown,      /* our callback */
                        NULL    /* no callback data needed */
        );
}

/** here we send a SNMP v2 trap (which can be sent through snmpv3 and
 *  snmpv1 as well) and send it out.
 *
 *     The various "send_trap()" calls allow you to specify traps in different
 *  formats.  And the various "trapsink" directives allow you to specify
 *  destinations to receive different formats.
 *  But *all* traps are sent to *all* destinations, regardless of how they
 *  were specified.
 *  
 *  
 *  I.e. it's
 * @verbatim
 *                                           ___  trapsink
 *                                          /
 *      send_easy_trap \___  [  Trap      ] ____  trap2sink
 *                      ___  [ Generator  ]
 *      send_v2trap    /     [            ] ----- informsink
 *                                          \____
 *                                                trapsess
 *  
 *  *Not*
 *       send_easy_trap  ------------------->  trapsink
 *       send_v2trap     ------------------->  trap2sink
 *       ????            ------------------->  informsink
 *       ????            ------------------->  trapsess
 * @endverbatim
 */
void
send_iptrap(char * ipdate)
{

    /*
     * In the notification, we have to assign our notification OID to
     * the snmpTrapOID.0 object. Here is it's definition. 
     */
    oid             objid_snmptrap[] = { 1, 3, 6, 1, 6, 3, 1, 1, 4, 1, 0 };
    size_t          objid_snmptrap_len = OID_LENGTH(objid_snmptrap);//snmpTrapOID.0

    /*
     * define the OID for the notification we're going to send
     * NET-SNMP-EXAMPLES-MIB::netSnmpExampleHeartbeatNotification 
     */
    oid             notification_oid[] =
        { 1, 3, 6, 1, 2, 1, 4, 20, 1, 1, 2 };//snmpTrapOID.0值，ipAdEntAddr
    size_t          notification_oid_len = OID_LENGTH(notification_oid);
   


    oid      hbeat_rate_oid[]   = { 1, 3, 6, 1, 6, 3, 1, 1, 4, 3, 0 };
    size_t   hbeat_rate_oid_len = OID_LENGTH(hbeat_rate_oid);
#if defined(CONFIG_ZBL_SNMP)
	oid             enterpris_oid[] =
        { 1, 3, 6, 1, 4, 1, 39596,2, 1 };
    size_t          enterpris_oid_len = OID_LENGTH(enterpris_oid);
#else
	oid             enterpris_oid[] =
        { 1, 3, 6, 1, 4, 1, NETSNMP_ENTERPRISE_OID,1, 15 };
    size_t          enterpris_oid_len = OID_LENGTH(enterpris_oid);
#endif

    
    oid      hbeat_name_oid[]   = { 1, 3, 6, 1, 2, 1, 4, 20, 1, 1, 2, 0 };//ipAdEntAddr
    size_t   hbeat_name_oid_len = OID_LENGTH(hbeat_name_oid);

    /*
     * here is where we store the variables to be sent in the trap 
     */
    netsnmp_variable_list *notification_vars = NULL;
   // const char *heartbeat_name = "A girl named Maria";

    DEBUGMSGTL(("example_notification", "defining the trap\n"));

    /*
     * add in the trap definition object 
     有多少数据要发送，就调用snmp_varlist_add_variable()多少次
     */
    snmp_varlist_add_variable(&notification_vars,
                              /*
                               * the snmpTrapOID.0 variable 
                               */
                              objid_snmptrap, objid_snmptrap_len,
                              /*
                               * value type is an OID 
                               */
                              ASN_OBJECT_ID,
                              /*
                               * value contents is our notification OID 
                               */
                              (u_char *) notification_oid,
                              /*
                               * size in bytes = oid length * sizeof(oid) 
                               */
                              notification_oid_len * sizeof(oid));

    /*
     * add in the additional objects defined as part of the trap
     */

    snmp_varlist_add_variable(&notification_vars,
                               hbeat_rate_oid, hbeat_rate_oid_len,
                               ASN_OBJECT_ID,
                              (u_char *)enterpris_oid,
                               enterpris_oid_len * sizeof(oid));

    snmp_varlist_add_variable(&notification_vars,
                           hbeat_name_oid, hbeat_name_oid_len,
                           ASN_OCTET_STR,
                           ipdate, strlen(ipdate));
    send_v2trap(notification_vars);
    //send_v3trap(notification_vars,"010203040506");
    /*
     * free the created notification variable list 
     */
    DEBUGMSGTL(("notification", "cleaning up\n"));
    snmp_free_varbind(notification_vars);
}


static void
send_linkUpDownNotifications(oid *notification_oid, size_t notification_oid_len, int if_index, int if_admin_status, int if_oper_status)
{
    /*
     * In the notification, we have to assign our notification OID to
     * the snmpTrapOID.0 object. Here is it's definition. 
     */
    oid             objid_snmptrap[] = { 1, 3, 6, 1, 6, 3, 1, 1, 4, 1, 0 };
    size_t          objid_snmptrap_len = OID_LENGTH(objid_snmptrap);

    /*
     * define the OIDs for the varbinds we're going to include
     *  with the notification -
     * IF-MIB::ifIndex,
     * IF-MIB::ifAdminStatus, and
     * IF-MIB::ifOperStatus
     */
    oid      if_index_oid[]   = { 1, 3, 6, 1, 2, 1, 2, 2, 1, 1, 0 };
    size_t   if_index_oid_len = OID_LENGTH(if_index_oid);
    oid      if_admin_status_oid[]   = { 1, 3, 6, 1, 2, 1, 2, 2, 1, 7, 0 };
    size_t   if_admin_status_oid_len = OID_LENGTH(if_admin_status_oid);
    oid      if_oper_status_oid[]   = { 1, 3, 6, 1, 2, 1, 2, 2, 1, 8, 0 };
    size_t   if_oper_status_oid_len = OID_LENGTH(if_oper_status_oid);

    /*
     * here is where we store the variables to be sent in the trap 
     */
    netsnmp_variable_list *notification_vars = NULL;

    DEBUGMSGTL(("rsys:linkUpDownNotifications", "defining the trap\n"));

    /*
     * update the instance for each variable to be sent in the trap
     */
    if_index_oid[10] = if_index;
    if_admin_status_oid[10] = if_index;
    if_oper_status_oid[10] = if_index;

    /*
     * add in the trap definition object 
     */
    snmp_varlist_add_variable(&notification_vars,
                              /*
                               * the snmpTrapOID.0 variable 
                               */
                              objid_snmptrap, objid_snmptrap_len,
                              /*
                               * value type is an OID 
                               */
                              ASN_OBJECT_ID,
                              /*
                               * value contents is our notification OID 
                               */
                              (u_char *) notification_oid,
                              /*
                               * size in bytes = oid length * sizeof(oid) 
                               */
                              notification_oid_len * sizeof(oid));

    /*
     * add in the additional objects defined as part of the trap
     */
    snmp_varlist_add_variable(&notification_vars,
                               if_index_oid, if_index_oid_len,
                               ASN_INTEGER,
                              (u_char *)&if_index,
                                  sizeof(if_index));

    /*
     * if we want to insert additional objects, we do it here 
     */
    snmp_varlist_add_variable(&notification_vars,
                               if_admin_status_oid, if_admin_status_oid_len,
                               ASN_INTEGER,
                              (u_char *)&if_admin_status,
                                  sizeof(if_admin_status));

    snmp_varlist_add_variable(&notification_vars,
                               if_oper_status_oid, if_oper_status_oid_len,
                               ASN_INTEGER,
                              (u_char *)&if_oper_status,
                                  sizeof(if_oper_status));

    /*
     * send the trap out.  This will send it to all registered
     * receivers (see the "SETTING UP TRAP AND/OR INFORM DESTINATIONS"
     * section of the snmpd.conf manual page. 
     */
    DEBUGMSGTL(("rsys:linkUpDownNotifications", "sending the trap\n"));
    send_v2trap(notification_vars);

    /*
     * free the created notification variable list 
     */
    DEBUGMSGTL(("rsys:linkUpDownNotifications", "cleaning up\n"));
    snmp_free_varbind(notification_vars);
}


void send_linkUpDown_trap(int port,int status)
{
    
    if (LINKUP_TRAP == status) 
    {
        oid notification_oid[] = { 1, 3, 6, 1, 6, 3, 1, 1, 5, 4 };
        send_linkUpDownNotifications(notification_oid, OID_LENGTH(notification_oid),
                                     port,
                                     LINKUP_TRAP,
                                     1);
    } 
    else if (LINKDOWN_TRAP == status) 
    {
        oid notification_oid[] = { 1, 3, 6, 1, 6, 3, 1, 1, 5, 3 };
        send_linkUpDownNotifications(notification_oid, OID_LENGTH(notification_oid),
                                     port,
                                     LINKDOWN_TRAP,                                           
                                     0);
    }
}



void
send_RemoteUpgrade_Error_trap(char * info)
{

    /*
     * In the notification, we have to assign our notification OID to
     * the snmpTrapOID.0 object. Here is it's definition. 
     */
    oid             objid_snmptrap[] = { 1, 3, 6, 1, 6, 3, 1, 1, 4, 1, 0 };
    size_t          objid_snmptrap_len = OID_LENGTH(objid_snmptrap);//snmpTrapOID.0

    /*
     * define the OID for the notification we're going to send
     * NET-SNMP-EXAMPLES-MIB::netSnmpExampleHeartbeatNotification 
     */
    oid             notification_oid[] =
        { 1, 3, 6, 1, 4, 1, NETSNMP_ENTERPRISE_OID, 1, 16, 2, 1, 4};//snmpTrapOID.0  cltConfigRemoteUpgradeFileName
    size_t          notification_oid_len = OID_LENGTH(notification_oid);
   


    oid      hbeat_rate_oid[]   = { 1, 3, 6, 1, 6, 3, 1, 1, 4, 3, 0 };
    size_t   hbeat_rate_oid_len = OID_LENGTH(hbeat_rate_oid);

    oid             enterpris_oid[] =
        { 1, 3, 6, 1, 4, 1, NETSNMP_ENTERPRISE_OID,1, 15};
    size_t          enterpris_oid_len = OID_LENGTH(enterpris_oid);


    
    oid      hbeat_name_oid[]   = {1, 3, 6, 1, 4, 1, NETSNMP_ENTERPRISE_OID, 1, 16, 2, 1, 4,0};//cltConfigRemoteUpgradeFileName
    size_t   hbeat_name_oid_len = OID_LENGTH(hbeat_name_oid);

    /*
     * here is where we store the variables to be sent in the trap 
     */
    netsnmp_variable_list *notification_vars = NULL;
   // const char *heartbeat_name = "A girl named Maria";

    DEBUGMSGTL(("netsnmp_udp", "defining the trap\n"));

    /*
     * add in the trap definition object 
     有多少数据要发送，就调用snmp_varlist_add_variable()多少次
     */
    snmp_varlist_add_variable(&notification_vars,
                              /*
                               * the snmpTrapOID.0 variable 
                               */
                              objid_snmptrap, objid_snmptrap_len,
                              /*
                               * value type is an OID 
                               */
                              ASN_OBJECT_ID,
                              /*
                               * value contents is our notification OID 
                               */
                              (u_char *) notification_oid,
                              /*
                               * size in bytes = oid length * sizeof(oid) 
                               */
                              notification_oid_len * sizeof(oid));

    /*
     * add in the additional objects defined as part of the trap
     */

    snmp_varlist_add_variable(&notification_vars,
                               hbeat_rate_oid, hbeat_rate_oid_len,
                               ASN_OBJECT_ID,
                              (u_char *)enterpris_oid,
                               enterpris_oid_len * sizeof(oid));

    snmp_varlist_add_variable(&notification_vars,
                           hbeat_name_oid, hbeat_name_oid_len,
                           ASN_OCTET_STR,
                           info, strlen(info));
    send_v2trap(notification_vars);
    /*
     * free the created notification variable list 
     */
    DEBUGMSGTL(("netsnmp_udp", "cleaning up\n"));
    snmp_free_varbind(notification_vars);
}

INT32 DRV_GetPortLinkStatus(UINT32 lport, BOOL* pStatus)
{
    unsigned int linksts = 0;
    if(NULL == pStatus)
    {
        return ERROR;
    }
    if(FALSE == IsValidLgcPort(lport)) 
    {
        return ERROR;
    }
    if(DRV_OK == Ioctl_GetPortCurrentLink(lport, &linksts))
    {
        switch (linksts)
        {
            case TRUE :
                *pStatus = TRUE;
                break;
            case FALSE :
                *pStatus = FALSE;
                break;
            default:
                return ERROR;
        }
    }
    return NO_ERROR;
}
//begin modify by zhouguanhua 2013/2/28 for EPN204QID0023
void send_portlinkUplinkDown()
{
    UINT8 state = 0;
	UINT8 port=1;
	BOOL bPortStatus = TRUE;
	static UINT8 snmp_first_start=1;	
	static UINT8 state_temp[4] = {0};
	for(port=1; port< (LOGIC_PORT_NO + 1); port++)
	{
		if ( NO_ERROR == DRV_GetPortLinkStatus((UINT32) port, &bPortStatus))
        {
            if ( TRUE == bPortStatus )
            {
                state = LINKUP_TRAP;
            }
            else
            {
                state = LINKDOWN_TRAP;
            }
			if(state_temp[port-1]!=state)
			{	
				state_temp[port-1]=state;
				if(!snmp_first_start)
				send_linkUpDown_trap(port,state);
			}
			if(port==4)
            snmp_first_start = 0;
        }	
	}
}
//end modify by zhouguanhua 2013/2/28 forEPN204QID0023
