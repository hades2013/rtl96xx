#!/bin/sh

if [ $# -lt 9 ];then 
echo "Parameters lost"
exit 1
fi

user=$1
pwd=$2
port=$3
url=$4
rfile=$5
file=$6
type=$7
opt=$8
mac=$9
pidfile=${file}.pid


echo "Get Input:"$1","$2","$3","$4","$5","$6","$7","$8","$9

ftpput -i $pidfile -u $user -p $pwd -P $port $url $rfile $file

result=$?

rm -f $pidfile

/usr/sbin/master wget $result $file $type $opt $mac

exit 0

