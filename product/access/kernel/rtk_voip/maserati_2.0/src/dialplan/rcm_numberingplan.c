#include "rcm_numberingplan.h"
#include "rcm_voiplog.h"


static void DoCallxxxByNumberingPlan( LinphoneCore *lc, guint32 ssid, char *src_ptr,prefixinfo_t *featurekey_p );
int DoSetFeatureByNumberingPlan( LinphoneCore *lc,  prefixinfo_t *featurekey_p );



/* define feature key , follow TR0104 , E8C */
prefixinfo_t prefixinfolist[PrefixlistEntries] =
{ /* feature key,min_length,max_length,facility ,remove feature key, remove pound key , txt log */
	{"*31#"	,	4,	40,		CA_ACTIVATE,			0,	1,	"Caller ID Display"},
	{"*31*"	,	4,	40,		X_CA_DEACTIVATE,		0,	1,	"Caller ID Hidden"},		
	{"*21*"	,	4,	40,		CFU_ACTIVATE,			0,	0,	"Enable CFU"},
	{"#21#"	,	4,	4, 		CFU_DEACTIVATE,			0,	0,	"Disable CFU"},
	{"*#21#" ,	5,	5,		X_CFU_STATUS, 			0,	0,	"CFU Status"},
	{"*69*"	,	4,	40,		CFB_ACTIVATE,			0,	0,	"Enable CFB"},
	{"#69#"	,	4,	4,		CFB_DEACTIVATE,			0,	0,	"Disable CFB"},
	{"*#69#",	5,	5,		X_CFB_STATUS,			0,	0,	"CFB STATUS"},
	{"*61*" ,	4,	40, 	CFNR_ACTIVATE,			0,	0,	"Enable CFNR"},
	{"#61#" ,	4,	4,		CFNR_DEACTIVATE, 		0,	0,	"Disable CFNR"},
	{"*#61#" ,	5,	5,		X_CFNR_STATUS,			0,	0,	"CFNR Status"},

	{"*43#"	,	4,	4,		CW_ACTIVATE,			1,	0,	"Enable CallWaiting"},
	{"#43#" ,	4,	4,		CW_DEACTIVATE,			1,	0,	"Disable CallWaiting"},

	
	{"*33#"	,	4,	4,		X_MCID_ACTIVATE,		0,	1,	"Activate MCID"},
	
	{"*54*"	,	4,	40,		OCB_ACTIVATE,			0,	1,	"Set Call out restriction"},
	{"#54*"	,	4,	40,		OCB_DEACTIVATE,			0,	1,	"Disable Call out restriction" },
	{"*26#" ,	4,	4,	 	X_DND_ACTIVATE,			0,	0,	"Enable DND"},
	{"#26#" ,	4,	4, 		X_DND_DEACTIVATE, 		0,	0,	"Disable DND"},
	
	{"*51*"	,	4,	40,		AA_REGISTER,			0,	0,	"Set Abbreviated dial phonebook"},
	{"**"	,	2,	40,		X_AA_ACTIVATE,			0,	1,	"Start Abbreviated dial"},
	{"#51*"	,	4,	40,		AA_ERASE,				0,	1,	"Disable Abbreviated dial phonebook"},
	
	{"*52*"	,	4,	40,		X_HOTLINE_ACTIVATE,		0,	1,	"Set Hotline"},
	{"#52#"	,	4,	4,		X_HOTLINE_DEACTIVATE,	0,	0,	"Cancel Hotline"},


	{"*55*"	,	4,	9,		X_ALARM_ACTIVATE,		0,	1,	"Set Alarm"},
	{"#55#"	,	4,	9,		X_ALARM_DEACTIVATE,		0,	0,	"Disable Alarm"},

	
	{"*50#"	,	4,	4,		X_ABSENT_ACTIVATE,		0,	0,	"Enable Interception service"},
	{"#50#"	,	4,	4,		X_ABSENT_DEACTIVATE,	0,	0,	"Disable Interception service"}

};


