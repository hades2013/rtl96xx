/*
 * asp_variable.c
 *
 *  Created on: Jan 10, 2011
 *      Author: root
 */

#include	"uemf.h"
#include	"wsIntrn.h"
#include	<signal.h>
#include	<unistd.h>
#include	<time.h>
#include	<sys/types.h>
#include 	"webs_nvram.h"
#include	<debug.h>

#include 	<config.h>
#include	<cable.h>
#include	"asp_variable.h"
#include    "pdt_config.h"


static int var_system_datetime(int argc, char_t **argv)
{
	return time(NULL);
}

/*
static int nvram_count(int argc, char_t **argv)
{
	int i;
	char nv_var[80];
	char *value;

	if(argc < 2) return 0;
	for(i = 0; i < 4096; i ++) {
		sprintf(nv_var, "%s_%d", argv[1], i);
		value = webs_nvram_get(nv_var);
		if(!value[0]) return i;
	}
	DBG_PRINTF("Too many loops!");
	return 0;
}
*/
const asp_variable_table_t asp_const_table[] = {
		{"EOC_MAX_VLAN_NUMS",	.data.integer = EOC_MAX_VLAN_NUMS,	VAR_TYPE_INT, 		NULL,	NULL},
		{"EOC_MAX_DEVS_NUMS",	.data.integer = MAX_SUPPORTED_CNU_MODELS,	VAR_TYPE_INT,		0,		NULL},
		{"EOC_MAX_TMPL_NUMS",	.data.integer = EOC_MAX_TMPL_NUMS,	VAR_TYPE_INT,		0,		NULL},
		{"EOC_MAX_USER_NUMS",	.data.integer = EOC_MAX_USER_NUMS,	VAR_TYPE_INT,		0,		NULL},
		{"SYS_MODEL_NAME",		.data.string = PROD_MODEL,			VAR_TYPE_STRING,	NULL,	NULL},
#if 0		
/*to be del*/	{"SYSTEM_DATETIME",		.data.integer = 0,					VAR_TYPE_DATETIME,	(var_func_t)var_system_datetime,	NULL},
/*to be del*/	{"SUPPORTED_DEVICES",	.data.integer = MAX_SUPPORTED_CNU_MODELS,	VAR_TYPE_STR_ARRAY, NULL,	"sta_dev_list"},
/*to be del*/	{"TEMPLATE_LIST",		.data.integer = EOC_MAX_TMPL_NUMS,	VAR_TYPE_STR_ARRAY,	NULL,	"tmpl"},
/*to be del*/	{"USER_LIST",			.data.integer = EOC_MAX_USER_NUMS,	VAR_TYPE_STR_ARRAY,	NULL,	"user"},
/*to be del*/	{"VLAN_LIST",			.data.integer = EOC_MAX_VLAN_NUMS,	VAR_TYPE_STR_ARRAY2, NULL,	"vlan"},
/*to be del*/	{"NVRAM_COUNT",			.data.integer = 0,					VAR_TYPE_INT,		(var_func_t)nvram_count,	NULL },
/*to be del*/	{"FWVER",				.data.string = FIRMWARE_VERSION,	VAR_TYPE_STRING,	NULL,	NULL },
/*to be del*/	{"FWBT",				.data.integer = BUILD_TIME,			VAR_TYPE_DATETIME,	NULL,	NULL },
#endif 
		{.name = NULL}
};


int asp_get_variable(int eid, webs_t wp, int argc, char_t **argv)
{
	int i, /*wlen,*/ integer;
	char *string;
//	struct tm TM;
//	time_t time;
//	char nv_name[80], *nv_val;
//	int idx, cnt;

	for(i = 0; asp_const_table[i].name; i++) {
		if(!strcmp(asp_const_table[i].name, argv[0])) {
			switch(asp_const_table[i].type) {
			case VAR_TYPE_INT:
				integer = asp_const_table[i].data.integer;
				if(asp_const_table[i].func) integer = asp_const_table[i].func(argc, argv);
				return websWrite(wp, T("%d"), integer);

			case VAR_TYPE_STRING:
				string = asp_const_table[i].data.string;
				if(asp_const_table[i].func) string = (char *)asp_const_table[i].func(argc, argv);
				return websWrite(wp, T("%s"), string);
/*
			case VAR_TYPE_DATETIME:

				time = asp_const_table[i].data.integer;
				if(asp_const_table[i].func) time = asp_const_table[i].func(argc, argv);
				gmtime_r(&time, &TM);
				return websWrite(wp, T("\"%d\",\"0\",\"%d/%d/%d\",\"%d:%d:%d\",\"\",\"\",\"\",\"\""), 0,
							TM.tm_mon+1,TM.tm_mday,TM.tm_year+1900,TM.tm_hour,TM.tm_min,TM.tm_sec);

			case VAR_TYPE_STR_ARRAY:
				for(idx = 0; idx < asp_const_table[i].data.integer; idx ++) {
					if(idx > 0)
						wlen += websWrite(wp, ",\n");
					else
						wlen += websWrite(wp, "\n");
					sprintf(nv_name, "%s_%d", asp_const_table[i].nvram_name, idx);
					nv_val = webs_nvram_get(nv_name);
					if(nv_val[0])
						wlen += websWrite(wp, "'%s'", nv_val);
					else
						wlen += websWrite(wp, "''");
				}
				return wlen;

			case VAR_TYPE_STR_ARRAY2:
				cnt = 0;
				for(idx = 0; idx < asp_const_table[i].data.integer; idx ++) {
					sprintf(nv_name, "%s_%d", asp_const_table[i].nvram_name, idx);
					nv_val = webs_nvram_get(nv_name);
					if(nv_val[0]) {
						if(cnt > 0)
							wlen += websWrite(wp, ",\n");
						wlen += websWrite(wp, "'%s'", nv_val);
						cnt ++;
					}
				}
				return wlen;
*/				
			}
		}

	}
	DBG_PRINTF("Variable [%s] not found!", argv[0]);
	return 0;
}

