#!/bin/bash

# Generate preloader version for projects

set -e

plr_ver_file="preloader/include/soc.in.h"
plr_version=`egrep "PLR_VERSION" $plr_ver_file | awk '{printf $3 "\n"}'`

echo $plr_version

cd vendors
c_prj_list=`ls`
echo $c_prj_list

for l in $c_prj_list ; do
    [ -d $l ] && echo $plr_version > $l/proj_ver
    if ! svn info $l/proj_ver > /dev/null 2>&1; then
        echo "Adding $l/proj to SVN"
        svn add $l/proj_ver
    fi
done
