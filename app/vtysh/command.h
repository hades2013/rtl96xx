/*
 * Zebra configuration command interface routine
 * Copyright (C) 1997, 98 Kunihiro Ishiguro
 *
 * This file is part of GNU Zebra.
 *
 * GNU Zebra is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2, or (at your
 * option) any later version.
 * 
 * GNU Zebra is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Zebra; see the file COPYING.  If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef _ZEBRA_COMMAND_H
#define _ZEBRA_COMMAND_H

#include "vector.h"
#include "vty.h"
#include "pdt_config.h"
#include "basetype.h"

#define CMD_CURRENT_SUPPORT 0

/* Host configuration variable */
struct host_t
{
  /* Host name of this router. */
  char *name;

  /* User name and Password for vty interface. */
  char *username;
  char *password;
  char *password_encrypt;

  /* Enable password */
  char *enable;
  char *enable_encrypt;

  /* System wide terminal lines. */
  int lines;

  /* Log filename. */
  char *logfile;

  /* config file name of this host */
  char *config;

  /* Flags for services */
  int advanced;
  int encrypt;

  /* Banner configuration. */
  const char *motd;
  char *motdfile;
};

/* There are some command levels which called from command node. */
enum node_type 
{
  /*begin added by liaohongjun 2012/9/5 of QID0015*/
  ENTER_NODE,       /*enter mode of vty interface.*/ 
  /*end added by liaohongjun 2012/9/5 of QID0015*/
  AUTH_NODE,		/* Authentication user mode of vty interface. */
  AUTH_PASSWD_NODE,	/* Authentication password mode of vty interface. */
  RESTRICTED_NODE,		/* Restricted view mode */ 
  VIEW_NODE,			/* View node. Default mode of vty interface. */
  AUTH_ENABLE_NODE,		/* Authentication mode for change enable. */
  ENABLE_NODE,			/* Enable node. */
  CONFIG_NODE,			/* Config node. Default mode of config file. */
  /*begin added by liaohongjun 2012/9/18 of QID0019*/
  VLAN_INTERFACE_NODE,   /* vlan interface node. */
  /*end added by liaohongjun 2012/9/18 of QID0019*/
  SERVICE_NODE, 		/* Service node. */
  DEBUG_NODE,			/* Debug node. */
  AAA_NODE,			/* AAA node. */
  KEYCHAIN_NODE,		/* Key-chain node. */
  KEYCHAIN_KEY_NODE,		/* Key-chain key node. */
  INTERFACE_NODE,		/* Interface mode node. */
  ZEBRA_NODE,			/* zebra connection node. */
  TABLE_NODE,			/* rtm_table selection node. */
  RIP_NODE,			/* RIP protocol mode node. */ 
  RIPNG_NODE,			/* RIPng protocol mode node. */
  BGP_NODE,			/* BGP protocol mode which includes BGP4+ */
  BGP_VPNV4_NODE,		/* BGP MPLS-VPN PE exchange. */
  BGP_IPV4_NODE,		/* BGP IPv4 unicast address family.  */
  BGP_IPV4M_NODE,		/* BGP IPv4 multicast address family.  */
  BGP_IPV6_NODE,		/* BGP IPv6 address family */
  BGP_IPV6M_NODE,		/* BGP IPv6 multicast address family. */
  OSPF_NODE,			/* OSPF protocol mode */
  OSPF6_NODE,			/* OSPF protocol for IPv6 mode */
  ISIS_NODE,			/* ISIS protocol mode */
  MASC_NODE,			/* MASC for multicast.  */
  IRDP_NODE,			/* ICMP Router Discovery Protocol mode. */ 
  IP_NODE,			/* Static ip route node. */
  ACCESS_NODE,			/* Access list node. */
  PREFIX_NODE,			/* Prefix list node. */
  ACCESS_IPV6_NODE,		/* Access list node. */
  PREFIX_IPV6_NODE,		/* Prefix list node. */
  AS_LIST_NODE,			/* AS list node. */
  COMMUNITY_LIST_NODE,		/* Community list node. */
  RMAP_NODE,			/* Route map node. */
  SMUX_NODE,			/* SNMP configuration node. */
  DUMP_NODE,			/* Packet dump node. */
  FORWARDING_NODE,		/* IP forwarding node. */
  PROTOCOL_NODE,                /* protocol filtering node */
  VTY_NODE,			/* Vty node. */
  SU_NODE,			/* Super user node */
  SYSCONFIG_NODE
};

