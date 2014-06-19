#!/bin/bash
#kernel_dynamic_len=`ls -lL $1/zImage | awk '{ print $5 }'`
#S1526_APP_SIZE=3670016
#S1526_KERNEL_SIZE=1572864
#S1526_FS_SIZE=2097152
#PRODUCT=`grep ENV_PRO= ./.env | sed "s/ENV_PRO=//" | awk '{printf $1}'`
#APP=`expr $[${PRODUCT}_APP_SIZE]`
#if [ $3 -eq 0 ];then
#kernel_len=`expr $[${PRODUCT}_KERNEL_SIZE]`
#fs_len=`expr $[${PRODUCT}_FS_SIZE]`
#else
#kernel_len=${kernel_dynamic_len}
#fs_len=`expr ${APP} - ${kernel_len}`
#fi

if [ $4 -eq 1 ]; then
echo "CONFIG_MTD_DYNAMIC_FLASH_PARTITION=y"
kernel_len=0
else
kernel_len=1572864
fi

BIN_TOOL_DIR=$3/mkfw
echo "bin tool dir = "$BIN_TOOL_DIR
APP_NAME=$2
echo "app name"$APP_NAME
cp $1/zImage ${BIN_TOOL_DIR}/
cp $1/squashfs_lzma.bin ${BIN_TOOL_DIR}/
make -C ${BIN_TOOL_DIR} app=${APP_NAME}.bin kernel_size=${kernel_len}
mv -f ${BIN_TOOL_DIR}/${APP_NAME}.bin $1/
rm -fr ${BIN_TOOL_DIR}/zImage ${BIN_TOOL_DIR}/squashfs_lzma.bin
