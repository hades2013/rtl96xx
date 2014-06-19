/*	
 * Copyright 2001-2010 Georges Menie (www.menie.org)
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the University of California, Berkeley nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <preloader.h>
#include <cpu_utils.h>
#include "swp_xmodem.h"
#include "swp_crc16.h"
#include "swp_util.h"


#define ONE_SEC         1000
#define ONE_HALF_SEC    1500
#define MAX_RETRANS     25

#if 0
SECTION_ON_FLASH static int 
get_char(u32_t msec_delay) {
    u32_t bound=msec_delay*100;
    while ((bound--)>0) {
        if (plr_tstc()) return plr_getc();
        udelay(10);
    }
    return -1;
}
#endif

SECTION_ON_FLASH static int 
get_char(u32_t msec_delay) {
    u32_t start_ms=pblr_get_timer(0);
    while ((pblr_get_timer(0)-start_ms)<msec_delay) {
        if (plr_tstc()) return plr_getc();
    }
    return -1;
}

SECTION_ON_FLASH static int 
check_crc(int check_crc, const u8_t *buf, int sz) {
	if (check_crc) {
	    // crc
		u16_t crc = crc16_ccitt_add_buffer(0, buf, sz);
		u16_t tcrc = (buf[sz]<<8)+buf[sz+1];
		return (crc == tcrc);
	} else {
	    // check-sum
		int i;
		u8_t cks = 0;
		for (i = 0; i < sz; ++i) cks += buf[i];
		return (cks == buf[sz]);
	}
}

SECTION_ON_FLASH static void 
flush_input(void) {
	while (get_char(ONE_HALF_SEC)>= 0);
}


SECTION_ON_FLASH int 
xmodem_receive(char *dest, int total_sz)
{
	//TODO: xbuff should be leveraged to dest
	u8_t xbuff[1030]; /* 1024 for XModem 1k + 3 head chars + 2 crc + nul */
	
	u8_t *p;
	int bufsz, crc = 0;
	u8_t trychar = 'C';
	u8_t packetno = 1;
	int i, c, len = 0;
	int retry, retrans = MAX_RETRANS;

	while (1) {
		for( retry = 0; retry < 16; ++retry) {
			if (trychar) plr_putc(trychar);
			if ((c = get_char((ONE_SEC)<<1)) >= 0) {
				switch (c) {
				case XDM_SOH:
					bufsz = 128;
					goto start_recv;
				case XDM_STX:
					bufsz = 1024;
					goto start_recv;
				case XDM_EOT:
					flush_input();
					plr_putc(XDM_ACK);
					return len; /* normal end */
				case XDM_CAN:
					if ((c = get_char(ONE_SEC)) == XDM_CAN) {
						flush_input();
						plr_putc(XDM_ACK);
						return XMF_CANCEL;
					}
					break;
				default:
					break;
				}
			}
		}
		if (trychar == 'C') { trychar = XDM_NAK; continue; }
		flush_input();
		plr_putc(XDM_CAN);
		plr_putc(XDM_CAN);
		plr_putc(XDM_CAN);
		return XMF_SYNC_ERROR;

	start_recv:
		if (trychar == 'C') crc = 1;
		trychar = 0;
		p = xbuff;
		*p++ = c;
		for (i = 0;  i < (bufsz+(crc?1:0)+3); ++i) {
			if ((c = get_char(ONE_SEC)) < 0) goto reject;
			*p++ = c;
		}

		if (((s8_t)xbuff[1] == (~xbuff[2])) && 
			(xbuff[1] == packetno || xbuff[1] == (u8_t)packetno-1) &&
			check_crc(crc, &xbuff[3], bufsz)) {
			if (xbuff[1] == packetno)	{
				register int count = total_sz - len;
				if (count > bufsz) count = bufsz;
				if (count > 0) {
					memcpy ((u8_t*)&dest[len], &xbuff[3], count);
					len += count;
				}
				++packetno;
				retrans = MAX_RETRANS+1;
			}
			if (--retrans <= 0) {
				flush_input();
				plr_putc(XDM_CAN);
				plr_putc(XDM_CAN);
				plr_putc(XDM_CAN);
				return XMF_RETRY;
			}
			plr_putc(XDM_ACK);
			continue;
		}
	reject:
		flush_input();
		plr_putc(XDM_NAK);
	}
}

