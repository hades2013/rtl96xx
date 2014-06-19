/*
 *	pkcs.c
 *	Release $Name: MATRIXSSL-3-1-3-OPEN $
 *
 *	Collection of RSA PKCS standards 
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

#include "../cryptoApi.h"

/******************************************************************************/
/*
    Pad a value to be encrypted by RSA, according to PKCS#1 v1.5
    http://www.rsasecurity.com/rsalabs/pkcs/pkcs-1/
    When encrypting a value with RSA, the value is first padded to be
    equal to the public key size using the following method:
        00 <id> <data> 00 <value to be encrypted>
    - id denotes a public or private key operation
    - if id is private, data is however many non-zero bytes it takes to pad the
        value to the key length (randomLen = keyLen - 3 - valueLen).
    - if id is public, data is FF for the same length as described above
    - There must be at least 8 bytes of data.
*/
int32 pkcs1Pad(unsigned char *in, uint32 inlen, unsigned char *out,
                       uint32 outlen, int32 cryptType)
{
    unsigned char   *c;
    int32           randomLen;

    randomLen = outlen - 3 - inlen;
    if (randomLen < 8) {
		psTraceCrypto("pkcs1Pad failure\n");
        return PS_LIMIT_FAIL;
    }
    c = out;
    *c = 0x00;
    c++;
    *c = (unsigned char)cryptType;
    c++;
    if (cryptType == PUBKEY_TYPE) {
        while (randomLen-- > 0) {
            *c++ = 0xFF;
        }
    } else {
        if (psGetPrng(NULL, c, (uint32)randomLen) < 0) {
            return PS_PLATFORM_FAIL;
        }
/*
        SECURITY:  Read through the random data and change all 0x0 to 0x01.
        This is per spec that no random bytes should be 0
*/
        while (randomLen-- > 0) {
            if (*c == 0x0) {
                *c = 0x01;
            }
            c++;
        }
    }
    *c = 0x00;
    c++;
    memcpy(c, in, inlen);

    return outlen;
}

/******************************************************************************/
/*
    Unpad a value decrypted by RSA, according to PKCS#1 v1.5
    http://www.rsasecurity.com/rsalabs/pkcs/pkcs-1/
   
    When decrypted, the data will look like the pad, including the inital
    byte (00).  Form:
        00 <decryptType> <random data (min 8 bytes)> 00 <value to be encrypted>

    We don't worry about v2 rollback issues because we don't support v2
*/
int32 pkcs1Unpad(unsigned char *in, uint32 inlen, unsigned char *out,
                        uint32 outlen, int32 decryptType)
{
    unsigned char   *c, *end;

    if (inlen < outlen + 10) {
		psTraceCrypto("pkcs1Unpad failure\n");
        return PS_ARG_FAIL;
    }
    c = in;
    end = in + inlen;
/*
    Verify the first byte (block type) is correct.
*/
    if (*c++ != 0x00 || *c != decryptType) {
		psTraceCrypto("pkcs1Unpad parse failure\n");
        return PS_FAILURE;
    }
    c++;
/*
    Skip over the random, non-zero bytes used as padding
*/
    while (c < end && *c != 0x0) {
        if (decryptType == PUBKEY_TYPE) {
            if (*c != 0xFF) {
				psTraceCrypto("pkcs1Unpad pubkey parse failure\n");
                return PS_FAILURE;
            }
        }
        c++;
    }
    c++;
/*
    The length of the remaining data should be equal to what was expected
    Combined with the initial length check, there must be >= 8 bytes of pad
    ftp://ftp.rsa.com/pub/pdfs/bulletn7.pdf
*/
    if ((uint32)(end - c) != outlen) {
		psTraceCrypto("pkcs1Unpad verification failure\n");
        return PS_LIMIT_FAIL;
    }
/*
    Copy the value bytes to the out buffer
*/
    while (c < end) {
        *out = *c;
        out++; c++;
    }
    return outlen;
}

