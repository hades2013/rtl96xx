/*
 *	rsa.c
 *	Release $Name: MATRIXSSL-3-1-3-OPEN $
 *
 *	RSA crypto
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

#ifdef USE_RSA
/******************************************************************************/
/*
 *	Free an RSA key.  pstm_clear will zero the memory of each element and free it.
 */
void psRsaFreeKey(psRsaKey_t *key)
{
	pstm_clear(&(key->N));
	pstm_clear(&(key->e));
	pstm_clear(&(key->d));
	pstm_clear(&(key->p));
	pstm_clear(&(key->q));
	pstm_clear(&(key->dP));
	pstm_clear(&(key->dQ));
	pstm_clear(&(key->qP));
	psFree(key);
}

/******************************************************************************/
/*
	Primary RSA encryption routine
*/
int32 psRsaCrypt(psPool_t *pool, const unsigned char *in, uint32 inlen,
				unsigned char *out, uint32 *outlen,	psRsaKey_t *key, int32 type)
{
	pstm_int			tmp, tmpa, tmpb;
	int32			res;
	uint32			x;

	if (in == NULL || out == NULL || outlen == NULL || key == NULL) {
		psTraceCrypto("NULL parameter error in psRsaCrypt\n");
		return PS_ARG_FAIL;
	}
	/* Init and copy into tmp */
	if (pstm_init_for_read_unsigned_bin(pool, &tmp, inlen + sizeof(pstm_digit))
			!= PS_SUCCESS) {
		return PS_FAILURE;
	}
	if (pstm_read_unsigned_bin(&tmp, (unsigned char *)in, inlen) != PS_SUCCESS) {
		pstm_clear(&tmp);
		return PS_FAILURE;
	}
	/* Sanity check on the input */
	if (pstm_cmp(&key->N, &tmp) == PSTM_LT) {
		res = PS_LIMIT_FAIL;
		goto done;
	}
	if (type == PRIVKEY_TYPE) {
		if (key->optimized) {
			if (pstm_init_size(pool, &tmpa, key->p.alloc) != PS_SUCCESS) {
				res = PS_FAILURE;
				goto done;
			}
			if (pstm_init_size(pool, &tmpb, key->q.alloc) != PS_SUCCESS) {
				pstm_clear(&tmpa);
				res = PS_FAILURE;
				goto done;
			}
			if (pstm_exptmod(pool, &tmp, &key->dP, &key->p, &tmpa) !=
					PS_SUCCESS) {
				psTraceCrypto("decrypt error: pstm_exptmod dP, p\n");
				goto error;
			}
			if (pstm_exptmod(pool, &tmp, &key->dQ, &key->q, &tmpb) !=
					PS_SUCCESS) {
				psTraceCrypto("decrypt error: pstm_exptmod dQ, q\n");
				goto error;
			}		
			if (pstm_sub(&tmpa, &tmpb, &tmp) != PS_SUCCESS) {
				psTraceCrypto("decrypt error: sub tmpb, tmp\n");
				goto error;
			}
			if (pstm_mulmod(pool, &tmp, &key->qP, &key->p, &tmp) != PS_SUCCESS) {
				psTraceCrypto("decrypt error: pstm_mulmod qP, p\n");
				goto error;
			}
			if (pstm_mul_comba(pool, &tmp, &key->q, &tmp, NULL, 0) != PS_SUCCESS){
				psTraceCrypto("decrypt error: pstm_mul q \n");
				goto error;
			}
			if (pstm_add(&tmp, &tmpb, &tmp) != PS_SUCCESS) {
				psTraceCrypto("decrypt error: pstm_add tmp \n");
				goto error;
			}			
		} else {
			if (pstm_exptmod(pool, &tmp, &key->d, &key->N, &tmp) !=
					PS_SUCCESS) {
				psTraceCrypto("psRsaCrypt error: pstm_exptmod\n");
				goto error;
			}
		}
	} else if (type == PUBKEY_TYPE) {
		if (pstm_exptmod(pool, &tmp, &key->e, &key->N, &tmp) != PS_SUCCESS) {
			psTraceCrypto("psRsaCrypt error: pstm_exptmod\n");
			goto error;
		}
	} else {
		psTraceCrypto("psRsaCrypt error: invalid type param\n");
		goto error;
	}
	/* Read it back */
	x = pstm_unsigned_bin_size(&key->N);

	if ((uint32)x > *outlen) {
		res = -1;
		psTraceCrypto("psRsaCrypt error: pstm_unsigned_bin_size\n");
		goto done;
	}
	/* We want the encrypted value to always be the key size.  Pad with 0x0 */
	while ((uint32)x < (unsigned long)key->size) {
		*out++ = 0x0;
		x++;
	}

	*outlen = x;
	/* Convert it */
	memset(out, 0x0, x);

	if (pstm_to_unsigned_bin(pool, &tmp, out+(x-pstm_unsigned_bin_size(&tmp)))
			!= PS_SUCCESS) {
		psTraceCrypto("psRsaCrypt error: pstm_to_unsigned_bin\n");
		goto error;
	}
	/* Clean up and return */
	res = PS_SUCCESS;
	goto done;
error:
	res = PS_FAILURE;
done:
	if (type == PRIVKEY_TYPE && key->optimized) {
		pstm_clear_multi(&tmpa, &tmpb, NULL, NULL, NULL, NULL, NULL, NULL);
	}
	pstm_clear(&tmp);
	return res;
}


