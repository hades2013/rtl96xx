# Rules for making busybox
-include ../../product/access/.config
#PATH+=:$(TARGETDIR)/../../../toolchains/opulan_toolchain/openwrt/bin:$(TARGETDIR)/../../../toolchains/opulan_toolchain/eldk/usr/bin
#PATH+=:$(shell pwd)/../../toolchains/rsdk-1.5.6-5281-EB-2.6.30-0.9.30.3-110915/bin
#SNMPD_DIR = net-snmp-5.4.2.1
SNMPD_DIR = net-snmp-5.6.2
ENTERPRISE_OID = 17409

SNMP_MIB_MODULES_INCLUDED = \
	mibII/system_mib \
	mibII/vacm_conf \
	snmpv3/usmConf \
	EPN104/miscIdent \
	EPN104/trap \
	EPN104/cltConfigRemoteUpgradeTable \
	EPN104/opRxInputTable \
	EPN104/opRxOutputTable \
	EPN104/dorDCPowerTable \
	product_common/snmp_ipc_interface 

	
ifeq ($(CONFIG_ZBL_SNMP), y) 
	SNMP_MIB_MODULES_INCLUDED += \
		EPN104/commonAdminGroup \
		EPN104/commonAgentTrapTable \
		EPN104/commonDeviceNum \
		EPN104/commonDeviceInfoTable \
		EPN104/analogPropertyTable \
		EPN104/alarmsIdent \
		EPN104/alarmLogTable \
		EPN104/zgNetwrokInfo \
		EPN104/zgSystemInfo \
	
endif
	

SNMP_MIB_MODULES_EXCLUDED = \
	agent_mibs \
	agentx \
	disman/event \
	disman/schedule \
	hardware \
	host \
	if-mib \
	mibII \
	notification \
	notification-log-mib \
	snmpv3mibs \
	target \
	tcp-mib \
	ucd_snmp \
	udp-mib \
	utilities \
	mibII/ifTable \
	mibII/sysORTable \
	mibII/interfaces \
	if-mib/ifXTable/ifXTable \
	mibII/snmp_mib \
	mibII/vacm_vars \
	mibII/vacm_context \
	
SNMP_TRANSPORTS_INCLUDED = Callback UDP 

SNMP_TRANSPORTS_EXCLUDED = TCP TCPIPv6 Unix

#TARGET_CFLAGS =-Os -pipe -mips32 -mtune=mips32 -funit-at-a-time  -fhonour-copts -msoft-float -fpic
#TARGET_CFLAGS =-Os -Wall -pipe  -mtune=mips32 -funit-at-a-time  -msoft-float -fpic

TARGET_CFLAGS =$(CXXFLAGS) 
TARGET_CFLAGS += -I$(SOFTWAREDIR)/include -I$(PDT_INC_DIR) -I$(TOPDIR)/shared -Wall

TARGET_LDFLAGS =  -T$(SOFTWAREDIR)/rules/shared.lds   -lifm -L$(PRODUCTDIR)/lwlib -lshared -L$(TOPDIR)/shared  -lcfg -L$(PRODUCTDIR)/lwlib
ifeq ($(CONFIG_ZBL_SNMP),y) 
CONFIGURE_ARGS = \
	--enable-static \
	--with-endianness=little \
	--with-logfile=/tmp/log/snmpd.log \
	--with-persistent-directory=/tmp/snmp/ \
	--with-default-snmp-version=2 \
	--with-sys-contact=root@localhost \
	--with-sys-location=Unknown \
	--with-enterprise-oid=$(ENTERPRISE_OID) \
	--with-enterprise-sysoid=1.3.6.1.4.1.$(ENTERPRISE_OID).1.9 \
	--with-enterprise-notification-oid=1.3.6.1.4.$(ENTERPRISE_OID)\
	--disable-manuals \
	--disable-mib-loading \
	--without-libwrap \
	--without-rpm \
	--with-openssl=internal \
	--without-zlib \
	--with-out-transports="$(SNMP_TRANSPORTS_EXCLUDED)" \
	--with-transports="$(SNMP_TRANSPORTS_INCLUDED)" \
	--with-out-mib-modules="$(SNMP_MIB_MODULES_EXCLUDED)" \
	--with-mib-modules="$(SNMP_MIB_MODULES_INCLUDED)" 
else 
CONFIGURE_ARGS = \
	--enable-static \
	--with-endianness=little \
	--with-logfile=/tmp/log/snmpd.log \
	--with-persistent-directory=/tmp/snmp/ \
	--with-default-snmp-version=2 \
	--with-sys-contact=root@localhost \
	--with-sys-location=Unknown \
	--with-enterprise-oid=$(ENTERPRISE_OID) \
	--with-enterprise-sysoid=1.3.6.1.4.1.$(ENTERPRISE_OID).1.15 \
	--with-enterprise-notification-oid=1.3.6.1.4.$(ENTERPRISE_OID)\
	--disable-manuals \
	--disable-mib-loading \
	--without-libwrap \
	--without-rpm \
	--with-openssl=internal \
	--without-zlib \
	--with-out-transports="$(SNMP_TRANSPORTS_EXCLUDED)" \
	--with-transports="$(SNMP_TRANSPORTS_INCLUDED)" \
	--with-out-mib-modules="$(SNMP_MIB_MODULES_EXCLUDED)" \
	--with-mib-modules="$(SNMP_MIB_MODULES_INCLUDED)" 
endif 

#--disable-debugging

snmpd-config:
	if [ ! -f $(SNMPD_DIR)/Makefile ]; then \
	cd $(SNMPD_DIR) ; \
	CC="$(CC)" \
	CFLAGS="$(TARGET_CFLAGS) " \
	LDFLAGS="$(TARGET_LDFLAGS)" \
	./configure \
	--target=mips-linux-uclibc \
	--host=mips-linux-uclibc \
	--build=i386-redhat-linux \
 	--prefix=$(TARGETDIR)/../../../app/$(SNMPD_DIR)/ipkg-install/ \
	--disable-nls  \
	--disable-ipv6 \
        --enable-mini-agent \
	$(CONFIGURE_ARGS); fi


snmpd-build: snmpd-config
	@$(MAKE) -C $(SNMPD_DIR) \
		LDFLAGS="$(TARGET_LDFLAGS) -lm -static" \
		all


snmpd-install:
	echo "$(CROSS_COMPILE)"
	$(CROSS_COMPILE)strip $(SNMPD_DIR)/agent/snmpd
	install -D $(SNMPD_DIR)/agent/snmpd $(TARGETDIR)/usr/sbin/
	
snmpd-clean:
	 if [ -f $(SNMPD_DIR)/Makefile ] ;then $(MAKE) -C $(SNMPD_DIR) clean ;rm -rf $(SNMPD_DIR)/Makefile;fi
