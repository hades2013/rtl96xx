#!/bin/sh
headline=`ls -lL ./zImage | awk '{ print $5 }'`
echo "#define KERNEL_DYNAMIC_SIZE $headline" > $1