#!/bin/sh

export MIBS=ALL

SCALAR_CONF=mib2c.scalar.conf
TABLE_CONF=mib2c.iterate.conf
TARGET_DIR=../agent/mibgroup/nscrtv/mib2c


SCALAR_NODES="\
modEoCCBATDevInfoGroup \
modEoCCBATNetworkAttrGroup \
modEoCCBATDevStatusGroup \
modEoCCBATSNMPSettingGroup \
modEoCCBATOtherAdminGroup \
modEoCSoftwareUpgradeGroup \
modEoCCNUWhiteList \
modEoCCNU \
modEoCCNUService \
modEoCCNUPort \
modEoCTrapGroup \
modEoCVLANGroup"


TABLE_NODES="\
modEoCCBATAdminSNMPCommunityTable \
modEoCCBATRFTable \
modEoCCBATMACCNUIsolationTable \
modEoCCBATCardTable \
modEoCSoftwareUpgradeTable \
modEoCCNUWhiteListTable \
modEoCCNUTable \
modEoCCNUServiceTable \
modEoCCNUPortTable \
modEoCCNUPortInfoTable \
modEoCRealTimeStatisticTable \
modEoCIGMPSnoopingTable \
modEoCTrapTable \
modEoCVLANTable \
modEoCVLANPortTable \
modEoCBroadcastStormProtectionTable \
modEoCCNUConfigurationModelTable"


echo "Creating Scalar Nodes..."
for i in ${SCALAR_NODES}
do 
./mib2c -c ${SCALAR_CONF} $i
done

echo "Creating Table Nodes..."
for i in ${TABLE_NODES}
do 
./mib2c -c ${TABLE_CONF} $i
done

find -name "*.c~" | xargs rm -f
find -name "*.h~" | xargs rm -f 


mkdir -p ${TARGET_DIR}

mv mod* ${TARGET_DIR}



exit 0




