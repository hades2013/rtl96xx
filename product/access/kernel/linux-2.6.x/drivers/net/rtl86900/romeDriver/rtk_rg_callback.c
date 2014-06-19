#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/config.h>


#include <rtk_rg_struct.h>
#include <rtk_rg_internal.h>
#include <rtk_rg_debug.h>
#include <rtk_rg_callback.h>
#include <rtk_rg_liteRomeDriver.h>


static char ifconfig[]="/bin/ifconfig";
static char SNAT_INFO_FILE[]="/tmp/SNAT_INFO";
static char DHCPC_PID[]="/var/run/udhcpc.pid";

mm_segment_t rg_callback_oldfs;


extern rtk_rg_globalDatabase_t rg_db;
extern rtk_rg_globalKernel_t rg_kernel;

struct file *openFile(char *path,int flag,int mode)
{
    struct file *fp;

    fp=filp_open(path, flag, 0);
 
   	if(IS_ERR(fp)) 
   		return NULL;
	else
		return fp;
}

int readFile(struct file *fp,char *buf,int readlen)
{
    if (fp->f_op && fp->f_op->read)
        return fp->f_op->read(fp,buf,readlen, &fp->f_pos);
    else
        return -1;
}

int closeFile(struct file *fp)
{
    filp_close(fp,NULL);
    return 0;
}

void initKernelEnv(void)
{
    rg_callback_oldfs = get_fs();
    set_fs(KERNEL_DS);
}

int read_pid(char *filename)
{
	struct file *fp;
	char pidBuf[32];
	int ret,pid;

	initKernelEnv();
	fp = openFile(filename,O_RDWR,0);
	if (fp!=NULL)
    {
        memset(pidBuf,0,32);
        if ((ret=readFile(fp,pidBuf,31))>0){
            //rtlglue_printf("buff:%s\n",snat_file_buff);
        }
        else{ 
			rtlglue_printf("[rg callback]openFile %s error, ret=%d\n",pidBuf,ret);
        }
		closeFile(fp);
    }
	set_fs(rg_callback_oldfs);

	sscanf(pidBuf,"%d",&pid);
	return pid;
}

/*IPv6 address to string*/
int8 *_rtk_rg_inet_n6toa(const uint8 *ipv6)
{
#define RG_IPV6_TMP_BUFFER_LENGTH 8

	static int8 buf[8*sizeof "FFFF:"];
    uint32  i;
    uint16  ipv6_ptr[RG_IPV6_TMP_BUFFER_LENGTH] = {0};

    for (i = 0; i < RG_IPV6_TMP_BUFFER_LENGTH ;i++)
    {
        ipv6_ptr[i] = ipv6[i*2+1];
        ipv6_ptr[i] |=  ipv6[i*2] << 8;
    }

    sprintf(buf, "%x:%x:%x:%x:%x:%x:%x:%x", ipv6_ptr[0], ipv6_ptr[1], ipv6_ptr[2], ipv6_ptr[3]
    , ipv6_ptr[4], ipv6_ptr[5], ipv6_ptr[6], ipv6_ptr[7]);
	return (buf);
}


/* convert IPv6 address from string to number. Length of ipv6_addr must more than 16 characters */
int32
_rg_str2ipv6(uint8 *ipv6, const uint8 *str)
{
#define RG_IN6ADDRSZ 16
#define RG_INT16SZ     2
    static const uint8 xdigits_l[] = "0123456789abcdef",
              xdigits_u[] = "0123456789ABCDEF";
    uint8 tmp[RG_IN6ADDRSZ], *tp, *endp, *colonp;
    const uint8 *xdigits, *curtok;
    int ch, saw_xdigit;
    int val;

    if ((NULL == str) || (NULL == ipv6))
    {
        return RT_ERR_FAILED;
    }

    memset((tp = tmp), '\0', RG_IN6ADDRSZ);
    endp = tp + RG_IN6ADDRSZ;
    colonp = NULL;
    /* Leading :: requires some special handling. */
    if (*str == ':')
        if (*++str != ':')
            return (RT_ERR_FAILED);
    curtok = str;
    saw_xdigit = 0;
    val = 0;
    while ((ch = *str++) != '\0') {
        const uint8 *pch;

        if ((pch = strchr((xdigits = xdigits_l), ch)) == NULL)
            pch = strchr((xdigits = xdigits_u), ch);
        if (pch != NULL) {
            val <<= 4;
            val |= (pch - xdigits);
            if (val > 0xffff)
                return (RT_ERR_FAILED);
            saw_xdigit = 1;
            continue;
        }
        if (ch == ':') {
            curtok = str;
            if (!saw_xdigit) {
                if (colonp)
                    return (RT_ERR_FAILED);
                colonp = tp;
                continue;
            }
            if (tp + RG_INT16SZ > endp)
                return (RT_ERR_FAILED);
            *tp++ = (uint8) (val >> 8) & 0xff;
            *tp++ = (uint8) val & 0xff;
            saw_xdigit = 0;
            val = 0;
            continue;
        }

        return (RT_ERR_FAILED);
    }
    if (saw_xdigit) {
        if (tp + RG_INT16SZ > endp)
            return (RT_ERR_FAILED);
        *tp++ = (uint8) (val >> 8) & 0xff;
        *tp++ = (uint8) val & 0xff;
    }
    if (colonp != NULL) {
        /*
         * Since some memmove()'s erroneously fail to handle
         * overlapping regions, we'll do the shift by hand.
         */
        const int n = tp - colonp;
        int i;

        for (i = 1; i <= n; i++) {
            endp[- i] = colonp[n - i];
            colonp[n - i] = 0;
        }
        tp = endp;
    }
    if (tp != endp)
        return (RT_ERR_FAILED);
    memcpy(ipv6, tmp, RG_IN6ADDRSZ);
    return (RT_ERR_OK);
}/* end of diag_util_str2Ipv6 */

/* Check if the MAC address is a broadcast address or not */



#ifdef CONFIG_RG_CALLBACK
char snat_file_buff[MAX_FILE_SIZE];
int _delete_iptables_snat(void){

	struct file *fp;
	char* buff_line[MAX_FILE_LINE]={NULL};
	char* token, *ptr_head, *ptr_tail;
	int ret,len,i;
	char intf[16];
	char ip[16];
	
	/*cat ipatbles SNAT info into /tmp/SNAT_INFO */
	user_pipe_cmd("/bin/iptables -t nat -nvL POSTROUTING > /tmp/SNAT_INFO");



	/*read /tmp/SNAT_INFO */
    initKernelEnv();
    fp=openFile(SNAT_INFO_FILE,O_RDWR,0);
    if (fp!=NULL)
    {
        memset(snat_file_buff,0,MAX_FILE_SIZE);
        if ((ret=readFile(fp,snat_file_buff,(MAX_FILE_SIZE-1)))>0){
            //rtlglue_printf("buff:%s\n",snat_file_buff);
        }
        else{ 
			rtlglue_printf("[rg callback]openFile %s error, ret=%d\n",SNAT_INFO_FILE,ret);
        }
		closeFile(fp);

    }
    set_fs(rg_callback_oldfs);
	
	/*parse /tmp/SNAT_INFO */	
	len = strlen(snat_file_buff);
	token = snat_file_buff; //buff is static, cannot use as strsep parameter!
	while (strsep(&token, "\n"));//parse each line to buff_line[i]
	for (i=0, token=snat_file_buff; token<(snat_file_buff+len) && i<MAX_FILE_LINE ;i++) { 
		buff_line[i] = token; 
		token=token+strlen(token)+1; 
	} 


	//start from buff_line[2] to parse intfname & ip, adn delete such iptables rule
	for(i=2; buff_line[i]!=NULL && i<5;i++){ //first two line are not rules in /tmp/SNAT_INFO
		/*parse intf name*/
		ptr_head = strstr(buff_line[i], "nas");
		if(ptr_head==NULL) continue;//string is not for SNAT rule 		
		ptr_tail = strchr(ptr_head, ' ');	
		if(ptr_tail==NULL) continue;//string is not for SNAT rule
		
		len = ptr_tail - ptr_head;
		strncpy(intf,ptr_head,len);
		intf[len]='\0';

		/*parse ip*/
		ptr_head = strstr(ptr_tail+1, "to:");
		if(ptr_head==NULL) continue;//string is not for SNAT rule 	
		ptr_tail = strchr(ptr_head, ' ');
		if(ptr_tail==NULL) continue;//string is not for SNAT rule
		
		len = ptr_tail - (ptr_head+3); //+3 to skip "to:"
		strncpy(ip,ptr_head+3,len);
		ip[len]='\0';
		
		user_cmd("/bin/iptables -t nat -D POSTROUTING -o %s -j SNAT --to-source %s",intf,ip);
	}

	/*remove  /tmp/SNAT_INFO */
	user_cmd("/bin/rm %s",SNAT_INFO_FILE);

	return SUCCESS;
}



