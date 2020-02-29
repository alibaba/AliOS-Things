#include "km.h"

#define ID2_KEY_NAME "id2_key"
#define ID2_KEY_NAME_LEN (sizeof(ID2_KEY_NAME) - 1)
#define RSA_GEN_NAME "rsa_gen_key"
#define RSA_GEN_NAME_LEN sizeof(RSA_GEN_NAME)
#define RSA_IM_NAME "rsa_im_key"
#define RSA_IM_NAME_LEN sizeof(RSA_IM_NAME)
#define AES_GEN_NAME "aes_gen_key"
#define AES_GEN_NAME_LEN sizeof(AES_GEN_NAME)
#define AES_IM_NAME "aes_im_key"
#define AES_IM_NAME_LEN sizeof(AES_IM_NAME)
#define HMAC_GEN_NAME "hmac_gen_key"
#define HMAC_GEN_NAME_LEN sizeof(HMAC_GEN_NAME)
#define HMAC_IM_NAME "hmac_im_key"
#define HMAC_IM_NAME_LEN sizeof(HMAC_IM_NAME)

#define km_malloc malloc
#define km_memset memset
#define km_memcpy memcpy
