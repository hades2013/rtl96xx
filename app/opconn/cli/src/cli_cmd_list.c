/*****************************************************************************
 * cli_cmd_list.c      Define All command prototype.
*****************************************************************************/
#include "cli.h"
#include "cli_cmd_list.h"
#include "lw_config.h"

#if defined(ONU_1PORT)
#define PORTLIST_HELP "Input port #1"
#elif defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
#define PORTLIST_HELP "Input port list, format: <1,2-4>, range: 1~4"
#else
#define PORTLIST_HELP "Input port list, format: <1,2-4>, range: 1~4"
#endif
extern STATUS cliCmdSwMdioRead(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdSwMdioWrite(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdSwMdioPhyRead(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdSwMdioPhyWrite(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdI2cInit(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdI2cRead(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
#if defined(CONFIG_PRODUCT_EPN200)
extern STATUS cliCmdSingleMdioDbgWrite(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdSingleMdioDbgRead(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
#endif
MODEL_INTF_INFO_t g_asCliCmdTable[]={
{
    "help",

    "{help,0, Description of interactive help system}",

    "",

    CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_GUEST,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdHelp
},

{
    "quit",

    "{quit,0, Terminate the session}",

    "",

    CLI_MODE_COMMON,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_GUEST,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdQuit
},
{
	"login",

	"{login, 0, Login system}",

	"",

	CLI_MODE_USER | CLI_MODE_ENABLE,

	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_GUEST,

	CLI_EXEC_FUNC,

	(FUNCPTR)cliCmdLogin
},

#if 0  /* Commented out unused feature */
{
    "logout",

    "{logout,0, Terminate the session}",

    "",

    CLI_MODE_COMMON,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdQuit
},
#endif /* #if 0 */

{
    "exit",

    "{exit,0, Return to upper mode}",

    "",

    CLI_MODE_COMMON & ~CLI_MODE_USER,

    CLI_NULL_SUB_MODE,

    ACCESS_LEVEL_GUEST,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdExiter
},

{
    "history",

    "{history,0, Display the session command history}",

    "",

    CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

    ACCESS_LEVEL_GUEST,

    CLI_EXEC_TASK,

    (FUNCPTR)cliCmdShowHistory
},

{
    "show version",

    "{show, 0,Show running system information},\
    {version,0,Show version information}",

    "",

    CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_GUEST,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdShowVersion
},

{
    "enable",

    "{enable,0, Turn on enable mode}",

    "",

    CLI_MODE_USER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_GUEST,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdEnable
},

{
    "debug module {dvrs|sys|user|qos|fdb|mcast|vlan|port|rstp|pon|log|storm|oam|stats|all}\
    level {off|critical|warning|information|debugging}",

    "{debug, 0, Configure debug},\
    {module, 0, Open debug module},\
    {dvrs, 0, Open drivers module},\
    {sys, 0, Open system module},\
    {user, 0, Open user module},\
    {qos, 0, Open QoS module},\
    {fdb, 0, Open FDB module},\
    {mcast, 0, Open multicast module},\
    {vlan, 0, Open VLAN module},\
    {port, 0, Open port module},\
    {rstp, 0, Open rstp module},\
    {pon, 0, Open pon module},\
    {log, 0, Open log module},\
    {storm, 0, Open storm module},\
    {oam, 0, Open oam module},\
    {stats, 0, Open stats module},\
    {all, 0, Open all module},\
    {level, 0, Open debug level},\
    {off,0, Close debug message output},\
    {critical,0, Critical message},\
    {warning,0, Warning message},\
    {information,0, Information message},\
    {debugging,0, Debugging message}",

    "",

    CLI_MODE_ENABLE | CLI_MODE_SUPER,
    CLI_NULL_SUB_MODE,
	ACCESS_LEVEL_USER,
    CLI_EXEC_FUNC,
    (FUNCPTR)cliCmdLogDebugLevel
},

{
    "show log [running-config]",

    "{show, 0,Show running system information},\
    {log,0,Show log information},\
    {running-config,0,Show log running configuration}",

    "",

    CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_CONFIG_SYS | CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_TASK,

    (FUNCPTR)cliCmdShowLog

},

{
    "log {disable|enable}",

    "{log, 0,Log information},\
    {disable,0,Disable log information},\
    {enable,0,Enable log information}",

    "",

    CLI_MODE_ENABLE | CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdLogEnable

},
{
	"syslog {disable|enable}",

	"{syslog, 0,syslog server configuration},\
	{disable,0,Disable syslog configuration},\
	{enable,0,Enable syslog configuration}",

	"",

	CLI_MODE_CONFIG_SYS,

	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

	CLI_EXEC_FUNC,

	(FUNCPTR)cliCmdSyslogEnable

},
{
	"syslog test [num]",

	"{syslog, 0, syslog }, {test, 0, test},\
	 {num,1,syslog test number}",

	"{num, CLI_UINT,1,1024,10}",
	CLI_MODE_SUPER,

	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

	CLI_EXEC_FUNC,
	(FUNCPTR)cliCmdSyslogTest
},
{
	"show syslog-server",

	"{show, 0,syslog server information},\
	{syslog-server,0,syslog-server}",

	"",

	CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_CONFIG_SYS | CLI_MODE_SUPER,

	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

	CLI_EXEC_FUNC,

	(FUNCPTR)cliCmdShowSyslogServer

},
{
	"syslog-server ipaddress [port portnum]",

	"{syslog-server,0,config syslog server},\
	  {ipaddress,1,syslog server ipaddress} ,\
	  {port,0,port},\
	  {portnum,1,syslog server port,default 514}",

    "{ipaddress,CLI_IPDOTADDR,0.0.0.0, 255.255.255.255, 0.0.0.0}, \
     {portnum,CLI_UINT,1,65535,514}",

	CLI_MODE_CONFIG_SYS,

	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

	CLI_EXEC_FUNC,

	(FUNCPTR)cliCmdSetSyslogServer

},
{
	"no syslog-server ipaddress",

	"{no,0,delete a syslog-server},\
	{syslog-server,0,syslog-server},\
	{ipaddress,1,syslog server ipaddress}",

	"{ipaddress,CLI_IPDOTADDR,0.0.0.0, 255.255.255.255, 0.0.0.0}",

	CLI_MODE_CONFIG_SYS,

	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

	CLI_EXEC_FUNC,

	(FUNCPTR)cliCmdDelSyslogServer

},
{
    "log clear",

    "{log, 0,Log information},\
    {clear,0,Clear log information}",

    "",

    CLI_MODE_ENABLE | CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_TASK,

    (FUNCPTR)cliCmdLogClear

},

#ifndef ONU_1PORT
{
    "local switch {disable|enable}",

    "{local, 0,Local configuration},\
    {switch, 0,Switch configuration},\
    {disable,0,Disable local switch},\
    {enable,0,Enable local switch}",

    "",

    CLI_MODE_ENABLE | CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdLocalSwitchEnable

},
#endif

{
    "show alarm",

    "{show, 0,Show running system information},\
    {alarm,0,Show alarm information}",

    "",

    CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_CONFIG_SYS | CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_TASK,

    (FUNCPTR)cliCmdShowAlarm

},

{
    "alarm clear",

    "{alarm, 0,Alarm information},\
    {clear,0,Clear alarm information}",

    "",

    CLI_MODE_ENABLE | CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_TASK,

    (FUNCPTR)cliCmdAlarmClear

},

{
    "super",

    "{super,0, Enter super mode}",

    "",

    CLI_MODE_ENABLE | CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdSuper
},

{
    "show classify rule {local|asic|all}",

    "{show,0, Show classify rule},\
     {classify,0,Classify },\
     {rule,0,Rule},\
     {local,0,Local},\
     {asic,0,Asic},\
     {all,0,All}",

    "",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdQoSShowClassRule
},
{
    "show classify mask {local|asic|all}",

    "{show,0, Show classify rule table},\
     {classify,0,Classify},\
     {mask,0,Mask},\
     {local,0,Local},\
     {asic,0,Asic},\
     {all,0,All}",

    "",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdQoSShowClassMask
},
{
    "show classify addr {local|asic|all}",

    "{show,0, Show classify address table},\
     {classify,0,Classify},\
     {addr,0,Addr},\
     {local,0,Local},\
     {asic,0,Asic},\
     {all,0,All}",

    "",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdQoSShowClassAddr
},
{
    "show classify config",

    "{show,0, Show classify configuration},\
     {classify,0,Classify},\
     {config,0,Config}",

    "",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdQoSShowCfg
},
{
	"show classify shadow-tbl [precedence]",

	"{show,0, Show classify configuration},\
	 {classify,0,Classify},\
	 {shadow-tbl,0,shadow table},\
	 {precedence,0,precedence only}",

	"",

	CLI_MODE_COMMON,

	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

	CLI_EXEC_FUNC,

	(FUNCPTR)cliCmdQoSShowPre
},

{
    "delete classify config value",

    "{delete,0, Delete classify configuration},\
     {classify,0,Delete classify configuration},\
     {config,0,Classify Item config},\
     {value,1,Input classify config Item num}",

    "value,CLI_UINT,0,127,0}",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdQoSDelCfg
},
{
    "delete classify hwrule value",

    "{delete,0, Delete classify configuration},\
     {classify,0,Delete classify configuration},\
     {hwrule,0,Hardware rule table },\
     {value,1,Input classify rule table index}",

    "value,CLI_UINT,0,127,0}",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdQoSDelRule
},
{
    "set classify hwrule index rmkVid value act {noopt|insettag|changeVID}",

    "{set,0,Set classify configuration},\
     {classify,0,Set classify configuration},\
     {hwrule,0,Hardware rule table },\
     {index,1,Input classify rule table index},\
     {rmkVid,0,Remark vlan},\
     {value,1,Input vid remark value},\
     {act,0,Remark vlan action},\
     {noopt,0,Do not operate},\
     {insettag,0,Inset tag},\
     {changeVID,0,Change VID}",


    "{index,CLI_UINT,0,128,0},\
     {value,CLI_UINT,0,4095,0}",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdQoSRmkVid
},
{
    "show interrupt",

    "{show,0, Show interrupt},\
     {interrupt,0,Interrupt}",
    "",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdInterruptShow
},
{
    "interrupt mod_val submod_val {disable | enable}",

    "{interrupt,0, Interrupt configuration},\
     {mod_val,1,Input level 1 index},\
     {submod_val,1,Input level 2 index},\
     {disable,0,Disable interrupt},\
     {enable,0,Enable interrupt}",

    "{mod_val,CLI_UINT,0,33,0},\
    {submod_val,CLI_UINT,0,33,0}",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdInterruptSet
},
#if 0/*wfxu 0403 will be done*/
{
    "i2c test start {polling|interrupt} frequency {330|288|217|146|88|59|44|36}",

    "{i2c,0, I2C},\
     {test,0,I2C Testing},\
     {start,0,Start testing},\
     {polling,0,I2C polling mode},\
     {interrupt,0,I2C interrupt mode},\
     {frequency,0,Serial clock frequency},\
     {330,0,330K serial clock frequency},\
     {288,0,288K serial clock frequency},\
     {217,0,217K serial clock frequency},\
     {146,0,146K serial clock frequency},\
     {88,0,88K serial clock frequency},\
     {59,0,59K serial clock frequency},\
     {44,0,44K serial clock frequency},\
     {36,0,36K serial clock frequency}",

    "",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdI2CTestStart
},

{
    "i2c test stop",

    "{i2c,0, I2C},\
     {test,0,I2C Testing},\
     {stop,0,Stop testing}",

    "",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdI2CTestStop
},

#endif
{
    "shell",

    "{shell,0, Enter linux shell}",

    "",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdLinuxShell
},
{
    "orr addr",

    "{orr,0,Read register},\
     {addr,1,Input register address}",

    "{addr,CLI_UINT,0,36864,0}",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdOrr
},
{
    "owr addr value",

    "{owr,0,Write register},\
     {addr,1,Input register address},\
     {value,1,Input register value}",

    "{addr,CLI_UINT,0,36864,0},\
     {value,CLI_UINT,0,4294967295,0}",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdOwr
},
{
    "mrr addr num",

    "{mrr,0,Read multiple register},\
     {addr,1,Input register address},\
     {num,1,Input the number to read}",

    "{addr,CLI_UINT,0,36864,0},\
     {num,CLI_UINT,0,36864,1}",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdMrr
},
{
    "mwr addr num value mode",

    "{mwr,0,Write multiple register},\
     {addr,1,Input register address},\
     {num,1,Input the number to write},\
     {value,1,Input register value},\
     {mode,1,Input value mode}",

    "{addr,CLI_UINT,0,36864,0},\
     {num,CLI_UINT,0,36864,1},\
     {value,CLI_UINT,0,4294967295,0},\
     {mode,CLI_UINT,0,1,0}",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdMwr
},
{
    "ort region starid num",

    "{ort,0,Read table},\
     {region,1,Input table region},\
     {starid,1,Input start table id},\
     {num,1,Input table index num}",

    "{region,CLI_UINT,0,64,0},\
     {starid,CLI_UINT,0,256,0},\
     {num,CLI_UINT,0,256,0}",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdOrt
},

{
    "owt tabid srecid numofrec offset value",

    "{owt,0,Write table},\
     {tabid,1,Input tabid},\
     {srecid,1,Input start record id},\
     {numofrec,1,Input num of record},\
     {offset,1,Input offset of record},\
     {value,1,Input write value}",

    "{tabid,CLI_UINT,0,64,0},\
     {srecid,CLI_UINT,0,256,0},\
     {numofrec,CLI_UINT,0,256,0},\
     {offset,CLI_UINT,0,20,0},\
     {value,CLI_WORD,NULL,NULL,10}",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdOwt
},

{
    "ost",

    "{ost,0, Read table description}",

    "",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdOst
},

{
    "mdio read device dev register addr [{number num}|{offset offsetval width widthval}]",

    "{mdio,0,MDIO control},\
     {read,0,Read MDIO register},\
     {device,0,MDIO device},\
     {dev,1,Input device address},\
     {register,0,MDIO register address},\
     {addr,1,Input register address},\
     {number,0,The number of register to read},\
     {num,1,Input the number of register to read},\
     {offset,0,The offset of register to read},\
     {offsetval,1,Input offset of register to read},\
     {width,0,The width of register to read},\
     {widthval,1,Input the width of register to read}",

    "{dev,CLI_UINT,0,31,0},\
     {addr,CLI_UINT,0,31,0},\
     {num,CLI_UINT,1,32,1},\
     {offsetval,CLI_UINT,0,31,0},\
     {widthval,CLI_UINT,1,32,1}",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdMdioRead
},
{
    "mdio write device dev register addr {{value val}|{offset offsetval width widthval field-value fld-val}}",

    "{mdio,0,MDIO control},\
     {write,0,Write MDIO register},\
     {device,0,MDIO device},\
     {dev,1,Input device address},\
     {register,0,MDIO register address},\
     {addr,1,Input register address},\
     {value,0,The value of register to write},\
     {val,1,Input the value of register to write},\
     {offset,0,The offset of register to write},\
     {offsetval,1,Input offset of register to write},\
     {width,0,The width of register to write},\
     {widthval,1,Input the width of register to write},\
     {field-value,0,The field-value of register to write},\
     {fld-val,1,Input the field-value of register to write}",

    "{dev,CLI_UINT,0,31,0},\
     {addr,CLI_UINT,0,31,0},\
     {val,CLI_UINT,0,65535,0},\
     {offsetval,CLI_UINT,0,31,0},\
     {widthval,CLI_UINT,1,32,1},\
     {fld-val,CLI_UINT,0,65535,0}",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdMdioWrite
},

{
    "vtt add entryid pstrip gstrip cvlan svlan",
    "{vtt,0,Vtt operation},\
     {add,0,Add vtt},\
     {entryid,1,Entry Id},\
     {pstrip,1,Pstrip},\
     {gstrip,1,Gstrip},\
     {cvlan,1,C-vlan},\
     {svlan,1,S-vlan}",
    "{entryid,CLI_UINT,0,15,0},\
     {pstrip,CLI_UINT,0,1,0},\
     {gstrip,CLI_UINT,0,1,0},\
     {cvlan,CLI_UINT,1,4094,0},\
     {svlan,CLI_UINT,1,4094,0}",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,
    (FUNCPTR)cliCmdBrgVttAdd
},
{
    "vtt del entryid",
    "{vtt,0,Vtt operation},\
     {del,0,Del vtt},\
     {entryid,1,Entry Id}",
    "{entryid,CLI_UINT,0,15,0}",
    CLI_MODE_SUPER,
    CLI_NULL_SUB_MODE,
    ACCESS_LEVEL_SUPER,
    CLI_EXEC_FUNC,
    (FUNCPTR)cliCmdBrgVttDel
},
#if defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
{
	"vtt show startid endid",
	"{vtt,0,vtt},\
	 {show,0,show},\
	 {startid,1,start id},\
	 {endid,1,end id}",

	 "{startid,CLI_UINT,0,15,0},\
     {endid,CLI_UINT,0,15,0}",

	CLI_MODE_SUPER,
	CLI_NULL_SUB_MODE,
	ACCESS_LEVEL_SUPER,
	CLI_EXEC_FUNC,
	(FUNCPTR)cliCmdBrgVttShow
},
#endif

{
    "shaper upstream usqueueid {enable-cir [cir cir_val cbs cbs_val] |disable-cir} {enable-pir [pir pir_val pbs pbs_val]|disable-pir} ",
    "{shaper,0,Shaper bd operation},\
     {upstream,0,Upstream},\
     {usqueueid,1,Entry id in upstream},\
     {enable-cir,0,Enable-cir shaper},\
     {cir,0,Cir value},\
     {cir_val,1,Input cir value,units is 32kbps},\
     {cbs,0,Cbs value},\
     {cbs_val,1,Input cbs value,units is 256byte},\
     {disable-cir,0,Disable-cir shaper},\
     {enable-pir,0,Enable-cir shaper},\
     {pir,0,Pir value},\
     {pir_val,1,Input pir value,units is 32kbps},\
     {pbs,0,Pbs value},\
     {pbs_val,1,Input pbs value,units is 256byte},\
     {disable-pir,0,Disable-cir shaper}",
    "{usqueueid,CLI_UINT,0,7,0},\
     {cir,CLI_UINT,0,32767,0},\
     {cbs,CLI_UINT,0,1023,0},\
     {pir,CLI_UINT,0,32767,0},\
     {pbs,CLI_UINT,0,1023,0}",
    CLI_MODE_SUPER,
    CLI_NULL_SUB_MODE,
    ACCESS_LEVEL_SUPER,
    CLI_EXEC_FUNC,
    (FUNCPTR)cliCmdTmUsShaperSet
},
{
    "shaper downstream dsqueueid {enable|disable} [cir cir_val cbs cbs_val]",
    "{shaper,0,Shaper bd operation},\
     {downstream,0,Downstream},\
     {dsqueueid,1,Entry Id},\
     {enable,0,enable shaper},\
     {disable,0,disable shaper},\
     {cir,0,cir value},\
     {cir,1,cir value,units is 32kbps},\
     {cbs,0,cbs value},\
     {cbs,1,cbs value,units is 256byte}",
     "{dsqueueid,CLI_UINT,0,7,0},\
     {cir,CLI_UINT,0,32767,0},\
     {cbs,CLI_UINT,0,1023,0}",
    CLI_MODE_SUPER,
    CLI_NULL_SUB_MODE,
    ACCESS_LEVEL_SUPER,
    CLI_EXEC_FUNC,
    (FUNCPTR)cliCmdTmDsShaperSet
},

{
    "qinfo entryid maxcellnum maxpktnum",
    "{qinfo,0,Queue bd operation},\
     {entryid,1,Entryid},\
     {maxcellnum,1,Max num of cell},\
     {maxpktnum,1,Max num of pkt}",
    "{entryid,CLI_UINT,0,15,0},\
     {maxcellnum,CLI_UINT,0,16383,0},\
     {maxpktnum,CLI_UINT,0,8000,0}",
    CLI_MODE_SUPER,
    CLI_NULL_SUB_MODE,
    ACCESS_LEVEL_SUPER,
    CLI_EXEC_FUNC,
    (FUNCPTR)cliCmdTmQueueInfoSet
},
{
    "wred stream entryid weigth minth maxth",

    "{wred,0,Wred bd operation},\
     {stream,1, 0 for upstream 1 for downstream},\
     {entryid,1,Entry id},\
     {weigth,1,Weight},\
     {minth,1,minth},\
     {maxth,1,maxth}",

    "{stream,CLI_UINT,0,1,0},\
     {entryid,CLI_UINT,0,7,0},\
     {weight,CLI_UINT,0,15,0},\
     {minth,CLI_UINT,0,1048575,0},\
     {maxth,CLI_UINT,0,1048575,0}",

    CLI_MODE_SUPER,
    CLI_NULL_SUB_MODE,
    ACCESS_LEVEL_SUPER,
    CLI_EXEC_FUNC,
    (FUNCPTR)cliCmdWredSet
},
{
    "show tm {CosIcosMap|QidIcosMap|InQueueCounter|ShaperConfig|InQueueConfig|WredConfig|QueueWeight|QueuePriority|RptValueOfQueue|TotalPktsInQueueIncMpcp}",
    "{show,0, Show config or info},\
    {tm,0, Tm mode},\
    {CosIcosMap,0,CosIcosMap},\
 	{QidIcosMap,0,QidIcosMap},\
 	{InQueueCounter,0,InQueueCounter},\
 	{ShaperConfig,0,ShaperConfig},\
 	{InQueueConfig,0,InQueueConfig},\
 	{WredConfig,0,WredConfig},\
 	{QueueWeight,0,QueueWeight},\
 	{QueuePriority,0,QueuePriority},\
 	{RptValueOfQueue,0,RptValueOfQueue},\
 	{TotalPktsInQueueIncMpcp,0,TotalPktsInQueueIncMpcp}",

    "",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdTmTabShow
},
{
    "show cnt {brgGeParserCounter|brgPortCounter|brgCounter|tmCounter|ponMpcpCounter|hostDmaCounter|allCounter|intStatus}",
	"{show,0, Show counter},\
    {cnt,0, Counter},\
	{brgGeParserCounter,0,BrgGeParserCounter},\
	{brgPortCounter,0,BrgPortCounter},\
	{brgCounter,0,BrgCounter},\
	{tmCounter,0,TmCounter},\
	{ponMpcpCounter,0,PonMpcpCounter},\
	{hostDmaCounter,0,HostDmaCounter},\
	{allCounter,0,AllCounter},\
	{intStatus,0,IntStatus}",

    "",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdCounterShow
},

{
    "show configuration all",

    "{show, 0,Show running system information},\
    {configuration,0,Show system configuration},\
    {all,0,Show all modules}",

    "",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdSystemShowCfgAll
},

{
    "xmodem {firmware|startup-config} {download|upload}",

    "{xmodem, 0, Transmit file via xmodem},\
    {firmware,0, Firmware download or upload},\
    {startup-config,0, Startup-config download or upload},\
    {download, 0, Download firmware image},\
    {upload, 0, Upload firmware image}",

    "",

    CLI_MODE_CONFIG_SYS,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_ADMINISTRATION,

    CLI_EXEC_TASK,

    (FUNCPTR)cliCmdSystemProcess
},

{
    "firmware switch back",

    "{firmware,0, Firmware switch back},\
    {switch,0, Firmware switch},\
    {back, 0, Switch back}",

    "",

    CLI_MODE_CONFIG_SYS,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_ADMINISTRATION,

    CLI_EXEC_TASK,

    (FUNCPTR)cliCmdSystemFirmwareSwitch
},


{
    "reset",

    "{reset,0,Reset system}",

    "",

    CLI_MODE_ENABLE | CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdReset

},

{
    "system",

    "{system,0,Enter system configuration mode}",

    "",

    CLI_MODE_ENABLE | CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdSystem

},
#if 0
{
    "interface {wan|lan} {share|{independent {admin|signal|media}}}  \
     {{static [ip-address ipaddr net-mask mask gateway gwaddr dns dnsaddr]}|\
     dhcp|{pppoe user username password psword auth {auto|pap|chap}}} \
     [qos {{dscp|tos} qos-value}] \
     [tag-mode {transparent|{tag cvlan cvlan1 priority pri1}|\
     {vlan-stacking cvlan cvlan2 priority pri2 svlan svlan2}}]",

    "{interface,0,Configure interface},\
    {wan,0,Configure WAN interface},\
    {lan,0,Configure LAN interface},\
    {share,0,Configure interface for share mode},\
    {independent,0,Configure interface for independent mode },\
    {admin,0,Enable administration interface},\
    {signal,0,Enable voip signal interface},\
    {media,0,Enable voip media interface},\
\
    {static,0,Configure static mode},\
    {ip-address,0,Configure IP address},\
    {ipaddr,1,Input IP address},\
    {net-mask, 0, Configure subnet mask},\
    {mask,1,Input subnet mask},\
    {gateway,0,Configure gateway},\
    {gwaddr, 1, Input gateway IP},\
    {dns,0,Configure primary DNS server IP},\
    {dnsaddr, 1, Input primary DNS server IP},\
\
    {dhcp, 0, Configure DHCP},\
\
	{pppoe, 0, Configure PPPoE},\
	{user, 0, Configure PPPoE username},\
	{username, 1, Input PPPoE username},\
	{password, 0, Configure PPPoE password},\
	{psword, 1, Input PPPoE password},\
	{auth, 0, Authentication type},\
	{auto, 0, Auto authentication type},\
	{pap, 0, PAP authentication type},\
	{chap, 0, CHAP authentication type},\
\
	{qos, 0, Configure QoS},\
	{dscp, 0, Configure DSCP},\
	{tos, 0, Configure TOS},\
	{qos-value, 1, Input DSCP or TOS value},\
\
	{tag-mode, 0, Configure tag mode},\
	{transparent, 0, Configure transparent mode},\
	{tag, 0, Configure tag mode},\
	{cvlan, 0, Configure CVLAN},\
	{cvlan1, 1, Input CVLAN tag},\
	{priority, 0, Configure priority},\
	{pri1, 1, Input priority},\
	{vlan-stacking, 0, Configure vlan-stacking mode},\
	{cvlan, 0, Configure CVLAN},\
	{cvlan2, 1, Input CVLAN tag},\
	{priority, 0, Configure priority},\
	{pri2, 1, Input priority},\
	{svlan, 0, Configure SVLAN},\
	{svlan2, 1, Input SVLAN tag}",

    "{ipaddr,CLI_IPDOTADDR,0.0.0.0,255.255.255.255,0.0.0.0},\
    {mask,CLI_IPDOTMASK,0.0.0.0,255.255.255.255,255.255.255.0},\
    {gwaddr,CLI_IPDOTADDR,0.0.0.0,255.255.255.255,0.0.0.0},\
    {dnsaddr,CLI_IPDOTADDR,0.0.0.0,255.255.255.255,0.0.0.0},\
\
    {username,CLI_WORD,NULL,NULL,32},\
	{psword,CLI_WORD,NULL,NULL,32},\
\
	{qos-value,CLI_UINT,0,255,0},\
\
	{cvlan1,CLI_UINT,1,4094,0},\
    {pri1,CLI_UINT,0,7,0},\
\
    {cvlan2,CLI_UINT,1,4094,0},\
    {pri2,CLI_UINT,0,7,32},\
	{svlan2,CLI_UINT,1,4094,0}",

    CLI_MODE_CONFIG_SYS,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_ADMINISTRATION,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdSystemIpAddrSet

},
#endif

#if defined(INTERFACE_THREE_WAN_THREE_LAN)
{
    "interface {lan|wan} {share|{independent {admin|signal|media}}}  \
     {{static [ip-address ipaddr net-mask mask gateway gwaddr dns dnsaddr]}|\
     dhcp|{pppoe user username password psword auth {auto|chap|pap}}} \
     [qos {{dscp|tos} qos-value}] \
     [tag-mode {transparent|{tag cvlan cvlan1 priority pri1}|\
     {vlan-stacking cvlan cvlan2 priority pri2 svlan svlan2}}]",

    "{interface,0,Configure interface},\
    {lan,0,Configure LAN interface},\
    {wan,0,Configure WAN interface},\
    {share,0,Configure interface for share mode},\
    {independent,0,Configure interface for independent mode },\
    {admin,0,Enable administration interface},\
    {signal,0,Enable voip signal interface},\
    {media,0,Enable voip media interface},\
\
    {static,0,Configure static mode},\
    {ip-address,0,Configure IP address},\
    {ipaddr,1,Input IP address},\
    {net-mask, 0, Configure subnet mask},\
    {mask,1,Input subnet mask},\
    {gateway,0,Configure gateway},\
    {gwaddr, 1, Input gateway IP},\
    {dns,0,Configure primary DNS server IP},\
    {dnsaddr, 1, Input primary DNS server IP},\
\
    {dhcp, 0, Configure DHCP},\
\
	{pppoe, 0, Configure PPPoE},\
	{user, 0, Configure PPPoE username},\
	{username, 1, Input PPPoE username},\
	{password, 0, Configure PPPoE password},\
	{psword, 1, Input PPPoE password},\
	{auth, 0, Authentication type},\
	{auto, 0, Auto authentication type},\
	{pap, 0, PAP authentication type},\
	{chap, 0, CHAP authentication type},\
\
	{qos, 0, Configure QoS},\
	{dscp, 0, Configure DSCP},\
	{tos, 0, Configure TOS},\
	{qos-value, 1, Input DSCP or TOS value},\
\
	{tag-mode, 0, Configure tag mode},\
	{transparent, 0, Configure transparent mode},\
	{tag, 0, Configure tag mode},\
	{cvlan, 0, Configure CVLAN},\
	{cvlan1, 1, Input CVLAN tag},\
	{priority, 0, Configure priority},\
	{pri1, 1, Input priority},\
	{vlan-stacking, 0, Configure vlan-stacking mode},\
	{cvlan, 0, Configure CVLAN},\
	{cvlan2, 1, Input CVLAN tag},\
	{priority, 0, Configure priority},\
	{pri2, 1, Input priority},\
	{svlan, 0, Configure SVLAN},\
	{svlan2, 1, Input SVLAN tag}",

    "{ipaddr,CLI_IPDOTADDR,0.0.0.0,255.255.255.255,0.0.0.0},\
    {mask,CLI_IPDOTMASK,0.0.0.0,255.255.255.255,255.255.255.0},\
    {gwaddr,CLI_IPDOTADDR,0.0.0.0,255.255.255.255,0.0.0.0},\
    {dnsaddr,CLI_IPDOTADDR,0.0.0.0,255.255.255.255,0.0.0.0},\
\
    {username,CLI_WORD,NULL,NULL,32},\
	{psword,CLI_WORD,NULL,NULL,32},\
\
	{qos-value,CLI_UINT,0,255,0},\
\
	{cvlan1,CLI_UINT,1,4094,0},\
    {pri1,CLI_UINT,0,7,0},\
\
    {cvlan2,CLI_UINT,1,4094,0},\
    {pri2,CLI_UINT,0,7,32},\
	{svlan2,CLI_UINT,1,4094,0}",

    CLI_MODE_CONFIG_SYS,

    CLI_NULL_SUB_MODE,

    ACCESS_LEVEL_ADMINISTRATION,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdSystemIpAddrSet

},

{
    "interface {lan|wan} {admin|signal|media} mac mac-addr ",

    "{interface,0,Configure interface},\
    {lan,0,Configure LAN interface},\
    {wan,0,Configure WAN interface},\
    {admin,0,Enable administration interface},\
    {signal,0,Enable voip signal interface},\
    {media,0,Enable voip media interface},\
    {mac,0,Configure interface MAC address},\
    {mac-addr,1,Input MAC address(xx:xx:xx:xx:xx:xx)}",

    "{mac-addr,CLI_MACADDR,NULL,NULL,32}",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdSystemMacAddrSet

},
#elif defined(INTERFACE_TWO_WAN_TWO_LAN)
{
    "interface {lan|wan} {share|{independent {admin|signal}}}  \
     {{static [ip-address ipaddr net-mask mask gateway gwaddr dns dnsaddr]}|\
     dhcp|{pppoe user username password psword auth {auto|chap|pap}}} \
     [qos {{dscp|tos} qos-value}] \
     [tag-mode {transparent|{tag cvlan cvlan1 priority pri1}|\
     {vlan-stacking cvlan cvlan2 priority pri2 svlan svlan2}}]",

    "{interface,0,Configure interface},\
    {lan,0,Configure LAN interface},\
    {wan,0,Configure WAN interface},\
    {share,0,Configure interface for share mode},\
    {independent,0,Configure interface for independent mode },\
    {admin,0,Enable administration interface},\
    {signal,0,Enable voip signal interface},\
\
    {static,0,Configure static mode},\
    {ip-address,0,Configure IP address},\
    {ipaddr,1,Input IP address},\
    {net-mask, 0, Configure subnet mask},\
    {mask,1,Input subnet mask},\
    {gateway,0,Configure gateway},\
    {gwaddr, 1, Input gateway IP},\
    {dns,0,Configure primary DNS server IP},\
    {dnsaddr, 1, Input primary DNS server IP},\
\
    {dhcp, 0, Configure DHCP},\
\
	{pppoe, 0, Configure PPPoE},\
	{user, 0, Configure PPPoE username},\
	{username, 1, Input PPPoE username},\
	{password, 0, Configure PPPoE password},\
	{psword, 1, Input PPPoE password},\
	{auth, 0, Authentication type},\
	{auto, 0, Auto authentication type},\
	{pap, 0, PAP authentication type},\
	{chap, 0, CHAP authentication type},\
\
	{qos, 0, Configure QoS},\
	{dscp, 0, Configure DSCP},\
	{tos, 0, Configure TOS},\
	{qos-value, 1, Input DSCP or TOS value},\
\
	{tag-mode, 0, Configure tag mode},\
	{transparent, 0, Configure transparent mode},\
	{tag, 0, Configure tag mode},\
	{cvlan, 0, Configure CVLAN},\
	{cvlan1, 1, Input CVLAN tag},\
	{priority, 0, Configure priority},\
	{pri1, 1, Input priority},\
	{vlan-stacking, 0, Configure vlan-stacking mode},\
	{cvlan, 0, Configure CVLAN},\
	{cvlan2, 1, Input CVLAN tag},\
	{priority, 0, Configure priority},\
	{pri2, 1, Input priority},\
	{svlan, 0, Configure SVLAN},\
	{svlan2, 1, Input SVLAN tag}",

    "{ipaddr,CLI_IPDOTADDR,0.0.0.0,255.255.255.255,0.0.0.0},\
    {mask,CLI_IPDOTMASK,0.0.0.0,255.255.255.255,255.255.255.0},\
    {gwaddr,CLI_IPDOTADDR,0.0.0.0,255.255.255.255,0.0.0.0},\
    {dnsaddr,CLI_IPDOTADDR,0.0.0.0,255.255.255.255,0.0.0.0},\
\
    {username,CLI_WORD,NULL,NULL,32},\
	{psword,CLI_WORD,NULL,NULL,32},\
\
	{qos-value,CLI_UINT,0,255,0},\
\
	{cvlan1,CLI_UINT,1,4094,0},\
    {pri1,CLI_UINT,0,7,0},\
\
    {cvlan2,CLI_UINT,1,4094,0},\
    {pri2,CLI_UINT,0,7,32},\
	{svlan2,CLI_UINT,1,4094,0}",


    CLI_MODE_CONFIG_SYS,

    CLI_NULL_SUB_MODE,

    ACCESS_LEVEL_ADMINISTRATION,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdSystemIpAddrSet

},
{
    "interface {lan|wan} {admin|signal} mac mac-addr ",

    "{interface,0,Configure interface},\
    {lan,0,Configure LAN interface},\
    {wan,0,Configure WAN interface},\
    {admin,0,Enable administration interface},\
    {signal,0,Enable voip signal interface},\
    {mac,0,Configure interface MAC address},\
    {mac-addr,1,Input MAC address(xx:xx:xx:xx:xx:xx)}",

    "{mac-addr,CLI_MACADDR,NULL,NULL,32}",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdSystemMacAddrSet

},
#elif defined(INTERFACE_ONE_WAN_ONE_LAN)
{
    "interface {lan|wan} {share|{independent {admin}}}  \
     {{static [ip-address ipaddr net-mask mask gateway gwaddr dns dnsaddr]}|\
     dhcp|{pppoe user username password psword auth {auto|chap|pap}}} \
     [qos {{dscp|tos} qos-value}] \
     [tag-mode {transparent|{tag cvlan cvlan1 priority pri1}|\
     {vlan-stacking cvlan cvlan2 priority pri2 svlan svlan2}}]",

    "{interface,0,Configure interface},\
    {lan,0,Configure LAN interface},\
    {wan,0,Configure WAN interface},\
    {share,0,Configure interface for share mode},\
    {independent,0,Configure interface for independent mode },\
    {admin,0,Enable administration interface},\
\
    {static,0,Configure static mode},\
    {ip-address,0,Configure IP address},\
    {ipaddr,1,Input IP address},\
    {net-mask, 0, Configure subnet mask},\
    {mask,1,Input subnet mask},\
    {gateway,0,Configure gateway},\
    {gwaddr, 1, Input gateway IP},\
    {dns,0,Configure primary DNS server IP},\
    {dnsaddr, 1, Input primary DNS server IP},\
\
    {dhcp, 0, Configure DHCP},\
\
	{pppoe, 0, Configure PPPoE},\
	{user, 0, Configure PPPoE username},\
	{username, 1, Input PPPoE username},\
	{password, 0, Configure PPPoE password},\
	{psword, 1, Input PPPoE password},\
	{auth, 0, Authentication type},\
	{auto, 0, Auto authentication type},\
	{pap, 0, PAP authentication type},\
	{chap, 0, CHAP authentication type},\
\
	{qos, 0, Configure QoS},\
	{dscp, 0, Configure DSCP},\
	{tos, 0, Configure TOS},\
	{qos-value, 1, Input DSCP or TOS value},\
\
	{tag-mode, 0, Configure tag mode},\
	{transparent, 0, Configure transparent mode},\
	{tag, 0, Configure tag mode},\
	{cvlan, 0, Configure CVLAN},\
	{cvlan1, 1, Input CVLAN tag},\
	{priority, 0, Configure priority},\
	{pri1, 1, Input priority},\
	{vlan-stacking, 0, Configure vlan-stacking mode},\
	{cvlan, 0, Configure CVLAN},\
	{cvlan2, 1, Input CVLAN tag},\
	{priority, 0, Configure priority},\
	{pri2, 1, Input priority},\
	{svlan, 0, Configure SVLAN},\
	{svlan2, 1, Input SVLAN tag}",

    "{ipaddr,CLI_IPDOTADDR,0.0.0.0,255.255.255.255,0.0.0.0},\
    {mask,CLI_IPDOTMASK,0.0.0.0,255.255.255.255,255.255.255.0},\
    {gwaddr,CLI_IPDOTADDR,0.0.0.0,255.255.255.255,0.0.0.0},\
    {dnsaddr,CLI_IPDOTADDR,0.0.0.0,255.255.255.255,0.0.0.0},\
\
    {username,CLI_WORD,NULL,NULL,32},\
	{psword,CLI_WORD,NULL,NULL,32},\
\
	{qos-value,CLI_UINT,0,255,0},\
\
	{cvlan1,CLI_UINT,1,4094,0},\
    {pri1,CLI_UINT,0,7,0},\
\
    {cvlan2,CLI_UINT,1,4094,0},\
    {pri2,CLI_UINT,0,7,32},\
	{svlan2,CLI_UINT,1,4094,0}",


    CLI_MODE_CONFIG_SYS,

    CLI_NULL_SUB_MODE,

    ACCESS_LEVEL_ADMINISTRATION,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdSystemIpAddrSet

},

{
    "interface {lan|wan} {admin} mac mac-addr ",

    "{interface,0,Configure interface},\
    {lan,0,Configure LAN interface},\
    {wan,0,Configure WAN interface},\
    {admin,0,Enable administration interface},\
    {mac,0,Configure interface MAC address},\
    {mac-addr,1,Input MAC address(xx:xx:xx:xx:xx:xx)}",

    "{mac-addr,CLI_MACADDR,NULL,NULL,32}",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdSystemMacAddrSet

},
#endif

{
    "time datestr timestr",

    "{time,0,Set system date and time},\
    {datestr,1,Enter date string},\
    {timestr,1,Enter time string}",

    "{datestr,CLI_DATE,NULL,NULL,20},\
    {timestr,CLI_TIME,NULL,NULL,20}",

    CLI_MODE_CONFIG_SYS,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_ADMINISTRATION,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdSystemTimeSet

},

{
    "hostname name",

    "{hostname,0,Set host name},\
    {name,1,Enter host name}",

    "{name,CLI_WORD,NULL,NULL,20}",

    CLI_MODE_CONFIG_SYS,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_ADMINISTRATION,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdSystemHostNameSet

},

{
    "location name",

    "{location,0,Set host location},\
    {name,1,Enter location name}",

    "{name,CLI_WORD,NULL,NULL,64}",

    CLI_MODE_CONFIG_SYS,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_ADMINISTRATION,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdSystemLocationSet

},

{
    "timeout seconds",

    "{timeout,0,Set session timeouts},\
    {seconds,1,Enter seconds}",

    "{seconds,CLI_UINT,0,30000,180}",

    CLI_MODE_CONFIG_SYS,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_ADMINISTRATION,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdSystemTimeout

},

{
	"cpupath-mtu size",

	"{cpupath-mtu,0,Set Maximum CPU Interface (ethx) Transmission Unit},\
	{size,1,Enter size}",
	"{size,CLI_UINT,68,1500,1500}",

	CLI_MODE_CONFIG_SYS,

	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_ADMINISTRATION,

	CLI_EXEC_FUNC,

	(FUNCPTR)cliCmdSystemCPUPathMtu

},

{
    "kill {{session number}|all}",

    "{kill,0,Kill telnet session},\
    {session,0,Telnet session},\
    {number,1,Enter telnet session number},\
    {all,0,Kill all telnet sessions}",

    "{number,CLI_UINT,1,5,1}",

    CLI_MODE_CONFIG_SYS,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_ADMINISTRATION,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdSystemKillSession

},

{
    "show system",

    "{show, 0,Show running system information},\
    {system,0,Show system information}",

    "",

    CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_CONFIG_SYS | CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_GUEST,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdSystemShow
},

{
    "show configuration",

    "{show, 0,Show running system information},\
    {configuration,0,Show system configuration}",

    "",

    CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_CONFIG_SYS | CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

    ACCESS_LEVEL_GUEST,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdSystemShowCfg
},

{
    "paging {disable | enable}",

    "{paging, 0, enable or disable paging display}, \
    {disable, 0, disable paging display}, \
    {enable, 0, enable paging display}",

    "",

    CLI_MODE_CONFIG_SYS,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_ADMINISTRATION,

    CLI_EXEC_FUNC,
    (FUNCPTR)cliCmdSystemDividepage
},

{
    "save configuration",

    "{save, 0,Save running system information},\
    {configuration,0,System configuration}",

    "",

    CLI_MODE_CONFIG_SYS,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_ADMINISTRATION,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdSystemSave
},

{
    "erase configuration",

    "{erase, 0,Erase configuration},\
    {configuration,0,System configuration}",

    "",

    CLI_MODE_CONFIG_SYS,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_ADMINISTRATION,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdSystemErase
},

{
    "load script filename",

    "{load, 0,Load configuration script},\
    {script,0,Configuration script},\
    {filename,1,Input configuration script filename}",

    "{filename,CLI_WORD,NULL,NULL,128}",

    CLI_MODE_CONFIG_SYS,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_ADMINISTRATION,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdSystemLoadScript
},

{
    "user",

    "{user,0,Enter user configuration mode}",

    "",

    CLI_MODE_ENABLE | CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdUser
},

{
    "add user_name password pass_word {guest | user | admin}",

    "{add,0, Add a user},\
    {user_name,1,Input user name},\
    {password, 0, Configure password},\
    {pass_word,1,Input password},\
    {guest, 0, Access level guest},\
    {user, 0, Access level user},\
    {admin, 0, Access level administrator}",

    "{user_name,CLI_WORD,NULL,NULL,33},\
    {pass_word,CLI_WORD,NULL,NULL,33}",

    CLI_MODE_CONFIG_USER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_ADMINISTRATION,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdUserAdd
},

{
    "user user_name password pass_word",

    "{user,0,User configuration},\
    {user_name,1,Enter user name},\
    {password, 0, Configure password},\
    {pass_word,1,Enter password}",

    "{user_name,CLI_WORD,NULL,NULL,33},\
    {pass_word,CLI_WORD,NULL,NULL,33}",

    CLI_MODE_CONFIG_USER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_ADMINISTRATION,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdUserSet
},

{
    "delete user user_name",

    "{delete,0, Delete user},\
    {user,0,Delete user},\
    {user_name,1,Enter user name}",

    "{user_name,CLI_WORD,NULL,NULL,33}",

    CLI_MODE_CONFIG_USER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_ADMINISTRATION,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdUserDelete

},

{
    "show user list",

    "{show,0,Show running system information},\
    {user, 0,Show User configuration},\
    {list,0,List configuration}",

    "",

    CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_CONFIG_USER | CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_GUEST,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdUserShow
},


{
    "qos",

    "{qos,0,Enter QoS configuration mode}",

    "",

    CLI_MODE_ENABLE | CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdQoS
},
{
	"erase classify [all]",

	"{erase,0,delete all precedence classify}, \
	 {classify,0,classify},\
	 {all,0,all}",

	"",

	CLI_MODE_CONFIG_QOS,

	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

	CLI_EXEC_FUNC,

	(FUNCPTR)cliCmdClearAll

},

{
	"show running-config  qos [precedence]",

	"{show,0,show}, \
	 {running-config,0,running-config}, \
	 {qos,0,qos}, \
	 {precedence,0,precedence-only}",

	"",

	CLI_MODE_COMMON,

	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

	CLI_EXEC_FUNC,

(FUNCPTR)cliCmdShowRunQos

},

{
    "create class name",

    "{create,0,Create Qos class},\
     {class,0,Qos class},\
     {name,1,Input class name}",

    "{name,CLI_WORD,NULL,NULL,32}",

    CLI_MODE_CONFIG_QOS,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdQoSCreateClass

},
{
    "delete class name",

    "{delete,0,Delete Qos class},\
     {class,0,Qos class},\
     {name,1,Input class name}",

    "{name,CLI_WORD,NULL,NULL,32}",

    CLI_MODE_CONFIG_QOS,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdQoSDeleteClass

},
{
	"delete precedence value",

	"{delete,0,Delete Qos class},\
	 {precedence,0,rule precedence},\
	 {value,1,Input rule pri}",

    "{value,CLI_UINT,1,8,1}",

	CLI_MODE_CONFIG_QOS,

	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

	CLI_EXEC_FUNC,

	(FUNCPTR)cliCmdQoSDeleteRulePre

},

{
    "show class name value",

    "{show, 0,Show class information},\
     {class,0,Show Qos class},\
     {name,0,Class name},\
     {value,1,Input class name}",

    "{value,CLI_WORD,NULL,NULL,32}",

    CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_CONFIG_QOS | CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdQoSShowClass
},
{
    "show class list",

    "{show, 0,Show class list},\
     {class,0,Show Qos class},\
     {list,0,Class name list}",

    "",

    CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_CONFIG_QOS | CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdQoSShowClassList
},
{
    "show class all",

    "{show, 0,Show class list},\
     {class,0,Show Qos class},\
     {all,0,All class}",

    "",

    CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_CONFIG_QOS | CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdQoSShowClassAll
},
{
    "class name",

    "{class,0,Enter QoS class configuration mode},\
    {name,1,Input class name}",

    "{name,CLI_WORD,NULL,NULL,32}",

    CLI_MODE_CONFIG_QOS,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdQoSClassMode

},
{
    "show ",

    "{show, 0,Show class information}",

    "",

    CLI_MODE_CONFIG_QOS_CLASS,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdQoSShowClass
},
{
    "match {dstmac|srcmac} macaddress",

    "{match,0,Match class config},\
     {dstmac,0,Destination mac},\
     {srcmac,0,Source mac},\
     {macaddress,1,Input mac address(xx:xx:xx:xx:xx:xx)}",

    "{macaddress,CLI_MACADDR,NULL,NULL,32}",

    CLI_MODE_CONFIG_QOS_CLASS,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdQoSAddClassMac

},
{
    "match {dstipv4|srcipv4} ipaddress",

    "{match,0,Match class config},\
     {dstipv4,0,Destination ip},\
     {srcipv4,0,Source ip},\
     {ipaddress,1,Input ip address(xx.xx.xx.xx)}",

    "{ipaddress,CLI_IPDOTADDR,0.0.0.0, 255.255.255.255, 0.0.0.0}",

    CLI_MODE_CONFIG_QOS_CLASS,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdQoSAddClassIp

},
{
    "match userpriority cos",

    "{match,0,Match class config},\
     {userpriority,0,User priority},\
     {cos,1,Input priority value(0-7)}",

    "{cos,CLI_UINT,0,7,0}",

    CLI_MODE_CONFIG_QOS_CLASS,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdQoSAddClassCos

},
{
    "match vlanid vid",

    "{match,0,Match class config},\
     {vlanid,0,VLAN id},\
     {vid,1,Input vlan id(1-4094)}",

    "{vid,CLI_UINT,1,4094,32}",

    CLI_MODE_CONFIG_QOS_CLASS,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdQoSAddClassVlan

},
{
    "match ethertype type",

    "{match,0,Match class config},\
     {ethertype,0,Ethernet type},\
     {type,1,Input ether type (1-0xffff)}",

    "{type,CLI_UINT,1,65535,1}",

    CLI_MODE_CONFIG_QOS_CLASS,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdQoSAddClassEthType

},
{
    "match ipprotocol protocol",

    "{match,0,Match class config},\
     {ipprotocol,0,IP protocol},\
     {protocol,1,Input protocol value(0-255)}",

    "{protocol,CLI_UINT,0,255,32}",

    CLI_MODE_CONFIG_QOS_CLASS,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdQoSAddClassIpType

},
{
    "match dscp value",

    "{match,0,Match class config},\
     {dscp,0,Ip Dscp},\
     {value,1,Input dscp value (0-63)}",

    "{value,CLI_UINT,0,63,32}",

    CLI_MODE_CONFIG_QOS_CLASS,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdQoSAddClassDscp

},
{
    "match tos value",
    "{match,0,Match class config},\
     {tos,0,Ip Tos},\
     {value,1,Input tos value (0-7)}",

    "{value,CLI_UINT,0,7,32}",

    CLI_MODE_CONFIG_QOS_CLASS,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdQoSAddClassTos

},

{
    "match {l4dstport|l4srcport} port",

    "{match,0,Match class config},\
     {l4dstport,0,Layer4 destination port number},\
     {l4srcport,0,Layer4 source port number},\
     {port,1,Input l4 port value (0-65535)}",

    "{port,CLI_UINT,0,65535,32}",

    CLI_MODE_CONFIG_QOS_CLASS,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdQoSAddClassL4Port

},

{
    "exclsmatchadd  { eport | iport | insertvid | tpid | ipversion}  paranum  num { drop | not_drop | copy2cpu}  [classitem  classItemID] ",
    "{exclsmatchadd,0, for add extend classify match debug},\
     {eport,0,match eport},\
     {iport,0,match iport},\
     {insertvid,0,insert vlanid for untag packet},\
     {tpid,0,match first tpid},\
     {ipversion,0,match ipversion},\
     {paranum,0,paranum},\
     {num,1, input paranum},\
     {drop,0,action is drop},\
     {not_drop,0,action is don't drop},\
     {copy2cpu,0,action is copy to cpu},\
     {classitem,0,classitem},\
     {classItemID,1,Input classItemID}",

    "{num,CLI_UINT,0,65535,0},\
     {classItemID,CLI_UINT,89,93,91}",
    CLI_MODE_SUPER,
    CLI_NULL_SUB_MODE,
    ACCESS_LEVEL_SUPER,
    CLI_EXEC_FUNC,
    (FUNCPTR)cliExClsMatchAdd
},
{
	"exclsmatchdel  { eport | iport | insertvid | tpid | ipversion}  [ classitem  classItemID ]",

	"{exclsmatchdel, 0, for delete extend classify match },\
	{eport, 0, match eport},\
	{iport, 0, match iport},\
	{insertvid, 0, insert vlanid for untag packet},\
	{tpid, 0, match first tpid},\
	{ipversion, 0, match ipversion},\
	{classitem, 0,  classitem},\
	{classItemID, 1,  classItemID}",


	"{classItemID,CLI_UINT,89,93,93}",

	CLI_MODE_SUPER,

	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

	CLI_EXEC_FUNC,
	(FUNCPTR)cliExClsMatchDel
},

{
    "no match {dstmac|srcmac|l4dstport|l4srcport|\
              dstipv4|srcipv4|userpriority|vlanid|ethertype|ipprotocol|tos|dscp}",

    "{no,0,No config class option},\
     {match,0,Clear class match option)},\
     {dstmac,0,Destination mac},\
     {srcmac,0,Source mac},\
     {l4dstport,0,Layer4 destination port number},\
     {l4srcport,0,Layer4 source port number},\
     {dstipv4,0,Destination ip},\
     {srcipv4,0,Source ip},\
     {userpriority,0,User priority},\
     {vlanid,0,VLAN id},\
     {ethertype,0,Ethernet type},\
     {ipprotocol,0,IP protocol},\
     {tos,0,Ip Tos},\
     {dscp,0,Ip Dscp}",

    "",

    CLI_MODE_CONFIG_QOS_CLASS,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdQoSDelClassPra

},
{
	"set queue queueid",

	"{set,0,Set class policy},\
	 {queue,0,Egress queue},\
	 {queueid,1,Input queue value (0-7)}",

#if 0
	"{queueid,CLI_UINT,0,3,0}",
#else
	"{queueid,CLI_UINT,0,7,0}",
#endif

	CLI_MODE_CONFIG_QOS_CLASS,

	CLI_NULL_SUB_MODE,

    ACCESS_LEVEL_USER,

	CLI_EXEC_FUNC,

	(FUNCPTR)cliCmdQoSAddClassRemarkQueue

},
{
	"set ethernetpriority priority",

	"{set,0,Set class policy},\
	 {ethernetpriority,0,Ethernet priority},\
	 {priority,1,Input priority value (0-7)}",

	"{priority,CLI_UINT,0,7,0}",

	CLI_MODE_CONFIG_QOS_CLASS,

	CLI_NULL_SUB_MODE,

    ACCESS_LEVEL_USER,

	CLI_EXEC_FUNC,

	(FUNCPTR)cliCmdQoSAddClassRemarkEtherPri

},
{
	"set precedence rulepri",

	"{set,0,Set class policy},\
	 {precedence,0,Ethernet precedence},\
	 {rulepri,1,Input rulepri value (1-8)}",

	"{rulepri,CLI_UINT,1,8,1}",

	CLI_MODE_CONFIG_QOS_CLASS,

	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

	CLI_EXEC_FUNC,

	(FUNCPTR)cliCmdQoSAddClassRemarkRulePri

},
{
    "set action {pass|drop|copytocpu|sendtocpu|remarkvid vid}",

    "{set,0,Set class policy},\
     {action,0,Set class action option},\
     {pass,0,Direct pass},\
     {drop,0,Direct drop},\
     {copytocpu,0,Copy to cpu},\
     {sendtocpu,0,Send to cpu},\
	 {remarkvid,0,remark vid},\
	 {vid,1,vlan id}",

    "{vid,CLI_UINT,1,4094,1}",

    CLI_MODE_CONFIG_QOS_CLASS,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdQoSAddClassAct

},
{
    "no set {queue|ethernetpriority|action|remarkvid}",

    "{no,0,No config class option},\
     {set,0,Clear class policy option)},\
     {queue,0,Egress queue},\
     {ethernetpriority,0,Egress ethernet priority},\
     {action,0,Egress Action},\
	 {remarkvid,0,remark vid}",

    "",

    CLI_MODE_CONFIG_QOS_CLASS,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdQoSDelClassAct

},
#if 0
{
	"apply portlist",

	"{apply,0,Enter port configuration mode},\
	 {portlist,1,Enter portlist;<1,2-3,4>}",

	"{portlist,CLI_WORD,NULL,NULL,32}",

	CLI_MODE_CONFIG_QOS_CLASS,

	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

	CLI_EXEC_FUNC,

	(FUNCPTR)cliCmdQoSClassApply

},
#else
{
	"apply [upstream|downstream]",

	"{apply,0,Apply class},\
	 {upstream,0,Match upstream only},\
	 {downstream,0,Match downstream only}",

	"",
	CLI_MODE_CONFIG_QOS_CLASS,

	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

	CLI_EXEC_FUNC,

	(FUNCPTR)cliCmdQoSClassApply

},
#endif

{
    "clear ",

    "{clear,0,Clear class apply}",

    "",
    CLI_MODE_CONFIG_QOS_CLASS,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdQoSClassRemove

},
{
    "fdb",

    "{fdb,0,Enter FDB configuration mode}",

    "",

    CLI_MODE_ENABLE | CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdFDB

},

#if defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
{
    "mirror-group mirror-group-name",

    "{mirror-group,0,Enter mirror group configuration mode},\
    {mirror-group-name,1,Input mirror group name(1-32 characters)}",

    "{mirror-group-name,CLI_WORD,NULL,NULL,32}",

    CLI_MODE_ENABLE | CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdMirrorGroupMode

},

{
    "mirroring-port mirroring-port-list {both|ingress|egress} monitor-port monitor-port-list",

    "{mirroring-port,0,Mirroring-port},\
     {mirroring-port-list,1,"PORTLIST_HELP"},\
     {both,0,Both},\
     {ingress,0,Ingress },\
     {egress,0,Egress },\
     {monitor-port,0,Monitor-port },\
     {monitor-port-list,1,"PORTLIST_HELP" }",

    "{mirroring-port-list,CLI_WORD,NULL,NULL,32} ,\
     {monitor-port-list,CLI_WORD,NULL,NULL,32}",

    CLI_MODE_CONFIG_MIRROR,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdMirrorPortApply

},

{
    "clear ",

    "{clear,0,Clear mirror config}",

    "",
    CLI_MODE_CONFIG_MIRROR,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdMirrorClear

},

{
    "show port-mirror all",

    "{show,0, Show port mirror entries},\
     {port-mirror,0,Port mirror},\
     {all,0,All}",

    "",

    CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_CONFIG_PORT | CLI_MODE_CONFIG_MIRROR | CLI_MODE_SUPER,
    CLI_NULL_SUB_MODE,
	ACCESS_LEVEL_USER,
    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdShowPortMirrorAll
},
#endif

{
    "multicast",

    "{multicast,0,Enter multicast configuration mode}",

    "",

    CLI_MODE_ENABLE | CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdMcast

},

{
    "vlan",

    "{vlan,0,Enter VLAN configuration mode}",

    "",

    CLI_MODE_ENABLE | CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdVLAN

},

{
    "create vlan name",

    "{create,0,Create VLAN name},\
     {vlan,0,Vlan},\
     {name,1,Input VLAN name}",

    "{name,CLI_WORD,NULL,NULL,32}",

    CLI_MODE_CONFIG_VLAN,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdCreateVlanName

},
{
    "delete vlan name",

    "{delete,0,Delete VLAN attribute},\
     {vlan,0,Delete VLAN name},\
     {name,1,Input VLAN name}",

    "{name,CLI_WORD,NULL,NULL,32}",

    CLI_MODE_CONFIG_VLAN,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdDeleteVlanName

},
{
    "clear vlan ",

    "{clear,0,Clear all VLAN },\
     {vlan,0,Vlan}",

    "",

    CLI_MODE_CONFIG_VLAN,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdClearVlan

},
{
    "vlan name tag vid",

    "{vlan,0,Add VlAN tag},\
     {name,1,Input VLAN name},\
     {tag,0,Tag},\
     {vid,1,Input VLAN ID(1-4094)}",

    "{name,CLI_WORD,NULL,NULL,32} ,\
     {vid,CLI_UINT,1,4094,32}",

    CLI_MODE_CONFIG_VLAN,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdSetVlanTag

},

{
    "add port portlist vlan name mode {tag|untag|unmodify}",

    "{add,0,Add VLAN port member},\
     {port,0,Port},\
     {portlist,1,"PORTLIST_HELP"},\
     {vlan,0,Vlan},\
     {name,1,Input VLAN name},\
     {mode,0,Mode},\
     {tag,0,Tag},\
     {untag,0,Untag },\
     {unmodify,0,Unmodify }",

    "{portlist,CLI_WORD,NULL,NULL,32} ,\
     {name,CLI_WORD,NULL,NULL,32}",

    CLI_MODE_CONFIG_VLAN,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdAddVlanPort

},
{
    "delete port portlist vlan name ",

    "{delete,0,Delete VLAN attribute},\
     {port,0,Delete VLAN port member},\
     {portlist,1,"PORTLIST_HELP"},\
     {vlan,0,VLAN },\
     {name,1,Input VLAN name}",


    "{portlist,CLI_WORD,NULL,NULL,32} ,\
     {name,CLI_WORD,NULL,NULL,32}",

    CLI_MODE_CONFIG_VLAN,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdDelVlanPort

},

{
    "vlan-aggregation {create|delete} port portlist targetvlan targetvlan-id aggregatedvlan aggregatedvlan-id",

    "{vlan-aggregation, 0, Configure VLAN N:1 aggregation}, \
     {create, 0, Create a vlan-aggregation}, \
     {delete, 0, Delete a vlan-aggregation}, \
     {port, 0, Input VLAN port member},\
     {portlist,1,"PORTLIST_HELP"},\
     {targetvlan, 0, Target VLAN}, \
     {targetvlan-id, 1, Input target VLAN ID}, \
     {aggregatedvlan, 0, Aggregated VLAN}, \
     {aggregatedvlan-id, 1, Input aggregated VLAN ID}",


    "{portlist,CLI_WORD,NULL,NULL,32} ,\
    {targetvlan-id, CLI_UINT, 1, 4094, 1}, \
    {aggregatedvlan-id, CLI_UINT, 1, 4094, 1}",

    CLI_MODE_CONFIG_VLAN,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdVlanAggAction

},

{
    "show aggregation all",

    "{show,0, Show VLAN aggregation entries},\
     {aggregation,0,VLAN aggregation},\
     {all,0,All}",

    "",

    CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_CONFIG_VLAN | CLI_MODE_SUPER,
    CLI_NULL_SUB_MODE,
	ACCESS_LEVEL_USER,
    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdShowVlanAggAll
},

{
    "show aggregation port portlist",

    "{show,0, Show VLAN aggregation entries},\
     {aggregation,0,VLAN aggregation},\
     {port, 0, Delete VLAN port member},\
     {portlist,1,"PORTLIST_HELP"}",


    "{portlist,CLI_WORD,NULL,NULL,32}",

    CLI_MODE_COMMON,
    CLI_NULL_SUB_MODE,
	ACCESS_LEVEL_USER,
    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdShowVlanAgg
},

{
    "show vlan {all|name value}",

    "{show,0,Show VLAN information},\
     {vlan,0,Show VLAN information},\
     {all,0,Show all VLAN information},\
	 {name,0,Show specific VLAN entry},\
	 {value,1,Input VLAN name}",

	"{value,CLI_WORD,NULL,NULL,256}",

    CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_CONFIG_VLAN | CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdShowVlan

},

{
    "port portlist",
    "{port,0,Enter port configuration mode},\
     {portlist,1,"PORTLIST_HELP"}",
    "{portlist,CLI_WORD,NULL,NULL,32}",

    CLI_MODE_ENABLE | CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPort

},

{
    "admin {enable|disable}",

    "{admin,0,Enable or disable admin status},\
     {enable,0,Enable admin status},\
     {disable,0,Disable admin status}",

	"",

    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortAdminEnable

},

{
    "auto {enable|disable}",

    "{auto,0,Enable of disable autonegotiation},\
     {enable,0,Enable auto negotiation},\
     {disable,0,Disable auto negotiation}",

	"",

    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortAutoEnable

},

{
    "speed {10|100}",

    "{speed,0,Select a speed (Mbps)},\
     {10,0,Select 10 mbps},\
     {100,0,Select 100 mbps}",

	"",

    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortSpeedSelect

},

{
    "duplex {full|half}",

    "{duplex,0,Select duplex mode},\
     {full,0,Select full duplex},\
    	{half,0,Select half duplex}",

	"",

	CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortDuplexSelect

},

{
    "flowcontrol {enable|disable}",

    "{flowcontrol,0,Enable or disable flow control},\
     {enable,0,Enable the flowcontrol},\
     {disable,0,Disable the flowcontrol}",

	"",

    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortFlowControlEnable
},

{
    "loopback {enable|disable}",
#if defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
    "{loopback,0,Enable or disable port loop back(internal)},\
     {enable,0,Enable the loop back},\
     {disable,0,Disable the loop back}",
#else
    "{loopback,0,Enable or disable port loop back(external)},\
     {enable,0,Enable the loop back},\
     {disable,0,Disable the loop back}",
#endif
	"",

    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortLoopbackEnable

},

{
    "learning {disable|enable}",

    "{learning,0,Enable or disable per port learnning},\
     {disable,0,Disable per port learning},\
     {enable,0,Enable per port learning}",

	"",

    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortLearningEnable

},


{
    "maclimit enable num",

    "{maclimit,0,Enable mac limit and specify num of mac the port can learn},\
     {enable,0,Enable maclimit},\
     {num,1,Num of limit}",

    "{num,CLI_UINT,0,64,64}",

    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortMacLimitNumEnable

},

{
    "maclimit disable",

    "{maclimit,0,Disable mac limit for port or ports},\
     {disable,0,Disable maclimit}",


	"",

    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortMacLimitNumDisable

},

{
    "pri-weight {upstream|downstream} qid pri pri_val weight weight_val",
    "{pri-weight,0,Configure qid priority and weight},\
     {upstream,0,Upstream},\
     {downstream,0,Downstream},\
     {qid,1,Entry queue},\
     {priority,0,Configure priority},\
     {pri_val,1,Priority value}\
     {weight,0,Configure weight},\
     {weight_val,1,Weight value}",
    "{qid,CLI_UINT,0,7,0},\
     {pri_val,CLI_UINT,0,7,0},\
     {weight_val,CLI_UINT,0,255,0}",

    CLI_MODE_SUPER,
    CLI_NULL_SUB_MODE,
    ACCESS_LEVEL_SUPER,
    CLI_EXEC_FUNC,
    (FUNCPTR)cliCmdPortQueuePriWeightSet
},

#if defined(ONU_1PORT)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
{
    "uspolicing {enable|disable}",

    "{uspolicing,0,Enable or disable upstream policing},\
     {enable,0,Enable},\
     {disable,0,Disable}",

  	"",

    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortUspolicingEnable

},

{
    "uscir uscirvalue",

#if defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)
   "{uscir,0,Set the upstream commit info rate},\
	{uscirvalue,1,The commit infomation rate value (kbps); step is 32kbps}",
#elif defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)
			"{uscir,0,Set the upstream commit info rate},\
			 {uscirvalue,1,The commit infomation rate value (kbps). Step:<1M:64K;<10M:128K;<100M:1M}",
#else
    "{uscir,0,Set the upstream commit info rate},\
     {uscirvalue,1,The commit infomation rate value (kbps) test}",
#endif

#if defined(ONU_1PORT)||defined(ONU_4PORT_AR8327)
   "{uscirvalue,CLI_UINT,32,1000000,1000000}",
#elif defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)
   "{uscirvalue,CLI_UINT,1,100000,100000}",
#elif defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)
   "{uscirvalue,CLI_UINT,32,100000,100000}",
#endif

    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortUscirConfig

},

#if defined(ONU_1PORT) || defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)
{
    "uscbs uscbsvalue",

    "{uscbs,0,Set the upstream commit burst size},\
     {uscbsvalue,1,The commit burst size value (byte)}",

#if defined(ONU_1PORT)
   "{uscbsvalue,CLI_UINT,8000,256000,256000}",
#elif defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)
   "{uscbsvalue,CLI_UINT,1,16777215,16777215}",
#endif

    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortUscbsConfig

},
#endif

#if defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)
{
    "usebs usebsvalue",

    "{usebs,0,Set the upstream exceed burst size},\
     {usebsvalue,1,The exceed burst size value (byte)}",

    "{usebsvalue,CLI_UINT,1,16777215,16777215}",

    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortUsebsConfig

},
#endif

#elif defined(ONU_4PORT_88E6045)

#endif
{
    "dsratelimit {enable|disable}",

    "{dsratelimit,0,Enable or disable downstream rate limit},\
     {enable,0,Enable},\
     {disable,0,Disable}",

	"",

    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortDsRateLimitEnable

},

{
    "dscir dscirvalue",

    "{dscir,0,Set downstream commit info rate},\
     {dscirvalue,1,The CIR value (kbps)}",
#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)
  "{dscirvalue,CLI_UINT,32,100000,100000}",
#else
  "{dscirvalue,CLI_UINT,32,1000000,1000000}",
#endif

    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortDscirConfig

},
#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
#else
{
    "dscbs dscbsvalue",

    "{dscbs,0,Set downstream commit burst size},\
     {dscbsvalue,1,The downstream cbs value}",

   "{dscbsvalue,CLI_UINT,8000,256000,256000}",

    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortDsCbsConfig

},
#endif
#if 0
{
    "tagstrip {enable|disable}",

    "{tagstrip,0,Strip tag when down stream for multicast},\
     {enable,0,Strip enable},\
     {disable,0,No strip tag}",

   "",

    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortMulticastTagStripEnable

},

{
    "maxgroup numval",

    "{maxgroup,0,Set max multicast group for the port of ports},\
     {numval,1,Num}",

   "{numval,CLI_UINT,0,8,8}",

    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortMaxMulticastGroupNumConfig

},
#endif

{
    "vlanmode transparent",

    "{vlanmode,0,Set vlan mode for the port or ports},\
     {transparent,0,Transparent mode},",

	"",

    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortVlanModeConfigTransparent

},

{
    "vlanmode tag vlan vlanvalue",

    "{vlanmode,0,Set vlan mode for the port or ports},\
     {tag,0,Tag mode},\
     {vlan,0,Set port vlan},\
     {vlanvalue,1,<1-4094>}",

	"{vlanvalue,CLI_UINT,1,4094,1}",

    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortVlanModeConfigTag

},

#if defined(ONU_4PORT_AR8228) || defined(ONU_1PORT)||defined(ONU_4PORT_AR8327)
{
    "vlanmode trunk defaultvlan vlanid",

    "{vlanmode,0,Set port to work in vlan trunk mode},\
     {trunk,0, Set port to work in vlan trunk mode},\
     {defaultvlan,0,Set default vlan id},\
     {vlanid,1, Input default vlan id <1-4094>}",

    "{vlanid,CLI_UINT,1,4094,1}",

    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortVlanModeConfigTrunk

},
#endif

#if defined(ONU_4PORT_88E6046)|| defined(ONU_4PORT_88E6045) || defined(ONU_4PORT_88E6097)
	/*not support vlan translation*/
#else
{
    "vlanmode translation defaultvlan value",

    "{vlanmode,0,Set vlan mode for the port or ports},\
     {translation,0,Translation mode},\
     {defaultvlan,0,Defaultvlan},\
     {value,1,Default vlan value}",

	"{vlanvalue,CLI_UINT,1,4094,1}",

    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortVlanModeConfigTranslation

},
#endif

#if defined(ONU_1PORT) || defined(ONU_4PORT_AR8228) ||defined(ONU_4PORT_AR8327)
{
    "vlanmode aggregation defaultvlan value",

    "{vlanmode,0,Set vlan mode for the port or ports},\
     {aggregation,0,Aggregation mode},\
     {defaultvlan,0,Defaultvlan},\
     {value,1,Default vlan value}",

  "{vlanvalue,CLI_UINT,1,4094,1}",

    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortVlanModeConfigAggregation

},
#endif

#if defined(ONU_4PORT_AR8228) || defined(ONU_1PORT)||defined(ONU_4PORT_AR8327)
{
    "add vlantrunk vlanid",

    "{add,0,Add a vlan trunk entry},\
     {vlantrunk,0,Add a vlan trunk entry},\
	{vlanid,1,Input vlanid}",

	"{vlanid,CLI_UINT,1,4094,1}",

    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortVlanTrunkAdd

},

{
    "del vlantrunk vlanid",

    "{del,0,Del a vlan trunk entry},\
     {vlantrunk,0, Del a vlan trunk entry},\
	{vlanid,1,Input vlanid}",

	"{vlanid,CLI_UINT,1,4094,1}",

    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortVlanTrunkDel

},
#endif

{
    "add vlanmapping cvlan svlan",

    "{add,0,Add a vlan translation mapping entry},\
     {vlanmapping,0,Customer vlan, system vlan},\
	{cvlan,1,Customer vlan},\
	{svlan,1,System vlan}",

	"{cvlan,CLI_UINT,1,4094,1}\
	{svlan,CLI_UINT,1,4094,1}",

    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortVttAdd

},

{
    "del vlanmapping oldvlan newvlan",

    "{del,0,Del a vlan translation mapping entry},\
     {vlanmapping,0,Oldvlan newvlan},\
	{oldvlan,1,Oldvlan},\
	{newvlan,1,Newvlan}",

	"{oldvlan,CLI_UINT,1,4094,1},\
	{newvlan,CLI_UINT,1,4094,1}",

    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortVttDel

},

{
    "show port {{list portlist}|all} {state|configuration|maclimit|ratelimit|vlan|vlanmapping|vlantrunk|mac {bind|filter}}",

    "{show,0,Show running system information},\
     {port,0,Show port information},\
     {list,0,Port list keyword},\
     {portlist,1,"PORTLIST_HELP"},\
     {all,0,All ports},\
     {state,0,Port state information},\
	 {configuration,0,Port configuration information},\
	 {maclimit,0,Port mac limit configuration},\
	 {ratelimit,0,Rate limit information},\
	 {vlan,0,VLAN configuration},\
	 {vlanmapping,0,VLAN mapping},\
	 {vlantrunk,0,VLAN trunk},\
 	{mac,0,mac information},\
    {bind,0,bind configuration},\
    {filter,0,filter configuration}",

	"{portlist,CLI_WORD,NULL,NULL,256}",
    CLI_MODE_COMMON,
    CLI_NULL_SUB_MODE,
	ACCESS_LEVEL_USER,
    CLI_EXEC_FUNC,
    (FUNCPTR)cliCmdPortListMiscShow
},


/* rstp command line */
#ifndef ONU_1PORT
{
    "rstp",

    "{rstp,0,Enter RSTP configuration mode}",

    "",

    CLI_MODE_ENABLE | CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdRSTP

},
/* begin: add by jiangmingli for RSTP process */
#if 0
{
	"show rstp",
	"{show, 0,Show system information},\
	{rstp,0,Show rstp information}",

	"",

	CLI_MODE_USER |  CLI_MODE_SUPER | CLI_MODE_ENABLE | CLI_MODE_CONFIG_RSTP | CLI_MODE_SUPER,
	CLI_NULL_SUB_MODE,
	ACCESS_LEVEL_USER,
	CLI_EXEC_FUNC,
	(FUNCPTR)cliCmdRstpPort
},
#endif
{
	"show rstp info",
	"{show, 0,Show rstp information},\
	{rstp,0,Show rstp information},\
	{info,0,rstp information}",

	"",

	CLI_MODE_COMMON,
	CLI_NULL_SUB_MODE,
	ACCESS_LEVEL_USER,
	CLI_EXEC_FUNC,
	(FUNCPTR)cliCmdRstpPort
},
{
	"show rstp cfg",
	"{show,0,Show rstp configuration},\
	{rstp,0,Show rstp configuration},\
	{cfg,0,Configure information}",

	"",

	CLI_MODE_COMMON,
	CLI_NULL_SUB_MODE,
	ACCESS_LEVEL_USER,
	CLI_EXEC_FUNC,
	(FUNCPTR)cliCmdRstpCfgShow
},
/* end: add by jiangmingli for RSTP process */
{
	"admin {disable|enable}",
	"{admin,0,Enable or disable rstp protocol},\
	{disable,0,Disable rstp protocol},\
	{enable,0,Enable rstp protocol}",

	"",

	CLI_MODE_CONFIG_RSTP,
	CLI_NULL_SUB_MODE,
	ACCESS_LEVEL_USER,
	CLI_EXEC_FUNC,
	(FUNCPTR)cliCmdRstpEnable
},

#if 0
{
    "one-port-loopdetect {disable|enable}",
    "{one-port-loopdetect,0,Enable or disable one-port-loopdetect},\
    {disable,0,Disable one-port-loopdetect},\
    {enable,0,Enable one-port-loopdetect}",

    "",

    CLI_MODE_CONFIG_RSTP,
    CLI_NULL_SUB_MODE,
    ACCESS_LEVEL_USER,
    CLI_EXEC_FUNC,
    (FUNCPTR)cliCmdRstpLoopDetect
},
#else
{
    "loop-detect port {all|{list port-id}} {disable|enable hold-down-time time-val}",

    "{loop-detect,0,Enable or disable loopdetect},\
    {port,0,port},\
    {all,0,All port},\
    {list,0,Port list},\
    {port-id,1,port-id},\
    {disable,0,disable loopdetect},\
    {enable,0,enable loopdetect},\
    {hold-down-time,0,holddowntime},\
    {time-val,1,input time seconds}",

    "{port-id, CLI_WORD, NULL, NULL, 256}\
    {time-val,CLI_UINT,1,300,300}",

    CLI_MODE_CONFIG_RSTP,
    CLI_NULL_SUB_MODE,
    ACCESS_LEVEL_USER,
    CLI_EXEC_FUNC,
    (FUNCPTR)cliCmdRstpLoopDetect
},
#endif

{
	"timer hello value",

	"{timer,0,Set rstp timer},\
	{hello,0,Set rstp hello time},\
	{value,1,Hello time value}",

	"{value,CLI_UINT,1,10,2}",

	CLI_MODE_CONFIG_RSTP,
	CLI_NULL_SUB_MODE,
	ACCESS_LEVEL_USER,
	CLI_EXEC_FUNC,
	(FUNCPTR)cliCmdRstpSetHello
},
{
	"timer forward-delay value",

	"{timer,0,Set rstp timer},\
	{forward-delay,0,Set rstp forward delay time},\
	{value,1,Forward delay time value}",

	"{value,CLI_UINT,4,30,15}",

	CLI_MODE_CONFIG_RSTP,
	CLI_NULL_SUB_MODE,
	ACCESS_LEVEL_USER,
	CLI_EXEC_FUNC,
	(FUNCPTR)cliCmdRstpSetForward
},
{
	"timer max-age value",

	"{timer,0,Set rstp timer},\
	{max-age,0,Set rstp max age},\
	{value,1,Max age value}",

	"{value,CLI_UINT,6,40,20}",

	CLI_MODE_CONFIG_RSTP,
	CLI_NULL_SUB_MODE,
	ACCESS_LEVEL_USER,
	CLI_EXEC_FUNC,
	(FUNCPTR)cliCmdRstpSetMaxage
},
{
	"mode {stp|rstp}",

	"{mode,0,Set rstp mode},\
	{stp,0,Run stp compatible mode},\
	{rstp,0,Run rstp mode}",

	"",

	CLI_MODE_CONFIG_RSTP,
	CLI_NULL_SUB_MODE,
	ACCESS_LEVEL_USER,
	CLI_EXEC_FUNC,
	(FUNCPTR)cliCmdRstpSetForver
},
{
	"priority value",

	"{priority,0,Set rstp bridge priority},\
	{value,1,Bridge priority value}",

	"{value,CLI_UINT,0,15,8}",

	CLI_MODE_CONFIG_RSTP,
	CLI_NULL_SUB_MODE,
	ACCESS_LEVEL_USER,
	CLI_EXEC_FUNC,
	(FUNCPTR)cliCmdRstpSetPriority
},
{
	"diameter value",

	"{diameter,0,Set rstp diameter},\
	{value,1,Diameter value}",

	"{value,CLI_UINT,2,7,7}",

	CLI_MODE_CONFIG_RSTP,
	CLI_NULL_SUB_MODE,
	ACCESS_LEVEL_USER,
	CLI_EXEC_FUNC,
	(FUNCPTR)cliCmdRstpSetDiameter
},
{
	"port {{list portlist}|all} {disable|enable|{edged-port {disable|enable}}|{point-to-point {force-true|force-false|auto}}|{priority pri-value}|{pathcost pcost-value}}",

	"{port,0,Set rstp port parameter},\
	{list,0,Port list},\
	{portlist,1,"PORTLIST_HELP"},\
	{all,0,All port},\
	{disable,0,Remove portlist from rstp instance},\
	{enable, 0,Add portlist into rstp instance},\
	{edged-port,0,Set port edge port or auto edge port},\
	{disable,0,Set port auto edge port},\
	{enable, 0,Set port edge port},\
	{point-to-point,0,Set port point-to-point parameter},\
	{force-true,0,Set port point-to-point port},\
	{force-false,0,Set port point-to-multipoint port},\
	{auto,0,Set port auto-decided point-to-point port},\
	{priority,0,Set port priority parameter},\
	{pri-value,1,Port priority value},\
	{pathcost,0,Set port path cost},\
	{pcost-value,1,Port path cost value}",

	"{portlist,CLI_WORD,NULL,NULL,256},\
	{pri-value,CLI_UINT,0,15,8},\
	{pcost-value,CLI_UINT,1,20000000,20000}",

	CLI_MODE_CONFIG_RSTP,
	CLI_NULL_SUB_MODE,
	ACCESS_LEVEL_USER,
	CLI_EXEC_FUNC,
	(FUNCPTR)cliCmdRstpPortSet
},

#if 0
{
	"port {portid|all} {disable|enable}",

	"{port,0,set rstp port parameter},\
	{portid,1,port id},\
	{all,0,all port},\
	{disable,0,remove portlist from rstp instance},\
	{enable, 0,add portlist into rstp instance}",

	"{portid,CLI_UINT,1,4,1}",

	CLI_MODE_CONFIG_RSTP,
	CLI_NULL_SUB_MODE,
	ACCESS_LEVEL_USER,
	CLI_EXEC_FUNC,
	(FUNCPTR)cliCmdRstpPortEnable
},
{
	"port {portid|all} edged-port {disable|enable}",

	"{port,0,set rstp port parameter},\
	{portid,1,port id},\
	{all,0,all port},\
	{edged-port,0,set port edge port or auto edge port},\
	{disable,0,set port auto edge port},\
	{enable, 0,set port edge port}",

	"{portid,CLI_UINT,1,4,1}",

	CLI_MODE_CONFIG_RSTP,
	CLI_NULL_SUB_MODE,
	ACCESS_LEVEL_USER,
	CLI_EXEC_FUNC,
	(FUNCPTR)cliCmdRstpPortEdge
},
{
	"port {portid|all} point-to-point {force-true|force-false|auto}",

	"{port,0,set rstp port parameter},\
	{portid,1,port id},\
	{all,0,all port},\
	{point-to-point,0,set port point-to-point parameter},\
	{force-true,0,set port point-to-point port},\
	{force-false,0,set port point-to-multipoint port},\
	{auto,0,set port auto-decided point-to-point port}",

	"{portid,CLI_UINT,1,4,1}",
	CLI_MODE_CONFIG_RSTP,
	CLI_NULL_SUB_MODE,
	ACCESS_LEVEL_USER,
	CLI_EXEC_FUNC,
	(FUNCPTR)cliCmdRstpPortP2plink
},
{
	"port {portid|all} priority value",

	"{port,0,set rstp port parameter},\
	{portid,1,port id},\
	{all,0,all port},\
	{priority,0,set port priority parameter},\
	{value,1,port priority value: step 16}",

	"{portid,CLI_UINT,1,4,1}, \
	{value,CLI_UINT,0,240,128}",

	CLI_MODE_CONFIG_RSTP,
	CLI_NULL_SUB_MODE,
	ACCESS_LEVEL_USER,
	CLI_EXEC_FUNC,
	(FUNCPTR)cliCmdRstpPortPriority
},
{
	"port {portid|all} pathcost value",

	"{port,0,set rstp port parameter},\
	{portid,1,port id},\
	{all,0,all port},\
	{pathcost,0,set port path cost},\
	{value,1,port path cost value}",

	"{portid,CLI_UINT,1,4,1}, \
	{value,CLI_UINT,1,20000000,20000}",

	CLI_MODE_CONFIG_RSTP,
	CLI_NULL_SUB_MODE,
	ACCESS_LEVEL_USER,
	CLI_EXEC_FUNC,
	(FUNCPTR)cliCmdRstpPortPathcost
},
#endif

{
	"undo {hello|forward-delay|max-age|diameter|priority}",

	"{undo,0,Set rstp bridge parameter default value},\
	{hello,0,Set hellotime default value},\
	{forward-delay,0,Set forward delay time default value},\
	{max-age,0,Set max age default value},\
	{diameter,0,Set diameter default value},\
	{priority,0,Set bridge priority default value}",

	"",

	CLI_MODE_CONFIG_RSTP,
	CLI_NULL_SUB_MODE,
	ACCESS_LEVEL_USER,
	CLI_EXEC_FUNC,
	(FUNCPTR)cliCmdRstpRecoverBrdSet
},
{
	"undo port {{list portlist}|all} {pathcost|priority}",

	"{undo,0,Set rstp port parameter default value},\
	{port,0,Set rstp port parameter default value},\
	{list,0,Port list},\
	{portlist,1,"PORTLIST_HELP"},\
	{all,0,All port},\
	{pathcost,0,Set port path cost default value},\
	{priority,0,Set port priority default value}",

	"{portlist,CLI_WORD,NULL,NULL,256}",

	CLI_MODE_CONFIG_RSTP,
	CLI_NULL_SUB_MODE,
	ACCESS_LEVEL_USER,
	CLI_EXEC_FUNC,
	(FUNCPTR)cliCmdRstpRecoverPortSet
},

/* begin added by jiangmingli for zte */
{
    "rstp handle-mode {trap-to-cpu|transparent}",

    "{rstp,0,Rstp},\
    {handle-mode,0,Set rstp handle mode},\
    {trap-to-cpu,0,Set rstp handle mode trap-to-cpu},\
    {transparent,0,Set rstp handle mode transparent}",

    "",

    CLI_MODE_CONFIG_RSTP,
    CLI_NULL_SUB_MODE,
	ACCESS_LEVEL_USER,
    CLI_EXEC_FUNC,
    (FUNCPTR)cliCmdRstpHandleModeSet
},

{
    "show rstp handle-mode",

    "{show,0,Show},\
    {rstp,0, Show rstp handle mode},\
    {handle-mode, 0, show rstp handle mode}",

    "",

    CLI_MODE_COMMON,
    CLI_NULL_SUB_MODE,
	ACCESS_LEVEL_USER,
    CLI_EXEC_FUNC,
    (FUNCPTR)cliCmdRstpHandleModeShow
},
#endif

{
    "pon",

    "{pon,0,Enter PON configuration mode}",

    "",

    CLI_MODE_ENABLE | CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPon
},

{
    "pon loopback {uni_llid|scb_llid} {enable|disable}",

    "{pon,0,Enter pon mode},\
     {loopback,0,Pon port loopback},\
     {uni_llid,0,Uni llid loopback},\
     {scb_llid,0,Scb llid loopback},\
     {enable,0,Enable uni llid or scb llid loopback},\
     {disable,0,Disable uni llid or scb llid loopback}",

	"",

    CLI_MODE_CONFIG_PON,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPonLoopbackConfig

},

{
    "fec {enable|disable}",

    "{fec,0,Enable or disble the fec},\
     {enable,0,Enable fec},\
	{disable,0,Disable fec}",

	"",

    CLI_MODE_CONFIG_PON,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPonFecEnable

},

{
    "laser {auto|on|off}",

    "{laser,0,Laser control},\
    {auto,0,Laser on/off affected by grant and upstream data},\
    {on,0,Laser on},\
    {off,0,Set laser off}",

    "",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPonLaserCtrlSet
},

{
    "set laseron active {low|high}",

    "{set,0,Set a config},\
    {laseron,0,Laseron},\
    {active,0,Laser on actvie},\
    {low,0,Low actvie},\
    {high,0,High actvie}",

    "",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPonTxPolarityConfig
},

{
    "set laseron delay value",

    "{set,0,Set a config},\
    {laseron,0,Laseron},\
    {delay,0,Laser on delay time(tq)},\
    {value,1,Laser on delay value(tq)}",

   "{delayvalue,CLI_UINT,0,255,4}",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPonLaserOnSet
},

{
    "set laseroff delay value",

    "{set,0,Set a config},\
    {laseroff,0,Laseoff},\
    {delay,0,Laser off delay time(tq)},\
    {value,1,Laser off delay value(tq)}",

   "{delayvalue,CLI_UINT,0,255,4}",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPonLaserOffSet
},

{
    "set serdeslay delayvalue",

    "{set,0,Set a config},\
    {serdeslay,0,Laseoff},\
    {delayvalue,1,Serdesdelay delay value(tq)}",

   "{delayvalue,CLI_UINT,0,255,8}",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPonSerdesDelaySet
},

{
    "set macid macaddress",

    "{set,0,Set a config},\
    {macid,0,Macaddress;<xx:xx:xx:xx:xx:xx>},\
    {macaddress,1,Pon mac address}",

   "{macaddress,CLI_WORD,NULL,NULL,18}",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPonMacIdSet
},

{
    "set dbaagent rptsendmode {last|first} \
    txnormalwhenfec txnormaldatawhenfecvalue \
    txnormalnofec txnormaldatanofecvalue \
	txmpcpwhenfec txmpcpwhenfecvalue \
	txmpcpnofec txmpcpnofecvalue",

    "{set,0,Set a config},\
    {dbaagent,0,Set dbaagent policy},\
	{rptsendmode,0,When report send,at beginning of grant or end of grant;<first|last>},\
	{last,0,Report Send at the end of grant},\
	{first,0,Report Send at beginning of grant},\
	{txnormalwhenfec,0,Config tx normal data when fec enable},\
	{txnormaldatawhenfecvalue,1,Delta of tx normal data when fec enable},\
	{txnormalnofec,0,Config  tx normal data when fec disable},\
	{txnormaldatanofecvalue,1,Delta of tx normal data when fec disable},\
	{txmpcpwhenfec,0,Config tx mpcp when fec enable},\
	{txmpcpwhenfecvalue,1,Delta of tx mpcp when fec enable},\
	{txmpcpnofec,0,Config tx mpcp when fec is disable},\
	{txmpcpnofecvalue,1,Delta of tx mpcp wehn fec is disable}",

   "{txnormaldatawhenfecvalue,CLI_UINT,0,127,73},\
   	{txnormaldatanofecvalue,CLI_UINT,0,127,52},\
   	{txmpcpwhenfecvalue,CLI_UINT,0,127,58},\
   	{txmpcpnofecvalue,CLI_UINT,0,127,42}",


    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPonDbaAgentAllParaSet
},

{
    "set delay rttcommen {disable|enable} \
	rttcommsyncen {disable|enable} \
	rttcorrecten {disable|enable} \
	rttcorrectsyncen {disable|enable} \
	txdelayen {disable|enable} \
	syncen {disable|enable} \
	rptsenddelta reportsenddeltavalue  \
	rttoffset rttoffsetvalue \
	txdelaydelta txdelaydeltavalue",

    "{set,0,Set a config},\
    {delay,0,Set delay config},\
    {rttcommen,0,Enable of disable rtt compensate;<enable|disable>},\
    {disable,0,Disable rtt compenstate},\
    {enable,0,Enable rtt compensate},\
    {rttcommsyncen,0,Enable or disable rtt compenstate include synctime},\
    {disable,0,Disable rtt commpenstate include synctime},\
    {enable,0,Enable rtt compenstate include synctime},\
    {rttcorrecten,0,Enable or disable rtt correct},\
    {disable,0,Disable rtt correct},\
    {enable,0,Enable rtt correct},\
    {rttcorrectsyncen,0,Enable or disable rtt correct includ synctime},\
    {disable,0,Disable rtt correct include synctime},\
    {enable,0,Enable rtt correct include synctime},\
    {txdelayen,0,Enable or disable tx delay},\
    {disable,0,Disable tx delay},\
    {enable,0,Enable tx delay},\
    {syncen,0,Include synctime or not},\
    {disable,0,No include synctime},\
    {enable,0,Include synctime},\
    {rptsenddelta,0,Config report send delta},\
    {reportsenddeltavalue,1,Report send delta value},\
    {rttoffset,0,Config rtt commpenstate offset},\
    {rttoffsetvalue,1,The rtt commpenstatu offset value},\
    {txdelaydelta,0,Specify tx delay delta},\
    {txdelaydeltavalue,1,Txdelaydeltavalue}",

	"{reportsenddeltavalue,CLI_UINT,0,63,73},  \
   	{rttoffsetvalue,CLI_UINT,0,1023,0},  \
   	{txdelaydeltavalue,CLI_UINT,0,1023,0}",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPonTxDelaySet
},

{
   "set report {65535|baseThreshold}",

   "{set,0,Set a config},\
    {report,0,Set last queue set report mode,base threshold or 65535},\
    {65535,0,Report less or equal 65535}, \
    {baseThreshold,0,Report base threshold of last qset}",

   "",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPonReportModeSet
},

{
   "set qset qsetnum queue queuenum threshold thresholdval",

   "{set,0,Set a config},\
    {qset,0,Qset num},\
    {qsetnum,1,Specify qset num},\
    {queue,0,The queue in the qset},\
    {queuenum,1,Queue num in qset},\
    {threshold,0,Specified threshold value},\
    {thresholdval,1,Specified threshold value}",

   "{qsetnum,CLI_UINT,0,1,0},\
    {queuenum,CLI_UINT,0,7,0},\
    {thresholdval,CLI_UINT,0,65535,65535}",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPonQsetThresholdSet
},


{
    "set reg regid value regvalue",

    "{set,0,Set a config},\
    {reg,0,Specify register id;<0x000000>},\
    {regid,1,Register id;<0xaaaa>},\
    {value,0,Config value;<0x0000000>},\
    {regvalue,1,Register value to be config}",

   "{regid,CLI_WORD,NULL,NULL,32},\
   {regvalue,CLI_WORD,NULL,NULL,32}",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPonRegSet
},

{
    "get reg regid",

    "{get,0,Get a config},\
    {reg,0,Register id;<0xaaaa>},\
    {regid,1,Regid}",

   "{regid,CLI_WORD,NULL,NULL,32}",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPonRegGet
},

{
   "mget reg regId numOfRegs num",

    "{mget,0,Get multiple config},\
    {reg,0,Start regId;<0xaaaaa>},\
    {regid,1,Regid;<0xaaaaa>},\
    {numOfRegs,0,Numofreg},\
    {num,1,Numofreg}",

   "{regId,CLI_WORD,NULL,NULL,32},\
   {num,CLI_UINT,1,100,1}",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPonMutiRegGet
},

{
   "mget tab tabId startRecId startRecIdNum numOfRecs num",

    "{mget,0,Get multiple config},\
    {tab,0,TabId;(<0-14>},\
    {tabId,1,StartRecId},\
    {startRecId,0,Start record num},\
    {startRecIdNum,1,Startrecidvalue},\
    {numOfRecs,0,Num of records},\
    {num,1,Num of records to be show}",

   "{tabId,CLI_UINT,0,14,0},\
    {startRecIdNum,CLI_UINT,0,128,0},\
   {num,CLI_UINT,1,128,1}",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPonMutiTabGet
},

{
    "set tab tabid record recordid recordval",

    "{set,0,Set a config},\
    {tab,0,Set tab valuee},\
    {tabid,1,Specified tab id},\
    {record,0,Specify recordid},\
    {recordid,1,Specified table recordid},\
    {value,0,Specify record value;<(MSB)0x0000000,0x0000000,0x000000>(LSB)}, \
    {recordval,1,Tab record value}",

   "{tabid,CLI_UINT,0,14,0},\
   {recordid,CLI_UINT,0,128,0},\
   {recordval,CLI_WORD,1,128,512}",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPonTabRecSet
},

{
    "show tabinfo",

    "{show, 0,Show running system information},\
    {tabinfo,0,Show all tab attribute}",

   "",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPonShowTabInfo
},

#ifdef OPL_COM_UART1
{
    "uart1com {disable|enable}",

    "{uart1com, 0,Uart1 communication},\
    {disable,0,Disable Uart1 communication},\
    {enable,0,Enable Uart1 communication}",

   "",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdSystemUart1Com
},
#endif

{
   "show pon {parameter|dbacfg|delaycfg}",

    "{show, 0,Show running system information},\
    {pon,0,Show Pon information},\
    {parameter,0,Show pon mac config},\
    {dbacfg,0,Show dba config},\
    {delaycfg,0,Delay config}",

   "",

    CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_CONFIG_PON | CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPonShowConfig
},

#if 0  /* Commented out unused feature */
{
    "storm",

    "{storm,0,Enter storm configuration mode}",

    "",

    CLI_MODE_ENABLE | CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdStorm

},

{
    "enable broadcast  percent",

    "{enable,0,Enable storm limit},\
    {broadcast,0,type},\
    {percent,1,percent(%<0-100>)}",

    "{percent,CLI_UINT,0,100,10}",

    CLI_MODE_CONFIG_STORM,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdStormEnableBroadcastLimit
},

{
    "disable broadcast",

    "{disable,0,disable storm limit},\
    {broadcast,0,type}",

    "",

    CLI_MODE_CONFIG_STORM,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdStormDisableBroadcastLimit
},

{
    "enable multicast percent",

    "{enable,0,disable storm limit}\
    {multicast,0,type},\
    {percent,1,percent<%0-100>}",

    "{percent,CLI_UINT,0,100,10}",

    CLI_MODE_CONFIG_STORM,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdStormEnableMulticastLimit
},

{
    "disable multicast",

    "{disable,0,disable storm limit}\
    {multicast,0,type}",

    "",

    CLI_MODE_CONFIG_STORM,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdStormDisableMulticastLimit
},

{
    "enable dlf percent",

    "{enable,0,disable storm limit}\
    {dlf,0,type},\
    {percent,1,percent(%<0-100>)}",


    "{percent,CLI_UINT,0,100,10}",

    CLI_MODE_CONFIG_STORM,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdStormEnableDlfLimit
},

{
    "disable dlf",

    "{disable,0,disable storm limit}\
    {dlf,0,type}",

    "",

    CLI_MODE_CONFIG_STORM,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdStormDisableDlfLimit
},

{
    "show port portlist storm",

    "{show,0,show port storm config},\
    {port,0,portlist},\
    {portlist,1,portlist},\
    {storm,0,storm module}",

   "{portlist, CLI_WORD, NULL, NULL, 256}",

    CLI_MODE_USER |  CLI_MODE_SUPER | CLI_MODE_ENABLE | CLI_MODE_CONFIG_STORM | CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdStromShowLimitConfig
},
#endif /* #if 0 */
#if defined(ONU_4PORT_AR8228) || defined(ONU_1PORT)||defined(ONU_4PORT_AR8327)
{
    "storm",

    "{storm,0,Enter storm configuration mode}",

    "",

    CLI_MODE_ENABLE | CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdStorm

},
{
	"show storm",

	"{show,0,show storm config},\
	{storm,0,storm module}",

   "",

	CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_CONFIG_STORM | CLI_MODE_SUPER,

	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

	CLI_EXEC_FUNC,

	(FUNCPTR)cliCmdStormShow
},


{
#if defined(ONU_1PORT)
    "set {upstream |downstream} type broadcast {enable|disable}",

	"{set,0,set storm type and status of the port},\
	{upstream,0, Set upstream storm},\
	{downstream,0,Set downstream storm},\
	{type,0,Set storm type},\
	{broadcast,0, The broadcast storm},\
	{enable, 0, Enable the storm},\
	{disable, 0, Disable the storm}",

    "",

#else
	"set {upstream port portlist |downstream} type {unicast|multicast|broadcast}{enable|disable}",

	"{set,0,set storm type and status of the port},\
	{upstream,0, Set upstream storm},\
	{port,0,Set the port of the storm},\
	{portlist, 1, "PORTLIST_HELP"},\
	{downstream,0,Set downstream storm},\
	{type,0,Set storm type},\
	{unicast,0, The unicast storm},\
	{multicast,0, The multicast storm},\
	{broadcast,0, The broadcast storm},\
	{enable, 0, Enable the storm},\
	{disable, 0, Disable the storm}",

	"{portlist, CLI_WORD, NULL, NULL, 256}",

#endif

	CLI_MODE_CONFIG_STORM,

	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

	CLI_EXEC_FUNC,

	(FUNCPTR)cliCmdStormCtrlFrameSet
}

,

{

#if defined(ONU_1PORT)
    "ratelimit {upstream |downstream} rate ratelist",

	"{ratelimit,0,Set storm ratelimit},\
	{upstream,0, Set upstream storm},\
	{downstream,0,Set downstream storm},\
	{rate,0,set rate limit},\
	{ratelist, 1, Enter rate size 32,64...32*N...1000000kbps}",

	 "{ratelist, CLI_UINT, 32, 1000000, 32}",
#elif defined(ONU_4PORT_AR8327)
       "ratelimit {upstream port portlist |downstream} rate ratelist",

	"{ratelimit,0,Set storm ratelimit},\
	{upstream,0, Set upstream storm},\
	{port,0,set storm port},\
	{portlist, 1, "PORTLIST_HELP"},\
	{downstream,0,Set downstream storm},\
	{rate,0,set rate limit},\
	{ratelist, 1, Enter rate size 32,64...32*N...10000000kbps}",

        "{portlist, CLI_WORD, NULL, NULL, 256},\
	 {ratelist, CLI_UINT, 32, 10000000, 32}",
#else
	"ratelimit {upstream port portlist |downstream} {1KPPS|2KPPS|4KPPS|8KPPS|16KPPS|32KPPS|64KPPS|128KPPS|256KPPS|512KPPS|1024KPPS}",

	"{ratelimit,0,Set storm ratelimit},\
	{upstream,0, Set upstream storm},\
	{port,0,set storm port},\
	{portlist, 1, "PORTLIST_HELP"},\
	{downstream,0,Set downstream storm},\
	{1KPPS,0,1KPPS rate limit},\
	{2KPPS,0,2KPPS rate limit},\
	{4KPPS,0,4KPPS rate limit},\
	{8KPPS,0,8KPPS rate limit},\
	{16KPPS,0,16KPPS rate limit},\
	{32KPPS,0,32KPPS rate limit},\
	{64KPPS,0,64KPPS rate limit},\
	{128KPPS,0,128KPPS rate limit},\
	{256KPPS,0,256KPPS rate limit},\
	{512KPPS,0,512KPPS rate limit},\
	{1024KPPS,0,1024KPPS rate limit}",

	"{portlist, CLI_WORD, NULL, NULL, 256}",
#endif

	CLI_MODE_CONFIG_STORM,

	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

	CLI_EXEC_FUNC,

	(FUNCPTR)cliCmdStormCtrlRateSet
},

#endif

/* begin added by jiangmingli, 2008-08-25 */
{
	"aging-time time",

#if defined(ONU_1PORT)
	"{aging-time, 0, Set fdb entry aging time},\
	{time, 1, Aging time(<0-1800> seconds, <1> not support,<0> means aging disabled)}",

   "{time, CLI_INTEGER, 0, 1800, 1200}",
#elif defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	"{aging-time, 0, Set fdb entry aging time},\
	 {time, 1, Aging time (<0-458745> seconds, <0-6> means aging disabled)}",

	"{time, CLI_INTEGER, 0, 458745, 1200}",
#else
   "{aging-time, 0, Set fdb entry aging time},\
	{time, 1, Aging time (<0-3825> seconds, <0> means aging disabled)}",

   "{time, CLI_INTEGER, 0, 3825, 1200}",
#endif

	CLI_MODE_CONFIG_FDB,

	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

	CLI_EXEC_FUNC,

	(FUNCPTR)cliCmdFdbSetAgingTime

},

{
#if defined(ONU_1PORT) || defined(ONU_4PORT_AR8327)
	"create fdbentry macaddr portlist vlanname",

	"{create, 0, Create fdb entry},\
	{fdbentry, 0, Create fdb entry},\
	{macaddr, 1, Mac address of fdb entry},\
	{portlist, 1, Portlist of fdb entry},\
	{vlanname, 1, Vlanid of fdb entry}",

   "{macaddr, CLI_MACADDR, NULL, NULL, 17},\
   {portlist, CLI_WORD, NULL, NULL, 256},\
   {vlanname, CLI_INTEGER, 0, 4094, 1}",
#else
	"create fdbentry macaddr portlist",

	"{create, 0, Create fdb entry},\
	{fdbentry, 0, Create fdb entry},\
	{macaddr, 1, Mac address of fdb entry},\
	{portlist, 1, Portlist of fdb entry}",

   "{macaddr, CLI_MACADDR, NULL, NULL, 17},\
   {portlist, CLI_WORD, NULL, NULL, 256}",
#endif

	CLI_MODE_CONFIG_FDB,

	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

	CLI_EXEC_FUNC,

	(FUNCPTR)cliCmdFdbAddFdbEntry

},

{
#if defined(ONU_1PORT)||defined(ONU_4PORT_AR8327)
	"delete fdbentry {{mac macaddr vlanname}|all}",

	"{delete, 0, Delete fdb entry},\
	{fdbentry, 0, Delete fdb entry},\
	{mac, 0, Mac address},\
	{macaddr, 1, Mac address of fdb entry},\
	{vlanname, 1, Vlanid of fdb entry},\
	{all, 0, All mac address}",

   "{macaddr, CLI_MACADDR, NULL, NULL, 17},\
   {vlanname, CLI_INTEGER, 1, 4094, 1}",
#else
   "delete fdbentry {{mac macaddr} |all}",

	"{delete, 0, Delete fdb entry},\
	{fdbentry, 0, Delete fdb entry},\
	{mac, 0, Mac address},\
	{macaddr, 1, Mac address of fdb entry},\
	{all, 0, All mac address}",

   "{macaddr, CLI_MACADDR, NULL, NULL, 17}",
#endif

	CLI_MODE_CONFIG_FDB,

	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

	CLI_EXEC_FUNC,

	(FUNCPTR)cliCmdFdbDelFdbEntry

},
#if 0
{
	"discard fdbentry macaddr [clear]",

	"{discard, 0, Discard packets of the fdb entry},\
	{fdbentry, 0, Discard packets of the fdb entry},\
	{macaddr, 1, Mac address of fdb entry},\
	{clear, 0, Clear the discard setting}",

   "{macaddr, CLI_MACADDR, NULL, NULL, 17}",

	CLI_MODE_CONFIG_FDB,

	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

	CLI_EXEC_FUNC,

	(FUNCPTR)cliCmdFdbDiscardFdbEntry

},
#endif
{
#if defined(ONU_4PORT_AR8327)
    "mac filter add macaddr vid",

    "{mac,     0, Add mac filter for one port},\
     {filter,  0, Add mac filter for one port},\
     {add,     0, Add mac filter for one port},\
	 {macaddr, 1, Mac address},\
	 {vid, 1, Vlan tag}",

	"{macaddr, CLI_MACADDR, NULL, NULL, 17},\
	{vid, CLI_INTEGER, 0, 4094, 1}",
#else
    "mac filter add macaddr",

    "{mac,     0, Add mac filter for one port},\
     {filter,  0, Add mac filter for one port},\
     {add,     0, Add mac filter for one port},\
	 {macaddr, 1, Mac address}",

	"{macaddr, CLI_MACADDR, NULL, NULL, 17}",
#endif
    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortMacFilterAdd
},

{
#if defined(ONU_4PORT_AR8327)
    "mac filter del macaddr vid",

    "{mac,     0, Delete mac filter for one port},\
     {filter,  0, Delete mac filter for one port},\
     {del,     0, Delete mac filter for one port},\
	 {macaddr, 1, Mac address},\
	 {vid, 1, Vlan tag}",

	"{macaddr, CLI_MACADDR, NULL, NULL, 17},\
	{vid, CLI_INTEGER, 0, 4094, 1}",
#else
    "mac filter del macaddr",

    "{mac,     0, Delete mac filter for one port},\
     {filter,  0, Delete mac filter for one port},\
     {del,     0, Delete mac filter for one port},\
	 {macaddr, 1, Mac address}",

	"{macaddr, CLI_MACADDR, NULL, NULL, 17},\
	{vid, CLI_INTEGER, 0, 4094, 1}",
#endif

    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortMacFilterDel

},

{
    "mac filter clear",

    "{mac,     0, Clear mac filter for one port},\
     {filter,  0, Clear mac filter for one port},\
	 {clear,   0, Clear mac filter for one port}",

	"",

    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortMacFilterClear

},

{
    "show mac filter",

    "{show,   0, Show mac filter},\
     {mac,    0, Show mac filter},\
     {filter, 0, Show mac filter}",

	"",

    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortMacFilterShow
},

/* for port based static mac config */
{
	/* modified by tanglin 2010-05-19
	    add  [vlan vid] and the Macro
	*/
#if defined(ONU_1PORT) || defined(ONU_4PORT_AR8327)
	 "mac static add macaddr [vlan vid]",

    "{mac,     0, Add static mac for one port},\
     {static,  0, Add static mac for one port},\
     {add,     0, Add static mac for one port},\
	{macaddr, 1, Mac address},\
	{vlan, 0, vlan},\
	 {vid, 1, Vlan tag}",

	"{macaddr, CLI_MACADDR, NULL, NULL, 17},\
	{vid, CLI_INTEGER, 0, 4094, 1}",
#else
     "mac static add macaddr",

    "{mac,     0, Add static mac for one port},\
     {static,  0, Add static mac for one port},\
     {add,     0, Add static mac for one port},\
	{macaddr, 1, Mac address}",

	"{macaddr, CLI_MACADDR, NULL, NULL, 17}",
#endif

    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortStaticMacAdd
},

{
#if defined(ONU_1PORT) || defined(ONU_4PORT_AR8327)
	 "mac static del macaddr [vid] ",

    "{mac,     0, Delete static mac for one port},\
     {static,  0, Delete static mac for one port},\
     {del,     0, Delete static mac for one port},\
	 {macaddr, 1, Mac address},\
	 {vid, 1, Vlan tag}",

	"{macaddr, CLI_MACADDR, NULL, NULL, 17},\
	{vid, CLI_INTEGER, 0, 4094, 1}",
#else
    "mac static del macaddr",

    "{mac,     0, Delete static mac for one port},\
     {static,  0, Delete static mac for one port},\
     {del,     0, Delete static mac for one port},\
	 {macaddr, 1, Mac address}",

	"{macaddr, CLI_MACADDR, NULL, NULL, 17}",
#endif
    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortStaticMacDel

},

{
    "mac static clear",

    "{mac,     0, Clear static mac for one port},\
     {static,  0, Clear static mac for one port},\
	 {clear,   0, Clear static mac for one port}",

	"",

    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortStaticMacClear

},

{
    "show mac static",

    "{show,   0, Show static mac},\
     {mac,    0, Show static mac},\
     {static, 0, Show static mac}",

	"",

    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortStaticMacShow
},

/* for port based bind mac config */
{
#if defined(ONU_1PORT) || defined(ONU_4PORT_AR8327)
    "mac bind add macaddr vid",

    "{mac,     0, Add bind mac for one port},\
     {bind,    0, Add bind mac for one port},\
     {add,     0, Add bind mac for one port},\
	 {macaddr, 1, Mac address},\
	 {vid, 1, Vlan tag}",

	"{macaddr, CLI_MACADDR, NULL, NULL, 17},\
	{vid, CLI_INTEGER, 0, 4094, 1}",
#else
    "mac bind add macaddr",

    "{mac,     0, Add bind mac for one port},\
     {bind,    0, Add bind mac for one port},\
     {add,     0, Add bind mac for one port},\
	 {macaddr, 1, Mac address}",

	"{macaddr, CLI_MACADDR, NULL, NULL, 17}",
#endif

    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortBindMacAdd
},

{
#if defined(ONU_1PORT) || defined(ONU_4PORT_AR8327)
    "mac bind del macaddr vid",

    "{mac,     0, Delete bind mac for one port},\
     {bind,    0, Delete bind mac for one port},\
     {del,     0, Delete bind mac for one port},\
	 {macaddr, 1, Mac address},\
	 {vid, 1, Vlan tag}",

	"{macaddr, CLI_MACADDR, NULL, NULL, 17},\
	{vid, CLI_INTEGER, 0, 4094, 1}",
#else
    "mac bind del macaddr",

    "{mac,     0, Delete bind mac for one port},\
     {bind,    0, Delete bind mac for one port},\
     {del,     0, Delete bind mac for one port},\
	 {macaddr, 1, Mac address}",

	"{macaddr, CLI_MACADDR, NULL, NULL, 17}",
#endif

    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortBindMacDel

},

{
    "mac bind clear",

    "{mac,     0, Clear bind mac for one port},\
     {bind,    0, Clear bind mac for one port},\
	 {clear,   0, Clear static mac for one port}",

	"",

    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortBindMacClear

},

{
    "show mac bind",

    "{show,   0, Show bind mac},\
     {mac,    0, Show bind mac},\
     {bind, 0, Show bind mac}",

	"",

    CLI_MODE_CONFIG_PORT,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPortBindMacShow
},

#if defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
{
	"move fdbentry portlist portlist",

	 "{move, 0, Move fdb entry},\
	 {fdbentry, 0, Move fdb entry},\
	 {portlist, 1, "PORTLIST_HELP"},\
	 {portlist, 1, "PORTLIST_HELP"},",

	"{portlist,CLI_UINT,1,4,1},\
	{portlist,CLI_UINT,1,4,1}",

	CLI_MODE_CONFIG_FDB,

	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

	CLI_EXEC_FUNC,

	(FUNCPTR)cliCmdFdbMoveFdbEntry
},
#endif

/* begin added by jiangmingli for software learning function */
#ifdef ONU_1PORT
{
    "soft-learning {disable|enable}",

	"{soft-learning, 0, Software learning function},\
	{disable, 0, Disable software learning function},\
	{enable, 0, Enable software learning function}",

   "",

	CLI_MODE_CONFIG_FDB,

	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

	CLI_EXEC_FUNC,

	(FUNCPTR)cliCmdFdbSetSwLearn
},

{
    "show fdb soft-learning",

    "{show, 0, Show fdb software learning},\
	{fdb, 0, Show fdb information},\
	{soft-learning, 0, Software learning function}",

   "",

    CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_CONFIG_FDB | CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdFdbShowSwLearn
},

/* end added by jiangmingli for software learning function */
#endif

{
	"show fdb aging-time",

	"{show, 0, Show fdb aging time},\
	{fdb, 0, Show fdb information},\
	{aging-time, 0, Aging time}",

    "",

	CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_CONFIG_FDB | CLI_MODE_SUPER,

	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

	CLI_EXEC_FUNC,

	(FUNCPTR)cliCmdShowAgingTime

},

{
	"show fdb all",

	"{show, 0, Show all fdb entry information},\
	{fdb, 0, Show fdb information},\
	{all, 0, All fdb entry}",

   "",

	CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_CONFIG_FDB | CLI_MODE_SUPER,

	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

	CLI_EXEC_FUNC,

	(FUNCPTR)cliCmdShowFdbAll

},

{
	"show fdb static [mac macaddr]",

	"{show, 0, Show static fdb entry information},\
	{fdb, 0, Show fdb information},\
	{static, 0, Static fdb entry},\
	{mac, 0, Mac address},\
	{macaddr, 1, Mac address of fdb entry}",

   "{macaddr, CLI_MACADDR, NULL, NULL, 17}",

	CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_CONFIG_FDB | CLI_MODE_SUPER,

	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

	CLI_EXEC_FUNC,

	(FUNCPTR)cliCmdShowFdbStatic

},

{
	"show fdb discard [mac macaddr]",

	"{show, 0, Show static fdb entry information},\
	{fdb, 0, Show fdb information},\
	{discard, 0, Discard fdb entry},\
	{mac, 0, Mac address},\
	{macaddr, 1, Mac address of fdb entry}",

   "{macaddr, CLI_MACADDR, NULL, NULL, 17}",

	CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_CONFIG_FDB | CLI_MODE_SUPER,

	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

	CLI_EXEC_FUNC,

	(FUNCPTR)cliCmdShowFdbDiscard

},

{
	"show fdb dynamic [port portlist] [mac macaddr]",

	"{show, 0, Show dynamic fdb entry information},\
	{fdb, 0, Show fdb information},\
	{dynamic, 0, Dynamic fdb entry},\
	{port, 0, Port},\
	{portlist, 1, Portlist},\
	{mac, 0, Mac address},\
	{macaddr, 1, Mac address of fdb entry}",

   "{portlist, CLI_WORD, NULL, NULL, 256},\
   {macaddr, CLI_MACADDR, NULL, NULL, 17}",

	CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_CONFIG_FDB | CLI_MODE_SUPER,

	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

	CLI_EXEC_FUNC,

	(FUNCPTR)cliCmdShowFdbDynamic

},

{
	"show fdb cfg",

	"{show, 0, Show fdb configure information},\
	{fdb, 0, Show fdb information},\
	{cfg, 0, Configure information}",

   "",

	CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_CONFIG_FDB | CLI_MODE_SUPER,

	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

	CLI_EXEC_FUNC,

	(FUNCPTR)cliCmdShowFdbCfg

},

/* end added by jiangmingli, 2008-08-25 */

{
	"show counter ge",
	"{show,0,Show running system information}, {counter, 0, Show Counter information}, {ge, 0, GE counter}",
	"",
	CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_SUPER,

	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

	CLI_EXEC_FUNC,

	(FUNCPTR)CliShowCntGe
},

{
	"show counter pon",
	"{show,0,Show running system information}, {counter, 0, Counter}, {pon, 0, PON counter}",
	"",
	CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_SUPER,

	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

	CLI_EXEC_FUNC,

	(FUNCPTR)CliShowCntPon
},

{
	"show counter fe",
	"{show,0,Show running system information}, {counter, 0, Counter}, {fe, 0, FE counter}",
	"",
	CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_SUPER,
	CLI_NULL_SUB_MODE,
	ACCESS_LEVEL_USER,
	CLI_EXEC_FUNC,
	(FUNCPTR)CliShowCntFe
},

{
	"show counter tm",
	"{show,0,Show running system information}, {counter, 0, Counter}, {tm, 0, Traffic management counter}",
	"",
	CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_SUPER,
	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,
	CLI_EXEC_FUNC,
	(FUNCPTR)CliShowCntTm
},

{
	"show counter cle",
	"{show,0,Show running system information}, {counter, 0, Counter}, {cle, 0, CLE counter}",
	"",
	CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_SUPER,
	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,
	CLI_EXEC_FUNC,
	(FUNCPTR)CliShowCntCle
},

{
	"show counter brg",
	"{show,0,Show running system information}, {counter, 0, Counter}, {brg, 0, Bridge counter}",
	"",
	CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_SUPER,
	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,
	CLI_EXEC_FUNC,
	(FUNCPTR)CliShowCntBrg
},

{
	"show counter mpcp",
	"{show,0,Show running system information}, {counter, 0, Counter}, {mpcp, 0, MPCP counter}",
	"",
	CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_SUPER,
	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,
	CLI_EXEC_FUNC,
	(FUNCPTR)CliShowCntMpcp
},

{
	"show counter marb",
	"{show,0,Show running system information}, {counter, 0, Counter}, {marb, 0, MARB counter}",
	"",
	CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_SUPER,
	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,
	CLI_EXEC_FUNC,
	(FUNCPTR)CliShowCntMarb
},

{
	"show counter cpdma",
	"{show,0,Show running system information}, {counter, 0, Counter}, {cpdma, 0, CPU DMA counter}",
	"",
	CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_SUPER,
	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,
	CLI_EXEC_FUNC,
	(FUNCPTR)CliShowCntCpdma
},

{
	"show counter fedma",
	"{show,0,Show running system information}, {counter, 0, Counter}, {fedma, 0, FE DMA counter}",
	"",
	CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_SUPER,
	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,
	CLI_EXEC_FUNC,
	(FUNCPTR)CliShowCntFedma
},

{
	"show counter geparser",
	"{show,0,Show running system information}, {counter, 0, Counter}, {geparser, 0, GE parser counter}",
	"",
	CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_SUPER,
	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,
	CLI_EXEC_FUNC,
	(FUNCPTR)CliShowCntGeParser
},

{
	"show counter clehit start num",
	"{show, 0, Show running system information}, {counter, 0, Counter}, {clehit, 0, CLE hit counter},\
	{start, 1, Start rule id}, {num, 1, Rule number}",
	"{start, CLI_UINT, 0, 127, 0},\
	{num, CLI_UINT, 1, 128, 0}",
	CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_SUPER,
	CLI_NULL_SUB_MODE,
	ACCESS_LEVEL_USER,
	CLI_EXEC_FUNC,
	(FUNCPTR)CliShowCntCleHit
},

{
	"show counter switch start num",
	"{show, 0, Show running system information}, {counter, 0, Counter}, {switch, 0, Switch port},\
	{start, 1, Start switch port id}, {num, 1, Switch port number}",
	"{start, CLI_UINT, 1, 4, 0},\
	{num, CLI_UINT, 1, 4, 0}",
	CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_SUPER,
	CLI_NULL_SUB_MODE,
	ACCESS_LEVEL_USER,
	CLI_EXEC_FUNC,
	(FUNCPTR)CliShowCntSwhPort
},

{
	"counter poll {disable|enable}",
	"{counter, 0, Counter configuration}, {poll, 0, Polling mode}, \
	{disable, 0, Disable counter}, {enable, 0, Enable counter}",
	"",
	CLI_MODE_SUPER,
	CLI_NULL_SUB_MODE,
	ACCESS_LEVEL_SUPER,
	CLI_EXEC_FUNC,
	(FUNCPTR)CliCntPollEndis
},

#if 0
{
    "clear port {{list portlist}|all} multicast-vlan",

    "{clear, 0, Clear ports in vlan},\
    {port, 0, Port}, \
    {list, 0, List}, \
    {portlist, 1, Portlist;(format:1-2,3,4-5)}, \
    {all, 0, All ports}, \
    {multicast-vlan, 0, Multicast vlan}",

    "{portlist, CLI_WORD, NULL, NULL, 256}",

    CLI_MODE_CONFIG_MCAST,
    CLI_NULL_SUB_MODE,
    ACCESS_LEVEL_USER,
    CLI_EXEC_TASK,

    (FUNCPTR)cliCmdMulticastMulticastVlanClear
},

#endif
{
	"counter clear {ge|pon|fe|tm|cle|brg|mpcp|marb|cpdma|fedma|geparser|clehit|{switch start num}|all}",
	"{counter, 0, Counter configuration}, {clear, 0, Clear counter}, {ge, 0, GE}, {pon, 0, PON}, {fe, 0, FE}, {tm, 0, TM},\
	{cle, 0, CLE}, {brg, 0, Bridge}, {mpcp, 0, MPCP}, {marb, 0, MARB}, {cpdma, 0, CPU DMA},\
	{fedma, 0, FE DMA}, {geparser, 0, GE parser}, {clehit, 0, CLE hit}, {switch, 0, Switch},\
	{start, 1, Start switch port id}, {num, 1, Switch port number}, {all, 0, All counters}",
	"{start, CLI_UINT, 1, 4, 0},\
	{num, CLI_UINT, 1, 4, 0}",
	CLI_MODE_ENABLE | CLI_MODE_SUPER,
	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,
	CLI_EXEC_FUNC,
	(FUNCPTR)CliCntClr
},

{
	"oam dbg {disable|enable}",
	"{oam, 0, OAM configuration}, {dbg, 0, Debug}, {disable, 0, Disable debug}, {enable, 0, Enable debug}",
	"",
	CLI_MODE_ENABLE | CLI_MODE_SUPER,
	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,
	CLI_EXEC_FUNC,
	(FUNCPTR)CliOamDbgEndis
},

{
	"oam alarm report",
	"{oam, 0, Test alarm report}, {alarm, 0, Test alarm report}, {report, 0, Test alarm report}",
	"",
	CLI_MODE_ENABLE | CLI_MODE_SUPER,
	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,
	CLI_EXEC_FUNC,
	(FUNCPTR)CliOamReportAlarm
},

{
	"oam alarm clear",
	"{oam, 0, Test alarm clear}, {alarm, 0, Test alarm clear}, {clear, 0, Test alarm clear}",
	"",
	CLI_MODE_ENABLE | CLI_MODE_SUPER,
	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,
	CLI_EXEC_FUNC,
	(FUNCPTR)CliOamClearAlarm
},

{
	"dyinggasp oam num",

    "{dyinggasp, 0, Dying gasp},\
     {oam, 0, oam},\
     {num,1,Input oam number for dying gasp}",

    "{num,CLI_UINT, 0, 127, 0}",


	CLI_MODE_ENABLE | CLI_MODE_SUPER,
	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,
	CLI_EXEC_FUNC,
	(FUNCPTR)CliOamDygaspnumConfig
},

{
    "fastleave {disable|enable}",

    "{fastleave, 0, Configure multicast fastleave mode},\
    {disable, 0, Enable Non-Fastleave mode},\
    {enable, 0, Enable Fastleave mode}",

    "",

    CLI_MODE_CONFIG_MCAST,
    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,
    CLI_EXEC_FUNC,
    (FUNCPTR)cliCmdMulticastFastleaveSet
},

{
    "mcdebug {disable|enable}",

    "{mcdebug, 0, Configure mcdebug mode},\
    {disable, 0, Disable multicast debug mode},\
    {enable, 0, Enable multicast debug mode}",

    "",

    CLI_MODE_CONFIG_MCAST,
    CLI_NULL_SUB_MODE,

    ACCESS_LEVEL_USER,
    CLI_EXEC_FUNC,
    (FUNCPTR)cliCmdMulticastDebugSet
},


{
    "host-timeout value",

    "{host-timeout, 0, Configure multicast host timeout value},\
    {value, 1, Timeout value (1s)}",

    "{value, CLI_UINT, 0, 1000, 260}",

    CLI_MODE_CONFIG_MCAST,
    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,
    CLI_EXEC_FUNC,
    (FUNCPTR)cliCmdMulticastHostTimeoutSet
},

{
	  "last-query-interval value",

    "{last-query-interval, 0, Configure multicast last query interval value},\
    {value, 1, Value (ms)}",

    "{value, CLI_UINT, 100, 27000, 1000}",

    CLI_MODE_CONFIG_MCAST,
    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,
    CLI_EXEC_FUNC,
    (FUNCPTR)cliCmdMulticastLastQueryIntervalSet
},

{
    "last-query-count value",

    "{last-query-count, 0, Configure multicast last query count value},\
    {value, 1, Query count}",

    "{value, CLI_UINT, 0, 10, 2}",

    CLI_MODE_CONFIG_MCAST,
    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,
    CLI_EXEC_FUNC,
    (FUNCPTR)cliCmdMulticastLastQueryCountSet
},

{
    "igmp {enable|disable}",

    "{igmp, 0, Configure IGMP mode},\
    {enable, 0, Enable IGMP}, \
    {disable, 0, Disable IGMP}",

    "",

    CLI_MODE_CONFIG_MCAST,
    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,
    CLI_EXEC_FUNC,
    (FUNCPTR)cliCmdMulticastIgmpConfig
},

{
    "xswitch {igmp-snooping|ctc}",

    "{xswitch, 0, Configure multicast mode},\
    {igmp-snooping, 0, Enable IGMP Snooping mode}, \
    {ctc, 0, Enable CTC mode}",

    "",

    CLI_MODE_CONFIG_MCAST,
    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,
    CLI_EXEC_FUNC,
    (FUNCPTR)cliCmdMulticastModeSet
},

{
    "controltype {gda-mac|gda-mac-vlan|gda-mac-sa-mac|gda-ip-vlan}",

    "{controltype, 0, Configure control type},\
    {gda-mac, 0, GDA£¨Group Destination Address£© MAC£¨DA MAC only£©}, \
    {gda-mac-vlan, 0, GDA MAC and multicast vlan ID}, \
    {gda-mac-sa-mac, 0, GDA MAC+SA MAC}, \
    {gda-ip-vlan, 0, GDA IP and multicast vlan ID}",

    "",

    CLI_MODE_CONFIG_MCAST,
    CLI_NULL_SUB_MODE,

    ACCESS_LEVEL_USER,
    CLI_EXEC_FUNC,
    (FUNCPTR)cliCmdMulticastControlTypeSet
},

{
    "add port {{list portlist}|all} multicast-vlan vlanid",

    "{add, 0, Add ports to vlan},\
    {port, 0, Port}, \
    {list, 0, List}, \
    {portlist, 1, "PORTLIST_HELP"}, \
    {all, 0, All ports}, \
    {multicast-vlan, 0, Multicast vlan}, \
    {vlanid, 1, Vlan ID}",

    "{portlist, CLI_WORD, NULL, NULL, 256} \
    {vlanid, CLI_UINT, 1, 4094, 1}",

    CLI_MODE_CONFIG_MCAST,
    CLI_NULL_SUB_MODE,

	  ACCESS_LEVEL_USER,
    CLI_EXEC_FUNC,
    (FUNCPTR)cliCmdMulticastMulticastVlanSet
},

{
    "delete port {{list portlist}|all} multicast-vlan vlanid",

    "{delete, 0, Delete ports in vlan},\
    {port, 0, Port}, \
    {list, 0, List}, \
    {portlist, 1, "PORTLIST_HELP"}, \
    {all, 0, All ports}, \
    {multicast-vlan, 0, Multicast vlan}, \
    {vlanid, 1, Vlan ID}",

    "{portlist, CLI_WORD, NULL, NULL, 256} \
    {vlanid, CLI_UINT, 1, 4094, 1}",

    CLI_MODE_CONFIG_MCAST,
    CLI_NULL_SUB_MODE,

	  ACCESS_LEVEL_USER,
    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdMulticastMulticastVlanDelete
},

{
    "clear port {{list portlist}|all} multicast-vlan",

    "{clear, 0, Clear ports in vlan},\
    {port, 0, Port}, \
    {list, 0, List}, \
    {portlist, 1, "PORTLIST_HELP"}, \
    {all, 0, All ports}, \
    {multicast-vlan, 0, Multicast vlan}",

    "{portlist, CLI_WORD, NULL, NULL, 256}",

    CLI_MODE_CONFIG_MCAST,
    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,
    CLI_EXEC_TASK,

    (FUNCPTR)cliCmdMulticastMulticastVlanClear
},
/*
{
    "show mport {{list portlist}|all} multicast",

    "{show, 0, Show multicast infomation belong to the ports},\
    {mport, 0, port}, \
    {list, 0, number}, \
    {portlist, 1, portlist;(format:1-2,3,4-5)}, \
    {all, 0, all ports}, \
    {multicast, 0, multicast}",

    "{portlist, CLI_WORD, NULL, NULL, 256}",

    CLI_MODE_USER |  CLI_MODE_SUPER | CLI_MODE_ENABLE | CLI_MODE_CONFIG_MCAST | CLI_MODE_SUPER,
    CLI_NULL_SUB_MODE,
    ACCESS_LEVEL_USER,
    CLI_EXEC_TASK,

    (FUNCPTR)cliCmdMulticastPortConfigShow
},
*/
{
    "show multicast",

    "{show, 0, Show running system information},\
    {multicast, 0, Show multicast information}",

    "",

    CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_CONFIG_MCAST | CLI_MODE_SUPER,
    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,
    CLI_EXEC_TASK,

    (FUNCPTR)cliCmdMulticastCfgShow
},
{
    "show multicast vlan {vlanid|all}",

    "{show, 0, Show running system information},\
    {multicast, 0, Show multicast information}, \
    {vlan, 0, Multicast vlan}, \
    {vlanid, 1, Vlan ID}, \
    {all, 0, All vlans}",

    "{vlanid, CLI_UINT, 1, 4094, 1}",

    CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_CONFIG_MCAST | CLI_MODE_SUPER,
    CLI_NULL_SUB_MODE,

	  ACCESS_LEVEL_USER,
    CLI_EXEC_FUNC,
    (FUNCPTR)cliCmdMulticastVlan
},

{
    "show multicast stats",

    "{show, 0, Show running system information}, \
    {multicast, 0, Show multicast information}, \
    {stats, 0, Show multicast stats}",

    "",

    CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_CONFIG_MCAST | CLI_MODE_SUPER,
    CLI_NULL_SUB_MODE,
    ACCESS_LEVEL_GUEST,
    CLI_EXEC_TASK,

    (FUNCPTR)cliCmdMulticastStats
},

{
    "show multicast group-limit",

    "{show, 0, Show running system information}, \
    {multicast, 0, Show multicast information}, \
    {group-limit, 0, Show multicast group limit information}",

    "",

    CLI_MODE_USER | CLI_MODE_ENABLE | CLI_MODE_CONFIG_MCAST | CLI_MODE_SUPER,
    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,
    CLI_EXEC_TASK,

    (FUNCPTR)cliCmdMulticastGroupLimitAll
},


{
    "multicast-tag port {{list portlist}|all} {transparent|strip|translation}",

    "{multicast-tag, 0, Configure multicast tag strip mode}, \
    {port, 0, Port},\
    {list, 0, List}, \
    {portlist, 1, "PORTLIST_HELP"}, \
    {all, 0, All ports}, \
    {transparent, 0, Enable transparent mode}, \
    {strip, 0, Enable strip mode}, \
    {translation, 0, Enable translation mode}",

    "{portlist, CLI_WORD, NULL, NULL, 256}",

    CLI_MODE_CONFIG_MCAST,
    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,
    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdMulticastMulticastTagSet
},

/*
{
    "config enable",

    "{config, 0, config},\
    {enable, 0, enable}",

    "",

    CLI_MODE_CONFIG_MCAST,
    CLI_NULL_SUB_MODE,
    ACCESS_LEVEL_USER,
    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdMulticastConfigEnable
},



*/
{
    "tag-translation port {{list portlist}|all} mcvlan mcvid uservlan uvid",

    "{tag-translation, 0, Configure tag-translation entry}, \
    {port, 0, Port},\
    {list, 0, List}, \
    {portlist, 1, "PORTLIST_HELP"}, \
    {all, 0, All ports}, \
    {mcvlan, 0, Multicast vlan}, \
    {mcvid, 1, Multicast vlan ID}, \
    {uservlan, 0, User vlan}, \
    {uservid, 1, User vlan ID}",

    "{portlist, CLI_WORD, NULL, NULL, 256}, \
    {mcvid, CLI_UINT, 1, 4094, 1}, \
    {uservid, CLI_UINT, 1, 4094, 1}",

    CLI_MODE_CONFIG_MCAST,
    CLI_NULL_SUB_MODE,

  ACCESS_LEVEL_USER,
    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdMulticastMulticastTagSetTranslation
},

{
    "group-limit port {{list portlist}|all} limit",

    "{group-limit, 0, Configure group limit}, \
    {port, 0, Port},\
    {list, 0, List}, \
    {portlist, 1, "PORTLIST_HELP"}, \
    {all, 0, All ports}, \
    {limit, 1, Group limit value}",

    "{portlist, CLI_WORD, NULL, NULL, 256}, \
    {limit, CLI_UINT, 0, 64, 0}",

    CLI_MODE_CONFIG_MCAST,
    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,
    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdMulticastMulticastGroupLimit
},

{
    "macgroup create port {{list portlist}|all} vlan vlanid mac macaddr",

    "{macgroup, 0, Multicast group entry}, \
    {create, 0, Create a multicast group entry}, \
    {port, 0, Port},\
    {list, 0, List}, \
    {portlist, 1, "PORTLIST_HELP"}, \
    {all, 0, All ports}, \
    {vlan, 0, Vlan}, \
    {vlanid, 1, Vlan ID}, \
    {mac, 0, Multicast mac address}, \
    {macaddr, 1, Multicast mac address (xx:xx:xx:xx:xx:xx)}",

    "{portlist, CLI_WORD, NULL, NULL, 256}, \
    {vlanid, CLI_UINT, 1, 4094, 1}, \
    {macaddr, CLI_MACADDR, NULL, NULL, 17}",

    CLI_MODE_CONFIG_MCAST,
    CLI_NULL_SUB_MODE,

  ACCESS_LEVEL_USER,
    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdMulticastMulticastGroupCreate
},

{
    "macgroup delete port {{list portlist}|all} vlan vlanid mac macaddr",

    "{macgroup, 0, Multicast group entry}, \
    {delete, 0, Delete a multicast group entry}, \
    {port, 0, Port},\
    {list, 0, List}, \
    {portlist, 1, "PORTLIST_HELP"}, \
    {all, 0, All ports}, \
    {vlan, 0, Vlan}, \
    {vlanid, 1, Vlan ID}, \
    {mac, 0, Multicast mac address}, \
    {macaddr, 1, Multicast mac address (xx:xx:xx:xx:xx:xx)}",

    "{portlist, CLI_WORD, NULL, NULL, 256}, \
    {vlanid, CLI_UINT, 1, 4094, 1}, \
    {macaddr, CLI_MACADDR, NULL, NULL, 17}",

    CLI_MODE_CONFIG_MCAST,
    CLI_NULL_SUB_MODE,

  ACCESS_LEVEL_USER,
    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdMulticastMulticastGroupDelete
},

{
    "clear vlan {{vlanid|all} group",

    "{clear, 0, Clear multicast group},\
    {vlan, 0, Vlan}, \
    {vlanid, 1, Vlan ID}, \
    {all, 0, All vlans}, \
    {group, 0, Multicast group}",

    "{vlanid, CLI_UINT, 1, 4094, 1}",

    CLI_MODE_CONFIG_MCAST,
    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,
    CLI_EXEC_FUNC,
    (FUNCPTR)cliCmdMulticastMulticastGroupClear
},

{
    "ipgroup create port {{list portlist}|all} vlan vlanid gdaip ipaddress",

    "{ipgroup, 0, Multicast group entry}, \
    {create, 0, Create a multicast group entry}, \
    {port, 0, Port},\
    {list, 0, List}, \
    {portlist, 1, "PORTLIST_HELP"}, \
    {all, 0, All ports}, \
    {vlan, 0, Vlan}, \
    {vlanid, 1, Vlan ID}, \
    {gdaip, 0, Multicast ip address}, \
    {ipaddress,1,Input ip address(xx.xx.xx.xx)}",

    "{portlist, CLI_WORD, NULL, NULL, 256}, \
    {vlanid, CLI_UINT, 1, 4094, 1}, \
    {ipaddress,CLI_IPDOTADDR,0.0.0.0, 255.255.255.255, 0.0.0.0}",

    CLI_MODE_CONFIG_MCAST,
    CLI_NULL_SUB_MODE,

  ACCESS_LEVEL_USER,
    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdMulticastMulticastGroupCreateByIp
},

{
    "ipgroup delete port {{list portlist}|all} vlan vlanid gdaip ipaddress",

    "{ipgroup, 0, Multicast group entry}, \
    {delete, 0, Delete a multicast group entry}, \
    {port, 0, Port},\
    {list, 0, List}, \
    {portlist, 1, "PORTLIST_HELP"}, \
    {all, 0, All ports}, \
    {vlan, 0, Vlan}, \
    {vlanid, 1, Vlan ID}, \
    {gdaip, 0, Multicast ip address}, \
    {ipaddress,1,Input ip address(xx.xx.xx.xx)}",

    "{portlist, CLI_WORD, NULL, NULL, 256}, \
    {vlanid, CLI_UINT, 1, 4094, 1}, \
    {ipaddress,CLI_IPDOTADDR,0.0.0.0, 255.255.255.255, 0.0.0.0}",

    CLI_MODE_CONFIG_MCAST,
    CLI_NULL_SUB_MODE,

  ACCESS_LEVEL_USER,
    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdMulticastMulticastGroupDeleteByIp
},

{
    "auth logical-onu-id loid [password pwd]",

    "{auth, 0, ONU logical id configuration},\
    {logical-onu-id, 0, ONU logical id}, \
    {loid, 1, ONU logical id(1-24 characters)}, \
    {password, 0, Password}, \
    {pwd, 1, Password(1-12 characters)}",

    "{loid, CLI_WORD, NULL, NULL, 24}, \
    {pwd, CLI_WORD, NULL, NULL, 12}",

    CLI_MODE_ENABLE | CLI_MODE_SUPER,
    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,
    CLI_EXEC_FUNC,
    (FUNCPTR)cliCmdOnuLogicalIdSet
},
{
    "mpcp state holdover {enable|disable}",

    "{mpcp, 0, MPCP state holdover control},\
    {state, 0, MPCP state holdover control}, \
    {holdover, 0, MPCP state holdover control}, \
    {enable, 0, Enable mpcp state holdover}, \
    {disable, 0, Disable mpcp state holdover}",

    "",

    CLI_MODE_CONFIG_PON,
    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,
    CLI_EXEC_FUNC,
    (FUNCPTR)cliCmdOnuMpcpHoldoverContrlSet
},
{
    "mpcp state holdover time holdtime",

    "{mpcp, 0, MPCP state holdover time configuration},\
    {state, 0, MPCP state holdover time configuration}, \
    {holdover, 0, MPCP state holdover time configuration}, \
    {time, 0, MPCP state holdover time configuration}, \
    {holdtime, 1, Holdover time value}",

    "{holdtime, CLI_UINT, 1, 68000, 200}",

    CLI_MODE_CONFIG_PON,
    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,
    CLI_EXEC_FUNC,
    (FUNCPTR)cliCmdOnuMpcpHoldoverTimeSet
},


/* multicast test command *
{
    "multicast_test",

    "{multicast_test,0, multicast_test}",

    "",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

    ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdMulticastIgmpTest
}
*/
    /* begin added by jiangmingli for host debug */
    {
        "host debug {us | ds} {enable | disable}",

        "{host, 0, Host},\
        {debug, 0, Debug}, \
        {us, 0, Upstream}, \
        {ds, 0, Downstream}, \
        {enable, 0, Enable}, \
        {disable, 0, Disable}",

        "",

        CLI_MODE_ENABLE | CLI_MODE_CONFIG_SYS | CLI_MODE_SUPER,

        CLI_NULL_SUB_MODE,

	    ACCESS_LEVEL_USER,

        CLI_EXEC_FUNC,
        (FUNCPTR)cliCmdHostDbgEn
    },
    /* end added by jiangmingli for host debug */

#if defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
    {
        "atheros read device dev phy phyaddr register regaddr number num",

        "{atheros,0,Atheros control},\
         {read,0,Read Atheros register},\
         {device,0,Atheros device},\
         {dev,1,Input device address},\
         {phy,0,Atheros PHY address},\
         {phyaddr,1,Input PHY address},\
         {register,0,Atheros register address},\
         {regaddr,1,Input register address},\
         {number,0,The number of register to read},\
         {num,1,Input the number of register to read}",

        "{dev,CLI_UINT,0,31,0},\
         {phyaddr,CLI_UINT,0,31,0},\
         {regaddr,CLI_UINT,0,65535,0},\
         {num,CLI_UINT,1,16,1}",

        CLI_MODE_SUPER,

        CLI_NULL_SUB_MODE,

	    ACCESS_LEVEL_SUPER,

        CLI_EXEC_FUNC,

        (FUNCPTR)cliCmdAtherosPhyRead
    },
    {
        "atheros write device dev phy phyaddr register regaddr value val",

        "{atheros,0,Atheros control},\
         {write,0,Write Atheros register},\
         {device,0,Atheros device},\
         {dev,1,Input device address},\
         {phy,0,Atheros PHY address},\
         {phyaddr,1,Input PHY address},\
         {register,0,Atheros register address},\
         {regaddr,1,Input register address},\
         {value,0,The value of register to write},\
         {val,1,Input the value of register to write}",

        "{dev,CLI_UINT,0,31,0},\
         {phyaddr,CLI_UINT,0,31,0},\
         {regaddr,CLI_UINT,0,65535,0},\
         {val,CLI_UINT,0,4294967295,0}",

        CLI_MODE_SUPER,

        CLI_NULL_SUB_MODE,

	    ACCESS_LEVEL_SUPER,

        CLI_EXEC_FUNC,

        (FUNCPTR)cliCmdAtherosPhyWrite
    },
    {
        "atheros read device dev register addr [{number num}|{offset offsetval width widthval}]",

        "{atheros,0,Atheros control},\
         {read,0,Read Atheros register},\
         {device,0,Atheros device},\
         {dev,1,Input device address},\
         {register,0,Atheros register address},\
         {addr,1,Input register address},\
         {number,0,The number of register to read},\
         {num,1,Input the number of register to read},\
         {offset,0,The offset of register to read},\
         {offsetval,1,Input offset of register to read},\
         {width,0,The width of register to read},\
         {widthval,1,Input the width of register to read}",

        "{dev,CLI_UINT,0,31,0},\
         {addr,CLI_UINT,0,65535,0},\
         {num,CLI_UINT,1,256,1},\
         {offsetval,CLI_UINT,0,31,0},\
         {widthval,CLI_UINT,1,32,1}",

        CLI_MODE_SUPER,

        CLI_NULL_SUB_MODE,

	    ACCESS_LEVEL_SUPER,

        CLI_EXEC_FUNC,

        (FUNCPTR)cliCmdAtherosRegRead
    },
    {
        "atheros write device dev register addr {{value val}|{offset offsetval width widthval field-value fld-val}}",

        "{atheros,0,Atheros control},\
         {write,0,Write Atheros register},\
         {device,0,Atheros device},\
         {dev,1,Input device address},\
         {register,0,Atheros register address},\
         {addr,1,Input register address},\
         {value,0,The value of register to write},\
         {val,1,Input the value of register to write},\
         {offset,0,The offset of register to write},\
         {offsetval,1,Input offset of register to write},\
         {width,0,The width of register to write},\
         {widthval,1,Input the width of register to write},\
         {field-value,0,The field-value of register to write},\
         {fld-val,1,Input the field-value of register to write}",

        "{dev,CLI_UINT,0,31,0},\
         {addr,CLI_UINT,0,65535,0},\
         {val,CLI_UINT,0,4294967295,0},\
         {offsetval,CLI_UINT,0,31,0},\
         {widthval,CLI_UINT,1,32,1},\
         {fld-val,CLI_UINT,0,4294967295,0}",

        CLI_MODE_SUPER,

        CLI_NULL_SUB_MODE,

	    ACCESS_LEVEL_SUPER,

        CLI_EXEC_FUNC,

        (FUNCPTR)cliCmdAtherosRegWrite
    },
      {
        "atheros classify  show",

        "{atheros, 0, atheros},\
        {classify, 0, classify}, \
        {show, 0, show}",

        "",

        CLI_MODE_SUPER,

        CLI_NULL_SUB_MODE,

	    ACCESS_LEVEL_USER,

        CLI_EXEC_FUNC,

        (FUNCPTR)cliCmdAtherosClassifyWrite
    },
          {
        "atheros counter show port",

        "{atheros, 0, atheros},\
        {counter, 0, counter}, \
        {show, 0, show}, \
        {port, 1, port}",

		"{port, CLI_UINT, 0, 6, 1}",

        CLI_MODE_COMMON,

        CLI_NULL_SUB_MODE,

	    ACCESS_LEVEL_USER,

        CLI_EXEC_FUNC,

        (FUNCPTR)cliCmdAtherosMibShow
    },

    {
        "atheros vtt show port",

        "{atheros, 0, atheros},\
        {vtt, 0, vtt}, \
        {show, 0, show}, \
        {port, 1, port}",

		"{port, CLI_UINT, 1, 4, 1}",

        CLI_MODE_COMMON,

        CLI_NULL_SUB_MODE,

	    ACCESS_LEVEL_USER,

        CLI_EXEC_FUNC,

        (FUNCPTR)cliCmdVttShow
    },

    {
        "atheros vlan show vid",

        "{atheros, 0, atheros},\
        {vlan, 0, vlan}, \
        {show, 0, show}, \
        {vid, 1, vid}",

		"{vid, CLI_UINT, 0, 4095, 32}",

        CLI_MODE_COMMON,

        CLI_NULL_SUB_MODE,

	    ACCESS_LEVEL_USER,

        CLI_EXEC_FUNC,

        (FUNCPTR)cliCmdVlanShow
    },
#endif

    {
        "onu silence {enable|disable}",

        "{onu, 0, Onu},\
        {silence, 0, Silence}, \
        {enable, 0, Enable}, \
        {disable, 0, Disable}",

        "",

        CLI_MODE_CONFIG_PON,

        CLI_NULL_SUB_MODE,

	    ACCESS_LEVEL_USER,

        CLI_EXEC_FUNC,

        (FUNCPTR)cliCmdOnuSilenceEnSet
    },

	{
		"onu silence time value",

		"{onu, 0, Configure onu silence time},\
		{silence, 0, Configure onu silence time}, \
		{time, 0, Configure onu silence time}, \
		{value, 1, Input silence time value}",

		"{value,CLI_UINT,0,180,60}",

		CLI_MODE_CONFIG_PON,

		CLI_NULL_SUB_MODE,

	    ACCESS_LEVEL_USER,

		CLI_EXEC_FUNC,

		(FUNCPTR)cliCmdOnuSilenceTimeSet
	},

    {
		"show thread",

		"{show, 0, show},\
		 {thread, 0, Show thread information}",

	    "",

        CLI_MODE_SUPER,

        CLI_NULL_SUB_MODE,

	    ACCESS_LEVEL_SUPER,

        CLI_EXEC_FUNC,

		(FUNCPTR)cliCmdShowThreadInfo
    },
#if 0 /*modified by xhpan for bug 3176*/
    {
		"optical module {tx-on|tx-off}",

		"{optical, 0, Optical control},\
		 {module, 0, Optical module control},\
		 {tx-on, 0, Turn on optical module tx},\
		 {tx-off, 0, Turn off optical module tx}",

	    "",

        CLI_MODE_SUPER,

        CLI_NULL_SUB_MODE,

	    ACCESS_LEVEL_SUPER,

        CLI_EXEC_FUNC,

		(FUNCPTR)cliCmdOpticalModuleOnOff
    },
    #endif
    {
        "oam ctc-ver {2.0|2.1|auto}",
        "{oam, 0, OAM configuration},\
        {ctc-ver, 0, CTC version},\
        {2.0, 0, Support CTC 2.0 version only},\
        {2.1, 0, Support CTC 2.1 version only}, \
        {auto, 0, Auto negotiate CTC version}",

        "",
        CLI_MODE_ENABLE | CLI_MODE_SUPER,
        CLI_NULL_SUB_MODE,

        ACCESS_LEVEL_USER,
        CLI_EXEC_FUNC,
        (FUNCPTR)CliOamVersionSet
	},
    {
	    "tftp {get|put} serverip ipaddress localfile lfile remotefile rfile",

	    "{tftp, 0, Transfer a file from/to tftp server},\
	    {get, 0, Get file from server},\
	    {put, 0, Put local file to server},\
	    {serverip,0, The tftp server ip address},\
	    {ipaddress,1, The ip address of server},\
	    {localfile,0, The local file},\
	    {lfile,1, The source file},\
	    {remotefile,0,The remote file},\
        {rfile,1,The remote file}",


		"{ipaddress,CLI_IPDOTADDR,0.0.0.0, 255.255.255.255, 0.0.0.0},\
		 {lfile,CLI_WORD, NULL, NULL, 256},\
		 {rfile,CLI_WORD, NULL, NULL, 256}",

	    CLI_MODE_CONFIG_SYS,

	    CLI_NULL_SUB_MODE,

		ACCESS_LEVEL_ADMINISTRATION,

	    CLI_EXEC_TASK,

	    (FUNCPTR)cliCmdTftp
	},
    {
	    "ftp {get|put} serverip ip username name password passwd localfilename localfile remotefilename remotefile",

	    "{ftp, 0, Transfer a file from/to ftp server},\
		 {get, 0, Get file from ftp server},\
		 {put, 0, Put file to ftp server},\
	     {serverip,0, The ip address of file transterred from},\
	     {ip,1, The server ip address},\
	     {username,0, The user name allowed to access the server},\
	     {name,1, The user name},\
         {password,0, The password allowed to access the server},\
	     {passwd,1,The password},\
         {localfilename,0,The local file name that to transter from or to},\
         {localfile,1,The local file name },\
         {remotefilename,0,The remote file name that to transter from or to},\
         {remotefile,1,The file name that to transter from or to}",


		"{ip,CLI_IPDOTADDR,0.0.0.0, 255.255.255.255, 0.0.0.0},\
		 {name,CLI_WORD, NULL, NULL, 32},\
		 {passwd,CLI_WORD, NULL, NULL, 32},\
         {localfile,CLI_WORD, NULL, NULL,256},\
         {remotefile,CLI_WORD, NULL, NULL,256}",

	    CLI_MODE_CONFIG_SYS,

	    CLI_NULL_SUB_MODE,

		ACCESS_LEVEL_ADMINISTRATION,

	    CLI_EXEC_TASK,

	    (FUNCPTR)cliCmdFtp
	},

    {
        "datapath-mtu size",

        "{datapath-mtu,0,Set Maximum DataPath Transmission Unit},\
         {size,1,Enter size}",

        "{size,CLI_UINT,64,2000,1518}",

        CLI_MODE_CONFIG_PON,

        CLI_NULL_SUB_MODE,

    	ACCESS_LEVEL_USER,

        CLI_EXEC_FUNC,

        (FUNCPTR)cliCmdDataPathMtuSizeSet

    },
    {
        "show datapath-mtu",

	    "{show, 0,Show information},\
	    {datapath-mtu,0,Show datapath-mtu}",

	    "",

        CLI_MODE_COMMON,

        CLI_NULL_SUB_MODE,

    	ACCESS_LEVEL_USER,

        CLI_EXEC_FUNC,

        (FUNCPTR)cliCmdDataPathMtuSizeGet

    },

	{
    "buffer reset {enable|disable}",

    "{buffer,0,buffer},\
     {reset,0,reset},\
     {enable,0,clear pon buffer},\
     {disable,0,not clear pon buffer}",

	"",

    CLI_MODE_CONFIG_PON,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdPonBufferResetConfig

},

{
    "sw-mdio read device dev register addr",

    "{sw-mdio,0,MDIO control},\
     {read,0,Read MDIO register},\
     {device,0,MDIO device},\
     {dev,1,Input device address},\
     {register,0,MDIO register address},\
     {addr,1,Input register address}",

    "{dev,CLI_UINT,0,31,0},\
     {addr,CLI_UINT,0,65535,0}",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdSwMdioRead
},
{
    "sw-mdio write device dev register addr value val",

    "{sw-mdio,0,MDIO control},\
     {write,0,Write MDIO register},\
     {device,0,MDIO device},\
     {dev,1,Input device address},\
     {register,0,MDIO register address},\
     {addr,1,Input register address},\
     {value,0,The value of register to write},\
     {val,1,Input the value of register to write}",

    "{dev,CLI_UINT,0,31,0},\
     {addr,CLI_UINT,0,65535,0},\
     {val,CLI_UINT,0,65535,0}",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdSwMdioWrite
},
{
    "sw-mdio phy read device dev phy phyid register addr",

    "{sw-mdio,0,MDIO control},\
     {phy,0,phy},\
     {read,0,Read MDIO register},\
     {device,0,MDIO device},\
     {dev,1,Input device address},\
     {phy,0,phy},\
     {phyid,1,phyid},\
     {register,0,MDIO register address},\
     {addr,1,Input register address}",

    "{dev,CLI_UINT,0,31,0},\
     {phyid,CLI_UINT,0,31,0},\
     {addr,CLI_UINT,0,65535,0}",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdSwMdioPhyRead
},
{
    "sw-mdio phy write device dev phy phyid register addr value val",

    "{sw-mdio,0,MDIO control},\
     {phy,0,phy},\
     {write,0,Write MDIO register},\
     {device,0,MDIO device},\
     {dev,1,Input device address},\
     {phy,0,phy},\
     {phyid,1,phyid},\
     {register,0,MDIO register address},\
     {addr,1,Input register address},\
     {value,0,The value of register to write},\
     {val,1,Input the value of register to write}",

    "{dev,CLI_UINT,0,31,0},\
     {phyid,CLI_UINT,0,31,0},\
     {addr,CLI_UINT,0,65535,0},\
     {val,CLI_UINT,0,65535,0}",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdSwMdioPhyWrite
},
#if defined(CONFIG_PRODUCT_EM200)
{
    "single-mdio dbg write register addr value val",

    "{single-mdio,0,MDIO control},\
     {dbg,0,dbg register},\
     {write,0,Write MDIO register},\
     {register,0,MDIO register address},\
     {addr,1,Input register address},\
     {value,0,The value of register to write},\
     {val,1,Input the value of register to write}",

    "{addr,CLI_UINT,0,65535,0},\
     {val,CLI_UINT,0,65535,0}",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdSingleMdioDbgWrite
},
{
    "single-mdio dbg read register addr",

    "{single-mdio,0,MDIO control},\
     {dbg,0,dbg},\
     {read,0,Read MDIO register},\
     {register,0,MDIO register address},\
     {addr,1,Input register address}",

    "{addr,CLI_UINT,0,65535,0}",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdSingleMdioDbgRead
},
#endif

{
    "i2c init mode intmode rate",

    "{i2c,0,I2C control},\
     {init,0,init},\
     {mode,1,I2C mode},\
     {intmode,1,intmode},\
     {rate_mode,1,Input rate mode}",

    "{mod,CLI_UINT,0,1,0},\
     {intmode,CLI_UINT,0,1,0},\
     {rate_mode,CLI_UINT,0,7,0}",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdI2cInit
},
{
    "i2c read addr regaddr nbyte",

    "{i2c,0,I2C control},\
     {read,0,read i2c},\
     {slavaddr,1,slavaddr},\
     {reg,1,reg},\
     {byte,1,byes}",

    "{slavaddr,CLI_UINT,0,65535,0},\
     {reg,CLI_UINT,0,65535,0},\
     {byte,CLI_UINT,0,32,0}",

    CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_SUPER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmdI2cRead
},

{
	"mode {base | flexible}",

	"{mode, 0, configure QinQ mode}, \
	{base, 0, configure QinQ to base mode}, \
	{flexible, 0, configure QinQ to flexible mode}",

	"",

	CLI_MODE_CONFIG_PON,

	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

	CLI_EXEC_FUNC,
	(FUNCPTR)cliCmdQinQMode
},
{
	"base {add | delete} vlan remarkPri insertPri",

	"{base, 0, configure QinQ base vlan}, \
	{add ,0, add the QinQ base vlan},\
	{delete,0, delete the QinQ base vlan},\
	{vlanid, 1, configure QinQ base vlan},\
	{remarkPri, 1, configure remark vlan priority},\
	{insertPri, 1, configure insert vlan priority}",

	"{vlanid, CLI_UINT, 1, 4094, 1},\
	{remarkPri, CLI_UINT, 1, 8, 1},\
	{insertPri, CLI_UINT, 1, 8, 1}",

	CLI_MODE_CONFIG_PON,

	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

	CLI_EXEC_FUNC,
	(FUNCPTR)cliCmdQinQBase
},
{
	"transparent {add | delete} vlan",

	"{transparent, 0, configure QinQ transparent vlan}, \
	{add ,0, add the QinQ transparent vlan},\
	{delete,0, delete the QinQ transparent vlan},\
	{vlanid, 1, configure QinQ transparent vlan}",

	"{vlanid, CLI_UINT, 1, 4094, 1}",

	CLI_MODE_CONFIG_PON,

	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

	CLI_EXEC_FUNC,
	(FUNCPTR)cliCmdQinQTransparent
},

{
	"flexible {add | delete} svlan svlanid cvlan cvlanid spri priority",

	"{flexible, 0, configure QinQ flexible vlan}, \
	{add ,0, add the QinQ flexible vlan},\
	{delete,0, delete the QinQ flexible vlan},\
	{svlan, 0, configure QinQ svlan Id},\
	{svlan, 1, svlan vlan},\
	{cvlan, 0, configure QinQ cvlan Id},\
	{cvlan, 1, cvlan vlan},\
	{spri, 0, Configure svlan priority},\
	{priority, 1, svlan priority}",

	"{svlanid, CLI_UINT, 1, 4094, 1},\
	{cvlanid, CLI_UINT, 1, 4094, 1},\
	{priority, CLI_UINT, 1, 8, 1}",

	CLI_MODE_CONFIG_PON,

	CLI_NULL_SUB_MODE,

	ACCESS_LEVEL_USER,

	CLI_EXEC_FUNC,
	(FUNCPTR)cliCmdQinQFlexible
},
#ifdef CONFIG_BOSA
{
    "imodset registerval",

    "{imodset,0, Set IMODSET register of 25L90},\
    {registerval, 1, register value}",

    "{registerval, CLI_UINT, 0, 255, 1}",

    CLI_MODE_ENABLE | CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

    ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmd25L90iModSet
},
{
    "apcset registerval",

    "{apcset,0, Set APCSET register of 25L90},\
    {registerval, 1, register value}",

    "{registerval, CLI_UINT, 0, 255, 1}",

    CLI_MODE_ENABLE | CLI_MODE_SUPER,

    CLI_NULL_SUB_MODE,

    ACCESS_LEVEL_USER,

    CLI_EXEC_FUNC,

    (FUNCPTR)cliCmd25L90ApcSet
},
#endif


};

/*****************************************************************************
 *cliCmdCountGet  - Get command number.
 *DESCRIPTION
 *.
 *Input:N/A.
 *Output:N/A.
 *Return:int.
 ****************************************************************************/
int cliCmdCountGet(void)
{
    return(sizeof(g_asCliCmdTable)/sizeof(MODEL_INTF_INFO_t));
}

