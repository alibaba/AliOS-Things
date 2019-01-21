/**
 *	@file    cryptoApi.h
 *	@version 33ef80f (HEAD, tag: MATRIXSSL-3-7-2-OPEN, tag: MATRIXSSL-3-7-2-COMM, origin/master, origin/HEAD, master)
 *
 *	Prototypes for the Matrix crypto public APIs.
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

#ifndef _h_PS_CRYPTOAPI
#define _h_PS_CRYPTOAPI

#include "coreApi.h" /* Must be first included */
#include "cryptoConfig.h" /* Must be second included */
#include "symmetric.h"
#include "digest.h"
#include "soc_types.h"
#include "attrs.h"

typedef uint8_t U8;
typedef uint32_t U32;

/******************************************************************************/
/*	Public return codes */
/******************************************************************************/
/*	Failure codses MUST be < 0  */
/*	NOTE: The range for crypto error codes must be between -30 and -49  */
#define	PS_PARSE_FAIL			-31

/*
	PS NOTE:  Any future additions to certificate authentication failures
	must be carried through to MatrixSSL code
*/
#define PS_CERT_AUTH_PASS			PS_TRUE
#define	PS_CERT_AUTH_FAIL_BC		-32 /* BasicConstraint failure */
#define	PS_CERT_AUTH_FAIL_DN		-33 /* DistinguishedName failure */
#define	PS_CERT_AUTH_FAIL_SIG		-34 /* Signature validation failure */
#define PS_CERT_AUTH_FAIL_REVOKED	-35 /* Revoked via CRL */
#define	PS_CERT_AUTH_FAIL			-36 /* Generic cert auth fail */
#define PS_CERT_AUTH_FAIL_EXTENSION -37 /* extension permission problem */
#define PS_CERT_AUTH_FAIL_PATH_LEN	-38 /* pathLen exceeded */
#define PS_CERT_AUTH_FAIL_AUTHKEY	-39 /* subjectKeyid != issuer authKeyid */

#define PS_SIGNATURE_MISMATCH	-40 /* Alorithms all work but sig not a match */

/* Set as authStatusFlags to certificate callback when authStatus
	is PS_CERT_AUTH_FAIL_EXTENSION */
#define PS_CERT_AUTH_FAIL_KEY_USAGE_FLAG	0x01
#define PS_CERT_AUTH_FAIL_EKU_FLAG			0x02
#define PS_CERT_AUTH_FAIL_SUBJECT_FLAG		0x04
#define PS_CERT_AUTH_FAIL_DATE_FLAG			0x08

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

#define	CRYPT_INVALID_KEYSIZE	-21
#define	CRYPT_INVALID_ROUNDS	-22

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
	Endian helper macros
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
x = (((uint64)((y)[7] & 255))<<56)|(((uint64)((y)[6] & 255))<<48)| \
(((uint64)((y)[5] & 255))<<40)|(((uint64)((y)[4] & 255))<<32)| \
(((uint64)((y)[3] & 255))<<24)|(((uint64)((y)[2] & 255))<<16)| \
(((uint64)((y)[1] & 255))<<8)|(((uint64)((y)[0] & 255))); \
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
x = (((uint64)((y)[0] & 255))<<56)|(((uint64)((y)[1] & 255))<<48) | \
(((uint64)((y)[2] & 255))<<40)|(((uint64)((y)[3] & 255))<<32) | \
(((uint64)((y)[4] & 255))<<24)|(((uint64)((y)[5] & 255))<<16) | \
(((uint64)((y)[6] & 255))<<8)|(((uint64)((y)[7] & 255))); \
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
x = (((uint64)((y)[0] & 255))<<56)|(((uint64)((y)[1] & 255))<<48) | \
(((uint64)((y)[2] & 255))<<40)|(((uint64)((y)[3] & 255))<<32) | \
(((uint64)((y)[4] & 255))<<24)|(((uint64)((y)[5] & 255))<<16) | \
(((uint64)((y)[6] & 255))<<8)|(((uint64)((y)[7] & 255))); }

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
x = (((uint64)((y)[7] & 255))<<56)|(((uint64)((y)[6] & 255))<<48)| \
(((uint64)((y)[5] & 255))<<40)|(((uint64)((y)[4] & 255))<<32)| \
(((uint64)((y)[3] & 255))<<24)|(((uint64)((y)[2] & 255))<<16)| \
(((uint64)((y)[1] & 255))<<8)|(((uint64)((y)[0] & 255))); \
}

