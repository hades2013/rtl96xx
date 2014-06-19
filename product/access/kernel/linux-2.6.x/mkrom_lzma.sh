#!/bin/sh

LINUX_BIN=linux.bin
LINUX_COMPRESS=$LINUX_BIN.lzma

$OBJCOPY -O binary -R .note -R .comment -S vmlinux linux.bin
[ -e $LINUX_COMPRESS ] && rm $LINUX_COMPRESS
../tools/u_boot_lzma e $LINUX_BIN $LINUX_COMPRESS
../tools/u_boot_mkimage -A mips -O linux -T kernel -C lzma -a 80000000 -e 80000000 -n "Linux Kernel Image" -d $LINUX_COMPRESS uImage

key="0xa0000203"
echo "key:"$key

flash_base=0xbd000000
ImageOffsetHex=`echo "obase=16;ibase=10; $(($flash_base))" | bc`

../tools/packimg/genhead -i uImage -o uImage.hdr -k $key -f $ImageOffsetHex -a 0x80000000 -e 0x80000000
cat uImage.hdr uImage > uImage_new
ls -al uImage_new


key="0xa0000403"
echo "key:"$key
../tools/packimg/genhead -i ../rootfs -o rootfs.hdr -k $key -f $ImageOffsetHex -a 0x80000000 -e 0x80000000
cat rootfs.hdr ../rootfs > rootfs_new
ls -al rootfs_new

cat uImage_new rootfs_new > vm.img
ls -al vm.img

