#!/bin/bash

ROOTDIR=$PWD

mkdir -p lib
mkdir -p usr/bin
mkdir -p usr/sbin
mkdir -p usr/lib
mkdir -p usr/share
mkdir -p usr/share/udhcpc
mkdir -p etc
mkdir -p mnt
mkdir -p sys
mkdir -p proc
mkdir -p etc
mkdir -p dev
mkdir -p bin
mkdir -p sbin
mkdir -p www

mkdir -p tmp
mkdir -p tmp/var
mkdir -p tmp/var/log
ln -sf tmp/var var
(cd $ROOTDIR/usr && ln -sf ../tmp)

echo "/lib" > etc/ld.so.conf
echo "/usr/lib" >> etc/ld.so.conf
echo "/mnt/lib" >> etc/ld.so.conf
echo "/mnt/usr/lib" >> etc/ld.so.conf
/sbin/ldconfig -r $ROOTDIR
