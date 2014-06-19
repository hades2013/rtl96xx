/*
 *	matrixsslApi.c
 *  Release $Name: MATRIXSSL-3-1-3-OPEN $
 * 
 *	MatrixSSL Public API Layer
 */
/*
 *	Copyright (c) PeerSec Networks, 2002-2010. All Rights Reserved.
 *	The latest version of this code is available at http://www.matrixssl.org
 *
 *	This software is open source; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This General Public License does NOT permit incorporating this software 
 *	into proprietary programs.  If you are unable to comply with the GPL, a 
 *	commercial license for this software may be purchased from PeerSec Networks
 *	at http://www.peersec.com
 *	
 *	This program is distributed in WITHOUT ANY WARRANTY; without even the 
 *	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 *	See the GNU General Public License for more details.
 *	
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *	http://www.gnu.org/copyleft/gpl.html
 */
/******************************************************************************/

#include "matrixsslApi.h"

/******************************************************************************/
/*
	Create a new client SSL session
	This creates internal SSL buffers and cipher structures
	Clients initiate the connection with a 'HelloRequest', and this data
		is placed in the outgoing buffer for the caller to send.
 
	sid		A pointer to storage for a session ID. If there is not yet session
			cache information, the sid.cipherId should be set to 
			SSL_NULL_WITH_NULL_NULL. After a successful connection to a server,
			this sid structure will be populated with session cache credentials
			and for subsequent connections should be used without modification 
			of the cipherId.

	keys	Keys structure initialized with matrixSslReadKeys
 
	certCb	
 
	extensions	Optional TLS extensions (usually NULL)
 
	ssl		The ssl_t session structure is returned using this value, on success
 
	Return	MATRIXSSL_REQUEST_SEND on success
			< 0 on error
			
	Do not need to call DeleteSession on failure
 */
#ifdef USE_CLIENT_SIDE_SSL
int32 matrixSslNewClientSession(ssl_t **ssl, sslKeys_t *keys,
		sslSessionId_t *sid, uint32 cipherSpec,
		int32 (*certCb)(ssl_t *ssl, psX509Cert_t *cert, int32 alert),
		tlsExtension_t *extensions, int32 (*extCb)(ssl_t *ssl,
		unsigned short extType, unsigned short extLen, void *e))
{
	ssl_t		*lssl;
	psBuf_t		tmp;
	uint32		len;
	int32		rc;

	if (!ssl) {
		return PS_ARG_FAIL;
	}
	
	*ssl = NULL;
	lssl = NULL;

/*
	Give priority to cipher suite if session id is provided and doesn't match
*/
	if (cipherSpec != 0 && sid != NULL && sid->cipherId != 0) {
		if (cipherSpec != sid->cipherId) {
			psTraceInfo("Explicit cipher suite will override session cache\n");
			memset(sid->id, 0, SSL_MAX_SESSION_ID_SIZE);
			memset(sid->masterSecret, 0, SSL_HS_MASTER_SIZE);
			sid->cipherId = 0;
		}
	}
	
	if ((rc = matrixSslNewSession(&lssl, keys, sid, 0)) < 0) {
		return rc;
	}

	if (certCb) {
		matrixSslSetCertValidator(lssl, (sslCertCb_t)certCb);
	}
	
	if (extCb) {
		lssl->extCb = (sslExtCb_t)extCb;
	}

RETRY_HELLO:
	tmp.size = lssl->outsize;
	tmp.buf = tmp.start = tmp.end = lssl->outbuf;
	if ((rc = matrixSslEncodeClientHello(lssl, &tmp, cipherSpec, &len,
			extensions)) < 0) {
		if (rc == SSL_FULL) {
			lssl->outsize = len;
			lssl->outbuf = psRealloc(lssl->outbuf, lssl->outsize);
			goto RETRY_HELLO;
		} else {
			matrixSslDeleteSession(lssl);
			return rc;
		}
	}
	psAssert(tmp.start == tmp.buf);
	lssl->outlen = tmp.end - tmp.start;
	
	*ssl = lssl;
	return MATRIXSSL_REQUEST_SEND;
}
#endif /* USE_CLIENT_SIDE_SSL */

