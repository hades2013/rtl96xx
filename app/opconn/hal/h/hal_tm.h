#ifndef HAL_TM_H
#define HAL_TM_H


#pragma pack (1)

typedef enum STREAM_TYPE_s
{
	HAL_UP_STREAM = 0,
	HAL_DOWN_STREAM = 1
}STREAM_TYPE_e;

typedef struct HAL_TM_SHAPER_PAR_s
{
    /*input parameters*/
    UINT8  queue;
    STREAM_TYPE_e stream;
    /*set--input parameters.
    get--output parameters*/
    UINT8 enable;
    UINT32   cir;
    UINT32   cbs;  
    /*return value*/
    UINT32 ret;
}HAL_TM_SHAPER_PAR_t;

typedef struct HAL_TM_WRED_PAR_s
{
    /*input parameters*/
    UINT8   queue;
    STREAM_TYPE_e stream;
    /*set--input parameters.
    get--output parameters*/
    UINT8  enable;
    UINT32 weight;
    UINT32 maxth;  
    UINT32 minth;
    /*return value*/
    UINT32 ret;
}HAL_TM_WRED_PAR_t;

typedef struct HAL_TM_PRIORITY_PAR_s
{
    /*input parameters*/
    UINT8   queue;
    STREAM_TYPE_e stream;
    /*set--input parameters.
    get--output parameters*/
    UINT8   pri;
    /*return value*/
    UINT32 ret;
}HAL_TM_PRIORITY_PAR_t;


typedef struct HAL_TM_WEIGHT_PAR_s
{
    /*input parameters*/
    UINT8   queue;
    STREAM_TYPE_e stream;
    /*set--input parameters.
    get--output parameters*/
    UINT32  weight;
    /*return value*/
    UINT32 ret;
}HAL_TM_WEIGHT_PAR_t;


#pragma pack ()

void HalTmInit(void );
#endif