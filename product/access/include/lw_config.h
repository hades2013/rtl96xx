#ifndef LW_CONFIG_H
#define LW_CONFIG_H
/*
 * Automatically generated header file: don't edit
 */


#undef HAVE_DOT_CONFIG


/*
 * Product Configuration
 */

/*
 * Product Select
 */
#define CONFIG_PRODUCT_EPN105 1


/*
 * Product Board Features select
 */
#define CONFIG_EOC_EXTEND 1


/*
 * SDK Configuration
 */
#define CONFIG_DEFAULTS_REALTEK 1

#define CONFIG_VENDOR "realtek"

#define CONFIG_DEFAULTS_REALTEK_SDK 1

#define CONFIG_SDK "sdk-1.0.0"


/*
 * libc select
 */
#define CONFIG_DEFAULTS_LIBC_UCLIBC_0_9_30 1

#define CONFIG_LIBCDIR "uClibc-0.9.30"


/*
 * System Configuration
 */
#define CONFIG_PRODUCT_NAME "EPN105"

#define CONFIG_COPYRIGHT_TIME "2014"

#define CONFIG_COPYRIGHT_STRING "Copyright (c) 2014 Hexicom Network Co., Ltd."

#define CONFIG_RELEASE_VER "EPN105V100R006"

#define CONFIG_DEVELOP_VER "EPN105V100D011"

#define CONFIG_DEVELOP_HARDWARE_VER "VER.B"

#define CONFIG_DEFAULT_HOSTNAME "EPN105"

#define DEFAULT_MANAGEIP "192.168.1.98"

#define DEFAULT_MANAGEMASK "255.255.255.0"

#define DEFAULT_MANAGEGW "192.168.1.1"

#define CONFIG_PRODUCT_SERIES_AND_FACTORY_CODE "EP001C"

#define CONFIG_PRODUCT_NAME_PRIV "EPN105"

#define CONFIG_RELEASE_VER_PRIV "EPN105V100R006"

#define CONFIG_DEVELOP_VER_PRIV "EPN105V100D011"


/*
 * load manufactory
 */
#define CONFIG_BOOT_RESTORE_APP 1

#define CONFIG_BOOT_MULTI_APP 1


/*
 * Platform Configuration
 */

/*
 * master configuration
 */
#undef CONFIG_MASTER


/*
 * cli configuration
 */
#undef CONFIG_VTYSH


/*
 * common configuration
 */
#define CONFIG_LIB 1

#define MAX_SYSTEM_NAME_LEN 30

#define MAX_PASSWORD_LEN 32

#define MAX_VLAN_DESC_LEN 32

#define MAX_VLANIF_DESC_LEN 80

#define MAX_PORT_DESC_LEN 80

#define MAX_IPSTR_LEN 20

#define MAC_ADDR_STR_LEN 15

#define MAX_AGGRIF_DESC_LEN 32


/*
 * webs configuration
 */
#undef CONFIG_HTTPD


/*
 * Realtek diag shell configuration
 */
#undef CONFIG_DIAG_DEBUG


/*
 * catv configuration
 */
#undef CONFIG_CATVCOM


/*
 * ponmonitor configuration
 */
#define CONFIG_PONMONITOR 1


/*
 * loodp configuration
 */
#undef CONFIG_LOOPD


/*
 * onu_compatible configuration
 */
#undef CONFIG_ONU_COMPATIBLE


/*
 * Priv product name configuration
 */
#undef PRIV_PRODUCT_NAME_SUPPORT

#endif /* MW_CONFIG_H */
