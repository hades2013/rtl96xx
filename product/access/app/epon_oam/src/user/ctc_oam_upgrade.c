#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
		
		/* EPON OAM include */
#include "epon_oam_config.h"
#include "epon_oam_err.h"
#include "epon_oam_db.h"
#include "epon_oam_dbg.h"
#include "epon_oam_rx.h"
		
#include "oam.h"
#include "h3c.h"
		
#include "ipc_client.h"
#include "oam_lw.h"
		
		
#include "lw_drv_pub.h"
#include "lw_drv_req.h"
#include "zte.h"
//#include "rtk_api.h"
#include <fcntl.h>
#include "h3c.h"
#include "mc_control.h"
#include "defs.h"
#include "system.h"
#include "stats.h"
#include "vos_thread.h"
#include "opconn_usr_ioctrl.h"
#include "vos_time.h"
#include "vos_libc.h"
#include "vos_alloc.h"
#include <signal.h>
#include "vos_config.h"
#include "mc_control.h"

extern  u8_t hostMac[6];
TFTP_FILE_t g_stImageFile;
uint8 g_upgradeOngoing;
uint8 g_waitForActive;
extern u8_t out_data[];
extern int oam_fd;
extern u16_t g_usOutDataIdx;
static u8_t g_ucLlid = 0;
#define CTC_HEADER_PROCESS(p_out) \
	do { \
		memset(out_data,0,OAM_MAXIMUM_PDU_SIZE);\
		out_data[0] = OAM_LOGIG_PON_PORT; \
		p_out = &out_data[1];\
		eopl_ctc_header_for_update(p_out); \
		g_usOutDataIdx = sizeof(oampdu_hdr_t)+sizeof(ctc_header_t)+1;\
		p_out += sizeof(oampdu_hdr_t)+sizeof(ctc_header_t);\
	} while(0); 

void eopl_ctc_header_for_update(u8_t *ptr)
{
	u8_t	*p_byte;
	ctc_header_t	*p_ctc;
	u8_t oam_dst_addr[6] = {0x1, 0x80, 0xc2, 0x0, 0x0, 0x2};
	u8_t oam_length_type[2] = {0x88, 0x09};
	u8_t ctc_oui[3] = {0x11, 0x11, 0x11};
	oampdu_hdr_t	*p_hdr;
	ctc_header_t	*p_ctc_header;
	
	p_hdr = (oampdu_hdr_t*)ptr;
	vosMemCpy(p_hdr->dest, oam_dst_addr,6);
	vosMemCpy(p_hdr->src, hostMac, 6);
	vosMemCpy(p_hdr->length, oam_length_type, 2);
	p_hdr->subtype = 0x03;
	p_hdr->pcode = 0xfe;
	p_hdr->flags = 0x0050;

	ptr += sizeof(oampdu_hdr_t);
	p_ctc_header = (ctc_header_t*)ptr;
	memcpy(p_ctc_header->oui, ctc_oui, 3);
	p_ctc_header->ext_code = 0x06;
}


OPL_STATUS eopl_ctc_tftp_file_write_req_handler(u8_t *pucFileName, u8_t ucNameLen)
{
	u8_t *p_out;
	SOFT_DOWNLOAD_HD_MASK_t *pstDownHd;
	TFTP_DATA_t *pstDataCur;
	TFTP_DATA_t *pstDataNext;

	if(g_upgradeOngoing)
	{
		pstDataCur = g_stImageFile.pstData;
		while(pstDataCur != NULL)
		{
			pstDataNext = pstDataCur->pstNext;
			vosFree(pstDataCur);
			pstDataCur = pstDataNext;
		}
	}
	//OP_DEBUG(DEBUG_LEVEL_INFO, "Start to receive file: %s\n", pucFileName);
	g_upgradeOngoing = 1;

	g_stImageFile.ucNameLen = ucNameLen;
	g_stImageFile.ucFlash = 0;
	g_stImageFile.usBlock1 = 0;
	g_stImageFile.uiFileSize = 0;
	g_stImageFile.pstData = 0;

	memcpy(g_stImageFile.aucFileName, pucFileName, ucNameLen);
	
	CTC_HEADER_PROCESS(p_out);
	
	pstDownHd = (SOFT_DOWNLOAD_HD_MASK_t *)p_out;
	pstDownHd->ucType = OAM_SOFT_TYPE_TFTP;
	pstDownHd->usLen = 0x09;
//	dalPonLlidGet(&uiLlid);/* zlu modify */
	pstDownHd->usTid = g_ucLlid;/* zlu modify */
	*(u16_t *)(pstDownHd->aucMsg) = TFTP_ACK;
	*(u16_t *)&(pstDownHd->aucMsg[2]) = 0x0000;
	
	g_usOutDataIdx = OAM_MINIMIZE_PDU_SIZE;

	eopl_host_send(out_data, g_usOutDataIdx);
	g_usOutDataIdx = 0;
	return 0;
}

