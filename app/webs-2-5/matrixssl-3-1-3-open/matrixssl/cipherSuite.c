/*
 *	cipherSuite.c
 *	Release $Name: MATRIXSSL-3-1-3-OPEN $
 *
 *	Wrappers for the various cipher suites.
 *	Enable specific suites at compile time in matrixsslConfig.h
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
/*	Symmetric cipher initializtion wrappers for cipher suites */
/******************************************************************************/
/*
	SSL_NULL_WITH_NULL_NULL cipher functions
	Used in handshaking before SSL_RECORD_TYPE_CHANGE_CIPHER_SPEC message
*/
static int32 csNullInit(sslSec_t *sec, int32 type, uint32 keysize)
{
	return 0;
}

/******************************************************************************/
#ifdef USE_ARC4_CIPHER_SUITE
/******************************************************************************/
static int32 csArc4Init(sslSec_t *sec, int32 type, uint32 keysize)
{
	if (type == INIT_ENCRYPT_CIPHER) {
		memset(&(sec->encryptCtx), 0, sizeof(psRc4Key_t));
		psArc4Init(&(sec->encryptCtx), sec->writeKey, keysize);
	} else {
		memset(&(sec->decryptCtx), 0, sizeof(psRc4Key_t));
		psArc4Init(&(sec->decryptCtx), sec->readKey, keysize);
	}
	return PS_SUCCESS;
}
#endif /* USE_ARC4_CIPHER_SUITE */
/******************************************************************************/

/******************************************************************************/
#ifdef USE_3DES_CIPHER_SUITE
/******************************************************************************/
static int32 csDes3Init(sslSec_t *sec, int32 type, uint32 keysize)
{
	int32	err;
	
	psAssert(keysize == DES3_KEY_LEN);
	
	if (type == INIT_ENCRYPT_CIPHER) {
		memset(&(sec->encryptCtx), 0, sizeof(des3_CBC));	
		if ((err = psDes3Init(&(sec->encryptCtx), sec->writeIV, sec->writeKey, 
							  DES3_KEY_LEN)) < 0) {
			return err;
		}
	} else {
		memset(&(sec->decryptCtx), 0, sizeof(des3_CBC));
		if ((err = psDes3Init(&(sec->decryptCtx), sec->readIV, sec->readKey, 
							  DES3_KEY_LEN)) < 0) {
			return err;
		}
	}
	return PS_SUCCESS;
}
#endif /* USE_3DES_CIPHER_SUITE */
/******************************************************************************/


#ifdef USE_AES_CIPHER_SUITE
/******************************************************************************/
static int32 csAesInit(sslSec_t *sec, int32 type, uint32 keysize)
{
	int32	err;
	
	if (type == INIT_ENCRYPT_CIPHER) {
		memset(&(sec->encryptCtx), 0, sizeof(aes_CBC));	
		if ((err = psAesInit(&(sec->encryptCtx), sec->writeIV, sec->writeKey,
							 keysize)) < 0) {
			return err;
		}
	} else {
		memset(&(sec->decryptCtx), 0, sizeof(aes_CBC));	
		if ((err = psAesInit(&(sec->decryptCtx), sec->readIV, sec->readKey,
							 keysize)) < 0) {
			return err;
		}
	}
	return PS_SUCCESS;
}
#endif /* USE_AES_CIPHER_SUITE */
/******************************************************************************/



/******************************************************************************/
/*	Null cipher crypto */
/******************************************************************************/
static int32 csNullEncrypt(psCipherContext_t *ctx, unsigned char *in,
						 unsigned char *out, uint32 len)
{
	if (out != in) {
		memcpy(out, in, len);
	}
	return len;
}

static int32 csNullDecrypt(psCipherContext_t *ctx, unsigned char *in,
						 unsigned char *out, uint32 len)
{
	if (out != in) {
		memmove(out, in, len);
	}
	return len;
}

/******************************************************************************/
/*	HMAC wrappers for cipher suites */
/******************************************************************************/
static int32 csNullGenerateMac(void *ssl, unsigned char type,
						unsigned char *data, uint32 len, unsigned char *mac)
{
	return 0;
}

static int32 csNullVerifyMac(void *ssl, unsigned char type,
						unsigned char *data, uint32 len, unsigned char *mac)
{
	return 0;
}