/* Node which has some commands and prompt string and configuration
   function pointer . */
struct cmd_node 
{
  /* Node index. */
  enum node_type node;		

  /* Prompt character at vty interface. */
  char *prompt;
  /* Prompt function if prompt is NULL */
  char *(*prompt_func)(struct vty *);

  /* Is this node's configuration goes to vtysh ? */
  int vtysh;
  
  /* Node's configuration write function */
  int (*func) (struct vty *);

  /* Vector of this node's command list. */
  vector cmd_vector;	
};

enum
{
  CMD_ATTR_DEPRECATED = 1,
  CMD_ATTR_HIDDEN,
};

/*Begin add by huangmingjian 2012/08/23 for EPN104QID0008*/ 
typedef enum {
	HOSTNAME = 0,
	SUPER_PASSWD = 1
};
/*End add by huangmingjian 2012/08/23 for EPN104QID0008 */


/* Structure of command element. */
struct cmd_element 
{
  const char *string;			/* Command specification by string. */
  int (*func) (struct cmd_element *, struct vty *, int, const char *[]);
  const char *doc;			/* Documentation of this command. */
  int daemon;                   /* Daemon to which this command belong. */
  vector strvec;		/* Pointing out each description vector. */
  unsigned int cmdsize;		/* Command index count. */
  char *config;			/* Configuration string */
  vector subconfig;		/* Sub configuration string */
  u_char attr;			/* Command attributes */
};

/* Command description structure. */
struct desc
{
  char *cmd;                    /* Command string. */
  char *str;                    /* Command's description. */
};

/* Return value of the commands. */
#define CMD_SUCCESS              0
#define CMD_WARNING              1
#define CMD_ERR_NO_MATCH         2
#define CMD_ERR_AMBIGUOUS        3
#define CMD_ERR_INCOMPLETE       4
#define CMD_ERR_EXEED_ARGC_MAX   5
#define CMD_ERR_NOTHING_TODO     6
#define CMD_COMPLETE_FULL_MATCH  7
#define CMD_COMPLETE_MATCH       8
#define CMD_COMPLETE_LIST_MATCH  9
#define CMD_SUCCESS_DAEMON      10

/* Argc max counts. */
#define CMD_ARGC_MAX   25

/* Turn off these macros when uisng cpp with extract.pl */
#ifndef VTYSH_EXTRACT_PL  

/* helper defines for end-user DEFUN* macros */
#define DEFUN_CMD_ELEMENT(funcname, cmdname, cmdstr, helpstr, attrs, dnum) \
  struct cmd_element cmdname = \
  { \
    .string = cmdstr, \
    .func = funcname, \
    .doc = helpstr, \
    .attr = attrs, \
    .daemon = dnum, \
  };

#define DEFUN_CMD_FUNC_DECL(funcname) \
  static int funcname (struct cmd_element *, struct vty *, int, const char *[]);

#define DEFUN_CMD_FUNC_TEXT(funcname) \
  static int funcname \
    (struct cmd_element *self __attribute__ ((unused)), \
     struct vty *vty __attribute__ ((unused)), \
     int argc __attribute__ ((unused)), \
     const char *argv[] __attribute__ ((unused)) )

/* DEFUN for vty command interafce. Little bit hacky ;-). */
#define DEFUN(funcname, cmdname, cmdstr, helpstr) \
  DEFUN_CMD_FUNC_DECL(funcname) \
  DEFUN_CMD_ELEMENT(funcname, cmdname, cmdstr, helpstr, 0, 0) \
  DEFUN_CMD_FUNC_TEXT(funcname)

#define DEFUN_ATTR(funcname, cmdname, cmdstr, helpstr, attr) \
  DEFUN_CMD_FUNC_DECL(funcname) \
  DEFUN_CMD_ELEMENT(funcname, cmdname, cmdstr, helpstr, attr, 0) \
  DEFUN_CMD_FUNC_TEXT(funcname)

#define DEFUN_HIDDEN(funcname, cmdname, cmdstr, helpstr) \
  DEFUN_ATTR (funcname, cmdname, cmdstr, helpstr, CMD_ATTR_HIDDEN)