OPL_STATUS eopl_ctc_gen_file(void)
{
	FILE *pFile;
	TFTP_DATA_t *pstData;

	pFile = fopen((char *)(g_stImageFile.aucFileName), "wb");

	pstData = g_stImageFile.pstData;
	while(pstData)
	{
		fwrite(pstData->aucDataBlock, 1, pstData->usLen, pFile);
		pstData = pstData->pstNext;
	}

	fclose(pFile);
	chmod((char *)g_stImageFile.aucFileName, S_IXUSR|S_IXGRP|S_IXOTH);

	return 0;
}

OPL_STATUS eopl_ctc_write_flash(void)
{
	OPL_STATUS iStatus=0;

	g_stImageFile.ucFlash = 1;

	return iStatus;
}

OPL_STATUS eopl_ctc_tftp_data_handler(u8_t *pucData, u16_t usDatalen)
{
	u8_t *pucIn;
	TFTP_DATA_t *pstData;
	SOFT_DOWNLOAD_HD_MASK_t *pstDownHd;
	u8_t *p_out;
	u16_t usBlockLen;
	u16_t usBlockNum;

	pucIn = pucData;
	usBlockNum = *((u16_t *)pucIn);
	pucIn += sizeof(u16_t);

	usBlockLen = usDatalen - sizeof(u16_t);
	if (( usBlockLen >0 && usBlockLen <= 1400))
	{
		pstData = (TFTP_DATA_t *)vosAlloc(sizeof(TFTP_DATA_t));
		memcpy(pstData->aucDataBlock, pucIn, usBlockLen);
		pstData->usLen = usBlockLen;
		pstData->pstNext = 0;
	}
	else
	{
		/* OAMDBG(("Receive invalid tftp file transfer data!\n"));*/
		OP_DEBUG(DEBUG_LEVEL_INFO, "Receive invalid tftp file transfer data, len is invalid!... \n");
		return 0;
	}

	if(g_stImageFile.usBlock1+1 == usBlockNum)
	{
		/* OAMDBG(("file data block id(%d) mactch, block length: %d.\n", usBlockNum, usBlockLen));*/

		g_stImageFile.usBlock1 = usBlockNum;

		if (0 == g_stImageFile.pstData)
		{
			g_stImageFile.pstData = pstData;
			g_stImageFile.pstDataEnd = pstData;
			g_stImageFile.uiFileSize = (u32_t)usBlockLen;
		}
		else
		{
			g_stImageFile.pstDataEnd->pstNext = pstData;
			g_stImageFile.pstDataEnd = pstData;
			g_stImageFile.uiFileSize += (u32_t)usBlockLen;
		}

		OP_DEBUG(DEBUG_LEVEL_INFO, "Receive tftp file transfer data, serial no is %d!... \n", usBlockNum);
	}
	else
	{
		OP_DEBUG(DEBUG_LEVEL_INFO, "Receive invalid tftp file transfer data, no is invalid!... \n");
	}

	CTC_HEADER_PROCESS(p_out);

	pstDownHd = (SOFT_DOWNLOAD_HD_MASK_t *)p_out;
	pstDownHd->ucType = OAM_SOFT_TYPE_TFTP;
	pstDownHd->usLen = 0x09;
//	dalPonLlidGet(&uiLlid);/* zlu modify */
	pstDownHd->usTid = g_ucLlid;/* zlu modify */
	*(u16_t *)(pstDownHd->aucMsg) = TFTP_ACK;
	*(u16_t *)&(pstDownHd->aucMsg[2]) = g_stImageFile.usBlock1;

	g_usOutDataIdx = OAM_MINIMIZE_PDU_SIZE;
	
	eopl_host_send(out_data, g_usOutDataIdx);
	g_usOutDataIdx = 0;

	if(usBlockLen < 1400)
	{
		/* OAMDBG(("file transfer finish, file size = %d\n", g_stImageFile.uiFileSize));*/
		OP_DEBUG(DEBUG_LEVEL_INFO, "Finish receiving file: %s (%d Bytes)\n", g_stImageFile.aucFileName, g_stImageFile.uiFileSize);
	}

	return 0;
}

