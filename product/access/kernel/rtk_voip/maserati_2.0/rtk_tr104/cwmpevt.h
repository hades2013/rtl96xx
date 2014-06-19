#ifndef CWMP_EVT_MSG_H
#define CWMP_EVT_MSG_H

#include "kernel_config.h"
#include "voip_feature.h"
#include "rtk_voip.h"
#include "mib_def.h"

//#define	MAX_PORTS			CON_CH_NUM 	///MAX channel number
#define	MAX_PORTS			8 	///MAX channel number

#define TEST_STRING_LEN					40		///< max string length


typedef enum{
    EVT_VOICEPROFILE_LINE_NONE,
	EVT_VOICEPROFILE_LINE_GET_STATUS,
	EVT_VOICEPROFILE_LINE_SET_STATUS,
	EVT_VOICEPROFILE_LINE_SET_DIAGNOSTICS
}cwmpEvt;

typedef enum{
	CALLER_NONE,
	CALLER_NO_DIALTONE,
	CALLER_OFFHOOK_RELEASE,
	CALLER_AFTERDIAL_RELEASE,
	CALLER_NO_ANSWER
}CallerFailReasonEvt;


typedef enum{
	CALLEE_NONE,
	CALLEE_NO_INCOMINGCALL,
	CALLEE_OFFHOOK_RELEASE,
	CALLEE_NO_ANSWER
}CalledFailReasonEvt;


#define IMS_SPECIAL_DIAL_TONE	(1<<0)
#define IMS_IMMEDIATE_CALLSETUP	(1<<1) // immediate hot line
#define IMS_MCID_ACTIVATE			(1<<2)
#define IMS_CONFERENCE_ACTIVATE	(1<<3)
#define IMS_CALL_WAITING_ACTIVATE	(1<<4)
#define IMS_DEFERE_CALLSETUP	(1<<5) //delay hot line
#define IMS_HOLD_SERVICE_ACTIVATE		(1<<6) // 0: support , 1:disable




typedef struct voiceProfileLineStatusObj{
	int profileID;
	int line;
	int incomingCallsReceived;
	int incomingCallsAnswered;
	int incomingCallsConnected;
	int incomingCallsFailed;
	int outgoingCallsAttempted;
	int outgoingCallsAnswered;
	int outgoingCallsConnected;
	int outgoingCallsFailed;
	int resetStatistics;
	unsigned int totalCallTime;
	unsigned long	featureFlags;
	int IADDiagnosticsState; //VoiceProfile.{i}.X_CT-COM_IADDiagnostics.IADDiagnosticsState ,0:None, 1:Requested, 2:Complete
	int TestServer;//VoiceProfile.{i}.X_CT-COM_IADDiagnostics.TestServer  ,0:None, 1:Requested, 2:Complete
	int RegistResult ;//VoiceProfile.{i}.X_CT-COM_IADDiagnostics.RegistResult  , register 0: ok , 1: fail
	int Reason;//VoiceProfile.{i}.X_CT-COM_IADDiagnostics.Reason  
	line_status_t LineStatus; //VoiceService.{i}.VoiceProfile.{i}.Line.{i}.Status
	
}voiceProfileLineStatus;





//ericchung for E8C tr104 auto test
typedef struct _LinphoneTR104Test
{
	
//VoiceService.{i}.PhyInterface.{i}.Tests.X_CT-COM_SimulateTest.

	int enable; //0 : disable , 1: enable  
	int TestType;	//0: Caller  or 1: Callee   
	char callednumber[TEST_STRING_LEN]; //phonenumber
	int DailDTMFConfirmEnable; //need play dtmf 
	int dtmfindex; //play dtmf index
	char DailDTMFConfirmNumber[TEST_STRING_LEN]; //for send dtmf
	int receive_dtmf_index; //receive dtmf index
	char ReceiveDTMFNumber[TEST_STRING_LEN]; //for receive dtmf
	int DailDTMFConfirmResult; //dtmf compare result

	
	int status;		// 0:not start, need offhook , 1:send dtmf  , internal linphone usage.
	
	char Simulate_Status[TEST_STRING_LEN]; // idle,Off-hook,Dialtone ,Receiving,ReceiveEnd,Ringing-back,Connected,Testend
	int Conclusion;	//Simulate success or not. 0:success, 1: fail
	CallerFailReasonEvt CallerFailReason;
	CalledFailReasonEvt CalledFailReason;
	int FailedResponseCode;

	int TestStatus;//VoiceService.{i}.PhyInterface.{i}.Tests.TestState , 0:none, 1:Requested  , 2:Complete
	int TestSelector;////VoiceService.{i}.PhyInterface.{i}.Tests.TestSelector  0: PhoneConnectivityTest,1: X_CT-COM_SimulateTest
	int PhoneConnectivity; //VoiceService.{i}.PhyInterface.{i}.Tests.TestState , 0:non
	
} LinphoneTR104Test_t;




typedef struct cwmpEvtMsgObj{
	cwmpEvt event;
	voiceProfileLineStatus voiceProfileLineStatusMsg[MAX_PORTS];	
	LinphoneTR104Test_t e8c_autotest; /* currently only support fxs 1 */
}cwmpEvtMsg;

/*new the evt msg*/
cwmpEvtMsg *cwmpEvtMsgNew(void);

/* free the evt msg */
void cwmpEvtMsgFree(cwmpEvtMsg *msg);

/*set and get the msg event*/
void cwmpEvtMsgSetEvent(cwmpEvtMsg *msg, cwmpEvt event);
cwmpEvt cwmpEvtMsgGetEvent(cwmpEvtMsg *msg);

/* set and get the voiceProfileLineStatus */
void cwmpEvtMsgSetVPLineStatus(cwmpEvtMsg *msg, voiceProfileLineStatus *VPLineStatus, int nPort);
voiceProfileLineStatus *cwmpEvtMsgGetVPLineStatus(cwmpEvtMsg *msg, int nPort);

/* the size of the evt msg */
int cwmpEvtMsgSizeof(void);

#endif /*CWMP_EVT_MSG_H*/
