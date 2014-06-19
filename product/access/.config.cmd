deps_config := \
	../../kconfig/priv_product_name.kg \
	../../kconfig/onu_compatible.kg \
	../../kconfig/loopd.kg \
	../../kconfig/ponmonitor.kg \
	../../kconfig/catvcom.kg \
	../../kconfig/diag.kg \
	../../kconfig/webs.kg \
	../../kconfig/common.kg \
	../../kconfig/cli.kg \
	../../kconfig/master.kg \
	./config.in

.config include/config.h: $(deps_config)

$(deps_config):
