#include "dal_lib.h"
#include "opconn_ioctl.h"
#include <asm/uaccess.h>
#include <asm/bitops.h>
#include <asm/io.h>

static int ioctl_swmdioglbregisterread( unsigned int arg );
static int ioctl_swmdioglbregisterwrite( unsigned int arg );
static int ioctl_swmdiophyregisterread( unsigned int arg );
static int ioctl_swmdiophyregisterwrite( unsigned int arg );
static int ioctl_QinqPvidSet(unsigned int arg);
static int ioctl_QinqPvidReSet(unsigned int arg);
static int ioctl_VttEntryTagAdd(unsigned int arg);
#ifdef CONFIG_BOSA
static int ioctl_dalpon25l90imodset(unsigned int arg);
static int ioctl_dalpon25l90Apcset(unsigned int arg);
#endif
int krn2usr( void *dst, void *src, UINT32 size )
{
  return copy_to_user( dst, src, size );
}

int usr2krn( void *dst, void *src, UINT32 size )
{
  return copy_from_user( dst, src, size );
}


static int ioctl_dalponmacidset( unsigned int arg )
{
	OPL_STATUS ret;
	dalponmacidset_params_t params;

	USR2KRN( params, (void*)arg );
	
	OPL_DRV_PRINTF(("mac = %x %x %x %x %x %x\n", params.mac[0],
		params.mac[1],params.mac[2],
		params.mac[3],params.mac[4],
		params.mac[5]));

	ret = dalPonMacIdSet( params.mac );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalponmacidget( unsigned int arg )
{
	OPL_STATUS ret;
	dalponmacidset_params_t params;

	ret = dalPonMacIdGet( params.mac );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalponphyfecmodeset( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalPonPhyFecModeSet( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalponphyfecmodeget( unsigned int arg )
{
	OPL_STATUS ret;
	UINT32 mode;

	ret = dalPonPhyFecModeGet( &mode );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, mode );

	return ret;
}

static int ioctl_dalponphytriplechuningmodeset( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalPonPhyTripleChuningModeSet( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalponphytriplechuningmodeget( unsigned int arg )
{
	OPL_STATUS ret;
	UINT32 mode;

	ret = dalPonPhyTripleChuningModeGet( &mode );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, mode );

	return ret;
}

static int ioctl_dalponphyaesmodeset( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalPonPhyAesModeSet( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalponphyaesmodeget( unsigned int arg )
{
	OPL_STATUS ret;
	UINT32 mode;

	ret = dalPonPhyAesModeGet( &mode );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, mode );

	return ret;
}

static int ioctl_dalponlaseroncntset( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalPonLaserOnCntSet( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalponlaseroncntget( unsigned int arg )
{
	OPL_STATUS ret;
	UINT32 count;

	ret = dalPonLaserOnCntGet( &count );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, count );

	return ret;
}

static int ioctl_dalponlaseroffcntset( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalPonLaserOffCntSet( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalponlaseroffcntget( unsigned int arg )
{
	OPL_STATUS ret;
	UINT32 count;

	ret = dalPonLaserOffCntGet( &count );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, count );

	return ret;
}

static int ioctl_dalponsynctimeget( unsigned int arg )
{
	OPL_STATUS ret;
	UINT32 count;

	ret = dalPonSyncTimeGet( &count );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, count );

	return ret;
}

static int ioctl_dalponllidget( unsigned int arg )
{
	OPL_STATUS ret;
	UINT32 llid;

	ret = dalPonLlidGet( &llid );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, llid );

	return ret;
}

static int ioctl_dalponserdesdelaycntset( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalPonSerdesDelayCntSet( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalponserdesdelaycntget( unsigned int arg )
{
	OPL_STATUS ret;
	UINT32 count;

	ret = dalPonSerdesDelayCntGet( &count );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, count );

	return ret;
}

static int ioctl_dalponsecuritymodeset( unsigned int arg )
{
	OPL_STATUS ret;
	dalponsecuritymodeset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPonSecurityModeSet( params.channel, params.mode );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalponsecuritymodeget( unsigned int arg )
{
	OPL_STATUS ret;
	dalponsecuritymodeget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPonSecurityModeGet( params.channel, &params.mode );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalponsecurityaesmodeset( unsigned int arg )
{
	OPL_STATUS ret;
	dalponsecurityaesmodeset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPonSecurityAesModeSet( params.channel, params.mode );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalponsecurityaesmodeget( unsigned int arg )
{
	OPL_STATUS ret;
	dalponsecurityaesmodeget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPonSecurityAesModeGet( params.channel, &params.mode );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalponunicasttriplechurningkeyget( unsigned int arg )
{
	OPL_STATUS ret;
	dalponunicasttriplechurningkeyget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPonUnicastTripleChurningKeyGet( params.keyIndex, params.keyBuff );
	if ( ret )
	{
		return ret;
	}

	//KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalponscbtriplechurningkeyset( unsigned int arg )
{
	OPL_STATUS ret;
	dalponscbtriplechurningkeyset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPonScbTripleChurningKeySet( params.keyIndex, params.keyBuff );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalponunicastaeskeyget( unsigned int arg )
{
	OPL_STATUS ret;
	dalponunicastaeskeyget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPonUnicastAesKeyGet( params.keyIndex, params.keyBuff, params.mode );
	if ( ret )
	{
		return ret;
	}

	//KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalponscbaeskeyset( unsigned int arg )
{
	OPL_STATUS ret;
	dalponscbaeskeyset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPonScbAesKeySet( params.keyIndex, params.keyBuff, params.mode );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalpontriplechurningmodeget( unsigned int arg )
{
	OPL_STATUS ret;
	UINT8 mode;

	ret = dalPonTripleChurningModeGet( &mode );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, mode );

	return ret;
}

static int ioctl_dalpontriplechurningmodeset( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalPonTripleChurningModeSet( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalponmpcpdbaagentconfigallset( unsigned int arg )
{
	OPL_STATUS ret;
	dalponmpcpdbaagentconfigallset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPonMpcpDbaAgentConfigAllSet( params.cpRptStaMode, params.cpMergeRptSendMode, params.cpRptSendMode, params.cpPktInitTimerCfg, params.cpDeltaTxNormalNoFecTime, params.cpTxMpcpNoFecTime );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalponmpcpdbaagentconfigallget( unsigned int arg )
{
	OPL_STATUS ret;
	dalponmpcpdbaagentconfigallget_params_t params;

	ret = dalPonMpcpDbaAgentConfigAllGet( &params.cpRptStaMode, &params.cpMergeRptSendMode, &params.cpRptSendMode, &params.cpPktInitTimerCfg, &params.cpDeltaTxNormalNoFecTime, &params.cpTxMpcpNoFecTime );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalpondelayconfigallset( unsigned int arg )
{
	OPL_STATUS ret;
	dalpondelayconfigallset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPonDelayConfigAllSet( params.cpTxDlyEn, params.cpSyncEn, params.cpRptsentDelt, params.cpTxDly );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalpondelayconfigallget( unsigned int arg )
{
	OPL_STATUS ret;
	dalpondelayconfigallget_params_t params;

	ret = dalPonDelayConfigAllGet( &params.cpTxDlyEn, &params.cpSyncEn, &params.cpRptsentDelt, &params.cpTxDly );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalponmpcpstatusget( unsigned int arg )
{
	OPL_STATUS ret;
	DAL_PON_STATUS_t ponStatus;

	ret = dalPonMpcpStatusGet( &ponStatus );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, ponStatus );

	return ret;
}

static int ioctl_dalponoamfinishedset( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalPonOamFinishedSet( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalponmpcpfsmholdoverctrlset( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalPonMpcpFsmHoldOverCtrlSet( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalponmpcpfsmholdoverctrlget( unsigned int arg )
{
	OPL_STATUS ret;
	UINT32 enable;

	ret = dalPonMpcpFsmHoldOverCtrlGet( &enable );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, enable );

	return ret;
}

static int ioctl_dalponmpcpfsmholdovertimeset( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalPonMpcpFsmHoldOverTimeSet( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalponmpcpfsmholdovertimeget( unsigned int arg )
{
	OPL_STATUS ret;
	UINT32 holdTime;

	ret = dalPonMpcpFsmHoldOverTimeGet( &holdTime );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, holdTime );

	return ret;
}

static int ioctl_dalclstblentryadd( unsigned int arg )
{
	OPL_STATUS ret;
	dalclstblentryadd_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsTblEntryAdd( params.pMaskEntry, params.pRuleEntry, &params.pCruleID, params.pClshwtbl );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalclstblentrydel( unsigned int arg )
{
	OPL_STATUS ret;
	dalclstblentrydel_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsTblEntryDel( params.pMaskEntry, params.pRuleEntry, &params.pCruleID );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalclsmasktblwr( unsigned int arg )
{
	OPL_STATUS ret;
	dalclsmasktblwr_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsMaskTblWr( params.pMask, params.maskID );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclsruletblwr( unsigned int arg )
{
	OPL_STATUS ret;
	dalclsruletblwr_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsRuleTblWr( params.pRule, params.ruleID );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclsaddrtblwr( unsigned int arg )
{
	OPL_STATUS ret;
	dalclsaddrtblwr_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsAddrTblWr( params.address, params.offset, params.ruleId );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclsmaskhwwr( unsigned int arg )
{
	OPL_STATUS ret;
	dalclsmaskhwwr_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsMaskHwWr( params.pMask, params.maskID );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclsrulehwwr( unsigned int arg )
{
	OPL_STATUS ret;
	dalclsrulehwwr_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsRuleHwWr( params.pRule, params.ruleID );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclsaddrhwwr( unsigned int arg )
{
	OPL_STATUS ret;
	dalclsaddrhwwr_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsAddrHwWr( params.address, params.offset, params.ruleId );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclsruletblrd( unsigned int arg )
{
	OPL_STATUS ret;
	dalclsruletblrd_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsRuleTblRd( params.pRule, params.ruleID, params.tblType );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalclsaddrtblrd( unsigned int arg )
{
	OPL_STATUS ret;
	dalclsaddrtblrd_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsAddrTblRd( params.address, params.offset, &params.pRuleId, params.tblType );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalclsentryset( unsigned int arg )
{
	OPL_STATUS ret;
	dalclsentryset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsEntrySet( params.pMaskRulePar, &params.pCruleID, &params.pCmaskID );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalclsentrymove( unsigned int arg )
{
	OPL_STATUS ret;
	dalclsentrymove_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsEntryMove( params.pRulePar, &params.pCruleID );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalclsinit( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalClsInit(  );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclstranactset( unsigned int arg )
{
	OPL_STATUS ret;
	dalclstranactset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsTranActSet( params.classRuleID, params.actValue );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclsrmkactset( unsigned int arg )
{
	OPL_STATUS ret;
	dalclsrmkactset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsRmkActSet( params.classRuleID, params.rmkactVal );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclsrmkcosset( unsigned int arg )
{
	OPL_STATUS ret;
	dalclsrmkcosset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsRmkCosSet( params.classRuleID, params.cosValue );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclsrmkicosset( unsigned int arg )
{
	OPL_STATUS ret;
	dalclsrmkicosset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsRmkIcosSet( params.classRuleID, params.iCosVal );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclsrmkvidset( unsigned int arg )
{
	OPL_STATUS ret;
	dalclsrmkvidset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsRmkVidSet( params.classRuleID, params.rmkVid );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclsrmkvidactset( unsigned int arg )
{
	OPL_STATUS ret;
	dalClsRmkVidActSet_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsRmkVidActSet( params.classRuleID, params.rmkVidAct , params.rmkVid );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclsegressportcfg( unsigned int arg )
{
	OPL_STATUS ret;
	dalclsegressportcfg_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsEgressPortCfg( params.classItemID, params.pktType, params.portID, params.eMask, params.bEnable );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclsingressportcfg( unsigned int arg )
{
	OPL_STATUS ret;
	dalclsingressportcfg_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsIngressPortCfg( params.classItemID, params.pktType, params.portID ,params.iMask, params.bEnable );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclstranactcfg( unsigned int arg )
{
	OPL_STATUS ret;
	dalclstranactcfg_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsTranActCfg( params.classItemID, params.pktType, params.actValue );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclsrmkactcfg( unsigned int arg )
{
	OPL_STATUS ret;
	dalclsrmkactcfg_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsRmkActCfg( params.classItemID, params.pktType, params.rmkActVal );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclsrmkcoscfg( unsigned int arg )
{
	OPL_STATUS ret;
	dalclsrmkcoscfg_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsRmkCosCfg( params.classItemID, params.pktType, params.cosValue );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclsrmkicoscfg( unsigned int arg )
{
	OPL_STATUS ret;
	dalclsrmkicoscfg_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsRmkIcosCfg( params.classItemID, params.pktType, params.iCosValue );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclsrmkvidcfg( unsigned int arg )
{
	OPL_STATUS ret;
	dalclsrmkvidcfg_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsRmkVidCfg( params.classItemID, params.pktType, params.rmkVid );
	if ( ret )
	{
		return ret;
	}

	return ret;
}



static int ioctl_dalclsmaskpriocfg( unsigned int arg )
{
	OPL_STATUS ret;
	dalclsmaskpriocfg_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsMaskPrioCfg( params.classItemID, params.pktType, params.maskPrio );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclsdestmacaddrcfg( unsigned int arg )
{
	OPL_STATUS ret;
	dalclsdestmacaddrcfg_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsDestMacAddrCfg( params.classItemID, params.pktType, params.pDestMac, params.pUsrMask, params.bEnable );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclssrcmacaddrcfg( unsigned int arg )
{
	OPL_STATUS ret;
	dalclssrcmacaddrcfg_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsSrcMacAddrCfg( params.classItemID, params.pktType, params.pSrcMac, params.pUsrMask, params.bEnable );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclsfirstcoscfg( unsigned int arg )
{
	OPL_STATUS ret;
	dalclsfirstcoscfg_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsFirstCosCfg( params.classItemID, params.pktType, params.cosVal, params.usrMask, params.bEnable );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclsfirstvidcfg( unsigned int arg )
{
	OPL_STATUS ret;
	dalclsfirstvidcfg_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsFirstVidCfg( params.classItemID, params.pktType, params.vlanID, params.usrMask, params.bEnable );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclstypelencfg( unsigned int arg )
{
	OPL_STATUS ret;
	dalclstypelencfg_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsTypeLenCfg( params.classItemID, params.pktType, params.typeLen, params.usrMask, params.bEnable );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclsdestipcfg( unsigned int arg )
{
	OPL_STATUS ret;
	dalclsdestipcfg_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsDestIpCfg( params.classItemID, params.pktType, params.pDestIp, params.pUsrMask, params.bEnable );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclssrcipcfg( unsigned int arg )
{
	OPL_STATUS ret;
	dalclssrcipcfg_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsSrcIpCfg( params.classItemID, params.pktType, params.pSrcIp, params.pUsrMask, params.bEnable );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclsiptypecfg( unsigned int arg )
{
	OPL_STATUS ret;
	dalclsiptypecfg_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsIpTypeCfg( params.classItemID, params.pktType, params.ipType, params.usrMask, params.bEnable );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclsiptoscfg( unsigned int arg )
{
	OPL_STATUS ret;
	dalclsiptoscfg_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsIpTosCfg( params.classItemID, params.pktType, params.ipTos, params.usrMask, params.bEnable );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclsl4destportcfg( unsigned int arg )
{
	OPL_STATUS ret;
	dalclsl4destportcfg_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsL4DestPortCfg( params.classItemID, params.pktType, params.L4DestPortID, params.usrMask, params.bEnable );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclsl4srcportcfg( unsigned int arg )
{
	OPL_STATUS ret;
	dalclsl4srcportcfg_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsL4SrcPortCfg( params.classItemID, params.pktType, params.L4SrcPortID, params.usrMask, params.bEnable );
	if ( ret )
	{
		return ret;
	}

	return ret;
}
#if 1 /*wfxu*/

static int ioctl_dalclsruleentryshow( unsigned int arg )
{
	OPL_STATUS ret;
	dalclsruleentryshow_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsRuleEntryShow( params.startId, params.endId, params.mode );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclsmaskentryshow( unsigned int arg )
{
	OPL_STATUS ret;
	dalclsmaskentryshow_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsMaskEntryShow(params.startId, params.endId, params.mode );
	if ( ret )
	{
		return ret;
	}

	return ret;
}


static int ioctl_dalclsaddrentryshow( unsigned int arg )
{
	OPL_STATUS ret;
	dalclsaddrentryshow_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsAddrEntryShow(params.startId, params.endId, params.mode );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclscfgshow( unsigned int arg )
{
	OPL_STATUS ret;
	dalclscfgshow_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsCfgShow(  params.usFirstItem, params.usLastItem);
	if ( ret )
	{
		return ret;
	}

	return ret;
}

#endif
static int ioctl_dalclsruledel( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalClsRuleDel( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclsconvertpara( unsigned int arg )
{
	OPL_STATUS ret;
	dalclsconvertpara_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsConvertPara( params.pClassItemInfo, params.stClassCfg, params.pktType );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalclsctcinit( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalClsCtcInit(  );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclsrulectcadd( unsigned int arg )
{
	OPL_STATUS ret;
	dalclsrulectcadd_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsRuleCtcAdd( params.stClassCfg, &params.ruleId );
	if ( ret )
	{
		return ret;
	}
	
	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalvoipclsrulectcadd( unsigned int arg )
{
	OPL_STATUS ret;
	dalvoipclsrulectcadd_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalVoipClsRuleCtcAdd( params.stClassCfg, &params.ruleId );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalclsrulectcdel( unsigned int arg )
{
	OPL_STATUS ret;
	dalclsrulectcdel_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsRuleCtcDel( params.stClassCfg, &params.ruleId );
	if ( ret )
	{
		return ret;
	}
	
	KRN2USR( (void*)arg, params );

	return ret;
}


static int ioctl_dalclsipversioncfg( unsigned int arg )
{
	OPL_STATUS ret;
	dalClsIpVersionCfg_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsIpVersionCfg( params.classItemID, params.pktType, params.ipVersion,params.usrMask,params.bEnable);
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclsfirsttpidcfg( unsigned int arg )
{
	OPL_STATUS ret;
	dalClsFirstTpidCfg_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsIpVersionCfg( params.classItemID, params.pktType, params.firsttpid,params.usrMask,params.bEnable);
	if ( ret )
	{
		return ret;
	}

	return ret;
}


static int ioctl_dalclsrulectcclear( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalClsRuleCtcClear(  );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

/* Begin Added of qinq */
static int ioctl_dalClsValidEntryGet(unsigned int arg)
{
    OPL_STATUS ret;
	UINT16 usClsId = 0;
	
	ret = dalClsValidEntryGet(&usClsId);
	if ( ret )
	{
		return ret;
	}
	
	KRN2USR( (void*)arg, usClsId );
	
	return ret;
}
/* End   Added of qinq */


static int ioctl_dalexclsdelentry( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalExClsDelEntry( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalchipreset( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalChipReset(  );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_daldemoninit( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalDemonInit(  );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalarlagetimeset( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalArlAgeTimeSet( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalarlagetimeget( unsigned int arg )
{
	OPL_STATUS ret;
	UINT32 ageingTime;

	ret = dalArlAgeTimeGet( &ageingTime );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, ageingTime );

	return ret;
}

static int ioctl_dalarlmacadd( unsigned int arg )
{
	OPL_STATUS ret;
	dalarlmacadd_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalArlMacAdd( params.portNum, params.macAddress, params.vlanIndex );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalarlmacdel( unsigned int arg )
{
	OPL_STATUS ret;
	dalarlmacdel_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalArlMacDel( params.portNum, params.macAddress, params.vlanIndex );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalarlmacremove( unsigned int arg )
{
	OPL_STATUS ret;
	dalarlmacremove_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalArlMacRemove( params.macAddress, params.vlanIndex );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalarlflushdynamic( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalArlFlushDynamic(  );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalarlflushall( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalArlFlushAll(  );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalarllearnenset( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalArlLearnEnSet( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalarllearnenget( unsigned int arg )
{
	OPL_STATUS ret;
	UINT8 enable;

	ret = dalArlLearnEnGet( &enable );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, enable );

	return ret;
}

static int ioctl_dalarlsoftlearnenset( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalArlSoftLearnEnSet( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalarlsoftlearnenget( unsigned int arg )
{
	OPL_STATUS ret;
	UINT8 enable;

	ret = dalArlSoftLearnEnGet( &enable );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, enable );

	return ret;
}

static int ioctl_dalarlageenableset( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalArlAgeEnableSet( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalarlageenableget( unsigned int arg )
{
	OPL_STATUS ret;
	UINT8 enable;

	ret = dalArlAgeEnableGet( &enable );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, enable );

	return ret;
}

static int ioctl_dalarlmacnumlimitset( unsigned int arg )
{
	OPL_STATUS ret;
	dalarlmacnumlimitset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalArlMacNumLimitSet( params.portID, params.enable, params.num );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalarlmultiportmacadd( unsigned int arg )
{
	OPL_STATUS ret;
	dalarlmultiportmacadd_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalArlMultiPortMacAdd( params.uiPortNum, &params.auiPortlist, params.aucMacAddress, params.usVlanId );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalarlmultiportmacdel( unsigned int arg )
{
	OPL_STATUS ret;
	dalarlmultiportmacdel_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalArlMultiPortMacDel( params.uiPortNum, &params.auiPortlist, params.aucMacAddress, params.usVlanId );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalarlmacentryfind( unsigned int arg )
{
	OPL_STATUS ret;
	dalarlmacentryfind_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalArlMacEntryFind( params.aucMacAddress, params.usVlanId, &params.pstMacEntryInfo, &params.pbFind );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalarlmacentrytraverse( unsigned int arg )
{
	OPL_STATUS ret;
	dalarlmacentrytraverse_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalArlMacEntryTraverse( &params.puiStartMacEntryId, &params.pstMacEntryInfo, &params.pbFind );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalarlmacentryshowone( unsigned int arg )
{
	OPL_STATUS ret= OPL_OK;
	dalarlmacentryshowone_params_t params;

	USR2KRN( params, (void*)arg );

	//wfxu ret = dalArlMacEntryShowOne( params.lFd, &params.aucMacAddress, params.usType, params.uiPortNum, &params.auiPortlist );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalarlmacentryshowall( unsigned int arg )
{
	OPL_STATUS ret = OPL_OK;
	dalarlmacentryshowall_params_t params;

	USR2KRN( params, (void*)arg );

	//wfxu ret = dalArlMacEntryShowAll( params.lFd, params.usType, params.uiPortNum, &params.auiPortlist );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalmulticastinit( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalMulticastInit(  );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalmulticastvlanaddport( unsigned int arg )
{
	OPL_STATUS ret;
	dalmulticastvlanaddport_params_t params;

	USR2KRN( params, (void*)arg );
#if defined(ONU_4PORT_AR8327)
	ret = dalMulticastVlanAddPort( params.portNum, params.vlanId, params.tag );
#else
	ret = dalMulticastVlanAddPort( params.portNum, params.vlanId );
#endif
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalmulticastvlandelport( unsigned int arg )
{
	OPL_STATUS ret;
	dalmulticastvlandelport_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalMulticastVlanDelPort( params.portNum, params.vlanId );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalmulticastvlantagstripe( unsigned int arg )
{
	OPL_STATUS ret;
	dalmulticastvlantagstripe_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalMulticastVlanTagStripe( params.portNum, params.enable );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalmulticastarladdport( unsigned int arg )
{
	OPL_STATUS ret;
	dalmulticastarladdport_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalMulticastArlAddPort( params.portNum, params.macAddr );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalmulticastarlportdel( unsigned int arg )
{
	OPL_STATUS ret;
	dalmulticastarlportdel_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalMulticastArlPortDel( params.portNum, params.macAddr );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalmulticastarlvlanadd( unsigned int arg )
{
	OPL_STATUS ret;
	dalmulticastarlvlanadd_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalMulticastArlVlanAdd( params.portNum, params.vlanId, params.macAddr );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalmulticastarlvlandel( unsigned int arg )
{
	OPL_STATUS ret;
	dalmulticastarlvlandel_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalMulticastArlVlanDel( params.portNum, params.vlanId, params.macAddr );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalmulticastarltabflush( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalMulticastArlTabFlush(  );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalmulticastportvlanmacadd( unsigned int arg )
{
	OPL_STATUS ret;
	dalmulticastportvlanmacadd_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalMulticastPortVlanMacAdd( params.portNum, params.vlanId, params.macAddr, params.fuzzy );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalmulticastportvlanmacdel( unsigned int arg )
{
	OPL_STATUS ret;
	dalmulticastportvlanmacdel_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalMulticastPortVlanMacDel( params.portNum, params.vlanId, params.macAddr, params.fuzzy );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalmulticastportvlanipadd( unsigned int arg )
{
	OPL_STATUS ret;
	dalmulticastportvlanipadd_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalMulticastPortVlanIpAdd( params.portNum, params.vlanId, params.ip );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalmulticastportvlanipdel( unsigned int arg )
{
	OPL_STATUS ret;
	dalmulticastportvlanipdel_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalMulticastPortVlanIpDel( params.portNum, params.vlanId, params.ip );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalmulticastruledelall( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalMulticastRuleDelAll(  );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalmulticastdatafilteradd( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalMulticastDataFilterAdd(  );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalmulticastdatafilterdel( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalMulticastDataFilterDel(  );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalmulticastnumofruleget( unsigned int arg )
{
	OPL_STATUS ret;
	UINT16 numOfRule;

	ret = dalMulticastNumofRuleGet( &numOfRule );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, numOfRule );

	return ret;
}

static int ioctl_dalportnumisinvalid( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalPortNumIsInvalid( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalportspecificduplexread( unsigned int arg )
{
	OPL_STATUS ret;
	dalportspecificduplexread_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPortSpecificDuplexRead( params.portId, &params.duplex );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalphyspecificautonegread( unsigned int arg )
{
	OPL_STATUS ret;
	dalphyspecificautonegread_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPhySpecificAutonegRead( params.portId, &params.state );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalportspecificspeedread( unsigned int arg )
{
	OPL_STATUS ret;
	dalportspecificspeedread_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPortSpecificSpeedRead( params.portId, &params.portSpeed );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalportstateget( unsigned int arg )
{
	OPL_STATUS ret;
	dalportstateget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPortStateGet( params.portId, &params.status );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalportspeedset( unsigned int arg )
{
	OPL_STATUS ret;
	dalportspeedset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPortSpeedSet( params.portId, params.portSpeed );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalportspeedget( unsigned int arg )
{
	OPL_STATUS ret;
	dalportspeedget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPortSpeedGet( params.portId, &params.portSpeed );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalportduplexset( unsigned int arg )
{
	OPL_STATUS ret;
	dalportduplexset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPortDuplexSet( params.portId, params.duplex );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalportduplexget( unsigned int arg )
{
	OPL_STATUS ret;
	dalportduplexget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPortDuplexGet( params.portId, &params.duplex );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalportflowcontrolset( unsigned int arg )
{
	OPL_STATUS ret;
	dalportflowcontrolset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPortFlowcontrolSet( params.portId, params.enable );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalportflowcontrolget( unsigned int arg )
{
	OPL_STATUS ret;
	dalportflowcontrolget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPortFlowcontrolGet( params.portId, &params.enable );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalportloopbackset( unsigned int arg )
{
	OPL_STATUS ret;
	dalportloopbackset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPortLoopbackSet( params.portId, params.enable );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalportuspolicingenableset( unsigned int arg )
{
	OPL_STATUS ret;
	dalportuspolicingenableset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPortUsPolicingEnableSet( params.portId, params.enable );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalportuspolicingcirset( unsigned int arg )
{
	OPL_STATUS ret;
	dalportuspolicingcirset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPortUsPolicingCirSet( params.portId, params.cir );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalportuspolicingcirget( unsigned int arg )
{
	OPL_STATUS ret;
	dalportuspolicingcirget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPortUsPolicingCirGet( params.portId, &params.cir );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalportuspolicingcbsset( unsigned int arg )
{
	OPL_STATUS ret;
	dalportuspolicingcbsset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPortUsPolicingCbsSet( params.portId, params.cbs );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalportuspolicingcbsget( unsigned int arg )
{
	OPL_STATUS ret;
	dalportuspolicingcbsget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPortUsPolicingCbsGet( params.portId, &params.cbs );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalportuspolicingebsset( unsigned int arg )
{
	OPL_STATUS ret;
	dalportuspolicingebsset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPortUsPolicingEbsSet( params.portId, params.ebs );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalportuspolicingebsget( unsigned int arg )
{
	OPL_STATUS ret;
	dalportuspolicingebsget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPortUsPolicingEbsGet( params.portId, &params.ebs );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalportuspolicingset( unsigned int arg )
{
	OPL_STATUS ret;
	dalportuspolicingset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPortUsPolicingSet( params.portId, params.enable, params.cir, params.cbs, params.ebs );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalportuspolicingget( unsigned int arg )
{
	OPL_STATUS ret;
	dalportuspolicingget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPortUsPolicingGet( params.portId, &params.enable, &params.cir, &params.cbs, &params.ebs );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalportdspolicingenableset( unsigned int arg )
{
	OPL_STATUS ret;
	dalportdspolicingenableset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPortDsPolicingEnableSet( params.portId, params.enable );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalportdspolicingenableget( unsigned int arg )
{
	OPL_STATUS ret;
	dalportdspolicingenableget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPortDsPolicingEnableGet( params.portId, &params.enable );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalportdspolicingcirset( unsigned int arg )
{
	OPL_STATUS ret;
	dalportdspolicingcirset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPortDsPolicingCirSet( params.portId, params.cir );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalportdspolicingcirget( unsigned int arg )
{
	OPL_STATUS ret;
	dalportdspolicingcirget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPortDsPolicingCirGet( params.portId, &params.cir );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalportdspolicingpirset( unsigned int arg )
{
	OPL_STATUS ret;
	dalportdspolicingpirset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPortDsPolicingPirSet( params.portId, params.pir );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalportdspolicingpirget( unsigned int arg )
{
	OPL_STATUS ret;
	dalportdspolicingpirget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPortDsPolicingPirGet( params.portId, &params.pir );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalportdspolicingset( unsigned int arg )
{
	OPL_STATUS ret;
	dalportdspolicingset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPortDsPolicingSet( params.portId, params.enable, params.cir, params.pir );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalportdspolicingget( unsigned int arg )
{
	OPL_STATUS ret;
	dalportdspolicingget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPortDsPolicingGet( params.portId, &params.enable, &params.cir, &params.pir );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalphyadminstateget( unsigned int arg )
{
	OPL_STATUS ret;
	dalphyadminstateget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPhyAdminStateGet( params.portId, &params.state );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalphyadmincontrolset( unsigned int arg )
{
	OPL_STATUS ret;
	dalphyadmincontrolset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPhyAdminControlSet( params.portId, params.state );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalphyautonegadminstateget( unsigned int arg )
{
	OPL_STATUS ret;
	dalphyautonegadminstateget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPhyAutonegAdminStateGet( params.portId, &params.state );
	if ( ret )
	{
		return ret;
	}
	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalphyautonegenableset( unsigned int arg )
{
	OPL_STATUS ret;
	
	dalphyautonegenableset_params_t params;

	USR2KRN( params, (void*)arg );
	ret = dalPhyAutonegEnableSet( params.portId, params.state );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalphyautonegrestart( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalPhyAutonegRestart( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalportinit( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalPortInit(  );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalrstpmodeset( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalRstpModeSet( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalrstpportstateget( unsigned int arg )
{
	OPL_STATUS ret;
	dalrstpportstateget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalRstpPortStateGet( params.ulPortId, &params.pucPortState );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalrstpportstateset( unsigned int arg )
{
	OPL_STATUS ret;
	dalrstpportstateset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalRstpPortStateSet( params.ulPortId, params.ucPortState );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalrstpportmacflush( unsigned int arg )
{
	OPL_STATUS ret;
	dalrstpportmacflush_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalRstpPortMacFlush( params.ulPortid, params.bOnlyThisPort );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalstormclsruleadd( unsigned int arg )
{
	OPL_STATUS ret;
	dalstormclsruleadd_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalStormClsRuleAdd( params.stormLimitType, params.streamType );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalstormclsruledel( unsigned int arg )
{
	OPL_STATUS ret;
	dalstormclsruledel_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalStormClsRuleDel( params.stormLimitType, params.streamType );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalstormctrlframeset( unsigned int arg )
{
	OPL_STATUS ret;
	dalstormctrlframeset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalStormCtrlFrameSet( params.portId, params.stormLimitType, params.limitEnable );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalstormctrlframeget( unsigned int arg )
{
	OPL_STATUS ret;
	dalstormctrlframeget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalStormCtrlFrameGet( params.portId, params.stormLimitType, &params.limitEnable );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalstormctrlrateset( unsigned int arg )
{
	OPL_STATUS ret;
	dalstormctrlrateset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalStormCtrlRateSet( params.portId, params.rateLimit );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalstormctrlrateget( unsigned int arg )
{
	OPL_STATUS ret;
	dalstormctrlrateget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalStormCtrlRateGet( params.portId, &params.rateLimit );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_oplverstrget( unsigned int arg )
{
	OPL_STATUS ret;
	UINT8 pString;

	ret = oplVerStrGet( &pString );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, pString );

	return ret;
}

#if 0/*implement in user space*/

static int ioctl_oplvernumget( unsigned int arg )
{
	OPL_STATUS ret;
	FLOAT32 verNum;

	ret = oplVerNumGet( &verNum );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, verNum );

	return ret;
}
#endif
static int ioctl_oplvershow( unsigned int arg )
{
	OPL_STATUS ret;

	ret = oplVerShow(  );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalvttmodeset( unsigned int arg )
{
	OPL_STATUS ret;
	dalvttmodeset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalVttModeSet( params.mode, params.portId );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalvttmodeget( unsigned int arg )
{
	OPL_STATUS ret;
	dalvttmodeget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalVttModeGet( &params.mode, params.portId );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalvtttpidset( unsigned int arg )
{
	OPL_STATUS ret;
	dalvtttpidset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalVttTpidSet( params.tpid, params.portId );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalvtttpidget( unsigned int arg )
{
	OPL_STATUS ret;
	dalvtttpidget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalVttTpidGet( &params.tpid, params.portId );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalvttvidset( unsigned int arg )
{
	OPL_STATUS ret;
	dalvttvidset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalVttVidSet( params.pvid, params.portId );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalvttvidget( unsigned int arg )
{
	OPL_STATUS ret;
	dalvttvidget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalVttVidGet( &params.pvid, params.portId );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalvttdefaultpriset( unsigned int arg )
{
	OPL_STATUS ret;
	dalvttdefaultpriset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalVttDefaultPriSet( params.etherPri, params.portId );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalvttdefaultpriget( unsigned int arg )
{
	OPL_STATUS ret;
	dalvttdefaultpriget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalVttDefaultPriGet( &params.etherPri, params.portId );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalvtttagentryadd( unsigned int arg )
{
	OPL_STATUS ret;
	dalvtttagentryadd_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalVttTagEntryAdd( params.pStrip, params.gStrip, params.cVlan, params.sVlan, params.portId, &params.pEntryIndex, params.type );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalvtttagentryget( unsigned int arg )
{
	OPL_STATUS ret;
	dalvtttagentryget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalVttTagEntryGet( params.ValidRecordIndex, &params.cVlan, &params.sVlan, params.portId );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalvtttagentrydel( unsigned int arg )
{
	OPL_STATUS ret;
	dalvtttagentrydel_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalVttTagEntryDel( params.pStrip, params.gStrip, params.cVlan, params.sVlan, params.portId, &params.pEntryIndex, params.type );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalvttnumofrecordget( unsigned int arg )
{
	OPL_STATUS ret;
	dalvttnumofrecordget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalVttNumOfRecordGet( &params.numOfRecords, params.portId );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalvttmulticastentryadd( unsigned int arg )
{
	OPL_STATUS ret;
	dalvttmulticastentryadd_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalVttMulticastEntryAdd( params.pStrip, params.gStrip, params.cVlan, params.sVlan, params.portId );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalvttmulticastentrydel( unsigned int arg )
{
	OPL_STATUS ret;
	dalvttmulticastentrydel_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalVttMulticastEntryDel( params.pStrip, params.gStrip, params.cVlan, params.sVlan, params.portId );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalvttmulticaststripenable( unsigned int arg )
{
	OPL_STATUS ret;
	dalvttmulticaststripenable_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalVttMulticastStripEnable( params.portId, params.enable );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalvttmulticastinit( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalVttMulticastInit( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalvttmulticastnumofrecordget( unsigned int arg )
{
	OPL_STATUS ret;
	UINT8 numOfRecords;

	ret = dalVttMulticastNumOfRecordGet( &numOfRecords );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, numOfRecords );

	return ret;
}

static int ioctl_dalvttinit( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalVttInit(  );
	if ( ret )
	{
		return ret;
	}

	return ret;
}


static int ioctl_tmgeshaperenablehwwrite( unsigned int arg )
{
	OPL_STATUS ret;
	tmgeshaperenablehwwrite_params_t params;

	USR2KRN( params, (void*)arg );

	ret = tmGeShaperEnableHwWrite( params.stream, params.enable );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_tmgeshaperenablehwread( unsigned int arg )
{
	OPL_STATUS ret;
	tmgeshaperenablehwread_params_t params;

	USR2KRN( params, (void*)arg );

	ret = tmGeShaperEnableHwRead( params.stream, &params.enable );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_tmqueueshaperenablehwwrite( unsigned int arg )
{
	OPL_STATUS ret;
	tmqueueshaperenablehwwrite_params_t params;

	USR2KRN( params, (void*)arg );

	ret = tmQueueShaperEnableHwWrite( params.stream, params.Qid, params.enable );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_tmqueueshaperenablehwread( unsigned int arg )
{
	OPL_STATUS ret;
	tmqueueshaperenablehwread_params_t params;

	USR2KRN( params, (void*)arg );

	ret = tmQueueShaperEnableHwRead( params.stream, params.Qid, &params.enable );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_tmqueueprihwwrite( unsigned int arg )
{
	OPL_STATUS ret;
	tmqueueprihwwrite_params_t params;

	USR2KRN( params, (void*)arg );

	ret = tmQueuePriHwWrite( params.stream, params.Qid, params.priority );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_tmqueueprihwread( unsigned int arg )
{
	OPL_STATUS ret;
	tmqueueprihwread_params_t params;

	USR2KRN( params, (void*)arg );

	ret = tmQueuePriHwRead( params.stream, params.Qid, &params.priority );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_tmqueueweighthwwrite( unsigned int arg )
{
	OPL_STATUS ret;
	tmqueueweighthwwrite_params_t params;

	USR2KRN( params, (void*)arg );

	ret = tmQueueWeightHwWrite( params.stream, params.Qid, params.weight );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_tmqueueweighthwread( unsigned int arg )
{
	OPL_STATUS ret;
	tmqueueweighthwread_params_t params;

	USR2KRN( params, (void*)arg );

	ret = tmQueueWeightHwRead( params.stream, params.Qid, &params.weight );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_tmgecirhwwrite( unsigned int arg )
{
	OPL_STATUS ret;
	tmgecirhwwrite_params_t params;

	USR2KRN( params, (void*)arg );

	ret = tmGeCirHwWrite( params.stream, params.geCir );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_tmgecirhwread( unsigned int arg )
{
	OPL_STATUS ret;
	tmgecirhwread_params_t params;

	USR2KRN( params, (void*)arg );

	ret = tmGeCirHwRead( params.stream, &params.geCir );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_tmgecbshwwrite( unsigned int arg )
{
	OPL_STATUS ret;
	tmgecbshwwrite_params_t params;

	USR2KRN( params, (void*)arg );

	ret = tmGeCbsHwWrite( params.stream, params.geCbs );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_tmgecbshwread( unsigned int arg )
{
	OPL_STATUS ret;
	tmgecbshwread_params_t params;

	USR2KRN( params, (void*)arg );

	ret = tmGeCbsHwRead( params.stream, &params.geCbs );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_tmcosicosmaphwwrite( unsigned int arg )
{
	OPL_STATUS ret;
	tmcosicosmaphwwrite_params_t params;

	USR2KRN( params, (void*)arg );

	ret = tmCosIcosMapHwWrite( params.stream, params.cos, params.icos );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_tmcosicosmaphwread( unsigned int arg )
{
	OPL_STATUS ret;
	tmcosicosmaphwread_params_t params;

	USR2KRN( params, (void*)arg );

	ret = tmCosIcosMapHwRead( params.stream, params.cos, &params.icos );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_tmqueuecbshwwrite( unsigned int arg )
{
	OPL_STATUS ret;
	tmqueuecbshwwrite_params_t params;

	USR2KRN( params, (void*)arg );

	ret = tmQueueCbsHwWrite( params.stream, params.Qid, params.cbs );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_tmqueuecbshwread( unsigned int arg )
{
	OPL_STATUS ret;
	tmqueuecbshwread_params_t params;

	USR2KRN( params, (void*)arg );

	ret = tmQueueCbsHwRead( params.stream, params.Qid, &params.cbs );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_tmqueuecirhwwrite( unsigned int arg )
{
	OPL_STATUS ret;
	tmqueuecirhwwrite_params_t params;

	USR2KRN( params, (void*)arg );

	ret = tmQueueCirHwWrite( params.stream, params.Qid, params.cir );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_tmqueuecirhwread( unsigned int arg )
{
	OPL_STATUS ret;
	tmqueuecirhwread_params_t params;

	USR2KRN( params, (void*)arg );

	ret = tmQueueCirHwRead( params.stream, params.Qid, &params.cir );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_tmwredcfgset( unsigned int arg )
{
	OPL_STATUS ret;
	tmwredcfgset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = tmWredCfgSet( params.stream, params.queueId, params.type, params.value );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_tmwredenablehwwrite( unsigned int arg )
{
	OPL_STATUS ret;
	tmwredenablehwwrite_params_t params;

	USR2KRN( params, (void*)arg );

	ret = tmWredEnableHwWrite( params.stream, params.queueId, params.enable );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_tmwredenablehwread( unsigned int arg )
{
	OPL_STATUS ret;
	tmwredenablehwread_params_t params;

	USR2KRN( params, (void*)arg );

	ret = tmWredEnableHwRead( params.stream, params.queueId, &params.enable );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_tmusshapertabdirecthwwrite( unsigned int arg )
{
	OPL_STATUS ret;
	tmusshapertabdirecthwwrite_params_t params;

	USR2KRN( params, (void*)arg );

	ret = tmUsShaperTabDirectHwWrite( params.entryId, params.cir, params.cbs );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_tmuswredtabdirecthwwrite( unsigned int arg )
{
	OPL_STATUS ret;
	tmuswredtabdirecthwwrite_params_t params;

	USR2KRN( params, (void*)arg );

	ret = tmUsWredTabDirectHwWrite( params.entryId, params.weight, params.minth, params.maxth );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_tmdswredtabdirecthwwrite( unsigned int arg )
{
	OPL_STATUS ret;
	tmdswredtabdirecthwwrite_params_t params;

	USR2KRN( params, (void*)arg );

	ret = tmDsWredTabDirectHwWrite( params.entryId, params.weight, params.minth, params.maxth );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_tmdsshapertabdirecthwwrite( unsigned int arg )
{
	OPL_STATUS ret;
	tmdsshapertabdirecthwwrite_params_t params;

	USR2KRN( params, (void*)arg );

	ret = tmDsShaperTabDirectHwWrite( params.entryId, params.cir, params.cbs );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_tmqueuemaxcellnumhwwrite( unsigned int arg )
{
	OPL_STATUS ret;
	tmqueuemaxcellnumhwwrite_params_t params;

	USR2KRN( params, (void*)arg );

	ret = tmQueueMaxCellNumHwWrite( params.stream, params.Qid, params.maxCellNum );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_tmwredcfgget( unsigned int arg )
{
	OPL_STATUS ret;
	tmwredcfgget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = tmWredCfgGet( params.stream, params.queueId, params.type, &params.value );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_tmMaxpUsWredTabDirectHwWrite( unsigned int arg )
{
	OPL_STATUS ret;
	tmMaxpUsWredTabDirectHwWrite_params_t params;

	USR2KRN( params, (void*)arg );

	ret = tmMaxpUsWredTabDirectHwWrite( params.entryId, params.c1, params.weight, params.minth, params.maxth);
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_tmMaxpDsWredTabDirectHwWrite( unsigned int arg )
{
	OPL_STATUS ret;
	tmMaxpDsWredTabDirectHwWrite_params_t params;

	USR2KRN( params, (void*)arg );

	ret = tmMaxpDsWredTabDirectHwWrite( params.entryId, params.c1, params.weight, params.minth, params.maxth);
	if ( ret )
	{
		return ret;
	}

	return ret;
}


static int ioctl_tmqueueshapercirenablehwwrite( unsigned int arg )
{
	OPL_STATUS ret;
	tmqueueshapercirenablehwwrite_params_t params;

	USR2KRN( params, (void*)arg );

	ret = tmQueueShaperCirEnableHwWrite( params.stream, params.Qid, params.enable);
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_tmusshapertabpirdirecthwwrite( unsigned int arg )
{
	OPL_STATUS ret;
	tmusshapertabpirdirecthwwrite_params_t params;

	USR2KRN( params, (void*)arg );

	ret = tmUsShaperTabPirDirectHwWrite( params.entryId, params.pir, params.pbs);
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_tmqueuemaxpktnumhwwrite( unsigned int arg )
{
	OPL_STATUS ret;
	tmqueuemaxpktNumhwwrite_params_t params;

	USR2KRN( params, (void*)arg );

	ret = tmQueueMaxPktNumHwWrite( params.stream, params.Qid, params.maxPktNum);
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_tmusshapertabcirdirecthwwrite( unsigned int arg )
{
	OPL_STATUS ret;
	tmusshapertabcirdirecthwwrite_params_t params;

	USR2KRN( params, (void*)arg );

	ret = tmUsShaperTabCirDirectHwWrite( params.entryId, params.cir, params.cbs);
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_tmusshapertabpirenablehwwrite( unsigned int arg )
{
	OPL_STATUS ret;
	tmqueueshaperpirenablehwwrite_params_t params;

	USR2KRN( params, (void*)arg );

	ret = tmQueueShaperPirEnableHwWrite( params.stream, params.Qid, params.enable);
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_tmcellnuminqueuehwread( unsigned int arg )
{
	OPL_STATUS ret;
	tmcellnuminqueuehwread_params_t params;

	USR2KRN( params, (void*)arg );

	ret = tmCellNumInQueueHwRead( params.stream, params.Qid, &params.numOfCell);
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_physpeedhwwrite( unsigned int arg )
{
	OPL_STATUS ret;

	ret = phySpeedHwWrite( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_physpeedhwread( unsigned int arg )
{
	OPL_STATUS ret;
	PHY_SPEED_MODE_e phySpeedMode;

	ret = phySpeedHwRead( &phySpeedMode );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, phySpeedMode );

	return ret;
}

static int ioctl_phyduplexhwwrite( unsigned int arg )
{
	OPL_STATUS ret;

	ret = phyDuplexHwWrite( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_phyduplexhwread( unsigned int arg )
{
	OPL_STATUS ret;
	PHY_DUPLEX_MODE_e phyDuplexMode;

	ret = phyDuplexHwRead( &phyDuplexMode );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, phyDuplexMode );

	return ret;
}

static int ioctl_phyautoneghwwrite( unsigned int arg )
{
	OPL_STATUS ret;

	ret = phyAutoNegHwWrite( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_phyautoneghwread( unsigned int arg )
{
	OPL_STATUS ret;
	UINT8 enable;

	USR2KRN( enable, (void*)arg );

	ret = phyAutoNegHwRead( &enable );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_phyrestartautoneg( unsigned int arg )
{
	OPL_STATUS ret;

	ret = phyRestartAutoNeg(  );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_phyloopbackenablehwwrite( unsigned int arg )
{
	OPL_STATUS ret;

	ret = phyLoopbackEnableHwWrite( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_phyloopbackenablehwread( unsigned int arg )
{
	OPL_STATUS ret;
	UINT8 enable;

	ret = phyLoopbackEnableHwRead( &enable );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, enable );

	return ret;
}

static int ioctl_phyreset( unsigned int arg )
{
	OPL_STATUS ret;

	ret = phyReset(  );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_phypowerdown( unsigned int arg )
{
	OPL_STATUS ret;

	ret = phyPowerDown( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_phylinkstatushwread( unsigned int arg )
{
	OPL_STATUS ret;
	UINT8 linkStatus;

	ret = phyLinkStatusHwRead( &linkStatus );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, linkStatus );

	return ret;
}

static int ioctl_phyflowcontrolenhwwrite( unsigned int arg )
{
	OPL_STATUS ret;

	ret = phyFlowControlEnHwWrite( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_physpecificautoread( unsigned int arg )
{
	OPL_STATUS ret;
	UINT32 phyAutonegDone;

	ret = phySpecificAutoRead( &phyAutonegDone );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, phyAutonegDone );

	return ret;
}

static int ioctl_physpecificspeedread( unsigned int arg )
{
	OPL_STATUS ret;
	PHY_SPEED_MODE_e phySpeed;

	ret = phySpecificSpeedRead( &phySpeed );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, phySpeed );

	return ret;
}

static int ioctl_physpecificduplexread( unsigned int arg )
{
	OPL_STATUS ret;
	PHY_DUPLEX_MODE_e phyDuplex;

	ret = phySpecificDuplexRead( &phyDuplex );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, phyDuplex );

	return ret;
}

static int ioctl_oplregread( unsigned int arg )
{
	OPL_STATUS ret;
	oplregread_params_t params;

	USR2KRN( params, (void*)arg );

	ret = oplRegRead( params.regAddr, &params.regVal );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_oplregwrite( unsigned int arg )
{
	OPL_STATUS ret;
	oplregwrite_params_t params;

	USR2KRN( params, (void*)arg );

	ret = oplRegWrite( params.regAddr, params.regVal );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_oplregfieldread( unsigned int arg )
{
	OPL_STATUS ret;
	oplregfieldread_params_t params;

	USR2KRN( params, (void*)arg );

	ret = oplRegFieldRead( params.regAddr, params.fieldOffset, params.fieldWidth, &params.data0 );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_oplregfieldwrite( unsigned int arg )
{
	OPL_STATUS ret;
	oplregfieldwrite_params_t params;

	USR2KRN( params, (void*)arg );

	ret = oplRegFieldWrite( params.regAddr, params.fieldOffset, params.fieldWidth, params.data0 );
	if ( ret )
	{
		return ret;
	}

	return ret;
}
static int ioctl_opltabread( unsigned int arg )
{
	OPL_STATUS ret;
	opltabread_params_t params;

	USR2KRN( params, (void*)arg );

	ret = oplTabRead( params.region, params.startId, params.num, params.pVal );
	if ( ret )
	{
		return ret;
	}

	//KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_opltabwrite( unsigned int arg )
{
	OPL_STATUS ret;
	opltabwrite_params_t params;

	USR2KRN( params, (void*)arg );

	ret = oplTabWrite( params.region, params.startId, params.num, params.pVal );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_mdioregisterread( unsigned int arg )
{
	OPL_STATUS ret;
	mdioregisterread_params_t params;

	USR2KRN( params, (void*)arg );

	ret = mdioRegisterRead( params.deviceAddr, params.regAddr, &params.data0 );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_mdioregisterwrite( unsigned int arg )
{
	OPL_STATUS ret;
	mdioregisterwrite_params_t params;

	USR2KRN( params, (void*)arg );

	ret = mdioRegisterWrite( params.deviceAddr, params.regAddr, params.data0 );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_mdioregisterfieldread( unsigned int arg )
{
	OPL_STATUS ret;
	mdioregisterfieldread_params_t params;

	USR2KRN( params, (void*)arg );

	ret = mdioRegisterFieldRead( params.deviceAddr, params.regAddr, params.offset, params.width, &params.data0 );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_mdioregisterfieldwrite( unsigned int arg )
{
	OPL_STATUS ret;
	mdioregisterfieldwrite_params_t params;

	USR2KRN( params, (void*)arg );

	ret = mdioRegisterFieldWrite( params.deviceAddr, params.regAddr, params.offset, params.width, params.data0 );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_mdioregisterinit( unsigned int arg )
{
	OPL_STATUS ret;

	ret = mdioRegisterInit(  );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_chipreset( unsigned int arg )
{
	OPL_STATUS ret;

	ret = chipReset(  );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_chipattrget( unsigned int arg )
{
	OPL_STATUS ret;
	chipattrget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = chipAttrGet( params.type, &params.value );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_chipinit( unsigned int arg )
{
	OPL_STATUS ret;

	ret = chipInit(  );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_linklistenable( unsigned int arg )
{
	OPL_STATUS ret;

	ret = linkListEnable(  );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_respininit( unsigned int arg )
{
	OPL_STATUS ret;

	ret = respinInit(  );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_gmacinit( unsigned int arg )
{
	OPL_STATUS ret;

	ret = gmacInit(  );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_pmacinit( unsigned int arg )
{
	OPL_STATUS ret;

	ret = pmacInit(  );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclsdelentry( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalClsDelEntry( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}


static int ioctl_dalvttvlanaggdel( unsigned int arg )
{
	OPL_STATUS ret;
	dalvttvlanaggdel_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalVttVlanAggDel( params.egressPort, params.targetVlan, params.aggedVlan, params.aggedMac );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalvttvlanaggadd( unsigned int arg )
{
	OPL_STATUS ret;
	dalvttvlanaggadd_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalVttVlanAggAdd( params.portId, params.sVlan, params.cVlan, params.aggedMac, &params.ruleId );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_brgrsvmacctrlhwread( unsigned int arg )
{
	OPL_STATUS ret;
	brgrsvmacctrlhwread_params_t params;

	USR2KRN( params, (void*)arg );

	ret = brgRsvMacCtrlHwRead( params.macNum, &params.icos, &params.macCtl );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_brgrsvmacctrlhwwrite( unsigned int arg )
{
	OPL_STATUS ret;
	brgrsvmacctrlhwwrite_params_t params;

	USR2KRN( params, (void*)arg );

	ret = brgRsvMacCtrlHwWrite( params.macNum, params.icos, params.macCtl );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalmulticastenable( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalMulticastEnable(  );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalmulticastdisable( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalMulticastDisable(  );
	if ( ret )
	{
		return ret;
	}

	return ret;
}


static int ioctl_i2cinit( unsigned int arg )
{
	int ret;
	i2cinit_params_t params;

	USR2KRN( params, (void*)arg );

	ret = i2cInit( params.i2cMode, params.intMode, params.slvAddr, params.nRate );
	return ret;
}

static int ioctl_i2creadrandom( unsigned int arg )
{
	int ret;
	i2creadrandom_params_t params;

	USR2KRN( params, (void*)arg );

	ret = i2cReadRandom( params.slvAddr, params.staRegAddr, params.nByte, params.pDataBuf );

	KRN2USR((void*)arg, params);
	
	return ret;
}

static int ioctl_dalonuclsrulectcadd( unsigned int arg )
{
	OPL_STATUS ret;
	opl_acl_list_t oamcls;

	USR2KRN( oamcls, (void*)arg );

	ret = dalOnuClsRuleCtcAdd( &oamcls );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalonuclsrulectcchange( unsigned int arg )
{
	OPL_STATUS ret;
	dalonuclsrulectcchange_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalOnuClsRuleCtcChange( params.oamcls, params.newmask );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalonuclsrulectcdel( unsigned int arg )
{
	OPL_STATUS ret;
	opl_acl_list_t oamcls;

	USR2KRN( oamcls, (void*)arg );

	ret = dalOnuClsRuleCtcDel( &oamcls );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalvoipclsdelentry( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalVoipClsDelEntry( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclacfgcheck( unsigned int arg )
{
	OPL_STATUS ret;
	dalclacfgcheck_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClaCfgCheck( params.stClassCfg, &params.ruleId );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalportpvidset( unsigned int arg )
{
	OPL_STATUS ret;
	dalportpvidset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPortPvidSet( params.portId, params.pvid );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalportpvidget( unsigned int arg )
{
	OPL_STATUS ret;
	dalportpvidget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPortPvidGet( params.portId, &params.pvid );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalportvlanmodeset( unsigned int arg )
{
	OPL_STATUS ret;
	dalportvlanmodeset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPortVlanModeSet( params.portId, params.mode );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalportvlanmodeget( unsigned int arg )
{
	OPL_STATUS ret;
	dalportvlanmodeget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPortVlanModeGet( params.portId, &params.mode );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalportdefaultpriset( unsigned int arg )
{
	OPL_STATUS ret;
	dalportdefaultpriset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPortDefaultPriSet( params.portId, params.defaultPri );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_daltrunkentryadd( unsigned int arg )
{
	OPL_STATUS ret;
	daltrunkentryadd_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalTrunkEntryAdd( params.portId, params.vlanId );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_daltrunkentrydel( unsigned int arg )
{
	OPL_STATUS ret;
	daltrunkentrydel_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalTrunkEntryDel( params.portId, params.vlanId );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_daltrunkentryvalueget( unsigned int arg )
{
	OPL_STATUS ret;
	daltrunkentryvalueget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalTrunkEntryValueGet( params.portId,params.entryIndex, &params.pVlanId );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_daltrunkentrynumget( unsigned int arg )
{
	OPL_STATUS ret;
	daltrunkentrynumget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalTrunkEntryNumGet( params.portId, &params.pNum );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_daltrunkentryclear( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalTrunkEntryClear( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalcreatevlanid( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalCreateVlanID( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalflushvlanall( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalFlushVlanAll(  );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalsetlocalswitch( unsigned int arg )
{
	OPL_STATUS ret;
	dalsetlocalswitch_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalSetLocalSwitch( params.lport, params.enable );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalclsrmkactget( unsigned int arg )
{
	OPL_STATUS ret;
	dalclsrmkactget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalClsRmkActGet( params.classItemID, params.pktType, &params.rmkActVal );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalponinit( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalPonInit(  );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_daldbaqueuethresholdset( unsigned int arg )
{
	OPL_STATUS ret;
	daldbaqueuethresholdset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalDbaQueueThresholdSet( params.qSetNum, params.qNum, params.thresholdVal );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_daldbaqueuethresholdget( unsigned int arg )
{
	OPL_STATUS ret;
	daldbaqueuethresholdget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalDbaQueueThresholdGet( params.qSetNum, params.qNum ,&params.thresholdVal );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_daldbareportmodeset( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalDbaReportModeSet( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_daldbareportmodeget( unsigned int arg )
{
	OPL_STATUS ret;
	UINT32 mode;

	ret = dalDbaReportModeGet( &mode );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, mode );

	return ret;
}

static int ioctl_dalonumtuset( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalOnuMtuSet( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalonumtuget( unsigned int arg )
{
	OPL_STATUS ret;
	UINT32 mtuSize;

	ret = dalOnuMtuGet( &mtuSize );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, mtuSize );

	return ret;
}

static int ioctl_dalonuponloopbackconfig( unsigned int arg )
{
	OPL_STATUS ret;
	dalonuponloopbackconfig_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalOnuPonLoopbackConfig( params.uniorscb, params.enable );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalonupontxpolarityconfig( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalOnuPonTxPolarityConfig( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_brgusrdefinersvmachwwrite( unsigned int arg )
{
	OPL_STATUS ret;
	brgusrdefinersvmachwwrite_params_t params;

	USR2KRN( params, (void*)arg );

	ret = brgUsrDefineRsvMacHwWrite( params.num, params.mac );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_brgusrdefinersvmachwread( unsigned int arg )
{
	OPL_STATUS ret;
	brgusrdefinersvmachwread_params_t params;

	USR2KRN( params, (void*)arg );

	ret = brgUsrDefineRsvMacHwRead( params.num, params.mac );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_brgusrdefinersvmacctrlhwwrite( unsigned int arg )
{
	OPL_STATUS ret;
	brgusrdefinersvmacctrlhwwrite_params_t params;

	USR2KRN( params, (void*)arg );

	ret = brgUsrDefineRsvMacCtrlHwWrite( params.macNum, params.icos, params.macCtl );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_brgusrdefinersvmacctrlhwread( unsigned int arg )
{
	OPL_STATUS ret;
	brgusrdefinersvmacctrlhwread_params_t params;

	USR2KRN( params, (void*)arg );

	ret = brgUsrDefineRsvMacCtrlHwRead( params.macNum, &params.icos, &params.macCtl );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalponportstatisticsget1( unsigned int arg )
{
	OPL_STATUS ret;
	ONU_PON_Port_Statistics_Get_1_t ponStats;

	ret = dalPONPortStatisticsGet1( &ponStats );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, ponStats );

	return ret;
}

static int ioctl_dalponportstatisticsget2( unsigned int arg )
{
	OPL_STATUS ret;
	ONU_PON_Port_Statistics_Get_2_t ponStats;

	ret = dalPONPortStatisticsGet2( &ponStats );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, ponStats );

	return ret;
}

static int ioctl_dalportfluxstatisticscounterget( unsigned int arg )
{
	OPL_STATUS ret;
	ONU_Port_Flux_Statistics_Counter_t portStats;

	ret = dalPortFluxStatisticsCounterGet( &portStats );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, portStats );

	return ret;
}

static int ioctl_dalcounterget( unsigned int arg )
{
	OPL_STATUS ret = OPL_OK;
	dalcounterget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalCounterGet( params.moduleBit, params.oplCounter, params.start, params.num );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalcounterenable( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalCounterEnable( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalcounterenableget( unsigned int arg )
{
	OPL_STATUS ret;
	unsigned int moduleBitmap;

	ret = dalCounterEnableGet( &moduleBitmap );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, moduleBitmap );

	return ret;
}

static int ioctl_dalcounterclear( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalCounterClear( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}
static int ioctl_dalvlanqinqmodeset( unsigned int arg )
{
	OPL_STATUS ret;
	printk("%s %d \n",__FUNCTION__,__LINE__);
	ret = dalVlanQinQModeSet( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalvlanqinqbaseadd( unsigned int arg )
{
	OPL_STATUS ret;
	dalvlanqinqbaseadd_params_t params;

	USR2KRN( params, (void*)arg );	

	ret = dalVlanQinQBaseAdd( params.vlan, params.remarkPri, params.insertPri);
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalvlanqinqbasedel( unsigned int arg )
{
	OPL_STATUS ret;
	dalvlanqinqbasedel_params_t params;

	USR2KRN( params, (void*)arg );	

	ret = dalVlanQinQBaseDel( params.vlan, params.remarkPri, params.insertPri );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalvlantransparentadd( unsigned int arg )
{
	OPL_STATUS ret;
	printk("%s %d \n",__FUNCTION__,__LINE__);

	ret = dalVlanTransparentAdd( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalvlantransparentdel( unsigned int arg )
{
	OPL_STATUS ret;
	printk("%s %d \n",__FUNCTION__,__LINE__);
	ret = dalVlanTransparentDel( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalvlanqinqflexibleadd( unsigned int arg )
{
	OPL_STATUS ret;
	dalvlanqinqflexibleadd_params_t params;

	USR2KRN( params, (void*)arg );
	printk("%s %d cvlan =%d svlan = %d spriority=%d\n",__FUNCTION__,__LINE__, params.cvlan, params.svlan,params.sPriority);

	ret = dalVlanQinQFlexibleAdd( params.svlan, params.cvlan, params.sPriority);
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalvlanqinqflexibledel( unsigned int arg )
{
	OPL_STATUS ret;
	dalvlanqinqflexibledel_params_t params;

	USR2KRN( params, (void*)arg );
	printk("%s %d cvlan =%d svlan = %d  spriority=%d \n",__FUNCTION__,__LINE__, params.cvlan, params.svlan,params.sPriority);

	ret = dalVlanQinQFlexibleDel( params.svlan, params.cvlan, params.sPriority );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

#if defined(ONU_4PORT_AR8327)
static int ioctl_dalvttnumexistget( unsigned int arg )
{
	OPL_STATUS ret;
	unsigned int num;

	ret = dalVttNumExistGet( &num );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, num );

	return ret;
}

static int ioctl_dalarldiscardmacadd( unsigned int arg )
{
	OPL_STATUS ret;
	dalarldiscardmacadd_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalArlDiscardMacAdd( params.mac );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalarldiscardmacdel( unsigned int arg )
{
	OPL_STATUS ret;
	dalarldiscardmacdel_params_t params;
	
	USR2KRN( params, (void*)arg );
	
	ret = dalArlDiscardMacDel( params.mac );
	if ( ret )
	{
		return ret;
	}
	
	return ret;
}

static int ioctl_dalaclrstpadd( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalAclRstpAdd( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalmulticastvlancreate( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalMulticastVlanCreate( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalmulticastvlandelete( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalMulticastVlanDelete( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalaclclsfindname( unsigned int arg )
{
	OPL_STATUS ret;
	dalaclclsfindname_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalAclClsFindName( params.stClassCfg, &params.name );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalarlflushallbyport( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalArlFlushAllByPort( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalportlearnenableset( unsigned int arg )
{
	OPL_STATUS ret;
	dalportlearnenableset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalPortLearnEnableSet( params.portId, params.enable );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalarlmacmove( unsigned int arg )
{
	OPL_STATUS ret;
	dalarlmacmove_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalArlMacMove( params.port, params.toPort );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalarlmacfilteradd( unsigned int arg )
{
	OPL_STATUS ret;
	dalarlmacfilteradd_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalArlMacFilterAdd( params.portId, params.vlanId, params.macAddress );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalarlmacfilterdel( unsigned int arg )
{
	OPL_STATUS ret;
	dalarlmacfilterdel_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalArlMacFilterDel( params.portId, params.vlanId, params.macAddress );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalarlmacdlffilteren( unsigned int arg )
{
	OPL_STATUS ret;
	dalarlmacdlffilteren_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalArlMacDlfFilterEn( params.portId, params.enable );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalarlmacportlockdropen( unsigned int arg )
{
	OPL_STATUS ret;
	dalarlmacportlockdropen_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalArlMacPortLockDropEn( params.portId, params.enable );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalmirportset( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalMirPortSet( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalmirportget( unsigned int arg )
{
	OPL_STATUS ret;
	UINT8 portId;

	ret = dalMirPortGet( &portId );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, portId );

	return ret;
}

static int ioctl_dalmiringportset( unsigned int arg )
{
	OPL_STATUS ret;
	dalmiringportset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalMirIngPortSet( params.portId, params.enable );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalmiringportget( unsigned int arg )
{
	OPL_STATUS ret;
	dalmiringportget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalMirIngPortGet( params.portId, &params.enable );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_dalmiregportset( unsigned int arg )
{
	OPL_STATUS ret;
	dalmiregportset_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalMirEgPortSet( params.portId, params.enable );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalmiregportget( unsigned int arg )
{
	OPL_STATUS ret;
	dalmiregportget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalMirEgPortGet( params.portId, &params.enable );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_daldeletevlanid( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalDeleteVlanID( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_daladdvlanport( unsigned int arg )
{
	OPL_STATUS ret;
	daladdvlanport_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalAddVlanPort( params.portId, params.vid, params.type );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalmovevlanport( unsigned int arg )
{
	OPL_STATUS ret;
	dalmovevlanport_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalMoveVlanPort( params.portId, params.vid );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalmibget( unsigned int arg )
{
	OPL_STATUS ret;
	dalmibget_params_t params;

	USR2KRN( params, (void*)arg );

	ret = dalMibGet( params.portId, params.mib );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_isis_phy_get( unsigned int arg )
{
	OPL_STATUS ret;
	isis_phy_get_params_t params;

	USR2KRN( params, (void*)arg );

	ret = isis_phy_get( params.dev_id, params.phy_addr, params.reg, &params.value );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_isis_phy_set( unsigned int arg )
{
	OPL_STATUS ret;
	isis_phy_set_params_t params;

	USR2KRN( params, (void*)arg );

	ret = isis_phy_set( params.dev_id, params.phy_addr, params.reg, params.value );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_isis_reg_get( unsigned int arg )
{
	OPL_STATUS ret;
	isis_reg_get_params_t params;

	USR2KRN( params, (void*)arg );

	ret = isis_reg_get( params.dev_id, params.reg_addr, params.value, params.value_len );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_isis_reg_set( unsigned int arg )
{
	OPL_STATUS ret;
	isis_reg_set_params_t params;

	USR2KRN( params, (void*)arg );

	ret = isis_reg_set( params.dev_id, params.reg_addr, params.value, params.value_len );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_isis_reg_field_get( unsigned int arg )
{
	OPL_STATUS ret;
	isis_reg_field_get_params_t params;

	USR2KRN( params, (void*)arg );

	ret = isis_reg_field_get( params.dev_id, params.reg_addr, params.bit_offset, params.field_len, params.value, params.value_len );
	if ( ret )
	{
		return ret;
	}

	KRN2USR( (void*)arg, params );

	return ret;
}

static int ioctl_isis_reg_field_set( unsigned int arg )
{
	OPL_STATUS ret;
	isis_reg_field_set_params_t params;

	USR2KRN( params, (void*)arg );

	ret = isis_reg_field_set( params.dev_id, params.reg_addr, params.bit_offset, params.field_len, params.value, params.value_len );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_isis_acl_rule_dump( unsigned int arg )
{
	OPL_STATUS ret;

	ret = isis_acl_rule_dump( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_isis_acl_list_dump( unsigned int arg )
{
	OPL_STATUS ret;

	ret = isis_acl_list_dump( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_atheros_show_cnt( unsigned int arg )
{
	OPL_STATUS ret;

	ret = atheros_show_cnt( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalArVttShow( unsigned int arg )
{
	OPL_STATUS ret;

	ret = dalArVttShow( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}

static int ioctl_dalVlanShow( unsigned int arg )
{
	OPL_STATUS ret;
	unsigned int num;

	ret = dalVlanShow( arg );
	if ( ret )
	{
		return ret;
	}

	return ret;
}


#endif
static const ioctlfunc dal_pon_expo_calltable[] = {
	ioctl_dalponmacidset,
	ioctl_dalponmacidget,
	ioctl_dalponphyfecmodeset,
	ioctl_dalponphyfecmodeget,
	ioctl_dalponphytriplechuningmodeset,
	ioctl_dalponphytriplechuningmodeget,
	ioctl_dalponphyaesmodeset,
	ioctl_dalponphyaesmodeget,
	ioctl_dalponlaseroncntset,
	ioctl_dalponlaseroncntget,
	ioctl_dalponlaseroffcntset,
	ioctl_dalponlaseroffcntget,
	ioctl_dalponsynctimeget,
	ioctl_dalponllidget,
	ioctl_dalponserdesdelaycntset,
	ioctl_dalponserdesdelaycntget,
	ioctl_dalponsecuritymodeset,
	ioctl_dalponsecuritymodeget,
	ioctl_dalponsecurityaesmodeset,
	ioctl_dalponsecurityaesmodeget,
	ioctl_dalponunicasttriplechurningkeyget,
	ioctl_dalponscbtriplechurningkeyset,
	ioctl_dalponunicastaeskeyget,
	ioctl_dalponscbaeskeyset,
	ioctl_dalpontriplechurningmodeget,
	ioctl_dalpontriplechurningmodeset,
	ioctl_dalponmpcpdbaagentconfigallset,
	ioctl_dalponmpcpdbaagentconfigallget,
	ioctl_dalpondelayconfigallset,
	ioctl_dalpondelayconfigallget,
	ioctl_dalponmpcpstatusget,
	ioctl_dalponoamfinishedset,
	ioctl_dalponmpcpfsmholdoverctrlset,
	ioctl_dalponmpcpfsmholdoverctrlget,
	ioctl_dalponmpcpfsmholdovertimeset,
	#ifdef CONFIG_BOSA
	ioctl_dalponmpcpfsmholdovertimeget,
	ioctl_dalpon25l90imodset,
	ioctl_dalpon25l90Apcset	
	#else
    ioctl_dalponmpcpfsmholdovertimeget
    #endif
};
static const ioctlfunc dal_cls_expo_calltable[] = {
	ioctl_dalclstblentryadd,
	ioctl_dalclstblentrydel,
	ioctl_dalclsmasktblwr,
	ioctl_dalclsruletblwr,
	ioctl_dalclsaddrtblwr,
	ioctl_dalclsmaskhwwr,
	ioctl_dalclsrulehwwr,
	ioctl_dalclsaddrhwwr,
	ioctl_dalclsruletblrd,
	ioctl_dalclsaddrtblrd,
	ioctl_dalclsentryset,
	ioctl_dalclsentrymove,
	ioctl_dalclsinit,
	ioctl_dalclstranactset,
	ioctl_dalclsrmkactset,
	ioctl_dalclsrmkcosset,
	ioctl_dalclsrmkicosset,
	ioctl_dalclsrmkvidset,
	ioctl_dalclsegressportcfg,
	ioctl_dalclsingressportcfg,
	ioctl_dalclstranactcfg,
	ioctl_dalclsrmkactcfg,
	ioctl_dalclsrmkcoscfg,
	ioctl_dalclsrmkicoscfg,
	ioctl_dalclsrmkvidcfg,
	ioctl_dalclsmaskpriocfg,
	ioctl_dalclsdestmacaddrcfg,
	ioctl_dalclssrcmacaddrcfg,
	ioctl_dalclsfirstcoscfg,
	ioctl_dalclsfirstvidcfg,
	ioctl_dalclstypelencfg,
	ioctl_dalclsdestipcfg,
	ioctl_dalclssrcipcfg,
	ioctl_dalclsiptypecfg,
	ioctl_dalclsiptoscfg,
	ioctl_dalclsl4destportcfg,
	ioctl_dalclsl4srcportcfg,
	ioctl_dalclsruledel,
	ioctl_dalclsconvertpara,
	ioctl_dalclsctcinit,
	ioctl_dalclsrulectcadd,
	ioctl_dalclsrulectcdel,
	ioctl_dalclsrulectcclear,
	ioctl_dalexclsdelentry,
	ioctl_dalclsrmkvidactset,
	ioctl_dalclsipversioncfg,
	ioctl_dalclsfirsttpidcfg,
	ioctl_dalvoipclsrulectcadd,	
	ioctl_dalClsValidEntryGet/* Added of qinq */
};
static const ioctlfunc dal_lib_expo_calltable[] = {
	ioctl_dalchipreset,
	ioctl_daldemoninit
};
static const ioctlfunc dal_mac_expo_calltable[] = {
	ioctl_dalarlagetimeset,
	ioctl_dalarlagetimeget,
	ioctl_dalarlmacadd,
	ioctl_dalarlmacdel,
	ioctl_dalarlmacremove,
	ioctl_dalarlflushdynamic,
	ioctl_dalarlflushall,
	ioctl_dalarllearnenset,
	ioctl_dalarllearnenget,
	ioctl_dalarlsoftlearnenset,
	ioctl_dalarlsoftlearnenget,
	ioctl_dalarlageenableset,
	ioctl_dalarlageenableget,
	ioctl_dalarlmacnumlimitset,
	ioctl_dalarlmultiportmacadd,
	ioctl_dalarlmultiportmacdel,
	ioctl_dalarlmacentryfind,
	ioctl_dalarlmacentrytraverse,
	ioctl_dalarlmacentryshowone,
	ioctl_dalarlmacentryshowall
};
static const ioctlfunc dal_multicast_expo_calltable[] = {
	ioctl_dalmulticastinit,
	ioctl_dalmulticastvlanaddport,
	ioctl_dalmulticastvlandelport,
	ioctl_dalmulticastvlantagstripe,
	ioctl_dalmulticastarladdport,
	ioctl_dalmulticastarlportdel,
	ioctl_dalmulticastarlvlanadd,
	ioctl_dalmulticastarlvlandel,
	ioctl_dalmulticastarltabflush,
	ioctl_dalmulticastportvlanmacadd,
	ioctl_dalmulticastportvlanmacdel,
	ioctl_dalmulticastportvlanipadd,
	ioctl_dalmulticastportvlanipdel,
	ioctl_dalmulticastruledelall,
	ioctl_dalmulticastdatafilteradd,
	ioctl_dalmulticastdatafilterdel,
	ioctl_dalmulticastnumofruleget
};
static const ioctlfunc dal_port_expo_calltable[] = {
	ioctl_dalportnumisinvalid,
	ioctl_dalportspecificduplexread,
	ioctl_dalphyspecificautonegread,
	ioctl_dalportspecificspeedread,
	ioctl_dalportstateget,
	ioctl_dalportspeedset,
	ioctl_dalportspeedget,
	ioctl_dalportduplexset,
	ioctl_dalportduplexget,
	ioctl_dalportflowcontrolset,
	ioctl_dalportflowcontrolget,
	ioctl_dalportloopbackset,
	ioctl_dalportuspolicingenableset,
	ioctl_dalportuspolicingcirset,
	ioctl_dalportuspolicingcirget,
	ioctl_dalportuspolicingcbsset,
	ioctl_dalportuspolicingcbsget,
	ioctl_dalportuspolicingebsset,
	ioctl_dalportuspolicingebsget,
	ioctl_dalportuspolicingset,
	ioctl_dalportuspolicingget,
	ioctl_dalportdspolicingenableset,
	ioctl_dalportdspolicingenableget,
	ioctl_dalportdspolicingcirset,
	ioctl_dalportdspolicingcirget,
	ioctl_dalportdspolicingpirset,
	ioctl_dalportdspolicingpirget,
	ioctl_dalportdspolicingset,
	ioctl_dalportdspolicingget,
	ioctl_dalphyadminstateget,
	ioctl_dalphyadmincontrolset,
	ioctl_dalphyautonegadminstateget,
	ioctl_dalphyautonegenableset,
	ioctl_dalphyautonegrestart,
	ioctl_dalportinit
};
static const ioctlfunc dal_rstp_expo_calltable[] = {
	ioctl_dalrstpmodeset,
	ioctl_dalrstpportstateget,
	ioctl_dalrstpportstateset,
	ioctl_dalrstpportmacflush
};
static const ioctlfunc dal_storm_expo_calltable[] = {
	ioctl_dalstormclsruleadd,
	ioctl_dalstormclsruledel,
	ioctl_dalstormctrlframeset,
	ioctl_dalstormctrlframeget,
	ioctl_dalstormctrlrateset,
	ioctl_dalstormctrlrateget
};
static const ioctlfunc dal_ver_expo_calltable[] = {
	ioctl_oplverstrget,
	ioctl_oplvershow
};
static const ioctlfunc dal_vtt_expo_calltable[] = {
	ioctl_dalvttmodeset,
	ioctl_dalvttmodeget,
	ioctl_dalvtttpidset,
	ioctl_dalvtttpidget,
	ioctl_dalvttvidset,
	ioctl_dalvttvidget,
	ioctl_dalvttdefaultpriset,
	ioctl_dalvttdefaultpriget,
	ioctl_dalvtttagentryadd,
	ioctl_dalvtttagentryget,
	ioctl_dalvtttagentrydel,
	ioctl_dalvttnumofrecordget,
	ioctl_dalvttmulticastentryadd,
	ioctl_dalvttmulticastentrydel,
	ioctl_dalvttmulticaststripenable,
	ioctl_dalvttmulticastinit,
	ioctl_dalvttmulticastnumofrecordget,
	ioctl_dalvttinit,
	/* Begin Added of qinq */
	ioctl_QinqPvidSet,
	ioctl_QinqPvidReSet,
	ioctl_VttEntryTagAdd
	/* End   Added of qinq */	
};

static const ioctlfunc phy_expo_calltable[] = {
	ioctl_physpeedhwwrite,
	ioctl_physpeedhwread,
	ioctl_phyduplexhwwrite,
	ioctl_phyduplexhwread,
	ioctl_phyautoneghwwrite,
	ioctl_phyautoneghwread,
	ioctl_phyrestartautoneg,
	ioctl_phyloopbackenablehwwrite,
	ioctl_phyloopbackenablehwread,
	ioctl_phyreset,
	ioctl_phypowerdown,
	ioctl_phylinkstatushwread,
	ioctl_phyflowcontrolenhwwrite,
	ioctl_physpecificautoread,
	ioctl_physpecificspeedread,
	ioctl_physpecificduplexread
};

static const ioctlfunc register_expo_calltable[] = {
	ioctl_oplregread,
	ioctl_oplregwrite,
	ioctl_oplregfieldread,
	ioctl_oplregfieldwrite,
	ioctl_opltabread,
	ioctl_opltabwrite
};

static const ioctlfunc mdio_expo_calltable[] = {	
	ioctl_mdioregisterread,
	ioctl_mdioregisterwrite,
	ioctl_mdioregisterfieldread,
	ioctl_mdioregisterfieldwrite,
	ioctl_mdioregisterinit,
	ioctl_swmdioglbregisterread,
	ioctl_swmdioglbregisterwrite,
	ioctl_swmdiophyregisterread,
	ioctl_swmdiophyregisterwrite
};	

static const ioctlfunc tm_expo_calltable[] = {
	ioctl_tmgeshaperenablehwwrite,
	ioctl_tmgeshaperenablehwread,
	ioctl_tmqueueshaperenablehwwrite,
	ioctl_tmqueueshaperenablehwread,
	ioctl_tmqueueprihwwrite,
	ioctl_tmqueueprihwread,
	ioctl_tmqueueweighthwwrite,
	ioctl_tmqueueweighthwread,
	ioctl_tmgecirhwwrite,
	ioctl_tmgecirhwread,
	ioctl_tmgecbshwwrite,
	ioctl_tmgecbshwread,
	ioctl_tmcosicosmaphwwrite,
	ioctl_tmcosicosmaphwread,
	ioctl_tmqueuecbshwwrite,
	ioctl_tmqueuecbshwread,
	ioctl_tmqueuecirhwwrite,
	ioctl_tmqueuecirhwread,
	ioctl_tmwredcfgset,
	ioctl_tmwredenablehwwrite,
	ioctl_tmwredenablehwread,
	ioctl_tmusshapertabdirecthwwrite,
	ioctl_tmuswredtabdirecthwwrite,
	ioctl_tmdswredtabdirecthwwrite,
	ioctl_tmdsshapertabdirecthwwrite,
	ioctl_tmqueuemaxcellnumhwwrite,
	ioctl_tmwredcfgget,
	ioctl_tmMaxpUsWredTabDirectHwWrite,
    ioctl_tmMaxpDsWredTabDirectHwWrite,
	ioctl_tmqueueshapercirenablehwwrite,
	ioctl_tmusshapertabpirdirecthwwrite,
	ioctl_tmqueuemaxpktnumhwwrite,		
	ioctl_tmusshapertabcirdirecthwwrite,		
	ioctl_tmusshapertabpirenablehwwrite,
	ioctl_tmcellnuminqueuehwread
};

static const ioctlfunc opconnmisc_expo_calltable[] = {
	ioctl_chipreset,
	ioctl_chipattrget,
	ioctl_chipinit,
	ioctl_linklistenable,
	ioctl_respininit,
	ioctl_gmacinit,
	ioctl_pmacinit
};

static const ioctlfunc comm_expo_calltable[] = {
	ioctl_dalclsdelentry,
	ioctl_dalvttvlanaggdel,
	ioctl_dalvttvlanaggadd,
	ioctl_brgrsvmacctrlhwread,
	ioctl_brgrsvmacctrlhwwrite,
	ioctl_dalmulticastenable,
	ioctl_dalmulticastdisable,
	ioctl_i2cinit,
	ioctl_i2creadrandom,
	ioctl_dalonuclsrulectcadd,
	ioctl_dalonuclsrulectcchange,
	ioctl_dalonuclsrulectcdel,
	ioctl_dalvoipclsdelentry,
	ioctl_dalclacfgcheck,
	ioctl_dalportpvidset,
	ioctl_dalportpvidget,
	ioctl_dalportvlanmodeset,
	ioctl_dalportvlanmodeget,
	ioctl_dalportdefaultpriset,
	ioctl_daltrunkentryadd,
	ioctl_daltrunkentrydel,
	ioctl_daltrunkentryvalueget,
	ioctl_daltrunkentrynumget,
	ioctl_daltrunkentryclear,
	ioctl_dalcreatevlanid,
	ioctl_dalflushvlanall,
	ioctl_dalsetlocalswitch,
	ioctl_dalclsrmkactget,
	ioctl_dalponinit,
	ioctl_daldbaqueuethresholdset,
	ioctl_daldbaqueuethresholdget,
	ioctl_daldbareportmodeset,
	ioctl_daldbareportmodeget,
	ioctl_dalonumtuset,
	ioctl_dalonumtuget,
	ioctl_dalonuponloopbackconfig,
	ioctl_dalonupontxpolarityconfig,
	ioctl_brgusrdefinersvmachwwrite,
	ioctl_brgusrdefinersvmachwread,
	ioctl_brgusrdefinersvmacctrlhwwrite,
	ioctl_brgusrdefinersvmacctrlhwread,
	ioctl_dalclsruleentryshow,
	ioctl_dalclsmaskentryshow,
	ioctl_dalclsaddrentryshow,
	ioctl_dalclscfgshow
};

static const ioctlfunc dal_stats_expo_calltable[] = {
	ioctl_dalponportstatisticsget1,
	ioctl_dalponportstatisticsget2,
	ioctl_dalportfluxstatisticscounterget,
	ioctl_dalcounterget,
	ioctl_dalcounterenable,
	ioctl_dalcounterenableget,
	ioctl_dalcounterclear
};

static const ioctlfunc atheros8327_expo_calltable[] = {
	ioctl_dalvlanqinqmodeset,
	ioctl_dalvlanqinqbaseadd,
	ioctl_dalvlanqinqbasedel,
	ioctl_dalvlantransparentadd,
	ioctl_dalvlantransparentdel,
	ioctl_dalvlanqinqflexibleadd,
	ioctl_dalvlanqinqflexibledel,
#if defined(ONU_4PORT_AR8327)	
	ioctl_dalarldiscardmacadd,
	ioctl_dalarldiscardmacdel,
	ioctl_dalaclrstpadd,
	ioctl_dalmulticastvlancreate,
	ioctl_dalmulticastvlandelete,
	ioctl_dalaclclsfindname,
	ioctl_dalarlflushallbyport,
	ioctl_dalportlearnenableset,
	ioctl_dalarlmacmove,
	ioctl_dalarlmacfilteradd,
	ioctl_dalarlmacfilterdel,
	ioctl_dalarlmacdlffilteren,
	ioctl_dalarlmacportlockdropen,
	ioctl_dalmirportset,
	ioctl_dalmirportget,
	ioctl_dalmiringportset,
	ioctl_dalmiringportget,
	ioctl_dalmiregportset,
	ioctl_dalmiregportget,
	ioctl_daldeletevlanid,
	ioctl_daladdvlanport,
	ioctl_dalmovevlanport,
	ioctl_dalmibget,
	ioctl_isis_phy_get,
	ioctl_isis_phy_set,
	ioctl_isis_reg_get,
	ioctl_isis_reg_set,
	ioctl_isis_reg_field_get,
	ioctl_isis_reg_field_set,
	ioctl_isis_acl_rule_dump,
	ioctl_isis_acl_list_dump,
	ioctl_atheros_show_cnt,
	ioctl_dalvttnumexistget,
	ioctl_dalArVttShow,
	ioctl_dalVlanShow
#endif	
};

static const ioctlfunc *calltable[] = {
	dal_pon_expo_calltable,
	dal_cls_expo_calltable,
	dal_lib_expo_calltable,
	dal_mac_expo_calltable,
	dal_multicast_expo_calltable,
	dal_port_expo_calltable,
	dal_rstp_expo_calltable,
	dal_storm_expo_calltable,
	dal_ver_expo_calltable,
	dal_vtt_expo_calltable,
	tm_expo_calltable,
	phy_expo_calltable,
	register_expo_calltable,
	mdio_expo_calltable,
	opconnmisc_expo_calltable,
	comm_expo_calltable,
	dal_stats_expo_calltable,
	atheros8327_expo_calltable
};
OPL_STATUS get_ioctl_cmd_indexes( int cmd, unsigned int *h_file_index, unsigned int *fn_index)
{
	unsigned int nr;
	unsigned int type;
	unsigned int magic_id_relative;
	unsigned int num_magic_ids_per_h_file = NUM_MAGIC_IDS_PER_H_FILE;

	type = _IOC_TYPE(cmd);
	nr = _IOC_NR(cmd);

	if (type<OPCONN_IOC_MAGIC_BASE ) 
	{
		return OPL_BAD_PARA;
	}

	magic_id_relative = type-OPCONN_IOC_MAGIC_BASE;
	*h_file_index = magic_id_relative/num_magic_ids_per_h_file;
	*fn_index = (magic_id_relative-((magic_id_relative/num_magic_ids_per_h_file)*num_magic_ids_per_h_file))*256+nr;

	return OPL_OK;
}


int opconn_dal_init(void)
{
	OPL_STATUS ret = OPL_OK;
	
	ret = dalDemonInit();

	ret += dalStatsThreadInit();

	return ret;
}

int opconn_dal_ioctl( unsigned int cmd, unsigned int arg )
{
	unsigned int fn_index;
	unsigned int h_file_index;
	
	if ( get_ioctl_cmd_indexes( cmd, &h_file_index, &fn_index )!=OPL_OK )
	{
		return OPL_FAIL;
	}
	
	return calltable[h_file_index][fn_index]( arg );
}

/* Begin Added*/
#define MDC_MDIO_DUMMY_ID           0
#define MDC_MDIO_CTRL0_REG          31
#define MDC_MDIO_START_REG          29
#define MDC_MDIO_CTRL1_REG          21
#define MDC_MDIO_ADDRESS_REG        23
#define MDC_MDIO_DATA_WRITE_REG     24
#define MDC_MDIO_DATA_READ_REG      25
#define MDC_MDIO_PREAMBLE_LEN       32

#define MDC_MDIO_START_OP          0xFFFF
#define MDC_MDIO_ADDR_OP           0x000E
#define MDC_MDIO_READ_OP           0x0001
#define MDC_MDIO_WRITE_OP          0x0003

#define SPI_READ_OP                 0x3
#define SPI_WRITE_OP                0x2
#define SPI_READ_OP_LEN             0x8
#define SPI_WRITE_OP_LEN            0x8
#define SPI_REG_LEN                 16
#define SPI_DATA_LEN                16

int rtk_smi_read(unsigned int mAddrs, unsigned int *rData)
{
    unsigned short usData;
    /* Write Start command to register 29 */
    (void)mdioRegisterWrite(MDC_MDIO_DUMMY_ID, MDC_MDIO_START_REG, MDC_MDIO_START_OP);

    /* Write address control code to register 31 */
    (void)mdioRegisterWrite(MDC_MDIO_DUMMY_ID, MDC_MDIO_CTRL0_REG, MDC_MDIO_ADDR_OP);

    /* Write Start command to register 29 */
    (void)mdioRegisterWrite(MDC_MDIO_DUMMY_ID, MDC_MDIO_START_REG, MDC_MDIO_START_OP);

    /* Write address to register 23 */
    (void)mdioRegisterWrite(MDC_MDIO_DUMMY_ID, MDC_MDIO_ADDRESS_REG, mAddrs);

    /* Write Start command to register 29 */
    (void)mdioRegisterWrite(MDC_MDIO_DUMMY_ID, MDC_MDIO_START_REG, MDC_MDIO_START_OP);

    /* Write read control code to register 21 */
    (void)mdioRegisterWrite(MDC_MDIO_DUMMY_ID, MDC_MDIO_CTRL1_REG, MDC_MDIO_READ_OP);

    /* Write Start command to register 29 */
    (void)mdioRegisterWrite(MDC_MDIO_DUMMY_ID, MDC_MDIO_START_REG, MDC_MDIO_START_OP);
    
    /* Read data from register 25 */
    (void)mdioRegisterRead(MDC_MDIO_DUMMY_ID, MDC_MDIO_DATA_READ_REG, &usData);
    *rData = ((unsigned int)usData);
    
    return 0;
}

int rtk_smi_write(unsigned int mAddrs, unsigned int rData)
{
    /* Write Start command to register 29 */
    (void)mdioRegisterWrite(MDC_MDIO_DUMMY_ID, MDC_MDIO_START_REG, MDC_MDIO_START_OP);

    /* Write address control code to register 31 */
    (void)mdioRegisterWrite(MDC_MDIO_DUMMY_ID, MDC_MDIO_CTRL0_REG, MDC_MDIO_ADDR_OP);

    /* Write Start command to register 29 */
    (void)mdioRegisterWrite(MDC_MDIO_DUMMY_ID, MDC_MDIO_START_REG, MDC_MDIO_START_OP);

    /* Write address to register 23 */
    (void)mdioRegisterWrite(MDC_MDIO_DUMMY_ID, MDC_MDIO_ADDRESS_REG, mAddrs);

    /* Write Start command to register 29 */
    (void)mdioRegisterWrite(MDC_MDIO_DUMMY_ID, MDC_MDIO_START_REG, MDC_MDIO_START_OP);

    /* Write data to register 24 */
    (void)mdioRegisterWrite(MDC_MDIO_DUMMY_ID, MDC_MDIO_DATA_WRITE_REG, rData);

    /* Write Start command to register 29 */
    (void)mdioRegisterWrite(MDC_MDIO_DUMMY_ID, MDC_MDIO_START_REG, MDC_MDIO_START_OP);

    /* Write data control code to register 21 */
    (void)mdioRegisterWrite(MDC_MDIO_DUMMY_ID, MDC_MDIO_CTRL1_REG, MDC_MDIO_WRITE_OP);

    return 0;
}

int rtk_getAsicReg(unsigned int reg, unsigned int *pValue)
{
	unsigned int regData;

	(void)rtk_smi_read(reg, &regData);

	*pValue = regData;

	return 0;
}

int rtk_setAsicReg(unsigned int reg, unsigned int value)
{
    (void)rtk_smi_write(reg, value);
    return 0;
}

int rtk_setAsicPHYReg( unsigned int phyNo, unsigned int phyAddr, unsigned int value)
{
	unsigned int regAddr;

    if(phyNo > 4)
        return -1;

    if(phyAddr > 0x1F)
        return -1;

    regAddr = 0x2000 + (phyNo << 5) + phyAddr;

    return rtk_setAsicReg(regAddr, value);
}

int rtk_getAsicPHYReg( unsigned int phyNo, unsigned int phyAddr, unsigned int *value)
{
	unsigned int regAddr;

    if(phyNo > 4)
        return -1;

    if(phyAddr > 0x1F)
        return -1;

    regAddr = 0x2000 + (phyNo << 5) + phyAddr;

    return rtk_getAsicReg(regAddr, value);
}

static int ioctl_swmdioglbregisterread( unsigned int arg )
{
	int iret = 0;
	swmdioregisterread_params_t params;

	USR2KRN( params, (void*)arg );

	iret = rtk_getAsicReg(params.regAddr, &(params.data0));
	if ( iret < 0 )
	{
		return OPL_FAIL;
	}

	KRN2USR( (void*)arg, params );

	return OPL_OK;
}

static int ioctl_swmdioglbregisterwrite( unsigned int arg )
{
	int iret = 0;
	swmdioregisterwrite_params_t params;

	USR2KRN( params, (void*)arg );

	//iret = rtl8367b_setAsicReg( params.regAddr, params.data0 );
	iret = rtk_setAsicReg( params.regAddr, params.data0 );
	if ( iret < 0 )
	{
		return OPL_FAIL;
	}

	return OPL_OK;
}

static int ioctl_swmdiophyregisterread( unsigned int arg )
{
	int iret = 0;
	swmdiophyregread_params_t params;

	USR2KRN( params, (void*)arg );

	iret = rtk_getAsicPHYReg(params.phyid, params.regAddr, &(params.data0));
	if ( iret < 0 )
	{
		return OPL_FAIL;
	}

	KRN2USR( (void*)arg, params );

	return OPL_OK;
}

static int ioctl_swmdiophyregisterwrite( unsigned int arg )
{
	int iret = 0;
	swmdiophyregwrite_params_t params;

	USR2KRN( params, (void*)arg );

	iret = rtk_setAsicPHYReg(params.phyid, params.regAddr, params.data0);
	if ( iret < 0 )
	{
		return OPL_FAIL;
	}

	return OPL_OK;
}
/* End   Added of porting */

/* Begin Added of qinq */
static int ioctl_VttEntryTagAdd(unsigned int arg)
{
    int iret = 0;
    vttentrytag_params_t params;

	USR2KRN( params, (void*)arg );
	iret = brgVttEntryTagAdd(params.pon_strip_en, params.ge_strip_en, params.c_vid, params.s_vid);
	if ( iret < 0 )
	{
		return OPL_FAIL;
	}
    return OPL_OK;
}

static int ioctl_QinqPvidSet(unsigned int arg)
{
    int iret = 0;
	qinqpvidset_params_t params;

	USR2KRN( params, (void*)arg );
	iret = brgQinqPvidSet(params.uiPvid, params.uiPriority);
	if ( iret < 0 )
	{
		return OPL_FAIL;
	}

	return OPL_OK;
}

static int ioctl_QinqPvidReSet(unsigned int arg)
{
    int iret = 0;
	qinqpvidset_params_t params;

	USR2KRN( params, (void*)arg );
	iret = brgQinqPvidReset(params.uiPvid, params.uiPriority);
	if ( iret < 0 )
	{
		return OPL_FAIL;
	}

	return OPL_OK;
}
/* End   Added of qinq */
#ifdef CONFIG_BOSA
static int ioctl_dalpon25l90imodset(unsigned int arg)
{
    int iret = 0;
	unsigned int params;
    params = arg;
	USR2KRN( params, arg );
	iret = dal_I2c25l90ImodSet(params);
	if ( iret < 0 )
	{
		return OPL_FAIL;
	}

	return OPL_OK;
}
static int ioctl_dalpon25l90Apcset(unsigned int arg)
{
    int iret = 0;
	unsigned int params;
    params = arg;
	USR2KRN( params, arg );
	iret = dal_I2c25l90ApcSet(params);
	if ( iret < 0 )
	{
		return OPL_FAIL;
	}

	return OPL_OK;
}
#endif /*end CONFIG_BOSA*/


