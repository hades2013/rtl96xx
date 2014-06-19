/*
 *	matrixssl.c
 *	Release $Name: MATRIXSSL-3-1-3-OPEN $
 *
 *	The session and authentication management portions of the MatrixSSL library
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

static const char copyright[] = 
"Copyright PeerSec Networks Inc. All rights reserved.";

#ifdef USE_SERVER_SIDE_SSL
static int32 verifyReadKeys(psPool_t *pool, sslKeys_t *keys);
#endif /* USE_SERVER_SIDE_SSL */


#ifdef USE_SERVER_SIDE_SSL
/*
	Static session table for session cache and lock for multithreaded env
*/
#ifdef USE_MULTITHREADING
static psMutex_t			sessionTableLock;
static psMutex_t			prngLock;
#endif /* USE_MULTITHREADING */

static sslSessionEntry_t	sessionTable[SSL_SESSION_TABLE_SIZE];
#endif /* USE_SERVER_SIDE_SSL */

#ifdef USE_FILE_SYSTEM
static int32 matrixSslLoadKeyMaterial(sslKeys_t *keys, const char *certFile,
				const char *privFile, const char *privPass, const char *CAfile,
				int32 privKeyType);
#endif
static int32 matrixSslLoadKeyMaterialMem(sslKeys_t *keys,
				unsigned char *certBuf,	int32 certLen, unsigned char *privBuf,
				int32 privLen, unsigned char *CAbuf, int32 CAlen,
				int32 privKeyType);

static psRandom_t gMatrixsslPrng;
/******************************************************************************/
/*
	Open and close the SSL module.  These routines are called once in the 
	lifetime of the application and initialize and clean up the library 
	respectively.
*/
int32 matrixSslOpen(void)
{
	if (psCoreOpen() < 0) {
		psError("pscore open failure\n");
		return PS_FAILURE;
	}
	psInitPrng(&gMatrixsslPrng);

#ifdef USE_SERVER_SIDE_SSL
	memset(sessionTable, 0x0, 
		sizeof(sslSessionEntry_t) * SSL_SESSION_TABLE_SIZE);
#ifdef USE_MULTITHREADING		
	psCreateMutex(&sessionTableLock);
	psCreateMutex(&prngLock);
#endif /* USE_MULTITHREADING */
#endif /* USE_SERVER_SIDE_SSL */


	return PS_SUCCESS;
}

/*
	MatrixSSL PRNG retrieval
*/
int32 matrixSslGetPrngData(unsigned char *bytes, uint32 size)
{
	int32	rc;
#ifdef USE_MULTITHREADING
	psLockMutex(&prngLock);
#endif /* USE_MULTITHREADING */
	rc = psGetPrng(&gMatrixsslPrng, bytes, size);
#ifdef USE_MULTITHREADING		
	psUnlockMutex(&prngLock);
#endif /* USE_MULTITHREADING */
	return rc;
}

/*
	matrixSslClose
*/
void matrixSslClose(void)
{
#ifdef USE_SERVER_SIDE_SSL
	int32		i;

#ifdef USE_MULTITHREADING
	psLockMutex(&sessionTableLock);
#endif /* USE_MULTITHREADING */
	for (i = 0; i < SSL_SESSION_TABLE_SIZE; i++) {
		if (sessionTable[i].inUse > 1) {
			psTraceInfo("Warning: closing while session still in use\n");
		}
	}
	memset(sessionTable, 0x0, 
		sizeof(sslSessionEntry_t) * SSL_SESSION_TABLE_SIZE);
#ifdef USE_MULTITHREADING		
	psUnlockMutex(&sessionTableLock);
	psDestroyMutex(&sessionTableLock);
	psUnlockMutex(&prngLock);
	psDestroyMutex(&prngLock);
#endif /* USE_MULTITHREADING */	
#endif /* USE_SERVER_SIDE_SSL */

	psCoreClose();
}

/******************************************************************************/
/*
	Must call to allocate the key structure now.  After which, LoadRsaKeys,
	LoadDhParams and/or LoadPskKey can be called 
	
	Memory info:
	Caller must free keys with matrixSslDeleteKeys on function success
	Caller does not need to free keys on function failure
*/
int32 matrixSslNewKeys(sslKeys_t **keys)
{
	psPool_t	*pool = NULL;
	sslKeys_t	*lkeys;
	
	
	lkeys = psMalloc(pool, sizeof(sslKeys_t));
	if (lkeys == NULL) {
		return PS_MEM_FAIL;
	}
	memset(lkeys, 0x0, sizeof(sslKeys_t));
	lkeys->pool = pool;
	
	*keys = lkeys;
	return PS_SUCCESS;
}


/******************************************************************************/
/*
	Key reads
*/
#ifdef USE_FILE_SYSTEM
int32 matrixSslLoadRsaKeys(sslKeys_t *keys, const char *certFile,
				const char *privFile, const char *privPass, const char *CAfile)
{
	return matrixSslLoadKeyMaterial(keys, certFile, privFile, privPass, CAfile,
				PS_RSA);

}


