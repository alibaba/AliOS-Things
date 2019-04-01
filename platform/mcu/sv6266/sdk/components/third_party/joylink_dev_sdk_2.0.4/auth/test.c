#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "joylinkCrypto.h"

#define AES_128 128
#define AES_LEN (AES_128/8)

#define DATA_SIZE 32

#ifdef LIB_TEST
int main()
{
    int ret = 0;
    int keyLen = AES_128;
    UINT8 key[AES_LEN] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
    UINT8 tmp_iv[AES_LEN];
    UINT8 iv[AES_LEN] = { 0xab, 0xcd, 0xef, 0xab, 0xcd, 0xef, 0xab, 0xcd, 0xef, 0xab, 0xcd, 0xef, 0xab, 0xcd, 0xef, 0x35 };
    //UINT8 iv[AES_LEN] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    char data[DATA_SIZE] = { "abc" };


    int dataLen = 4;// ***** the being encrypted data length ******

    memcpy(tmp_iv, iv, sizeof(tmp_iv));
    ret = joylinkEnc2Crypt(key, AES_128, iv, data, &dataLen, DATA_SIZE, 1, JOYLINK_ENC2_ENCRYPT);

    memcpy(iv, tmp_iv, sizeof(iv));
    ret = joylinkEnc2Crypt(key, AES_128, iv, data, &dataLen, DATA_SIZE, 1, JOYLINK_ENC2_DECRYPT);

    return 0;
}
#endif