/*Default callback functions*/

#ifdef CONFIG_RG_SIMPLE_PROTOCOL_STACK

//callback for rg init
int _rtk_rg_initParameterSetByHwCallBack(void)
{
	int i;
	char intf_name[20];

	user_cmd("%s br0 down",ifconfig);

	for(i=0;i<8;i++){
		sprintf(intf_name,"eth%d",i);
		user_cmd("%s %s down",ifconfig,intf_name);

		//1 FIXME: PPPoE disconnet issue
		/*
		if(rg_kernel.ppp_diaged[i]==ENABLED){
			user_cmd("/bin/spppctl down ppp%d",i);
			rg_kernel.ppp_diaged[i]=DISABLED;
		}
		*/
		//user_cmd("/bin/spppctl down ppp%d",i);
		//user_cmd("/bin/spppd");
	}

	//patch for ipatbles
	_delete_iptables_snat();
	
    return SUCCESS;

}


//rg_db.systemGlobal.initParam.interfaceAddByHwCallBack;
int _rtk_rg_interfaceAddByHwCallBack(rtk_rg_intfInfo_t* intfInfo, int* intfIdx)
{

	unsigned char ip[16];
	unsigned char gw_ip[16];
    unsigned char mask[16];
	unsigned char brcast_ip[16];
	int i;
    char dev[8]; //device name, ex:nas0_0
    char* wan_type; //wan type: ex:PPPoE
    int vid=0;
	rtk_ip_addr_t bc_ip; //broadCast ip for wan intf
	rtk_rg_ipStaticInfo_t static_info;

	
    if(intfInfo->is_wan)
    {
        //add wan interface        
		memset(&static_info,0,sizeof(rtk_rg_ipStaticInfo_t));

		//get static_info by different type
		if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_STATIC){
			 memcpy(&static_info,&intfInfo->wan_intf.static_info,sizeof(rtk_rg_ipStaticInfo_t));
		}else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_DHCP){
			 memcpy(&static_info,&intfInfo->wan_intf.dhcp_client_info.hw_info,sizeof(rtk_rg_ipStaticInfo_t));
		}else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_PPPoE){
			 memcpy(&static_info,&intfInfo->wan_intf.pppoe_info.after_dial.hw_info,sizeof(rtk_rg_ipStaticInfo_t));
		}
		
        //sprintf(dev,"nas0_%d",(*intfIdx-1));
        sprintf(dev,"eth%d",(*intfIdx));
        memcpy(ip,(void*)_inet_ntoa(static_info.ip_addr),16);
		memcpy(gw_ip,(void*)_inet_ntoa(static_info.gateway_ipv4_addr),16);
        memcpy(mask,(void*)_inet_ntoa(static_info.ip_network_mask),16);

		//caculate broacast ip
		bc_ip = static_info.gateway_ipv4_addr & static_info.ip_network_mask;

		for(i=0;i<32;i++){
			if(static_info.ip_network_mask & (1<<i)){
				//wan domain (mask)
			}else{
				//wan broacast ip
				bc_ip |= (1<<i);
			}
		}
		memcpy(brcast_ip,(void*)_inet_ntoa(bc_ip),16);

        switch(intfInfo->wan_intf.wan_intf_conf.wan_type)
        {
	        case RTK_RG_STATIC:
	            wan_type="ipoe";
	            break;
	        case RTK_RG_DHCP:
	            wan_type="dhcp";
	            break;
	        case RTK_RG_PPPoE:
	            wan_type="pppoe";
	            break;
	        case RTK_RG_BRIDGE:
	            wan_type="bridge";
	            break;
	        default:
				wan_type=" ";
	            break;
        }

        if(intfInfo->wan_intf.wan_intf_conf.egress_vlan_tag_on)
            vid = intfInfo->wan_intf.wan_intf_conf.egress_vlan_id;

        //do cmd
        user_pipe_cmd("echo %d wan0 > /proc/rtl8686gmac/dev_port_mapping",intfInfo->wan_intf.wan_intf_conf.wan_port_idx);

		//create nas0_X
		if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_DHCP){
			//DHCP protocal-stack setting is set by _rtk_rg_dhcpRequestByHwCallBack
		}else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_PPPoE){
			//PPPoE protocal-stack setting is set by _rtk_rg_pppoeBeforeDiagByHwCallBack
		}else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_BRIDGE){
			/*user_cmd("%s nas0 up",ifconfig);
			user_cmd("/bin/ethctl remsmux bridge nas0 %s",dev);
			
			if(static_info.napt_enable && intfInfo->wan_intf.wan_intf_conf.egress_vlan_tag_on)
			{	
				//NAPT + VALN TAG
				user_cmd("/bin/ethctl addsmux %s nas0 %s napt vlan %d",wan_type,dev,vid); 
			}
			else if(static_info.napt_enable)
			{
				//NAPT
				user_cmd("/bin/ethctl addsmux %s nas0 %s napt",wan_type,dev);
			}
			else if(intfInfo->wan_intf.wan_intf_conf.egress_vlan_tag_on){
				//VALN TAG
				user_cmd("/bin/ethctl addsmux %s nas0 %s nonapt vlan %d",wan_type,dev,vid); 
			}
			else{
				user_cmd("/bin/ethctl addsmux %s nas0 %s",wan_type,dev);
			}*/
			
			user_cmd("%s %s hw ether %02x%02x%02x%02x%02x%02x",ifconfig,dev,
					   intfInfo->wan_intf.wan_intf_conf.gmac.octet[0],
					   intfInfo->wan_intf.wan_intf_conf.gmac.octet[1],
					   intfInfo->wan_intf.wan_intf_conf.gmac.octet[2],
					   intfInfo->wan_intf.wan_intf_conf.gmac.octet[3],
					   intfInfo->wan_intf.wan_intf_conf.gmac.octet[4],
					   intfInfo->wan_intf.wan_intf_conf.gmac.octet[5]);

			user_cmd("%s %s txqueuelen 10",ifconfig,dev);
			user_cmd("%s %s up",ifconfig,dev);
			//user_cmd("/bin/brctl addif br0 %s",dev);

		}
		else{
			//set STATIC & BRIDGE protocal-stack
	        /*user_cmd("%s nas0 up",ifconfig);
	        user_cmd("/bin/ethctl remsmux bridge nas0 %s",dev);

			if(static_info.napt_enable && intfInfo->wan_intf.wan_intf_conf.egress_vlan_tag_on)
	        {	
	        	//NAPT + VALN TAG
	            user_cmd("/bin/ethctl addsmux %s nas0 %s napt vlan %d",wan_type,dev,vid); 
	        }
	        else if(static_info.napt_enable)
	        {
	        	//NAPT
	            user_cmd("/bin/ethctl addsmux %s nas0 %s napt",wan_type,dev);
	        }
			else if(intfInfo->wan_intf.wan_intf_conf.egress_vlan_tag_on){
				//VALN TAG
				user_cmd("/bin/ethctl addsmux %s nas0 %s nonapt vlan %d",wan_type,dev,vid); 
			}
			else{
				user_cmd("/bin/ethctl addsmux %s nas0 %s",wan_type,dev);
			}*/
			
	        user_cmd("%s %s hw ether %02x%02x%02x%02x%02x%02x",ifconfig,dev,
	                   intfInfo->wan_intf.wan_intf_conf.gmac.octet[0],
	                   intfInfo->wan_intf.wan_intf_conf.gmac.octet[1],
	                   intfInfo->wan_intf.wan_intf_conf.gmac.octet[2],
	                   intfInfo->wan_intf.wan_intf_conf.gmac.octet[3],
	                   intfInfo->wan_intf.wan_intf_conf.gmac.octet[4],
	                   intfInfo->wan_intf.wan_intf_conf.gmac.octet[5]);
			
	        user_cmd("%s %s up",ifconfig,dev);
			user_cmd("%s %s mtu %d",ifconfig,dev,static_info.mtu);       
    	}


		/***IPv4 Setting***/
		if(static_info.ip_version==IPVER_V4ONLY || static_info.ip_version==IPVER_V4V6){

			/*if(static_info.ipv4_default_gateway_on){
				user_cmd("/bin/route add default gw %s",gw_ip);
			}
			
			//enable forwarding state
			user_pipe_cmd("echo 1 > /proc/sys/net/ipv4/ip_forward");*/

			if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_DHCP){
				//NAT set by _rtk_rg_dhcpRequestByHwCallBack
			}
			else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_PPPoE){
				//NAT set by _rtk_rg_pppoeBeforeDiagByHwCallBack
			}
			else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_BRIDGE){
			
			}
			else{		
				user_cmd("%s %s %s netmask %s broadcast %s",ifconfig,dev,ip,mask,brcast_ip);
				/*if(static_info.napt_enable){//add NAPT in iptables
					user_cmd("/bin/iptables -t nat -A POSTROUTING -o %s -j SNAT --to-source %s",dev,ip);
				}else{
					//pure routing
				}*/
			}
			
			//bring message to DDNS
			user_cmd("/bin/updateddctrl %s",dev);

			//do_sys_cmd("/bin/ip route add default via %s table 32",gw_ip);
			//do_sys_cmd("/bin/ip route add 192.168.150.0/24 dev nas0_0 table 32");
		}

		/***IPv6 Setting***/
		if(static_info.ip_version==IPVER_V6ONLY || static_info.ip_version==IPVER_V4V6){
			if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_DHCP){
				//NAT set by _rtk_rg_dhcpRequestByHwCallBack
			}
			else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_PPPoE){
				//NAT set by _rtk_rg_pppoeBeforeDiagByHwCallBack
			}
			else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_BRIDGE){
			
			}
			else{		
				//current support ipv6_static only!!!
				user_cmd("%s %s add %s/%d",ifconfig,dev,_rtk_rg_inet_n6toa(&static_info.ipv6_addr.ipv6_addr[0]),static_info.ipv6_mask_length);
				//user_pipe_cmd("echo 1 > /proc/sys/net/ipv6/conf/all/forwarding");
				/*if(static_info.ipv6_default_gateway_on){
					user_cmd("/bin/route -A inet6 add ::/0 gw %s %s",_rtk_rg_inet_n6toa(&static_info.gateway_ipv6_addr.ipv6_addr[0]),dev);
				}*/			
			}
		}
		
    }
    else
    {
    	//add lan interface
        memcpy(ip,(void*)_inet_ntoa(intfInfo->lan_intf.ip_addr),16);
        memcpy(mask,(void*)_inet_ntoa(intfInfo->lan_intf.ip_network_mask),16);

		user_cmd("brctl addbr br0");
		user_cmd("brctl addif eth0");
#ifdef CONFIG_DUALBAND_CONCURRENT
		user_cmd("/bin/brctl addif br0 wlan1");
		user_cmd("%s wlan1 down",ifconfig);
#endif		
        user_cmd("%s eth0 down",ifconfig);
        user_cmd("%s br0 down",ifconfig);

		user_cmd("%s br0 hw ether %02x%02x%02x%02x%02x%02x",ifconfig,
				   intfInfo->lan_intf.gmac.octet[0],
				   intfInfo->lan_intf.gmac.octet[1],
				   intfInfo->lan_intf.gmac.octet[2],
				   intfInfo->lan_intf.gmac.octet[3],
				   intfInfo->lan_intf.gmac.octet[4],
				   intfInfo->lan_intf.gmac.octet[5]);


		/***IPv4 Setting***/
		if(intfInfo->lan_intf.ip_version==IPVER_V4ONLY || intfInfo->lan_intf.ip_version==IPVER_V4V6){
	        user_cmd("%s br0 %s netmask %s mtu %d",ifconfig,ip,mask,intfInfo->lan_intf.mtu);
		}

		/***IPv6 Setting***/
		if(intfInfo->lan_intf.ip_version==IPVER_V6ONLY || intfInfo->lan_intf.ip_version==IPVER_V4V6){
			user_cmd("%s br0 add %s/%d",ifconfig,_rtk_rg_inet_n6toa(&intfInfo->lan_intf.ipv6_addr.ipv6_addr[0]),intfInfo->lan_intf.ipv6_network_mask_length);
		}

		user_cmd("%s br0 up",ifconfig);
        user_cmd("%s eth0 up",ifconfig);


#ifdef CONFIG_DUALBAND_CONCURRENT
		user_cmd("%s wlan1 up",ifconfig);
#endif

        //patch for iptables
        user_cmd("/bin/iptables -A INPUT -s %s -d %s -j ACCEPT",ip,ip);
#if 0
		//restart udhcpd: this will make signal hang
		do_sys_cmd("/bin/udhcpd -B /var/udhcpd/udhcpd.conf");
#endif
    }

	//rtlglue_printf("intf[0] valid==%d\n",rg_db.systemGlobal.interfaceInfo[0].valid);
	//rtlglue_printf("intfIdx=%d	wan_type=%d@@@@@@n",*intfIdx,rg_db.systemGlobal.interfaceInfo[*intfIdx].storedInfo.wan_intf.wan_intf_conf.wan_type);

    return SUCCESS;
}

