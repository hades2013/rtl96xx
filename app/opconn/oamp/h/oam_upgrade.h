/*****************************************************************************


*****************************************************************************/

#ifndef __OAM_UPGRADE_H__
#define __OAM_UPGRADE_H__

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/
#define OAM_IS_REGISTER (oam_status == EX_DISCOVERY_SEND_ANY)

#define ONU_UPDATESW_BLOCK_HEAD  9      /* ONU���������ͷ�Ĵ�С*/

#define IMAGE_TAIL_SIZE         sizeof(IMAGE_TAIL_S)

/* ʱ�䳤�� */
#define TIME_LENGTH 8


#define IMAGE_TARGET_CTC            0x07


#define CTC_UPGRAGE_ONU_ID_ET704      0x01
#define CTC_UPGRAGE_ONU_ID_ET702      0x02
#define CTC_UPGRAGE_ONU_ID_LSN2PU1S   0x03
#define CTC_UPGRAGE_ONU_ID_ET254L     0x04
#define CTC_UPGRAGE_ONU_ID_ET708      0x05
#define HEADER_ONU_ID_CC750E  	      0xA
#define HEADER_ONU_ID_ET254G          0x0B
#define HEADER_ONU_ID_ET704LA         0x0C
#define HEADER_ONU_ID_ET254LA         0x0D
#define HEADER_ONU_ID_ET254GA         0x0E
#define HEADER_ONU_ID_CC754E	      0x0F

#define HEADER_ONU_ID_EM200  	      0x0F

#define UPGRADE_TYPE_BOOT       (0x01)
#define UPGRADE_TYPE_SW         (0x02)

#define CTC_OUI                 0x111111

/* TODO ET254G */
/* ONU���IMAGE�ļ�����Ӧ�ò���ں�����������������Լ������16�ֽ�β�� */
/* ET254-G�����Ŀǰֻ�в���2M�����ڽ����ֵ����СһЩ��ʡһЩ�ռ��HEAP */
#define MAX_IMAGE_SIZE          (4096 * 1024)

#define NETENDIANBUF_2_ULONG( buffer, x )\
    do {\
        x =  buffer[3] + (buffer[2] << 8) + (buffer[1] << 16) + (buffer[0] << 24);\
    } while (0)

/* upgrade begin*/
#define UPGRADE_STEP_OFF                  0
#define UPGRADE_LLID_MASK_OFF0            1
#define UPGRADE_LLID_MASK_OFF1            5
#define UPGRADE_SW_SIZE_OFF               9
#define UPGRADE_H3C_OUI_OFF               13
#define UPGRADE_ONU_TYPE_OFF              16
#define UPGRADE_H3C_HEAD_SW_SIZE_OFF      17
#define UPGRADE_SW_TYPE_OFF               21
#define UPGRADE_FILE_H3C_HEAD_SIZE        12

#define OUI_SIZE                          3

/* ������ʱʱ�䣬3���� */
#define OAM_UPGRADE_TIME (180 * 1000)
/* upgrade end */

#define TASK_PRIO_BURNIMAGE 200

/*----------------------------------------------*
 * �ṹ�嶨��                                   *
 *----------------------------------------------*/
/* �����ļ���ʱ���ʽ */
typedef struct tagIMAGE_TAIL_TIME
{
    USHORT      usYear;
    UCHAR       ucMonth;
    UCHAR       ucDate;
    UCHAR       ucHour;
    UCHAR       ucMinute;
    UCHAR       ucSecond;
    UCHAR       ucReserved;
} IMAGE_TAIL_TIME_S;

/* �����ļ���β�� */
typedef struct tagIMAGE_TAIL
{
    IMAGE_TAIL_TIME_S   stTime;     /* �����ļ�����ʱ��                    */
    ULONG               ulSize;     /* �����ļ��Ĵ�С(ԭʼ��bin�ļ��Ĵ�С) */
    UCHAR               ucVersion;  /* �����ļ��İ汾                      */
    UCHAR               ucTarget;   /* �����ļ���Ե�Ŀ�굥��              */
    USHORT              usCrc;      /* ���������ļ�(��ȥCRC�ֶ�)��У���   */
} IMAGE_TAIL_S;

/*ONU��������������ö��������*/
typedef enum tagONU_UPGRADE_COMMANDER
{
    ONU_COMMANDER_65,
    ONU_COMMANDER_31,
    ONU_COMMANDER_ONU,
    ONU_COMMANDER_BUTT
} ONU_COMMANDER_E;

#define ONU_UPGRADE_SOFTWARE    0
#define ONU_UPGRADE_BOOTROM     1
#define ONU_GPGRADE_ERROR       2

#define UPGRADE_TIME_SIZE  8

/* ONU��������¼�ṹ��*/
typedef struct tagONU_UPGRADE_RECORD
{
    UCHAR           aucTime[UPGRADE_TIME_SIZE];         /* ����ʱ��             */
    USHORT          usVersion;                          /* ��������汾         */
    USHORT          usCorrectFlag;                      /* ������������Ƿ�ɹ� */
    USHORT          usSftType;                          /* 0:ONU��� 1:Bootrom  */
    ONU_COMMANDER_E enCommander;                        /* ��������������       */    
}ONU_UPGRADE_RECORD_S;


/*----------------------------------------------*
 * �ⲿ����                                     *
 *----------------------------------------------*/
/* ��ǰ�Ƿ���WEB����״̬ */
extern BOOL_T g_bIsWebUpgradingNow;

/* ��ǰ�Ƿ���EOAM����״̬ */
extern BOOL_T g_bIsEoamUpgradingNow;

/* ��ǰ�Ƿ���CTC����״̬ */
extern BOOL_T g_bIsCtcUpgradingNow;

/* ONU ����汾��*/
extern USHORT g_usSoftVer;

/*----------------------------------------------*
 * �ⲿ����                                     *
 *----------------------------------------------*/
extern USHORT CalcCrc(IN USHORT usCrc, IN USHORT *pusBuf, IN ULONG ulSize);
extern ULONG UPGRADE_ProcessUpgradeFrame(IN UCHAR *pucBuf, IN ULONG ulBufLen );
extern ULONG halBurnImageThreadInit(VOID);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __OAM_UPGRADE_H__ */