#ifdef USE_PRIVATE_KEY_PARSING
#ifdef USE_RSA
/******************************************************************************/
/*
	Parse a a private key structure in DER formatted ASN.1
	Per ftp://ftp.rsasecurity.com/pub/pkcs/pkcs-1/pkcs-1v2-1.pdf
	RSAPrivateKey ::= SEQUENCE {
		version Version,
		modulus INTEGER, -- n
		publicExponent INTEGER, -- e
		privateExponent INTEGER, -- d
		prime1 INTEGER, -- p
		prime2 INTEGER, -- q
		exponent1 INTEGER, -- d mod (p-1)
		exponent2 INTEGER, -- d mod (q-1)
		coefficient INTEGER, -- (inverse of q) mod p
		otherPrimeInfos OtherPrimeInfos OPTIONAL
	}
	Version ::= INTEGER { two-prime(0), multi(1) }
	  (CONSTRAINED BY {-- version must be multi if otherPrimeInfos present --})

	Which should look something like this in hex (pipe character 
	is used as a delimiter):
	ftp://ftp.rsa.com/pub/pkcs/ascii/layman.asc
	30	Tag in binary: 00|1|10000 -> UNIVERSAL | CONSTRUCTED | SEQUENCE (16)
	82	Length in binary: 1 | 0000010 -> LONG LENGTH | LENGTH BYTES (2)
	04 A4	Length Bytes (1188)
	02	Tag in binary: 00|0|00010 -> UNIVERSAL | PRIMITIVE | INTEGER (2)
	01	Length in binary: 0|0000001 -> SHORT LENGTH | LENGTH (1)
	00	INTEGER value (0) - RSAPrivateKey.version
	02	Tag in binary: 00|0|00010 -> UNIVERSAL | PRIMITIVE | INTEGER (2)
	82	Length in binary: 1 | 0000010 -> LONG LENGTH | LENGTH BYTES (2)
	01 01	Length Bytes (257)
	[]	257 Bytes of data - RSAPrivateKey.modulus (2048 bit key)
	02	Tag in binary: 00|0|00010 -> UNIVERSAL | PRIMITIVE | INTEGER (2)
	03	Length in binary: 0|0000011 -> SHORT LENGTH | LENGTH (3)
	01 00 01	INTEGER value (65537) - RSAPrivateKey.publicExponent
	...

	OtherPrimeInfos is not supported in this routine, and an error will be
	returned if they are present
*/
int32 pkcs1ParsePrivBin(psPool_t *pool, unsigned char *p,
				uint32 size, psPubKey_t **pubkey)
{
	unsigned char	*end, *seq;
	psRsaKey_t		*key;
	int32			version;
	uint32			seqlen;

	*pubkey = psNewPubKey(pool);
	if (*pubkey == NULL) {
		psError("Memory allocation error in pkcs1ParsePrivBin\n");
		return PS_MEM_FAIL;
	}
	(*pubkey)->type = PS_RSA; /* PKCS1 is always RSA */
	key = &((*pubkey)->key->rsa);
	key->optimized = 0;
	end = p + size;
	if (getAsnSequence(&p, size, &seqlen) < 0) {
		psFreePubKey(*pubkey);
		return PS_PARSE_FAIL;
	}
	seq = p;
	if (getAsnInteger(&p, (uint32)(end - p), &version) < 0 || version != 0 ||
		getAsnBig(pool, &p, (uint32)(end - p), &(key->N)) < 0 ||
		getAsnBig(pool, &p, (uint32)(end - p), &(key->e)) < 0 ||
		getAsnBig(pool, &p, (uint32)(end - p), &(key->d)) < 0 ||
		getAsnBig(pool, &p, (uint32)(end - p), &(key->p)) < 0 ||
		getAsnBig(pool, &p, (uint32)(end - p), &(key->q)) < 0 ||
		getAsnBig(pool, &p, (uint32)(end - p), &(key->dP)) < 0 ||
		getAsnBig(pool, &p, (uint32)(end - p), &(key->dQ)) < 0 ||
		getAsnBig(pool, &p, (uint32)(end - p), &(key->qP)) < 0 ||
			(uint32)(p - seq) != seqlen) {
		psTraceCrypto("ASN RSA private key extract parse error\n");
		psFreePubKey(*pubkey);
		return PS_PARSE_FAIL;
	}
/*
	 If we made it here, the key is ready for optimized decryption
	 Set the key length of the key
 */
	key->optimized = 1;
	key->size = pstm_unsigned_bin_size(&key->N);	
	(*pubkey)->keysize = key->size;
	return PS_SUCCESS;
}
#endif /* USE_RSA */


#ifdef USE_FILE_SYSTEM
static const char encryptHeader[] = "DEK-Info: DES-EDE3-CBC,";

