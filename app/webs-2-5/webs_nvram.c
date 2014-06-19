/*
 * asp_nvram.c
 *
 *  Created on: Jan 10, 2011
 *      Author: root
 */
#include	"uemf.h"
#include	"wsIntrn.h"
#include	"asp_variable.h"
#include	<signal.h>
#include	<unistd.h>
#include	<time.h>
#include	<sys/types.h>
#include 	<ipc_client.h>
#include 	<nvram.h>
#include	<debug.h>

extern int ipc_fd;

nvram_handle_t *nvram_handle;

int webs_nvram_init(void)
{
	nvram_handle = nvram_open_rdonly();
	if(nvram_handle)
		return 0;
	return -1;
}

int webs_nvram_reopen(void)
{
	nvram_close(nvram_handle);
	return webs_nvram_init();
}

char *webs_nvram_get(char *name)
{
	return nvram_safe_get(nvram_handle, name);
}

#if 0
int webs_nvram_init(void)
{
	return 0;
}

char *webs_nvram_get(char *name)
{
	static char value[256];
	ipc_nvram_access_ack_t *pack;

	pack = ipc_nvram_access(ipc_fd, NVRAM_ACCESS_GET, name, NULL);
	if(pack && pack->hdr.status == IPC_STATUS_OK)
		strcpy(value, pack->value);
	else
		DBG_PRINTF("IPC fail: %s!", pack == NULL ? "NO ACK" : ipc_strerror(pack->hdr.status));
	if(pack) free(pack);
	return value;
}
#endif

int webs_nvram_set(char *name, char *value)
{
	ipc_nvram_access_ack_t *pack;

	pack = ipc_nvram_access(ipc_fd, NVRAM_ACCESS_SET, name, value);
	DBG_ASSERT(pack && pack->hdr.status == IPC_STATUS_OK, "IPC fail: %s!", pack == NULL ? "NO ACK" : ipc_strerror(pack->hdr.status));
	if(pack) free(pack);
	return (pack && pack->hdr.status == IPC_STATUS_OK) ? 0 : -1;
}

int webs_nvram_commit(void)
{
	ipc_nvram_access_ack_t *pack;

	pack = ipc_nvram_access(ipc_fd, NVRAM_ACCESS_COMMIT, NULL, NULL);
	if(pack && pack->hdr.status == IPC_STATUS_OK) {
		webs_nvram_reopen();
	}
	else
		DBG_PRINTF("IPC fail: %s!", pack == NULL ? "NO ACK" : ipc_strerror(pack->hdr.status));

	if(pack) free(pack);

	return 0;
}

int webs_nvram_unset(char *name)
{
	ipc_nvram_access_ack_t *pack;

	pack = ipc_nvram_access(ipc_fd, NVRAM_ACCESS_UNSET, name, NULL);
	DBG_ASSERT(pack && pack->hdr.status == IPC_STATUS_OK, "IPC fail: %s!", pack == NULL ? "NO ACK" : ipc_strerror(pack->hdr.status));
	if(pack) free(pack);
	return (pack && pack->hdr.status == IPC_STATUS_OK) ? 0 : -1;
}
