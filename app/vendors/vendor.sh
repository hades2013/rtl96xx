#!/bin/sh

WEBS="default en"


for w in $WEBS
do 
 $2/vendors/do_vendor.sh $1 $2 $w
done

exit 0
