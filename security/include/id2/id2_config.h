/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __ID2_CONFIG_H__
#define __ID2_CONFIG_H__

#define ID2_KEY_TYPE_3DES               1
#define ID2_KEY_TYPE_AES                2
#define ID2_KEY_TYPE_RSA                3
#define ID2_KEY_TYPE_SM1                4
#define ID2_KEY_TYPE_SM4                5

#define ID2_HASH_TYPE_SHA256            2
#define ID2_HASH_TYPE_SM3               3

////////////////////////////////////////////////////////////////////////////////
#ifndef CONFIG_ID2_KEY_TYPE
#define CONFIG_ID2_KEY_TYPE         ID2_KEY_TYPE_AES
#endif

#if (CONFIG_ID2_KEY_TYPE == ID2_KEY_TYPE_SM4)
#define CONFIG_ID2_HASH_TYPE        ID2_HASH_TYPE_SM3
#endif

#ifndef CONFIG_ID2_HASH_TYPE
#define CONFIG_ID2_HASH_TYPE        ID2_HASH_TYPE_SHA256
#endif

////////////////////////////////////////////////////////////////////////////////

#if (CONFIG_ID2_KEY_TYPE != ID2_KEY_TYPE_3DES && \
     CONFIG_ID2_KEY_TYPE != ID2_KEY_TYPE_AES &&  \
     CONFIG_ID2_KEY_TYPE != ID2_KEY_TYPE_RSA &&  \
     CONFIG_ID2_KEY_TYPE != ID2_KEY_TYPE_SM1 &&  \
     CONFIG_ID2_KEY_TYPE != ID2_KEY_TYPE_SM4)
#error "CONFIG_ID2_KEY_TYPE error."
#endif

#if (CONFIG_ID2_HASH_TYPE != ID2_HASH_TYPE_SHA256 && \
     CONFIG_ID2_HASH_TYPE != ID2_HASH_TYPE_SM3)
#error "CONFIG_ID2_HASH_TYPE error."
#endif

#if defined(CONFIG_ID2_OTP)
#if (CONFIG_ID2_KEY_TYPE != ID2_KEY_TYPE_AES)
#error "CONFIG_ID2_OTP error, which is only supported for aes."
#endif
#endif

////////////////////////////////////////////////////////////////////////////////

#endif  /* __ID2_CONFIG_H__ */