static int32 matrixSslLoadKeyMaterial(sslKeys_t *keys, const char *certFile,
				const char *privFile, const char *privPass, const char *CAfile,
				int32 privKeyType)
{
	psPool_t	*pool;
	int32		err, flags;

	if (keys == NULL) {
		return PS_ARG_FAIL;
	}
	pool = keys->pool;

/*
	Setting flags to store raw ASN.1 stream for SSL CERTIFICATE message use
*/
	flags = CERT_STORE_UNPARSED_BUFFER;
	

	if (certFile) {
#ifdef USE_SERVER_SIDE_SSL	
		if (keys->cert != NULL) {
			return PS_UNSUPPORTED_FAIL;
		}
		if ((err = psX509ParseCertFile(pool, (char*)certFile,
				&keys->cert, flags)) < 0) {
			return err;
		}
#else
		psTraceStrInfo("Ignoring %s certFile in matrixSslReadKeys\n",
					(char*)certFile);
#endif /* USE_SERVER_SIDE_SSL */		
	}
/*
	Parse the private key file
*/
	if (privFile) {
#ifdef USE_SERVER_SIDE_SSL
		if (keys->privKey != NULL) {
			return PS_UNSUPPORTED_FAIL;
		}
		if (privKeyType == PS_RSA) {
			if ((err = pkcs1ParsePrivFile(pool, (char*)privFile,
					(char*)privPass, &keys->privKey)) < 0) {
				psX509FreeCert(keys->cert);
				return err;
			}
		}
#else
		psTraceStrInfo("Ignoring %s privFile in matrixSslReadKeys\n",
					(char*)privFile);
#endif /* USE_SERVER_SIDE_SSL */			
	}

#ifdef USE_SERVER_SIDE_SSL
	if (verifyReadKeys(pool, keys) < PS_SUCCESS) {			
		psX509FreeCert(keys->cert);
		psFreePubKey(keys->privKey);
		return PS_CERT_AUTH_FAIL;
	}
#endif /* USE_SERVER_SIDE_SSL */
	
/*
	Not necessary to store binary representations of CA certs
*/
	flags &= ~CERT_STORE_UNPARSED_BUFFER;

	if (CAfile) {	
#ifdef USE_CLIENT_SIDE_SSL
		if (keys->CAcerts != NULL) {
			return PS_UNSUPPORTED_FAIL;
		}
		if ((err = psX509ParseCertFile(pool, (char*)CAfile,
				&keys->CAcerts, flags)) < 0) {
#ifdef USE_SERVER_SIDE_SSL				
			psX509FreeCert(keys->cert);
			psFreePubKey(keys->privKey);
#endif /* USE_SERVER_SIDE_SSL */	
			return err;
		}
#else
		psTraceStrInfo("Ignoring %s CAfile in matrixSslReadKeys\n", (char*)CAfile);		
#endif /* USE_CLIENT_SIDE_SSL */
	}

	return PS_SUCCESS;
}
#endif /* USE_FILE_SYSTEM */

/******************************************************************************/
/*
	Memory buffer versions of ReadKeys

	This function supports cert chains and multiple CAs.  Just need to
	string them together and let psX509ParseCert handle it 
*/
int32 matrixSslLoadRsaKeysMem(sslKeys_t *keys, unsigned char *certBuf,
			int32 certLen, unsigned char *privBuf, int32 privLen,
			unsigned char *CAbuf, int32 CAlen)
{
	return matrixSslLoadKeyMaterialMem(keys, certBuf, certLen, privBuf, privLen,
				CAbuf, CAlen, PS_RSA);

}


static int32 matrixSslLoadKeyMaterialMem(sslKeys_t *keys,
				unsigned char *certBuf,	int32 certLen, unsigned char *privBuf,
				int32 privLen, unsigned char *CAbuf, int32 CAlen,
				int32 privKeyType)
{
	psPool_t	*pool;
	int32		err, flags = 0;

	if (keys == NULL) {
		return PS_ARG_FAIL;
	}
	pool = keys->pool;
	
/*
	 Setting flags to store raw ASN.1 stream for SSL CERTIFICATE message use
*/
	flags = CERT_STORE_UNPARSED_BUFFER;
	
	
	if (certBuf) {	
#ifdef USE_SERVER_SIDE_SSL
		if (keys->cert != NULL) {
			return PS_UNSUPPORTED_FAIL;
		}
		if ((err = psX509ParseCert(pool, certBuf, (uint32)certLen, &keys->cert,
				flags)) < 0) {
			return err;
		}
#else
		psTraceInfo("Ignoring certBuf in matrixSslReadKeysMem\n");
#endif /* USE_SERVER_SIDE_SSL */			
	}
	
	if (privBuf) {
#ifdef USE_SERVER_SIDE_SSL
		if (keys->privKey != NULL) {
			return PS_UNSUPPORTED_FAIL;
		}
		if (privKeyType == PS_RSA) {		
			if ((err = pkcs1ParsePrivBin(pool, privBuf, (uint32)privLen,
					&keys->privKey)) < 0) {
				psX509FreeCert(keys->cert);
				return err;
			}
		}

#else
		psTraceInfo("Ignoring privBuf in matrixSslReadKeysMem\n");
#endif /* USE_SERVER_SIDE_SSL */		
	}
	
#ifdef USE_SERVER_SIDE_SSL
	if (verifyReadKeys(pool, keys) < PS_SUCCESS) {
		psX509FreeCert(keys->cert);
		psFreePubKey(keys->privKey);
		return PS_CERT_AUTH_FAIL;
	}
#endif /* USE_SERVER_SIDE_SSL */

/*
	 Not necessary to store binary representations of CA certs
*/
	flags &= ~CERT_STORE_UNPARSED_BUFFER;	
	
	if (CAbuf) {
#ifdef USE_CLIENT_SIDE_SSL
		if (keys->CAcerts != NULL) {
			return PS_UNSUPPORTED_FAIL;
		}
		if ((err = psX509ParseCert(pool, CAbuf, (uint32)CAlen, &keys->CAcerts,
				flags)) < 0) {
#ifdef USE_SERVER_SIDE_SSL		
			psFreePubKey(keys->privKey);
			psX509FreeCert(keys->cert);
#endif			
			return err;
		}
#else
		psTraceInfo("Ignoring CAbuf in matrixSslReadKeysMem\n");		
#endif /* USE_CLIENT_SIDE_SSL */
	}

	return PS_SUCCESS;
}

