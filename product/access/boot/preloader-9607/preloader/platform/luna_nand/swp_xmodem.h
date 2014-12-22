#ifndef SWP_XMODEM_H
#define SWP_XMODEM_H

#define XMF_RETRY       -1
#define XMF_CANCEL      -2
#define XMF_SYNC_ERROR  -3

#define XDM_SOH         0x01
#define XDM_STX         0x02
#define XDM_EOT         0x04
#define XDM_ACK         0x06
#define XDM_NAK         0x15
#define XDM_CAN         0x18
#define XDM_CTRLZ       0x1A

int xmodem_receive(char *dest, int total_sz);
u32_t xmodem_2_flash(u32_t sf_start, u32_t esz_kb); 

#endif




