#include <stdio.h>
#include <stdint.h>
#include "web_voip.h"

#ifdef SUPPORT_CODEC_DESCRIPTOR
#include "codec_table.h"
#endif

extern char *supported_codec_string[SUPPORTED_CODEC_MAX];

#ifdef SUPPORT_CODEC_DESCRIPTOR
CT_ASSERT( ( sizeof( supported_codec_string ) / sizeof( supported_codec_string[ 0 ] ) ) == SUPPORTED_CODEC_MAX );
CT_ASSERT( SUPPORTED_CODEC_MAX == NUM_CODEC_TABLE );
#endif

//[SD6, bohungwu, e8c web
#define E8C_CODEC_NUM 4
static 	const char *e8c_codec_str[E8C_CODEC_NUM] = {"G711-ulaw", "G711-alaw", "G722", "G729"};
static 	int e8c_codec_idx[E8C_CODEC_NUM] = {0};
static	int rank[E8C_CODEC_NUM] = {0};
void asp_e8c_codec_get(request * wp, voipCfgPortParam_t *pCfg)
{
	int i, j, k;
	int e8c_codec_preced[E8C_CODEC_NUM] = {0}; //For trivial sorting

	//Find out E8C codec index from the codec set
	for (i=0, j=0; i<SUPPORTED_CODEC_MAX; i++)
	{
		for(k=0; k<E8C_CODEC_NUM; k++)
		{
			if( (strcmp(supported_codec_string[i], e8c_codec_str[k]) == 0) )
			{
				e8c_codec_idx[j]=i;
				e8c_codec_preced[j++] = pCfg->precedence[i];
				break;
			}
		}
	}

	//Sorting
	for(i=0; i<E8C_CODEC_NUM; i++)
	{
		int smallest = INT32_MAX;
		int s_idx = -1;
		for(j=0; j<E8C_CODEC_NUM; j++)
		{
			if(smallest > e8c_codec_preced[j])
			{
				s_idx = j;
				smallest = e8c_codec_preced[j];
			}
		}
		if(s_idx != -1)
		{
			rank[i] = e8c_codec_idx[s_idx];
			e8c_codec_preced[s_idx] = INT32_MAX;
		}
	}
	
	for (i=0; i<E8C_CODEC_NUM; i++)
	{
		boaWrite(wp, "<tr><td width=200px>codec priority %d:</td>\n", i+1);
		
		boaWrite(wp, "<td><select name=preced%d>\n", i);
		for (j=0; j<E8C_CODEC_NUM; j++)
		{
			if(rank[i] == e8c_codec_idx[j])
			{
				boaWrite(wp, "<option value=%d selected>%s", e8c_codec_idx[j], supported_codec_string[e8c_codec_idx[j]]);
			}
			else
			{
				boaWrite(wp, "<option value=%d>%s", e8c_codec_idx[j], supported_codec_string[e8c_codec_idx[j]]);
			}
		}
		boaWrite(wp, "</select></td></tr>\n");
	}
}

void asp_e8c_codec_set(request * wp, voipCfgPortParam_t *pCfg)
{
	int i;
	char szPrecedence[12];
	int codec_index;
	int e8c_codec_rank[E8C_CODEC_NUM] = {0};

	for(i=0; i<E8C_CODEC_NUM; i++)
	{
		printf("pCfg->precedence[rank[%d]]=%d\n", i, pCfg->precedence[rank[i]]);
		e8c_codec_rank[i] = pCfg->precedence[rank[i]];
	}
	
	for (i=0; i<E8C_CODEC_NUM; i++)
	{
		sprintf(szPrecedence, "preced%d", i);
		codec_index = atoi(boaGetVar(wp, szPrecedence, "-1"));
		printf("%s=%d\n", szPrecedence, codec_index);
		if( (codec_index >= 0) && (codec_index < SUPPORTED_CODEC_MAX) )
			pCfg->precedence[codec_index] = e8c_codec_rank[i];
		else
			printf("Error: invalid/unknown codec_index=%d\n", codec_index);

	}
}
//]