#else /* 64-bit words then  */
#define STORE32L(x, y) \
{ unsigned long __t = (x); memcpy(y, &__t, 4); }

#define LOAD32L(x, y) \
{ memcpy(&(x), y, 4); x &= 0xFFFFFFFF; }

#define STORE64L(x, y) \
{ uint64 __t = (x); memcpy(y, &__t, 8); }

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
x = (((uint64)((y)[7] & 255))<<56)|(((uint64)((y)[6] & 255))<<48) | \
(((uint64)((y)[5] & 255))<<40)|(((uint64)((y)[4] & 255))<<32) | \
(((uint64)((y)[3] & 255))<<24)|(((uint64)((y)[2] & 255))<<16) | \
(((uint64)((y)[1] & 255))<<8)|(((uint64)((y)[0] & 255))); \
}

#ifdef ENDIAN_32BITWORD
#define STORE32H(x, y) \
{ unsigned int __t = (x); memcpy(y, &__t, 4); }

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
x = (((uint64)((y)[0] & 255))<<56)|(((uint64)((y)[1] & 255))<<48)| \
(((uint64)((y)[2] & 255))<<40)|(((uint64)((y)[3] & 255))<<32)| \
(((uint64)((y)[4] & 255))<<24)|(((uint64)((y)[5] & 255))<<16)| \
(((uint64)((y)[6] & 255))<<8)| (((uint64)((y)[7] & 255))); \
}

#else /* 64-bit words then  */

#define STORE32H(x, y) \
{ unsigned long __t = (x); memcpy(y, &__t, 4); }

#define LOAD32H(x, y) \
{ memcpy(&(x), y, 4); x &= 0xFFFFFFFF; }

#define STORE64H(x, y) \
{ uint64 __t = (x); memcpy(y, &__t, 8); }

#define LOAD64H(x, y) \
{ memcpy(&(x), y, 8); }

#endif /* ENDIAN_64BITWORD */
#endif /* ENDIAN_BIG */

#ifdef HAVE_NATIVE_INT64
#define ROL64c(x, y) \
( (((x)<<((uint64)(y)&63)) | \
(((x)&CONST64(0xFFFFFFFFFFFFFFFF))>>((uint64)64-((y)&63)))) & CONST64(0xFFFFFFFFFFFFFFFF))

#define ROR64c(x, y) \
( ((((x)&CONST64(0xFFFFFFFFFFFFFFFF))>>((uint64)(y)&CONST64(63))) | \
((x)<<((uint64)(64-((y)&CONST64(63)))))) & CONST64(0xFFFFFFFFFFFFFFFF))
#endif /* HAVE_NATIVE_INT64 */
/******************************************************************************/

#if defined PSTM_X86 || defined PSTM_X86_64 || defined PSTM_ARM || \
	defined PSTM_MIPS
 #define PSTM_ASM_CONFIG_STR "Y"
#else
 #define PSTM_ASM_CONFIG_STR "N"
#endif
#ifdef PSTM_64BIT
 #define PSTM_64_CONFIG_STR "Y"
#else
 #define PSTM_64_CONFIG_STR "N"
#endif
#ifdef USE_AESNI_CRYPTO
 #define AESNI_CONFIG_STR "Y"
#else
 #define AESNI_CONFIG_STR "N"
#endif
 #define HW_PKA_CONFIG_STR "N"
#ifdef USE_PKCS11
 #define PKCS11_CONFIG_STR "Y"
#else
 #define PKCS11_CONFIG_STR "N"
#endif
 #define FIPS_CONFIG_STR "N"

#define PSCRYPTO_CONFIG \
	"Y" \
	PSTM_ASM_CONFIG_STR \
	PSTM_64_CONFIG_STR \
	AESNI_CONFIG_STR \
	HW_PKA_CONFIG_STR \
	PKCS11_CONFIG_STR \
	FIPS_CONFIG_STR

