#ifndef SWP_YMODEM_H
#define SWP_YMODEM_H

#define xyzModem_xmodem     1
#define xyzModem_ymodem     2
/* Don't define this until the protocol support is in place */
/*#define xyzModem_zmodem 3 */

#define xyzModem_access     -1
#define xyzModem_noZmodem   -2
#define xyzModem_timeout    -3
#define xyzModem_eof        -4
#define xyzModem_cancel     -5
#define xyzModem_frame      -6
#define xyzModem_cksum      -7
#define xyzModem_sequence   -8

#define xyzModem_close      1
#define xyzModem_abort      2

#define CYGACC_CALL_IF_DELAY_US(x) udelay(x)

typedef struct {
    char *filename;
    int   mode;
    int   chan;
} connection_info_t;

#ifndef	BOOL_WAS_DEFINED
#define BOOL_WAS_DEFINED
typedef unsigned int bool;
#endif

#define false 0
#define true 1


int   xyzModem_stream_open(connection_info_t *info, int *err);
void  xyzModem_stream_close(int *err);
void  xyzModem_stream_terminate(bool method, int (*getc)(void));
int   xyzModem_stream_read(char *buf, int size, int *err);
char *xyzModem_error(int err);


#endif //SWP_YMODEM_H
