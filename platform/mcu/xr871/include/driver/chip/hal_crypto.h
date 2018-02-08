/**
  * @file  hal_crypto.h
  * @author  XRADIO IOT WLAN Team
  */

/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _DRIVER_CHIP_HAL_CRYPTO_H_
#define _DRIVER_CHIP_HAL_CRYPTO_H_

#include <stdbool.h>
#include "driver/chip/hal_def.h"
#include "driver/chip/hal_dma.h"
#include "sys/xr_debug.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
	__IO uint32_t CTL;   			/*!< CRYP Control Register,                                       Address offset: 0x00       */
//	__IO uint32_t KEY0;  			/*!< CRYP Input Key 0/PRNG Seed 0 Register,                       Address offset: 0x04       */
//	__IO uint32_t KEY1;  			/*!< CRYP Input Key 1/PRNG Seed 1 Register,                       Address offset: 0x08       */
//	__IO uint32_t KEY2;  			/*!< CRYP Input Key 2/PRNG Seed 2 Register,                       Address offset: 0x0C       */
//	__IO uint32_t KEY3;  			/*!< CRYP Input Key 3/PRNG Seed 3 Register,                       Address offset: 0x10       */
//	__IO uint32_t KEY4;  			/*!< CRYP Input Key 4/PRNG Seed 4 Register,                       Address offset: 0x14       */
//	__IO uint32_t KEY5;  			/*!< CRYP Input Key 5/PRNG Seed 5 Register,                       Address offset: 0x18       */
//	__IO uint32_t KEY6;  			/*!< CRYP Input Key 6/PRNG Seed 6 Register,                       Address offset: 0x1C       */
//	__IO uint32_t KEY7;  			/*!< CRYP Input Key 7/PRNG Seed 7 Register,                       Address offset: 0x20       */
	__IO uint32_t KEY[8];
//	__IO uint32_t IV0;   			/*!< CRYP Initialization Vector 0 Register,                       Address offset: 0x24       */
//	__IO uint32_t IV1;   			/*!< CRYP Initialization Vector 1 Register,                       Address offset: 0x28       */
//	__IO uint32_t IV2;   			/*!< CRYP Initialization Vector 2 Register,                       Address offset: 0x2C       */
//	__IO uint32_t IV3;   			/*!< CRYP Initialization Vector 3 Register,                       Address offset: 0x30       */
	__IO uint32_t IV[4];
//	__IO uint32_t CNT0;  			/*!< CRYP Counter 0 Register,                                     Address offset: 0x34       */
//	__IO uint32_t CNT1;  			/*!< CRYP Counter 1 Register,                                     Address offset: 0x38       */
//	__IO uint32_t CNT2;  			/*!< CRYP Counter 2 Register,                                     Address offset: 0x3C       */
//	__IO uint32_t CNT3;  			/*!< CRYP Counter 3 Register,                                     Address offset: 0x40       */
	__IO uint32_t CNT[4];
	__IO uint32_t FCSR;  			/*!< CRYP FIFO Control/Status Register,                           Address offset: 0x44       */
	__IO uint32_t ICSR;  			/*!< CRYP Interrupt Control/Status Register,                      Address offset: 0x48       */
	__I  uint32_t MD0;   			/*!< CRYP Message Digest 0/PRNG Data 0 Register,                  Address offset: 0x4C       */
	__I  uint32_t MD1;   			/*!< CRYP Message Digest 1/PRNG Data 1 Register,                  Address offset: 0x50       */
	__I  uint32_t MD2;   			/*!< CRYP Message Digest 2/PRNG Data 2 Register,                  Address offset: 0x54       */
	__I  uint32_t MD3;   			/*!< CRYP Message Digest 3/PRNG Data 3 Register,                  Address offset: 0x58       */
	__I  uint32_t MD4;				/*!< CRYP Message Digest 4/PRNG Data 4 Register,                  Address offset: 0x5C       */
	__IO uint32_t CTS_LEN;			/*!< CRYP AES-CTS txt Length Register,                            Address offset: 0x60       */
	     uint32_t RESERVED1[3];		/*!< Reserved, 0x64-0x6C															          */
	__IO uint32_t CRC_POLY;			/*!< CRYP CRC Poly Register, 						               Address offset: 0x70       */
	__IO uint32_t CRC_RESULT;		/*!< CRYP CRC Result Register, 						           Address offset: 0x74       */
	     uint32_t RESERVED2[10];	/*!< Reserved, 0x78-0x9C															          */
    __I  uint32_t MD5;				/*!< CRYP SHA256 Message Digest Data 5 Register, 				   Address offset: 0xA0       */
    __I  uint32_t MD6;				/*!< CRYP SHA256 Message Digest Data 6 Register, 				   Address offset: 0xA4       */
    __I  uint32_t MD7;				/*!< CRYP SHA256 Message Digest Data 7 Register, 				   Address offset: 0xA8       */
	     uint32_t RESERVED3[85];	/*!< Reserved, 0xAC-0x1FC															          */
	__O  uint32_t RXFIFO;			/*!< CRYP RX FIFO input Register,				                   Address offset: 0x200      */
	__I  uint32_t TXFIFO;			/*!< CRYP TX FIFO output Register,				                   Address offset: 0x204      */

} CE_T;