/******************************************************************************/
/* Public APIs */
/******************************************************************************/

PSPUBLIC int32 psCryptoOpen(char *config);
PSPUBLIC void psCryptoClose(void);

/******************************************************************************/
/* cryptography padding APIs */
/******************************************************************************/
typedef enum t_PADDING_MODE{
    ZERO,
    PKCS5,
    PKCS7,
}PADDING_MODE;

typedef enum t_BLOCK_SIZE{
    BLOCK08 = 0x08,
    BLOCK16 = 0x10,
}BLOCK_SIZE;

int pspadding_enc(U8 *in, U32 inlen, U8 *out, U32 *outlen, PADDING_MODE mode, BLOCK_SIZE blocksize);
int pspadding_dec(U8 *in, U32 inlen, U8* out, U32 *outlen, PADDING_MODE mode, BLOCK_SIZE blocksize);
#ifdef USE_AES
/******************************************************************************/
int aes_ecb_enc( const void *kek, int key_len, const void *src, void *dest, int data_len);
int aes_ecb_dec( const void *kek, int key_len, const void *src, void *dest, int data_len);

/*
	CBC Mode AES
*/
PSPUBLIC int32 psAesInit(psCipherContext_t *ctx, unsigned char *IV,
						unsigned char *key, uint32 keylen);
PSPUBLIC int32 psAesDecrypt(psCipherContext_t *ctx, unsigned char *ct,
						unsigned char *pt, uint32 len);
PSPUBLIC int32 psAesEncrypt(psCipherContext_t *ctx, unsigned char *pt,
						unsigned char *ct, uint32 len);
/*
	Block Mode AES
*/
PSPUBLIC int32 psAesInitKey(const unsigned char *key, uint32 keylen,
						psAesKey_t *skey);
PSPUBLIC void psAesEncryptBlock(const unsigned char *pt, unsigned char *ct,
						psAesKey_t *skey);
PSPUBLIC void psAesDecryptBlock(const unsigned char *ct, unsigned char *pt,
						psAesKey_t *skey);

int aes_unwrap2 ( const void *kek, const void *src, void *dest, int nblk );
int aes_wrap2 ( const void *kek, const void *src, void *dest, int nblk );

#ifdef USE_AES_GCM
PSPUBLIC int32 psAesInitGCM(psCipherContext_t *ctx, unsigned char *key,
				int32 keylen);
PSPUBLIC int32 psAesReadyGCM(psCipherContext_t *ctx, unsigned char *IV,
					unsigned char *aad,	int32 aadLen);
PSPUBLIC int32 psAesEncryptGCM(psCipherContext_t *ctx, unsigned char *pt,
			unsigned char *ct, int32 len);
PSPUBLIC int32 psAesGetGCMTag(psCipherContext_t *ctx, int32 TagBytes,
			unsigned char *tag);
PSPUBLIC int32 psAesDecryptGCM(psCipherContext_t *ctx, unsigned char *ct,
			int32 ctLen, unsigned char *pt, int32 ptLen);
PSPUBLIC int32 psAesDecryptGCMtagless(psCipherContext_t *ctx, unsigned char *ct,
			unsigned char *pt, int32 len);

#endif /* USE_AES_GCM */



#endif /* USE_AES */
/******************************************************************************/

#ifdef USE_SEED
/******************************************************************************/
PSPUBLIC int32 psSeedInit(psCipherContext_t *ctx, unsigned char *IV,
						unsigned char *key, uint32 keylen);
PSPUBLIC int32 psSeedDecrypt(psCipherContext_t *ctx, unsigned char *ct,
						unsigned char *pt, uint32 len);
PSPUBLIC int32 psSeedEncrypt(psCipherContext_t *ctx, unsigned char *pt,
						unsigned char *ct, uint32 len);

PSPUBLIC int32 psSeedInitKey(const unsigned char *key, uint32 keylen,
						psSeedKey_t *skey);
PSPUBLIC void psSeedEncryptBlock(const unsigned char *pt, unsigned char *ct,
						psSeedKey_t *skey);
