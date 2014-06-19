#/bin/sh
if [ $2 -eq 0 ]; then
$1/mkimage-new -A mips -O linux -T kernel -C none -a 80041000 -e 0x$(sed -n '/T kernel_entry/s/ T kernel_entry//p' $4/System.map) -n "MIPS Linux-$(date)" -d $5/vmlinux_org.bin $5/vmlinux.bix
else
$1/mkimage-new -i $2 -r $3 -A mips -O linux -T kernel -C none -a 80041000 -e 0x$(sed -n '/T kernel_entry/s/ T kernel_entry//p' $4/System.map) -n "MIPS Linux-$(date)" -d $5/vmlinux_org.bin $5/vmlinux.bix
fi
