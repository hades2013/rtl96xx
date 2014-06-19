/*
 * sysmon.h
 *
 *  Created on: Jan 11, 2011
 *      Author: root
 */

#ifndef SYSMON_H_
#define SYSMON_H_

#define	SYSCMD_NONE		0x0000
#define	SYSCMD_MANIP	0x0001
#define	SYSCMD_NTP		0x0002
#define	SYSCMD_SNMP		0x0004
#define	SYSCMD_ACCOUNT	0x0008
#define	SYSCMD_CONFIG	0x0010
#define	SYSCMD_RESET	0x0020
#define	SYSCMD_SYSLOG	0x0040
#define	SYSCMD_VLAN		0x0080
#define	SYSCMD_MANSEC	0x0100
#define	SYSCMD_TMPL		0x0200		// Template changed
#define	SYSCMD_USER		0x0400		// User configuration changed
#define	SYSCMD_CABLE	0x0800

#define	SYSCMD_FLAG_NOFLAG		0x00
#define	SYSCMD_FLAG_REBOOT		0x80
#define	SYSCMD_FLAG_DEFCONF		0x40

#define	SYSCMD_MANIP_DHCP_BOUND		0x01

#define	SYSCMD_DELAY_MAX		0xff

#define	SYSMON_MKCMD(cmd, flag, delay) ( (cmd & 0xffff) | ( (flag & 0xff) << 16) | ((delay & 0xff) << 24) )
#define	SYSMON_GETCMD(msg)		(msg & 0x0000ffff)
#define	SYSMON_GETFLAG(msg)		((msg & 0x00ff0000) >> 16)
#define	SYSMON_GETDELAY(msg)	((msg & 0xff000000) >> 24)
#define	SYSMON_TESTCMD(msg, cmd)	((SYSMON_GETCMD(msg) & cmd) == cmd)
#define	SYSMON_TESTFLAG(msg, flag)	((SYSMON_GETFLAG(msg) & flag) == flag)

#endif /* SYSMON_H_ */
