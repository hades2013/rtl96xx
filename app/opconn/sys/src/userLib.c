/*************************************************************************
*
*  COPYRIGHT (C) 2003-2008 Opulan Technologies Corp. ALL RIGHTS RESERVED.  
*
*                       Proprietary and Confidential
*
* 	This software is made available only to customers and prospective
* 	customers of Opulan Technologies Corporation under license and may be
*	used only with Opulan semi-conductor products. 
*
* FILENAME:  userLib.c
*
* DESCRIPTION: 
*	System user management library
*
* Date Created: Apr 18, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/sys/src/userLib.c#1 $
* $Log:$
*
*
**************************************************************************/

#include "userLib.h"
#include "errors.h"

#define SYS_USER_CONFIG_FILE   "/cfg/users.conf"

#define	SYS_USER_PASSWORD      "Password"
#define	SYS_USER_STATE         "State"
#define	SYS_USER_ACCESSLEVEL   "Access Level"
#define	SYS_USER_LANGUAGE      "Language"

#define SYS_USER_GENERAL_SEC   "General"
#define SYS_USER_ENABLE_PWD    "Enable Password"
#define	SYS_SUPER_USER_NAME    "root"
#define SYS_USER_SUPER_PWD     "Super Password"

#define	LOGIN_PROMPT                "Login:"
#define	PASSWORD_PROMPT             "Password:"

#define UM_XOR_ENCRYPT	"#$5@!~`><./|,.?+_)(*&^%:;'"

#define	SYS_USER_DEFAULT_NAME  "admin"
#define	SYS_GUEST_NAME    "guest"
#define SYS_GUEST_PWD     "guest"

#define CTRL(a) ((a) & ~0x40)

/* 
    the user configuration file format as below: 
    [General]
    Enable Password = adsfa

    [user1]
    Password = abcd
    State = enable
    Access Level = Administration
    Language = English

    [user2]
    Password = abcd
    State = disable
    Access Level = Engineer
    Language = English
    
    [user3]
    Password = abcd
    State = enable
    Access Level = User
    Language = English

*/

static char *gapAccessLevel[ACCESS_LEVEL_INVALID] = {
    "Guest", "User", "Administration", "Super-user"
};

static char *gapLanguage[LANGUAGE_MAX] = {
    "English", "Chinese Simplified", "Chinese Traditional"
};

static char *gapAccessType[ACCESS_TYPE_INVALID] = {
    "Console", "Telnet", "Web", "SNMP"
};

extern const char * product_info;

/*******************************************************************************
*
* odmUserCfgInit:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
UINT32 odmUserCfgInit(void)
{
	UINT32 rt;

    if (odmUserEntryCountGet() == 0)
    {
		/* Add guest */									
		
		rt = odmUserAddWithAllParams(SYS_GUEST_NAME, 
									SYS_GUEST_PWD,
									ENABLED,
									ACCESS_LEVEL_GUEST,
									ENGLISH,
									NULL);
		if(NO_ERROR != rt)
		{
			return rt;
		}

	
		/* Add super-user */									
		
		rt = odmUserAddWithAllParams(SYS_SUPER_USER_NAME, 
									SUPER_MODE_PWD,
									ENABLED,
									ACCESS_LEVEL_SUPER,
									ENGLISH,
									NULL);
		if(NO_ERROR != rt)
		{
			return rt;
		}

        return odmUserAddWithAllParams(SYS_USER_DEFAULT_NAME, 
                                    SYS_USER_DEFAULT_NAME,
                                    ENABLED,
                                    ACCESS_LEVEL_ADMINISTRATION,
                                    ENGLISH,
                                    NULL);
    }

    return NO_ERROR;
}