#ifdef USE_SERVER_SIDE_SSL
/******************************************************************************/
/*
	Create a new server SSL session
	This creates internal SSL buffers and cipher structures
	Internal SSL state is set to expect an incoming 'HelloRequest'

	Return	MATRIXSSL_SUCCESS on success
			< 0 on error
 */
int32 matrixSslNewServerSession(ssl_t **ssl, sslKeys_t *keys,
			int32 (*certCb)(ssl_t *ssl, psX509Cert_t *cert, int32 alert))
{
	ssl_t		*lssl;
	
	if (!ssl) {
		return PS_ARG_FAIL;
	}
	
	*ssl = NULL;
	lssl = NULL;
	
	psAssert(certCb == NULL);
	if (matrixSslNewSession(&lssl, keys, NULL, SSL_FLAGS_SERVER) < 0) {
		goto NEW_SVR_ERROR;
	}

	*ssl = lssl;
	return MATRIXSSL_SUCCESS;
	
NEW_SVR_ERROR:
	if (lssl) matrixSslDeleteSession(lssl);
	return PS_FAILURE;
}
#endif /* USE_SERVER_SIDE_SSL */

/******************************************************************************/
/*
	Caller is asking for allocated buffer storage to recv data into
 
	buf		Populated with a transient area where data can be read into
 
	Return	> 0, size of 'buf' in bytes
			<= 0 on error
 */
int32 matrixSslGetReadbuf(ssl_t *ssl, unsigned char **buf)
{
	if (!ssl || !buf) {
		return PS_ARG_FAIL;
	}
	psAssert(ssl && ssl->insize > 0 && ssl->inbuf != NULL);
	if (buf == NULL) {
		return PS_ARG_FAIL;
	}
	/* If there's unprocessed data in inbuf, have caller append to it */
	*buf = ssl->inbuf + ssl->inlen;
	return ssl->insize - ssl->inlen;
}

/******************************************************************************/
/*
 	Caller is asking if there is any encoded, outgoing SSL data that should be 
		sent out the transport layer.
 
	buf		if provided, is updated to point to the data to be sent

	Return	> 0, the number of bytes to send
			0 if there is no pending data
			< 0 on error
 */
int32 matrixSslGetOutdata(ssl_t *ssl, unsigned char **buf)
{
	if (!ssl || !buf) {
		return PS_ARG_FAIL;
	}
	psAssert(ssl->outsize > 0 && ssl->outbuf != NULL);
	if (buf) {
		*buf = ssl->outbuf;
	}
	return ssl->outlen;	/* Can be 0 */
}

/******************************************************************************/
/*
	Caller is asking for an allocated buffer to write plaintext into.
		That plaintext will then be encoded when the caller subsequently calls
		matrixSslEncodeWriteBuf()
	
	ssl		SSL session context
 
	buf		The data storage to write into will be populated here on success
 
	reqLen	The amount of buffer space the caller would like to use
 
	Return	> 0, # bytes available in buf on success
			PS_MEM_FAIL if reqLen too large for current memory
			<= 0 on error
 */
