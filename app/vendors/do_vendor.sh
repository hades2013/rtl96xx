#!/bin/sh

WEB_ATTR=default

if [ ! $2 = "" ];then
TOP_DIR=$2
else 
echo "Lose of TOP_DIR"
exit 1
fi

if [ ! $3 = "" ];then
WEB_ATTR=$3
fi

echo "Doing vendor config..."
echo "$1"
echo "$2"
echo "$3"


# default settings
WEBPAGES_DIR=${TOP_DIR}/www

HTML_DIR=${WEBPAGES_DIR}/web_asp 
FILE_SPEC_DEFAULT_JS=${WEBPAGES_DIR}/custom/spec_default.js

# English WEB pages
if [ ${WEB_ATTR} = "en" ];then
HTML_DIR=${WEBPAGES_DIR}/web_asp_en 
FILE_SPEC_DEFAULT_JS=${WEBPAGES_DIR}/custom/spec_default_en.js

elif [ ${WEB_ATTR} = "plc" ];then
HTML_DIR=${WEBPAGES_DIR}/web_asp_plc 
FILE_SPEC_DEFAULT_JS=${WEBPAGES_DIR}/custom/spec_default_plc.js

fi


FILE_VENDOR_H=${TOP_DIR}/shared/vendor.h
FILE_SPEC_JS=${HTML_DIR}/spec.js
FILE_LOGO=${HTML_DIR}/images/logo_*
FILE_BGGIF=${HTML_DIR}/images/bg.gif
FILE_DEVCFG=${TOP_DIR}/master/cfg/vendor_defaults.c

