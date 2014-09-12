/* This is a generated file, don't edit */

#define NUM_APPLETS 64

const char applet_names[] ALIGN1 = ""
"ash" "\0"
"basename" "\0"
"brctl" "\0"
"cat" "\0"
"chmod" "\0"
"clear" "\0"
"cp" "\0"
"date" "\0"
"df" "\0"
"dmesg" "\0"
"dos2unix" "\0"
"echo" "\0"
"flash_eraseall" "\0"
"flash_lock" "\0"
"flash_unlock" "\0"
"halt" "\0"
"hwclock" "\0"
"ifconfig" "\0"
"init" "\0"
"insmod" "\0"
"kill" "\0"
"killall" "\0"
"killall5" "\0"
"klogd" "\0"
"ln" "\0"
"logger" "\0"
"logread" "\0"
"ls" "\0"
"lsmod" "\0"
"md5sum" "\0"
"mesg" "\0"
"mkdir" "\0"
"mknod" "\0"
"mount" "\0"
"mv" "\0"
"pidof" "\0"
"ping" "\0"
"poweroff" "\0"
"ps" "\0"
"pwd" "\0"
"rdate" "\0"
"reboot" "\0"
"rm" "\0"
"rmmod" "\0"
"route" "\0"
"sh" "\0"
"sleep" "\0"
"switch_root" "\0"
"sync" "\0"
"syslogd" "\0"
"tar" "\0"
"telnetd" "\0"
"tftp" "\0"
"top" "\0"
"touch" "\0"
"udhcpc" "\0"
"umount" "\0"
"uname" "\0"
"unix2dos" "\0"
"uptime" "\0"
"vconfig" "\0"
"vi" "\0"
"watchdog" "\0"
"wget" "\0"
;

#ifndef SKIP_applet_main
int (*const applet_main[])(int argc, char **argv) = {
ash_main,
basename_main,
brctl_main,
cat_main,
chmod_main,
clear_main,
cp_main,
date_main,
df_main,
dmesg_main,
dos2unix_main,
echo_main,
flash_eraseall_main,
flash_lock_unlock_main,
flash_lock_unlock_main,
halt_main,
hwclock_main,
ifconfig_main,
init_main,
insmod_main,
kill_main,
kill_main,
kill_main,
klogd_main,
ln_main,
logger_main,
logread_main,
ls_main,
lsmod_main,
md5_sha1_sum_main,
mesg_main,
mkdir_main,
mknod_main,
mount_main,
mv_main,
pidof_main,
ping_main,
halt_main,
ps_main,
pwd_main,
rdate_main,
halt_main,
rm_main,
rmmod_main,
route_main,
ash_main,
sleep_main,
switch_root_main,
sync_main,
syslogd_main,
tar_main,
telnetd_main,
tftp_main,
top_main,
touch_main,
udhcpc_main,
umount_main,
uname_main,
dos2unix_main,
uptime_main,
vconfig_main,
vi_main,
watchdog_main,
wget_main,
};
#endif

const uint16_t applet_nameofs[] ALIGN2 = {
0x0000,
0x3004,
0x000d,
0x3013,
0x2017,
0x001d,
0x2023,
0x0026,
0x002b,
0x002e,
0x2034,
0x303d,
0x0042,
0x0051,
0x005c,
0x0069,
0x006e,
0x0076,
0x007f,
0x0084,
0x008b,
0x0090,
0x0098,
0x00a1,
0x20a7,
0x00aa,
0x00b1,
0x20b9,
0x00bc,
0x20c2,
0x00c9,
0x30ce,
0x20d4,
0x00da,
0x00e0,
0x00e3,
0x40e9,
0x00ee,
0x00f7,
0x30fa,
0x00fe,
0x0104,
0x310b,
0x010e,
0x0114,
0x011a,
0x011d,
0x0123,
0x312f,
0x0134,
0x013c,
0x0140,
0x0148,
0x014d,
0x3151,
0x0157,
0x015e,
0x0165,
0x216b,
0x0174,
0x017b,
0x0183,
0x0186,
0x018f,
};


#define MAX_APPLET_NAME_LEN 14