//rg_db.systemGlobal.initParam.interfaceDelByHwCallBack;
int _rtk_rg_interfaceDelByHwCallBack(rtk_rg_intfInfo_t* intfInfo, int* intfIdx)
{
	int ret,dhcpc_pid;
	struct task_struct *t;
	
	if(intfInfo->is_wan==0){
		user_cmd("%s eth0 down",ifconfig);
		//user_cmd("%s br0 down",ifconfig);
	}else{
		char dev[8]; //device name, ex:nas0_0
		//sprintf(dev,"nas0_%d",(*intfIdx-1));
		sprintf(dev,"eth%d",(*intfIdx));

		if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_DHCP)
		{
			//1 FIXME:if there is two or more WAN use DHCP, this may not working
			dhcpc_pid = read_pid((char*)DHCPC_PID);
			DEBUG("the dhcpc_pid is %d",dhcpc_pid);
			if (dhcpc_pid > 0)
			{
				rcu_read_lock();
				t = find_task_by_vpid(dhcpc_pid);
				if(t == NULL){
					DEBUG("no such pid");
					rcu_read_unlock();
				}
				else
				{
					rcu_read_unlock();
					ret = send_sig(SIGTERM,t,0);//send_sig_info(SIG_TEST, &info, t);    //send the signal
					if (ret < 0) {
						DEBUG("error sending signal\n");
					}
				}
				
				//kill(dhcpc_pid, SIGTERM);
				//user_cmd("kill -%d %d",SIGTERM,dhcpc_pid);//kill(dhcpc_pid, SIGTERM);
			}
		}
		else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_PPPoE)
		{
			if(rg_db.systemGlobal.not_disconnect_ppp==0){
				user_cmd("/bin/spppctl down %d",*intfIdx);
				//let spppd to down the eth wan for us, otherwise PADT won't send out
				return SUCCESS;
			}else{
				//don't call spppctl to disconnet interface pppX, because this case is "Server disconnet problem".
				//so, do nothing!
				rg_db.systemGlobal.not_disconnect_ppp=0;
			}
		}
		
		//wan-intf
		user_cmd("%s %s down",ifconfig,dev);
	}

    return SUCCESS;
}

