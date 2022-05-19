#include "aliot_hmac.h"

#include <string.h>

#include "aliot_sha1.h"

#define KEY_IOPAD_SIZE   (64)
#define SHA1_DIGEST_SIZE (20)

void aliot_hmac_sha1(const char *msg, int msg_len, char *digest, const char *key, int key_len)
{
    iot_sha1_context context;
    unsigned char k_ipad[KEY_IOPAD_SIZE]; /* inner padding - key XORd with ipad  */
    unsigned char k_opad[KEY_IOPAD_SIZE]; /* outer padding - key XORd with opad */
    unsigned char out[SHA1_DIGEST_SIZE];
    int i;

    /* start out by storing key in pads */
    memset(k_ipad, 0, sizeof(k_ipad));
    memset(k_opad, 0, sizeof(k_opad));
    memcpy(k_ipad, key, key_len);
    memcpy(k_opad, key, key_len);

    /* XOR key with ipad and opad values */
    for (i = 0; i < KEY_IOPAD_SIZE; i++) {
        k_ipad[i] ^= 0x36;
        k_opad[i] ^= 0x5c;
    }

    /* perform inner SHA */
    aliot_sha1_init(&context);                                  /* init context for 1st pass */
    aliot_sha1_starts(&context);                                /* setup context for 1st pass */
    aliot_sha1_update(&context, k_ipad, KEY_IOPAD_SIZE);        /* start with inner pad */
    aliot_sha1_update(&context, (unsigned char *)msg, msg_len); /* then text of datagram */
    aliot_sha1_finish(&context, out);                           /* finish up 1st pass */

    /* perform outer SHA */
    aliot_sha1_init(&context);                           /* init context for 2nd pass */
    aliot_sha1_starts(&context);                         /* setup context for 2nd pass */
    aliot_sha1_update(&context, k_opad, KEY_IOPAD_SIZE); /* start with outer pad */
    aliot_sha1_update(&context, out, SHA1_DIGEST_SIZE);  /* then results of 1st hash */
    aliot_sha1_finish(&context, out);                    /* finish up 2nd pass */

    memcpy(digest, out, SHA1_DIGEST_SIZE);

    /*
    for (i = 0; i < SHA1_DIGEST_SIZE; ++i) {
        digest[i * 2] = aliot_hb2hex(out[i] >> 4);
        digest[i * 2 + 1] = aliot_hb2hex(out[i]);
    }
    */
}