#ifdef USE_SHA1_MAC
/******************************************************************************/
static int32 csSha1GenerateMac(void *sslv, unsigned char type,
					unsigned char *data, uint32 len, unsigned char *mac)
{
	ssl_t	*ssl = (ssl_t*)sslv;

#ifdef USE_TLS
	if (ssl->flags & SSL_FLAGS_TLS) {
		return tlsHMACSha1(ssl, HMAC_CREATE, type, data, len, mac);
	} else {
#endif /* USE_TLS */
		return ssl3HMACSha1(ssl->sec.writeMAC, ssl->sec.seq, type, data,
				len, mac);
#ifdef USE_TLS
	}
#endif /* USE_TLS */
}

static int32 csSha1VerifyMac(void *sslv, unsigned char type,
					unsigned char *data, uint32 len, unsigned char *mac)
{
	unsigned char	buf[SHA1_HASH_SIZE];
	ssl_t	*ssl = (ssl_t*)sslv;
	
#ifdef USE_TLS
	if (ssl->flags & SSL_FLAGS_TLS) {
		tlsHMACSha1(ssl, HMAC_VERIFY, type, data, len, buf);
	} else {
#endif /* USE_TLS */
		ssl3HMACSha1(ssl->sec.readMAC, ssl->sec.remSeq, type, data, len, buf);
#ifdef USE_TLS
	}
#endif /* USE_TLS */
	if (memcmp(buf, mac, SHA1_HASH_SIZE) == 0) {
		return PS_SUCCESS;
	}
	return PS_FAILURE;
}
#endif /* USE_SHA1_MAC */
/******************************************************************************/

/******************************************************************************/
#ifdef USE_MD5_MAC
/******************************************************************************/
static int32 csMd5GenerateMac(void *sslv, unsigned char type,
					unsigned char *data, uint32 len, unsigned char *mac)
{
	ssl_t	*ssl = (ssl_t*)sslv;
#ifdef USE_TLS
	if (ssl->flags & SSL_FLAGS_TLS) {
		return tlsHMACMd5(ssl, HMAC_CREATE, type, data, len, mac);
	} else {
#endif /* USE_TLS */
		return ssl3HMACMd5(ssl->sec.writeMAC, ssl->sec.seq, type, data,
						   len, mac);
#ifdef USE_TLS
	}
#endif /* USE_TLS */
}

static int32 csMd5VerifyMac(void *sslv, unsigned char type, unsigned char *data,
					uint32 len, unsigned char *mac)
{
	unsigned char	buf[MD5_HASH_SIZE];
	ssl_t	*ssl = (ssl_t*)sslv;
	
#ifdef USE_TLS
	if (ssl->flags & SSL_FLAGS_TLS) {
		tlsHMACMd5(ssl, HMAC_VERIFY, type, data, len, buf);
	} else {
#endif /* USE_TLS */
		ssl3HMACMd5(ssl->sec.readMAC, ssl->sec.remSeq, type, data, len, buf);
#ifdef USE_TLS
	}
#endif /* USE_TLS */
	if (memcmp(buf, mac, MD5_HASH_SIZE) == 0) {
		return PS_SUCCESS;
	}
	return PS_FAILURE;
}
#endif /* USE_MD5_MAC */
/******************************************************************************/

/******************************************************************************/
/*	Public Key operations wrappers for cipher suites */
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int32 csRsaEncryptPub(psPool_t *pool, psPubKey_t *key, 
			unsigned char *in, uint32 inlen, unsigned char *out, uint32 outlen)
{
	psAssert(key->type == PS_RSA);
	return psRsaEncryptPub(pool, (psRsaKey_t*)key->key, in, inlen, out, outlen);
}

int32 csRsaDecryptPub(psPool_t *pool, psPubKey_t *key, 
			unsigned char *in, uint32 inlen, unsigned char *out, uint32 outlen)
{
	psAssert(key->type == PS_RSA);
	return psRsaDecryptPub(pool, (psRsaKey_t*)key->key, in, inlen, out, outlen);
}

int32 csRsaEncryptPriv(psPool_t *pool, psPubKey_t *key, 
			unsigned char *in, uint32 inlen, unsigned char *out, uint32 outlen)
{
	return PS_UNSUPPORTED_FAIL;
}

int32 csRsaDecryptPriv(psPool_t *pool, psPubKey_t *key, 
			unsigned char *in, uint32 inlen, unsigned char *out, uint32 outlen)
{
	psAssert(key->type == PS_RSA);
	return psRsaDecryptPriv(pool, (psRsaKey_t*)key->key, in, inlen, out, outlen);
}
/******************************************************************************/