/******************************************************************************/
/*
	This will free the struct and any key material that was loaded via:
		matrixSslLoadRsaKeys
		matrixSslLoadDhParams
		matrixSslLoadPsk	
*/
void matrixSslDeleteKeys(sslKeys_t *keys)
{
	psPool_t	*pool;
	
	if (keys == NULL) {
		return;
	}
#ifdef USE_SERVER_SIDE_SSL	
	if (keys->cert) {
		psX509FreeCert(keys->cert);
	}
	
	if (keys->privKey) {
		psFreePubKey(keys->privKey);
	}
#endif /* USE_SERVER_SIDE_SSL */
	
#ifdef USE_CLIENT_SIDE_SSL
	if (keys->CAcerts) {
		psX509FreeCert(keys->CAcerts);
	}
#endif /* USE_CLIENT_SIDE_SSL */



	pool = keys->pool;
	psFree(keys);
}

#ifdef USE_SERVER_SIDE_SSL
/*
	Validate the cert chain and the private key for the material passed
	to matrixSslReadKeys.  Good to catch any user certifiate errors as
	soon as possible
*/
static int32 verifyReadKeys(psPool_t *pool, sslKeys_t *keys)
{
#ifdef USE_CERT_PARSE
	psX509Cert_t	*tmp;
#endif

	if (keys->cert == NULL && keys->privKey == NULL) {
		return PS_SUCCESS;
	}	
/*
	 Not allowed to have a certficate with no matching private key or 
	 private key with no cert to match with
*/
	if (keys->cert != NULL && keys->privKey == NULL) {
		psTraceInfo("No private key given to matrixSslReadKeys cert\n");
		return PS_CERT_AUTH_FAIL;
	}
	if (keys->privKey != NULL && keys->cert == NULL) {
		psTraceInfo("No cert given with private key to matrixSslReadKeys\n");
		return PS_CERT_AUTH_FAIL;
	}
#ifdef USE_CERT_PARSE	
/*
	If this is a chain, we can validate it here with psX509AuthenticateCert
	Don't check the error return code from this call because the chaining
	usage restrictions will test parent-most cert for self-signed.
	 
	But we can look at 'authStatus' on all but the final cert to see
	if the rest looks good
*/
	if (keys->cert != NULL && keys->cert->next != NULL) {
		psX509AuthenticateCert(pool, keys->cert, NULL);
		tmp = keys->cert;
		while (tmp->next != NULL) {
			if (tmp->authStatus != PS_TRUE) {
				psTraceInfo("Failed to authenticate cert chain\n");
				return PS_CERT_AUTH_FAIL;
			}
			tmp = tmp->next;
		}
	}

	if (keys->privKey != NULL && keys->privKey->type == PS_RSA) {
/*
		Testing the N member just as a sanity measure rather than
		attempting a full RSA crypt operation
*/
		if (pstm_cmp(&(keys->privKey->key->rsa.N),
				   &(keys->cert->publicKey.key->rsa.N)) != PSTM_EQ) {
			psTraceInfo("Private key doesn't match cert\n");	
			return PS_CERT_AUTH_FAIL;
		}
	}
#endif /* USE_CERT_PARSE */	
	return PS_SUCCESS;
}
#endif	/* USE_SERVER_SIDE_SSL */
/******************************************************************************/



