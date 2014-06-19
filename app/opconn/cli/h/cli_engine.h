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
* FILENAME:  cli_engine.h
*
* DESCRIPTION: 
*	cli module engine
*
* Date Created: Aug 12, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/cli/h/cli_engine.h#1 $
* $Log:$
*
*
**************************************************************************/
 
#ifndef __CLI_ENGINE_H_
#define __CLI_ENGINE_H_
 
#ifdef __cplusplus
extern "C" {
#endif
 
#include <sys/socket.h>
#include <netinet/in.h>
#include "defs.h"


#define CTRLC                       0x03    /* Ctrl+C */
#define NL                          0x0A    /* line feed, new line */
#define CR                          0x0D    /* carriage return */
#define BSP                         0x08    /* backspace */
#define BLANKSPACE                  0x20    /* space */
#define TAB                         0x09    /* TAB */
#define CTRLZ                       0x1A    /* Ctrl+Z  */
#define RUBOUT                      0x7F    /* RUBOUT  */
#define CTRLP                       0x10    /* Ctrl+P  */
#define CTRLN                       0x0E    /* Ctrl+N  */

#define CURSOR_UP                   0x1001  /* arrow up */
#define CURSOR_DOWN                 0x1002  /* arrow down */
#define CURSOR_LEFT                 0x1003  /* arrow left */
#define CURSOR_RIGHT                0x1004  /* arrow right */

#define LEFT_FLAG_CHARACTOR         '^'

#define TERMINAL_IO_FAULT           0xffff  /* failed to read from I/O */

#define CLI_CMD_PHASE_INIT          0       /* init phase */
#define CLI_CMD_PHASE_SUPPLY        1       /* Press <TAB> */     
#define CLI_CMD_PHASE_HELP          2       /* Press <?> */
#define CLI_CMD_PHASE_WHOLE         3       /* Press <CR> */ 
#define CLI_CMD_PHASE_HISTORY       4       /* doskey */
#define CLI_CMD_PHASE_EXIT          5       /* now for console only */
#define CLI_CMD_PHASE_SYS_SHELL     6       /* enter system shell */

#define TERMINAL_MODE_NORMAL        0       /* for debugging */
#define TERMINAL_MODE_VT100_C       1       /* VT_100, CONSOLE */
#define TERMINAL_MODE_VT100_N       2       /* VT_100, TELNET */
#define TERMINAL_MODE_RESERVE               /* RESERVED */

#define CLI_CLOSE_NULL              0       /* connective */
#define CLI_CLOSE_ACTIVE            1       /* active close */
#define CLI_CLOSE_PASSIVE           2       /* passive close */


#define CLI_EXEC_FUNC               0       /* execute by function call */
#define CLI_EXEC_TASK               1       /* execute by task */

#define FORWARD                     1       /* Forward search */
#define BACKWARD                    -1      /* backward search */
                                            
#define EQ                          0       /* equal */
#define NEQ                         1       /* not equal */
#define RANGE                       2       /* range */
#define GT                          3       /* greater than */
#define LT                          4       /* less than */
                                            
#define EXTRA_TYPE_A                5       /* continuous data type */
#define EXTRA_TYPE_D                6       /* discrete data type */
#define EXTRA_TYPE_STRING           7       /* string type */

/*  command process definition */
#define CLI_CHILD_TASK_PRIORITY     120
#define CLI_CHILD_TASK_STACK_SIZE   0x20000

#define ENABLE_MODE_PWD             "admin"  /* default password of enable mode  */
//#define SUPER_MODE_PWD              "Opconn<M>!^*"  /* default password of super mode  */
#define SUPER_MODE_PWD              "root" 

#define CLI_MODE_USER               0x01    /* user mode */
#define CLI_MODE_ENABLE             0x02    /* enable mode */
#define CLI_MODE_CONFIG_SYS         0x04    /* config system mode */
#define CLI_MODE_CONFIG_USER        0x08    /* config user mode */
#define CLI_MODE_CONFIG_QOS         0x10    /* config qos mode */
#define CLI_MODE_CONFIG_FDB         0x20    /* config fdb mode */
#define CLI_MODE_CONFIG_MCAST       0x40    /* config multicast mode */
#define CLI_MODE_CONFIG_VLAN        0x80    /* config vlan mode */
#define CLI_MODE_CONFIG_PORT        0x100   /* config port mode */
#define CLI_MODE_CONFIG_RSTP        0x200   /* config rstp mode */
#define CLI_MODE_CONFIG_PON         0x400   /* config pon mode */
#define CLI_MODE_CONFIG_STORM       0x800   /* config storm mode */
#define CLI_MODE_CONFIG_QOS_CLASS   0x1000  /* config qos class mode */
#define CLI_MODE_CONFIG_MIRROR    0x2000  /* config mirror mode */
#define CLI_MODE_SUPER              0x4000  /* super mode */


#define CLI_MODE_COMMON             (CLI_MODE_USER | \
                                    CLI_MODE_ENABLE | \
                                    CLI_MODE_CONFIG_SYS | \
                                    CLI_MODE_CONFIG_USER | \
                                    CLI_MODE_CONFIG_QOS | \
                                    CLI_MODE_CONFIG_FDB | \
                                    CLI_MODE_CONFIG_MCAST | \
                                    CLI_MODE_CONFIG_VLAN | \
                                    CLI_MODE_CONFIG_PORT | \
                                    CLI_MODE_CONFIG_RSTP | \
                                    CLI_MODE_CONFIG_PON | \
                                    CLI_MODE_CONFIG_STORM | \
                                    CLI_MODE_CONFIG_QOS_CLASS | \
                                    CLI_MODE_CONFIG_MIRROR | \
                                    CLI_MODE_SUPER)

