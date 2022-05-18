
#ifndef _ALIOT_COMMON_HMAC_H_
#define _ALIOT_COMMON_HMAC_H_

#include <string.h>

void aliot_hmac_md5(const char *msg, int msg_len, char *digest, const char *key, int key_len);

void aliot_hmac_sha1(const char *msg, int msg_len, char *digest, const char *key, int key_len);

#endif
