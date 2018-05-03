/**
 *	@file    digest.h
 *	@version 33ef80f (HEAD, tag: MATRIXSSL-3-7-2-OPEN, tag: MATRIXSSL-3-7-2-COMM, origin/master, origin/HEAD, master)
 *
 *	Header for internal symmetric key cryptography support.
 */
/*
 *	Copyright (c) 2013-2015 INSIDE Secure Corporation
 *	Copyright (c) PeerSec Networks, 2002-2011
 *	All Rights Reserved
 *
 *	The latest version of this code is available at http://www.matrixssl.org
 *
 *	This software is open source; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This General Public License does NOT permit incorporating this software
 *	into proprietary programs.  If you are unable to comply with the GPL, a
 *	commercial license for this software may be purchased from INSIDE at
 *	http://www.insidesecure.com/eng/Company/Locations
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

#ifndef _h_PS_DIGEST
#define _h_PS_DIGEST
/******************************************************************************/
//#include "md5.h"

#define SHA1_HASH_SIZE 20
#ifdef USE_SHA1
struct sha1_state {
#ifdef HAVE_NATIVE_INT64
	uint64		length;
#else
	uint32		lengthHi;
	uint32		lengthLo;
#endif /* HAVE_NATIVE_INT64 */
	uint32		state[5], curlen;
	unsigned char	buf[64];
};
#endif /* USE_SHA1 */

#define SHA256_HASH_SIZE 32
#ifdef USE_SHA256
struct sha256_state {
#ifdef HAVE_NATIVE_INT64
	uint64		length;
#else
	uint32		lengthHi;
	uint32		lengthLo;
#endif /* HAVE_NATIVE_INT64 */
	uint32		state[8], curlen;
	unsigned char buf[64];
};
#endif /* USE_SHA256 */

#define MD5_HASH_SIZE 16
#ifdef USE_MD5
struct md5_state {
#ifdef HAVE_NATIVE_INT64
	uint64 length;
#else
	uint32 lengthHi;
	uint32 lengthLo;
#endif /* HAVE_NATIVE_INT64 */
	uint32 state[4], curlen;
	unsigned char buf[64];
};
#endif /* USE_MD5 */

#ifdef USE_MD4
struct md4_state {
#ifdef HAVE_NATIVE_INT64
	uint64 length;
#else
	uint32 lengthHi;
	uint32 lengthLo;
#endif /* HAVE_NATIVE_INT64 */
	uint32 state[4], curlen;
	unsigned char buf[64];
};
#endif /* USE_MD4 */

#ifdef USE_MD2
struct md2_state {
	unsigned char	chksum[16], X[48], buf[16];
	uint32			curlen;
};
#endif /* USE_MD2 */

#define SHA224_HASH_SIZE 28
#ifdef USE_SHA224
#ifndef USE_SHA256
#error "Must enable USE_SHA256 in cryptoConig.h if USE_SHA224 is enabled"
#endif
#endif /* USE_SHA224 */

#define SHA512_HASH_SIZE 64
#ifdef USE_SHA512
#ifndef HAVE_NATIVE_INT64
#error "Must enable HAVE_NATIVE_INT64 in coreConig.h if USE_SHA512 is enabled"
#endif
struct sha512_state {
	uint64  length, state[8];
	unsigned long curlen;
	unsigned char buf[128];
};
#endif

#define SHA384_HASH_SIZE 48
#ifdef USE_SHA384
#ifndef USE_SHA512
#error "Must enable USE_SHA512 in cryptoConig.h if USE_SHA384 is enabled"
#endif
#endif /* USE_SHA384 */


#ifdef USE_SHA512
	#define MAX_HASH_SIZE SHA512_HASH_SIZE /* SHA384 depends on SHA512 */
#else
	#ifdef USE_SHA256
		#define MAX_HASH_SIZE SHA256_HASH_SIZE
	#else
		#define MAX_HASH_SIZE SHA1_HASH_SIZE
	#endif
#endif

/******************************************************************************/
typedef union {
#ifndef USE_PKCS11_HASH
#ifdef USE_SHA1
	struct sha1_state	sha1;
#endif /* USE_SHA1 */

#ifdef USE_MD5
	struct md5_state	md5;
#endif /* USE_MD5 */

#ifdef USE_MD2
	struct md2_state	md2;
#endif /* USE_MD2 */

#ifdef USE_MD4
	struct md4_state	md4;
#endif /* USE_MD4 */

#ifdef USE_SHA256 /* SHA224 uses */
	struct sha256_state sha256;
#endif

#ifdef USE_SHA512 /* SHA384 uses */
	struct sha512_state sha512;
#endif

#else /* USE_PKCS11_HASH  */
	CK_SESSION_HANDLE   sess;
#ifdef USE_MD5
	struct md5_state	md5; /* X.509 helper functionality */
#endif /* USE_MD5 */
#endif /* USE_PKCS11_HASH */

} psDigestContext_t;


extern void sha1_compress(psDigestContext_t *md);
extern void sha256_compress(psDigestContext_t *md, unsigned char *buf);
extern void sha512_compress(psDigestContext_t * md, unsigned char *buf);

/******************************************************************************/
#ifdef USE_HMAC
/******************************************************************************/
typedef struct {
#ifdef USE_SHA384
	unsigned char	pad[128];
#else
	unsigned char	pad[64];
#endif
	union {
		psDigestContext_t	md5;
		psDigestContext_t	sha1;
		psDigestContext_t	sha256;
		psDigestContext_t	sha512;
	} u;
} psHmacContext_t;
#endif /* USE_HMAC */



/******************************************************************************/

#endif /* _h_PS_DIGEST */
/******************************************************************************/

