/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__


////////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
#define snprintf _snprintf
#endif
////////////////////////////////////////////////////////////////////////////////

#define CHIP_DEBUG 0

////////////////////////////////////////////////////////////////////////////////

#define CHIP_TYPE_SECURE_MCU 1
#define CHIP_TYPE_SE_STD_CMD 2
#define CHIP_TYPE_SE_MTK_CMD 3

#define CHIP_CRYPTO_TYPE_3DES 1
#define CHIP_CRYPTO_TYPE_AES 2
#define CHIP_CRYPTO_TYPE_RSA 3

////////////////////////////////////////////////////////////////////////////////

#define CHIP_APDU_CMD_ADAPTER 1
#define CHIP_SEND_SELECT_COMMAND 1
#define CHIP_CONST_TEST_KEY 1

////////////////////////////////////////////////////////////////////////////////

#define CHIP_TYPE_CONFIG CHIP_TYPE_SE_STD_CMD
#define CHIP_CRYPTO_TYPE_CONFIG CHIP_CRYPTO_TYPE_3DES

////////////////////////////////////////////////////////////////////////////////

#if ((CHIP_TYPE_CONFIG != CHIP_TYPE_SECURE_MCU) && \
     (CHIP_TYPE_CONFIG != CHIP_TYPE_SE_STD_CMD) && \
     (CHIP_TYPE_CONFIG != CHIP_TYPE_SE_MTK_CMD))
#error("CHIP_TYPE_CONFIG error.");
#endif

#if ((CHIP_CRYPTO_TYPE_CONFIG != CHIP_CRYPTO_TYPE_3DES) && \
     (CHIP_CRYPTO_TYPE_CONFIG != CHIP_CRYPTO_TYPE_AES) &&  \
     (CHIP_CRYPTO_TYPE_CONFIG != CHIP_CRYPTO_TYPE_RSA))
#error("CHIP_CRYPTO_TYPE_CONFIG error.");
#endif

#endif
