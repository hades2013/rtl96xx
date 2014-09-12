#!/bin/sh

if [ $# -lt 5 ];then 
echo "Parameters lost"
exit 1
fi

output=$2
url=$1
type=$3
opt=$4
mac=$5
pidfile=${output}.pid

echo "Get Input:"$1","$2","$3","$4","$5

rm -f $output

wget -O $output -p $pidfile $url

result=$?

rm -f $pidfile

echo "wget done:"$result" "$output" "$type" "$opt" "$mac
/usr/sbin/master wget $result $output $type $opt $mac

exit 0