PSPUBLIC void psSeedDecryptBlock(const unsigned char *ct, unsigned char *pt,
						psSeedKey_t *skey);
#endif /* USE_SEED */
/******************************************************************************/

/******************************************************************************/
#ifdef USE_3DES
/******************************************************************************/
/*
	CBC Mode DES3
*/
PSPUBLIC int32 psDes3Init(psCipherContext_t *ctx, unsigned char *IV,
						unsigned char *key, uint32 keylen);
PSPUBLIC int32 psDes3Decrypt(psCipherContext_t *ctx, unsigned char *ct,
						unsigned char *pt, uint32 len);
PSPUBLIC int32 psDes3Encrypt(psCipherContext_t *ctx, unsigned char *pt,
						unsigned char *ct, uint32 len);
/*
	Block Mode DES3
*/
PSPUBLIC int32 psDes3InitKey(const unsigned char *key, uint32 keylen,
						psDes3Key_t *skey);
PSPUBLIC void psDes3EncryptBlock(const unsigned char *pt, unsigned char *ct,
						psDes3Key_t *skey);
PSPUBLIC void psDes3DecryptBlock(const unsigned char *ct, unsigned char *pt,
						psDes3Key_t *skey);
#endif /* USE_3DES */
/******************************************************************************/

/******************************************************************************/
#ifdef USE_DES
PSPUBLIC int32 psDesInitKey(const unsigned char *key, int32 keylen,
						psDes3Key_t *skey);
PSPUBLIC void psDesEncryptBlock(const unsigned char *pt, unsigned char *ct,
						psDes3Key_t *skey);
PSPUBLIC void psDesDecryptBlock(const unsigned char *ct, unsigned char *pt,
						psDes3Key_t *skey);
#endif /* USE_DES */
/******************************************************************************/

#ifdef USE_IDEA
/******************************************************************************/
/*
	CBC Mode IDEA
*/
PSPUBLIC int32 psIdeaInit(psCipherContext_t *ctx, unsigned char *IV,
						unsigned char *key, uint32 keylen);
PSPUBLIC int32 psIdeaDecrypt(psCipherContext_t *ctx, unsigned char *ct,
						unsigned char *pt, uint32 len);
PSPUBLIC int32 psIdeaEncrypt(psCipherContext_t *ctx, unsigned char *pt,
						unsigned char *ct, uint32 len);
#endif

/******************************************************************************/
#ifdef USE_ARC4
PSPUBLIC void psArc4Init(psCipherContext_t *ctx, unsigned char *key,
						uint32 keylen);
PSPUBLIC int32 psArc4(psCipherContext_t *ctx, unsigned char *in,
						unsigned char *out, uint32 len);
void arc4_skip ( const void *key, size_t keylen, size_t skip,
		 const void *src, void *dst, size_t msglen );
#endif /* USE_ARC4 */
/******************************************************************************/

#ifdef USE_RC2
/******************************************************************************/
PSPUBLIC int32 psRc2Init(psCipherContext_t *ctx, unsigned char *IV,
						unsigned char *key, uint32 keylen);
PSPUBLIC int32 psRc2Decrypt(psCipherContext_t *ctx, unsigned char *ct,
						unsigned char *pt, uint32 len);
PSPUBLIC int32 psRc2Encrypt(psCipherContext_t *ctx, unsigned char *pt,
						unsigned char *ct, uint32 len);
PSPUBLIC int32 psRc2InitKey(unsigned char *key, uint32 keylen, uint32 rds,
						psRc2Key_t *skey);
PSPUBLIC int32 psRc2EncryptBlock(unsigned char *pt, unsigned char *ct,
						psRc2Key_t *skey);
PSPUBLIC int32 psRc2DecryptBlock(unsigned char *ct, unsigned char *pt,
						psRc2Key_t *skey);
#endif /* USE_RC2 */
/******************************************************************************/
/******************************************************************************/
#ifdef USE_SHA1
/******************************************************************************/
void sha1hw_init(psDigestContext_t *context);
int sha1hw_input(psDigestContext_t *context, unsigned char *data, int datalen) ;
void sha1hw_end(psDigestContext_t *context) ;
void sha1hw_end2(psDigestContext_t *context, unsigned char *hash) ;
PSPUBLIC void psSha1Init(psDigestContext_t * md);
PSPUBLIC void psSha1Update(psDigestContext_t * md, const unsigned char *buf,
				uint32 len);