//rg_db.systemGlobal.initParam.dhcpRequestByHwCallBack;
int _rtk_rg_dhcpRequestByHwCallBack(int* intfIdx){

	char dev[8]; //device name, ex:nas0_0
	int dev_idx;
	//int vid;
	rtk_rg_intfInfo_t intfInfo;
	debug("%s is called!!!",__func__);

	memcpy(&intfInfo,&rg_db.systemGlobal.interfaceInfo[*intfIdx].storedInfo,sizeof(rtk_rg_intfInfo_t));

	////check wan_intf info & set dev name
	if((*intfIdx>7) || (*intfIdx<0)){
		debug("invalid wan_intf_idx\n");
		return RT_ERR_RG_INVALID_PARAM;
	}else{
		//dev_idx = (*intfIdx-1);
		dev_idx = (*intfIdx);
		sprintf(dev,"eth%d",dev_idx);
	}
	
	if(rg_db.systemGlobal.interfaceInfo[*intfIdx].valid==0){
		debug("Assigned wan_intf_idx is not valid.\n");
		return RT_ERR_RG_INVALID_PARAM;
	}else if(!intfInfo.is_wan){
		debug("Assigned wan_intf_idx is not wan.\n");
		return RT_ERR_RG_INVALID_PARAM;
	}else if(intfInfo.wan_intf.wan_intf_conf.wan_type!=RTK_RG_DHCP){
		debug("Assigned wan_intf_idx is not DHCP.\n");
		return RT_ERR_RG_INVALID_PARAM;
	}
	
	//ps cmd for bring up nas0 & nas0_x for dhcp

	//do cmd
    user_pipe_cmd("echo %d wan0 > /proc/rtl8686gmac/dev_port_mapping",intfInfo.wan_intf.wan_intf_conf.wan_port_idx);
	
	//user_cmd("%s nas0 up",ifconfig);
	user_cmd("%s %s down",ifconfig,dev);
	//user_cmd("/bin/ethctl remsmux bridge nas0 %s",dev);

	//smux: ignor napt cmd in ppp, just care vlan
	/*if(intfInfo.wan_intf.wan_intf_conf.egress_vlan_tag_on){
		//have vlan tag
		vid = intfInfo.wan_intf.wan_intf_conf.egress_vlan_id;
		user_cmd("/bin/ethctl addsmux ipoe nas0 %s nonapt vlan %d",dev,vid); 
	}else{
		//no vlan tag
		user_cmd("/bin/ethctl addsmux ipoe nas0 %s ",dev);
	}*/
	
	user_cmd("%s %s hw ether %02x%02x%02x%02x%02x%02x",ifconfig,dev,
			   intfInfo.wan_intf.wan_intf_conf.gmac.octet[0],
			   intfInfo.wan_intf.wan_intf_conf.gmac.octet[1],
			   intfInfo.wan_intf.wan_intf_conf.gmac.octet[2],
			   intfInfo.wan_intf.wan_intf_conf.gmac.octet[3],
			   intfInfo.wan_intf.wan_intf_conf.gmac.octet[4],
			   intfInfo.wan_intf.wan_intf_conf.gmac.octet[5]);

	user_cmd("/bin/ifconfig %s txqueuelen 10",dev);
	user_cmd("/bin/ifconfig %s mtu 1500",dev);
	user_cmd("/bin/iptables -A INPUT -i %s -p UDP --dport 69 -d 255.255.255.255 -m state --state NEW -j ACCEPT",dev);

	user_cmd("%s %s up",ifconfig,dev);

	user_pipe_cmd("/bin/udhcpc -i %s -W &",dev);
	//user_cmd("/bin/iptables -t nat -A POSTROUTING -o %s -j MASQUERADE",dev);
	
	return SUCCESS;
}

//rg_db.systemGlobal.initParam.pppoeBeforeDiagByHwCallBack;
int _rtk_rg_pppoeBeforeDiagByHwCallBack(rtk_rg_pppoeClientInfoBeforeDial_t* before_diag, int* intfIdx){


	char dev[8]; //device name, ex:nas0_0
	int dev_idx;
	//int vid;
	rtk_rg_intfInfo_t intfInfo;
	memcpy(&intfInfo,&rg_db.systemGlobal.interfaceInfo[*intfIdx].storedInfo,sizeof(rtk_rg_intfInfo_t));

	////check wan_intf info & set dev name
	if((*intfIdx>7) || (*intfIdx<0)){
		debug("invalid wan_intf_idx\n");
		return RT_ERR_RG_INVALID_PARAM;
	}else{
		//dev_idx = (*intfIdx-1);
		dev_idx = (*intfIdx);	
		sprintf(dev,"eth%d",dev_idx);
	}
	
	if(rg_db.systemGlobal.interfaceInfo[*intfIdx].valid==0){
		debug("Assigned wan_intf_idx is not valid.\n");
		return RT_ERR_RG_INVALID_PARAM;
	}else if(!intfInfo.is_wan){
		debug("Assigned wan_intf_idx is not wan.\n");
		return RT_ERR_RG_INVALID_PARAM;
	}else if(intfInfo.wan_intf.wan_intf_conf.wan_type!=RTK_RG_PPPoE){
		debug("Assigned wan_intf_idx is not PPPoE.\n");
		return RT_ERR_RG_INVALID_PARAM;
	}
	
	//ps cmd for bring up nas0 & nas0_x for ppp	

	//do cmd
    user_pipe_cmd("echo %d wan0 > /proc/rtl8686gmac/dev_port_mapping",intfInfo.wan_intf.wan_intf_conf.wan_port_idx);
	
	//user_cmd("%s nas0 up",ifconfig);
	//user_cmd("/bin/ethctl remsmux bridge nas0 %s",dev);

	//smux: ignor napt cmd in ppp, just care vlan
	/*if(intfInfo.wan_intf.wan_intf_conf.egress_vlan_tag_on){
		//have vlan tag
		vid = intfInfo.wan_intf.wan_intf_conf.egress_vlan_id;
		user_cmd("/bin/ethctl addsmux pppoe nas0 %s nonapt vlan %d",dev,vid); 
	}else{
		//no vlan tag
		user_cmd("/bin/ethctl addsmux pppoe nas0 %s ",dev);
	}*/
	
	user_cmd("%s %s hw ether %02x%02x%02x%02x%02x%02x",ifconfig,dev,
			   intfInfo.wan_intf.wan_intf_conf.gmac.octet[0],
			   intfInfo.wan_intf.wan_intf_conf.gmac.octet[1],
			   intfInfo.wan_intf.wan_intf_conf.gmac.octet[2],
			   intfInfo.wan_intf.wan_intf_conf.gmac.octet[3],
			   intfInfo.wan_intf.wan_intf_conf.gmac.octet[4],
			   intfInfo.wan_intf.wan_intf_conf.gmac.octet[5]);
	

	user_cmd("%s %s txqueuelen 10",ifconfig,dev);
	user_cmd("%s %s up",ifconfig,dev);


	user_cmd("/bin/ifconfig ppp%d txqueuelen 0",dev_idx);


	/*set proc for Radvd create global v6 ip*/
	user_pipe_cmd("/bin/echo 1 > /proc/sys/net/ipv6/conf/ppp%d/autoconf",dev_idx);
	user_pipe_cmd("/bin/echo 1 > /proc/sys/net/ipv6/conf/all/forwarding");
	user_pipe_cmd("/bin/echo 0 > /proc/sys/net/ipv6/conf/ppp%d/forwarding",dev_idx);
	//enable,disable ipv6 ppp diag
	//user_pipe_cmd("/bin/echo 1 > proc/sys/net/ipv6/conf/ppp%d/disable_ipv6",dev_idx);

	
	user_cmd("/bin/spppctl add %d pppoe %s username %s password %s gw 1 mru 1492 nohwnat ippt 0 debug 0 ipt 2 ",dev_idx,dev,before_diag->username,before_diag->password);
	user_cmd("/bin/spppctl katimer 100");

	//1 FIX ME: PPPoE disconnet issue
	/*Need to judge if PPPoE diag failed!!!*/
	//rg_kernel.ppp_diaged[dev_idx]=ENABLED;

	user_cmd("/bin/iptables -t nat -A POSTROUTING -o ppp%d -j MASQUERADE",dev_idx);
	
	return SUCCESS;
}