#define CE ((CE_T *)CE_BASE)

/*
 * @brief SPI Global Control Register
 */
#define CE_CTL_CRC_XOR_OUT_SHIFT				(31)
#define CE_CTL_CRC_XOR_OUT_MASK					(0x1U << CE_CTL_CRC_XOR_OUT_SHIFT)

#define CE_CTL_CRC_REF_OUT_SHIFT				(30)
#define CE_CTL_CRC_REF_OUT_MASK					(0x1U << CE_CTL_CRC_REF_OUT_SHIFT)

#define CE_CTL_CRC_REF_IN_SHIFT					(29)
#define CE_CTL_CRC_REF_IN_MASK					(0x1U << CE_CTL_CRC_REF_IN_SHIFT)

#define CE_CTL_CRC_INIT_SHIFT					(28)
#define CE_CTL_CRC_INIT_MASK					(0x1U << CE_CTL_CRC_INIT_SHIFT)

#define CE_CTL_KEY_SEL_SHIFT					(24)
#define CE_CTL_KEY_SEL_MASK						(0xFU << CE_CTL_KEY_SEL_SHIFT)
typedef enum {
	CE_CTL_KEYSOURCE_INPUT 			= 0 		<< CE_CTL_KEY_SEL_SHIFT,	/*!< key from input(normal mode) */
	CE_CTL_KEYSOURCE_SID 			= 1 		<< CE_CTL_KEY_SEL_SHIFT,    /*!< key from SID */
	CE_CTL_KEYSOURCE_INTERNAL0 		= 3 		<< CE_CTL_KEY_SEL_SHIFT,    /*!< Not support for now */
	CE_CTL_KEYSOURCE_INTERNAL1 		= 4 		<< CE_CTL_KEY_SEL_SHIFT,    /*!< Not support for now */
	CE_CTL_KEYSOURCE_INTERNAL2 		= 5 		<< CE_CTL_KEY_SEL_SHIFT,    /*!< Not support for now */
	CE_CTL_KEYSOURCE_INTERNAL3 		= 6 		<< CE_CTL_KEY_SEL_SHIFT,    /*!< Not support for now */
	CE_CTL_KEYSOURCE_INTERNAL4 		= 7 		<< CE_CTL_KEY_SEL_SHIFT,    /*!< Not support for now */
	CE_CTL_KEYSOURCE_INTERNAL5 		= 8 		<< CE_CTL_KEY_SEL_SHIFT,    /*!< Not support for now */
	CE_CTL_KEYSOURCE_INTERNAL6 		= 9 		<< CE_CTL_KEY_SEL_SHIFT,    /*!< Not support for now */
	CE_CTL_KEYSOURCE_INTERNAL7 		= 10 		<< CE_CTL_KEY_SEL_SHIFT     /*!< Not support for now */
} CE_CTL_KeySource;

#define CE_CTL_PRNG_MODE_SHIFT					(15)
#define CE_CTL_PRNG_MODE_MASK					(0x1U << CE_CTL_PRNG_MODE_SHIFT)

#define CE_CTL_CRC_CONT_SHIFT					(15)
#define CE_CTL_CRC_CONT_MASK					(0x1U << CE_CTL_CRC_CONT_SHIFT)

#define CE_CTL_IV_MODE_SHIFT					(14)
#define CE_CTL_IV_MODE_MASK						(0x1U << CE_CTL_IV_MODE_SHIFT)
typedef enum {
	CE_CTL_IVMODE_SHA_MD5_FIPS180 	= 0 		<< CE_CTL_IV_MODE_SHIFT,	/*!< IV in FIPS-180 defined */
	CE_CTL_IVMODE_SHA_MD5_INPUT 	= 1 		<< CE_CTL_IV_MODE_SHIFT     /*!< IV from input */
} CE_CTL_IvMode_SHA_MD5;

