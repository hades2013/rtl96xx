
#include "master.h"
#include "timer.h"
#include <ipc_protocol.h>
#include "build_time.h"
//#include "switch.h"
#include <shutils.h>
#include <str_utils.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time_zone.h>
#include "utils.h"
#include "sys_upgrade.h"
#include <drv_wtd_user.h>
#include <lw_type.h>
#include <lw_config_oid.h>
#include <lw_config_api.h>
#include <lw_if_pub.h>
#include <if_notifier.h>

int sshd_start(void)
{
	
	system("/usr/local/bin/dropbearkey -t rsa -f /etc/dropbear/dropbear_rsa_host_key");
	system("/usr/local/bin/dropbearkey -t dss -f /etc/dropbear/dropbear_dss_host_key");
	system("/usr/local/sbin/dropbear -v");
	return 0;
}

