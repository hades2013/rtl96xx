#ifndef __CTC_OAM_UPGRADE_H__
#define __CTC_OAM_UPGRADE_H__
#define CTC_OAM_UPGRADE_ENABLE 1
extern int eopl_ctc_soft_download(
    unsigned char llidIdx,      /* LLID index of the incoming operation */
    unsigned char *pFrame,      /* Frame payload current pointer */
    unsigned short length,      /* Frame payload length */
    unsigned char *pReplyBuf,   /* Frame buffer for reply OAM */
    unsigned short bufLen,      /* Frame buffer size */
    unsigned short *pReplyLen);
#endif