#ifdef USE_PKCS5
/******************************************************************************/
/*
	Convert an ASCII hex representation to a binary buffer.
	Decode enough data out of 'hex' buffer to produce 'binlen' bytes in 'bin'
	Two digits of ASCII hex map to the high and low nybbles (in that order),
	so this function assumes that 'hex' points to 2x 'binlen' bytes of data.
	Return the number of bytes processed from hex (2x binlen) or < 0 on error.
*/
static int32 hexToBinary(unsigned char *hex, unsigned char *bin, int32 binlen)
{
	unsigned char	*end, c, highOrder;

	highOrder = 1;
	for (end = hex + binlen * 2; hex < end; hex++) {
		c = *hex;
		if ('0' <= c && c <='9') {
			c -= '0';
		} else if ('a' <= c && c <='f') {
			c -= ('a' - 10);
		} else if ('A' <= c && c <='F') {
			c -= ('A' - 10);
		} else {
			psTraceCrypto("hexToBinary failure\n");
			return PS_FAILURE;
		}
		if (highOrder++ & 0x1) {
			*bin = c << 4;
		} else {
			*bin |= c;
			bin++;
		}
	}
	return binlen * 2;
}
#endif /* USE_PKCS5 */

/******************************************************************************/
/*
	File must be a PEM format RSA keys
	Memory info:
		Caller must call psFreePubKey on outkey on function success
		Caller does not need to free outkey on function failure
*/
int32 pkcs1ParsePrivFile(psPool_t *pool, char *fileName, char *password,
			psPubKey_t **outkey)
{
	unsigned char	*DERout;
	int32			rc;
	uint32			DERlen;
	psPubKey_t		*rsakey;

	*outkey = NULL;

	if ((rc = pkcs1DecodePrivFile(pool, fileName, password, &DERout, &DERlen))
			< PS_SUCCESS) {
		return rc;
	}

	if ((rc = pkcs1ParsePrivBin(pool, DERout, DERlen, &rsakey)) < 0) {
		psFree(DERout);
		return rc;
	}
	
	psFree(DERout);
	*outkey = rsakey;
	return PS_SUCCESS;
}

/******************************************************************************/
/*
	Return the DER stream from a private key PEM file
	
	Memory info:
		Caller must call psFree on DERout on function success
*/
int32 pkcs1DecodePrivFile(psPool_t *pool, char *fileName, char *password,
			unsigned char **DERout, uint32 *DERlen)
{
	unsigned char	*keyBuf;
	char			*start, *end, *endTmp;
	int32			keyBufLen, rc;
	uint32			PEMlen = 0;
#ifdef USE_PKCS5
	psCipherContext_t	ctx;
	unsigned char	passKey[DES3_KEY_LEN];
	unsigned char	cipherIV[DES3_IV_LEN];
	int32			tmp, encrypted = 0;
#endif /* USE_PKCS5 */

	if (fileName == NULL) {
		psTraceCrypto("No fileName passed to pkcs1ParsePrivFile\n");
		return PS_ARG_FAIL;
	}
	if ((rc = psGetFileBuf(pool, fileName, &keyBuf, &keyBufLen)) < PS_SUCCESS) {
		return rc;
	}
	start = end = NULL;

/*
 *	Check header and encryption parameters.
 */
	if (((start = strstr((char*)keyBuf, "-----BEGIN")) != NULL) && 
			((start = strstr((char*)keyBuf, "PRIVATE KEY-----")) != NULL) &&
			((end = strstr(start, "-----END")) != NULL) &&
			((endTmp = strstr(end, "PRIVATE KEY-----")) != NULL)) {
		start += strlen("PRIVATE KEY-----");
		while (*start == '\r' || *start == '\n') {
			start++;
		}
		PEMlen = (uint32)(end - start);
	} else {
		psTraceCrypto("File buffer does not look to be in PKCS#1 PEM format\n");
		psFree(keyBuf);
		return PS_PARSE_FAIL;
	}

	if (strstr((char*)keyBuf, "Proc-Type:") &&
			strstr((char*)keyBuf, "4,ENCRYPTED")) {
#ifdef USE_PKCS5
		encrypted++;
		if (password == NULL) {
			psTraceCrypto("No password given for encrypted private key file\n");
			psFree(keyBuf);
			return PS_ARG_FAIL;
		}
		if ((start = strstr((char*)keyBuf, encryptHeader)) == NULL) {
			psTraceCrypto("Unrecognized private key file encoding\n");
			psFree(keyBuf);
			return PS_PARSE_FAIL;
		}
		start += strlen(encryptHeader);
		/* we assume here that header points to at least 16 bytes of data */
		tmp = hexToBinary((unsigned char*)start, cipherIV, DES3_IV_LEN);
		if (tmp < 0) {
			psTraceCrypto("Invalid private key file salt\n");
			psFree(keyBuf);
			return PS_FAILURE;
		}
		start += tmp;
		pkcs5pbkdf1((unsigned char*)password, strlen(password),
			cipherIV, 1, (unsigned char*)passKey);
		PEMlen = (int32)(end - start);
#else  /* !USE_PKCS5 */
/*
 *		The private key is encrypted, but PKCS5 support has been turned off
 */
		psTraceCrypto("USE_PKCS5 must be enabled for key file password\n");
		psFree(keyBuf);
		return PS_UNSUPPORTED_FAIL;  
#endif /* USE_PKCS5 */
	}

/*
	Take the raw input and do a base64 decode
 */
	*DERout = psMalloc(pool, PEMlen);
	if (*DERout == NULL) {
		psFree(keyBuf);
		psError("Memory allocation error in pkcs1ParsePrivFile\n");
		return PS_MEM_FAIL;
	}
	*DERlen = PEMlen;
	if ((rc = psBase64decode((unsigned char*)start, PEMlen, *DERout,
			DERlen)) != 0) {
		psFree(*DERout);
		psFree(keyBuf);
		return rc;
	}
	psFree(keyBuf);
	
#ifdef USE_PKCS5
/*
 *	Decode
 */
	if (encrypted == 1 && password) {
		psDes3Init(&ctx, cipherIV, passKey, DES3_KEY_LEN);
		psDes3Decrypt(&ctx, *DERout, *DERout, *DERlen);
	}
#endif /* USE_PKCS5 */

	return PS_SUCCESS;
}