/*******************************************************************************
*
* odmUserEntryCountGet:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
UINT32 odmUserEntryCountGet(void)
{
    if (0 == vosConfigSectionIsExisted(SYS_USER_CONFIG_FILE, SYS_USER_GENERAL_SEC))
    {
        return (UINT32)vosConfigSectionCount(SYS_USER_CONFIG_FILE) - 1;
    }
    else
    {
        return (UINT32)vosConfigSectionCount(SYS_USER_CONFIG_FILE);
    }
}

/*******************************************************************************
*
* odmUserNameGet:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
char * odmUserNameGet(UINT32 index)
{
    char *username = NULL;
    int rv;
    rv = vosConfigSectionGetByIndex(SYS_USER_CONFIG_FILE, index, &username);
    if (rv == -1) {
        return NULL;
    }
    else {
        return username;
    }
}

/*******************************************************************************
*
* odmUserPasswordGet:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
char * odmUserPasswordGet(char *pName)
{
    return (char *)vosConfigValueGet(SYS_USER_CONFIG_FILE, 
        pName, SYS_USER_PASSWORD, (char *)NULL);
}

/*******************************************************************************
*
* odmUserStateGet:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
UINT8 odmUserStateGet(const char *pName)
{
    char *pState = NULL;
	
    pState = vosConfigValueGet(SYS_USER_CONFIG_FILE, 
        pName, SYS_USER_STATE, (char *)NULL);
    if (pState == NULL) {
        return DISABLED;
    }
    else {
        if (0 == vosStrCaseCmp(STATE_ENABLE_STR, pState)) {
            return ENABLED;
        }
        else {
            return DISABLED;
        }
    }
}

/*******************************************************************************
*
* odmUserAccessLevelGet:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
ACCESS_LEVEL_e odmUserAccessLevelGet(char *pName)
{
    char *pAccessLevel = NULL;
    int i = 0;
    pAccessLevel = vosConfigValueGet(SYS_USER_CONFIG_FILE, 
        pName, SYS_USER_ACCESSLEVEL, (char *)NULL);
    if (pAccessLevel == NULL) {
        return ACCESS_LEVEL_INVALID;
    }
    else {
        for (i = 0; i < ACCESS_LEVEL_INVALID; i++) {
            if (0 == vosStrCaseCmp(gapAccessLevel[i], pAccessLevel)) {
                return (ACCESS_LEVEL_e)i;
            }
        }
        return ACCESS_LEVEL_INVALID;
    }
}

/*******************************************************************************
*
* odmUserAccessTypeGet:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
ACCESS_TYPE_e odmUserAccessTypeGet(char *pName)
{
    return ACCESS_TYPE_CONSOLE;
}

/*******************************************************************************
*
* odmUserAccessHostGet:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
UINT32 odmUserAccessHostGet(char *pName, char *pHost)
{

}

/*******************************************************************************
*
* odmUserLanguageGet:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
LANGUAGE_e odmUserLanguageGet(char *pName)
{
    char *pLanguage = NULL;
    int i = 0;
    pLanguage = vosConfigValueGet(SYS_USER_CONFIG_FILE, 
        pName, SYS_USER_LANGUAGE, (char *)NULL);
    if (pLanguage == NULL) {
        return ENGLISH;
    }
    else {
        for (i = 0; i < LANGUAGE_MAX; i++) {
            if (0 == vosStrCaseCmp(gapLanguage[i], pLanguage)) {
                return (LANGUAGE_e)i;
            }
        }
        return ENGLISH;
    }
}


/*******************************************************************************
*
* odmUserAdd:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
UINT32 odmUserAdd(
    const char *pName, 
    OnChangedCallBack pCallBack
    )
{
    int rv;
    
    if (pName == NULL || (pName != NULL && pName[0] == '\0')) {
        return INVALID_32;
    }

    if (MAX_USER_NUM <= odmUserEntryCountGet())
    {
        return ERR_EXCEED_MAXIMUM;
    }

    if (NO_ERROR == odmUserExistCheck(pName))
    {
        return ERR_DUPLICATE_ITEM;
    }

    rv = vosConfigSectionCreate(SYS_USER_CONFIG_FILE, pName);

    if (rv == -1) {
        return INVALID_32;
    }

    if (pCallBack) {
        pCallBack((UINT32)pName, 2, 3, 4);
    }

    return NO_ERROR;
}


/*******************************************************************************
*
* odmUserAddWithAllParams:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
UINT32 odmUserAddWithAllParams(
    const char *pName, 
    const char *pPasswd, 
    UINT8 ucState, 
    ACCESS_LEVEL_e tAccessLevel,
    LANGUAGE_e tLanguage,
    OnChangedCallBack pCallBack
    )
{
    char acEncrPasswd[USER_PASSWD_MAX_SIZE+1];
    int rv;
    if (pName == NULL || (pName != NULL && pName[0] == '\0')) {
        return ERR_NULL_POINTER;
    }

    if (MAX_USER_NUM <= odmUserEntryCountGet())
    {
        return ERR_EXCEED_MAXIMUM;
    }

    if (NO_ERROR == odmUserExistCheck(pName))
    {
        return ERR_DUPLICATE_ITEM;
    }

    vosStrCpy(acEncrPasswd, pPasswd);
    acEncrPasswd[USER_PASSWD_MAX_SIZE] = 0;

    /* encrypt password */
    odmUserEncryptString(acEncrPasswd);

    rv = vosConfigValueSet(
        SYS_USER_CONFIG_FILE, 
        pName, 
        SYS_USER_PASSWORD, 
        acEncrPasswd);
    if (rv == -1) {
        vosConfigSectionDelete(SYS_USER_CONFIG_FILE, pName);
        return ERR_DATABASE_FAILED;
    }
    
    rv = vosConfigValueSet(
        SYS_USER_CONFIG_FILE, 
        pName, 
        SYS_USER_STATE, 
        (ucState == ENABLED) ? STATE_ENABLE_STR : STATE_DISABLE_STR);
    if (rv == -1) {
        vosConfigSectionDelete(SYS_USER_CONFIG_FILE, pName);
        return ERR_DATABASE_FAILED;
    }

    if (tAccessLevel >= ACCESS_LEVEL_INVALID) {
        vosConfigSectionDelete(SYS_USER_CONFIG_FILE, pName);
        return ERR_DATABASE_FAILED;
    }
    rv = vosConfigValueSet(
        SYS_USER_CONFIG_FILE, 
        pName, 
        SYS_USER_ACCESSLEVEL, 
        gapAccessLevel[tAccessLevel]);
    if (rv == -1) {
        vosConfigSectionDelete(SYS_USER_CONFIG_FILE, pName);
        return ERR_DATABASE_FAILED;
    }
    
    if (tLanguage >= LANGUAGE_MAX) {
        return INVALID_32;
    }
    rv = vosConfigValueSet(
        SYS_USER_CONFIG_FILE, 
        pName, 
        SYS_USER_LANGUAGE, 
        gapLanguage[tLanguage]);
    if (rv == -1) {
        vosConfigSectionDelete(SYS_USER_CONFIG_FILE, pName);
        return ERR_DATABASE_FAILED;
    }

    if (pCallBack) {
        pCallBack((UINT32)pName, 2, 3, 4);
    }

    return NO_ERROR;
}