int32 matrixSslGetWritebuf(ssl_t *ssl, unsigned char **buf, uint32 reqLen)
{
	int32			sz;
	
	if (!ssl || !buf) {
		return PS_ARG_FAIL;
	}
	psAssert(ssl->outsize > 0 && ssl->outbuf != NULL);
	/* reqLen might be slightly longer than we need due to using MAX sizes */
	reqLen += (matrixSslGetEncodedHeaderSize(ssl) + SSL_MAX_BLOCK_SIZE + 
			   SSL_MAX_MAC_SIZE);
	reqLen = min(reqLen, SSL_MAX_BUF_SIZE);
	if ((sz = ssl->outsize - ssl->outlen) < 0) {
		return PS_FAILURE;
	}
	if ((uint32)sz < reqLen) {
		ssl->outsize += reqLen - sz;
		if ((ssl->outbuf = psRealloc(ssl->outbuf, ssl->outsize)) == NULL) {
			return PS_MEM_FAIL;
		}
	}
	reqLen = ssl->outsize - (ssl->outlen + matrixSslGetEncodedHeaderSize(ssl)+
							SSL_MAX_BLOCK_SIZE + SSL_MAX_MAC_SIZE);
							
	*buf = ssl->outbuf + ssl->outlen + matrixSslGetEncodedHeaderSize(ssl);
	return reqLen; /* may not be what was passed in */
}

/******************************************************************************/
/*
	Encode (encrypt) 'len' bytes of plaintext data placed into buffer given
	by matrixSslGetWritebuf()
	SHOULD ONLY BE CALLED AFTER A PREVIOUS CALL TO matrixSslGetWritebuf
 
	Returns < 0 on error, total #bytes in outgoing data buf on success
 */
int32 matrixSslEncodeWritebuf(ssl_t *ssl, uint32 len)
{
	unsigned char	*origbuf;
	int32			rc;
	
	if (!ssl || len == 0) {
		return PS_ARG_FAIL;
	}
	if (ssl->bFlags & BFLAG_CLOSE_AFTER_SENT) {
		return PS_PROTOCOL_FAIL;
	}
	psAssert(ssl->outsize > 0 && ssl->outbuf != NULL);
	/* Caller was given proper locations and lengths in GetWritebuf() */
	origbuf = ssl->outbuf + ssl->outlen;
	if (ssl->outbuf == NULL || (ssl->outsize - ssl->outlen) < (int32)len) {
		return PS_FAILURE;
	}
	
	rc = matrixSslEncode(ssl, origbuf, (ssl->outsize - ssl->outlen),
						 origbuf + matrixSslGetEncodedHeaderSize(ssl), &len);
	if (rc < 0) {
		psAssert(rc != SSL_FULL);	/* should not happen */
		return PS_FAILURE;
	}
	ssl->outlen += len;
	return ssl->outlen;
}

/******************************************************************************/
/*
	Caller has received data from the network and is notifying the SSL layer
 */