if [ $# -lt 1 ];
then 
echo "Lost the vendor specify file"
exit 1
fi

if [ $1 = "clean" ];then
rm -f ${FILE_VENDOR_H}
rm -f ${HTML_DIR}/spec.js 
rm -f ${HTML_DIR}/images/logo_* 
rm -f ${HTML_DIR}/images/bg.gif
echo "default" > ${TOP_DIR}/vendors/vendor.cfg 
exit 0
fi


CURRENT_VENDOR=`cat ${TOP_DIR}/vendors/vendor.cfg`

if [ $WEB_ATTR = default ] && [ $CURRENT_VENDOR = $1 ];then
exit 0
fi

echo $1 > ${TOP_DIR}/vendors/vendor.cfg

VENDOR_FILE=${TOP_DIR}/vendors/config/$1

if [ ! -f $VENDOR_FILE ]
then
echo "The config file '$VENDOR_FILE' doesn't exist"
exit 1
fi

source ${VENDOR_FILE}

#
# Check the variables that we need.
#

if [ "${VENDOR_MODEL}" == "" ];then
echo "Lost the value for 'VENDOR_MODEL'"
exit 1
fi

if [ "${VENDOR_LOGOPATH}" == "" ];then
echo "Lost the value for 'VENDOR_LOGOPATH'"
exit 1
fi

if [ "${VENDOR_BGGIF}" == "" ];then
echo "Lost the value for 'VENDOR_BGGIF'"
exit 1
fi

if [ "${VENDOR_DEVCFG}" == "" ];then
echo "Lost the value for 'VENDOR_DEVCFG'"
exit 1
fi

if [ "${VENDOR_STRING}" == "" ];then
echo "Lost the value for 'VENDOR_STRING'"
exit 1
fi

if [ "${VENDOR_SHORT}" == "" ];then
echo "Lost the value for 'VENDOR_SHORT'"
exit 1
fi

if [ "${VENDOR_NAME}" == "" ];then
echo "Lost the value for 'VENDOR_NAME'"
exit 1
fi

if [ "${VENDOR_LOCATION}" == "" ];then
echo "Lost the value for 'VENDOR_LOCATION'"
exit 1
fi

if [ "${VENDOR_YEAR}" == "" ];then
echo "Lost the value for 'VENDOR_YEAR'"
exit 1
fi

if [ "${BOARD_TYPE}" == "" ];then
BOARD_TYPE=CLT502
fi

VENDOR_LOGO_SRC=${TOP_DIR}/vendors/config/images/${VENDOR_LOGOPATH}
VENDOR_LOGO_DST=${HTML_DIR}/images/logo_${VENDOR_LOGOPATH}

rm -f ${FILE_LOGO}
cp -f ${VENDOR_LOGO_SRC} ${VENDOR_LOGO_DST}


VENDOR_BG_SRC=${TOP_DIR}/vendors/config/images/${VENDOR_BGGIF}
VENDOR_BG_DST=${HTML_DIR}/images/bg.gif

rm -f ${FILE_BGGIF}
cp -f ${VENDOR_BG_SRC} ${VENDOR_BG_DST}


VENDOR_DEVCFG_SRC=${TOP_DIR}/vendors/config/dev_cfg/${VENDOR_DEVCFG}
VENDOR_DEVCFG_DST=${FILE_DEVCFG}

rm -f ${VENDOR_DEVCFG_DST}
ln -s ${VENDOR_DEVCFG_SRC} ${VENDOR_DEVCFG_DST}

#
# Don't modify the text below.
#
#

str="COPYRIGHT © "
str+=$VENDOR_YEAR 
str+=" "
str+=$VENDOR_SHORT
str+=". ALL RIGHTS RESERVED."
COPYRIGHT_WEB=`echo $str | tr [a-z] [A-Z]`>/dev/null

VENDOR_NAME_U=`echo ${VENDOR_NAME} | tr [a-z] [A-Z]`>/dev/null


VTY_MOTD_CLI="\r\nEoC Master Controller.\r\nCopyright(C) "
VTY_MOTD_CLI+=$VENDOR_YEAR
VTY_MOTD_CLI+=" "
VTY_MOTD_CLI+=$VENDOR_SHORT
VTY_MOTD_CLI+=".\r\n\r\n"

#echo ${COPYRIGHT_WEB}
#echo ${VTY_MOTD_CLI}


#create vendor.js 
echo "" > $FILE_SPEC_JS
echo "/*This is an vendor specified javascript file," >>  $FILE_SPEC_JS
echo "It is created by vendor.sh automatically*/" >> $FILE_SPEC_JS
echo "" >> $FILE_SPEC_JS
echo "var str_copyright = '${COPYRIGHT_WEB}';" >> $FILE_SPEC_JS
echo "var PROD_MODEL = '${VENDOR_MODEL}';" >> $FILE_SPEC_JS
echo "var logo_name = 'images/logo_${VENDOR_LOGOPATH}';" >> $FILE_SPEC_JS
echo "" >> $FILE_SPEC_JS
cat ${FILE_SPEC_DEFAULT_JS} >> $FILE_SPEC_JS


#create vendor.h
echo "" > $FILE_VENDOR_H
echo "#ifndef __VENDOR_H__" >> $FILE_VENDOR_H
echo "#define __VENDOR_H__" >> $FILE_VENDOR_H
echo "" >> $FILE_VENDOR_H

echo "/*This is an vendor specified header file," >>  $FILE_VENDOR_H
echo "It is created by vendor.sh automatically*/" >> $FILE_VENDOR_H
echo "" >> $FILE_VENDOR_H

echo "#define MANUF_NAME		\"${VENDOR_STRING}\"" >> $FILE_VENDOR_H
echo "#define	MANUF_LOCATION		\"${VENDOR_LOCATION}\""  >> $FILE_VENDOR_H
echo "#define	COMPANY_NAME		\"${VENDOR_NAME}\""   >> $FILE_VENDOR_H
echo "#define	COMPANY_STRING		\"${VENDOR_STRING}\""   >> $FILE_VENDOR_H
echo "#define	PROD_MODEL		\"${VENDOR_MODEL}\""  >> $FILE_VENDOR_H
echo "#define	VTY_MOTD		\"${VTY_MOTD_CLI}\""  >> $FILE_VENDOR_H
echo "#define	FOR_${VENDOR_NAME_U}"  >> $FILE_VENDOR_H
echo "#define	BOARD_TYPE              \"${BOARD_TYPE}\""       >> $FILE_VENDOR_H


if [ "$SYS_MACROS" != "" ];then
 for m in $SYS_MACROS
 do 
 echo "#define $m " >> $FILE_VENDOR_H
 done
fi


echo " " >> $FILE_VENDOR_H
echo "#endif /*#ifndef __VENDOR_H__*/"  >> $FILE_VENDOR_H
echo " " >> $FILE_VENDOR_H

exit 0