#define DEFUN_DEPRECATED(funcname, cmdname, cmdstr, helpstr) \
  DEFUN_ATTR (funcname, cmdname, cmdstr, helpstr, CMD_ATTR_DEPRECATED) \

/* DEFUN_NOSH for commands that vtysh should ignore */
#define DEFUN_NOSH(funcname, cmdname, cmdstr, helpstr) \
  DEFUN(funcname, cmdname, cmdstr, helpstr)

/* DEFSH for vtysh. */
#define DEFSH(daemon, cmdname, cmdstr, helpstr) \
  DEFUN_CMD_ELEMENT(NULL, cmdname, cmdstr, helpstr, 0, daemon) \

/* DEFUN + DEFSH */
#define DEFUNSH(daemon, funcname, cmdname, cmdstr, helpstr) \
  DEFUN_CMD_FUNC_DECL(funcname) \
  DEFUN_CMD_ELEMENT(funcname, cmdname, cmdstr, helpstr, 0, daemon) \
  DEFUN_CMD_FUNC_TEXT(funcname)

/* DEFUN + DEFSH with attributes */
#define DEFUNSH_ATTR(daemon, funcname, cmdname, cmdstr, helpstr, attr) \
  DEFUN_CMD_FUNC_DECL(funcname) \
  DEFUN_CMD_ELEMENT(funcname, cmdname, cmdstr, helpstr, attr, daemon) \
  DEFUN_CMD_FUNC_TEXT(funcname)

#define DEFUNSH_HIDDEN(daemon, funcname, cmdname, cmdstr, helpstr) \
  DEFUNSH_ATTR (daemon, funcname, cmdname, cmdstr, helpstr, CMD_ATTR_HIDDEN)

#define DEFUNSH_DEPRECATED(daemon, funcname, cmdname, cmdstr, helpstr) \
  DEFUNSH_ATTR (daemon, funcname, cmdname, cmdstr, helpstr, CMD_ATTR_DEPRECATED)

/* ALIAS macro which define existing command's alias. */
#define ALIAS(funcname, cmdname, cmdstr, helpstr) \
  DEFUN_CMD_ELEMENT(funcname, cmdname, cmdstr, helpstr, 0, 0)

#define ALIAS_ATTR(funcname, cmdname, cmdstr, helpstr, attr) \
  DEFUN_CMD_ELEMENT(funcname, cmdname, cmdstr, helpstr, attr, 0)

#define ALIAS_HIDDEN(funcname, cmdname, cmdstr, helpstr) \
  DEFUN_CMD_ELEMENT(funcname, cmdname, cmdstr, helpstr, CMD_ATTR_HIDDEN, 0)

#define ALIAS_DEPRECATED(funcname, cmdname, cmdstr, helpstr) \
  DEFUN_CMD_ELEMENT(funcname, cmdname, cmdstr, helpstr, CMD_ATTR_DEPRECATED, 0)

#define ALIAS_SH(daemon, funcname, cmdname, cmdstr, helpstr) \
  DEFUN_CMD_ELEMENT(funcname, cmdname, cmdstr, helpstr, 0, daemon)

#define ALIAS_SH_HIDDEN(daemon, funcname, cmdname, cmdstr, helpstr) \
  DEFUN_CMD_ELEMENT(funcname, cmdname, cmdstr, helpstr, CMD_ATTR_HIDDEN, daemon)

#define ALIAS_SH_DEPRECATED(daemon, funcname, cmdname, cmdstr, helpstr) \
  DEFUN_CMD_ELEMENT(funcname, cmdname, cmdstr, helpstr, CMD_ATTR_DEPRECATED, daemon)

#endif /* VTYSH_EXTRACT_PL */

/* Some macroes */
#define CMD_OPTION(S)   ((S[0]) == '[')
#define CMD_VARIABLE(S) (((S[0]) >= 'A' && (S[0]) <= 'Z') || ((S[0]) == '<'))
#define CMD_VARARG(S)   ((S[0]) == '.')
#define CMD_RANGE(S)	((S[0] == '<'))

#define CMD_IPV4(S)	   ((strcmp ((S), "A.B.C.D") == 0))
#define CMD_IPV4_PREFIX(S) ((strcmp ((S), "A.B.C.D/M") == 0))
#define CMD_IPV6(S)        ((strcmp ((S), "X:X::X:X") == 0))
#define CMD_IPV6_PREFIX(S) ((strcmp ((S), "X:X::X:X/M") == 0))

