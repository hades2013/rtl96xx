/*
 *	matrixsslApi.h
 *	Release $Name: MATRIXSSL-3-1-3-OPEN $
 *	
 *	Public header file for MatrixSSL
 *	Implementations interacting with the matrixssl library should
 *	only use the APIs and definitions used in this file.
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

#ifndef _h_MATRIXSSL
#define _h_MATRIXSSL

#ifdef __cplusplus
extern "C" {
#endif

#include "core/coreApi.h"
#include "crypto/cryptoApi.h"

#include "matrixsslConfig.h"
#include "matrixssllib.h"
#include "version.h"

/******************************************************************************/
/*
	Public return codes
*/
#define MATRIXSSL_SUCCESS			PS_SUCCESS   /* Generic success */
#define MATRIXSSL_ERROR				PS_PROTOCOL_FAIL  /* Generic SSL error */
#define MATRIXSSL_REQUEST_SEND		1	/* API produced data to be sent */
#define MATRIXSSL_REQUEST_RECV		2	/* API requres more data to continue */
#define MATRIXSSL_REQUEST_CLOSE		3	/* API indicates clean close is req'd */
#define MATRIXSSL_APP_DATA			4	/* App data is avail. to caller */
#define MATRIXSSL_HANDSHAKE_COMPLETE	5	/* Handshake completed */
#define MATRIXSSL_RECEIVED_ALERT	6	/* An alert was received */
	
/******************************************************************************/
/*
 *	Public API set
 */
PSPUBLIC int32	matrixSslOpen(void);
PSPUBLIC void	matrixSslClose(void);

PSPUBLIC int32	matrixSslNewKeys(sslKeys_t **keys);

PSPUBLIC void	matrixSslDeleteKeys(sslKeys_t *keys);

PSPUBLIC void	matrixSslDeleteSession(ssl_t *ssl);

PSPUBLIC int32 matrixSslGetReadbuf(ssl_t *ssl, unsigned char **buf);
PSPUBLIC int32 matrixSslGetOutdata(ssl_t *ssl, unsigned char **buf);
PSPUBLIC int32 matrixSslGetWritebuf(ssl_t *ssl, unsigned char **buf,
				uint32 reqLen);
PSPUBLIC int32 matrixSslEncodeWritebuf(ssl_t *ssl, uint32 len);
PSPUBLIC int32 matrixSslSentData(ssl_t *ssl, uint32 bytes);
PSPUBLIC int32 matrixSslReceivedData(ssl_t *ssl, uint32 bytes,
				unsigned char **ptbuf, uint32 *ptlen);
PSPUBLIC int32 matrixSslProcessedData(ssl_t *ssl, 
				unsigned char **ptbuf, uint32 *ptlen);
PSPUBLIC int32 matrixSslEncodeClosureAlert(ssl_t *ssl);
								
PSPUBLIC int32	matrixSslLoadRsaKeys(sslKeys_t *keys, const char *certFile,
						const char *privFile, const char *privPass,
						const char *trustedCAFile);

PSPUBLIC int32	matrixSslLoadRsaKeysMem(sslKeys_t *keys,
						unsigned char *certBuf, int32 certLen,
						unsigned char *privBuf, int32 privLen,
						unsigned char *trustedCABuf, int32 trustedCALen);	
/******************************************************************************/
/*
	Advanced feature public APIS
 */
PSPUBLIC void	matrixSslGetAnonStatus(ssl_t *ssl, int32 *anonArg);
PSPUBLIC int32	matrixSslEncodeRehandshake(ssl_t *ssl, sslKeys_t *keys,
					int32 (*certCb)(ssl_t *ssl, psX509Cert_t *cert,int32 alert),
					uint32 sessionOption, uint32 cipherSpec);

#ifdef USE_CLIENT_SIDE_SSL
/******************************************************************************/
/*
	Client side APIs
*/
#define	matrixSslInitSessionId(SID) SID.cipherId = SSL_NULL_WITH_NULL_NULL
	
PSPUBLIC int32 matrixSslNewClientSession(ssl_t **ssl, sslKeys_t *keys,
			sslSessionId_t *sid, uint32 cipherSpec,
			int32 (*certCb)(ssl_t *ssl, psX509Cert_t *cert, int32 alert),
			tlsExtension_t *extensions,	int32 (*extCb)(ssl_t *ssl,
			unsigned short extType, unsigned short extLen, void *e));
/* Hello extension support.  RFC 3546 */
PSPUBLIC int32	matrixSslNewHelloExtension(tlsExtension_t **extension);
PSPUBLIC int32	matrixSslLoadHelloExtension(tlsExtension_t *extension,
					 unsigned char *extData, uint32 length, uint32 extType);
PSPUBLIC void	matrixSslDeleteHelloExtension(tlsExtension_t *extension);
#endif /* USE_CLIENT_SIDE_SSL */
/******************************************************************************/

#ifdef USE_SERVER_SIDE_SSL
/******************************************************************************/
/*
	Server side APIs
*/
PSPUBLIC int32 matrixSslNewServerSession(ssl_t **ssl, sslKeys_t *keys,
				  int32 (*certCb)(ssl_t *ssl, psX509Cert_t *cert, int32 alert));
#endif /* USE_SERVER_SIDE_SSL */


/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _h_MATRIXSSL */

/******************************************************************************/

