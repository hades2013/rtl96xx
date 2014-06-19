/*
 *	cryptolib.h
 *	Release $Name: MATRIXSSL-3-1-3-OPEN $
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

#ifndef _h_PS_CRYPTOLIB
#define _h_PS_CRYPTOLIB

#include "symmetric/symmetric.h"
#include "digest/digest.h"
#include "math/pstm.h"
#include "pubkey/pubkey.h"
#include "keyformat/asn1.h"
#include "keyformat/x509.h"
#include "prng/prng.h"

/******************************************************************************/
/*
	Configuration checks
*/
#ifdef USE_CERT_PARSE
	#ifndef USE_X509
	#error "Must enable USE_X509 if USE_CERT_PARSE is enabled"
	#endif	
	#if !defined(USE_MD5) || !defined(USE_SHA1)
	#error "Both USE_MD5 and USE_SHA1 must be enabled when enabling USE_X509"
	#endif
#endif 

#ifdef USE_HMAC
	#if !defined(USE_MD5) && !defined(USE_SHA1)
	#error "Must enable either MD5 or SHA1 in cryptoConfig.h for HMAC support"
	#endif
#endif

#ifdef USE_PKCS5
	#ifndef USE_MD5
	#error "Enable USE_MD5 in cryptoConfig.h for PKCS5 support"
	#endif
	#ifndef USE_3DES
	#error "Enable USE_3DES in cryptoConfig.h for PKCS5 support"
	#endif
#endif


/******************************************************************************/
/*
	Crypto trace
*/
#ifndef USE_CRYPTO_TRACE
#define psTraceCrypto(x) 
#define psTraceStrCrypto(x, y) 
#define psTraceIntCrypto(x, y)
#define psTracePtrCrypto(x, y)
#else
#define psTraceCrypto(x) _psTrace(x)
#define psTraceStrCrypto(x, y) _psTraceStr(x, y)
#define psTraceIntCrypto(x, y) _psTraceInt(x, y)
#define psTracePtrCrypto(x, y) _psTracePtr(x, y)
#endif /* USE_CRYPTO_TRACE */


/******************************************************************************/
/*
	Helpers
*/
extern int32 psBase64decode(const unsigned char *in,  uint32 len,
                    unsigned char *out, uint32 *outlen);

/******************************************************************************/
/*
    RFC 3279 OID
    PeerSec uses an oid summing mechanism to arrive at these defines.
	The byte values of the OID are summed to produce a "relatively unique" int

    The duplicate defines do not pose a problem as long as they don't 
    exist in the same OID groupings
*/
#ifdef USE_X509
/* Raw digest algorithms */
#define OID_SHA1_ALG			88
#define OID_MD2_ALG				646
#define OID_MD5_ALG				649

/* Signature algorithms */
#define OID_MD2_RSA_SIG			646 
#define OID_MD5_RSA_SIG			648 /* 42.134.72.134.247.13.1.1.4 */
#define OID_SHA1_RSA_SIG		649 /* 42.134.72.134.247.13.1.1.5 */
#define OID_SHA1_ECDSA_SIG		520	/* 42.134.72.206.61.4.1 */
#define OID_SHA224_ECDSA_SIG	523 /* 42.134.72.206.61.4.3.1 */
#define OID_SHA256_ECDSA_SIG	524 /* 42.134.72.206.61.4.3.2 */
#endif /* USE_X509 */

/* Public key algorithms */
#define OID_RSA_KEY_ALG			645 /* 42.134.72.134.247.13.1.1.1 */
#define OID_ECDSA_KEY_ALG		518 /* 42.134.72.206.61.2.1 */

#ifdef USE_PKCS5
#define OID_DES_EDE3_CBC		652 /* 42.134.72.134.247.13.3.7 */

#define OID_PKCS_PBKDF2			660 /* 42.134.72.134.247.13.1.5.12 */
#define OID_PKCS_PBES2			661 /* 42.134.72.134.247.13.1.5.13 */
#endif /* USE_PKCS5 */

/******************************************************************************/

#define	CRYPT_INVALID_KEYSIZE	-21 
#define	CRYPT_INVALID_ROUNDS	-22

/******************************************************************************/
/* 32-bit Rotates */
/******************************************************************************/
#if defined(_MSC_VER)
/******************************************************************************/

/* instrinsic rotate */
#include <stdlib.h>
#pragma intrinsic(_lrotr,_lrotl)
#define ROR(x,n) _lrotr(x,n)
#define ROL(x,n) _lrotl(x,n)