int asp_voip_e8c_get(int ejid, request * wp, int argc, char **argv)
{
	voipCfgParam_t *pVoIPCfg;
	voipCfgPortParam_t *pCfg;
	int i;
	int voip_port;

	if (web_flash_get(&pVoIPCfg) != 0)
		return -1;

	voip_port = atoi(boaGetVar(wp, "port", "0"));
	if (voip_port < 0 || voip_port >= VOIP_PORTS)
		return -1;

	pCfg = &pVoIPCfg->ports[voip_port];

	if (strcmp(argv[0], "sip_number")==0)
	{
		if(pCfg->proxies[0].number != NULL)
			boaWrite(wp, "%s", pCfg->proxies[0].number);
	}
	else if (strcmp(argv[0], "registerStatus")==0) 
	{
		FILE *fh;
		char buf[MAX_VOIP_PORTS * MAX_PROXY];

		i = atoi(boaGetVar(wp, "index", "0"));
		if (i < 0 || i >= MAX_PROXY)
		{
			printf("Unknown proxy index %d", i);
			boaWrite(wp, "%s", "ERROR");
			return 0;
		}
//		fprintf(stderr, "proxy index %d", i);

		if ((pCfg->proxies[i].enable & PROXY_ENABLED) == 0) {
			boaWrite(wp, "%s", "Not Registered");
			return 0;
		}
		
		fh = fopen(_PATH_TMP_STATUS, "r");
		if (!fh) {
			printf("Warning: cannot open %s. Limited output.\n", _PATH_TMP_STATUS);
			printf("\nerrno=%d\n", errno);
		}

		memset(buf, 0, sizeof(buf));
		if (fread(buf, sizeof(buf), 1, fh) == 0) {
			printf("Web: The content of /tmp/status is NULL!!\n");
			printf("\nerrno=%d\n", errno);
			boaWrite(wp, "%s", "ERROR");
		}
		else {
			/* SD6, bohungwu, fix incorrect registration status for the secondary proxy */
			char p0_reg_st, p1_reg_st, p_reg_st;

			p0_reg_st = buf[voip_port * MAX_PROXY];
			p1_reg_st = buf[voip_port * MAX_PROXY + 1];
			if(p1_reg_st == '1')
			{
				p_reg_st = p1_reg_st;
			}
			else
			{
				p_reg_st = p0_reg_st;
			}

			//fprintf(stderr, "buf is %s.\n", buf);
			switch (p_reg_st) {
				case '0':
					boaWrite(wp, "%s", "Not Registered");
					break;
				case '1':
					boaWrite(wp, "%s", "Registered");
					break;
				case '2':
					boaWrite(wp, "%s", "Registering");
					break;
				default:
					boaWrite(wp, "%s", "ERROR");
					break;
			}
		}

		fclose(fh);
	}
	
	else if (strcmp(argv[0], "proxy0_addr")==0)
	{
		if(pCfg->proxies[0].addr != NULL)
		{
			boaWrite(wp, "%s", pCfg->proxies[0].addr);
		}
		else
			printf("Proxy 0 addr is null!!!\n");
	}
	else if (strcmp(argv[0], "proxy0_port")==0)
	{
		boaWrite(wp, "%d", pCfg->proxies[0].port);
	}
	else if (strcmp(argv[0], "proxy1_addr")==0)
	{
		if(pCfg->proxies[1].addr != NULL)
			boaWrite(wp, "%s", pCfg->proxies[1].addr);
	}
	else if (strcmp(argv[0], "proxy1_port")==0)
	{
		boaWrite(wp, "%d", pCfg->proxies[1].port);
	}
	else if (strcmp(argv[0], "login_id")==0)
	{
		boaWrite(wp, "%s", pCfg->proxies[0].login_id);
	}
	else if (strcmp(argv[0], "password")==0)
	{
		boaWrite(wp, "%s", pCfg->proxies[0].password);
	}
	else if (strcmp(argv[0], "e8c_codec")==0)
	{
		asp_e8c_codec_get(wp, pCfg);
	}
	else if (strcmp(argv[0], "account_enable")==0)
	{
		boaWrite(wp, "%s", (pCfg->proxies[0].enable & PROXY_ENABLED) ? "checked" : "");
	}
	else if (strcmp(argv[0], "useLec")==0)
		boaWrite(wp, "%s", (pCfg->lec) ? "checked" : "");

	//]

	else
	{
		return -1;
	}

	return 0;
}

