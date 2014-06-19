/*****************************************************************************


*****************************************************************************/

#ifndef __OAM_UPGRADE_H__
#define __OAM_UPGRADE_H__

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/
#define OAM_IS_REGISTER (oam_status == EX_DISCOVERY_SEND_ANY)

#define ONU_UPDATESW_BLOCK_HEAD  9      /* ONU升级软件块头的大小*/

#define IMAGE_TAIL_SIZE         sizeof(IMAGE_TAIL_S)

/* 时间长度 */
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
/* ONU软件IMAGE文件包括应用层和内核软件，包含了我们自己定义的16字节尾部 */
/* ET254-G的软件目前只有不到2M，现在将这个值定义小一些，省一些空间给HEAP */
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

/* 升级超时时间，3分钟 */
#define OAM_UPGRADE_TIME (180 * 1000)
/* upgrade end */

#define TASK_PRIO_BURNIMAGE 200

/*----------------------------------------------*
 * 结构体定义                                   *
 *----------------------------------------------*/
/* 镜像文件中时间格式 */
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

/* 镜像文件的尾部 */
typedef struct tagIMAGE_TAIL
{
    IMAGE_TAIL_TIME_S   stTime;     /* 镜像文件生成时间                    */
    ULONG               ulSize;     /* 镜像文件的大小(原始的bin文件的大小) */
    UCHAR               ucVersion;  /* 镜像文件的版本                      */
    UCHAR               ucTarget;   /* 镜像文件针对的目标单板              */
    USHORT              usCrc;      /* 整个镜像文件(除去CRC字段)的校验和   */
} IMAGE_TAIL_S;

/*ONU升级操作发起者枚举量定义*/
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

/* ONU的升级记录结构体*/
typedef struct tagONU_UPGRADE_RECORD
{
    UCHAR           aucTime[UPGRADE_TIME_SIZE];         /* 升级时间             */
    USHORT          usVersion;                          /* 升级软件版本         */
    USHORT          usCorrectFlag;                      /* 升级软件接收是否成功 */
    USHORT          usSftType;                          /* 0:ONU软件 1:Bootrom  */
    ONU_COMMANDER_E enCommander;                        /* 升级操作发起者       */    
}ONU_UPGRADE_RECORD_S;


/*----------------------------------------------*
 * 外部变量                                     *
 *----------------------------------------------*/
/* 当前是否处于WEB升级状态 */
extern BOOL_T g_bIsWebUpgradingNow;

/* 当前是否处于EOAM升级状态 */
extern BOOL_T g_bIsEoamUpgradingNow;

/* 当前是否处于CTC升级状态 */
extern BOOL_T g_bIsCtcUpgradingNow;

/* ONU 软件版本号*/
extern USHORT g_usSoftVer;

/*----------------------------------------------*
 * 外部函数                                     *
 *----------------------------------------------*/
extern USHORT CalcCrc(IN USHORT usCrc, IN USHORT *pusBuf, IN ULONG ulSize);
extern ULONG UPGRADE_ProcessUpgradeFrame(IN UCHAR *pucBuf, IN ULONG ulBufLen );
extern ULONG halBurnImageThreadInit(VOID);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __OAM_UPGRADE_H__ */