#define CLI_SPACIFY_SUB_MODE        0x8000 /* spacified mode */
#define NUM_OF_CLI_MAIN_MODE        15     /* the number of cli main mode */
#define CLI_NULL_SUB_MODE           0x0    /* no sub-mnode */
#define NUM_OF_CLI_SUB_MODE         0      /* the number of sub-mode */
#define NUM_OF_CLI_MODE             (NUM_OF_CLI_MAIN_MODE+NUM_OF_CLI_SUB_MODE+1) /* the total mode number */
#define DEF_CLI_MODE                CLI_MODE_USER

#define MAX_POOL_LENGTH            (8*1024) /* pool size */
#define MAX_HISTORY_CMD_NUMBER     20  /* max history number */

#define SAVE                       0   /* save command */
#define GETPRE                     1   /* get the previous command */
#define GETNEXT                    2   /* get the next command */

#define DISPLAYWIDTH MAX_POOL_LENGTH   /* we'll change line if too long */
#define LINEWIDTH                  78  /* display charater number per line */
#define INDENTWIDTH                1   /* indent width */


/*  parameter type definition */
#define CLI_WORD                   0x01   /* character type */
#define CLI_STRING                 0x02   /* string type */
#define CLI_INTEGER                0x04   /* integer type */
#define CLI_UINT                   0x08   /* unsigned integer type */
#define CLI_IPDOTADDR              0x10   /* IP address with dot */
#define CLI_IPINTADDR              0x20   /* IP address without dot */
#define CLI_IPDOTMASK              0x40   /* subnet mask with dot */
#define CLI_IPINTMASK              0x80   /* subnet mask without dot */
#define CLI_MACADDR                0x100  /* MAC address */
#define CLI_DATE                   0x200  /* Date type */ 
#define CLI_TIME                   0x400  /* Time type */