int32 matrixSslReceivedData(ssl_t *ssl, uint32 bytes, unsigned char **ptbuf, 
							uint32 *ptlen)
{
	unsigned char	*buf, *prevBuf;
	int32			rc, decodeRet, size, sanity, decodeErr;
	uint32			processed, start, len, reqLen;
	unsigned char	alertLevel, alertDesc;

	if (!ssl || !ptbuf || !ptlen) {
		return PS_ARG_FAIL;
	}
	psAssert(ssl->outsize > 0 && ssl->outbuf != NULL);
	psAssert(ssl->insize > 0 && ssl->inbuf != NULL);
	*ptbuf = NULL;
	*ptlen = 0;
	ssl->inlen += bytes;
	if (ssl->inlen == 0) {
		return PS_ARG_FAIL;
	}
	/* Parameterized sanity check to avoid infinite loops */
	if (matrixSslHandshakeIsComplete(ssl)) {
		/* Minimum possible record size once negotiated */
		sanity = ssl->inlen / (SSL3_HEADER_LEN + MD5_HASH_SIZE);
	} else {
		/* Even with an SSLv2 hello, the sanity check will let 1 pass through */
		sanity = ssl->inlen / (SSL3_HEADER_LEN + SSL3_HANDSHAKE_HEADER_LEN);
	}
	/* This is outside the loop b/c we may want to parse within inbuf later */
	buf = ssl->inbuf;
DECODE_MORE:
	if (unlikely(sanity-- < 0)) {
		return PS_PROTOCOL_FAIL;	/* We've tried to decode too many times */
	}
	len = ssl->inlen;
	size = ssl->insize - (buf - ssl->inbuf);
	prevBuf = buf;
	decodeRet = matrixSslDecode(ssl, &buf, &len, size, &start, &reqLen,
						 &decodeErr, &alertLevel, &alertDesc);
/* 
	Convenience for the cases that expect buf to have moved
		- calculate the number of encoded bytes that were decoded
*/
	processed = buf - prevBuf;
	rc = PS_PROTOCOL_FAIL;
	switch (decodeRet) {

	case MATRIXSSL_SUCCESS:
		
		ssl->inlen -= processed;
		if (ssl->inlen > 0) {
			psAssert(buf > ssl->inbuf);
/*
			Pack cp->inbuf so there is immediate maximum room for potential
			outgoing data that needs to be written
*/
			memmove(ssl->inbuf, buf, ssl->inlen);
			buf = ssl->inbuf;
			goto DECODE_MORE;	/* More data in buffer to process */
		}
/*
		In this case, we've parsed a finished message and no additional data is
		available to parse. We let the client know the handshake is complete, 
		which can be used as a trigger to begin for example a HTTP request.
*/
		if (!(ssl->bFlags & BFLAG_HS_COMPLETE) && 
				matrixSslHandshakeIsComplete(ssl)) {
			ssl->bFlags |= BFLAG_HS_COMPLETE;
#ifdef USE_CLIENT_SIDE_SSL
			matrixSslGetSessionId(ssl, ssl->sid);
#endif /* USE_CLIENT_SIDE_SSL */
			rc = MATRIXSSL_HANDSHAKE_COMPLETE;
		} else {
			rc = MATRIXSSL_REQUEST_RECV; /* Need to recv more handshake data */
		}
		break;

	case SSL_SEND_RESPONSE:
/*
		This must be handshake data (or alert) or we'd be in PROCESS_DATA
		so there is no way there is anything left inside inbuf to process.
		
		...so processed isn't valid because the output params are outbuf
		related and we simply reset inlen
*/
		ssl->inlen = 0;

		/* If alert, close connection after sending */
		if (alertDesc != SSL_ALERT_NONE) {
			ssl->bFlags |= BFLAG_CLOSE_AFTER_SENT;
		}
		psAssert(prevBuf == buf);
		psAssert(ssl->insize >= (int32)len);
		psAssert(start == 0);
		psAssert(buf == ssl->inbuf);
		if (ssl->outlen > 0) { 	/* if data's in outbuf, append inbuf */
			if (ssl->outlen + (int32)len > ssl->outsize) {
				ssl->outsize = ssl->outlen + len;
				ssl->outbuf = psRealloc(ssl->outbuf, ssl->outsize);
			}
			memcpy(ssl->outbuf + ssl->outlen, ssl->inbuf, len);
			ssl->outlen += len;
		} else { /* otherwise, swap inbuf and outbuf */
			buf = ssl->outbuf; ssl->outbuf = ssl->inbuf; ssl->inbuf = buf;
			ssl->outlen = len;
			len = ssl->outsize; ssl->outsize = ssl->insize; ssl->insize = len;
			buf = ssl->inbuf;
			len = ssl->outlen;
		}
		rc = MATRIXSSL_REQUEST_SEND;	/* We queued data to send out */
		break;

	case MATRIXSSL_ERROR:
		return decodeErr; /* Will be a negative value */

	case SSL_ALERT:
		if (alertLevel == SSL_ALERT_LEVEL_FATAL) {
			psTraceIntInfo("Received FATAL alert %d.\n", alertDesc);
		} else {
			/* Closure notify is the normal case */
			if (alertDesc == SSL_ALERT_CLOSE_NOTIFY) {
				psTraceInfo("Normal SSL closure alert\n");
			} else {
				psTraceIntInfo("Received WARNING alert %d\n", alertDesc);
			}
		}
		/* Let caller access the 2 data bytes (severity and description) */
		psAssert(len == 2);
		*ptbuf = prevBuf;
		*ptlen = len;
		ssl->inlen -= processed;
		return MATRIXSSL_RECEIVED_ALERT;
			
	case SSL_PARTIAL:
		if (reqLen > SSL_MAX_BUF_SIZE) {
			return PS_MEM_FAIL;
		}
		if (reqLen > (uint32)ssl->insize) {
			ssl->insize = reqLen;
			if ((ssl->inbuf = psRealloc(ssl->inbuf, ssl->insize)) == NULL) {
				return PS_MEM_FAIL;
			}
			buf = ssl->inbuf;
			/* Don't need to change inlen */
		}
		rc = MATRIXSSL_REQUEST_RECV;	/* Expecting more data */
		break;

	/* We've got outgoing data that's larger than our buffer */
	case SSL_FULL:
		if (reqLen > SSL_MAX_BUF_SIZE) {
			return PS_MEM_FAIL;
		}
		/* We balk if we get a large handshake message */
		if (reqLen > SSL_MAX_PLAINTEXT_LEN &&
				!matrixSslHandshakeIsComplete(ssl)) {
			if (reqLen > SSL_MAX_PLAINTEXT_LEN) {
				return PS_MEM_FAIL;
			}
		}
		/* 
			Can't envision any possible case where there is remaining data
			in inbuf to process and are getting SSL_FULL. 
		*/
		ssl->inlen = 0;
		
		/* Grow inbuf */
		if (reqLen > (uint32)ssl->insize) {
			len = ssl->inbuf - buf;
			ssl->insize = reqLen;
			if ((ssl->inbuf = psRealloc(ssl->inbuf, ssl->insize)) == NULL) {
				return PS_MEM_FAIL;
			}
			buf = ssl->inbuf + len;
			/* Note we leave inlen untouched here */
		} else {
			return PS_PROTOCOL_FAIL;	/* error in our encoding */
		}
		goto DECODE_MORE;

	case SSL_PROCESS_DATA:
/*
		Possible we received a finished message and app data in the same
		flight. In this case, the caller is not notified that the handshake
		is complete, but rather is notified that there is application data to
		process.
 */
		if (!(ssl->bFlags & BFLAG_HS_COMPLETE) && 
			matrixSslHandshakeIsComplete(ssl)) {
			ssl->bFlags |= BFLAG_HS_COMPLETE;
#ifdef USE_CLIENT_SIDE_SSL
			matrixSslGetSessionId(ssl, ssl->sid);
#endif /* USE_CLIENT_SIDE_SSL */
		}
/*
		 .	prevbuf points to start of unencrypted data
		 .	buf points to start of any remaining unencrypted data
		 .	start is length of remaining encrypted data yet to decode
		 .	len is length of unencrypted data ready for user processing
 */			
		ssl->inlen -= processed;
		psAssert((uint32)ssl->inlen == start);
		
/*
		Blank SSL app data records are vaild in the SSL/TLS spec
		(They are simply a header, Mac and padding)
		Here, we "eat them", although we could potentially return this
		blank to the user as a valid 'ptbuf' pointer, and a 'ptlen' of zero.
 */
		if (len == 0) {
			psTraceInfo("Ignoring blank SSL record\n");
			/* Move any remaining data to the beginning of the buffer */
			if (ssl->inlen > 0) {
				memmove(ssl->inbuf, buf, ssl->inlen);
				buf = ssl->inbuf;
			}
			goto DECODE_MORE;
		}
		/* Call user plaintext data handler */
		psAssert(ssl->bFlags & BFLAG_HS_COMPLETE);		
		*ptbuf = prevBuf;
		*ptlen = len;

		return MATRIXSSL_APP_DATA;
	} /* switch decodeRet */
	
	if (ssl->inlen > 0 && (buf != ssl->inbuf)) {
		psAssert(0);
	}
	/* Shrink inbuf to default size once inlen < default size */
	if (ssl->insize > SSL_DEFAULT_BUF_SIZE && 
			ssl->inlen < SSL_DEFAULT_BUF_SIZE) {
		psAssert(*ptbuf == NULL);
		ssl->insize	 = SSL_DEFAULT_BUF_SIZE;
		/* It's not fatal if we can't realloc it smaller */
		if ((buf = psRealloc(ssl->inbuf, ssl->insize)) != NULL) {
			ssl->inbuf = buf;
		}
	}
	return rc;
}

