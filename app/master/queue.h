/*
 * queue.h
 *
 *  Created on: Dec 29, 2010
 *      Author: root
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include "master.h"

#define	DYNAMIC_QUEUE_REQ_BUFFER	1

#define	MAX_QUEUE_SIZE	256

#define	MME_QUEUE_EMPTY		0
#define	MME_QUEUE_REQSENT	1
#define	MME_QUEUE_ACKRECV	2

#define	MME_QUEUE_FULL	-1

#define	MME_STATUS_OK			0
#define	MME_STATUS_TIMEOUT		-1

#define	MME_DEFAULT_RESEND		3	// re-send every 3 second

typedef void (*rawpacket_timeoutfunc_t)(void *parm);
typedef void (*mme_callback_func_t)(void *parm);
typedef int (*rawpacket_sendfunc_t)(int sock, ethernet_addr_t dst, uint16_t mmtype);
typedef int (*rawpacket_recvfunc_t)(uint8_t *buffer, int size, void *parm);

typedef struct {
	// Register variables
	void *mme_parm;

	ethernet_addr_t dst;
	uint16_t mmtype;
#ifdef DYNAMIC_QUEUE_REQ_BUFFER
	uint8_t *req_buffer;
#else
	uint8_t *req_buffer[1600];
#endif
	int req_size;

	time_t time_issue;
	time_t time_send;
	int max_retry;
	rawpacket_recvfunc_t recvfunc;
	rawpacket_timeoutfunc_t tmofunc;

	// Running status variables
	int status;
	int retry_count;
}
eoc_mme_queue_t;

int mme_request(uint8_t *buffer, int size, int max_retry,
		rawpacket_recvfunc_t recv,
		void *parm);

#endif /* QUEUE_H_ */