/* Common descriptions. */
#define SHOW_STR "Show running system information\n"
#define IP_STR "IP information\n"
#define IPV6_STR "IPv6 information\n"
#define NO_STR "Negate a command or set its defaults\n"
#define REDIST_STR "Redistribute information from another routing protocol\n"
#define CLEAR_STR "Reset functions\n"
#define RIP_STR "RIP information\n"
#define BGP_STR "BGP information\n"
#define OSPF_STR "OSPF information\n"
#define NEIGHBOR_STR "Specify neighbor router\n"
#define DEBUG_STR "Debugging functions (see also 'undebug')\n"
#define UNDEBUG_STR "Disable debugging functions (see also 'debug')\n"
#define ROUTER_STR "Enable a routing process\n"
#define AS_STR "AS number\n"
#define MBGP_STR "MBGP information\n"
#define MATCH_STR "Match values from routing table\n"
#define SET_STR "Set values in destination routing protocol\n"
#define OUT_STR "Filter outgoing routing updates\n"
#define IN_STR  "Filter incoming routing updates\n"
#define V4NOTATION_STR "specify by IPv4 address notation(e.g. 0.0.0.0)\n"
#define OSPF6_NUMBER_STR "Specify by number\n"
#define INTERFACE_STR "Interface infomation\n"
#define IFNAME_STR "Interface name(e.g. ep0)\n"
#define IP6_STR "IPv6 Information\n"
#define OSPF6_STR "Open Shortest Path First (OSPF) for IPv6\n"
#define OSPF6_ROUTER_STR "Enable a routing process\n"
#define OSPF6_INSTANCE_STR "<1-65535> Instance ID\n"
#define SECONDS_STR "<1-65535> Seconds\n"
#define ROUTE_STR "Routing Table\n"
#define PREFIX_LIST_STR "Build a prefix list\n"
#define OSPF6_DUMP_TYPE_LIST \
"(neighbor|interface|area|lsa|zebra|config|dbex|spf|route|lsdb|redistribute|hook|asbr|prefix|abr)"
#define ISIS_STR "IS-IS information\n"
#define AREA_TAG_STR "[area tag]\n"

#define CONF_BACKUP_EXT ".sav"

#define ACCESS_DESC_STR "Access exist mode\n"
#define ENABLE_DESC_STR "Enable function\n"
#define NO_DESC_STR "Negate a command or set its defaults\n"
#define SHOW_DESC_STR "Show running system information\n"
#define CURRENT_DESC_STR "Current config infomation of this mode\n"
#define INTERFACE_DESC_STR "Access the interface configuration mode\n"
#define SHUTDOWN_DESC_STR "Shutdown current interface\n"

/* IPv4 only machine should not accept IPv6 address for peer's IP
   address.  So we replace VTY command string like below. */
#ifdef HAVE_IPV6
#define NEIGHBOR_CMD       "neighbor (A.B.C.D|X:X::X:X) "
#define NO_NEIGHBOR_CMD    "no neighbor (A.B.C.D|X:X::X:X) "
#define NEIGHBOR_ADDR_STR  "Neighbor address\nIPv6 address\n"
#define NEIGHBOR_CMD2      "neighbor (A.B.C.D|X:X::X:X|WORD) "
#define NO_NEIGHBOR_CMD2   "no neighbor (A.B.C.D|X:X::X:X|WORD) "
#define NEIGHBOR_ADDR_STR2 "Neighbor address\nNeighbor IPv6 address\nNeighbor tag\n"
#else
#define NEIGHBOR_CMD       "neighbor A.B.C.D "
#define NO_NEIGHBOR_CMD    "no neighbor A.B.C.D "
#define NEIGHBOR_ADDR_STR  "Neighbor address\n"
#define NEIGHBOR_CMD2      "neighbor (A.B.C.D|WORD) "
#define NO_NEIGHBOR_CMD2   "no neighbor (A.B.C.D|WORD) "
#define NEIGHBOR_ADDR_STR2 "Neighbor address\nNeighbor tag\n"
#endif /* HAVE_IPV6 */