/*______________________________________________________________________________
**	rcm_checkPrefixKey()
**
**	descriptions: 
**
**      E8C use feature key to inform sip server to on/off features.
**
**      1. Search prefixrange(feature) in table.
**		2. Set relative flag and 
**      3. call function to generate *src_ptr if feature key matched.
**
**  output: *src_ptr [Will be set if matching feature key, otherwise empty]
**
**	return: 
**      0: Do NOT call to sip server
**      1: call out to sip server
**____________________________________________________________________________*/
int rcm_checkPrefixKey( LinphoneCore *lc, SIGNSTATE s_sign,guint32 ssid, char *src_ptr)
{
	int i;
	int ret = 0;
	int input_len=0;
	char tempstr[50];
	char tempsrc[50];
	prefixinfo_t *prefix_p = &prefixinfolist[i];

//	printf("checkPrefixKey , lc->dial_date is %s\n",lc->dial_data);
//	printf("checkPrefixKey , lc->dial_index is %d\n",lc->dial_data_index);
	
	for( i=0; i<PrefixlistEntries; i++ ) {
		prefix_p=&prefixinfolist[i];

		if(prefix_p==NULL){
			// never happen
			printf("feature table null\n");
			return 0;
		}
			
		/* check input data length */
		if(lc->dial_data_index >= prefix_p->PrefixMinNumberOfDigits) {

			/* compare dial_data with Prefixrange */

			/* check dial digits match feature key in Prefixtable */
			if(strncmp(lc->dial_data, prefix_p->Prefixrange, prefix_p->PrefixMinNumberOfDigits)==0) {

				/* match prefix */

				if(( lc->dial_data_index == prefix_p->PrefixMinNumberOfDigits ) &&
                   ( lc->dial_data_index == prefix_p->PrefixMaxNumberOfDigits ) ){

					/* dial_len = min_prefix = max_prefix */
					/* example: match *56* */
					/* Enable/Disable a specific feature */

					printf("match the prefixrange maxnumber type 1 , must dial out to sip server or set call feature\n");

					/* Modify linphone flag, record phone feature for oSIP usage. */
					rcm_LinphoneSyslog(LOG_CALL_MGNT,lc,prefix_p->featurename);

					/* Set Feature Flags */
					if( DoSetFeatureByNumberingPlan(lc, prefix_p) ) {
						/* Generate *src_ptr (input) for linphone command parse */
						DoCallxxxByNumberingPlan(lc, ssid, src_ptr, prefix_p);
						return 1;
					}
					return 0;
					
				}else if((lc->dial_data_index <= prefix_p->PrefixMaxNumberOfDigits)
					&& (lc->dial_data_index > prefix_p->PrefixMinNumberOfDigits)){

					/* example: user dial *62 + phonenumber + #  , # is end digit */

					printf(" in prefix min <--> max range");

					if((lc->dial_data[lc->dial_data_index -1]=='#') || (s_sign==SIGN_AUTODIAL)){

						printf("match the prefixrange maxnumber type 2 , must dial out to sip server \n");
						rcm_LinphoneSyslog(LOG_CALL_MGNT,lc,prefix_p->featurename);

						if(DoSetFeatureByNumberingPlan(lc,prefix_p)){
							/* modify src_ptr (input), for linphone command parse */
							DoCallxxxByNumberingPlan(lc,ssid,src_ptr,prefix_p);
							return 1;
						}
						return 0;
					}	
				} else {
					/* do nothing */
				}

			} /* strncmp() match prefix */
		} /* if() */
	} /* for() */

	return 0;

}

/*______________________________________________________________________________
**	DoCallxxxByNumberingPlan()
**
**	descriptions: 
**      Remove leading feature key and ending pound key if has.
**		Generate dial out command of *src_ptr, for example, "call 1101"
**		Channge SS state to SS_STATE_CALLOUT.
**
**  output: *src_ptr
**	return: N/A
**____________________________________________________________________________*/