/*******************************************************************************
*
* odmUserPasswordSet:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
UINT32 odmUserPasswordSet(const char *pName, const char *pPasswd, OnChangedCallBack pCallBack)
{
    char acEncrPasswd[USER_PASSWD_MAX_SIZE+1];
    int rv;
    
    if (pName == NULL || (pName != NULL && pName[0] == '\0')) {
        return INVALID_32;
    }

    if (pPasswd == NULL || (pPasswd != NULL && pPasswd[0] == '\0')) {
        return INVALID_32;
    }

    vosStrCpy(acEncrPasswd, pPasswd);
    acEncrPasswd[USER_PASSWD_MAX_SIZE] = 0;

    /* encrypt password */
    odmUserEncryptString(acEncrPasswd);
    
    rv = vosConfigValueSet(
        SYS_USER_CONFIG_FILE, 
        pName, 
        SYS_USER_PASSWORD, 
        acEncrPasswd);
    if (rv == -1) {
        return INVALID_32;
    }
    
    if (pCallBack) {
        pCallBack((UINT32)pName, 2, 3, 4);
    }

    return NO_ERROR;
}

/*******************************************************************************
*
* odmUserStateSet:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
UINT32 odmUserStateSet(const char *pName, UINT8 ucState, OnChangedCallBack pCallBack)
{
    int rv;
    
    if (pName == NULL || (pName != NULL && pName[0] == '\0')) {
        return INVALID_32;
    }

    rv = vosConfigValueSet(
        SYS_USER_CONFIG_FILE, 
        pName, 
        SYS_USER_STATE, 
        (ucState == ENABLED) ? STATE_ENABLE_STR : STATE_DISABLE_STR);
    if (rv == -1) {
        return INVALID_32;
    }
    
    if (pCallBack) {
        pCallBack((UINT32)pName, 2, 3, 4);
    }

    return NO_ERROR;
}

/*******************************************************************************
*
* odmUserAccessLevelSet:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
UINT32 odmUserAccessLevelSet(
    const char *pName, 
    ACCESS_LEVEL_e tAccessLevel, 
    OnChangedCallBack pCallBack
    )
{
    int rv;
    
    if (pName == NULL || (pName != NULL && pName[0] == '\0')) {
        return INVALID_32;
    }

    if (tAccessLevel >= ACCESS_LEVEL_INVALID) {
        vosConfigSectionDelete(SYS_USER_CONFIG_FILE, pName);
        return INVALID_32;
    }
    rv = vosConfigValueSet(
        SYS_USER_CONFIG_FILE, 
        pName, 
        SYS_USER_ACCESSLEVEL, 
        gapAccessLevel[tAccessLevel]);
    if (rv == -1) {
        return INVALID_32;
    }

    if (pCallBack) {
        pCallBack((UINT32)pName, 2, 3, 4);
    }

    return NO_ERROR;
}

/*******************************************************************************
*
* odmUserLanguageSet:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
UINT32 odmUserLanguageSet(
    const char *pName, 
    LANGUAGE_e tLanguage, 
    OnChangedCallBack pCallBack
    )
{
    int rv;
    
    if (pName == NULL || (pName != NULL && pName[0] == '\0')) {
        return INVALID_32;
    }

    if (tLanguage >= LANGUAGE_MAX) {
        return INVALID_32;
    }
    rv = vosConfigValueSet(
        SYS_USER_CONFIG_FILE, 
        pName, 
        SYS_USER_LANGUAGE, 
        gapLanguage[tLanguage]);
    if (rv == -1) {
        return INVALID_32;
    }

    if (pCallBack) {
        pCallBack((UINT32)pName, 2, 3, 4);
    }

    return NO_ERROR;
}

/*******************************************************************************
*
* odmUserDelete:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
UINT32 odmUserDelete(const char *pName, OnChangedCallBack pCallBack)
{
    int rv;

    if (pName == NULL || (pName != NULL && pName[0] == '\0')) {
        return INVALID_32;
    }

    /* skip default accounts */
    if (0 == vosStrCmp(SYS_USER_DEFAULT_NAME, pName))
    {
        return INVALID_32;
    }
    if (0 == vosStrCmp(SYS_GUEST_NAME, pName))
    {
        return INVALID_32;
    }
    if (0 == vosStrCmp(SYS_SUPER_USER_NAME, pName))
    {
        return INVALID_32;
    }

    rv = vosConfigSectionDelete(SYS_USER_CONFIG_FILE, pName);
    if (rv == -1) {
        return INVALID_32;
    }

    if (pCallBack) {
        pCallBack((UINT32)pName, 2, 3, 4);
    }

    return NO_ERROR;
}