void eopl_ctc_send_end_download_response(u8_t ucRpsCode)
{
	ctc_tftp_header_t *pstTftpHdr;
	u8_t *p_out;
	OP_DEBUG(DEBUG_LEVEL_INFO, "Send End Download Response PDU, RpsCode = %d!\n", ucRpsCode);
	CTC_HEADER_PROCESS(p_out);

	pstTftpHdr = (ctc_tftp_header_t *)p_out;

	pstTftpHdr->data_type = OAM_SOFT_TYPE_CRC;
	pstTftpHdr->len = sizeof(ctc_tftp_header_t) + sizeof(u16_t) + sizeof(u8_t);
//	dalPonLlidGet(&uiLlid);/* zlu modify */
	pstTftpHdr->tid = g_ucLlid;/* zlu modify */

	p_out += sizeof(ctc_tftp_header_t);
	g_usOutDataIdx += sizeof(ctc_tftp_header_t);

	*((u16_t *)p_out) = TFTP_END_DOWNLOAD_RSP;
	p_out += sizeof(u16_t);/* zlu add */
	g_usOutDataIdx += sizeof(u16_t);

	*((u8_t *)p_out) = ucRpsCode;
	g_usOutDataIdx += sizeof(u8_t);

	if(OAM_MINIMIZE_PDU_SIZE > g_usOutDataIdx)
	{
		g_usOutDataIdx = OAM_MINIMIZE_PDU_SIZE;
	}

	eopl_host_send(out_data, g_usOutDataIdx);
	g_usOutDataIdx = 0;
}

int BootFlagGet(u8_t *upgradeflag)
{
    if(NULL == upgradeflag)
    {
        OP_DEBUG(DEBUG_LEVEL_INFO,"upgradeflag para error!", UFILE_BOOTFLAG);
        return UP_FAILED;
    }
    *upgradeflag = UPGRADE_APP_0;
    #ifdef CONFIG_BOOT_MULTI_APP
	int fp;
	int  ucount = 1;
	char bootflag[4];
	
	if((fp = open(UFILE_BOOTFLAG, O_RDONLY)) < 0)
	{
		OP_DEBUG(DEBUG_LEVEL_INFO,"File [%s] open failed!", UFILE_BOOTFLAG);
		return UP_ERR_IO;
	}
    if(2 != read(fp, bootflag, 2))
    {
    	OP_DEBUG(DEBUG_LEVEL_INFO,"File [%s] read failed!", UFILE_BOOTFLAG);
		return UP_ERR_IO;
    }
    if(0 != close(fp))
    {
    	OP_DEBUG(DEBUG_LEVEL_INFO,"File [%s] close failed!", UFILE_BOOTFLAG);
		return UP_ERR_IO;
    }
    
    if('1' == bootflag[0])
    {
    	*upgradeflag = UPGRADE_APP_0;
    }
    else
    {
    	*upgradeflag = UPGRADE_APP_1;
    }
    #endif
    
    return UP_OK;
}

