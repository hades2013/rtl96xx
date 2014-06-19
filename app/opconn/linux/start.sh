#!/bin/sh
#
#  COPYRIGHT (C) 2003-2008 Opulan Technologies Corp. ALL RIGHTS RESERVED.
#
#  Opulan reference design system startup script
#  

LOADIMAGE=/bin/opconn.img
LZMAIMAGE=/bin/opconn.lzma
APP=/bin/opconn

if [ -f $LOADIMAGE ]; then

	#insert uart_drv module
	insmod /modules/uart_drv.ko

	# Create device for uart
	mknod /dev/fpgaOnuUfile c 210 1

	# Insert opl_xmodem module
	insmod /modules/opl_xmodem.ko

	# Create device for xmodem
	mknod /dev/ufile c 220 0 

	# Create device for ppp
	mknod /dev/ppp c 108 0

	# Start application
	echo "Loading $LOADIMAGE ... "
	mv $LOADIMAGE $LZMAIMAGE
	unlzma $LZMAIMAGE
	chmod 777 $APP

	sh /etc/watchdog.sh $APP &

	cd /
	if [ -f $APP ]; then
		$APP
	else
		echo "Application start failed."
	fi;

else
	sh /etc/mountnfs.sh
fi;

