

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <str_utils.h>

#include <sys/types.h>
#include <ipc_client.h>
#include <ipc_protocol.h>
#include <debug.h>


ipc_acknowledge_hdr_t *ipc_sys_dhcpc_event_req(int ipc_fd, sys_dhcpc_t *dhcpc)
{
	ipc_sys_dhcpc_req_t req;

	req.hdr.ipc_type = IPC_SYS_DHCPC_EVENT;
	if (dhcpc) memcpy(&req.dhcpc, dhcpc, sizeof(sys_dhcpc_t));
	req.hdr.msg_len = REQ_MSGLEN(ipc_sys_dhcpc_req_t);
	ipc_send_request(ipc_fd, &req);
	
	return (ipc_acknowledge_hdr_t *)ipc_recv_acknowledge(ipc_fd);
}


static char *ipc_strerror_s(int err)
{
	static char buffer[64];
	sprintf(buffer, "IPC->Status = %d", err);
	if(err < IPC_STATUS_END)
		return (char *)buffer;
	else
		return "Unknown error";
}

int dhcpc_event_handler(int argc, char *argv[])
{
	sys_dhcpc_t req;
	ipc_acknowledge_hdr_t *pack;
	char *env, *vars[64];
	int ipc_fd = -1, items, i;
	
	DBG_ASSERT(argc > 1, "Invalid dhcpc event: empty");	
	if (argc <= 1){
		exit(1);
	}	
	
	memset(&req, 0, sizeof(req));
	
	ipc_fd = ipc_client_connect("dhcpc_event");
	if (ipc_fd < 0){
		exit(1);
	}

	env = getenv("interface");
	DBG_ASSERT(env, "Env interface lost");
	if (!env){
		goto err_exit;
	}
	safe_strncpy(req.ifname, env, sizeof(req.ifname));
	
	if (!strcmp(argv[1], "deconfig")){
		req.event = DHCPC_DECONFIG;
		pack = ipc_sys_dhcpc_event_req(ipc_fd, &req);
		if (!pack || (pack->status != IPC_STATUS_OK)){
			DBG_PRINTF("IPC Failed: %s", (pack == NULL) ? "No Ack" : ipc_strerror_s(pack->status));
		}
		if (pack) free(pack);
		
	}else if (!strcmp(argv[1], "bound") || !strcmp(argv[1], "renew")){
		req.event = !strcmp(argv[1], "renew") ? DHCPC_RENEW : DHCPC_BOUND;
		env = getenv("ip");
		if (!env || !inet_aton(env, &req.ip)){
			DBG_ASSERT(env, "Env ip lost or invalid");
			goto err_exit;
		}
		env = getenv("subnet");
		if (!env || !inet_aton(env, &req.subnet)){
			//DBG_ASSERT(env, "Env subnet lost or invalid");
			//goto err_exit;
		}
		env = getenv("broadcast");
		if (!env || !inet_aton(env, &req.broadcast)){
			//DBG_ASSERT(env, "Env broadcast lost or invalid");
			//goto err_exit;
		}		
		env = getenv("router");
		if (!env || !inet_aton(env, &req.gateway)){
			//DBG_ASSERT(env, "Env router lost or invalid");
			//goto err_exit;
		}

		env = getenv("dns");
		if (env){
			env = strdup(env);
			items = split_string(env, ' ', vars);
			if (items > MAX_DNS_NUM) items = MAX_DNS_NUM;
			for (i = 0; i < items; i ++){
				if (!inet_aton(vars[i], &req.dns[i])){
					break;
				}
			}
			free(env);
		}
		
		env = getenv("domain");
		if (env){
			safe_strncpy(req.domain, env, sizeof(req.domain));
		}	
		env = getenv("lease");
		if (env){
			req.lease_time = strtoul(env, NULL, 0);
		}			
		pack = ipc_sys_dhcpc_event_req(ipc_fd, &req);
		if (!pack || (pack->status != IPC_STATUS_OK)){
			DBG_PRINTF("IPC Failed: %s", (pack == NULL) ? "No Ack" : ipc_strerror_s(pack->status));
		}
		if (pack) free(pack);
	}

	if (ipc_fd > 0){
		close(ipc_fd);
	}
	exit(0);	

err_exit:
	if (ipc_fd > 0){
		close(ipc_fd);
	}
	exit(1);
}