//rg_db.systemGlobal.initParam.arpAddByHwCallBack;
int _rtk_rg_arpAddByHwCallBack(rtk_rg_arpInfo_t* arpInfo){
	return SUCCESS;
}


//rg_db.systemGlobal.initParam.arpDelByHwCallBack;
int _rtk_rg_arpDelByHwCallBack(rtk_rg_arpInfo_t* arpInfo){
	return SUCCESS;
}


//rg_db.systemGlobal.initParam.macAddByHwCallBack=NULL;
int _rtk_rg_macAddByHwCallBack(rtk_rg_macEntry_t* macInfo){
	return SUCCESS;
}

//rg_db.systemGlobal.initParam.macDelByHwCallBack=NULL;
int _rtk_rg_macDelByHwCallBack(rtk_rg_macEntry_t* macInfo){
	return SUCCESS;
}


//rg_db.systemGlobal.initParam.naptAddByHwCallBack=NULL;
int _rtk_rg_naptAddByHwCallBack(rtk_rg_naptInfo_t* naptInfo){
	return SUCCESS;
}

//rg_db.systemGlobal.initParam.naptDelByHwCallBack=NULL;
int _rtk_rg_naptDelByHwCallBack(rtk_rg_naptInfo_t* naptInfo){
	return SUCCESS;
}


//rg_db.systemGlobal.initParam.routingAddByHwCallBack=NULL;
int _rtk_rg_routingAddByHwCallBack(rtk_rg_ipv4RoutingEntry_t* routingInfo){
	return SUCCESS;
}


//rg_db.systemGlobal.initParam.routingDelByHwCallBack=NULL;
int _rtk_rg_routingDelByHwCallBack(rtk_rg_ipv4RoutingEntry_t* routingInfo){
	return SUCCESS;
}


//rg_db.systemGlobal.initParam.bindingAddByHwCallBack=NULL;
int _rtk_rg_bindingAddByHwCallBack(rtk_rg_bindingEntry_t* bindingInfo){
	return SUCCESS;
}

//rg_db.systemGlobal.initParam.bindingDelByHwCallBack=NULL;
int _rtk_rg_bindingDelByHwCallBack(rtk_rg_bindingEntry_t* bindingInfo){
	return SUCCESS;
}


int _rtk_rg_neighborAddByHwCallBack(rtk_rg_neighborInfo_t* neighborInfo){
	return SUCCESS;
}

int _rtk_rg_neighborDelByHwCallBack(rtk_rg_neighborInfo_t* neighborInfo){
	return SUCCESS;
}

int _rtk_rg_v6RoutingAddByHwCallBack(rtk_rg_ipv6RoutingEntry_t* cb_routv6Et){
	return SUCCESS;
}

int _rtk_rg_v6RoutingDelByHwCallBack(rtk_rg_ipv6RoutingEntry_t* cb_routv6Et){
	return SUCCESS;
}

#else		//normal multi eth wan

//callback for rg init
int _rtk_rg_initParameterSetByHwCallBack(void)
{
	int i;
	char intf_name[20];


	user_cmd("%s eth0 down",ifconfig);
	user_cmd("%s nas0 down",ifconfig);

	for(i=0;i<8;i++){
		sprintf(intf_name,"nas0_%d",i);
		user_cmd("%s %s down",ifconfig,intf_name);


		//1 FIX ME: PPPoE disconnet issue
		/*
		if(rg_kernel.ppp_diaged[i]==ENABLED){
			user_cmd("/bin/spppctl down ppp%d",i);
			rg_kernel.ppp_diaged[i]=DISABLED;
		}
		*/
		//user_cmd("/bin/spppctl down ppp%d",i);
		//user_cmd("/bin/spppd");
	}

	//patch for ipatbles
	_delete_iptables_snat();

	//fix ptorocol stack send duplicated broadcast and multicast packets problem
	user_cmd("/bin/ebtables -N disBCMC");
	user_cmd("/bin/ebtables -A disBCMC -d Broadcast -j DROP");
	user_cmd("/bin/ebtables -A disBCMC -d Multicast -j DROP");
	user_cmd("/bin/ebtables -I FORWARD 1 -j disBCMC");

	//enable IGMP query if IGMP snooping is turn on in RG
	if(rg_db.systemGlobal.initParam.igmpSnoopingEnable)
	{
		user_pipe_cmd("echo 1 > /proc/br_igmpquery");
	}
	else
	{
		user_pipe_cmd("echo 0 > /proc/br_igmpquery");
	}
	
    return SUCCESS;

}