#define CE_CTL_OP_MODE_SHIFT					(12)
#define CE_CTL_OP_MODE_MASK						(0x3U << CE_CTL_OP_MODE_SHIFT)
typedef enum {
	CE_CTL_CRYPT_MODE_ECB 			= 0 		<< CE_CTL_OP_MODE_SHIFT,	/*!< Crypto mode: ECB mode */
	CE_CTL_CRYPT_MODE_CBC 			= 1 		<< CE_CTL_OP_MODE_SHIFT,    /*!< Crypto mode: CBC mode */
	CE_CTL_CRYPT_MODE_CTR 			= 2 		<< CE_CTL_OP_MODE_SHIFT,    /*!< Crypto mode: CTR mode, only AES can support */
	CE_CTL_CRYPT_MODE_CTS 			= 3 		<< CE_CTL_OP_MODE_SHIFT     /*!< Crypto mode: CTS mode, not support for now */
} CE_CTL_Crypto_Mode;

#define CE_CTL_CTR_WIDTH_SHIFT					(10)
#define CE_CTL_CTR_WIDTH_MASK					(0x3U << CE_CTL_CTR_WIDTH_SHIFT)
typedef enum {
	CE_CTL_CTRWITDH_16BITS 			= 0 		<< CE_CTL_CTR_WIDTH_SHIFT,	/*!< CTR mode counter size: 16 bits */
	CE_CTL_CTRWITDH_32BITS 			= 1 		<< CE_CTL_CTR_WIDTH_SHIFT,  /*!< CTR mode counter size: 32 bits */
	CE_CTL_CTRWITDH_64BITS 			= 2 		<< CE_CTL_CTR_WIDTH_SHIFT,  /*!< CTR mode counter size: 64 bits */
	CE_CTL_CTRWITDH_128BITS 		= 3 		<< CE_CTL_CTR_WIDTH_SHIFT   /*!< CTR mode counter size: 128 bits */
} CE_CTL_CtrWidth;

#define CE_CTL_AES_KEY_SIZE_SHIFT				(8)
#define CE_CTL_AES_KEY_SIZE_MASK				(0x3U << CE_CTL_AES_KEY_SIZE_SHIFT)
typedef enum {
	CE_CTL_AES_KEYSIZE_128BITS 		= 0 		<< CE_CTL_AES_KEY_SIZE_SHIFT,	/*!< AES type: AES128 */
	CE_CTL_AES_KEYSIZE_192BITS 		= 1 		<< CE_CTL_AES_KEY_SIZE_SHIFT,   /*!< AES type: AES192 */
	CE_CTL_AES_KEYSIZE_256BITS 		= 2 		<< CE_CTL_AES_KEY_SIZE_SHIFT    /*!< AES type: AES256 */
} CE_CTL_AES_KeySize;

#define CE_CTL_OP_DIR_SHIFT						(7)
#define CE_CTL_OP_DIR_MASK						(0x1U << CE_CTL_OP_DIR_SHIFT)
typedef enum {
	CE_CRYPT_OP_ENCRYPTION 			= 0 		<< CE_CTL_OP_DIR_SHIFT,		/*!< Encryption operation */
	CE_CRYPT_OP_DECRYPTION 			= 1 		<< CE_CTL_OP_DIR_SHIFT      /*!< Decryption operation */
} CE_Crypto_Op;

#define CE_CTL_METHOD_SHIFT						(4)
#define CE_CTL_METHOD_MASK						(0x7U << CE_CTL_METHOD_SHIFT)
typedef enum {
	CE_CTL_METHOD_AES 				= 0 		<< CE_CTL_METHOD_SHIFT,		/*!< AES 	*/
	CE_CTL_METHOD_DES 				= 1 		<< CE_CTL_METHOD_SHIFT,     /*!< DES 	*/
	CE_CTL_METHOD_3DES	 			= 2 		<< CE_CTL_METHOD_SHIFT,     /*!< 3DES	*/
	CE_CTL_METHOD_SHA1 				= 3 		<< CE_CTL_METHOD_SHIFT,     /*!< SHA1 	*/
	CE_CTL_METHOD_MD5				= 4 		<< CE_CTL_METHOD_SHIFT,     /*!< MD5	*/
	CE_CTL_METHOD_PRNG				= 5 		<< CE_CTL_METHOD_SHIFT,     /*!< PRNG	*/
	CE_CTL_METHOD_SHA256			= 6 		<< CE_CTL_METHOD_SHIFT,     /*!< SHA256 */
	CE_CTL_METHOD_CRC				= 7 		<< CE_CTL_METHOD_SHIFT      /*!< CRC	*/
} CE_CTL_Method;

