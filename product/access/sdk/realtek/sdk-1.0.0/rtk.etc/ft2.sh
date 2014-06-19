#!/bin/sh

mount -t proc proc proc
mount -t ramfs ramfs /var
echo "Your choice is $1";

LINE="================="

case $1 in
	"pcm")
		echo 1 > /proc/ft2/pcm_test ;
		echo "$LINE";
		cat /proc/ft2/pcm_test;
		echo "$LINE";
	;;
	"prbs")
		echo 31 > /proc/ft2/prbs_test;
		sleep $2;
		echo "$LINE";
		cat /proc/ft2/prbs_test;
		echo "$LINE";
	;;
	"usb")
		
		mkdir /var/usb1;
		mkdir /var/usb2;
		cp /var/config_voip.dat	/var/usb1/a
		usb1w=`diff /var/config_voip.dat /var/usb1/a`
		if [ $usb1w != "" ]; then \
			echo "USB1 write fiaild!"; \
			exit; \
		fi;
		cp /var/usb1/a /var/b
		usb1r=`diff /var/usb1/a /var/b`
		if [ $usb1r != "" ]; then \
			echo "USB1 read failed"!; \
			exit; \
		fi;
		rm -rf /var/b;
		rm -rf /var/usb1/a
		echo $LINE
		echo "USB1 Test PASS";
		echo $LINE
		#USB2
		cp /var/config_voip.dat /var/usb2/a
                usb2w=`diff /var/config_voip.dat /var/usb2/a`
                if [ $usb2w != "" ]; then \
                        echo "USB2 write fiaild!"; \
                        exit; \
                fi;
                cp /var/usb2/a /var/b
                usb2r=`diff /var/usb2/a /var/b`
                if [ $usb2r != "" ]; then \
                        echo "USB2 read failed"!; \
                        exit; \
		fi;
		rm -rf /var/b;
		rm -rf /var/usb2/a;
		rm -rf /var/usb1;
		rm -rf /var/usb2;
		echo $LINE
		echo "USB2 Test PASS";
		echo $LINE
	;;
	"all")
		echo 1 > /proc/ft2/pcm_test ;
 		echo 31 > /proc/ft2/prbs_test;
		sleep $2;
		echo "$LINE";
		cat /proc/ft2/pcm_test;
		cat /proc/ft2/prbs_test;
		echo "$LINE";
	;;
	*)
		echo "Usage:";
		echo "ft2.sh [pcm | prbs | all] [time]" ;
		exit;
	;;
esac
