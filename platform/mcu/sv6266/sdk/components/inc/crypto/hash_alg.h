#ifndef _HASH_ALG_H_
#define _HASH_ALG_H_

#include "soc_types.h"

#define HMAC_SHA1_DIGEST_LENGTH	20
#define HMAC_SHA1_BLOCK_LENGTH	64

/* The HMAC_SHA1 structure: */
typedef struct _HMAC_SHA1_CTX {
	unsigned char	ipad[HMAC_SHA1_BLOCK_LENGTH];
	unsigned char	opad[HMAC_SHA1_BLOCK_LENGTH];
	unsigned char	key[HMAC_SHA1_BLOCK_LENGTH];
	unsigned int	keylen;
} HMAC_SHA1_CTX;

int prf_sha1(unsigned char *key, int keylen, const char *prefix, unsigned char *salt, int saltlen, unsigned char *ptk, int ptklen) ;
int hmac_sha1_cal(unsigned char *key, int keylen, unsigned char *salt, int saltlen, unsigned char *digest) ;
int pbkdf2_sha1(unsigned char *key, int keylen, unsigned char *salt, int saltlen, unsigned int iterc, unsigned char *pmk, int pmklen) ;

#endif /* _HASH_ALG_H_ */