#define CE_CTL_CRC_WIDTH_SHIFT					(3)
#define CE_CTL_CRC_WIDTH_MASK					(0x1U << CE_CTL_CRC_WIDTH_SHIFT)
typedef enum {
	CE_CTL_CRC_WIDTH_16BITS 		= 0 		<< CE_CTL_CRC_WIDTH_SHIFT,	/*!< CRC type: 16bits CRC */
	CE_CTL_CRC_WIDTH_32BITS 		= 1 		<< CE_CTL_CRC_WIDTH_SHIFT   /*!< CRC type: 32bits CRC */
} CE_CTL_CRC_Width;

#define CE_CTL_END_BIT_SHIFT					(2)
#define CE_CTL_END_BIT_MASK						(0x1U << CE_CTL_END_BIT_SHIFT)

#define CE_CTL_PRNG_START_SHIFT					(1)
#define CE_CTL_PRNG_START_MASK					(0x1U << CE_CTL_PRNG_START_SHIFT)

#define CE_CTL_ENABLE_SHIFT						(0)
#define CE_CTL_ENABLE_MASK						(0x1U << CE_CTL_ENABLE_SHIFT)


#define CE_FCSR_RXFIFO_STATUS_SHIFT				(30)
#define CE_FCSR_RXFIFO_STATUS_MASK				(0x1U << CE_FCSR_RXFIFO_STATUS_SHIFT)

#define CE_FCSR_RXFIFO_EMP_CNT_SHIFT			(24)
#define CE_FCSR_RXFIFO_EMP_CNT_MASK				(0x3FU << CE_FCSR_RXFIFO_EMP_CNT_SHIFT)
#define CE_FCSR_RXFIFO_EMP_CNT_VMASK			(0x3FU)

#define CE_FCSR_TXFIFO_STATUS_SHIFT				(22)
#define CE_FCSR_TXFIFO_STATUS_MASK				(0x1U << CE_FCSR_TXFIFO_STATUS_SHIFT)

#define CE_FCSR_TXFIFO_AVA_CNT_SHIFT			(16)
#define CE_FCSR_TXFIFO_AVA_CNT_MASK				(0x3FU << CE_FCSR_TXFIFO_AVA_CNT_SHIFT)
#define CE_FCSR_TXFIFO_AVA_CNT_VMASK			(0x3FU)

#define CE_FCSR_RXFIFO_INT_TRIG_LEVEL_SHIFT		(8)
#define CE_FCSR_RXFIFO_INT_TRIG_LEVEL_MASK		(0x1FU << CE_FCSR_RXFIFO_INT_TRIG_LEVEL_SHIFT)

#define CE_FCSR_TXFIFO_INT_TRIG_LEVEL_SHIFT		(0)
#define CE_FCSR_TXFIFO_INT_TRIG_LEVEL_MASK		(0x1FU << CE_FCSR_TXFIFO_INT_TRIG_LEVEL_SHIFT)


#define CE_ICSR_HASH_CRC_END_SHIFT				(11)
#define CE_ICSR_HASH_CRC_END_MASK				(0x1U << CE_ICSR_HASH_CRC_END_SHIFT)

#define CE_ICSR_RXFIFO_EMP_SHIFT				(10)
#define CE_ICSR_RXFIFO_EMP_MASK					(0x1U << CE_ICSR_RXFIFO_EMP_SHIFT)

#define CE_ICSR_TXFIFO_AVA_SHIFT				(8)
#define CE_ICSR_TXFIFO_AVA_MASK					(0x1U << CE_ICSR_TXFIFO_AVA_SHIFT)

#define CE_ICSR_DRQ_ENABLE_SHIFT				(4)
#define CE_ICSR_DRQ_ENABLE_MASK					(0x1U << CE_ICSR_DRQ_ENABLE_SHIFT)

#define CE_ICSR_HASH_CRC_END_INT_EN_SHIFT		(3)
#define CE_ICSR_HASH_CRC_END_INT_EN_MASK		(0x1U << CE_ICSR_HASH_CRC_END_INT_EN_SHIFT)

