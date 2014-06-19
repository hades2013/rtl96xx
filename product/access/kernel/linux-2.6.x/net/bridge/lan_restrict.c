#include <linux/module.h>	
#include <linux/proc_fs.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/kernel_stat.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
//#include <linux/brlock.h>
#include <linux/net.h>
#include <linux/socket.h>

#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/string.h>
#include <net/ip.h>
#include <net/protocol.h>
#include <net/route.h>
#include <net/sock.h>
#include <net/arp.h>
#include <net/raw.h>
#include <net/checksum.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netlink.h>
#include <linux/inetdevice.h>
#include "lan_restrict.h"
#include "common/rtl_utils.h"
static lan_restrict_info	lan_restrict_tbl[EXT3];
static char			lan_restrict_flag[1024];
int8					enable_lanrestrict = FALSE;
static struct proc_dir_entry *res=NULL;



int	lan_restrict_rcv(struct sk_buff *skb, struct net_device *dev)
{
	int32 found = FAILED;
	ether_addr_t *macAddr;
//	int8 port_num;
	int32 column;
	rtl865x_tblAsicDrv_l2Param_t	fdbEntry;
	rtl865x_filterDbTableEntry_t		l2temp_entry;

/*	printk("\n lan_restrict_rcv, dev is %s\n", skb->dev->name);*/
	//if ((memcmp(skb->dev->name, "br0", 3) ==0) || (memcmp(skb->dev->name, "eth0", 4) ==0) && (memcmp(skb->dev->name, "wlan0", 5) ==0))


	if ((memcmp(skb->dev->name, "br0", 3) ==0) || (memcmp(skb->dev->name, "eth0", 4) ==0) )//
	{
		macAddr = (ether_addr_t *)(eth_hdr(skb)->h_source);
		found = rtl865x_Lookup_fdb_entry(0, macAddr, FDB_DYNAMIC, &column, &fdbEntry);
/*		printk("\nrecv packet from dev:%s\n", skb->dev->name);*/
		/*can found in asic , do noting here , in linux fdb module , it can be authed*/
		if (found == SUCCESS )
		{
#if 0		
			port_num = rtl865x_ConvertPortMasktoPortNum(fdbEntry.memberPortMask);
			
			if (lan_restrict_tbl[port_num].enable == TRUE)
			{
				if ((lan_restrict_tbl[port_num].curr_num < lan_restrict_tbl[port_num].max_num))
				{
/*					printk("\nPASS:lan_restrict_tbl[%d] current number is %d\n", port_num, lan_restrict_tbl[port_num].curr_num);*/
					return NET_RX_SUCCESS;
				}
				else
				{					
					if (fdbEntry.auth == TRUE)
					{
/*						printk("\nPASS1:lan_restrict_tbl[%d] current number is %d\n", port_num, lan_restrict_tbl[port_num].curr_num);*/
						return NET_RX_SUCCESS;
					}
					else
					{
/*						printk("\nDROP:lan_restrict_tbl[%d] current number is %d\n", port_num, lan_restrict_tbl[port_num].curr_num);*/
						l2temp_entry.l2type = (fdbEntry.nhFlag==0)?RTL865x_L2_TYPEI: RTL865x_L2_TYPEII;
						l2temp_entry.process = FDB_TYPE_FWD;
						l2temp_entry.memberPortMask = fdbEntry.memberPortMask;
						l2temp_entry.auth = FALSE;
						l2temp_entry.SrcBlk = TRUE;
						memcpy(&(l2temp_entry.macAddr), macAddr, sizeof(ether_addr_t));
						rtl865x_addAuthFilterDatabaseEntryExtension(fdbEntry.fid, &l2temp_entry);
						return NET_RX_AUTH_BLOCK;
					}
				}
			}
			else
			{
				return NET_RX_SUCCESS;				
			}			
#endif		
			return NET_RX_SUCCESS;		
		}
		else
		{
/*			printk(" \nnot found in hw table, src port is %d\n", skb->srcPort);*/
			if (lan_restrict_tbl[skb->srcPort].enable == TRUE)
			{
				/*found in sw l2 table*/
				if(rtl865x_lookup_FilterDatabaseEntry(0, macAddr, &l2temp_entry) == SUCCESS)
				{
					if (l2temp_entry.SrcBlk == TRUE)/*sw block*/
					{
						return NET_RX_AUTH_BLOCK;
					}
					else
					{
						return NET_RX_SUCCESS;
					}
				}
				else	/*not found in sw l2 table*/
				{
/*					printk(" \nnot found ind hw and sw table\n");*/
					if ((lan_restrict_tbl[skb->srcPort].curr_num < lan_restrict_tbl[skb->srcPort].max_num))
					{
						/*try to add into sw l2 table*/
/*						printk("\ntry to add into sw l2 table\n");*/
						rtl865x_addAuthFDBEntry((unsigned char *)macAddr, TRUE, skb->srcPort);
						return NET_RX_SUCCESS;
					}
					else
					{
						return NET_RX_AUTH_BLOCK;
					}
				}

			}
			else
			{
/*				printk("dev name is %s\n", skb->dev->name);*/
				return NET_RX_SUCCESS;
			}
		}
	}
	else
	{
		return NET_RX_SUCCESS;
	}
}
#if 0
static struct packet_type lan_restrict_packet_type = {
	.type =	__constant_htons(ETH_P_ALL),
	.func =	lan_restrict_rcv,
};
#endif
static int  lan_restrict_tbl_int(void)
{
	uint8 i;

	for (i=0; i < EXT3; i++  )
	{
		lan_restrict_tbl[i].port_num	=	0;
		lan_restrict_tbl[i].enable		=	FALSE;
		lan_restrict_tbl[i].max_num	=	0;
		lan_restrict_tbl[i].curr_num	= 	0;
	}
	return TRUE;
}

