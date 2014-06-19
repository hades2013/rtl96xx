#include "stats.h"

#define CNT_SW_AR8306	39

OPL_CNT_t g_astCntSwhPort[SWITCH_PORT_NUM][CNT_SW_AR8306]={
{
{0, "RxBroad",  {0, 0}, "The number of good broadcast frames received."},
{0, "RxPause",  {0, 0}, "The number of PAUSE frames received."},
{0, "RxMulti",  {0, 0}, "The number of good multicast frames received."},
{0, "RxFcsErr",  {0, 0}, "The number of frames invalid FCS and integral number of octets."},
{0, "RxAllignErr",  {0, 0}, "The number of frames invalid FCS and not integral number of octets."},
{0, "RxRunt",  {0, 0}, "The number of frames received that less than 64 bytes long and have a bad FCS."},
{0, "RxFragements",  {0, 0}, "The number of frames received less than 64 bytes long and have a bad FCS."},
{0, "Rx64Byte",  {0, 0}, "The number of frames received exactly 64 bytes long including those with errors."},
{0, "Rx128Byte",  {0, 0}, "The number of frames received length between 65 and 127 bytes, including errors."},
{0, "Rx256Byte",  {0, 0}, "The number of frames received length between 128 and 255 bytes, including errors."},
{0, "Rx512Byte",  {0, 0}, "The number of frames received length between 256 and 511 bytes, including errors."},
{0, "Rx1024Byte",  {0, 0}, "The number of frames received length between 512 and 1023 bytes, including errors."},
{0, "Rx1518Byte",  {0, 0}, "The number of frames received length between 1024 and 1518 bytes, including errors."},
{0, "RxMaxByte",  {0, 0}, "The number of frames received length between 1519 and max length, including errors.(Jumbo)"},
{0, "RxTooLong",  {0, 0}, "The number of frames received length exceed max length, including errors."},
{0, "RxGoodByte",  {0, 0}, "Total data octets received in a frame with a valid FCS."},			/* slot 15, 64 bits*/
{0, "RxBadByte",  {0, 0}, "Total data octets received in all bad mode."}, /* slot 16, 64 bits*/
{0, "RxOverFlow",  {0, 0}, "Total valid frames received that are discarded due to lack of buffer space."},
{0, "Filtered",  {0, 0}, "Port disabled and unknown VID."},
{0, "TxBroad",  {0, 0}, "Total good frames tranmitted with a broadcast Destination address."},
{0, "TxPause",  {0, 0}, "Total good PAUSE frames transmitted."},
{0, "TxMulti",  {0, 0}, "Total good frames transmitted with a multicast Destination address."},
{0, "TxUnderRun",  {0, 0}, "Total valid frames discarded due to transmit FIFO buffer underflow."},
{0, "Tx64Byte",  {0, 0}, "Total frames tranmitted length of exactly 64 bytes, including errors."},
{0, "Tx128Byte",  {0, 0}, "Total frames tranmitted length between 65 and 127 bytes, including errors."},
{0, "Tx256Byte",  {0, 0}, "Total frames tranmitted length between 128 and 255 bytes, including errors."},
{0, "Tx512Byte",  {0, 0}, "Total frames tranmitted length between 256 and 511 bytes, including errors."},
{0, "Tx1024Byte",  {0, 0}, "Total frames tranmitted length between 512 and 1023 bytes, including errors."},
{0, "Tx1518Byte",  {0, 0}, "Total frames tranmitted length between 1024 and 1518 bytes, including errors."},
{0, "TxMaxByte",  {0, 0}, "Total frames tranmitted length between 1519 and max length, including errors.(Jumbo)"},
{0, "TxOverSize",  {0, 0}, "Total frames over Maxlength but transmitted truncated with bad FCS."},
{0, "TxByte",  {0, 0}, "Total data octets transmitted from counted, including with bad FCS"},	/* slot 31, 64 bits*/
{0, "TxCollision",  {0, 0}, "Total collisions experienced by a port during packet transmission."},
{0, "TxAbortCol",  {0, 0}, "Total frames not transmitted experienced 16 transmission attempts."},
{0, "TxMultiCol",  {0, 0}, "Total successfully transmitted fromes experienced more than one collision."},
{0, "TxSingleCol",  {0, 0}, "Total successfully transmitted fromes experienced exactly one collision."},
{0, "TxExcDefer",  {0, 0}, "The number of frames that defered for an excessive period of time."},
{0, "TxDefer",  {0, 0}, "Total frames delayed on first attempt because the medium way busy."},
{0, "TXLateCol",  {0, 0}, "Total times a collision detected later than 512 bit-times into the transmission."},
},
};
typedef unsigned int a_uint32_t;
typedef struct _fal_mib_info_t {
    a_uint32_t RxBroad;
    a_uint32_t RxPause;
    a_uint32_t RxMulti;
    a_uint32_t RxFcsErr;
    a_uint32_t RxAllignErr;
    a_uint32_t RxRunt;
    a_uint32_t RxFragment;
    a_uint32_t Rx64Byte;
    a_uint32_t Rx128Byte;
    a_uint32_t Rx256Byte;
    a_uint32_t Rx512Byte;
    a_uint32_t Rx1024Byte;
    a_uint32_t Rx1518Byte;
    a_uint32_t RxMaxByte;
    a_uint32_t RxTooLong;
    a_uint32_t RxGoodByte_lo; 		/**<  low 32 bits of RxGoodByte statistc item */
    a_uint32_t RxGoodByte_hi; 		/**<   high 32 bits of RxGoodByte statistc item*/
    a_uint32_t RxBadByte_lo; 		/**<   low 32 bits of RxBadByte statistc item */
    a_uint32_t RxBadByte_hi; 		/**<   high 32 bits of RxBadByte statistc item */
    a_uint32_t RxOverFlow;
    a_uint32_t Filtered;
    a_uint32_t TxBroad;
    a_uint32_t TxPause;
    a_uint32_t TxMulti;
    a_uint32_t TxUnderRun;
    a_uint32_t Tx64Byte;
    a_uint32_t Tx128Byte;
    a_uint32_t Tx256Byte;
    a_uint32_t Tx512Byte;
    a_uint32_t Tx1024Byte;
    a_uint32_t Tx1518Byte;
    a_uint32_t TxMaxByte;
    a_uint32_t TxOverSize;
    a_uint32_t TxByte_lo; 		/**<  low 32 bits of TxByte statistc item */
    a_uint32_t TxByte_hi; 		/**<  high 32 bits of TxByte statistc item */
    a_uint32_t TxCollision;
    a_uint32_t TxAbortCol;
    a_uint32_t TxMultiCol;
    a_uint32_t TxSingalCol;
    a_uint32_t TxExcDefer;
    a_uint32_t TxDefer;
    a_uint32_t TxLateCol;
}fal_mib_info_t;