static sslCipherSpec_t	supportedCiphers[] = {
/*
	New ciphers should be added here, similar to the ones below

	Ciphers are listed in order of greater security at top... this generally
	means the slower ones are on top as well.
*/

#ifdef USE_TLS_RSA_WITH_AES_128_CBC_SHA
	{TLS_RSA_WITH_AES_128_CBC_SHA,
		CS_RSA,
		20,			/* macSize */
		16,			/* keySize */
		16,			/* ivSize */
		16,			/* blocksize */
		csAesInit,
		psAesEncrypt, 
		psAesDecrypt,  
		csSha1GenerateMac, 
		csSha1VerifyMac},
#endif /* USE_TLS_RSA_WITH_AES_128_CBC_SHA */


#ifdef USE_SSL_RSA_WITH_3DES_EDE_CBC_SHA
	{SSL_RSA_WITH_3DES_EDE_CBC_SHA,
		CS_RSA,
		20,			/* macSize */
		24,			/* keySize */
		8,			/* ivSize */
		8,			/* blocksize */
		csDes3Init,
		psDes3Encrypt, 
		psDes3Decrypt,  
		csSha1GenerateMac, 
		csSha1VerifyMac},
#endif /* USE_SSL_RSA_WITH_3DES_EDE_CBC_SHA */

#ifdef USE_SSL_RSA_WITH_RC4_128_SHA
	{SSL_RSA_WITH_RC4_128_SHA,
		CS_RSA,
		20,			/* macSize */
		16,			/* keySize */
		0,			/* ivSize */
		1,			/* blocksize */
		csArc4Init,
		psArc4, 
		psArc4, 
		csSha1GenerateMac, 
		csSha1VerifyMac},
#endif /* USE_SSL_RSA_WITH_RC4_128_SHA */

#ifdef USE_SSL_RSA_WITH_RC4_128_MD5
	{SSL_RSA_WITH_RC4_128_MD5,
		CS_RSA,
		16,			/* macSize */
		16,			/* keySize */
		0,			/* ivSize */
		1,			/* blocksize */
		csArc4Init,
		psArc4, 
		psArc4,  
		csMd5GenerateMac, 
		csMd5VerifyMac},
#endif /* USE_SSL_RSA_WITH_RC4_128_MD5 */


/*
	These two USE_SSL_RSA_WITH_NULL ciphers are not recommended for use
	in production applications.
*/
#ifdef USE_SSL_RSA_WITH_NULL_SHA
	{SSL_RSA_WITH_NULL_SHA,
		CS_RSA,
		20,			/* macSize */
		0,			/* keySize */
		0,			/* ivSize */
		0,			/* blocksize */
		csNullInit,
		csNullEncrypt, 
		csNullDecrypt, 
		csSha1GenerateMac, 
		csSha1VerifyMac},
#endif /* USE_SSL_RSA_WITH_NULL_SHA */

#ifdef USE_SSL_RSA_WITH_NULL_MD5
	{SSL_RSA_WITH_NULL_MD5,
		CS_RSA,
		16,			/* macSize */
		0,			/* keySize */
		0,			/* ivSize */
		0,			/* blocksize */
		csNullInit,
		csNullEncrypt, 
		csNullDecrypt,  
		csMd5GenerateMac, 
		csMd5VerifyMac},
#endif /* USE_SSL_RSA_WITH_NULL_MD5 */

/*
	The NULL Cipher suite must exist and be the last in this list
*/
	{SSL_NULL_WITH_NULL_NULL,
		CS_NULL,
		0,
		0,
		0,
		0,
		csNullInit,
		csNullEncrypt, 
		csNullDecrypt, 
		csNullGenerateMac, 
		csNullVerifyMac}
};


#ifdef VALIDATE_KEY_MATERIAL
/*
	TODO: these currently allow any cert in the chain to return success but
	it would be better to force only the parent-most(?) to be checked
*/
static int32 haveCorrectKeyAlg(psX509Cert_t *cert, int32 keyAlg)
{
	while (cert) {
		if (cert->pubKeyAlgorithm == keyAlg) {
			return PS_SUCCESS;
		}
		cert = cert->next;
	}
	return PS_FAILURE;
}


/******************************************************************************/
/*
	Don't report a matching cipher suite if the user hasn't loaded the 
	proper public key material to support it
 */