/*Begin add by huangmingjian 2012/08/23 for EPN104QID0008*/ 
//#define DEFAULT_VAL_HOSTNAME  "LOSO"
#define DEFAULT_VAL_SUPER     ""
/*End add by huangmingjian 2012/08/23 for EPN104QID0008 */


#define DEFAULT_TELNET_SERVICE   "enable"
#define DEFAULT_LOOPBACK_ENABLE 0  /*Add by huangmingjian 2012/11/28 for EPN104QID0083*/ 
#define ENABLE_STRING "enable"
#define DISABLE_STRING "disable"

/*Begin add by huangmingjian 2012/08/20 for EPN104QID0007*/ 
#define BUF_SIZE_256 256
struct orig_item_t{ 
	char username[BUF_SIZE_256];
	char password[BUF_SIZE_256];
	char enable[BUF_SIZE_256];
	
	char proto[BUF_SIZE_256];
	char ip[BUF_SIZE_256];
	char mask[BUF_SIZE_256];
	char dns[BUF_SIZE_256];
	char gateway[BUF_SIZE_256];
	char mac[BUF_SIZE_256];			
};
/*End add by huangmingjian 2012/08/20 for EPN104QID0007 */

/* Prototypes. */
extern void install_node (struct cmd_node *, int (*) (struct vty *));
extern void install_default (enum node_type);
extern void install_element (enum node_type, struct cmd_element *);
extern void sort_node (void);

/* Concatenates argv[shift] through argv[argc-1] into a single NUL-terminated
   string with a space between each element (allocated using
   XMALLOC(MTYPE_TMP)).  Returns NULL if shift >= argc. */
extern char *argv_concat (const char **argv, int argc, int shift);

extern vector cmd_make_strvec (const char *);
extern void cmd_free_strvec (vector);
extern vector cmd_describe_command (vector, struct vty *, int *status);
extern char **cmd_complete_command (vector, struct vty *, int *status);
extern const char *cmd_prompt (struct vty *);
extern int config_from_file (struct vty *, FILE *);
extern enum node_type node_parent (enum node_type);
extern int cmd_execute_command (vector, struct vty *, struct cmd_element **, int);
extern int cmd_execute_command_strict (vector, struct vty *, struct cmd_element **);
extern void config_replace_string (struct cmd_element *, char *, ...);
extern void cmd_init (int);

/*begin added by huangmingjian 2012-08-15 for EPN104QID0005*/
extern void host_init (void);
/*end added by huangmingjian 2012-08-15 for EPN104QID0005*/


extern void cmd_terminate (void);

/*funcs below can be called by cmd show running-config or show current*/
extern int func_vty_node_show_current(struct vty *vty);

/* Export typical functions. */
extern struct cmd_element config_end_cmd;
extern struct cmd_element config_exit_cmd;
extern struct cmd_element config_quit_cmd;
extern struct cmd_element config_help_cmd;
extern struct cmd_element config_list_cmd;

extern struct cmd_element config_who_cmd;
extern struct cmd_element show_history_cmd;
extern struct cmd_element show_version_cmd;
extern struct cmd_element show_startup_config_cmd;
extern struct cmd_element show_running_config_cmd;
	   struct cmd_element show_host_startup_cmd;
	   struct cmd_element show_networking_startup_cmd;
extern struct cmd_element hostname_cmd;
extern struct cmd_element no_hostname_cmd;
extern struct cmd_element no_super_cmd;
extern struct cmd_element super_password_cmd;	
extern struct cmd_element show_system_info_cmd;
extern struct cmd_element show_system_security_cmd;
extern struct cmd_element show_system_networking_cmd;
extern struct cmd_element save_cmd;
extern struct cmd_element reboot_cmd;
extern struct cmd_element show_ip_interface_cmd;
#ifdef CONFIG_LOOPD	
extern struct cmd_element show_loopback_detection_cmd;
#endif
extern struct cmd_element show_vlan_interface_cmd;


extern char *host_config_file (void);
extern void host_config_set (char *);

extern void print_version (const char *);

/* struct host global, ick */
extern struct host_t host; 

/* "<cr>" global */
extern char *command_cr;

extern char DEBUG_FLAG;

//extern int if_arg_decode2(char *arg, int *s1, int *s2);
//extern int if_arg_decode3(char *arg, int *s1, int *s2, int *s3);

#endif /* _ZEBRA_COMMAND_H */
