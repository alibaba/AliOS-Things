#ifndef _NODECACHE_H
#define _NODECACHE_H

#include "joylink.h"
#include "joylink_aes.h"

typedef struct {
	int isInited;
	uint8_t priKey[uECC_BYTES];
	uint8_t devPubKey[uECC_BYTES * 2];
	uint8_t devPubKeyC[uECC_BYTES + 1];
}JLEccContex_t;

typedef struct{
	uint8_t pubkeyC[JL_MAX_KEY_BIN_LEN];		// ѹ????ʽ?Ĺ?Կ
	uint8_t sharedkey[uECC_BYTES];		// ?豸?Ĺ?????Կ
}JLKey_t;

extern JLEccContex_t __g_ekey;

/**
 * brief: 
 *
 * @Param: key
 * @Param: keyLength
 * @Param: iv
 * @Param: pEncIn
 * @Param: encLength
 * @Param: pPlainOut
 * @Param: maxOutLen
 *
 * @Returns: 
 */
extern int device_aes_decrypt(const UINT8 * key, int keyLength, 
        const UINT8 * iv, const UINT8 *pEncIn, 
        int encLength, UINT8 *pPlainOut, int maxOutLen);

/**
 * brief: 
 *
 * @Param: key
 * @Param: keyLength
 * @Param: iv
 * @Param: pPlainIn
 * @Param: plainLength
 * @Param: pEncOut
 * @Param: maxOutLen
 *
 * @Returns: 
 */
extern int device_aes_encrypt(const UINT8 * key, int keyLength,
        const UINT8 * iv, const UINT8 *pPlainIn,
        int plainLength, UINT8 *pEncOut, int maxOutLen);
#endif