/* command tree definition */
#define NT_REQUIRED_KEY            0x01   /* required key */      
#define NT_OPTIONAL_KEY            0x02   /* optional key */
#define NT_KEY                     0x03   /* key */
#define NT_REQUIRED_PARAMETER      0x04   /* required parameter */
#define NT_OPTIONAL_PARAMETER      0x08   /* optional parameter */
#define NT_PARAMETER               0x0c   /* parameter */
#define NT_VIRTUAL_REQUIRED        0x10   /* virtual required */
#define NT_VIRTUAL_OPTIONAL        0x20   /* virtual optional */
#define NT_VIRTUAL                 0x30   /* virtual */

/*
  virtual_load used when tow or more commands share a common prefix
  this type of node's explain pointer points to virtual_function type node
 */
#define NT_VIRTUAL_LOAD            0x40         

/*
  virtual_function used when two or more commands share a common prefix
  this type of node's explain pointer points to the rest of a command
  except the common prefix
 */
#define NT_VIRTUAL_FUNCTION        0x80

/* the last node of a command's tree */
#define NT_VIRTUAL_COMMAND         0x0100       

/* Define word type */
#define WORD_TAB                   0x01  /* TAB key */
#define WORD_QUESTION              0x02  /* '?' key */
#define WORD_END_TAB               0x04  /* end of 'TAB' */
#define WORD_END_QUESTION          0x08  /* end of '?' */
#define WORD_NORMAL                0x10  /* normal word */

#pragma pack(1)		/* pragma pack definition */

typedef struct CMD_POOL_s
{
	char achCmdPool[MAX_POOL_LENGTH+1];  /* command pool */
	int  nDispBegin;                     /* the beginning of display command index */  
	int  nDispEnd;                       /* the end of display command index */
    int  nDispLen;                       /* the length of display command */
	int  nCursorPos;                     /* current cursor position */	
	int  nInputPos;                      /* the cursor position of input command */
	int  nInputTailPos;                  /* the tail cursor of input command */
    int  nInputLen;                      /* the length of input command */
	BOOL bIsInsert;                      /* whether it is insert mode */
	BOOL bIsLeftFlag;                    /* whether it is overflow on left side */
	BOOL bIsRightFlag;                   /* whether it is overflow on right side */
	BOOL bIsNeedMove;                    /* whether it need to move cursor */
	BOOL bIsPosAtHead;                   /* whether the cursor is at the beginning */
	BOOL bIsPosAtTail;                   /* whether the cursor is at the end */
    BOOL bIsEmpty;                       /* whether the command pool is empty */
    BOOL bIsFull;                        /* whether the command pool is full */
} CMD_POOL_t;

typedef struct CMD_HISTORY_s
{
	int head;                          /* the header of the command */
	int tail;                          /* the tailer of the command */
	int pos;                           /* the current command position */
	unsigned char flag;
	char full;                         /* whether the command pool is full */
	char *cmd[MAX_HISTORY_CMD_NUMBER]; /* the command pool */
} CMD_HISTORY_t;

/*  Define parameter information vessel data struct */
typedef struct PARA_INFO_s
{
	unsigned int type;
	int min;
    int max;
    int fail;
} PARA_INFO_t;


/*  Define function information vessel data struct */
typedef struct PARA_LINK_s
{
	int position;     /* parameter position */
	PARA_INFO_t *p;     /* parameter information */
	struct PARA_LINK_s *next;  /* the next node of parameter */
} PARA_LINK_t;

/*  Define Union for transport parameters to model */
typedef union PARA_TABLE_u
{
	int i;
	unsigned int u;
	char *p;
} PARA_TABLE_t;

typedef struct PAGING_s
{
    char    *pPrivMem;            /* the pointer of paging display */
    int     nOldStdOutFd;         /* the original STD_OUT */
    int     nPageFd;              /* the file description of paging display */
} PAGING_t;

