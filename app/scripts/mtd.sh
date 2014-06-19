#!/bin/sh
for a in `seq 0 16` ; do
        mknod -m 0666 rootfs/dev/mtd$a c 90 `expr $a + $a`
        mknod -m 0666 rootfs/dev/mtdr$a c 90 `expr $a + $a + 1`
        mknod -m 0666 rootfs/dev/mtdblock$a b 31 $a
done