#define CE_ICSR_RXFIFO_EMP_INT_EN_SHIFT			(2)
#define CE_ICSR_RXFIFO_EMP_INT_EN_MASK			(0x1U << CE_ICSR_RXFIFO_EMP_INT_EN_SHIFT)

#define CE_ICSR_TXFIFO_AVA_INT_EN_SHIFT			(0)
#define CE_ICSR_TXFIFO_AVA_INT_EN_MASK			(0x1U << CE_ICSR_TXFIFO_AVA_INT_EN_SHIFT)


#define CE_3DES_KEY_COUNT 						(6)
#define CE_DES_KEY_COUNT 						(2)


/************************ public **************************************/
#define CE_WAIT_TIME (4000)

/*
 * @brief aes/des/3des
 * @notice Only support ECB & CBC mode. CTR & CTS mode have hardware bug.
 */
typedef enum {
	CE_CRYPT_MODE_ECB = CE_CTL_CRYPT_MODE_ECB,		 /*!< Crypto Mode: ECB mode */
	CE_CRYPT_MODE_CBC = CE_CTL_CRYPT_MODE_CBC        /*!< Crypto Mode: CBC mode */
} CE_Crypto_Mode;

typedef CE_CTL_KeySource CE_Crypto_KeySrc;
typedef CE_CTL_AES_KeySize CE_AES_KeySize;

#define AES_BLOCK_SIZE 			(16)
#define DES_BLOCK_SIZE			(8)
#define DES3_BLOCK_SIZE			(8)

typedef struct {
	struct {
		CE_Crypto_Mode mode;	    /*!< ECB & CBC */
		struct {
			uint8_t iv[16];         /*!< initialization value, same as a block size which is 16 bytes */
#ifdef CE_CTR_MODE
			uint8_t cnt[16];
#endif
		};
#ifdef CE_CTR_MODE
		CE_CTL_CtrWidth width;
#endif
	};

	struct {
		CE_Crypto_KeySrc src;       /*!< key source */
		uint8_t key[32];            /*!< key */
		CE_CTL_AES_KeySize keysize;	/*!< AES key size according to AES type like AES128 */
	};
} CE_AES_Config;

typedef struct {
	struct {
		CE_Crypto_Mode mode;        /*!< ECB & CBC */
		union {
			uint8_t iv[8];          /*!< initialization value, same as a block size which is 8 bytes */
#ifdef CE_CTR_MODE
			uint8_t cnt[8];
#endif
		};
#ifdef CE_CTR_MODE
		CE_CTL_CtrWidth width;
#endif
	};

	struct {
		CE_Crypto_KeySrc src;       /*!< key source */
		uint8_t key[8];             /*!< key */
	};
} CE_DES_Config;

typedef struct {
	struct {
		CE_Crypto_Mode mode;        /*!< ECB & CBC */
		union {
			uint8_t iv[8];          /*!< initialization value, same as a block size which is 8 bytes */
#ifdef CE_CTR_MODE
			uint8_t cnt[8];
#endif
		};
#ifdef CE_CTR_MODE
		CE_CTL_CtrWidth width;
#endif
	};

	struct {
		CE_Crypto_KeySrc src;       /*!< key source */
		uint8_t key[24];            /*!< key */
	};
} CE_3DES_Config;


typedef struct {
	uint8_t word[4];
	uint8_t word_size;
} CE_Fifo_Align;

/*
 * @brief crc
 */
typedef enum {
	CE_CRC16_IBM,     /*!< x16+x15+x2+1, poly 0x8005, init 0x0000, refin-true, refout-true, xorout-0x0000 */
	CE_CRC16_MAXIM,   /*!< x16+x15+x2+1, poly 0x8005, init 0x0000, refin-true, refout-true, xorout-0xffff */
	CE_CRC16_USB,     /*!< x16+x15+x2+1, poly 0x8005, init 0xffff, refin-true, refout-true, xorout-0xffff */
	CE_CRC16_MODBUS,  /*!< x16+x15+x2+1, poly 0x8005, init 0xffff, refin-true, refout-true, xorout-0x0000 */
	CE_CRC16_CCITT,   /*!< x16+x15+x5+1, poly 0x1021, init 0x0000, refin-true, refout-true, xorout-0x0000 */
	CE_CRC16_CCITT_1, /*!< x16+x15+x5+1, poly 0x1021, init 0xffff, refin-false, refout-false, xorout-0x0000 */
	CE_CRC16_X25,     /*!< x16+x15+x5+1, poly 0x1021, init 0xffff, refin-true, refout-true, xorout-0xffff */
	CE_CRC16_XMODEM,  /*!< x16+x15+x5+1, poly 0x1021, init 0x0000, refin-false, refout-false, xorout-0x0000 */
	CE_CRC16_DNP,     /*!< x16+x13+x12+x11+x10+x8+x6+x5+x2+1, poly 0x3d65, init 0x0000, refin-true, refout-true, xorout-0xffff */
	CE_CRC32,         /*!< x32+x26+x23+x22+x16+x12+x11+x10+x8+x7+x5+x4+x2+x+1, poly 0x04c11db7, init 0xffffffff, refin-true, refout-true, xorout-0xffffffff */
	CE_CRC32_MPEG2,   /*!< x32+x26+x23+x22+x16+x12+x11+x10+x8+x7+x5+x4+x2+x+1, poly 0x04c11db7, init 0xffffffff, refin-false, refout-false, xorout-0x00000000 */
} CE_CRC_Types;