PSPUBLIC int32 psSha1Final(psDigestContext_t * md, unsigned char *hash);

#ifdef USE_HMAC
PSPUBLIC int32 psHmacSha1(unsigned char *key, uint32 keyLen,
				const unsigned char *buf, uint32 len,
				unsigned char *hash, unsigned char *hmacKey,
				uint32 *hmacKeyLen);
PSPUBLIC void psHmacSha1Init(psHmacContext_t *ctx, unsigned char *key,
				uint32 keyLen);
PSPUBLIC void psHmacSha1Update(psHmacContext_t *ctx, const unsigned char *buf,
				uint32 len);
PSPUBLIC int32 psHmacSha1Final(psHmacContext_t *ctx, unsigned char *hash);
#endif /* USE_HMAC */
#endif /* USE_SHA1 */
/******************************************************************************/

/******************************************************************************/
#ifdef USE_SHA256
#ifdef USE_HMAC
PSPUBLIC int32 psHmacSha2(unsigned char *key, uint32 keyLen,
				const unsigned char *buf, uint32 len,
				unsigned char *hash, unsigned char *hmacKey,
				uint32 *hmacKeyLen, uint32 hashSize);
PSPUBLIC void psHmacSha2Init(psHmacContext_t *ctx, unsigned char *key,
				uint32 keyLen, uint32 hashSize);
PSPUBLIC void psHmacSha2Update(psHmacContext_t *ctx, const unsigned char *buf,
				uint32 len, uint32 hashSize);
PSPUBLIC int32 psHmacSha2Final(psHmacContext_t *ctx, unsigned char *hash,
				uint32 hashSize);
#endif /* USE_HMAC */
#endif /* USE_SHA256 */
/******************************************************************************/
#ifdef USE_SHA256
PSPUBLIC void psSha256Init(psDigestContext_t * md);
PSPUBLIC void psSha256Update(psDigestContext_t * md, const unsigned char *buf,
				uint32 len);
PSPUBLIC int32 psSha256Final(psDigestContext_t * md, unsigned char *hash);
#endif /* USE_SHA256 */

#ifdef USE_SHA224
PSPUBLIC void psSha224Init(psDigestContext_t * md);
PSPUBLIC void psSha224Update(psDigestContext_t * md, const unsigned char *buf,
				uint32 len);
PSPUBLIC int32 psSha224Final(psDigestContext_t * md, unsigned char *hash);
#endif /* USE_SHA224 */

#ifdef USE_SHA384
PSPUBLIC void psSha384Init(psDigestContext_t * md);
PSPUBLIC void psSha384Update(psDigestContext_t * md, const unsigned char *buf,
				uint32 len);
PSPUBLIC int32 psSha384Final(psDigestContext_t * md, unsigned char *hash);
#endif /* USE_SHA384 */

#ifdef USE_SHA512
PSPUBLIC void psSha512Init(psDigestContext_t * md);
PSPUBLIC void psSha512Update(psDigestContext_t * md, const unsigned char *buf,
				uint32 len);
PSPUBLIC int32 psSha512Final(psDigestContext_t * md, unsigned char *hash);
#endif /* USE_SHA512 */

/******************************************************************************/

#ifdef USE_ECC
PSPUBLIC int32 psEcdsaParsePrivKey(psPool_t *pool, unsigned char *keyBuf,
				int32 keyBufLen, psPubKey_t **keyPtr, psEccSet_t *curve);
PSPUBLIC int32 psEcdsaParsePrivFile(psPool_t *pool, char *fileName,
				char *password,	psPubKey_t **outkey);
PSPUBLIC int32 psEccX963ImportKey(psPool_t *pool, const unsigned char *inbuf,
				uint32 inlen, psEccKey_t *key, psEccSet_t *dp);
PSPUBLIC int32 psEccX963ExportKey(psPool_t *pool, psEccKey_t *key,
				unsigned char *outbuf, uint32 *outlen);