static int  lan_restrict_tbl_reset(void)
{
	uint8 i;

	for (i=0; i < EXT3; i++  )
	{
		lan_restrict_tbl[i].port_num	=	0;
		lan_restrict_tbl[i].enable		=	FALSE;
		lan_restrict_tbl[i].max_num	=	0;
		lan_restrict_tbl[i].curr_num	= 	0;
	}
	return TRUE;
}


static int  lan_restrict_set_singleport(uint8 portnum , int8 enable, int32 max_num)
{
	int32 ret;
	if (enable == TRUE)
	{
		lan_restrict_tbl[portnum].max_num = max_num;
	}
	else
	{
		lan_restrict_tbl[portnum].max_num = 0;		
	}
	
	ret =rtl865x_setRestrictPortNum(portnum, enable, max_num);
	return ret;
}

static int  lan_restrict_perport_setting(void)
{
	int i;
	for (i=0; i < EXT3; i++  )
	{
		lan_restrict_set_singleport(lan_restrict_tbl[i].port_num, lan_restrict_tbl[i].enable, lan_restrict_tbl[i].max_num);
	}
	return TRUE;
}

static int lan_restrict_enable(void)
{
	/*
		enable
	*/
	rtl865x_enableLanPortNumRestrict(TRUE);
	lan_restrict_perport_setting();
	return TRUE;
}

static int lan_restrict_disable(void)
{
	/*
		disable
	*/
	rtl865x_enableLanPortNumRestrict(FALSE);	
	lan_restrict_tbl_reset();
	lan_restrict_perport_setting();
	return TRUE;
}
/*
int32 lanrestrict_addfdbentry(const unsigned char *addr)
{
	int32 found = FAILED;
	ether_addr_t *macAddr;
	int32 ret=FAILED;
	int8 port_num;
	int32 column;
	rtl865x_tblAsicDrv_l2Param_t	fdbEntry;
	rtl865x_filterDbTableEntry_t		l2temp_entry;

	macAddr = (ether_addr_t *)(addr);
	found = rtl865x_Lookup_fdb_entry(0, macAddr, FDB_DYNAMIC, &column, &fdbEntry);
	if (found == SUCCESS )
	{
		port_num = rtl865x_ConvertPortMasktoPortNum(fdbEntry.memberPortMask);
		
		if (rtl865x_lookup_FilterDatabaseEntry(fdbEntry.fid, macAddr) != SUCCESS)
		{
			l2temp_entry.l2type = (fdbEntry.nhFlag==0)?RTL865x_L2_TYPEI: RTL865x_L2_TYPEII;
			l2temp_entry.process = FDB_TYPE_FWD;
			l2temp_entry.memberPortMask = fdbEntry.memberPortMask;
			l2temp_entry.auth = TRUE;
			l2temp_entry.SrcBlk = FALSE;
			memcpy(&(l2temp_entry.macAddr), macAddr, sizeof(ether_addr_t));
			ret =rtl865x_addAuthFilterDatabaseEntryExtension(fdbEntry.fid, &l2temp_entry);						
		}	
	}
	return ret;
}
*/
static int32 lanrestrict_callbackFn_for_add_fdb(void *param)
{
	int32 port_num;
	rtl865x_filterDbTableEntry_t  *fdbEntry; 
	
	if(param==NULL)
	{
		return EVENT_CONTINUE_EXECUTE;
	}
	
	fdbEntry=(rtl865x_filterDbTableEntry_t *)param;

	port_num = rtl865x_ConvertPortMasktoPortNum(fdbEntry->memberPortMask);	

	if (lan_restrict_tbl[port_num].enable == TRUE)
	{
/*		printk("\nadd authed port[%d], num:%d\n", port_num, lan_restrict_tbl[port_num].curr_num);*/
		lan_restrict_tbl[port_num].curr_num ++;
	}
	
	return SUCCESS;
}