/******************************************************************************/
/*
	New SSL protocol context
	This structure is associated with a single SSL connection.  Each socket
	using SSL should be associated with a new SSL context.

	certBuf and privKey ARE NOT duplicated within the server context, in order
	to minimize memory usage with multiple simultaneous requests.  They must 
	not be deleted by caller until all server contexts using them are deleted.
*/
int32 matrixSslNewSession(ssl_t **ssl, sslKeys_t *keys, sslSessionId_t *session,
						int32 flags)
{
	psPool_t	*pool = NULL;
	ssl_t		*lssl;

/*
	First API level chance to make sure a user is not attempting to use
	client or server support that was not built into this library compile
*/
#ifndef USE_SERVER_SIDE_SSL
	if (flags & SSL_FLAGS_SERVER) {
		psTraceInfo("SSL_FLAGS_SERVER passed to matrixSslNewSession but MatrixSSL lib was not compiled with server support\n");
		return PS_ARG_FAIL;
	}
#endif

#ifndef USE_CLIENT_SIDE_SSL
	if (!(flags & SSL_FLAGS_SERVER)) {
		psTraceInfo("SSL_FLAGS_SERVER was not passed to matrixSslNewSession but MatrixSSL was not compiled with client support\n");
		return PS_ARG_FAIL;
	}
#endif

	if (flags & SSL_FLAGS_CLIENT_AUTH) {
		psTraceInfo("SSL_FLAGS_CLIENT_AUTH passed to matrixSslNewSession but MatrixSSL was not compiled with USE_CLIENT_AUTH enabled\n");
		return PS_ARG_FAIL;
	}


	if (flags & SSL_FLAGS_SERVER) {
		if (keys == NULL) {
			psTraceInfo("NULL keys parameter passed to matrixSslNewSession\n");
			return PS_ARG_FAIL;
		}
		if (session != NULL) {
			psTraceInfo("Ignoring session parameter to matrixSslNewSession\n");
		}
	}

	*ssl = lssl = psMalloc(pool, sizeof(ssl_t));
	if (lssl == NULL) {
		return PS_MEM_FAIL;
	}
	memset(lssl, 0x0, sizeof(ssl_t));

/*
	Data buffers
*/
	lssl->outsize = SSL_DEFAULT_BUF_SIZE;
	lssl->outbuf = psMalloc(PEERSEC_NO_POOL, lssl->outsize);
	lssl->insize = SSL_DEFAULT_BUF_SIZE;
	lssl->inbuf = psMalloc(PEERSEC_NO_POOL, lssl->insize);

	lssl->sPool = pool;
	lssl->keys = keys;
	lssl->cipher = sslGetCipherSpec(lssl, SSL_NULL_WITH_NULL_NULL);
	sslActivateReadCipher(lssl);
	sslActivateWriteCipher(lssl);
	
	lssl->recordHeadLen = SSL3_HEADER_LEN;
	lssl->hshakeHeadLen = SSL3_HANDSHAKE_HEADER_LEN;
		

	if (flags & SSL_FLAGS_SERVER) {
		lssl->flags |= SSL_FLAGS_SERVER;
/*
		Client auth can only be requested by server, not set by client
*/
		if (flags & SSL_FLAGS_CLIENT_AUTH) {
			lssl->flags |= SSL_FLAGS_CLIENT_AUTH;
		}
		lssl->hsState = SSL_HS_CLIENT_HELLO;
	} else {
/*
		Client is first to set protocol version information based on
		compile and/or the 'flags' parameter so header information in
		the handshake messages will be correctly set.
*/
#ifdef USE_TLS
		lssl->majVer = TLS_MAJ_VER;
		lssl->minVer = TLS_MIN_VER;
		lssl->flags |= SSL_FLAGS_TLS;

#else /* USE_TLS */
		lssl->majVer = SSL3_MAJ_VER;
		lssl->minVer = SSL3_MIN_VER;
#endif /* USE_TLS */
		lssl->hsState = SSL_HS_SERVER_HELLO;
		if (session != NULL && session->cipherId != SSL_NULL_WITH_NULL_NULL) {
			lssl->cipher = sslGetCipherSpec(lssl, session->cipherId);
			if (lssl->cipher == NULL) {
				psTraceInfo("Invalid session id to matrixSslNewSession\n");
			} else {
				memcpy(lssl->sec.masterSecret, session->masterSecret, 
					SSL_HS_MASTER_SIZE);
				lssl->sessionIdLen = SSL_MAX_SESSION_ID_SIZE;
				memcpy(lssl->sessionId, session->id, SSL_MAX_SESSION_ID_SIZE);
			}
		}
		lssl->sid = session;
	}
	lssl->err = SSL_ALERT_NONE;

	return PS_SUCCESS;
}

/******************************************************************************/
/*
	Delete an SSL session.  Some information on the session may stay around
	in the session resumption cache.
	SECURITY - We memset relevant values to zero before freeing to reduce 
	the risk of our keys floating around in memory after we're done.
*/
void matrixSslDeleteSession(ssl_t *ssl)
{

	if (ssl == NULL) {
		return;
	}
	ssl->flags |= SSL_FLAGS_CLOSED;
/*
	If we have a sessionId, for servers we need to clear the inUse flag in 
	the session cache so the ID can be replaced if needed.  In the client case
	the caller should have called matrixSslGetSessionId already to copy the
	master secret and sessionId, so free it now.

	In all cases except a successful updateSession call on the server, the
	master secret must be freed.
*/
#ifdef USE_SERVER_SIDE_SSL
	if (ssl->sessionIdLen > 0 && (ssl->flags & SSL_FLAGS_SERVER)) {
		matrixUpdateSession(ssl);
	}
#endif /* USE_SERVER_SIDE_SSL */
	ssl->sessionIdLen = 0;

#ifdef USE_CLIENT_SIDE_SSL
	if (ssl->sec.cert) {
		psX509FreeCert(ssl->sec.cert);
		ssl->sec.cert = NULL;
	}

#endif /* USE_CLIENT_SIDE_SSL */



/*
	Premaster could also be allocated if this DeleteSession is the result
	of a failed handshake.  This test is fine since all frees will NULL pointer
*/
	if (ssl->sec.premaster) {
		psFree(ssl->sec.premaster);
	}


/*
	Free the data buffers
*/
	psFree(ssl->outbuf);
	psFree(ssl->inbuf);
	
/*
	The cipher and mac contexts are inline in the ssl structure, so
	clearing the structure clears those states as well.
*/
	memset(ssl, 0x0, sizeof(ssl_t));
	psFree(ssl);
}

/******************************************************************************/
/*
	Generic session option control for changing already connected sessions.
	(ie. rehandshake control).  arg param is future for options that may
	require a value.
*/
void matrixSslSetSessionOption(ssl_t *ssl, int32 option, void *arg)
{
	if (option == SSL_OPTION_FULL_HANDSHAKE) {
#ifdef USE_SERVER_SIDE_SSL
		if (ssl->flags & SSL_FLAGS_SERVER) {
			matrixClearSession(ssl, 1);
		}
#endif /* USE_SERVER_SIDE_SSL */
		ssl->sessionIdLen = 0;
		memset(ssl->sessionId, 0x0, SSL_MAX_SESSION_ID_SIZE);
	}
	
}

