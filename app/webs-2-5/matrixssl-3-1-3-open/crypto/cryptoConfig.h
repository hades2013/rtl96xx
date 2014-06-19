/*
 *	cryptoConfig.h
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

#ifndef _h_PS_CRYPTOCONFIG
#define _h_PS_CRYPTOCONFIG

/******************************************************************************/
/* Configurable features */
/******************************************************************************/
/*
	Enable psTraceCrypto family of APIs for debugging the crypto module
*/
/* #define USE_CRYPTO_TRACE */

/******************************************************************************/
/*
	Public-Key Algorithms	
*/
#define USE_RSA

/******************************************************************************/
/*
	Symmetric bock ciphers (including CBC mode)
*/

#define USE_AES
#define USE_3DES
/* #define USE_DES */

/******************************************************************************/
/*
	Symmetric stream ciphers
*/
/* #define USE_ARC4 */

/******************************************************************************/
/*
	Digest algorithms	
*/
#define USE_SHA1
#define USE_MD5

#define USE_HMAC /* Requires USE_MD5 and/or USE_SHA1 */

/******************************************************************************/
/*
	X.509 Certificate
*/
#define USE_X509
#define USE_CERT_PARSE /* Usually required.  USE_X509 must be enabled */
/* #define USE_FULL_CERT_PARSE */ /* USE_CERT_PARSE must be enabled */

#define USE_BASE64_DECODE

/******************************************************************************/
/*
	PKCS support
*/
#define USE_PRIVATE_KEY_PARSING
/* #define USE_PKCS5 */ /* PBKDF key generation for encrypted private keys */

/******************************************************************************/
/*
  	PRNG Algorithms 
*/
#define USE_YARROW

/******************************************************************************/
/*
    All below here are configurable tweaks (do not need to touch, in general)
*/
#define SMALL_CODE
#define USE_BURN_STACK

#endif /* _h_PS_CRYPTOCONFIG */
/******************************************************************************/

