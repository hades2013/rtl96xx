#!/bin/sh
insmod pr.ko & 2> /dev/null
insmod rtcore.ko
insmod rtk.ko
insmod liteRomeDriver.ko & 2> /dev/null
insmod rtdrv.ko
pon_mode=`flash get PON_MODE`
if [ "$pon_mode" = "PON_MODE=1" ]; then
	omci_app &
	echo "running GPON mode..."
elif [ "$pon_mode" = "PON_MODE=2" ]; then
	eponoamd &
	insmod epon_polling.ko & 2> /dev/null
	echo "running EPON mode ..."
else
	echo "running ether mode..."
fi