/*
  check image file is invalid

  imagefile: image file to write

  return:
  0: done
  -1: fail
*/
int check_image(const char *imagefile)
{
	image_header_t headinfo;
	int fd;
	UINT32 chipid;
    //char *pData=NULL;
    char *pFileData=NULL;
	uint32 crc, checksum;
	int bytes_read;
	char *cp;

	struct stat buf;
	if (stat(imagefile, &buf) < 0){
        fprintf(stderr, "Image check: image file size error!\r\n");
		return 0;
	}

    //oplRegRead(REG_GB_SOFTRST_ADN_DEVID, &chipid);

	fd = open(imagefile, O_RDONLY);
	if (fd < 0)
	{
		fprintf(stderr, "Image check: can't open image file!\r\n");
		return -1;
	}
    
    pFileData = (char *)malloc(buf.st_size);
    if(NULL == pFileData)
    {
		fprintf(stderr, "Image check: can't malloc mem!\r\n");
		return -1;        
    }
    
	if (vosSafeRead(fd, pFileData, buf.st_size) < 0)
	{
		close(fd);
		fprintf(stderr, "Image check: can't read image file!\r\n");
        vosFree(pFileData);
		return -1;
	}
	#if 1
	if(UP_OK != fw_head_check(pFileData, buf.st_size))
    {
		close(fd);
		fprintf(stderr, "Image check: image file invalid!\r\n");
        vosFree(pFileData);
		return -1;
    }
	#else
    if(UP_OK != ufile_validate(pFileData, buf.st_size))
    {
		close(fd);
		fprintf(stderr, "Image check: image file invalid!\r\n");
        vosFree(pFileData);
		return -1;
    }
    #endif
    vosFree(pFileData);
	close(fd);
	return 0;
}