/******************************************************************************/
/*
	RSA public encryption.

	The outlen param must be set to the strength of the key:  key->size
*/
int32 psRsaEncryptPub(psPool_t *pool, psRsaKey_t *key,
						unsigned char *in, uint32 inlen,
						unsigned char *out, uint32 outlen)
{
	int32	err;
	uint32	size;

	size = key->size;
	if (outlen < size) {
		psTraceCrypto("Error on bad outlen parameter to psRsaEncryptPub\n");
		return PS_ARG_FAIL;
	}

	if ((err = pkcs1Pad(in, inlen, out, size, PRIVKEY_TYPE)) < PS_SUCCESS) {
		psTraceCrypto("Error padding psRsaEncryptPub. Likely data too long\n");
		return err;
	}
	if ((err = psRsaCrypt(pool, out, size, out, (uint32*)&outlen, key,
			PUBKEY_TYPE)) < PS_SUCCESS) {
		psTraceCrypto("Error performing psRsaEncryptPriv\n");	
		return err;
	}
	if (outlen != size) {
		psTraceCrypto("Encrypted size error in psRsaEncryptPub\n");
		return PS_FAILURE;
	}
	return size;
}

/******************************************************************************/
/*
	RSA private decryption.
*/
int32 psRsaDecryptPriv(psPool_t *pool, psRsaKey_t *key,
						 unsigned char *in, uint32 inlen,
						 unsigned char *out, uint32 outlen)
{
	int32	err;
	uint32	ptLen;

	if (inlen != key->size) {
		psTraceCrypto("Error on bad inlen parameter to psRsaDecryptPriv\n");
		return PS_ARG_FAIL;
	}
	ptLen = inlen;
	if ((err = psRsaCrypt(pool, in, inlen, in, (uint32*)&ptLen, key,
			PRIVKEY_TYPE)) < PS_SUCCESS) {
		psTraceCrypto("Error performing psRsaDecryptPriv\n");	
		return err;
	}
	if (ptLen != inlen) {
		psTraceCrypto("Decrypted size error in psRsaDecryptPriv\n");
		return PS_FAILURE;
	}
	err = pkcs1Unpad(in, inlen, out, outlen, PRIVKEY_TYPE);
	memset(in, 0x0, inlen);
	return err;
}

/******************************************************************************/
/*
	RSA public decryption.
	NOTE: this function writes over the *in contents
*/
int32 psRsaDecryptPub(psPool_t *pool, psRsaKey_t *key,
						unsigned char *in, uint32 inlen,
						unsigned char *out,	uint32 outlen)
{
	int32	err;
	uint32	ptLen;

	if (inlen != key->size) {
		psTraceCrypto("Error on bad inlen parameter to psRsaDecryptPub\n");
		return PS_ARG_FAIL;
	}
	ptLen = inlen;
	if ((err = psRsaCrypt(pool, in, inlen, in, (uint32*)&ptLen, key,
			PUBKEY_TYPE)) < PS_SUCCESS) {
		psTraceCrypto("Error performing psRsaDecryptPub\n");	
		return err;
	}
	if (ptLen != inlen) {
		psTraceCrypto("Decrypted size error in psRsaDecryptPub\n");
		return PS_FAILURE;
	}
	if ((err = pkcs1Unpad(in, inlen, out, outlen, PUBKEY_TYPE)) < 0) {
		return err;
	}
	return PS_SUCCESS;
}
#endif /* USE_RSA */

/******************************************************************************/
