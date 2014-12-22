#!/bin/sh

cd vendors

c_prj_list=`ls | egrep '8696_|9601_|9607_' | egrep -v '8696_demo|9601_demo|9607_demo|9607_nand_demo'`
echo $c_prj_list

rel_dir=../customer_porject_release
test -d $rel_dir && rm -rf $rel_dir
mkdir $rel_dir
for l in $c_prj_list ; do
    tar -cjf $rel_dir/$l.tar.bz2 --exclude=.svn $l    
done
tree $rel_dir
