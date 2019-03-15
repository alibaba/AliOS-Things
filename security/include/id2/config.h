/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__

////////////////////////////////////////////////////////////////////////////////

#ifdef WIN32
#define snprintf                        _snprintf
#endif
#define __FUNC_NAME__                   __FUNCTION__

////////////////////////////////////////////////////////////////////////////////

#define ID2_HASH_TYPE_SHA256            2

#define ID2_CRYPTO_TYPE_3DES            1
#define ID2_CRYPTO_TYPE_AES             2
#define ID2_CRYPTO_TYPE_RSA             3

#define ID2_HASH_TYPE_CONFIG            ID2_HASH_TYPE_SHA256

////////////////////////////////////////////////////////////////////////////////

//[config: local verification]

#ifndef ID2_DEBUG
    #define ID2_DEBUG                   0
#endif
#ifndef ID2_OTP_LOCAL_TEST
    #define ID2_OTP_LOCAL_TEST          0
#endif
#ifndef ID2_LOCAL_TEST
    #define ID2_LOCAL_TEST              0
#endif
#ifndef ID2_SET_ID_KEY_SUPPORTED
    #define ID2_SET_ID_KEY_SUPPORTED    0
#endif
#ifndef ID2_KM_API_EMU
    #define ID2_KM_API_EMU              0
#endif

//[config: iTLS and OTP]
#ifndef ID2_ITLS_SUPPORTED
    #define ID2_ITLS_SUPPORTED          1
#endif

#ifndef ID2_OTP_SUPPORTED
    #define ID2_OTP_SUPPORTED           0
#endif

#ifndef ID2_USE_ALI_CRYPTO
    #define ID2_USE_ALI_CRYPTO          1
#endif

#ifndef ID2_CRYPTO_TYPE_CONFIG
    #define ID2_CRYPTO_TYPE_CONFIG      ID2_CRYPTO_TYPE_AES
#endif

////////////////////////////////////////////////////////////////////////////////

#if ((ID2_CRYPTO_TYPE_CONFIG != ID2_CRYPTO_TYPE_3DES) && (ID2_CRYPTO_TYPE_CONFIG != ID2_CRYPTO_TYPE_AES) && (ID2_CRYPTO_TYPE_CONFIG != ID2_CRYPTO_TYPE_RSA))
#error("ID2_CRYPTO_TYPE_CONFIG error.");
#endif

#if (ID2_HASH_TYPE_CONFIG != ID2_HASH_TYPE_SHA256)
#error("ID2_HASH_TYPE_CONFIG error.");
#endif

////////////////////////////////////////////////////////////////////////////////

#endif