/******************************************************************************/
/*
	Will be true if the cipher suite is an 'anon' variety OR if the
	user certificate callback returned SSL_ALLOW_ANON_CONNECTION
*/
void matrixSslGetAnonStatus(ssl_t *ssl, int32 *certArg)
{
	*certArg = ssl->sec.anon;
}

/******************************************************************************/
/*
	Returns PS_TURE if we've completed the SSL handshake.
*/
int32 matrixSslHandshakeIsComplete(ssl_t *ssl)
{
	return (ssl->hsState == SSL_HS_DONE) ? PS_TRUE : PS_FALSE;
}

#ifdef USE_CLIENT_SIDE_SSL
/******************************************************************************/
/*
	Set a custom callback to receive the certificate being presented to the
	session to perform custom authentication if needed

	NOTE: Must define either USE_CLIENT_SIDE_SSL or USE_CLIENT_AUTH
	in matrixConfig.h
*/
void matrixSslSetCertValidator(ssl_t *ssl,
			int32 (*certValidator)(void *, psX509Cert_t *, int32))
{
	if ((ssl != NULL) && (certValidator != NULL)) {
		ssl->sec.validateCert = certValidator;
	}
}
#endif /* USE_CLIENT_SIDE_SSL */

/******************************************************************************/
/*
	Initialize the SHA1 and MD5 hash contexts for the handshake messages
*/
int32 sslInitHSHash(ssl_t *ssl)
{
	psSha1Init(&ssl->sec.msgHashSha1);
	psMd5Init(&ssl->sec.msgHashMd5);
	return 0;
}

/******************************************************************************/
/*
	Add the given data to the running hash of the handshake messages
*/
int32 sslUpdateHSHash(ssl_t *ssl, unsigned char *in, uint32 len)
{
	psMd5Update(&ssl->sec.msgHashMd5, in, len);
	psSha1Update(&ssl->sec.msgHashSha1, in, len);
	return 0;
}

/******************************************************************************/
/*
	Snapshot is called by the receiver of the finished message to produce
	a hash of the preceeding handshake messages for comparison to incoming
	message.
*/
int32 sslSnapshotHSHash(ssl_t *ssl, unsigned char *out, int32 senderFlag)
{
	psDigestContext_t	md5, sha1;
	int32				len;
	
/*
	Use a backup of the message hash-to-date because we don't want
	to destroy the state of the handshaking until truly complete
*/
	md5 = ssl->sec.msgHashMd5;
	sha1 = ssl->sec.msgHashSha1;

#ifdef USE_TLS
	if (ssl->flags & SSL_FLAGS_TLS) {
		len = tlsGenerateFinishedHash(&md5, &sha1, ssl->sec.masterSecret,
			out, senderFlag);
	} else {
#endif /* USE_TLS */
		len = sslGenerateFinishedHash(&md5, &sha1, ssl->sec.masterSecret,
			out, senderFlag);
#ifdef USE_TLS
	}
#endif /* USE_TLS */

	return len;
}

int32 sslCreateKeys(ssl_t *ssl)
{
#ifdef USE_TLS
		if (ssl->flags & SSL_FLAGS_TLS) {
			return tlsDeriveKeys(ssl);
		} else {
			return sslDeriveKeys(ssl);
		}
#else 
		return sslDeriveKeys(ssl);
#endif /* USE_TLS */
}

/******************************************************************************/
/*
	Cipher suites are chosen before they are activated with the 
	ChangeCipherSuite message.  Additionally, the read and write cipher suites
	are activated at different times in the handshake process.  The following
	APIs activate the selected cipher suite callback functions.
*/
int32 sslActivateReadCipher(ssl_t *ssl)
{
	ssl->decrypt = ssl->cipher->decrypt;
	ssl->verifyMac = ssl->cipher->verifyMac;
	ssl->deMacSize = ssl->cipher->macSize;
	ssl->deBlockSize = ssl->cipher->blockSize;
	ssl->deIvSize = ssl->cipher->ivSize;
/*
	Reset the expected incoming sequence number for the new suite
*/
	memset(ssl->sec.remSeq, 0x0, sizeof(ssl->sec.remSeq));

	if (ssl->cipher->ident != SSL_NULL_WITH_NULL_NULL) {
		ssl->flags |= SSL_FLAGS_READ_SECURE;
/*
		Copy the newly activated read keys into the live buffers
*/
		memcpy(ssl->sec.readMAC, ssl->sec.rMACptr, ssl->cipher->macSize);
		memcpy(ssl->sec.readKey, ssl->sec.rKeyptr, ssl->cipher->keySize);
		memcpy(ssl->sec.readIV, ssl->sec.rIVptr, ssl->cipher->ivSize);
/*
		set up decrypt contexts
 */
		if (ssl->cipher->init(&(ssl->sec), INIT_DECRYPT_CIPHER,
				ssl->cipher->keySize) < 0) {
			psTraceInfo("Unable to initialize read cipher suite\n");
			return PS_FAILURE;
		}
	}
	return PS_SUCCESS;
}

