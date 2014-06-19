# image loading script
echo "    Opconn image file loading script"
echo "  ===================================="

if [ -z $1 ]; then
    echo "  Usage: $0 <version>"
    echo "         version: x.y.z"
    echo " "
else
    echo "  Loading Opconn image file (version: $1 ) ......"
    if [ -e /nfs/proj/onu/img/onu-A.$1.img ]; then
        cp /nfs/proj/onu/img/onu-A.$1.img /jffs2/opconn.img
        cp /nfs/proj/onu/img/onu-A.$1.img /jffs2/opconn.img.bak
    else
        echo "  failed, invalid version number"
        return 1
    fi
    cp /nfs/proj/onu/img/cfg.tar.gz /jffs2/cfg.tar.gz
    cp /nfs/proj/onu/img/opl_xmodem.ko /jffs2/
    cp /nfs/proj/onu/img/uart_drv.ko /jffs2/
    cp /nfs/proj/onu/img/start.sh /jffs2/
    cp /nfs/proj/onu/img/watchdog.sh /jffs2/
    chmod 755 /jffs2/start.sh
    chmod 755 /jffs2/watchdog.sh
    echo "  done "
fi