//[SD6, bohungwu, e8c web
//#define DBG_WEB_VAR(VAR_NAME) printf(#VAR_NAME "=%s\n", websGetVar(wp, T(#VAR_NAME), "0"))
#define DBG_WEB_VAR(VAR_NAME) do {} while(0)
//#define RCM_DBG printf("%s-%d\n", __FILE__, __LINE__)
#define RCM_DBG do {} while(0)
void asp_voip_e8c_set(request * wp, char *path, char *query)
{
	voipCfgParam_t *pVoIPCfg;
	voipCfgPortParam_t *pCfg;
	int i;
	char szPrecedence[12];
	char redirect_url[50];
	int voip_port;
	char szName[20];

	printf("---Enter %s---\n", __FUNCTION__);
	if (web_flash_get(&pVoIPCfg) != 0)
		return;

	RCM_DBG;
	voip_port = atoi(boaGetVar(wp, "voipPort", "0"));
	if (voip_port < 0 || voip_port >= VOIP_PORTS)
		return;

	RCM_DBG;
	pCfg = &pVoIPCfg->ports[voip_port];

	//Extract and set all received parameters
	//proxy0 addr
	RCM_DBG;
	DBG_WEB_VAR(proxy0_addr);
	snprintf(pCfg->proxies[0].addr, DNS_LEN, boaGetVar(wp, "proxy0_addr", ""));

	//proxy0 port
	RCM_DBG;
	DBG_WEB_VAR(proxy0_port);	
	pCfg->proxies[0].port = atoi(boaGetVar(wp, "proxy0_port", "5060"));
	
	//proxy1 addr
	RCM_DBG;
	DBG_WEB_VAR(proxy1_addr);	
	snprintf(pCfg->proxies[1].addr, DNS_LEN, boaGetVar(wp, "proxy1_addr", ""));
	
	//proxy1 port
	DBG_WEB_VAR(proxy1_port);	
	pCfg->proxies[1].port = atoi(boaGetVar(wp, "proxy1_port", "5060"));
	
	//account enable/disable
	DBG_WEB_VAR(account_enable);
	pCfg->proxies[0].enable = 0;//Reset setting
	pCfg->proxies[1].enable = 0;//Reset setting
	if( strcmp("enable", boaGetVar(wp, "account_enable", "")) == 0)
	{
		pCfg->proxies[0].enable |= PROXY_ENABLED;
		pCfg->proxies[1].enable |= PROXY_ENABLED;

		pCfg->proxies[0].enable |= PROXY_SUBSCRIBE; /* default enable subscribe*/
		pCfg->proxies[1].enable |= PROXY_SUBSCRIBE;
	}
	
	//login_id
	DBG_WEB_VAR(login_id);	
	snprintf(pCfg->proxies[0].login_id, DNS_LEN, boaGetVar(wp, "login_id", ""));
	snprintf(pCfg->proxies[1].login_id, DNS_LEN, boaGetVar(wp, "login_id", ""));
	//e8c, login id and sip number are the same
	snprintf(pCfg->proxies[0].number, DNS_LEN, boaGetVar(wp,"login_id", ""));
	snprintf(pCfg->proxies[1].number, DNS_LEN, boaGetVar(wp, "login_id", ""));
	
	//password
	DBG_WEB_VAR(password);	
	snprintf(pCfg->proxies[0].password, DNS_LEN, boaGetVar(wp, "password", ""));
	snprintf(pCfg->proxies[1].password, DNS_LEN, boaGetVar(wp, "password", ""));
	
	//codec
	asp_e8c_codec_set(wp, pCfg);

	//echo cancellation
	DBG_WEB_VAR(echo_cancellation);	
	i = atoi(boaGetVar(wp, "echo_cancellation", ""));
	if(i == 0)
	{
		pCfg->lec = 0;
	}
	else if	(i == 1)
	{
		pCfg->lec = 1;
	}
	else
	{
		fprintf(stderr, "Error, illega echo cancelation setting=%d\n", i);
	}
	web_flash_set(pVoIPCfg);

	sprintf(redirect_url, "/app_voip.asp");
#ifdef REBOOT_CHECK
	OK_MSG(redirect_url);
#else
	web_restart_solar();
	boaRedirect(wp, redirect_url);
#endif
}
//]