/*  Define ENV_t vessel data struct */
typedef struct ENV_s
{
    /* the current session index */
	unsigned char ucIndex;

    /* indicate if it is console */
    unsigned char ucIsConsole;
    
    /* current session thread id */
	VOS_THREAD_t pstMainTaskId;
	
	/*
	  pstCommandTaskId is task id of task spawned to 
	  exec user command, used when send signal to 
	  task at accepting user's ctrl-c interrupt
	*/
	VOS_THREAD_t pstCommandTaskId;
	
    int closeTask;  /* use when a task need to be closed */
	
    /* use when task need parameter ,Max vars equ 5 */
    void *taskVar[5];
	
    /* use when press Esc Character */    
    int startESC;

	/* sockaddr_in_clientAddress is remote socket address */
	struct sockaddr_in clientAddress;
	
    /* the following fd can be STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO, or client socket */
    int nReadFd;
    int nWriteFd;
    int nErrFd;
	
	/* tmode(terminal mode) indicates terminal type(class) */
	unsigned char tmode;
	
	/* pmode is EXEC mode of current user */
	unsigned int pmode;

    /* pmode is EXEC sub mode of current user */
	unsigned int sub_pmode;
    
	/* Echo Bi-buttom */
	unsigned char echo;
	
	/* used indicates if this env block is used */
	unsigned char used;   

	/* command prompt, each bit indicate one parameter should be involved */
	int prompt;
	
    /* command points to command cache pool data sturct */
	CMD_POOL_t *command;
    
    /* history commands records of user and privilged mode */
	CMD_HISTORY_t *userHistoryCommand;
    
	/* history commands records of configure and forward mode */
	CMD_HISTORY_t *configureHistoryCommand;
	
	/* mode parameter */
	void *para;

    /* actual terminal width, default is 80 */
    unsigned int ulTermWidth; 
    /* actual terminal height, default is 23 */
    unsigned int ulTermHeight; 
	
	/* input error code */
	int err;
	
	/* connection was closed by terminal */
	int  close; 
	int  free;
	int  phase;

	unsigned int interface_type;
	unsigned int unit_number;
	unsigned int sub_unit_number;

	/*  print in Page  */
	PAGING_t    page;

    char *pcUserName;

	/* Access level */
	ACCESS_LEVEL_e accessLevel;

} ENV_t;


typedef struct FUNC_INFO_s
{
	int(*function_point)(ENV_t * , PARA_TABLE_t * );
	/*
	  is_task indicates if this command's function is realized
	  by creating a task
	*/
	int is_task;    
	PARA_LINK_t *parameter;
} FUNC_INFO_t;


/*  Define data struct EXTRA_CONDITION_t. */
typedef struct EXTRA_CONDITION_s
{
	void *keyorparanode;
	union _para
	{
		struct _d_para
		{
			int value;
			struct d_para *next;
			int operate;
		}d_para;
		struct _a_para
		{
			int min;
			int max;
			int operate;
		}a_para;
		struct _pstring
		{
			char *pString;
            int  strlen;
			int  operate;
		}p_string;
	}para;
	int para_type;
	
    struct EXTRA_CONDITION_s *next;
} EXTRA_CONDITION_t;

/*  Define command & keyword & parameter data struct  */
typedef struct CMD_KEY_PARA_LINK_s
{
	char *key;
	unsigned int type;
	char *description;
	struct CMD_KEY_PARA_LINK_s *left;
    struct CMD_KEY_PARA_LINK_s *right;
	struct CMD_KEY_PARA_LINK_s *pre;
    struct CMD_KEY_PARA_LINK_s *next;
	struct CMD_KEY_PARA_LINK_s *parent;
	struct CMD_KEY_PARA_LINK_s *explain;
	/*
	  Parameter_position indicates that if this is a parameter,
	  what is its position in function parameters table.
	*/
	int parameter_postion; 
	/*
	  if this keyword is one of alternational keywords, what is 
	  its position. if this key is not alternational, 0 was filled.
	*/
	int keyword_position;
	PARA_INFO_t *parameter;
	EXTRA_CONDITION_t *extra_condiction;
	unsigned int visualable;
	unsigned int exec_mode;
	unsigned int exec_sub_mode;
	ACCESS_LEVEL_e access_level;	
	FUNC_INFO_t *func_info;
	
} CMD_KEY_PARA_LINK_t;