static int32 haveKeyMaterial(ssl_t *ssl, int32 cipherType)
{
/*
	Standard RSA ciphers types
*/
	if (cipherType == CS_RSA) {
		if (ssl->flags & SSL_FLAGS_SERVER) {
#ifdef USE_SERVER_SIDE_SSL		
			if (ssl->keys == NULL || ssl->keys->cert == NULL ||
					ssl->keys->privKey == NULL) {
				return PS_FAILURE;
			}
			if (haveCorrectKeyAlg(ssl->keys->cert, OID_RSA_KEY_ALG) < 0) {
				return PS_FAILURE;
			}
#else
			return PS_FAILURE;
#endif /* USE_SERVER_SIDE_SSL */			
			
#ifdef USE_CLIENT_SIDE_SSL
		} else {
			if (ssl->keys == NULL || ssl->keys->CAcerts == NULL) {
				return PS_FAILURE;
			}
			if (haveCorrectKeyAlg(ssl->keys->CAcerts, OID_RSA_KEY_ALG) < 0) {
				return PS_FAILURE;
			}
#endif /* USE_CLIENT_SIDE_SSL */
		}
	}
	


	 
	return PS_SUCCESS;
}
#endif /* VALIDATE_KEY_MATERIAL */

/******************************************************************************/
/*
	Lookup the given cipher spec ID and return a pointer to the structure
	if found.  This is used when negotiating security, to find out what suites
	we support.
*/
sslCipherSpec_t *sslGetCipherSpec(ssl_t *ssl, uint32 id)
{
	int32	i;

	i = 0;
	do {
		if ((int32)supportedCiphers[i].ident == id) {
#ifdef VALIDATE_KEY_MATERIAL
			if (haveKeyMaterial(ssl, supportedCiphers[i].type) == PS_SUCCESS) {
				return &supportedCiphers[i];
			}
			psTraceIntInfo("Matched cipher suite %d but no supporting keys\n",
				id);
#else
			return &supportedCiphers[i];
#endif /* VALIDATE_KEY_MATERIAL */
		}
	} while (supportedCiphers[i++].ident != SSL_NULL_WITH_NULL_NULL) ;

	return NULL;
}


/******************************************************************************/
/*
	Write out a list of the supported cipher suites to the caller's buffer
	First 2 bytes are the number of cipher suite bytes, the remaining bytes are
	the cipher suites, as two byte, network byte order values.
*/
int32 sslGetCipherSpecList(ssl_t *ssl, unsigned char *c, int32 len,
		int32 addScsv)
{
	unsigned char	*end, *p;
	unsigned short	i;
	int32			noSupportingKeys;

	if (len < 4) {
		return -1;
	}
	end = c + len;
	p = c; c += 2;
	
	noSupportingKeys = 0;
	for (i = 0; supportedCiphers[i].ident != SSL_NULL_WITH_NULL_NULL; i++) {
		if (end - c < 2) {
			return -1;
		}
#ifdef VALIDATE_KEY_MATERIAL
		if (haveKeyMaterial(ssl, supportedCiphers[i].type) != PS_SUCCESS) {
			noSupportingKeys += 2;
			continue;
		}
#endif 		
		*c = (unsigned char)((supportedCiphers[i].ident & 0xFF00) >> 8); c++;
		*c = (unsigned char)(supportedCiphers[i].ident & 0xFF); c++;
	}
	i *= 2;
	i -= (unsigned short)noSupportingKeys;
#ifdef ENABLE_SECURE_REHANDSHAKES
	if (addScsv == 1) {
		*c = ((TLS_EMPTY_RENEGOTIATION_INFO_SCSV & 0xFF00) >> 8); c++;
		*c = TLS_EMPTY_RENEGOTIATION_INFO_SCSV  & 0xFF; c++;
		i += 2;
	}
#endif	
	*p = (unsigned char)(i >> 8); p++;
	*p = (unsigned char)(i & 0xFF);
	return i + 2;
}

/******************************************************************************/
/*
	Return the length of the cipher spec list, including initial length bytes,
	(minus any suites that we don't have the key material to support)
*/
int32 sslGetCipherSpecListLen(ssl_t *ssl)
{
	int32	i, noSupportingKeys;

	noSupportingKeys = 0;
	for (i = 0; supportedCiphers[i].ident != SSL_NULL_WITH_NULL_NULL; i++) {
#ifdef VALIDATE_KEY_MATERIAL
		if (haveKeyMaterial(ssl, supportedCiphers[i].type) != PS_SUCCESS) {
			noSupportingKeys += 2;
		}
#endif 	
	}
	return (i * 2) + 2 - noSupportingKeys;
}

/******************************************************************************/