OPL_STATUS eopl_ctc_end_download_req_handler(u32_t ulFileSize)
{
	u8_t ucRpsCode;
	FILE *pFile;
	TFTP_DATA_t *pstData;
    u8_t bootFlag;
    ipc_system_ack_t *pack = NULL;
	struct timeval tv;
	struct tm *tm;
    char appPath[64] = {0};
    char cmd[64] = {0};

	/* OAMDBG(("file size: %d\n", ulFileSize));*/
	OP_DEBUG(DEBUG_LEVEL_INFO, "Receive End Download Request PDU!\n");

	/* check if wait for active request */
	if(g_waitForActive)
	{
		OP_DEBUG(DEBUG_LEVEL_INFO, "Send end download response, write success... \n");
		ucRpsCode = OAM_FIRMWARE_WRITE_SUCCESS;
		eopl_ctc_send_end_download_response(ucRpsCode);
		vosThreadExit(0);
	}

	/* check if flash writing in progress */
	if(g_stImageFile.ucFlash)
	{
		OP_DEBUG(DEBUG_LEVEL_INFO, "Send end download response, file is writing... \n");
		ucRpsCode = OAM_FIRMWARE_WRITING;
		eopl_ctc_send_end_download_response(ucRpsCode);
		vosThreadExit(0);
	}

	/* check file len */
	#if 0
	if(ulFileSize != g_stImageFile.uiFileSize)
	{
		ucRpsCode = OAM_FIRMWARE_VERIFY_ERROR;
		printf("\OAM_FIRMWARE_VERIFY_ERROR!\n");
		goto send_rsp_msg;
	}
	#endif
	/* check crc */

	/* start writing firmware to flash */
	#if 1
    gettimeofday(&tv, NULL);
    tm = localtime(&tv.tv_sec);
    OP_DEBUG(DEBUG_LEVEL_INFO,"Start writing Flash:%04d.%02d.%02d %02d:%02d:%02d\n", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);

	#endif
	/* write firmware to flash */
    sprintf(appPath, "/tmp/%s", APP_UPGRADE_FILE);
    sprintf(cmd,"rm -rf %s",appPath);
	vosSystem(cmd);
	pFile = fopen(appPath, "wb");

	if(pFile != NULL)
	{
		pstData = g_stImageFile.pstData;
		while(pstData)
		{
			fwrite(pstData->aucDataBlock, 1, pstData->usLen, pFile);
			pstData = pstData->pstNext;
		}
		fclose(pFile);
	}

	/* indicate flash writing finish */

	if (check_image(appPath) != OK)
	{
	//	OP_DEBUG(DEBUG_LEVEL_INFO, "Send end download response, verify error... \n");
		ucRpsCode = OAM_FIRMWARE_VERIFY_ERROR;
		eopl_ctc_send_end_download_response(ucRpsCode);

		vosThreadExit(0);
	}

	/* indicate flash writing in progress */
	g_stImageFile.ucFlash = 1;

	/* send end download response with rpsCode = OAM_FIRMWARE_WRITING */
	OP_DEBUG(DEBUG_LEVEL_INFO, "Send end download response, start writing file... \n");
	ucRpsCode = OAM_FIRMWARE_WRITING;
	eopl_ctc_send_end_download_response(ucRpsCode);
    (void)BootFlagGet(&bootFlag);
    pack = ipc_system_req(oam_fd, IPC_SYS_UPGRADE_APP, bootFlag);
	if(pack && (pack->hdr.status != IPC_STATUS_OK)){
		free(pack);
        OP_DEBUG(DEBUG_LEVEL_INFO, "do IPC_SYS_UPGRADE_APP error! \n");
        vosThreadExit(0);
	}
    if (pack) 
    {
        free(pack);
    }
    #if 0
    bootFlag = vosConfigBootFlagGet(FLASH_BOOT_FLAG_NORMAL);
    if (FLASH_BOOT_OS2_FLAG == bootFlag)
    {
		OP_DEBUG(DEBUG_LEVEL_INFO, "Writing image to FLASH_DEV_NAME_OS1!\n");
        write_flash("/tmp/opconn.img.bak.oam", FLASH_DEV_NAME_OS1);
		
        vosSystem("rm -f /tmp/opconn.img.bak.oam ");
    }
    else {
		OP_DEBUG(DEBUG_LEVEL_INFO, "Writing image to FLASH_DEV_NAME_OS2!\n");
        write_flash("/tmp/opconn.img.bak.oam", FLASH_DEV_NAME_OS2);
		
        vosSystem("rm -f /tmp/opconn.img.bak.oam ");
    }
    #endif
	/* indicate flash writing finish */
	g_stImageFile.ucFlash = 0;
	#if 1
    gettimeofday(&tv, NULL);
    tm = localtime(&tv.tv_sec);
    OP_DEBUG(DEBUG_LEVEL_INFO, "Finish writing flash:%04d.%02d.%02d %02d:%02d:%02d\n", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);

	#endif
	ucRpsCode = OAM_FIRMWARE_WRITE_SUCCESS;

	g_waitForActive = 1;

	vosThreadExit(0);

	return 0;
}

OPL_STATUS eopl_ctc_tftp_handler(u8_t *pucData, u16_t msg_len)
{
	u8_t *pucIn;
	u16_t opcode;
	u16_t usDataLen;

	pucIn = pucData;

	opcode = *((u16_t *)pucIn);
	pucIn += sizeof(u16_t);

	switch(opcode)
	{
		case TFTP_R_REQ:
			break;
		case TFTP_W_REQ:
			eopl_ctc_tftp_file_write_req_handler(pucIn, strlen(pucIn));
			break;
		case TFTP_DATA:
			usDataLen = msg_len - sizeof(u16_t);
			eopl_ctc_tftp_data_handler(pucIn, usDataLen);
			break;
		case TFTP_ACK:
			break;
		case TFTP_ERR:
			break;
		default:
//			OAMDBG(("\nno such tftp cmd\n"));
			break;
	}

	return 0;
}

OPL_STATUS eopl_ctc_crc_handler(u8_t *pucData)
{
	u8_t *pucIn;
	u16_t usOpcode;
	u32_t ulFileSize;

	pucIn = pucData;
	usOpcode = *((u16_t *)pucIn);
	pucIn += sizeof(u16_t);

	switch(usOpcode)
	{
		case TFTP_END_DOWNLOAD_REQ:
			ulFileSize = *((u32_t *)pucIn);
			/*
			eopl_ctc_end_download_req_handler(ulFileSize);
			*/
#if 1
			vosThreadCreate("tUpgraded", OP_VOS_THREAD_STKSZ, 45,
					(void *)eopl_ctc_end_download_req_handler, (void *)ulFileSize);


#endif
			break;
		case TFTP_END_DOWNLOAD_RSP:
			break;
		default:
//			OAMDBG(("\nunknow opcode %d\n", usOpcode));
			break;
	}

	return 0;
}