void DrvgetArx8306(unsigned int portid, fal_mib_info_t *pMib)
{
		dalMibGet( portid+1, pMib );
#if 0
		memset( pMib,0, sizeof(fal_mib_info_t));
		pMib->RxGoodByte_lo += 1;
		pMib->RxBadByte_lo += 1;
		pMib->RxBadByte_lo += 1;
		pMib->TxByte_lo += 1;


#endif
#ifdef ONU_4PORT_AR8228
//		shiva_get_mib_info( 0, portid, pMib );
#endif
#ifdef ONU_4PORT_AR8306
//		garuda_get_mib_info( 0, portid, pMib);
#endif



}
#define M_ReadCounter( start, end, offset )	\
				for( ucCnt=start;ucCnt<end;ucCnt++ )	\
				{		\
					tmpEle = (long long)puiCounter[ucCnt];	\
					memcpy( (void*)&tmpRlt, (void*)&g_astCntSwhPort[ucPortId][ucCnt-offset].stAdded, 8 );	\
					tmpRlt += tmpEle;	\
					memcpy( (void*)&g_astCntSwhPort[ucPortId][ucCnt-offset].stAdded, (void*)&tmpRlt, 8 );	\
				}
#define M_ReadCounter64( cnt,offset ) \
				pDword = (unsigned int*)&tmpEle;	\
				*pDword++ = puiCounter[offset+1];	\
				*pDword	 = puiCounter[offset];	\
				memcpy( (void*)&tmpRlt, (void*)&g_astCntSwhPort[ucPortId][cnt].stAdded, 8 );	\
				tmpRlt += tmpEle;	\
				memcpy( (void*)&g_astCntSwhPort[ucPortId][cnt].stAdded, (void*)&tmpRlt, 8 );

void stats_getArx8306(int ucPortId)
{
				fal_mib_info_t  mData;
				unsigned long long tmpEle,tmpRlt;	
				unsigned int *puiCounter;
				unsigned int *pDword;
				UINT8 	ucCnt;
	
				puiCounter = (unsigned int*)&mData;
				DrvgetArx8306(ucPortId, &mData);
	

				/* drv: [0,15); Cnt: [0,15) */
				M_ReadCounter( 0, 15, 0 );
				/* drv: [19,33); Cnt: [17,31) */
				M_ReadCounter( 19, 33, 2 );
				/* drv: [35,42); Cnt: [32,39) */
				M_ReadCounter( 35, 42, 3 );
				/* drv: 15; Cnt: 15 */
				M_ReadCounter64( 15, 15 );
				/* drv: 16; Cnt: [17,18] */
				M_ReadCounter64( 16, 17 );
				/* drv: 31; Cnt: [33,34] */
				M_ReadCounter64( 31, 33 );
		
}
void stats_clearArx8306(int ucPortId)
{
		unsigned int	cnt;
		
		for( cnt=0; cnt<CNT_SW_AR8306;cnt++ )
			memset( (void*)&g_astCntSwhPort[ucPortId-1][cnt].stAdded, 0, 8 );
}	


void stats_showArx8306( int iWrFd, UINT8 ucStart, UINT8 ucNum )
{
		UINT8	ucPortId,ucCnt;	
		unsigned char buffer[100];
		


//		vosPrintf(iWrFd, "\r\n%d %d", ucStart,ucNum);
		for(ucPortId=ucStart-1; ucPortId<ucStart+ucNum-1; ucPortId++)
			stats_getArx8306(ucPortId);

		vosPrintf(iWrFd, "\r\n%-15s","Name");
		for(ucPortId=ucStart; ucPortId<ucStart+ucNum; ucPortId++)
		{
			OPL_MEMSET(buffer, 0, 100);
			sprintf(buffer,"(%1d)Value", ucPortId );
//			vosPrintf(iWrFd, "  (%1d)Accumulative", ucPortId);
			vosPrintf(iWrFd, "%-20s", buffer);
		}
		for(ucCnt=0; ucCnt<CNT_SW_AR8306; ucCnt++)
		{
			vosPrintf(iWrFd, "\r\n%-15s ", g_astCntSwhPort[0][ucCnt].pucName);
			for(ucPortId=ucStart-1; ucPortId<ucStart-1+ucNum; ucPortId++)
			{
				OPL_MEMSET(buffer, 0, 100);
				sprintf(buffer,"%llu", g_astCntSwhPort[ucPortId][ucCnt].stAdded);
				vosPrintf(iWrFd, "%-20s", buffer);
			}
		}
		vosPrintf(iWrFd, "\r\n");
}