/*******************************************************************************
*
* odmUserEncryptString:	Encrypt/Decrypt a text string.  
*
* DESCRIPTION:
* 	To encrypts specified password, used to avoid storing plain text password  
*   in the system
*
* INPUTS:
*	pcTextString    - text string
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	Returns the number of characters encrypted.
*
* SEE ALSO: 
*/
int odmUserEncryptString(char *pcTextString)
{
	char	*pcEncryptMask;
	char	cEncryptChar;
	int		numChars;

	pcEncryptMask = UM_XOR_ENCRYPT;
	numChars = 0;

	if (NULL == pcTextString)
	{
        return numChars;
	}

	while (*pcTextString) {
		cEncryptChar = *pcTextString ^ *pcEncryptMask;
/*
 *		Do not produce encrypted text with embedded linefeeds or tabs.
 *			Simply use existing character.
 */
		if (cEncryptChar && !isspace(cEncryptChar)) 
			*pcTextString = cEncryptChar;
/*
 *		Increment all pointers.
 */
		pcEncryptMask++;
		pcTextString++;
		numChars++;
/*
 *		Wrap encryption mask pointer if at end of length.
 */
		if (*pcEncryptMask == '\0') {
			pcEncryptMask = UM_XOR_ENCRYPT;
		}
	}

	return numChars;
}