static void DoCallxxxByNumberingPlan ( 
	LinphoneCore *lc, 
 	guint32 ssid, 
    char *src_ptr,
 	prefixinfo_t *featurekey_p )
{
	int ret = 0;
	char tempstr[50];
	int input_len=0;
	

	input_len = lc->dial_data_index;

	//printf("\r\n input is %d\n",input_len);
	strncpy(tempstr, lc->dial_data, lc->dial_data_index);

	/* Remove leading feature key */
	/* Most leading feature key will be sent to sip proxy server. */
	/* Because most features are implementing on sip server */
	/* User use feature key to inform sip server to on/off features. */
	if(featurekey_p->remove_feature_key == 1) {

		strncpy(tempstr, lc->dial_data + featurekey_p->PrefixMinNumberOfDigits,
				lc->dial_data_index - featurekey_p->PrefixMinNumberOfDigits);	
				
		input_len -= featurekey_p->PrefixMinNumberOfDigits;
	}
					
	// FIXME: Refine

	/* remove ending pound key, check tempstr [input_len-1] */
	if( featurekey_p->remove_pound_key == 1) {

		if( tempstr[input_len-1] == '#' ) {
			strcpy(src_ptr, "call ");
			input_len--;
			strncat(src_ptr, tempstr, input_len);
		} else {
			/* user does NOT dial ending # key. */
			strcpy(src_ptr, "call ");
			strncat(src_ptr, tempstr, input_len);
		}

	} else {
		strcpy(src_ptr, "call ");
		strncat(src_ptr, tempstr, input_len);
	}

	/* src_ptr may */
	// Fixme: Why Set SS in this function? Should set outside this function?
	SetSessionState(lc->chid, ssid, SS_STATE_CALLOUT);

}

int DoSetFeatureByNumberingPlan( LinphoneCore *lc, 
								 prefixinfo_t *featurekey_p )
{
	int ret = 1;

	
	switch( featurekey_p->FacilityAction ) {
		case CA_ACTIVATE:
			printf("\r\n DoSetFeatureByNumberingPlan , set FXS_ONE_SHOT_CID_DISPLAY\n");
			lc->fxsfeatureFlags |= FXS_ONE_SHOT_CID_DISPLAY;
			break;

		case X_CA_DEACTIVATE:
			printf("\r\n DoSetFeatureByNumberingPlan , set FXS_ONE_SHOT_CID_HIDDEN\n");
			lc->fxsfeatureFlags |= FXS_ONE_SHOT_CID_HIDDEN;
			break;

		case X_MCID_ACTIVATE:
			printf("\r\n DoSetFeatureByNumberingPlan , set MCID\n");
			lc->fxsfeatureFlags |= FXS_MCID_ACTIVATE;
			ret=0; //special case, do NOT direct dial to sip server.
			break;		

		case CW_ACTIVATE:
			printf("\r\n enable call waiting\n");
			lc->sip_conf.call_waiting_enable =1;

			ret=0; //do not dial to sip serve 
			break;

		case CW_DEACTIVATE:
			printf("\r\n disable call waiting\n");
			lc->sip_conf.call_waiting_enable =0;

			ret=0; //do not dial to sip serve 
			break;
			
		default:
			break;
	}

	return ret;
}


int rcm_checkInternalPhoneBook( LinphoneCore *lc, SIGNSTATE s_sign,guint32 ssid, char *src_ptr)
{

	g_message("\r\n check rcm_checkInternalPhoneBook 001 ******\n");

	if(RTK_VOIP_SLIC_NUM(g_VoIP_Feature)<2){
		//only one fxs
		return 0;
	}
	//printf("lc->dial_code is %s\n",lc->dial_code);
	if (strcmp(lc->dial_code,LC_FUNCKEY_FXS_0) == 0)
		return 1;
	else if (strcmp(lc->dial_code,LC_FUNCKEY_FXS_1) == 0)
		return 1;
	else 
		return 0;

}