/******************************************************************************/
/*
	Plaintext data has been processed as a response to MATRIXSSL_APP_DATA or
	MATRIXSSL_RECEIVED_ALERT return codes from matrixSslReceivedData()
	Return:
		< 0 on error
		0 if there is no more incoming ssl data in the buffer
			Caller should take whatever action is appropriate to the specific
			protocol implementation, eg. read for more data, close, etc.
		> 0 error code is same meaning as from matrixSslReceivedData()
			In this case, ptbuf and ptlen will be modified and caller should
			handle return code identically as from matrixSslReceivedData()
			This is the case when more than one SSL record is in the buffer
 */
int32 matrixSslProcessedData(ssl_t *ssl, unsigned char **ptbuf, uint32 *ptlen)
{
	uint32	ctlen;
	void	*p;
	
	if (!ssl || !ptbuf || !ptlen) {
		return PS_ARG_FAIL;
	}
	*ptbuf = NULL;
	*ptlen = 0;
	
	psAssert(ssl->insize > 0 && ssl->inbuf != NULL);
	/* Move any remaining data to the beginning of the buffer */
	if (ssl->inlen > 0) {
		ctlen = ssl->rec.len + SSL3_HEADER_LEN;
		memmove(ssl->inbuf, ssl->inbuf + ctlen, ssl->inlen);
	}
	/* Shrink inbuf to default size once inlen < default size */
	if (ssl->insize > SSL_DEFAULT_BUF_SIZE && 
			ssl->inlen < SSL_DEFAULT_BUF_SIZE) {
		ssl->insize	 = SSL_DEFAULT_BUF_SIZE;
		/* It's not fatal if we can't realloc it smaller */
		if ((p = psRealloc(ssl->inbuf, ssl->insize)) != NULL) {
			ssl->inbuf = p;
		}
	}
	/* If there's more data, try to decode it here and return that code */
	if (ssl->inlen > 0) {
		/* NOTE: ReceivedData cannot return 0 */
		return matrixSslReceivedData(ssl, 0, ptbuf, ptlen);
	}
	return MATRIXSSL_SUCCESS;
}