typedef struct {
	uint64_t total_size;
	struct {
		uint8_t word[4];
		uint8_t word_size;
	};

	CE_CRC_Types type;
	uint32_t crc;
} CE_CRC_Handler;

/*
 * @brief md5/sha1/sha256
 */
typedef CE_CTL_IvMode_SHA_MD5 CE_Hash_IVsrc;

#define CE_MD5_IV_SIZE 			(4)
#define CE_SHA1_IV_SIZE		 	(5)
#define CE_SHA256_IV_SIZE 		(8)

typedef struct {
	uint64_t total_size;
	struct {
		uint8_t word[4];
		uint8_t word_size;
	};
} CE_MD5_Handler, CE_SHA1_Handler, CE_SHA256_Handler;


/********************************** Public Interface *************************************/
HAL_Status HAL_CE_Init();
HAL_Status HAL_CE_Deinit();

HAL_Status HAL_AES_Encrypt(CE_AES_Config *aes, uint8_t *plain, uint8_t *cipher, uint32_t size);
HAL_Status HAL_AES_Decrypt(CE_AES_Config *aes, uint8_t *cipher, uint8_t *plain, uint32_t size);

HAL_Status HAL_DES_Encrypt(CE_DES_Config *des, uint8_t *plain, uint8_t *cipher, uint32_t size);
HAL_Status HAL_DES_Decrypt(CE_DES_Config *des, uint8_t *cipher, uint8_t *plain, uint32_t size);

HAL_Status HAL_3DES_Encrypt(CE_3DES_Config *des, uint8_t *plain, uint8_t *cipher, uint32_t size);
HAL_Status HAL_3DES_Decrypt(CE_3DES_Config *des, uint8_t *cipher, uint8_t *plain, uint32_t size);

HAL_Status HAL_CRC_Init(CE_CRC_Handler *hdl, CE_CRC_Types type, uint32_t total_size);
HAL_Status HAL_CRC_Append(CE_CRC_Handler *hdl, uint8_t *data, uint32_t size);
HAL_Status HAL_CRC_Finish(CE_CRC_Handler *hdl, uint32_t *crc);

HAL_Status HAL_MD5_Init(CE_MD5_Handler *hdl, CE_Hash_IVsrc src, uint32_t iv[4]);
HAL_Status HAL_MD5_Append(CE_MD5_Handler *hdl, uint8_t *data, uint32_t size);
HAL_Status HAL_MD5_Finish(CE_MD5_Handler *hdl, uint32_t digest[4]);

HAL_Status HAL_SHA1_Init(CE_SHA1_Handler *hdl, CE_Hash_IVsrc src, uint32_t iv[5]);
HAL_Status HAL_SHA1_Append(CE_SHA1_Handler *hdl, uint8_t *data, uint32_t size);
HAL_Status HAL_SHA1_Finish(CE_SHA1_Handler *hdl, uint32_t digest[5]);

HAL_Status HAL_SHA256_Init(CE_SHA256_Handler *hdl, CE_Hash_IVsrc src, uint32_t iv[8]);
HAL_Status HAL_SHA256_Append(CE_SHA256_Handler *hdl, uint8_t *data, uint32_t size);
HAL_Status HAL_SHA256_Finish(CE_SHA256_Handler *hdl, uint32_t digest[8]);

HAL_Status HAL_PRNG_Generate(uint8_t *random, uint32_t size);


#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_CHIP_HAL_CRYPTO_H_ */
