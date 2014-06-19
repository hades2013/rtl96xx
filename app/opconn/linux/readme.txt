readme.txt

opl_xmodem
==========

1. xmodem.c 

    implement xmodem protocol and transmit/receive data

2. ufile.c

    kernel module file, transmit/receive file date via xmodem protocol

3. opl_xmodem.ko

    use xmodem.c ,ufile.c, Makefile to generate opl_xmodem.ko


Startup opconn
==============

1. Scripts

    load.onu-A.img.sh   - copy the associated files to flash for single port onu.
    load.onu-B.img.sh   - copy the associated files to flash for 4 ports onu with MV6046.
    load.onu-C.img.sh   - copy the associated files to flash for 4 ports onu with MV6045.
    load.onu-D.img.sh   - copy the associated files to flash for 4 ports onu with MV6097.
    start.sh            - opconn startup script
    watchdog.sh         - opconn watchdog script

2. Startup
    
    a. Prevision, execute the following command to put the files to nfs

        1) copy xmodem module
            cp <onu_source_path>/linux/opl_xmodem/opl_xmodem.ko <nfs_path>/proj/onu/img/

        2) copy onu loading script
            cp <onu_source_path>/linux/load.onu-A.img.sh <nfs_path>/proj/onu/img/
            cp <onu_source_path>/linux/load.onu-B.img.sh <nfs_path>/proj/onu/img/
            cp <onu_source_path>/linux/load.onu-C.img.sh <nfs_path>/proj/onu/img/
            cp <onu_source_path>/linux/load.onu-D.img.sh <nfs_path>/proj/onu/img/

        3) copy other scripts
            cp <onu_source_path>/linux/start.sh <nfs_path>/proj/onu/img/
            cp <onu_source_path>/linux/watchdog.sh <nfs_path>/proj/onu/img/

        5) build images
            a) Single port:
                I) make sure Makefile.conf is used for single port
                II) build
                    shell> make nuke;make;cp -f build/opconn /ucnfs/proj/onu/onu-A.<version>; cp -f build/opconn.img /ucnfs/proj/onu/img/onu-A.<version>.img
            b) 4 ports with MV6046:
                I) make sure Makefile.conf is used for 4 port with MV6046
                II) build
                    shell> make nuke;make;cp -f build/opconn /ucnfs/proj/onu/onu-B.<version>; cp -f build/opconn.img /ucnfs/proj/onu/img/onu-B.<version>.img
            c) 4 ports with MV6045:
                I) make sure Makefile.conf is used for 4 port with MV6045
                II) build
                    shell> make nuke;make;cp -f build/opconn /ucnfs/proj/onu/onu-C.<version>; cp -f build/opconn.img /ucnfs/proj/onu/img/onu-C.<version>.img
            d) 4 ports with MV6097:
                I) make sure Makefile.conf is used for 4 port with MV6097
                II) build
                    shell> make nuke;make;cp -f build/opconn /ucnfs/proj/onu/onu-D.<version>; cp -f build/opconn.img /ucnfs/proj/onu/img/onu-D.<version>.img

    b. When the board bootup, mount nfs and execute onu loading script file
    
    c. Reboot, the opconn will startup automatically


