include .linux_config
include Makefile.rcm

ifndef DIR_TOOLS
DIR_TOOLS=$(DIR_LINUX)/../AP
endif

RC_LEVEL=/etc/init.d/rc28

all:
	make -C flash
#	make -C rtk_tr104
ifdef CONFIG_RTK_VOIP_DECT_SITEL_SUPPORT
	make -C dect_sitel/sc1445x_src sacvm.a 
endif
ifdef CONFIG_RTK_VOIP_DECT_DSPG_SUPPORT
	make -C src/dspg_cmbs_app
endif
ifdef RCM_SIMULATION
	make -C cmsim
endif
ifdef CONFIG_VOIP_SDK_TEMP
	make -C voip_manager
  ifeq ($(CONFIG_RTK_VOIP_IVR),y)
	make -C src/ivr
  endif
  ifneq ($(CONFIG_RTK_VOIP_PACKAGE_867X), y)
	make -C src/netlink
	make -C src/fwupdate
  endif
else
	make -C src
endif
ifdef CONFIG_RTK_VOIP_IP_PHONE
	make -C ui
endif
	make -C misc
	make -C test
	make -C gwdt

ecos-all:
	make -C voip_manager ecos-all
	make -C test ecos-all

romfs:
	# rock: let solar.conf can be modified in read-only fs
	$(ROMFSINST)	solar.conf /etc/solar.conf.in
	$(ROMFSINST)	-s /var/solar.conf  /etc/solar.conf
ifdef CONFIG_RTK_VOIP_IP_PHONE
	$(ROMFSINST)	ui/tmp/ui	/bin/ui
endif
	make -C voip_manager romfs
	make -C src/ivr romfs
	make -C gwdt romfs
ifdef CONFIG_VOIP_SDK_TEMP
 ifdef CONFIG_RTK_VOIP_IP_PHONE
	$(ROMFSINST)    src/solar_ipphone   /bin/solar
  else
	$(ROMFSINST)	src/solar	/bin/solar
  endif
	$(ROMFSINST)	src/solar_monitor	/bin/solar_monitor
#	$(ROMFSINST)	src/dns_task /bin/dns_task
  ifneq ($(CONFIG_RTK_VOIP_PACKAGE_867X), y)
	$(ROMFSINST)	src/fwupdate/fwupdate /bin/fwupdate
	$(ROMFSINST)	src/fwupdate/fwdownload.sh /bin/fwdownload.sh
	$(ROMFSINST)	src/netlink/netlink /bin/netlink
	$(ROMFSINST)	src/netlink/wanlink.sh /bin/wanlink.sh
  endif
else
	make -C src romfs
	make -C voip_manager romfs
endif
	make -C misc romfs 
	make -C test romfs
ifneq ($(CONFIG_RTK_VOIP_PACKAGE_867X), y)
	$(ROMFSINST)	script/autocfg.sh	/bin/autocfg.sh
endif
ifneq ($(RCSCONFIG),)
	make rcsconf
endif

.PHONY: rcX
rcX:
#	$(ROMFSINST) rc_rcmvoip            /etc/rc_rcmvoip
#	$(ROMFSINST) -a "/etc/rc_rcmvoip"  $(RC_LEVEL)
	$(ROMFSINST) rc_dev_voip               /etc/scripts/rc_dev_voip
	$(ROMFSINST) -a "/etc/scripts/rc_dev_voip"     $(RC_LEVEL)
	$(ROMFSINST) rc_voip.aipc              /etc/scripts/rc_voip
	$(ROMFSINST) -a "/etc/scripts/rc_voip&"        $(RC_LEVEL)
clean:
ifdef CONFIG_RTK_VOIP_IP_PHONE
	make -C ui clean
endif
ifdef CONFIG_RTK_VOIP_DECT_SITEL_SUPPORT
	make -C dect_sitel/sc1445x_src clean
endif
ifdef CONFIG_RTK_VOIP_DECT_DSPG_SUPPORT
	make -C src/dspg_cmbs_app clean
endif
ifdef CONFIG_VOIP_SDK_TEMP
	make -C voip_manager clean
	make -C src/ivr clean
ifneq ($(CONFIG_RTK_VOIP_PACKAGE_867X), y)
	make -C src/netlink clean
	make -C src/fwupdate clean
endif
else
	make -C src clean
endif
	make -C test clean
	make -C flash clean
	make -C gwdt clean
ifdef RCM_SIMULATION
	make -C cmsim clean
	-find $(ROMFSDIR)/etc -name solar.conf          | xargs -r rm -rf
	-find $(ROMFSDIR)/lib -name libvoip_manager.so  | xargs -r rm -rf
	-find $(ROMFSDIR)/bin -name fxo                 | xargs -r rm -rf
	-find $(ROMFSDIR)/bin -name rtp                 | xargs -r rm -rf
