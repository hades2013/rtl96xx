#!/bin/sh

touch /.init_enable_core
sysctl -w "kernel.core_pattern=/tmp/%e.%p.%s.%t.core"
ulimit -c unlimited

unlzma /lib/*.lzma

#unlzma /apps/opl_reg.lzma
#unlzma /apps/opl_mem.lzma


#=================== install modules ============================
echo "install modules : opl_reg_mmap, opl_minte, opl_cpuif, opl_mem"
insmod /modules/opl_reg_mmap.ko
insmod /modules/opl_minte.ko
insmod /modules/opl_cpuif.ko
#insmod /modules/opl_mem.ko

# resolve can't use CTRL+C in shell 
echo Redirect input to console,can use CTRL+C to interrupt in shell > /dev/console

#=================== mount jffs2 filesystem =====================
#sh /etc/mountjffs2.sh
sh /etc/mountmtd

mknod /dev/urandom c 1 9
mknod /dev/ppp c 108 0
mkdir /var/run

#=================== mount nfs OR start application in jffs2 ====
sh /etc/start_app.sh