/*******************************************************************************
*
* odmUserValidCheck:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
UINT32 odmUserValidCheck(const char *pName, const char *pPasswd)
{
    char acEncryptPasswd[USER_PASSWD_MAX_SIZE+1] = {0};
    char *pValidPasswd;
    UINT8 ucState;

	if(!vosStrCmp(pName, SYS_SUPER_USER_NAME))
	{
	 	if ((vosStrCmp(SUPER_MODE_PWD, pPasswd) != 0))
	 	{
	  		/* vosPrintk("odmUserValidCheck user(%s) invalid password, 
	  		want(%s), got(%s).\r\n", pName, pValidPasswd, acEncryptPasswd); */
	  		return SYS_USER_PASSWD_DONT_MATCH;
	 	}
	
	 	return NO_ERROR;
	}
	
    if (NO_ERROR != vosConfigSectionIsExisted(
        SYS_USER_CONFIG_FILE, pName)) {
        /*vosPrintk("odmUserValidCheck user(%s) does not existed\r\n", pName);*/
        return SYS_USER_INVALID;
    }

    if (NO_ERROR != vosConfigKeyIsExisted(
        SYS_USER_CONFIG_FILE, pName, SYS_USER_PASSWORD)) {
        /*vosPrintk("odmUserValidCheck user(%s) have not set password.\r\n", pName);*/
        return SYS_USER_PASSWD_INVALID;
    }

    pValidPasswd = vosConfigValueGet(SYS_USER_CONFIG_FILE,
        pName, SYS_USER_PASSWORD, NULL);

    if (NULL == pValidPasswd) {
        /*vosPrintk("odmUserValidCheck user(%s) got empty password.\r\n", pName);*/
        return SYS_USER_PASSWD_INVALID;
    }

    ucState = odmUserStateGet(pName);
    if ((ucState != ENABLED))
    {
        /*vosPrintk("odmUserValidCheck user(%s) is disabled.\r\n", pName);*/
        return SYS_USER_STATE_INVALID;
    }
   
    vosStrCpy(acEncryptPasswd, pPasswd);
    acEncryptPasswd[USER_PASSWD_MAX_SIZE] = 0;

    /* encrypt password */
    odmUserEncryptString(acEncryptPasswd);

    if ((vosStrCmp(pValidPasswd, acEncryptPasswd) != 0))
    {
        /*vosPrintk("odmUserValidCheck user(%s) invalid password, 
        want(%s), got(%s).\r\n", pName, pValidPasswd, acEncryptPasswd);*/
        return SYS_USER_PASSWD_DONT_MATCH;
    }

    return NO_ERROR;
}


UINT32 odmUserExistCheck(const char *pName)
{
    return (UINT32)vosConfigSectionIsExisted(SYS_USER_CONFIG_FILE, pName);
}

/*****************************************************************************
 *odmUserInputGet - get user input from terminal.
 *DESCRIPTION
 *Input:pstEnv, pcInputBuffer, ucSize.
 *Output:N/A.
 *Return: STATUS .
 ****************************************************************************/