//rg_db.systemGlobal.initParam.interfaceAddByHwCallBack;
int _rtk_rg_interfaceAddByHwCallBack(rtk_rg_intfInfo_t* intfInfo, int* intfIdx)
{
	int i;
	unsigned char ip[16];
	unsigned char gw_ip[16];
    unsigned char mask[16];
	unsigned char brcast_ip[16];

	
    if(intfInfo->is_wan)
    {
        //add wan interface
        int i;
        char dev[8]; //device name, ex:nas0_0
        char* wan_type; //wan type: ex:PPPoE
        int vid=0;
		rtk_ip_addr_t bc_ip; //broadCast ip for wan intf
		rtk_rg_ipStaticInfo_t static_info;
		memset(&static_info,0,sizeof(rtk_rg_ipStaticInfo_t));

		//get static_info by different type
		
		
		if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_STATIC){
			 memcpy(&static_info,&intfInfo->wan_intf.static_info,sizeof(rtk_rg_ipStaticInfo_t));
		}else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_DHCP){
			 memcpy(&static_info,&intfInfo->wan_intf.dhcp_client_info.hw_info,sizeof(rtk_rg_ipStaticInfo_t));
		}else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_PPPoE){
			 memcpy(&static_info,&intfInfo->wan_intf.pppoe_info.after_dial.hw_info,sizeof(rtk_rg_ipStaticInfo_t));
		}
		
        //sprintf(dev,"nas0_%d",(*intfIdx-1));
        sprintf(dev,"nas0_%d",(*intfIdx));
        memcpy(ip,(void*)_inet_ntoa(static_info.ip_addr),16);
		memcpy(gw_ip,(void*)_inet_ntoa(static_info.gateway_ipv4_addr),16);
        memcpy(mask,(void*)_inet_ntoa(static_info.ip_network_mask),16);

		//caculate broacast ip
		bc_ip = static_info.gateway_ipv4_addr & static_info.ip_network_mask;

		for(i=0;i<32;i++){
			if(static_info.ip_network_mask & (1<<i)){
				//wan domain (mask)
			}else{
				//wan broacast ip
				bc_ip |= (1<<i);
			}
		}
		memcpy(brcast_ip,(void*)_inet_ntoa(bc_ip),16);

        switch(intfInfo->wan_intf.wan_intf_conf.wan_type)
        {
        case RTK_RG_STATIC:
            wan_type="ipoe";
            break;
        case RTK_RG_DHCP:
            wan_type="dhcp";
            break;
        case RTK_RG_PPPoE:
            wan_type="pppoe";
            break;
        case RTK_RG_BRIDGE:
            wan_type="bridge";
            break;
        default:
			wan_type=" ";
            break;
        }

        if(intfInfo->wan_intf.wan_intf_conf.egress_vlan_tag_on)
            vid = intfInfo->wan_intf.wan_intf_conf.egress_vlan_id;

        //do cmd
        user_pipe_cmd("echo %d nas0 > /proc/rtl8686gmac/dev_port_mapping",intfInfo->wan_intf.wan_intf_conf.wan_port_idx);


		//create nas0_X
		if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_DHCP){
			//DHCP protocal-stack setting is set by _rtk_rg_dhcpRequestByHwCallBack
		}else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_PPPoE){
			//PPPoE protocal-stack setting is set by _rtk_rg_pppoeBeforeDiagByHwCallBack
		}else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_BRIDGE){
			user_cmd("%s nas0 up",ifconfig);
			user_cmd("/bin/ethctl remsmux bridge nas0 %s",dev);
			
			if(static_info.napt_enable && intfInfo->wan_intf.wan_intf_conf.egress_vlan_tag_on)
			{	
				//NAPT + VALN TAG
				user_cmd("/bin/ethctl addsmux %s nas0 %s napt vlan %d",wan_type,dev,vid); 
			}
			else if(static_info.napt_enable)
			{
				//NAPT
				user_cmd("/bin/ethctl addsmux %s nas0 %s napt",wan_type,dev);
			}
			else if(intfInfo->wan_intf.wan_intf_conf.egress_vlan_tag_on){
				//VALN TAG
				user_cmd("/bin/ethctl addsmux %s nas0 %s nonapt vlan %d",wan_type,dev,vid); 
			}
			else{
				user_cmd("/bin/ethctl addsmux %s nas0 %s",wan_type,dev);
			}
			
			user_cmd("%s %s hw ether %02x%02x%02x%02x%02x%02x",ifconfig,dev,
					   intfInfo->wan_intf.wan_intf_conf.gmac.octet[0],
					   intfInfo->wan_intf.wan_intf_conf.gmac.octet[1],
					   intfInfo->wan_intf.wan_intf_conf.gmac.octet[2],
					   intfInfo->wan_intf.wan_intf_conf.gmac.octet[3],
					   intfInfo->wan_intf.wan_intf_conf.gmac.octet[4],
					   intfInfo->wan_intf.wan_intf_conf.gmac.octet[5]);

			user_cmd("%s %s txqueuelen 10",ifconfig,dev);
			user_cmd("%s %s up",ifconfig,dev);
			user_cmd("/bin/brctl addif br0 %s",dev);

		}
		else{
			//set STATIC & BRIDGE protocal-stack
	        user_cmd("%s nas0 up",ifconfig);
	        user_cmd("/bin/ethctl remsmux bridge nas0 %s",dev);

			if(static_info.napt_enable && intfInfo->wan_intf.wan_intf_conf.egress_vlan_tag_on)
	        {	
	        	//NAPT + VALN TAG
	            user_cmd("/bin/ethctl addsmux %s nas0 %s napt vlan %d",wan_type,dev,vid); 
	        }
	        else if(static_info.napt_enable)
	        {
	        	//NAPT
	            user_cmd("/bin/ethctl addsmux %s nas0 %s napt",wan_type,dev);
	        }
			else if(intfInfo->wan_intf.wan_intf_conf.egress_vlan_tag_on){
				//VALN TAG
				user_cmd("/bin/ethctl addsmux %s nas0 %s nonapt vlan %d",wan_type,dev,vid); 
			}
			else{
				user_cmd("/bin/ethctl addsmux %s nas0 %s",wan_type,dev);
			}
			
	        user_cmd("%s %s hw ether %02x%02x%02x%02x%02x%02x",ifconfig,dev,
	                   intfInfo->wan_intf.wan_intf_conf.gmac.octet[0],
	                   intfInfo->wan_intf.wan_intf_conf.gmac.octet[1],
	                   intfInfo->wan_intf.wan_intf_conf.gmac.octet[2],
	                   intfInfo->wan_intf.wan_intf_conf.gmac.octet[3],
	                   intfInfo->wan_intf.wan_intf_conf.gmac.octet[4],
	                   intfInfo->wan_intf.wan_intf_conf.gmac.octet[5]);
			
	        user_cmd("%s %s up",ifconfig,dev);
			user_cmd("%s %s mtu %d",ifconfig,dev,static_info.mtu);       
    	}


		/***IPv4 Setting***/
		if(static_info.ip_version==IPVER_V4ONLY || static_info.ip_version==IPVER_V4V6){

			if(static_info.ipv4_default_gateway_on){
				user_cmd("/bin/route add default gw %s",gw_ip);
			}
			
			//enable forwarding state
			user_pipe_cmd("echo 1 > /proc/sys/net/ipv4/ip_forward");

			if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_DHCP){
				//NAT set by _rtk_rg_dhcpRequestByHwCallBack
			}
			else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_PPPoE){
				//NAT set by _rtk_rg_pppoeBeforeDiagByHwCallBack
			}
			else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_BRIDGE){
			
			}
			else{		
				user_cmd("%s %s %s netmask %s broadcast %s",ifconfig,dev,ip,mask,brcast_ip);
				if(static_info.napt_enable){//add NAPT in iptables
					user_cmd("/bin/iptables -t nat -A POSTROUTING -o %s -j SNAT --to-source %s",dev,ip);
				}else{
					//pure routing
				}
			}
			
			//bring message to DDNS
			//user_cmd("/bin/updateddctrl %s",dev);

			//do_sys_cmd("/bin/ip route add default via %s table 32",gw_ip);
			//do_sys_cmd("/bin/ip route add 192.168.150.0/24 dev nas0_0 table 32");
		}



		/***IPv6 Setting***/
		if(static_info.ip_version==IPVER_V6ONLY || static_info.ip_version==IPVER_V4V6){
			if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_DHCP){
				//NAT set by _rtk_rg_dhcpRequestByHwCallBack
			}
			else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_PPPoE){
				//NAT set by _rtk_rg_pppoeBeforeDiagByHwCallBack
			}
			else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_BRIDGE){
			
			}
			else{		
				//current support ipv6_static only!!!
				user_cmd("%s %s add %s/%d",ifconfig,dev,_rtk_rg_inet_n6toa(&static_info.ipv6_addr.ipv6_addr[0]),static_info.ipv6_mask_length);
				user_pipe_cmd("echo 1 > /proc/sys/net/ipv6/conf/all/forwarding");
				if(static_info.ipv6_default_gateway_on){
					user_cmd("/bin/route -A inet6 add ::/0 gw %s %s",_rtk_rg_inet_n6toa(&static_info.gateway_ipv6_addr.ipv6_addr[0]),dev);
				}			
			}
		}

    }
    else
    {
    	//add lan interface
        memcpy(ip,(void*)_inet_ntoa(intfInfo->lan_intf.ip_addr),16);
        memcpy(mask,(void*)_inet_ntoa(intfInfo->lan_intf.ip_network_mask),16);
        
        user_cmd("%s eth0 down",ifconfig);
        user_cmd("%s br0 down",ifconfig);
#ifdef CONFIG_DUALBAND_CONCURRENT
		user_cmd("/bin/brctl addif br0 wlan1");
		//user_cmd("%s wlan1 down",ifconfig);
		user_pipe_cmd("echo 3 > /proc/vwlan");	//echo 3 means down
#endif


		user_cmd("%s br0 hw ether %02x%02x%02x%02x%02x%02x",ifconfig,
				   intfInfo->lan_intf.gmac.octet[0],
				   intfInfo->lan_intf.gmac.octet[1],
				   intfInfo->lan_intf.gmac.octet[2],
				   intfInfo->lan_intf.gmac.octet[3],
				   intfInfo->lan_intf.gmac.octet[4],
				   intfInfo->lan_intf.gmac.octet[5]);

		user_cmd("%s eth0 hw ether %02x%02x%02x%02x%02x%02x",ifconfig,
				   intfInfo->lan_intf.gmac.octet[0],
				   intfInfo->lan_intf.gmac.octet[1],
				   intfInfo->lan_intf.gmac.octet[2],
				   intfInfo->lan_intf.gmac.octet[3],
				   intfInfo->lan_intf.gmac.octet[4],
				   intfInfo->lan_intf.gmac.octet[5]);


		/***IPv4 Setting***/
		if(intfInfo->lan_intf.ip_version==IPVER_V4ONLY || intfInfo->lan_intf.ip_version==IPVER_V4V6){
	        user_cmd("%s br0 %s netmask %s mtu %d",ifconfig,ip,mask,intfInfo->lan_intf.mtu);
		}

		/***IPv6 Setting***/
		if(intfInfo->lan_intf.ip_version==IPVER_V6ONLY || intfInfo->lan_intf.ip_version==IPVER_V4V6){
			user_cmd("%s br0 add %s/%d",ifconfig,_rtk_rg_inet_n6toa(&intfInfo->lan_intf.ipv6_addr.ipv6_addr[0]),intfInfo->lan_intf.ipv6_network_mask_length);
		}

		user_cmd("%s br0 up",ifconfig);
        user_cmd("%s eth0 up",ifconfig);
#ifdef CONFIG_DUALBAND_CONCURRENT
		//user_cmd("%s wlan1 up",ifconfig);
		user_pipe_cmd("echo 2 > /proc/vwlan");	//echo 2 means up
#endif		
		
		//do cmd
		for(i=0;i<RTK_RG_MAC_PORT_MAX;i++){
			if(intfInfo->lan_intf.port_mask.portmask & (1<<i)){
				user_pipe_cmd("echo %d eth0 > /proc/rtl8686gmac/dev_port_mapping",i);
			}
		}

        //patch for iptables
        user_cmd("/bin/iptables -A INPUT -s %s -d %s -j ACCEPT",ip,ip);
#if 0
		//restart udhcpd: this will make signal hang
		do_sys_cmd("/bin/udhcpd -B /var/udhcpd/udhcpd.conf");
#endif
    }

	rtlglue_printf("intf[0] valid==%d\n",rg_db.systemGlobal.interfaceInfo[0].valid);
	//rtlglue_printf("intfIdx=%d	wan_type=%d@@@@@@n",*intfIdx,rg_db.systemGlobal.interfaceInfo[*intfIdx].storedInfo.wan_intf.wan_intf_conf.wan_type);

    return SUCCESS;
}