/******************************************************************************/
/*
	Returns < 0 on error
 */
int32 matrixSslEncodeClosureAlert(ssl_t *ssl)
{
	sslBuf_t		sbuf;
	unsigned char	*p;
	int32			rc;
	uint32			reqLen, newLen;
	
	if (!ssl) {
		return PS_ARG_FAIL;
	}
	psAssert(ssl->outsize > 0 && ssl->outbuf != NULL);
/*
	Only encode the closure alert if we aren't already flagged for close
	If we are flagged, we do not want to send any more data
 */
	newLen = 0;
	if (!(ssl->bFlags & BFLAG_CLOSE_AFTER_SENT)) {
		ssl->bFlags |= BFLAG_CLOSE_AFTER_SENT;
L_CLOSUREALERT:
		sbuf.buf = sbuf.start = sbuf.end = ssl->outbuf + ssl->outlen;
		sbuf.size = ssl->outsize - ssl->outlen;
		rc = sslEncodeClosureAlert(ssl, &sbuf, &reqLen);
		if (rc == SSL_FULL && newLen == 0) {
			newLen = ssl->outlen + reqLen;
			if (newLen > SSL_MAX_BUF_SIZE ||
					(p = psRealloc(ssl->outbuf, newLen)) == NULL) {
				return PS_MEM_FAIL;
			}
			ssl->outsize = newLen;
			ssl->outbuf = p;
			goto L_CLOSUREALERT; /* Try one more time */
		} else if (rc != PS_SUCCESS) {
			return rc;
		}
		ssl->outlen += sbuf.end - sbuf.start;
	}
	return MATRIXSSL_SUCCESS;
}