static int32 lanrestrict_callbackFn_for_del_fdb(void *param)
{
	int32 port_num;
	rtl865x_filterDbTableEntry_t  *fdbEntry; 
	
	if(param==NULL)
	{
		return EVENT_CONTINUE_EXECUTE;
	}
	
	fdbEntry=(rtl865x_filterDbTableEntry_t *)param;

	port_num = rtl865x_ConvertPortMasktoPortNum(fdbEntry->memberPortMask);	
	if (lan_restrict_tbl[port_num].enable == TRUE)
	{
/*		printk("\ndel authed port[%d], num:%d\n", port_num, lan_restrict_tbl[port_num].curr_num);*/
		lan_restrict_tbl[port_num].curr_num --;
		if (lan_restrict_tbl[port_num].curr_num < 0)
		{
			lan_restrict_tbl[port_num].curr_num = 0;
		}
	}
	return SUCCESS;
}

static int32 lanrestrict_authadd_unRegister_event(void)
{
	rtl865x_event_Param_t eventParam;
	eventParam.eventLayerId=DEFAULT_LAYER2_EVENT_LIST_ID;
	eventParam.eventId=EVENT_ADD_AUTHED_FDB;
	eventParam.eventPriority=0;
	eventParam.event_action_fn=lanrestrict_callbackFn_for_add_fdb;
	rtl865x_unRegisterEvent(&eventParam);
	return SUCCESS;
}

static int32 lanrestrict_authdel_unRegister_event(void)
{
	rtl865x_event_Param_t eventParam;
	eventParam.eventLayerId=DEFAULT_LAYER2_EVENT_LIST_ID;
	eventParam.eventId=EVENT_DEL_AUTHED_FDB;
	eventParam.eventPriority=0;
	eventParam.event_action_fn=lanrestrict_callbackFn_for_del_fdb;
	rtl865x_unRegisterEvent(&eventParam);
	return SUCCESS;
}

static int32 lanrestrict_unRegister_event(void)
{
	lanrestrict_authadd_unRegister_event();
	lanrestrict_authdel_unRegister_event();
}

static int32 lanrestrict_authadd_register_event(void)
{
	rtl865x_event_Param_t eventParam;
	eventParam.eventLayerId=DEFAULT_LAYER2_EVENT_LIST_ID;
	eventParam.eventId=EVENT_ADD_AUTHED_FDB;
	eventParam.eventPriority=0;
	eventParam.event_action_fn=lanrestrict_callbackFn_for_add_fdb;
	rtl865x_registerEvent(&eventParam);
	return SUCCESS;
}

static int32 lanrestrict_authdel_register_event(void)
{
	rtl865x_event_Param_t eventParam;
	eventParam.eventLayerId=DEFAULT_LAYER2_EVENT_LIST_ID;
	eventParam.eventId=EVENT_DEL_AUTHED_FDB;
	eventParam.eventPriority=0;
	eventParam.event_action_fn=lanrestrict_callbackFn_for_del_fdb;
	rtl865x_registerEvent(&eventParam);
	return SUCCESS;
}

static int32 lanrestrict_register_event(void)
{
	lanrestrict_authadd_register_event();
	lanrestrict_authdel_register_event();
}


int32 lan_restrict_getBlockAddr(int32 port , const unsigned char *swap_addr)
{
	int32 ret = FAILED;

	if 	(lan_restrict_tbl[port].enable == TRUE) 
	{
		ret = rtl865x_check_authfdbentry_Byport(port , swap_addr);
	}

	return ret;
}