OPL_STATUS eopl_ctc_load_new_img(void)
{
	OPL_STATUS	iStatus=OPL_OK;
	u8_t *p_out;
	ctc_tftp_header_t *pusTftpHdr;
	u16_t usPktLen;
	u8_t ucAck;
	FILE *pFile;
	u8_t ucBootOption;
	u8_t bootFlag;

	g_waitForActive = 0;
	/* set boot option */
	ucBootOption = 1;
	OP_DEBUG(DEBUG_LEVEL_INFO, "Receive Active Image PDU!\n");

    //bootFlag = vosConfigBootFlagGet(FLASH_BOOT_FLAG_NORMAL);
    if (FLASH_BOOT_OS2_FLAG == bootFlag)
    {
		//OP_DEBUG(DEBUG_LEVEL_INFO, "Set FLASH_BOOT_FLAG_NORMAL option to FLASH_BOOT_OS1_FLAG!\n");
		//vosConfigBootFlagSet(FLASH_BOOT_FLAG_NORMAL, FLASH_BOOT_OS1_FLAG);
    }
    else {
		//OP_DEBUG(DEBUG_LEVEL_INFO, "Set FLASH_BOOT_FLAG_NORMAL option to FLASH_BOOT_OS2_FLAG!\n");
		//vosConfigBootFlagSet(FLASH_BOOT_FLAG_NORMAL, FLASH_BOOT_OS2_FLAG);
    }
	
    /* for FLASH_BOOT_FLAG_UPGRADE, we use FLASH_BOOT_OS2_FLAG to indicate
       that the os upgrade by OAM
    */
	//OP_DEBUG(DEBUG_LEVEL_INFO, "Set FLASH_BOOT_FLAG_UPGRADE to FLASH_BOOT_OS2_FLAG!\n");
    //vosConfigBootFlagSet(FLASH_BOOT_FLAG_UPGRADE, FLASH_BOOT_OS2_FLAG);

	ucAck = 0;

	CTC_HEADER_PROCESS(p_out);

	pusTftpHdr = (ctc_tftp_header_t *)p_out;

	pusTftpHdr->data_type = OAM_SOFT_TYPE_LOAD;
	pusTftpHdr->len = sizeof(ctc_tftp_header_t) + sizeof(u16_t) + sizeof(u8_t);

	//dalPonLlidGet(&uiLlid);/* zlu modify */
	pusTftpHdr->tid = g_ucLlid;/* zlu modify */

	p_out += sizeof(ctc_tftp_header_t);
	g_usOutDataIdx += sizeof(ctc_tftp_header_t);

	*((u16_t *)(p_out)) = TFTP_ACT_IMG_RSP;
	p_out += sizeof(u16_t);
	g_usOutDataIdx += sizeof(u16_t);

	*((u8_t *)(p_out)) = ucAck;
	p_out += sizeof(u8_t);
	g_usOutDataIdx += sizeof(u8_t);

 	if(g_usOutDataIdx < OAM_MINIMIZE_PDU_SIZE)
 	{
		g_usOutDataIdx = OAM_MINIMIZE_PDU_SIZE;
 	}

	eopl_host_send(out_data, g_usOutDataIdx);
	g_usOutDataIdx = 0;

	return iStatus;
}