#ifdef SSL_REHANDSHAKES_ENABLED
/******************************************************************************/
/*
	Encode a CLIENT_HELLO or HELLO_REQUEST to re-handshake an existing
	connection.
		
	Can't "downgrade" the re-handshake.  This means if keys or certCb are
	NULL we stick with whatever the session already has loaded.
	
	keys should be NULL if no change in key material is being made
	
	cipherSpec is only used by clients
 */
int32 matrixSslEncodeRehandshake(ssl_t *ssl, sslKeys_t *keys,
				int32 (*certCb)(ssl_t *ssl, psX509Cert_t *cert, int32 alert),
				uint32 sessionOption, uint32 cipherSpec)
{
	sslBuf_t		sbuf;
	int32			rc;		
	uint32			reqLen, newLen;
	
	if (!ssl) {
		return PS_ARG_FAIL;
	}
	if (ssl->bFlags & BFLAG_CLOSE_AFTER_SENT) {
		return PS_PROTOCOL_FAIL;
	}	
	psAssert(ssl->outsize > 0 && ssl->outbuf != NULL);
/*
	The only explicit option that can be passsed in is
	SSL_OPTION_FULL_HANDSHAKE to indicate no resumption is allowed
*/
	if (sessionOption == SSL_OPTION_FULL_HANDSHAKE) {
		matrixSslSetSessionOption(ssl, sessionOption, NULL);
	}
/*
	If the key material or cert callback are provided we have to assume it
	was intentional to "upgrade" the re-handshake and we force full handshake
	No big overhead calling SetSessionOption with FULL_HS multiple times.
*/
	if (keys != NULL) {
		ssl->keys = keys;
		matrixSslSetSessionOption(ssl, SSL_OPTION_FULL_HANDSHAKE, NULL);
	}

	if (certCb != NULL) { 
		if (ssl->flags & SSL_FLAGS_SERVER) {
			psAssert(certCb == NULL);
#ifdef USE_CLIENT_SIDE_SSL
		} else {
			matrixSslSetCertValidator(ssl, (sslCertCb_t)certCb);
			matrixSslSetSessionOption(ssl, SSL_OPTION_FULL_HANDSHAKE, NULL);
#endif /* USE_CLIENT_SIDE_SSL */
		}
	}	
	
/*
	If cipher spec is different from current, assume a full handshake
*/
	if (!(ssl->flags & SSL_FLAGS_SERVER)) {
		if (cipherSpec != 0 && ssl->cipher->ident != cipherSpec) {
			matrixSslSetSessionOption(ssl, SSL_OPTION_FULL_HANDSHAKE, NULL);
		}
	}
/*
	Options are set.  Encode the HELLO message
*/
	newLen = 0;
L_REHANDSHAKE:
	if (ssl->flags & SSL_FLAGS_SERVER) {
		sbuf.buf = sbuf.start = sbuf.end = ssl->outbuf + ssl->outlen;
		sbuf.size = ssl->outsize - ssl->outlen;
		if ((rc = matrixSslEncodeHelloRequest(ssl, &sbuf, &reqLen)) < 0) {
			if (rc == SSL_FULL && newLen == 0) {
				newLen = ssl->outlen + reqLen;
				if (newLen < SSL_MAX_BUF_SIZE) {
					ssl->outsize = newLen;
					ssl->outbuf = psRealloc(ssl->outbuf, ssl->outsize);
					if (ssl->outbuf == NULL) {
						return PS_MEM_FAIL;
					}
					goto L_REHANDSHAKE;
				}
			}
			return rc;
		}
	} else {
		sbuf.buf = sbuf.start = sbuf.end = ssl->outbuf + ssl->outlen;
		sbuf.size = ssl->outsize - ssl->outlen;
		if ((rc = matrixSslEncodeClientHello(ssl, &sbuf, cipherSpec, &reqLen,
				NULL)) < 0) {
			if (rc == SSL_FULL && newLen == 0) {
				newLen = ssl->outlen + reqLen;
				if (newLen < SSL_MAX_BUF_SIZE) {
					ssl->outsize = newLen;
					ssl->outbuf = psRealloc(ssl->outbuf, ssl->outsize);
					if (ssl->outbuf == NULL) {
						return PS_MEM_FAIL;
					}
					goto L_REHANDSHAKE;
				}
			}
			return rc;
		}
	}
	ssl->outlen += sbuf.end - sbuf.start;
	return MATRIXSSL_SUCCESS;
}
#else
int32 matrixSslEncodeRehandshake(ssl_t *ssl, sslKeys_t *keys,
				int32 (*certCb)(ssl_t *ssl, psX509Cert_t *cert, int32 alert),
				uint32 sessionOption, uint32 cipherSpec)
{
	psTraceInfo("Rehandshaking is disabled.  matrixSslEncodeRehandshake off\n");
	return PS_FAILURE;
}
#endif /* SSL_REHANDSHAKES_ENABLED */