int32 lan_restrict_CheckStatusByport(int32 port)
{
	if 	(lan_restrict_tbl[port].enable == TRUE) 
	{
		if (lan_restrict_tbl[port].curr_num < lan_restrict_tbl[port].max_num)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FAILED;
	}
}
static int lan_restrict_read_proc(char *page, char **start, off_t off,
		     int count, int *eof, void *data)
{
	int len, i ;
	len = sprintf(page, "%s\n", "lan restrict table:");
	if (len <= off+count) 
		*eof = 1;

	for (i = 0; i < EXT3; i++)
	{
		len += sprintf(page + len, "  PORT[%d]      ", i);
		len += sprintf(page + len,"%6s %6d %6d  ",lan_restrict_tbl[i].enable?"ON":"OFF", lan_restrict_tbl[i].max_num,  lan_restrict_tbl[i].curr_num);
		len += sprintf(page + len,"\n");
	}

	return len;
}

static int lan_restrict_write_proc(struct file *file, const char *buffer,
		      unsigned long count, void *data)
{
	char tmpbuf[1024];
	char *entryPtr, *portnumPtr, *enablePtr, *maxnumPtr, *strptr=tmpbuf;
	int8 port, port_enable, maxnum;

	if (count < 2)
		return -EFAULT;
	/*
	format:  entry1;entry2;entry3
	entry format: port_num enable max_num curr_num;
	port_num: 	0,1,2...
	enable:		on/off
	max_num:	0,1,2...
	curr_num:	0,1,2..., can not write, can only read from proc file and write again, just for display
	*/

	memset(lan_restrict_flag,0,strlen(lan_restrict_flag));
	if (buffer && !copy_from_user(tmpbuf, buffer, count))
 	{
 		if(memcmp(strptr,"enable", strlen("enable")) == 0)
		{
			lan_restrict_enable();
			enable_lanrestrict = TRUE;		
/*			printk("Fun[%s][%d]\n", __FUNCTION__, __LINE__);*/
			printk("enable lan restrict FUNC.....\n");			
		}
		else if(memcmp(strptr,"disable", strlen("enable")) == 0)
		{
			lan_restrict_disable();
			enable_lanrestrict = FALSE;
/*			printk("Fun[%s][%d]\n", __FUNCTION__, __LINE__);*/
			printk("disable lan restrict FUNC.....\n");		
		}
		else
		{
			if (lan_restrict_enable == FALSE)
				return count;

				/*
				format:  entry1;entry2;entry3
				entry format: port_num enable max_num curr_num;
				port_num: 	0,1,2...
				enable:		on/off
				max_num:	0,1,2...
				curr_num:	0,1,2..., can not write, can only read from proc file and write again, just for display
				*/
				entryPtr = strsep(&strptr,";");
				while (entryPtr != NULL)
		      		{
					/*1. port_num*/
					portnumPtr = strsep(&entryPtr," ");
					if(portnumPtr == NULL)
					{
						printk("lan restrict setting format error1\n");
						break;
					}	
			      		port = simple_strtol(portnumPtr,NULL,0);	
					printk("set port num is %d\n", port);	

					/*2. enable or not*/	
			      		enablePtr = strsep(&entryPtr," ");			
					if(enablePtr == NULL)
					{
						printk("lan restrict setting format error2\n");
						break;
					}
					if(_strncasecmp(enablePtr,"OFF",3) == 0)
					{
						port_enable = FALSE;
					}
					else if (_strncasecmp(enablePtr,"ON",3) == 0)
					{
						port_enable = TRUE;
					}
					else
					{
						printk("lan restrict setting format error3\n");						
						break;
					}
					printk("port_enable is %d\n", port_enable);

					/*3max num*/
					maxnumPtr = strsep(&entryPtr," ");
					if(maxnumPtr == NULL)
					{
						printk("lan restrict setting format error4\n");
						break;
					}
			      		maxnum = simple_strtol(maxnumPtr,NULL,0);	
					printk("set max num is %d\n", maxnum);						
					
					lan_restrict_tbl[port].enable = 	port_enable;
					lan_restrict_tbl[port].max_num = maxnum;
					/*
						set Asic
					*/
					lan_restrict_set_singleport(port, port_enable, maxnum);
				}
		}
	}

	return count;
}

static int lan_restrict_proc_init(void)
{
	res = create_proc_entry("lan_restrict_info",0,NULL);
	if(res)
	{
		res->read_proc = lan_restrict_read_proc;
		res->write_proc = lan_restrict_write_proc;
		lanrestrict_unRegister_event();
		lanrestrict_register_event();
		return TRUE;
	}
	return FALSE;
}
	
int __init lan_restrict_init(void)
{
	lan_restrict_tbl_int();
//	dev_add_pack(&lan_restrict_packet_type);
	lan_restrict_proc_init();
	return 0;
}