/******************************************************************************/
#elif defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__)) && \
		!defined(INTEL_CC) && !defined(PS_NO_ASM)

static inline unsigned ROL(unsigned word, int i)
{
   asm ("roll %%cl,%0"
      :"=r" (word)
      :"0" (word),"c" (i));
   return word;
}

static inline unsigned ROR(unsigned word, int i)
{
   asm ("rorl %%cl,%0"
      :"=r" (word)
      :"0" (word),"c" (i));
   return word;
}

/******************************************************************************/
#else

/* rotates the hard way */
#define ROL(x, y) \
	( (((unsigned long)(x)<<(unsigned long)((y)&31)) | \
	(((unsigned long)(x)&0xFFFFFFFFUL)>>(unsigned long)(32-((y)&31)))) & \
	0xFFFFFFFFUL)
#define ROR(x, y) \
	( ((((unsigned long)(x)&0xFFFFFFFFUL)>>(unsigned long)((y)&31)) | \
	((unsigned long)(x)<<(unsigned long)(32-((y)&31)))) & 0xFFFFFFFFUL)

#endif /* 32-bit Rotates */
/******************************************************************************/

#ifdef HAVE_NATIVE_INT64
#ifdef _MSC_VER
	#define CONST64(n) n ## ui64
#else
	#define CONST64(n) n ## ULL
#endif
#endif

/******************************************************************************/
/*
 Controls endianess and size of registers.  Leave uncommented to get
 platform neutral [slower] code detect x86-32 machines somewhat
 */
#if (defined(_MSC_VER) && defined(WIN32)) || \
(defined(__GNUC__) && (defined(__DJGPP__) || defined(__CYGWIN__) || \
defined(__MINGW32__) || defined(__i386__)))
#define ENDIAN_LITTLE
#define ENDIAN_32BITWORD
#endif

/* #define ENDIAN_LITTLE */
/* #define ENDIAN_BIG */

/* #define ENDIAN_32BITWORD */
/* #define ENDIAN_64BITWORD */

#if (defined(ENDIAN_BIG) || defined(ENDIAN_LITTLE)) && \
!(defined(ENDIAN_32BITWORD) || defined(ENDIAN_64BITWORD))
#error You must specify a word size as well as endianess
#endif

#if !(defined(ENDIAN_BIG) || defined(ENDIAN_LITTLE))
#define ENDIAN_NEUTRAL
#endif

/*
 helper macros
 */
#if defined (ENDIAN_NEUTRAL)

#define STORE32L(x, y) { \
(y)[3] = (unsigned char)(((x)>>24)&255); \
(y)[2] = (unsigned char)(((x)>>16)&255);  \
(y)[1] = (unsigned char)(((x)>>8)&255); \
(y)[0] = (unsigned char)((x)&255); \
}

#define LOAD32L(x, y) { \
x = ((unsigned long)((y)[3] & 255)<<24) | \
((unsigned long)((y)[2] & 255)<<16) | \
((unsigned long)((y)[1] & 255)<<8)  | \
((unsigned long)((y)[0] & 255)); \
}

#define STORE64L(x, y) { \
(y)[7] = (unsigned char)(((x)>>56)&255); \
(y)[6] = (unsigned char)(((x)>>48)&255); \
(y)[5] = (unsigned char)(((x)>>40)&255); \
(y)[4] = (unsigned char)(((x)>>32)&255); \
(y)[3] = (unsigned char)(((x)>>24)&255); \
(y)[2] = (unsigned char)(((x)>>16)&255); \
(y)[1] = (unsigned char)(((x)>>8)&255); \
(y)[0] = (unsigned char)((x)&255); \
}

#define LOAD64L(x, y) { \
x = (((ulong64)((y)[7] & 255))<<56)|(((ulong64)((y)[6] & 255))<<48)| \
(((ulong64)((y)[5] & 255))<<40)|(((ulong64)((y)[4] & 255))<<32)| \
(((ulong64)((y)[3] & 255))<<24)|(((ulong64)((y)[2] & 255))<<16)| \
(((ulong64)((y)[1] & 255))<<8)|(((ulong64)((y)[0] & 255))); \
}

#define STORE32H(x, y) { \
(y)[0] = (unsigned char)(((x)>>24)&255); \
(y)[1] = (unsigned char)(((x)>>16)&255); \
(y)[2] = (unsigned char)(((x)>>8)&255); \
(y)[3] = (unsigned char)((x)&255); \
}