OPL_STATUS eopl_ctc_image_mark(void)
{
	u8_t					*p_out;
	OPL_STATUS				iStatus=OPL_OK;
	SOFT_DOWNLOAD_HD_MASK_t	*pstDownHd;

	CTC_HEADER_PROCESS(p_out);
	pstDownHd = (SOFT_DOWNLOAD_HD_MASK_t *)p_out;
	pstDownHd->ucType = OAM_SOFT_TYPE_LOAD;
	pstDownHd->usLen = 0x08;
//	dalPonLlidGet(&uiLlid);/* zlu modify */
	pstDownHd->usTid = g_ucLlid;/* zlu modify */
	*(u16_t *)(pstDownHd->aucMsg) = TFTP_ACT_IMG_RSP;
	pstDownHd->aucMsg[2] = 0;

	
 	if(g_usOutDataIdx < OAM_MINIMIZE_PDU_SIZE)
 	{
		g_usOutDataIdx = OAM_MINIMIZE_PDU_SIZE;
 	}
	eopl_host_send(out_data, g_usOutDataIdx);
	g_usOutDataIdx = 0;


	return iStatus;
}

OPL_STATUS eopl_ctc_image_reboot(void)
{
	u8_t					*p_out;
	OPL_STATUS				iStatus=OPL_OK;
	SOFT_DOWNLOAD_HD_MASK_t	*pstDownHd;

	CTC_HEADER_PROCESS(p_out);
	pstDownHd = (SOFT_DOWNLOAD_HD_MASK_t *)p_out;
	pstDownHd->ucType = OAM_SOFT_TYPE_LOAD;
	pstDownHd->usLen = 0x08;
//	dalPonLlidGet(&uiLlid);/* zlu modify */
	pstDownHd->usTid = g_ucLlid;/* zlu modify */
	*(u16_t *)(pstDownHd->aucMsg) = TFTP_ACT_IMG_RSP;
	pstDownHd->aucMsg[2] = 0;

	
 	if(g_usOutDataIdx < OAM_MINIMIZE_PDU_SIZE)
 	{
		g_usOutDataIdx = OAM_MINIMIZE_PDU_SIZE;
 	}
	eopl_host_send(out_data, OAM_MINIMIZE_PDU_SIZE);
	g_usOutDataIdx = 0;
	return iStatus;
}

OPL_STATUS eopl_ctc_active_req_handler(u8_t *pucData)
{
	u8_t *pucIn;
	u8_t ucFlag;

	pucIn = pucData;

	ucFlag = *((u8_t *)pucIn);

	OPL_STATUS	iStatus=OPL_OK;
	switch(ucFlag)
	{
		case LOAD_NEW_IMG:
			eopl_ctc_load_new_img();
			break;

		default:
//			OAMDBG(("unknown active imgage req flag = %d\n", ucFlag));
			break;
	}

	return iStatus;
}

OPL_STATUS eopl_ctc_commit_req_handler(u8_t *pucData)
{
	OPL_STATUS	iStatus=OPL_OK;
	u8_t *pucIn;
	u8_t ucFlag;

	pucIn = pucData;

	ucFlag = *((u8_t *)pucIn);

	switch(ucFlag)
	{
		case COMMIT_NEW_IMG:
			eopl_ctc_commit_new_img();
			break;

		default:
//			OAMDBG(("unknown commit imgage req flag = %d\n", ucFlag));
			break;
	}

	return iStatus;
}