PSPUBLIC int32 psEccMakeKeyEx(psPool_t *pool, psEccKey_t **keyPtr,
				psEccSet_t *dp, void *eccData);
PSPUBLIC void psEccFreeKey(psEccKey_t **key);
PSPUBLIC int32 psEccGenSharedSecret(psPool_t *pool, psEccKey_t *private_key,
				psEccKey_t *public_key, unsigned char *outbuf,
				uint32 *outlen, void *eccData);
PSPUBLIC int32 psEcDsaValidateSignature(psPool_t *pool, psEccKey_t *myPubKey,
				unsigned char *signature, int32 sigLen,	unsigned char *hash,
				int32 hashLen, int32 *stat, void *eccData);
PSPUBLIC int32 psEccSignHash(psPool_t *pool, unsigned char *inbuf,
				int32 inlen, unsigned char *c, int32 outlen,
				psEccKey_t *privKey, int32 *bytesWritten, int32 includeSize,
				void *eccData);
#endif /* USE_ECC */
/******************************************************************************/

/******************************************************************************/
#ifdef USE_MD5
/******************************************************************************/
PSPUBLIC void psMd5Init(psDigestContext_t * md);
PSPUBLIC void psMd5Update(psDigestContext_t * md, const unsigned char *buf,
				uint32 len);
PSPUBLIC int32 psMd5Final(psDigestContext_t * md, unsigned char *hash);

#ifdef USE_HMAC
PSPUBLIC int32 psHmacMd5(unsigned char *key, uint32 keyLen,
				const unsigned char *buf, uint32 len,
				unsigned char *hash, unsigned char *hmacKey,
				uint32 *hmacKeyLen);
PSPUBLIC void psHmacMd5Init(psHmacContext_t *ctx, unsigned char *key,
				uint32 keyLen);
PSPUBLIC void psHmacMd5Update(psHmacContext_t *ctx, const unsigned char *buf,
				uint32 len);
PSPUBLIC int32 psHmacMd5Final(psHmacContext_t *ctx, unsigned char *hash);
#endif /* USE_HMAC */
#endif /* USE_MD5 */
/******************************************************************************/

/******************************************************************************/
#ifdef USE_MD5
/******************************************************************************/
PSPUBLIC void psMd5Init(psDigestContext_t * md);
PSPUBLIC void psMd5Update(psDigestContext_t * md, const unsigned char *buf,
				uint32 len);
PSPUBLIC int32 psMd5Final(psDigestContext_t * md, unsigned char *hash);

#ifdef USE_HMAC
PSPUBLIC int32 psHmacMd5(unsigned char *key, uint32 keyLen,
				const unsigned char *buf, uint32 len,
				unsigned char *hash, unsigned char *hmacKey,
				uint32 *hmacKeyLen);
PSPUBLIC void psHmacMd5Init(psHmacContext_t *ctx, unsigned char *key,
				uint32 keyLen);
PSPUBLIC void psHmacMd5Update(psHmacContext_t *ctx, const unsigned char *buf,
				uint32 len);
PSPUBLIC int32 psHmacMd5Final(psHmacContext_t *ctx, unsigned char *hash);
#endif /* USE_HMAC */
#endif /* USE_MD5 */
/******************************************************************************/

/******************************************************************************/
#ifdef USE_MD4
/******************************************************************************/
PSPUBLIC void psMd4Init(psDigestContext_t * md);
PSPUBLIC void psMd4Update(psDigestContext_t * md, const unsigned char *buf,
				uint32 len);
PSPUBLIC int32 psMd4Final(psDigestContext_t * md, unsigned char *hash);
#endif /* USE_MD4 */
/******************************************************************************/

/******************************************************************************/
#ifdef USE_MD2
/******************************************************************************/
PSPUBLIC void psMd2Init(psDigestContext_t * md);
PSPUBLIC int32 psMd2Update(psDigestContext_t * md, const unsigned char *buf,
				uint32 len);
PSPUBLIC int32 psMd2Final(psDigestContext_t * md, unsigned char *hash);
#endif /* USE_MD2 */
/******************************************************************************/

#endif /* _h_PS_CRYPTOAPI */
/******************************************************************************/