#	-find $(ROMFSDIR)/bin -name solar               | xargs -r rm -rf
#	-find $(ROMFSDIR)/bin -name solar_monitor       | xargs -r rm -rf
	-find $(ROMFSDIR)/bin -name dns_task            | xargs -r rm -rf
	-find $(ROMFSDIR)/bin -name ivrserver           | xargs -r rm -rf
	-find $(ROMFSDIR)/bin -name voipbox             | xargs -r rm -rf
	-find $(ROMFSDIR)/bin -name dbg                 | xargs -r rm -rf
	-find $(ROMFSDIR)/bin -name fskgen              | xargs -r rm -rf
	-find $(ROMFSDIR)/bin -name pulse_dial          | xargs -r rm -rf
	-find $(ROMFSDIR)/bin -name cp3_measure         | xargs -r rm -rf
	-find $(ROMFSDIR)/bin -name reg                 | xargs -r rm -rf
	-find $(ROMFSDIR)/bin -name ram                 | xargs -r rm -rf
	-find $(ROMFSDIR)/bin -name slic_reset          | xargs -r rm -rf
	-find $(ROMFSDIR)/bin -name switchmii           | xargs -r rm -rf
	-find $(ROMFSDIR)/bin -name vmwigen             | xargs -r rm -rf
	-find $(ROMFSDIR)/bin -name rtcp_statistic      | xargs -r rm -rf
	-find $(ROMFSDIR)/bin -name clone_mac           | xargs -r rm -rf
	-find $(ROMFSDIR)/bin -name bandwidth_mgr       | xargs -r rm -rf
	-find $(ROMFSDIR)/bin -name gpio_init           | xargs -r rm -rf
	-find $(ROMFSDIR)/bin -name gpio_read           | xargs -r rm -rf
	-find $(ROMFSDIR)/bin -name gpio_write          | xargs -r rm -rf
	-find $(ROMFSDIR)/bin -name send_2833           | xargs -r rm -rf
	-find $(ROMFSDIR)/bin -name voipcli             | xargs -r rm -rf
	-find $(ROMFSDIR)/bin -name tone                | xargs -r rm -rf
	-find $(ROMFSDIR)/bin -name playtone            | xargs -r rm -rf
	-find $(ROMFSDIR)/bin -name rtptalk             | xargs -r rm -rf
	-find $(ROMFSDIR)/bin -name autocfg.sh          | xargs -r rm -rf
endif

rcsconf:
	echo "CONFIG_RTK_VOIP_ETHERNET_DSP_IS_HOST=$(CONFIG_RTK_VOIP_ETHERNET_DSP_IS_HOST)" >> $(RCSCONFIG)
	echo "CONFIG_RTK_VOIP_ETHERNET_DSP_IS_DSP=$(CONFIG_RTK_VOIP_ETHERNET_DSP_IS_DSP)" >> $(RCSCONFIG)
	echo "CONFIG_RTK_VOIP_DSP_DEVICE_NR=$(CONFIG_RTK_VOIP_DSP_DEVICE_NR)" >> $(RCSCONFIG)
	echo "CONFIG_RTK_VOIP_IP_PHONE=$(CONFIG_RTK_VOIP_IP_PHONE)" >> $(RCSCONFIG)
	echo "CONFIG_RTK_VOIP_DECT_UART_SUPPORT=$(CONFIG_RTK_VOIP_DECT_UART_SUPPORT)" >> $(RCSCONFIG)
	echo "CONFIG_APP_RTK_VOIP=$(CONFIG_RTK_VOIP)" >> $(RCSCONFIG)

_prepare:
ifeq ($(CONFIG_DEFAULTS_KERNEL_2_6),y)
	ln -fs $(DIR_LINUX)/include/linux/autoconf.h includes/kernel_config.h
else
ifeq ($(CONFIG_DEFAULTS_KERNEL_3_4),y)
	ln -fs $(DIR_LINUX)/include/generated/autoconf.h includes/kernel_config.h
endif
endif
	ln -fs $(DIR_LINUX)/rtk_voip/include/rtk_voip.h includes/rtk_voip.h
	#ln -fs $(DIR_LINUX)/rtk_voip/include/voip_version.h includes/voip_version.h
	ln -fs $(DIR_LINUX)/rtk_voip/include/voip_control.h includes/voip_control.h
	ln -fs $(DIR_LINUX)/rtk_voip/include/voip_params.h includes/voip_params.h
	ln -fs $(DIR_LINUX)/rtk_voip/include/voip_feature.h includes/voip_feature.h
	ln -fs $(DIR_LINUX)/rtk_voip/include/voip_types.h includes/voip_types.h
	ln -fs $(DIR_LINUX)/rtk_voip/include/voip_debug.h includes/voip_debug.h
	ln -fs $(DIR_LINUX)/rtk_voip/include/voip_errno.h includes/voip_errno.h
	ln -fs $(DIR_LINUX)/acmw_lx/include/userdef.h includes/acmw_userdef.h
	ln -fs $(DIR_LINUX)/rtk_voip/srtp/crypto/include/crypto_types.h includes/crypto_types.h
	ln -fs $(DIR_LINUX)/include/linux/rtnetlink.h includes/rtnetlink.h
	ln -fs $(DIR_LINUX)/include/linux/netlink.h includes/netlink.h
	ln -fs $(DIR_LINUX)/include/linux/if_link.h includes/if_link.h
#common include
	ln -fs common/rcm_common_tapi.h includes/rcm_tapi.h
	ln -fs common/rcm_flash_client.h includes/voip_flash_client.h
	ln -fs common/rcm_flash.h includes/voip_flash.h
	ln -fs common/rcm_common_defs.h includes/rcm_sim_defs.h
	#tar -zxvf var.tgz

prepare:
	make _prepare


ifdef CONFIG_RTK_VOIP_IP_PHONE
	@if [ ! -e ui/include/ui_flash_layout.h ]; then	\
		make -C ui dep;	\
	fi
endif

	