OPL_STATUS eopl_ctc_commit_new_img(void)
{
	OPL_STATUS	iStatus=OPL_OK;
	u8_t *p_out;
	ctc_tftp_header_t *pusTftpHdr;
	u8_t ucAck, bootFlag;

	/* commit new image as primary bootable image*/

    //bootFlag = vosConfigBootFlagGet(FLASH_BOOT_FLAG_NORMAL);
    if (FLASH_BOOT_OS2_FLAG == bootFlag)
    {
		//OP_DEBUG(DEBUG_LEVEL_INFO, "Set FLASH_BOOT_FLAG_NORMAL option to FLASH_BOOT_OS1_FLAG!\n");
		//vosConfigBootFlagSet(FLASH_BOOT_FLAG_NORMAL, FLASH_BOOT_OS1_FLAG);
    }
    else {
		//OP_DEBUG(DEBUG_LEVEL_INFO, "Set FLASH_BOOT_FLAG_NORMAL option to FLASH_BOOT_OS2_FLAG!\n");
		//vosConfigBootFlagSet(FLASH_BOOT_FLAG_NORMAL, FLASH_BOOT_OS2_FLAG);
    }

	ucAck = 0;

send_rsp_msg:
	CTC_HEADER_PROCESS(p_out);


	pusTftpHdr = (ctc_tftp_header_t *)p_out;

	pusTftpHdr->data_type = OAM_SOFT_TYPE_COMMIT;
	pusTftpHdr->len = sizeof(ctc_tftp_header_t) + sizeof(u16_t) + sizeof(u8_t);
//	dalPonLlidGet(&uiLlid);/* zlu modify */
	pusTftpHdr->tid = g_ucLlid;/* zlu modify */
	p_out += sizeof(ctc_tftp_header_t);
	g_usOutDataIdx+= sizeof(ctc_tftp_header_t);

	*((u16_t *)(p_out)) = TFTP_COMMIT_IMG_RSP;
	p_out += sizeof(u16_t);
	g_usOutDataIdx += sizeof(u16_t);

	*((u8_t *)(p_out)) = ucAck;
	p_out += sizeof(u8_t);
	g_usOutDataIdx += sizeof(u8_t);

 	if(g_usOutDataIdx < OAM_MINIMIZE_PDU_SIZE)
 	{
		g_usOutDataIdx = OAM_MINIMIZE_PDU_SIZE;
 	}
	
	eopl_host_send(out_data, OAM_MINIMIZE_PDU_SIZE);
	g_usOutDataIdx = 0;	

	return iStatus;
}

OPL_STATUS eopl_ctc_load_handler(u8_t *pucData)
{
	u8_t *pucIn;
	u16_t usOpcode;

	pucIn = pucData;

	usOpcode = *((u16_t *)pucIn);
	pucIn += sizeof(u16_t);

	switch(usOpcode)
	{
		case TFTP_ACT_IMG_REQ:
			eopl_ctc_active_req_handler(pucIn);
			break;
		case TFTP_ACT_IMG_RSP:
			break;

		case TFTP_COMMIT_IMG_REQ:
			eopl_ctc_commit_req_handler(pucIn);
			break;

		case TFTP_COMMIT_IMG_RSP:
			break;

		default:
//			OAMDBG(("\nno such tftp cmd\n"));
			break;
	}

	return 0;
}

OPL_STATUS eopl_ctc_commit_handler(u8_t *pucData)
{
	u8_t *pucIn;
	u16_t usOpcode;

	pucIn = pucData;

	usOpcode = *((u16_t *)pucIn);
	pucIn += sizeof(u16_t);

	switch(usOpcode)
	{
		case TFTP_COMMIT_IMG_REQ:
			eopl_ctc_commit_req_handler(pucIn);
			break;
	}

	return 0;
}

OPL_STATUS eopl_ctc_soft_download(
    unsigned char llidIdx,      /* LLID index of the incoming operation */
    unsigned char *pFrame,      /* Frame payload current pointer */
    unsigned short length,      /* Frame payload length */
    unsigned char *pReplyBuf,   /* Frame buffer for reply OAM */
    unsigned short bufLen,      /* Frame buffer size */
    unsigned short *pGenLen)
{
	u8_t *pucIn;
	ctc_tftp_header_t *p_tftp_header;
	u16_t usMsgLen;

	pucIn = pFrame;
	*pGenLen = 0;
	p_tftp_header = (ctc_tftp_header_t *)pucIn;
	pucIn += sizeof(ctc_tftp_header_t);
	g_ucLlid = llidIdx;
	usMsgLen = (p_tftp_header->len) - sizeof(ctc_tftp_header_t);

	switch(p_tftp_header->data_type)
	{
		case OAM_SOFT_TYPE_TFTP:
			eopl_ctc_tftp_handler(pucIn, usMsgLen);
			break;
		case OAM_SOFT_TYPE_CRC:
			eopl_ctc_crc_handler(pucIn);
			break;
		case OAM_SOFT_TYPE_LOAD:
			eopl_ctc_load_handler(pucIn);
			break;
		case OAM_SOFT_TYPE_COMMIT:
			eopl_ctc_commit_handler(pucIn);
			break;
		default:
			return 0;
	}

	return 0;
}



