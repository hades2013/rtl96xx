
#ifndef _MP_SYS_H
#define _MP_SYS_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#ifdef SUNOS_5
#define _XPG4_2
#define __EXTENSIONS__
#endif /* SUNOS_5 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#ifdef HAVE_STROPTS_H
#include <stropts.h>
#endif /* HAVE_STROPTS_H */
#include <sys/fcntl.h>
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif /* HAVE_SYS_SELECT_H */
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/param.h>
#ifdef HAVE_SYS_SYSCTL_H
#include <sys/sysctl.h>
#endif /* HAVE_SYS_SYSCTL_H */
#include <sys/ioctl.h>
#ifdef HAVE_SYS_CONF_H
#include <sys/conf.h>
#endif /* HAVE_SYS_CONF_H */
#ifdef HAVE_SYS_KSYM_H
#include <sys/ksym.h>
#endif /* HAVE_SYS_KSYM_H */
#include <syslog.h>
#include <time.h>
#include <sys/uio.h>
#include <sys/utsname.h>
#ifdef HAVE_RUSAGE
#include <sys/resource.h>
#endif /* HAVE_RUSAGE */

/* machine dependent includes */
#ifdef SUNOS_5
#include <limits.h>
#include <strings.h>
#endif /* SUNOS_5 */

/* machine dependent includes */
#ifdef HAVE_LINUX_VERSION_H
#include <linux/version.h>
#endif /* HAVE_LINUX_VERSION_H */

#ifdef HAVE_ASM_TYPES_H
#include <asm/types.h>
#endif /* HAVE_ASM_TYPES_H */

/* misc include group */
#include <stdarg.h>
#include <assert.h>

/* network include group */

#include <sys/socket.h>

#ifdef HAVE_SYS_SOCKIO_H
#include <sys/sockio.h>
#endif /* HAVE_SYS_SOCKIO_H */

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif /* HAVE_NETINET_IN_H */
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

#ifdef HAVE_NET_NETOPT_H
#include <net/netopt.h>
#endif /* HAVE_NET_NETOPT_H */

#include <net/if.h>

#ifdef HAVE_NET_IF_DL_H
#include <net/if_dl.h>
#endif /* HAVE_NET_IF_DL_H */

#ifdef HAVE_NET_IF_VAR_H
#include <net/if_var.h>
#endif /* HAVE_NET_IF_VAR_H */

//#include <net/route.h>

#ifdef HAVE_NETLINK
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#else
#define RT_TABLE_MAIN		0
#endif /* HAVE_NETLINK */

#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif /* HAVE_NETDB_H */

#include <arpa/inet.h>
#include <arpa/telnet.h>

#ifdef HAVE_INET_ND_H
#include <inet/nd.h>
#endif /* HAVE_INET_ND_H */

#ifdef HAVE_NETINET_IN_VAR_H
#include <netinet/in_var.h>
#endif /* HAVE_NETINET_IN_VAR_H */

#ifdef HAVE_NETINET_IN6_VAR_H
#include <netinet/in6_var.h>
#endif /* HAVE_NETINET_IN6_VAR_H */

#ifdef HAVE_NETINET6_IN_H
#include <netinet6/in.h>
#endif /* HAVE_NETINET6_IN_H */


#ifdef HAVE_NETINET6_IP6_H
#include <netinet6/ip6.h>
#endif /* HAVE_NETINET6_IP6_H */

#ifdef HAVE_NETINET_ICMP6_H
#include <netinet/icmp6.h>
#endif /* HAVE_NETINET_ICMP6_H */

#ifdef HAVE_NETINET6_ND6_H
#include <netinet6/nd6.h>
#endif /* HAVE_NETINET6_ND6_H */

#ifdef HAVE_LIBUTIL_H
#include <libutil.h>
#endif /* HAVE_LIBUTIL_H */

#ifdef BSDI_NRL

#ifdef HAVE_NETINET6_IN6_H
#include <netinet6/in6.h>
#endif /* HAVE_NETINET6_IN6_H */

#ifdef NRL
#include <netinet6/in6.h>
#endif /* NRL */

#define IN6_ARE_ADDR_EQUAL IN6_IS_ADDR_EQUAL

/* BSD/OS 4.0 has lost belows defines, it should appear at
   /usr/include/sys/socket.h.  */
#define CMSG_ALIGN(n)           (((n) + 3) & ~3)
#define CMSG_SPACE(l)   (CMSG_ALIGN(sizeof(struct cmsghdr)) + CMSG_ALIGN(l))
#define CMSG_LEN(l)     (CMSG_ALIGN(sizeof(struct cmsghdr)) + (l))

#endif /* BSDI_NRL */


/* Flag manipulation macros. */
#define CHECK_FLAG(V,F)      ((V) & (F))
#define SET_FLAG(V,F)        (V) = (V) | (F)
#define UNSET_FLAG(V,F)      (V) = (V) & ~(F)

/* Define as the return type of signal handlers (`int' or `void'). */
#define RETSIGTYPE void

#endif /*_MP_SYS_H */