/******************************************************************************/
/*
	Caller is indicating 'bytes' of data was written
 */
int32 matrixSslSentData(ssl_t *ssl, uint32 bytes)
{
	int32		rc;
	void		*p;
	
	if (!ssl) {
		return PS_ARG_FAIL;
	}
	if (bytes == 0) {
		if (ssl->outlen > 0) {
			return MATRIXSSL_REQUEST_SEND;
		} else {
			return MATRIXSSL_SUCCESS;	/* Nothing to do */
		}
	}
	psAssert(ssl->outsize > 0 && ssl->outbuf != NULL);
	ssl->outlen -= bytes;

	rc = MATRIXSSL_SUCCESS;
	if (ssl->outlen > 0) {
		memmove(ssl->outbuf, ssl->outbuf + bytes, ssl->outlen);
		rc = MATRIXSSL_REQUEST_SEND;
	}
	/* 	If there's nothing left to flush, reallocate the buffer smaller. */
	if ((ssl->outlen == 0) && (ssl->bFlags & BFLAG_CLOSE_AFTER_SENT)) {
		/* We want to close the connection now */
		rc = MATRIXSSL_REQUEST_CLOSE;
	} else if (ssl->outsize > SSL_DEFAULT_BUF_SIZE && 
			   ssl->outlen < SSL_DEFAULT_BUF_SIZE) {
		ssl->outsize = SSL_DEFAULT_BUF_SIZE;
		/* It's not fatal if we can't realloc it smaller */
		if ((p = psRealloc(ssl->outbuf, ssl->outsize)) != NULL) {
			ssl->outbuf	= p;
		}
	}
	/* Indicate the handshake is complete, in this case, the finished message
		is being/has been just sent. Occurs in session resumption. */
	if (!(ssl->bFlags & BFLAG_HS_COMPLETE) && 
			matrixSslHandshakeIsComplete(ssl)) {
		ssl->bFlags |= BFLAG_HS_COMPLETE;
#ifdef USE_CLIENT_SIDE_SSL
		matrixSslGetSessionId(ssl, ssl->sid);
#endif /* USE_CLIENT_SIDE_SSL */
		rc = MATRIXSSL_HANDSHAKE_COMPLETE;
	}
	return rc;
}

/******************************************************************************/
