This document describes the difference between AP and ADSL product.


1. Difference of Kconfig
The following changes are specific for ADSL WiFi driver.

* config USE_PCIE_SLOT_1: change from 'y' to 'n'
  choice "PCIe Slot 1 device": change from 'SLOT_1_92D' to 'SLOT_1_92C'

* config RTL_CLIENT_MODE_SUPPORT: change from 'y' to 'n'

* config RTL_WDS_SUPPORT: change from 'y' to 'n'

* config PHY_EAT_40MHZ: change from 'y' to 'n'
	
* add config RTL_TX_RESERVE_DESC:
config  RTL_TX_RESERVE_DESC
	bool "TX desc reserving mechanism"
	depends on RTL8192CD
	default y


2. Update version code (DRV_RELDATE) in 8192cd_cfg.h for CONFIG_RTL8672 path.

example:

#define DRV_RELDATE		"2012-07-24/2012-07-31"
Part 1, "2012-07-24": AP team driver date
Part 2, "2012-07-31": ADSL team driver date