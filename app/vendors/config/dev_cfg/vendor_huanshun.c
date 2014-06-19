#ifndef __VENDOR_DEFAULTS_C__
#define __VENDOR_DEFAULTS_C__

#include <nvram.h>

#define DEVNAME_CNU101  "CNU101-H"
#define DEVNAME_CNU104  "CNU104-H"
#define DEVNAME_STB103  "STB103"
#define DEVNAME_CB201  "CB201"
#define DEVNAME_CB203  "CB203"
#define DEVNAME_CNU204  "CNU204-H"



struct nvram_tuple vendor_defaults[] = {

{"sta_dev_list_0",		"CNU101;"DEVNAME_CNU101";2;PHY", 		0 },
{"sta_dev_list_1",		"CNU104;"DEVNAME_CNU104";5;RTL8306;4,3,2,1,0",		0 },
{"sta_dev_list_2",		"STB103;"DEVNAME_STB103";4;RTL8306;4,2,1,0,3",		0 },
{"sta_dev_list_3",		"CB201;"DEVNAME_CB201";2;PHY",			0 },
{"sta_dev_list_4",		"CB203;"DEVNAME_CB203";4;RTL8306;4,2,1,0,3",		0 },
{"sta_dev_list_5",		"CNU204;"DEVNAME_CNU204";5;RTL8306;4,3,2,1,0",		0 },

{"tmpl_0",				"0;0;Default;Default template for "DEVNAME_CNU101,	0},
{"tmpl_0_0",			"0;100;full;0;off;0;0;on;1;ena",	0}, 
{"tmpl_0_1",			"1;auto;auto;0;off;0;0;off;1;ena",	0},
{"tmpl_0_misc", 		"0;0;off;0",							0},

{"tmpl_1",				"1;1;Default;Default template for "DEVNAME_CNU104,	0},
{"tmpl_1_0",			"0;100;full;0;off;0;0;on;1;ena",	0},
{"tmpl_1_1",			"1;auto;auto;0;off;0;0;off;1;ena",	0},
{"tmpl_1_2",			"2;auto;auto;0;off;0;0;off;1;ena",	0},
{"tmpl_1_3",			"3;auto;auto;0;off;0;0;off;1;ena",	0},
{"tmpl_1_4",			"4;auto;auto;0;off;0;0;off;1;ena",	0},
{"tmpl_1_misc",			"0;0;off;0",							0},

{"tmpl_2",				"2;2;Default;Default template for "DEVNAME_STB103,	0},
{"tmpl_2_0",			"0;100;full;0;off;0;0;on;1;ena",	0},
{"tmpl_2_1",			"1;auto;auto;0;off;0;0;off;1;ena",	0},
{"tmpl_2_2",			"2;auto;auto;0;off;0;0;off;1;ena",	0},
{"tmpl_2_3",			"3;auto;auto;0;off;0;0;off;1;ena",	0},
{"tmpl_2_misc",			"0;0;off;0",							0},

{"tmpl_3",				"3;3;Default;Default template for "DEVNAME_CB201,	0},
{"tmpl_3_0",			"0;100;full;0;off;0;0;on;1;ena",	0},
{"tmpl_3_1",			"1;auto;auto;0;off;0;0;off;1;ena",	0},
{"tmpl_3_misc",			"0;0;off;0",							0},

{"tmpl_4",				"4;4;Default;Default template for "DEVNAME_CB203,	0},
{"tmpl_4_0",			"0;100;full;0;off;0;0;on;1;ena",	0},
{"tmpl_4_1",			"1;auto;auto;0;off;0;0;off;1;ena",	0},
{"tmpl_4_2",			"2;auto;auto;0;off;0;0;off;1;ena",	0},
{"tmpl_4_3",			"3;auto;auto;0;off;0;0;off;1;ena",	0},
{"tmpl_4_misc",			"0;0;off;0",							0},

{"tmpl_5",				"5;5;Default;Default template for "DEVNAME_CNU204,	0},
{"tmpl_5_0",			"0;100;full;0;off;0;0;on;1;ena",	0},
{"tmpl_5_1",			"1;auto;auto;0;off;0;0;off;1;ena",	0},
{"tmpl_5_2",			"2;auto;auto;0;off;0;0;off;1;ena",	0},
{"tmpl_5_3",			"3;auto;auto;0;off;0;0;off;1;ena",	0},
{"tmpl_5_4",			"4;auto;auto;0;off;0;0;off;1;ena",	0},
{"tmpl_5_misc",			"0;0;off;0",							0},

{NULL, NULL, 0}
};

#endif /*#ifndef __VENDOR_DEFAULTS_C__*/


