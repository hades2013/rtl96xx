#!/bin/sh

export MIBS=ALL

SCALAR_CONF=mib2c.scalar.conf
TABLE_CONF=mib2c.iterate.conf
TARGET_DIR=../agent/mibgroup/hexicom/mib2c


SCALAR_NODES="\
eocCLTSystemInfo \
eocCLTConfigSystem \
cltConfigNetworking \
cltConfigService \
cltConfigAdministrator \
cltConfigRemoteUpgrade \
cltConfigRTC \
eocCLTConfigInterface \
eocCLTConfigVLAN \
cltConfigCableScalar \
eocCLTTemplateGroup \
eocCLTNetworkUnitGroup \
eocCLTNetworkUnitScalarGroup \
eocCLTDebug"


TABLE_NODES="\
cltConfigAdministratorTable \
cltConfigInterfaceTable \
cltConfigVLANTable \
cltConfigVLANTrunkTable \
cltConfigVLANHybridTable \
cltConfigCableTable \
cltDeviceModelTable \
cltTemplateTable \
cltTemplateInterfaceTable \
cltNetworkUnitTopologyTable \
cltNetworkUnitInterfaceTable \
cltNetworkUnitStatisticsTable \
cltNetworkUnitUserListTable "


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

mv clt* ${TARGET_DIR}
mv eoc* ${TARGET_DIR}



exit 0




