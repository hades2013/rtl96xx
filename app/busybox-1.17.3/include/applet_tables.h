/* This is a generated file, don't edit */

#define NUM_APPLETS 57

const char applet_names[] ALIGN1 = ""
"ash" "\0"
"basename" "\0"
"cat" "\0"
"chmod" "\0"
"clear" "\0"
"cp" "\0"
"date" "\0"
"df" "\0"
"dmesg" "\0"
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
"rmdir" "\0"
"rmmod" "\0"
"route" "\0"
"sh" "\0"
"sleep" "\0"
"switch_root" "\0"
"sync" "\0"
"syslogd" "\0"
"tar" "\0"
"touch" "\0"
"udhcpc" "\0"
"umount" "\0"
"uname" "\0"
"uptime" "\0"
"vconfig" "\0"
"watchdog" "\0"
"wget" "\0"
;

#ifndef SKIP_applet_main
int (*const applet_main[])(int argc, char **argv) = {
ash_main,
basename_main,
cat_main,
chmod_main,
clear_main,
cp_main,
date_main,
df_main,
dmesg_main,
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
rmdir_main,
rmmod_main,
route_main,
ash_main,
sleep_main,
switch_root_main,
sync_main,
syslogd_main,
tar_main,
touch_main,
udhcpc_main,
umount_main,
uname_main,
uptime_main,
vconfig_main,
watchdog_main,
wget_main,
};
#endif

const uint16_t applet_nameofs[] ALIGN2 = {
0x0000,
0x3004,
0x300d,
0x2011,
0x0017,
0x201d,
0x0020,
0x0025,
0x0028,
0x302e,
0x0033,
0x0042,
0x004d,
0x005a,
0x005f,
0x0067,
0x0070,
0x0075,
0x007c,
0x0081,
0x0089,
0x0092,
0x2098,
0x009b,
0x00a2,
0x20aa,
0x00ad,
0x00b3,
0x30b8,
0x00be,
0x00c4,
0x00ca,
0x00cd,
0x40d3,
0x00d8,
0x00e1,
0x30e4,
0x00e8,
0x00ee,
0x30f5,
0x30f8,
0x00fe,
0x0104,
0x010a,
0x310d,
0x0113,
0x311f,
0x0124,
0x012c,
0x3130,
0x0136,
0x013d,
0x0144,
0x014a,
0x0151,
0x0159,
0x0162,
};


#define MAX_APPLET_NAME_LEN 14