/*  Define model interface data struct */
typedef struct MODEL_INTF_INFO_s
{
	char *command;
	char *description;
	char *parameter;
	unsigned int exec_mode;
	unsigned int exec_sub_mode;
	ACCESS_LEVEL_e access_level;
	int is_task;
	FUNCPTR p;
	
} MODEL_INTF_INFO_t;


/*  Define data struct for collecting parameters */
typedef struct PARA_COLLECTION_s
{
	PARA_TABLE_t *p;
	int pos;
	unsigned int type;
	struct PARA_COLLECTION_s *next;
	struct PARA_COLLECTION_s *last;
    
} PARA_COLLECTION_t;


/*  Define data struct for collection nodes that match some conditions */
typedef struct NODE_COLLECTION_s
{
	int total;
	CMD_KEY_PARA_LINK_t *p;
	struct NODE_COLLECTION_s *next;
	struct NODE_COLLECTION_s *last;
    
} NODE_COLLECTION_t;

/* indicate the invalid commands while loading configuration */
typedef enum CMD_VALIDITY_e
{
    CMD_VALID,
    CMD_INCOMPLETE,
    CMD_UNRECOGNIZED,
    CMD_INVALID_INPUT
} CMD_VALIDITY_t;

#pragma pack()

/*  These functions below are mainly for create the forest  */
STATUS cliWordExtract(char **,char *,int);
STATUS cliTreeCreate(CMD_KEY_PARA_LINK_t **,MODEL_INTF_INFO_t *);
STATUS cliTreeNodeCreate(char *,CMD_KEY_PARA_LINK_t **);
STATUS cliParameterInfoGet(MODEL_INTF_INFO_t *,CMD_KEY_PARA_LINK_t **);
STATUS cliTreeParameterAdjust(CMD_KEY_PARA_LINK_t *,CMD_KEY_PARA_LINK_t *,
                       int *,PARA_LINK_t **,PARA_LINK_t ***);
STATUS cliTreePointerAdjust(CMD_KEY_PARA_LINK_t *);
STATUS cliTreeNodeAdd(CMD_KEY_PARA_LINK_t **,MODEL_INTF_INFO_t *);
STATUS cliTreeVirtualNodeMatch(CMD_KEY_PARA_LINK_t *,char **,
                           CMD_KEY_PARA_LINK_t **,unsigned int);
STATUS cliTreeNodeFree(CMD_KEY_PARA_LINK_t *);
STATUS cliTreeBranchNodeMatch(CMD_KEY_PARA_LINK_t *,char **,unsigned int);
STATUS cliForestCreate(MODEL_INTF_INFO_t *,int,CMD_KEY_PARA_LINK_t **,
                     CMD_KEY_PARA_LINK_t **);

/*
  These functions below are maily for match the command and realize
  the command's function
 */
BOOL   cliTreeNodeIsAllEqual(NODE_COLLECTION_t *ps);
STATUS cliCmdInterpreter(ENV_t *,CMD_KEY_PARA_LINK_t *, CMD_VALIDITY_t *);
STATUS cliCmdMatch(ENV_t *,CMD_KEY_PARA_LINK_t *,char *,NODE_COLLECTION_t **);
STATUS cliCmdExecute(ENV_t *,PARA_COLLECTION_t *,NODE_COLLECTION_t *);
STATUS cliWordTypeGet(char *,unsigned int *);
STATUS cliCollectionFree(PARA_COLLECTION_t *,NODE_COLLECTION_t *,CMD_POOL_t *);
STATUS cliParameterCheck(PARA_COLLECTION_t *,CMD_KEY_PARA_LINK_t *,char *,char **);
STATUS cliTreeNodeMatch(CMD_KEY_PARA_LINK_t *, char *,
                        CMD_KEY_PARA_LINK_t **);

 
#ifdef __cplusplus
}
#endif
 
#endif /* #ifndef __CLI_ENGINE_H_ */