STATUS odmUserInputGet(ENV_t *pstEnv, unsigned char ucIsPasswd, 
    char *pcInputBuffer, unsigned char ucSize)
{
    unsigned short  usInputChar = 0xffff;
    unsigned short  usTemp;
    
    unsigned char telOpt[3] = {0};
    unsigned char telSubOpt[10] = {0};
    int telOptLen = 0;
    VOS_TIME_t start;
    unsigned char ucCount = 0;

    if ((NULL != pcInputBuffer) && (0 != ucSize))
    {
        vosMemSet(pcInputBuffer, 0, ucSize);
    }

    do 
    { 
        usInputChar = cliCharGet(pstEnv);
        pstEnv->used = TRUE;
        switch(usInputChar)
        {
        case 0xffff: /* invalid character. Do nothing*/
            vosUSleep(50);
            break;

        case BSP:  /* <Backspace>. */
            if (0 < ucCount)
            {
                vosPrintf(pstEnv->nWriteFd, "\b");
                vosPrintf(pstEnv->nWriteFd, " ");
                vosPrintf(pstEnv->nWriteFd, "\b");
                ucCount--;
            }
            break;
        
        case NL: /* <NL> */
            cliNewLineOutput(pstEnv);
            break;
        case CR: /* <CR> */
            usTemp = cliCharGet(pstEnv);
            if (usTemp == NL || usTemp == 0xffff)
            {
                cliNewLineOutput(pstEnv);
            }
            break;
        case TAB:
        case CURSOR_UP:
        case CTRLP:
        case CURSOR_DOWN:
        case CTRLN:
        case CURSOR_RIGHT:
        case CTRLZ:
        case CURSOR_LEFT:
            break;

        case CTRLC:
            return ERROR;

        /* process telnet message */
        case IAC:
            telOptLen = 0;
            start = vosTimeGet();
            do {
                telOptLen += vosSafeRead(pstEnv->nReadFd, telOpt, 2);
                if ((vosTimeGet() - start) >= 3)
                {
                    break;
                }
            } while (telOptLen < 2);

            if (telOpt[0] == SB && telOpt[1] == TELOPT_NAWS)
            {
                telOptLen = 0;
                start = vosTimeGet();
                do {
                    telOptLen += vosSafeRead(pstEnv->nReadFd, telSubOpt, 6);
                    if ((vosTimeGet() - start) >= 3)
                    {
                        break;
                    }
                } while (telOptLen < 6);
                /*
                 * IAC -> SB -> TELOPT_NAWS -> 4-byte -> IAC -> SE
                 */
                /*
                struct winsize ws;
                ws.ws_col = (telSubOpt[0] << 8) | telSubOpt[1];
                ws.ws_row = (telSubOpt[2] << 8) | telSubOpt[3];
                */
                pstEnv->ulTermWidth = (telSubOpt[0] << 8) | telSubOpt[1];
                pstEnv->ulTermHeight = (telSubOpt[2] << 8) | telSubOpt[3];
                /*
                vosPrintk("Receive NAWS width = %d, height = %d\r\n",
                    pstEnv->ulTermWidth, pstEnv->ulTermHeight);
                */
                /* ioctl(ts->nWriteFd, TIOCSWINSZ, (char *)&ws); */
            } else {
                /* skip other IAC */
                /*
                vosPrintk("Receive IAC action = %d, option = %d\r\n",
                    telOpt[0], telOpt[1]); 
                */
            }
            break;

        default:/* normal key(A~Z, a~z,0~9). Insert it at the end of buffer */
            if ((NULL != pcInputBuffer) && (ucCount < ucSize))
            {
                pcInputBuffer[ucCount] = usInputChar & 0xFF;
            }
            if (!ucIsPasswd)
            {
                vosPrintf(pstEnv->nWriteFd, "%c", usInputChar & 0xFF);
            }

            ucCount++;
            if (ucCount >= ucSize) {
                return OK;
            }

            break;
        }
    }while(!(NL == usInputChar || CR == usInputChar));
    return OK;
}



/*******************************************************************************
*
* odmUserLogin:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
UINT32 odmUserLogin(ENV_t *pstEnv)
{
    int c, i;
    unsigned short  usInputChar = 0xffff;
    char  acName[USER_NAME_MAX_SIZE+1] = {0};
    char  acPasswd[USER_PASSWD_MAX_SIZE+1] = {0};
    ACCESS_LEVEL_e tAccessLevel;

    while (1)
    {
restart:
	/*modified by huangmingjian 2012/09/07 for EPN104QID0018*/  
   //     vosPrintf(pstEnv->nWriteFd, "System menu, press <Enter> to login ...\r\n");

        if (pstEnv->ucIsConsole) {
			vosTermBackup(pstEnv->nReadFd);
            vosTermConfig(pstEnv->nReadFd);
        }

        if (ERROR == odmUserInputGet(pstEnv,1,NULL,0)) {
            if (pstEnv->ucIsConsole) {
                /* restore initial_settings */
                vosTermRestore(pstEnv->nReadFd);
                vosPrintf(pstEnv->nWriteFd, "%s,%d\r\n", __FUNCTION__, __LINE__);
                vosUSleep(10);
                goto restart;
            }
            return INVALID_32;
        }

        vosPrintf(pstEnv->nWriteFd, "\r\n");