int32 sslActivateWriteCipher(ssl_t *ssl)
{

	ssl->encrypt = ssl->cipher->encrypt;
	ssl->generateMac = ssl->cipher->generateMac;
	ssl->enMacSize = ssl->cipher->macSize;
	ssl->enBlockSize = ssl->cipher->blockSize;
	ssl->enIvSize = ssl->cipher->ivSize;
/*
	Reset the outgoing sequence number for the new suite
*/
	memset(ssl->sec.seq, 0x0, sizeof(ssl->sec.seq));
	if (ssl->cipher->ident != SSL_NULL_WITH_NULL_NULL) {
		ssl->flags |= SSL_FLAGS_WRITE_SECURE;
/*
		Copy the newly activated write keys into the live buffers
*/
		memcpy(ssl->sec.writeMAC, ssl->sec.wMACptr, ssl->cipher->macSize);
		memcpy(ssl->sec.writeKey, ssl->sec.wKeyptr, ssl->cipher->keySize);
		memcpy(ssl->sec.writeIV, ssl->sec.wIVptr, ssl->cipher->ivSize);
/*
		set up encrypt contexts
 */
		if (ssl->cipher->init(&(ssl->sec), INIT_ENCRYPT_CIPHER,
				ssl->cipher->keySize) < 0) {
			psTraceInfo("Unable to init write cipher suite\n");
			return PS_FAILURE;
		}
	}
	return PS_SUCCESS;
}

#ifdef USE_SERVER_SIDE_SSL
/******************************************************************************/
/*
	Register a session in the session resumption cache.  If successful (rc >=0),
	the ssl sessionId and sessionIdLength fields will be non-NULL upon
	return.
*/
int32 matrixRegisterSession(ssl_t *ssl)
{
	uint32		i, j;
	psTime_t	t;

	if (!(ssl->flags & SSL_FLAGS_SERVER)) {
		return PS_FAILURE;
	}
/*
	Iterate the session table, looking for an empty entry (cipher null), or
	the oldest entry that is not in use
*/
#ifdef USE_MULTITHREADING
	psLockMutex(&sessionTableLock);
#endif /* USE_MULTITHREADING */
	j = SSL_SESSION_TABLE_SIZE;
	t = sessionTable[0].accessTime;
	for (i = 0; i < SSL_SESSION_TABLE_SIZE; i++) {
		if (sessionTable[i].cipher == NULL) {
			break;
		}
		if (psCompareTime(sessionTable[i].accessTime, t) &&
				sessionTable[i].inUse == 0) {
			t = sessionTable[i].accessTime;
			j = i;
		}
	}
/*
	If there were no empty entries, get the oldest unused entry.
	If all entries are in use, return -1, meaning we can't cache the
	session at this time
*/
	if (i >= SSL_SESSION_TABLE_SIZE) {
		if (j < SSL_SESSION_TABLE_SIZE) {
			i = j;
		} else {
#ifdef USE_MULTITHREADING		
			psUnlockMutex(&sessionTableLock);
#endif /* USE_MULTITHREADING */			
			return PS_LIMIT_FAIL;
		}
	}
/*
	Register the incoming masterSecret and cipher, which could still be null, 
	depending on when we're called.
*/
	memcpy(sessionTable[i].masterSecret, ssl->sec.masterSecret,
		SSL_HS_MASTER_SIZE);
	sessionTable[i].cipher = ssl->cipher;
	sessionTable[i].inUse += 1;
#ifdef USE_MULTITHREADING	
	psUnlockMutex(&sessionTableLock);
#endif /* USE_MULTITHREADING */		
/*
	The sessionId is the current serverRandom value, with the first 4 bytes
	replaced with the current cache index value for quick lookup later.
	FUTURE SECURITY - Should generate more random bytes here for the session
	id.  We re-use the server random as the ID, which is OK, since it is
	sent plaintext on the network, but an attacker listening to a resumed
	connection will also be able to determine part of the original server
	random used to generate the master key, even if he had not seen it
	initially.
*/
	memcpy(sessionTable[i].id, ssl->sec.serverRandom, 
		min(SSL_HS_RANDOM_SIZE, SSL_MAX_SESSION_ID_SIZE));
	ssl->sessionIdLen = SSL_MAX_SESSION_ID_SIZE;
	sessionTable[i].id[0] = (unsigned char)(i & 0xFF);
	sessionTable[i].id[1] = (unsigned char)((i & 0xFF00) >> 8);
	sessionTable[i].id[2] = (unsigned char)((i & 0xFF0000) >> 16);
	sessionTable[i].id[3] = (unsigned char)((i & 0xFF000000) >> 24);
	memcpy(ssl->sessionId, sessionTable[i].id, SSL_MAX_SESSION_ID_SIZE);
/*
	startTime is used to check expiry of the entry
	accessTime is used to for cache replacement logic
	The versions are stored, because a cached session must be reused 
	with same SSL version.
*/
	psGetTime(&sessionTable[i].startTime);
	sessionTable[i].accessTime = sessionTable[i].startTime;
	sessionTable[i].majVer = ssl->majVer;
	sessionTable[i].minVer = ssl->minVer;

	return i;
}