#define LOAD32H(x, y) { \
x = ((unsigned long)((y)[0] & 255)<<24) | \
((unsigned long)((y)[1] & 255)<<16) | \
((unsigned long)((y)[2] & 255)<<8)  | \
((unsigned long)((y)[3] & 255)); \
}

#define STORE64H(x, y) { \
(y)[0] = (unsigned char)(((x)>>56)&255); \
(y)[1] = (unsigned char)(((x)>>48)&255); \
(y)[2] = (unsigned char)(((x)>>40)&255); \
(y)[3] = (unsigned char)(((x)>>32)&255); \
(y)[4] = (unsigned char)(((x)>>24)&255); \
(y)[5] = (unsigned char)(((x)>>16)&255); \
(y)[6] = (unsigned char)(((x)>>8)&255); \
(y)[7] = (unsigned char)((x)&255); \
}

#define LOAD64H(x, y) { \
x = (((ulong64)((y)[0] & 255))<<56)|(((ulong64)((y)[1] & 255))<<48) | \
(((ulong64)((y)[2] & 255))<<40)|(((ulong64)((y)[3] & 255))<<32) | \
(((ulong64)((y)[4] & 255))<<24)|(((ulong64)((y)[5] & 255))<<16) | \
(((ulong64)((y)[6] & 255))<<8)|(((ulong64)((y)[7] & 255))); \
}

#endif /* ENDIAN_NEUTRAL */

#ifdef ENDIAN_LITTLE

#define STORE32H(x, y) { \
(y)[0] = (unsigned char)(((x)>>24)&255); \
(y)[1] = (unsigned char)(((x)>>16)&255); \
(y)[2] = (unsigned char)(((x)>>8)&255); \
(y)[3] = (unsigned char)((x)&255); \
}

#define LOAD32H(x, y) { \
x = ((unsigned long)((y)[0] & 255)<<24) | \
((unsigned long)((y)[1] & 255)<<16) | \
((unsigned long)((y)[2] & 255)<<8)  | \
((unsigned long)((y)[3] & 255)); \
}

#define STORE64H(x, y) { \
(y)[0] = (unsigned char)(((x)>>56)&255); \
(y)[1] = (unsigned char)(((x)>>48)&255); \
(y)[2] = (unsigned char)(((x)>>40)&255); \
(y)[3] = (unsigned char)(((x)>>32)&255); \
(y)[4] = (unsigned char)(((x)>>24)&255); \
(y)[5] = (unsigned char)(((x)>>16)&255); \
(y)[6] = (unsigned char)(((x)>>8)&255); \
(y)[7] = (unsigned char)((x)&255); \
}

#define LOAD64H(x, y) { \
x = (((ulong64)((y)[0] & 255))<<56)|(((ulong64)((y)[1] & 255))<<48) | \
(((ulong64)((y)[2] & 255))<<40)|(((ulong64)((y)[3] & 255))<<32) | \
(((ulong64)((y)[4] & 255))<<24)|(((ulong64)((y)[5] & 255))<<16) | \
(((ulong64)((y)[6] & 255))<<8)|(((ulong64)((y)[7] & 255))); }

#ifdef ENDIAN_32BITWORD 

#define STORE32L(x, y) { \
unsigned long __t = (x); memcpy(y, &__t, 4); \
}

#define LOAD32L(x, y)  memcpy(&(x), y, 4);

#define STORE64L(x, y) { \
(y)[7] = (unsigned char)(((x)>>56)&255); \
(y)[6] = (unsigned char)(((x)>>48)&255); \
(y)[5] = (unsigned char)(((x)>>40)&255); \
(y)[4] = (unsigned char)(((x)>>32)&255); \
(y)[3] = (unsigned char)(((x)>>24)&255); \
(y)[2] = (unsigned char)(((x)>>16)&255); \
(y)[1] = (unsigned char)(((x)>>8)&255); \
(y)[0] = (unsigned char)((x)&255); \
}

#define LOAD64L(x, y) { \
x = (((ulong64)((y)[7] & 255))<<56)|(((ulong64)((y)[6] & 255))<<48)| \
(((ulong64)((y)[5] & 255))<<40)|(((ulong64)((y)[4] & 255))<<32)| \
(((ulong64)((y)[3] & 255))<<24)|(((ulong64)((y)[2] & 255))<<16)| \
(((ulong64)((y)[1] & 255))<<8)|(((ulong64)((y)[0] & 255))); \
}

#else /* 64-bit words then  */

#define STORE32L(x, y) \
{ unsigned long __t = (x); memcpy(y, &__t, 4); }

#define LOAD32L(x, y) \
{ memcpy(&(x), y, 4); x &= 0xFFFFFFFF; }