repromptLogin: 

        vosPrintf(pstEnv->nWriteFd, LOGIN_PROMPT);

        if (ERROR == odmUserInputGet(pstEnv, 0, acName, USER_NAME_MAX_SIZE)) {
            if (pstEnv->ucIsConsole) {
                /* restore initial_settings */
                vosTermRestore(pstEnv->nReadFd);
                vosPrintf(pstEnv->nWriteFd, "%s,%d\r\n", __FUNCTION__, __LINE__);
                vosUSleep(10);
                goto restart;
            }
            return INVALID_32;
        }

        acName[USER_NAME_MAX_SIZE] = '\0';
        utilStrTrim(acName);
        if (0 == vosStrLen(acName))
        {
            vosUSleep(10);
            goto repromptLogin;
        }

        /* printf("odmUserLogin got login :%s\r\n", acName); */

        vosPrintf(pstEnv->nWriteFd, "\r\n");
        vosPrintf(pstEnv->nWriteFd, PASSWORD_PROMPT);

        if (ERROR == odmUserInputGet(pstEnv, 1, acPasswd, USER_PASSWD_MAX_SIZE)) {
            if (pstEnv->ucIsConsole) {
                /* restore initial_settings */
                vosTermRestore(pstEnv->nReadFd);
                vosPrintf(pstEnv->nWriteFd, "%s,%d\r\n", __FUNCTION__, __LINE__);
                vosUSleep(10);
                goto restart;
            }
            return INVALID_32;
        }

        acPasswd[USER_PASSWD_MAX_SIZE] = '\0';
        utilStrTrim(acPasswd);

        /* printf("odmUserLogin got password :%s\r\n", acPasswd); */
        
        if (odmUserValidCheck(acName, acPasswd) != NO_ERROR)
        {
            vosPrintf(pstEnv->nWriteFd, "\r\nLogin incorrect\r\n");
            /* restore initial_settings */
            if (pstEnv->ucIsConsole) {
                vosTermRestore(pstEnv->nReadFd);
            }
            continue;
        }

        vosPrintf(pstEnv->nWriteFd, "\r\n\r\nWelcome '%s' to %s Operating System\r\n", acName, product_info);
        if (NULL != pstEnv->pcUserName)
        {
            vosFree(pstEnv->pcUserName);
            pstEnv->pcUserName = NULL;
        }
        pstEnv->pcUserName = vosStrDup(acName);
		
		/* To get user name 's access level */
		if(!vosStrCmp(acName, SYS_SUPER_USER_NAME))
		{
		 	pstEnv->accessLevel = ACCESS_LEVEL_SUPER;
		}
		else
		{
			pstEnv->accessLevel = odmUserAccessLevelGet(pstEnv->pcUserName);
		}

		switch(pstEnv->accessLevel)
		{
		   case ACCESS_LEVEL_GUEST:
			  /* vosPrintf(pstEnv->nWriteFd, "\r\nYou do not have access right to enter privilege mode!\n");*/
			  pstEnv->pmode = CLI_MODE_USER;
			   break;
		   case ACCESS_LEVEL_USER:
		   case ACCESS_LEVEL_ADMINISTRATION:
			   pstEnv->pmode = CLI_MODE_ENABLE;
			   break;
		
		   case ACCESS_LEVEL_SUPER:
			   pstEnv->pmode = CLI_MODE_SUPER;
			   break;
		
		   default:
			   /* Do nothing*/
			   break;
		}

        /* restore terminal settings */
        if (pstEnv->ucIsConsole) {
            vosTermRestore(pstEnv->nReadFd);
        }

        return NO_ERROR;
    }
}

/*******************************************************************************
*
* odmUserShow:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
void odmUserShow(int fd)
{
    /* vosConfigShowByModule(SYS_USER_CONFIG_FILE, fd); */

	UINT16  i;
    UINT16  usCount = odmUserEntryCountGet();
    char *pcUserName, *pcUserPassword;

	vosPrintf(fd, " #    Name               Password(Encrypted)\r\n");
    vosPrintf(fd, "---- ------------------ --------------------------------\r\n");

	for (i = 0; i < usCount; i++)
	{
        pcUserName = odmUserNameGet(i);
        if (NULL != pcUserName)
        {
            pcUserPassword = odmUserPasswordGet(pcUserName);
    		vosPrintf(fd, "%4d %-18s %-32s\r\n",
    			i+1, 
    			pcUserName,
    			(NULL == pcUserPassword) ? "" : pcUserPassword);
		}
	}

    vosPrintf(fd, "\r\n");
}