/******************************************************************************/
/*
	Decrement inUse to keep the reference count meaningful
*/
int32 matrixClearSession(ssl_t *ssl, int32 remove)
{
	char	*id;
	uint32	i;

	if (ssl->sessionIdLen <= 0) {
		return PS_ARG_FAIL;
	}
	id = ssl->sessionId;
	
	i = (id[3] << 24) + (id[2] << 16) + (id[1] << 8) + id[0];
	if (i >= SSL_SESSION_TABLE_SIZE || i < 0) {
		return PS_LIMIT_FAIL;
	}
#ifdef USE_MULTITHREADING	
	psLockMutex(&sessionTableLock);
#endif /* USE_MULTITHREADING */
	sessionTable[i].inUse -= 1;
/*
	If this is a full removal, actually delete the entry.  Also need to
	clear any RESUME flag on the ssl connection so a new session
	will be correctly registered.
*/
	if (remove) {
		memset(ssl->sessionId, 0x0, SSL_MAX_SESSION_ID_SIZE);
		ssl->sessionIdLen = 0;
		memset(&sessionTable[i], 0x0, sizeof(sslSessionEntry_t));
		ssl->flags &= ~SSL_FLAGS_RESUMED;
	}
#ifdef USE_MULTITHREADING	
	psUnlockMutex(&sessionTableLock);
#endif /* USE_MULTITHREADING */	
	return PS_SUCCESS;
}

/******************************************************************************/
/*
	Look up a session ID in the cache.  If found, set the ssl masterSecret
	and cipher to the pre-negotiated values
*/
int32 matrixResumeSession(ssl_t *ssl)
{
	char	*id;
	uint32	i;

	if (!(ssl->flags & SSL_FLAGS_SERVER)) {
		return PS_ARG_FAIL;
	}
	if (ssl->sessionIdLen <= 0) {
		return PS_ARG_FAIL;
	}
	id = ssl->sessionId;

	i = (id[3] << 24) + (id[2] << 16) + (id[1] << 8) + id[0];
#ifdef USE_MULTITHREADING		
	psLockMutex(&sessionTableLock);
#endif /* USE_MULTITHREADING */
	if (i >= SSL_SESSION_TABLE_SIZE || i < 0 ||
			sessionTable[i].cipher == NULL) {
#ifdef USE_MULTITHREADING			
		psUnlockMutex(&sessionTableLock);
#endif /* USE_MULTITHREADING */		
		return PS_LIMIT_FAIL;
	}
/*
	Id looks valid.  Update the access time for expiration check.
	Expiration is done on daily basis (86400 seconds)
*/
	psGetTime(&sessionTable[i].accessTime);
	if ((memcmp(sessionTable[i].id, id, 
			(uint32)min(ssl->sessionIdLen, SSL_MAX_SESSION_ID_SIZE)) != 0) ||
			(psDiffMsecs(sessionTable[i].startTime,
				sessionTable[i].accessTime) > SSL_SESSION_ENTRY_LIFE) ||
			(sessionTable[i].majVer != ssl->majVer) ||
			(sessionTable[i].minVer != ssl->minVer)) {
#ifdef USE_MULTITHREADING			
		psUnlockMutex(&sessionTableLock);
#endif /* USE_MULTITHREADING */
		return PS_FAILURE;
	}
	
	memcpy(ssl->sec.masterSecret, sessionTable[i].masterSecret,
		SSL_HS_MASTER_SIZE);
	ssl->cipher = sessionTable[i].cipher;
	sessionTable[i].inUse += 1;
#ifdef USE_MULTITHREADING		
	psUnlockMutex(&sessionTableLock);
#endif /* USE_MULTITHREADING */	
	return PS_SUCCESS;
}

/******************************************************************************/
/*
	Update session information in the cache.
	This is called when we've determined the master secret and when we're
	closing the connection to update various values in the cache.
*/
int32 matrixUpdateSession(ssl_t *ssl)
{
	char	*id;
	uint32	i;

	if (!(ssl->flags & SSL_FLAGS_SERVER)) {
		return PS_ARG_FAIL;
	}
	if ((id = ssl->sessionId) == NULL) {
		return PS_ARG_FAIL;
	}
	i = (id[3] << 24) + (id[2] << 16) + (id[1] << 8) + id[0];
	if (i < 0 || i >= SSL_SESSION_TABLE_SIZE) {
		return PS_LIMIT_FAIL;
	}
/*
	If there is an error on the session, invalidate for any future use
*/
#ifdef USE_MULTITHREADING
	psLockMutex(&sessionTableLock);
#endif /* USE_MULTITHREADING */		
	sessionTable[i].inUse += ssl->flags & SSL_FLAGS_CLOSED ? -1 : 0;
	if (ssl->flags & SSL_FLAGS_ERROR) {
		memset(sessionTable[i].masterSecret, 0x0, SSL_HS_MASTER_SIZE);
		sessionTable[i].cipher = NULL;
#ifdef USE_MULTITHREADING		
		psUnlockMutex(&sessionTableLock);
#endif /* USE_MULTITHREADING */			
		return PS_FAILURE;
	}
	memcpy(sessionTable[i].masterSecret, ssl->sec.masterSecret,
		SSL_HS_MASTER_SIZE);
	sessionTable[i].cipher = ssl->cipher;
#ifdef USE_MULTITHREADING	
	psUnlockMutex(&sessionTableLock);
#endif /* USE_MULTITHREADING */		
	return PS_SUCCESS;
}
#endif /* USE_SERVER_SIDE_SSL */