#endif /* USE_FILE_SYSTEM */
#endif /* USE_PRIVATE_KEY_PARSING */
/******************************************************************************/

/******************************************************************************/
#ifdef USE_PKCS5
/******************************************************************************/
/*
 Generate a key given a password and salt value.
 PKCS#5 2.0 PBKDF1 key derivation format with MD5 and count == 1 per:
 http://www.rsasecurity.com/rsalabs/pkcs/pkcs-5/index.html
 
 This key is compatible with the algorithm used by OpenSSL to encrypt keys
 generated with 'openssl genrsa'.  If other encryption formats are used
 (for example PBKDF2), or an iteration count != 1 is used, they are not 
 compatible with this simple implementation.  OpenSSL provides many options
 for converting key formats to the one used here.
 
 A 3DES key is 24 bytes long, to generate it with this algorithm,
 we md5 hash the password and salt for the first 16 bytes.  We then 
 hash these first 16 bytes with the password and salt again, generating 
 another 16 bytes.  We take the first 16 bytes and 8 of the second 16 to 
 form the 24 byte key.
 
 salt is assumed to point to 8 bytes of data
 key is assumed to point to 24 bytes of data
 */
void pkcs5pbkdf1(unsigned char *pass, uint32 passlen, unsigned char *salt,
				 int32 iter, unsigned char *key)
{
	psDigestContext_t	state;
	unsigned char		md5[MD5_HASH_SIZE];
	
	psAssert(iter == 1);
	
	psMd5Init(&state);
	psMd5Update(&state, pass, passlen);
	psMd5Update(&state, salt, 8);
	psMd5Final(&state, md5);
	memcpy(key, md5, MD5_HASH_SIZE);
	
	psMd5Init(&state);
	psMd5Update(&state, md5, MD5_HASH_SIZE);
	psMd5Update(&state, pass, passlen);
	psMd5Update(&state, salt, 8);
	psMd5Final(&state, md5);
	memcpy(key + MD5_HASH_SIZE, md5, 24 - MD5_HASH_SIZE);
	
	memset(md5, 0x0, sizeof(md5));	/* zero out sensitive info */
}

#endif /* USE_PKCS5 */


/******************************************************************************/
/******************************************************************************/

/******************************************************************************/