//rg_db.systemGlobal.initParam.interfaceDelByHwCallBack;
int _rtk_rg_interfaceDelByHwCallBack(rtk_rg_intfInfo_t* intfInfo, int* intfIdx)
{
	int ret,dhcpc_pid;
	struct task_struct *t;
	
	if(intfInfo->is_wan==0){
		user_cmd("%s eth0 down",ifconfig);
		user_cmd("%s br0 down",ifconfig);
	}else{
		char dev[8]; //device name, ex:nas0_0
		//sprintf(dev,"nas0_%d",(*intfIdx-1));
		sprintf(dev,"nas0_%d",(*intfIdx));
		if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_DHCP)
		{
			//1 FIXME:if there is two or more WAN use DHCP, this may not working
			dhcpc_pid = read_pid((char*)DHCPC_PID);
			DEBUG("the dhcpc_pid is %d",dhcpc_pid);
			if (dhcpc_pid > 0)
			{
				rcu_read_lock();
				t = find_task_by_vpid(dhcpc_pid);
				if(t == NULL){
					DEBUG("no such pid");
					rcu_read_unlock();
				}
				else
				{
					rcu_read_unlock();
					ret = send_sig(SIGTERM,t,0);//send_sig_info(SIG_TEST, &info, t);    //send the signal
					if (ret < 0) {
						DEBUG("error sending signal\n");
					}
				}
				
				//kill(dhcpc_pid, SIGTERM);
				//user_cmd("kill -%d %d",SIGTERM,dhcpc_pid);//kill(dhcpc_pid, SIGTERM);
			}
		}
		else if(intfInfo->wan_intf.wan_intf_conf.wan_type==RTK_RG_PPPoE)
		{
			if(rg_db.systemGlobal.not_disconnect_ppp==0){
				user_cmd("/bin/spppctl down %d",*intfIdx);
			}else{
				//don't call spppctl to disconnet interface pppX, because this case is "Server disconnet problem".
				//so, do nothing!
				rg_db.systemGlobal.not_disconnect_ppp=0;
			}
		}
		
		//wan-intf
		user_cmd("%s %s down",ifconfig,dev);
	}

    return SUCCESS;
}


//rg_db.systemGlobal.initParam.dhcpRequestByHwCallBack;
int _rtk_rg_dhcpRequestByHwCallBack(int* intfIdx){

	char dev[8]; //device name, ex:nas0_0
	int dev_idx;
	int vid;
	rtk_rg_intfInfo_t intfInfo;
	debug("%s is called!!!",__func__);

	memcpy(&intfInfo,&rg_db.systemGlobal.interfaceInfo[*intfIdx].storedInfo,sizeof(rtk_rg_intfInfo_t));


	////check wan_intf info & set dev name
	if((*intfIdx>7) || (*intfIdx<0)){
		debug("invalid wan_intf_idx\n");
		return RT_ERR_RG_INVALID_PARAM;
	}else{
		//dev_idx = (*intfIdx-1);
		dev_idx = (*intfIdx);
		sprintf(dev,"nas0_%d",dev_idx);
	}
	
	if(rg_db.systemGlobal.interfaceInfo[*intfIdx].valid==0){
		debug("Assigned wan_intf_idx is not valid.\n");
		return RT_ERR_RG_INVALID_PARAM;
	}else if(!intfInfo.is_wan){
		debug("Assigned wan_intf_idx is not wan.\n");
		return RT_ERR_RG_INVALID_PARAM;
	}else if(intfInfo.wan_intf.wan_intf_conf.wan_type!=RTK_RG_DHCP){
		debug("Assigned wan_intf_idx is not DHCP.\n");
		return RT_ERR_RG_INVALID_PARAM;
	}
	
	//ps cmd for bring up nas0 & nas0_x for dhcp
	//do cmd
    user_pipe_cmd("echo %d nas0 > /proc/rtl8686gmac/dev_port_mapping",intfInfo.wan_intf.wan_intf_conf.wan_port_idx);

	user_cmd("%s nas0 up",ifconfig);
	user_cmd("%s %s down",ifconfig,dev);
	user_cmd("/bin/ethctl remsmux bridge nas0 %s",dev);

	//smux: ignor napt cmd in ppp, just care vlan
	if(intfInfo.wan_intf.wan_intf_conf.egress_vlan_tag_on){
		//have vlan tag
		vid = intfInfo.wan_intf.wan_intf_conf.egress_vlan_id;
		user_cmd("/bin/ethctl addsmux ipoe nas0 %s nonapt vlan %d",dev,vid); 
	}else{
		//no vlan tag
		user_cmd("/bin/ethctl addsmux ipoe nas0 %s ",dev);
	}
	
	user_cmd("%s %s hw ether %02x%02x%02x%02x%02x%02x",ifconfig,dev,
			   intfInfo.wan_intf.wan_intf_conf.gmac.octet[0],
			   intfInfo.wan_intf.wan_intf_conf.gmac.octet[1],
			   intfInfo.wan_intf.wan_intf_conf.gmac.octet[2],
			   intfInfo.wan_intf.wan_intf_conf.gmac.octet[3],
			   intfInfo.wan_intf.wan_intf_conf.gmac.octet[4],
			   intfInfo.wan_intf.wan_intf_conf.gmac.octet[5]);

	user_cmd("/bin/ifconfig %s txqueuelen 10",dev);
	user_cmd("/bin/ifconfig %s mtu 1500",dev);
	user_cmd("/bin/iptables -A INPUT -i %s -p UDP --dport 69 -d 255.255.255.255 -m state --state NEW -j ACCEPT",dev);

	user_cmd("%s %s up",ifconfig,dev);

	//Delete the file to avoid accessing the last one.
	user_cmd("/bin/rm -f /var/udhcpc/udhcpc.info");
	user_pipe_cmd("/bin/udhcpc -i %s -W &",dev);
	user_cmd("/bin/iptables -t nat -A POSTROUTING -o %s -j MASQUERADE",dev);

	rtlglue_printf("Please cat /var/udhcpc/udhcpc.info to read DHCP request information.\n");

	return SUCCESS;
}