#define STORE64L(x, y) \
{ ulong64 __t = (x); memcpy(y, &__t, 8); }

#define LOAD64L(x, y) \
{ memcpy(&(x), y, 8); }

#endif /* ENDIAN_64BITWORD */
#endif /* ENDIAN_LITTLE */

#ifdef ENDIAN_BIG

#define STORE32L(x, y) { \
(y)[3] = (unsigned char)(((x)>>24)&255); \
(y)[2] = (unsigned char)(((x)>>16)&255); \
(y)[1] = (unsigned char)(((x)>>8)&255); \
(y)[0] = (unsigned char)((x)&255); \
}

#define LOAD32L(x, y) { \
x = ((unsigned long)((y)[3] & 255)<<24) | \
((unsigned long)((y)[2] & 255)<<16) | \
((unsigned long)((y)[1] & 255)<<8)  | \
((unsigned long)((y)[0] & 255)); \
}

#define STORE64L(x, y) { \
(y)[7] = (unsigned char)(((x)>>56)&255); \
(y)[6] = (unsigned char)(((x)>>48)&255); \
(y)[5] = (unsigned char)(((x)>>40)&255); \
(y)[4] = (unsigned char)(((x)>>32)&255); \
(y)[3] = (unsigned char)(((x)>>24)&255); \
(y)[2] = (unsigned char)(((x)>>16)&255); \
(y)[1] = (unsigned char)(((x)>>8)&255); \
(y)[0] = (unsigned char)((x)&255); \
}

#define LOAD64L(x, y) { \
x = (((ulong64)((y)[7] & 255))<<56)|(((ulong64)((y)[6] & 255))<<48) | \
(((ulong64)((y)[5] & 255))<<40)|(((ulong64)((y)[4] & 255))<<32) | \
(((ulong64)((y)[3] & 255))<<24)|(((ulong64)((y)[2] & 255))<<16) | \
(((ulong64)((y)[1] & 255))<<8)|(((ulong64)((y)[0] & 255))); \
}

#ifdef ENDIAN_32BITWORD 

#define STORE32H(x, y) \
{ unsigned long __t = (x); memcpy(y, &__t, 4); }

#define LOAD32H(x, y) memcpy(&(x), y, 4);

#define STORE64H(x, y) { \
(y)[0] = (unsigned char)(((x)>>56)&255); \
(y)[1] = (unsigned char)(((x)>>48)&255); \
(y)[2] = (unsigned char)(((x)>>40)&255); \
(y)[3] = (unsigned char)(((x)>>32)&255); \
(y)[4] = (unsigned char)(((x)>>24)&255); \
(y)[5] = (unsigned char)(((x)>>16)&255); \
(y)[6] = (unsigned char)(((x)>>8)&255); \
(y)[7] = (unsigned char)((x)&255); \
}

#define LOAD64H(x, y) { \
x = (((ulong64)((y)[0] & 255))<<56)|(((ulong64)((y)[1] & 255))<<48)| \
(((ulong64)((y)[2] & 255))<<40)|(((ulong64)((y)[3] & 255))<<32)| \
(((ulong64)((y)[4] & 255))<<24)|(((ulong64)((y)[5] & 255))<<16)| \
(((ulong64)((y)[6] & 255))<<8)| (((ulong64)((y)[7] & 255))); \
}

#else /* 64-bit words then  */

#define STORE32H(x, y) \
{ unsigned long __t = (x); memcpy(y, &__t, 4); }

#define LOAD32H(x, y) \
{ memcpy(&(x), y, 4); x &= 0xFFFFFFFF; }

#define STORE64H(x, y) \
{ ulong64 __t = (x); memcpy(y, &__t, 8); }

#define LOAD64H(x, y) \
{ memcpy(&(x), y, 8); }

#endif /* ENDIAN_64BITWORD */
#endif /* ENDIAN_BIG */
/******************************************************************************/


/******************************************************************************/
/*
	Return the length of padding bytes required for a record of 'LEN' bytes
	The name Pwr2 indicates that calculations will work with 'BLOCKSIZE'
	that are powers of 2.
	Because of the trailing pad length byte, a length that is a multiple
	of the pad bytes
*/
#define psPadLenPwr2(LEN, BLOCKSIZE) \
	BLOCKSIZE <= 1 ? (unsigned char)0 : \
	(unsigned char)(BLOCKSIZE - ((LEN) & (BLOCKSIZE - 1)))

#endif /* _h_PS_CRYPTOLIB */
/******************************************************************************/