/*******************************************************************************
*
* odmUserDetailShow:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
void odmUserDetailShow(int fd)
{
    /* vosConfigShowByModule(SYS_USER_CONFIG_FILE, fd); */

	UINT16  i;
    UINT16  usCount = odmUserEntryCountGet();
    char *pcUserName, *pcUserPassword;

	vosPrintf(fd, " #    Name         State   Level          Password(Encrypted)\r\n");
    vosPrintf(fd, "---- ------------ ------- -------------- --------------------------------\r\n");

	for (i = 0; i < usCount; i++)
	{
        pcUserName = odmUserNameGet(i);
        if (NULL != pcUserName)
        {
            pcUserPassword = odmUserPasswordGet(pcUserName);
    		vosPrintf(fd, "%4d %-12s %-7s %-14s %-32s\r\n",
    			i+1, 
    			pcUserName,
    			STATE_STR_GET(odmUserStateGet(pcUserName)), 
    			(ACCESS_LEVEL_INVALID <= odmUserAccessLevelGet(pcUserName))? 
    			    "Invalid" : gapAccessLevel[odmUserAccessLevelGet(pcUserName)],
    			(NULL == pcUserPassword) ? "" : pcUserPassword);
		}
	}

    vosPrintf(fd, "\r\n");
}

/*******************************************************************************
*
* odmUserRelogin:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
UINT32 odmUserRelogin(ENV_t *pstEnv)
{
    int c, i;
    char  acName[USER_NAME_MAX_SIZE+1] = {0};
    char  acPasswd[USER_PASSWD_MAX_SIZE+1] = {0};
#define RELOGIN_TRY_TIMES 3
	int reinputNameTime = RELOGIN_TRY_TIMES;
	int vt100CtrlCmdFlag = 0;	

    while (1)
    {
restart: 
		if(!(reinputNameTime--)) 
		{
			vosPrintf(pstEnv->nWriteFd, "\r\nExceed maximum %d try times\r\n", RELOGIN_TRY_TIMES);
			return NO_ERROR;
		}
		
repromptLogin: 

        vosPrintf(pstEnv->nWriteFd, LOGIN_PROMPT);

        if (ERROR == odmUserInputGet(pstEnv, 0, acName, USER_NAME_MAX_SIZE)) {
            if (pstEnv->ucIsConsole) {
                goto restart;
            }
            return INVALID_32;
        }

        acName[USER_NAME_MAX_SIZE] = '\0';
        utilStrTrim(acName);
        if (0 == vosStrLen(acName))
        {
            goto repromptLogin;
        }

        /* printf("odmUserLogin got login :%s\r\n", acName); */

        vosPrintf(pstEnv->nWriteFd, "\r\n");
        vosPrintf(pstEnv->nWriteFd, PASSWORD_PROMPT);

        if (ERROR == odmUserInputGet(pstEnv, 1, acPasswd, USER_PASSWD_MAX_SIZE)) {
            if (pstEnv->ucIsConsole) {
                goto restart;
            }
            return INVALID_32;
        }

        acPasswd[USER_PASSWD_MAX_SIZE] = '\0';
        utilStrTrim(acPasswd);

        /* printf("odmUserLogin got password :%s\r\n", acPasswd); */
        
        if (odmUserValidCheck(acName, acPasswd) != NO_ERROR)
        {
            vosPrintf(pstEnv->nWriteFd, "\r\nLogin incorrect\r\n");

            continue;
        }
				
        vosPrintf(pstEnv->nWriteFd, "\r\n\r\nWelcome '%s' to %s Operating System\r\n", acName, product_info);
        if (NULL != pstEnv->pcUserName)
        {
            vosFree(pstEnv->pcUserName);
            pstEnv->pcUserName = NULL;
        }
        pstEnv->pcUserName = vosStrDup(acName);

		/* To get user name 's access level */	
		if(!vosStrCmp(acName, SYS_SUPER_USER_NAME))
		{
		 	pstEnv->accessLevel = ACCESS_LEVEL_SUPER;
		}
		else
		{
			pstEnv->accessLevel = odmUserAccessLevelGet(pstEnv->pcUserName);
		}

		switch(pstEnv->accessLevel)
		{
		   case ACCESS_LEVEL_GUEST:
			 /* vosPrintf(pstEnv->nWriteFd, "\r\nYou do not have access right to enter privilege mode!\n");*/
			   pstEnv->pmode = CLI_MODE_USER;
			   break;
		   case ACCESS_LEVEL_USER:
		   case ACCESS_LEVEL_ADMINISTRATION:
			   pstEnv->pmode = CLI_MODE_ENABLE;
			   break;
		
		   case ACCESS_LEVEL_SUPER:
			   pstEnv->pmode = CLI_MODE_SUPER;
			   break;
		
		   default:
			   /* Do nothing */
			   break;
		}
	
		vosPrintf(pstEnv->nWriteFd, "\r\n");
		
        return NO_ERROR;
    }
}

 
 