//rg_db.systemGlobal.initParam.pppoeBeforeDiagByHwCallBack;
int _rtk_rg_pppoeBeforeDiagByHwCallBack(rtk_rg_pppoeClientInfoBeforeDial_t* before_diag, int* intfIdx){


	char dev[8]; //device name, ex:nas0_0
	int dev_idx;
	int vid;
	rtk_rg_intfInfo_t intfInfo;
	memcpy(&intfInfo,&rg_db.systemGlobal.interfaceInfo[*intfIdx].storedInfo,sizeof(rtk_rg_intfInfo_t));

	////check wan_intf info & set dev name
	if((*intfIdx>7) || (*intfIdx<0)){
		debug("invalid wan_intf_idx\n");
		return RT_ERR_RG_INVALID_PARAM;
	}else{
		//dev_idx = (*intfIdx-1);
		dev_idx = (*intfIdx);	
		sprintf(dev,"nas0_%d",dev_idx);
	}
	
	if(rg_db.systemGlobal.interfaceInfo[*intfIdx].valid==0){
		debug("Assigned wan_intf_idx is not valid.\n");
		return RT_ERR_RG_INVALID_PARAM;
	}else if(!intfInfo.is_wan){
		debug("Assigned wan_intf_idx is not wan.\n");
		return RT_ERR_RG_INVALID_PARAM;
	}else if(intfInfo.wan_intf.wan_intf_conf.wan_type!=RTK_RG_PPPoE){
		debug("Assigned wan_intf_idx is not PPPoE.\n");
		return RT_ERR_RG_INVALID_PARAM;
	}
	
	
	//ps cmd for bring up nas0 & nas0_x for ppp
	//do cmd
	user_pipe_cmd("echo %d nas0 > /proc/rtl8686gmac/dev_port_mapping",intfInfo.wan_intf.wan_intf_conf.wan_port_idx);

	user_cmd("%s nas0 up",ifconfig);
	user_cmd("/bin/ethctl remsmux bridge nas0 %s",dev);

	//smux: ignor napt cmd in ppp, just care vlan
	if(intfInfo.wan_intf.wan_intf_conf.egress_vlan_tag_on){
		//have vlan tag
		vid = intfInfo.wan_intf.wan_intf_conf.egress_vlan_id;
		user_cmd("/bin/ethctl addsmux pppoe nas0 %s nonapt vlan %d",dev,vid); 
	}else{
		//no vlan tag
		user_cmd("/bin/ethctl addsmux pppoe nas0 %s ",dev);
	}

	
	
	user_cmd("%s %s hw ether %02x%02x%02x%02x%02x%02x",ifconfig,dev,
			   intfInfo.wan_intf.wan_intf_conf.gmac.octet[0],
			   intfInfo.wan_intf.wan_intf_conf.gmac.octet[1],
			   intfInfo.wan_intf.wan_intf_conf.gmac.octet[2],
			   intfInfo.wan_intf.wan_intf_conf.gmac.octet[3],
			   intfInfo.wan_intf.wan_intf_conf.gmac.octet[4],
			   intfInfo.wan_intf.wan_intf_conf.gmac.octet[5]);
	

	user_cmd("%s %s txqueuelen 10",ifconfig,dev);
	user_cmd("%s %s up",ifconfig,dev);


	user_cmd("/bin/ifconfig ppp%d txqueuelen 0",dev_idx);


	/*set proc for Radvd create global v6 ip*/
	user_pipe_cmd("/bin/echo 1 > /proc/sys/net/ipv6/conf/ppp%d/autoconf",dev_idx);
	user_pipe_cmd("/bin/echo 1 > /proc/sys/net/ipv6/conf/all/forwarding");
	user_pipe_cmd("/bin/echo 0 > /proc/sys/net/ipv6/conf/ppp%d/forwarding",dev_idx);
	//enable,disable ipv6 ppp diag
	//user_pipe_cmd("/bin/echo 1 > proc/sys/net/ipv6/conf/ppp%d/disable_ipv6",dev_idx);

	
	user_cmd("/bin/spppctl add %d pppoe %s username %s password %s gw 1 mru 1492 nohwnat ippt 0 debug 0 ipt 2 ",dev_idx,dev,before_diag->username,before_diag->password);
	user_cmd("/bin/spppctl katimer 100");

	//1 FIX ME: PPPoE disconnet issue
	/*Need to judge if PPPoE diag failed!!!*/
	//rg_kernel.ppp_diaged[dev_idx]=ENABLED;

	user_cmd("/bin/iptables -t nat -A POSTROUTING -o ppp%d -j MASQUERADE",dev_idx);
	
	return SUCCESS;
}



//rg_db.systemGlobal.initParam.arpAddByHwCallBack;
int _rtk_rg_arpAddByHwCallBack(rtk_rg_arpInfo_t* arpInfo){
	return SUCCESS;
}


//rg_db.systemGlobal.initParam.arpDelByHwCallBack;
int _rtk_rg_arpDelByHwCallBack(rtk_rg_arpInfo_t* arpInfo){
	return SUCCESS;
}


//rg_db.systemGlobal.initParam.macAddByHwCallBack=NULL;
int _rtk_rg_macAddByHwCallBack(rtk_rg_macEntry_t* macInfo){
	return SUCCESS;
}

//rg_db.systemGlobal.initParam.macDelByHwCallBack=NULL;
int _rtk_rg_macDelByHwCallBack(rtk_rg_macEntry_t* macInfo){
	return SUCCESS;
}


//rg_db.systemGlobal.initParam.naptAddByHwCallBack=NULL;
int _rtk_rg_naptAddByHwCallBack(rtk_rg_naptInfo_t* naptInfo){
	return SUCCESS;
}

//rg_db.systemGlobal.initParam.naptDelByHwCallBack=NULL;
int _rtk_rg_naptDelByHwCallBack(rtk_rg_naptInfo_t* naptInfo){
	return SUCCESS;
}


//rg_db.systemGlobal.initParam.routingAddByHwCallBack=NULL;
int _rtk_rg_routingAddByHwCallBack(rtk_rg_ipv4RoutingEntry_t* routingInfo){
	return SUCCESS;
}


//rg_db.systemGlobal.initParam.routingDelByHwCallBack=NULL;
int _rtk_rg_routingDelByHwCallBack(rtk_rg_ipv4RoutingEntry_t* routingInfo){
	return SUCCESS;
}


//rg_db.systemGlobal.initParam.bindingAddByHwCallBack=NULL;
int _rtk_rg_bindingAddByHwCallBack(rtk_rg_bindingEntry_t* bindingInfo){
	return SUCCESS;
}

//rg_db.systemGlobal.initParam.bindingDelByHwCallBack=NULL;
int _rtk_rg_bindingDelByHwCallBack(rtk_rg_bindingEntry_t* bindingInfo){
	return SUCCESS;
}


int _rtk_rg_neighborAddByHwCallBack(rtk_rg_neighborInfo_t* neighborInfo){
	return SUCCESS;
}

int _rtk_rg_neighborDelByHwCallBack(rtk_rg_neighborInfo_t* neighborInfo){
	return SUCCESS;
}

int _rtk_rg_v6RoutingAddByHwCallBack(rtk_rg_ipv6RoutingEntry_t* cb_routv6Et){
	return SUCCESS;
}

int _rtk_rg_v6RoutingDelByHwCallBack(rtk_rg_ipv6RoutingEntry_t* cb_routv6Et){
	return SUCCESS;
}
#endif

#endif