#ifdef USE_CLIENT_SIDE_SSL
/******************************************************************************/
/*
	Get session information from the ssl structure and populate the given
	session structure.  Session will contain a copy of the relevant session
	information, suitable for creating a new, resumed session.

	NOTE: Must define USE_CLIENT_SIDE_SSL in matrixConfig.h 
	
	sslSessionId_t myClientSession;
	
	...&myClientSession
*/
int32 matrixSslGetSessionId(ssl_t *ssl, sslSessionId_t *session)
{

	if (ssl == NULL || ssl->flags & SSL_FLAGS_SERVER || session == NULL) {
		return PS_ARG_FAIL;
	}

	if (ssl->cipher != NULL && ssl->cipher->ident != SSL_NULL_WITH_NULL_NULL && 
			ssl->sessionIdLen == SSL_MAX_SESSION_ID_SIZE) {
		session->cipherId = ssl->cipher->ident;
		memcpy(session->id, ssl->sessionId, ssl->sessionIdLen);
		memcpy(session->masterSecret, ssl->sec.masterSecret, 
			SSL_HS_MASTER_SIZE);
		return PS_SUCCESS;
	}

	return PS_FAILURE;
}
#endif /* USE_CLIENT_SIDE_SSL */

/******************************************************************************/
/*
	Rehandshake. Free any allocated sec members that will be repopulated
*/
void sslResetContext(ssl_t *ssl)
{
	ssl->sec.anon = 0;
#ifdef USE_SERVER_SIDE_SSL
	if (ssl->flags & SSL_FLAGS_SERVER) {
		matrixClearSession(ssl, 0);
	}
#endif /* USE_SERVER_SIDE_SSL */



	ssl->bFlags = 0;  /* Reset buffer control */
}

#ifdef USE_CLIENT_SIDE_SSL
/******************************************************************************/
/*
*/
int32 matrixValidateCerts(psPool_t *pool, psX509Cert_t *subjectCerts,
						  psX509Cert_t *issuerCerts)
{
	psX509Cert_t    *ic, *sc, *icNext, *scNext;
	int32           rc;
	
/*
	Case #1 is no issuing cert.  Going to want to check that the final
	subject cert presented is a SelfSigned CA
*/
	if (issuerCerts == NULL) {
		return psX509AuthenticateCert(pool, subjectCerts, NULL);
	}
/*
	Case #2 is an issuing cert AND a chain of subjectCerts.
 */
	sc = subjectCerts;
	if ((ic = sc->next) != NULL) {
/*
		 We do have a chain. Authenticate the chain before even looking
		 to our issuer CAs.  We do a bit of trickery here with the chain
		 to comply with psX509AuthenticateCert restriction that chain-only
		 testing will do a self-signed test on the parent-most.
*/
		while (ic->next != NULL) {
			scNext = sc->next;
			sc->next = NULL;
			icNext = ic->next;
			ic->next = NULL;
			if ((rc = psX509AuthenticateCert(pool, sc, ic)) < PS_SUCCESS) {
				sc->next = scNext;
				ic->next = icNext;
				return rc; 
			}
			sc->next = scNext;
			ic->next = icNext;
			
			sc = sc->next;
			ic = sc->next;
		}
/*
		Test using the parent-most in chain as the subject 
*/
		scNext = sc->next;
		sc->next = NULL;
		if ((rc = psX509AuthenticateCert(pool, sc, ic)) < PS_SUCCESS) {
			sc->next = scNext;
			return rc; 
		}
		sc->next = scNext;
/*
		Lastly, set the subject to the final cert for the real issuer test
*/
		sc = sc->next;
	}
	
/*
	 Now loop through the issuer certs and see if we can authenticate this chain
	 
	 If subject cert was a chain, that has already been authenticated above so
	 we only need to pass in the single parent-most cert to be tested against
*/
	ic = issuerCerts;
	while (ic != NULL) {
		icNext = ic->next;
		ic->next = NULL;
		if ((rc = psX509AuthenticateCert(pool, sc, ic)) == PS_SUCCESS) {
			ic->next = icNext;
			return rc;
		} else if (rc == PS_MEM_FAIL) {
/*
			OK to fail on the authentication because there may be a list here
			but MEM failures prevent us from continuing at all.
*/
			return rc;
		}
		ic->next = icNext;
		ic = ic->next;
	}
/*
	Success would have returned if it happen
*/
	return PS_CERT_AUTH_FAIL;
}
#endif /* USE_CLIENT_SIDE_SSL */

/******************************************************************************/
/*
	Calls a user defined callback to allow for manual validation of the
	certificate.
*/
int32 matrixUserCertValidator(ssl_t *ssl, int32 alert, 
							  psX509Cert_t *subjectCert,
							  sslCertCb_t certValidator)
{
	int32			status;

/*
	If there is no callback, return PS_SUCCESS because there has already been
	a test for the case where the certificate did NOT PASS pubkey test
	and a callback does not exist to manually handle.  
	
	It is highly recommended that the user manually verify, but the cert
	material has internally authenticated and the user has implied that
	is sufficient enough.
*/
	if (certValidator == NULL) {
		psTraceInfo("Internal cert auth passed. No user callback registered\n");
		return PS_SUCCESS;
	}
	
/*
	Finally, let the user know what the alert status is and 
	give them the cert material to access.  Any non-zero value in alert
	indicates there is a pending fatal alert.	
	 
	The user can look at authStatus members if they want to examine the cert
	that did not pass.
*/
	if (alert == SSL_ALERT_NONE) {
		status = 0;
	} else {
		status = alert;
	}

/*
	The user callback
*/
	return certValidator(ssl, subjectCert, status);
}

/******************************************************************************/

