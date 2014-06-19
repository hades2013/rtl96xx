/*
 *	matrixsslConfig.h
 *	Release $Name: MATRIXSSL-3-1-3-OPEN $
 *
 *	Configuration settings for building the MatrixSSL library.
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

#ifndef _h_MATRIXSSLCONFIG
#define _h_MATRIXSSLCONFIG


#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/*
	Recommended cipher suites:

	Define the following to enable various cipher suites
	At least one of these must be defined.  If multiple are defined,
	the handshake will determine which is best for the connection.
*/
#define USE_TLS_RSA_WITH_AES_128_CBC_SHA
#define USE_SSL_RSA_WITH_3DES_EDE_CBC_SHA
/* #define USE_SSL_RSA_WITH_RC4_128_SHA */
/* #define USE_SSL_RSA_WITH_RC4_128_MD5 */

/******************************************************************************/
/*
	These cipher suites do not combine authentication and encryption and
	are not recommended for use-cases that require strong security
*/
/* #define USE_SSL_RSA_WITH_NULL_SHA */
/* #define USE_SSL_RSA_WITH_NULL_MD5 */

/******************************************************************************/
/*
	Support for TLS protocols.
	TLS versions must 'stack' (can't support 1.1 without 1.0)
*/
#define USE_TLS			/* TLS 1.0 */

/******************************************************************************/
/*
	Compile time support for server or client side SSL
*/
#define USE_CLIENT_SIDE_SSL
#define USE_SERVER_SIDE_SSL

/******************************************************************************/
/*
	The initial buffer size for send and receive buffers in each ssl_t session.
	Buffers are internally grown if more incoming or outgoing data storage is
	needed, up to a maximum of SSL_MAX_BUF_SIZE. Once the memory used by the
	buffer again drops below SSL_DEFAULT_BUF_SIZE, the buffer will be reduced
	to this size. Most standard SSL handshakes require on the order of 1024 B.
	
	SSL_DEFAULT_BUF_SIZE	value in bytes, maximum SSL_MAX_BUF_SIZE
 */
#define	SSL_DEFAULT_BUF_SIZE	256	/* Baseline send/recv buf size */

/******************************************************************************/
/*
	If SERVER you may define the number of sessions to cache and how
	long a session will remain valid in the cache from first access.
	
	Session caching enables very fast "session resumption handshakes". Session
	caching can effectively by disabled by setting SSL_SESSION_ENTRY_LIFE to 0

	SSL_SESSION_TABLE_SIZE minimum value is 1
	SSL_SESSION_ENTRY_LIFE is in milliseconds, minimum 0

*/
#define SSL_SESSION_TABLE_SIZE	32
#define SSL_SESSION_ENTRY_LIFE	86400 * 1000 /* one day */


/******************************************************************************/
/*
	REHANDSHAKING SUPPORT
	In late 2009 An "authentication gap" exploit was discovered in the
	SSL re-handshaking protocol.  The fix to the exploit was introduced
	in RFC 5746 and is referred to here	as SECURE_REHANDSHAKES.
	
	ENABLE_SECURE_REHANDSHAKES implements RFC 5746 and will securely
	renegotiate with any implementations that support it.  It is
	recommended to leave this enabled.
	
	By enabling REQUIRE_SECURE_REHANDSHAKES, the library will test that each
	communicating peer has implemented RFC 5746 and will terminate handshakes
	with any that have not.
	
	If working with SSL peers that have not implemented RFC 5746 and
	rehandshakes are required, you may enable ENABLE_INSECURE_REHANDSHAKES
	but it is NOT RECOMMENDED
			
	It is a conflict to enable both ENABLE_INSECURE_REHANDSHAKES and
	REQUIRE_SECURE_REHANDSHAKES and a compile error will occur
	
	To completely disable rehandshaking comment out all three of these defines
*/
#define ENABLE_SECURE_REHANDSHAKES
/* #define REQUIRE_SECURE_REHANDSHAKES */
/* #define ENABLE_INSECURE_REHANDSHAKES */ /* NOT RECOMMENDED */

/******************************************************************************/
/*
	Enable certificate chain message "stream" parsing.  This allows single
	certificates to be parsed on-the-fly without having to wait for the entire
	certificate chain to be recieved in the buffer.  This is a memory saving
	feature for the application buffer but will add a small amount of code 
	size for the parsing and structure overhead.

	This feature will only save memory if the CERTIFICATE message is the
	only message in the record, and multiple certs are present in the chain.
*/
/* #define USE_CERT_CHAIN_PARSING */

/******************************************************************************/
/*
	Show which SSL messages are created and parsed
*/
/* #define USE_SSL_HANDSHAKE_MSG_TRACE */

/******************************************************************************/
/*
	Informational trace that could help pinpoint problems with SSL connections
*/
/* #define USE_SSL_INFORMATIONAL_TRACE */

/******************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* _h_MATRIXCONFIG */
/******************************************************************************/

