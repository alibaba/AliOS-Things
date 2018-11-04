/**
 *
 * \file
 *
 * \brief APIs and data types for cryptographic algorithms.
 *
 * Copyright (c) 2016 - 2018 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef __CRYPTO_H__
#define __CRYPTO_H__

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
INCLUDES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#include "driver/include/m2m_types.h"


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
 MACROS
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#ifdef CONF_WILC_BIG_ENDIAN
#define GETU16(BUF,OFFSET)				(((uint16)((BUF)[OFFSET])) | (((uint16)((BUF)[OFFSET + 1]) << 8)))
/*!< Retrieve 2 bytes from the given buffer at the given 
	offset as 16-bit unsigned integer in the Network byte order.
*/


#define GETU32(BUF,OFFSET)				(((uint32)((BUF)[OFFSET])) | (((uint32)((BUF)[OFFSET + 1]) << 8))  | \
										 (((uint32)((BUF)[OFFSET + 2]) << 16)) | (((uint32)((BUF)[OFFSET + 3]) << 24)))
/*!< Retrieve 4 bytes from the given buffer at the given 
	offset as 32-bit unsigned integer in the Network byte order.
*/

#else
#define GETU16(BUF,OFFSET)				((((uint16)((BUF)[OFFSET]) << 8)) |(((uint16)(BUF)[OFFSET + 1])))
/*!< Retrieve 2 bytes from the given buffer at the given 
	offset as 16-bit unsigned integer in the Network byte order.
*/


#define GETU32(BUF,OFFSET)				((((uint32)((BUF)[OFFSET]) << 24)) | (((uint32)((BUF)[OFFSET + 1]) << 16)) | \
										(((uint32)((BUF)[OFFSET + 2]) << 8)) | ((uint32)((BUF)[OFFSET + 3])))
/*!< Retrieve 4 bytes from the given buffer at the given 
	offset as 32-bit unsigned integer in the Network byte order.
*/
#endif
#define PUTU32(VAL32,BUF,OFFSET)	\
	do	\
	{	\
		(BUF)[OFFSET	] = BYTE_3((VAL32));	\
		(BUF)[OFFSET +1 ] = BYTE_2((VAL32));	\
		(BUF)[OFFSET +2 ] = BYTE_1((VAL32));	\
		(BUF)[OFFSET +3 ] = BYTE_0((VAL32));	\
	}while(0)
	
	
#define PUTU16(VAL16,BUF,OFFSET)	\
	do	\
	{	\
		(BUF)[OFFSET	] = BYTE_1((VAL16));		\
		(BUF)[OFFSET +1 ] = BYTE_0((VAL16));		\
	}while(0)
	
										
#define M2M_MEMSET	m2m_memset
#define M2M_MEMCPY	m2m_memcpy


#define SHA_BLOCK_SIZE						(64)

#define MD4_DIGEST_SIZE					(16)
#define MD5_DIGEST_SIZE					(16)
#define SHA1_DIGEST_SIZE					(20)
#define SHA256_DIGEST_SIZE 				(32)

#define SHA_MAX_DIGEST_SIZE				SHA256_DIGEST_SIZE


/*======*======*
	HASH FLAGS
*======*======*/

#define SHA_FLAGS_INIT						0x01
/*!< A flag to tell the hash function to 
	start hash from the initial state.
*/

#define SHA_FLAGS_UPDATE					0x02
/*!< Update the current hash with the 
	given data.
*/


#define SHA_FLAGS_FINISH					0x04
/*!< Finalize the hashing and calculate
	the final result.
*/


#define SHA_FLAGS_FULL_HASH				(SHA_FLAGS_INIT | SHA_FLAGS_UPDATE | SHA_FLAGS_FINISH)
/*!< Perform Full hash operation with the DIGEST
	returned to the caller.
*/



#define SHA1_INIT(ctxt)						SHA1_Hash((ctxt), SHA_FLAGS_INIT, NULL, 0, NULL)
#define SHA1_UPDATE(ctxt,data,dataLen)		SHA1_Hash((ctxt), SHA_FLAGS_UPDATE, (data), (dataLen), NULL)
#define SHA1_FINISH(ctxt,digest)				SHA1_Hash((ctxt), SHA_FLAGS_FINISH, NULL, 0, digest)

#define MD5_INIT(ctxt)						MD5_Hash((ctxt), SHA_FLAGS_INIT, NULL, 0, NULL)
#define MD5_UPDATE(ctxt,data,dataLen)		MD5_Hash((ctxt), SHA_FLAGS_UPDATE, (data), (dataLen), NULL)
#define MD5_FINISH(ctxt,digest)				MD5_Hash((ctxt), SHA_FLAGS_FINISH, NULL, 0, digest)



#define AES_BLOCK_SIZE 						(16)

#define BI_SUCCESS							(0)
#define BI_FAIL								(-1)

#define BITS_PER_DIGIT						((uint32)32)


#define SHA_SHR(x,n) 						((x & 0xFFFFFFFF) >> n)
#define SHA_SHL(x,n) 						((x & 0xFFFFFFFF) << n)
#define SHA_ROTR(x,n) 						(SHA_SHR(x,n) | SHA_SHL(x , (32 - n)))
#define SHA_ROTL(x,n) 						(SHA_SHL(x,n) | SHA_SHR(x , (32 - n)))


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
 DATA TYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
typedef struct{
	uint16  u16BufferSize;
	uint8	*pu8Data;
}tstrBuffer;


/*!
@struct
@brief
*/
typedef struct{
	uint16	u16NSize;
	uint16	u16ESize;
	uint8	*pu8N;
	uint8	*pu8E;
}tstrRSAPublicKey;


/*!
@typedef
@brief
*/
typedef void (*tpfHashProcessBlock)(uint32* pu32HashState, const uint8* pu8MessageBlock);


/*!
@struct
@brief
*/
typedef struct{
	uint32				au32HashState[SHA_MAX_DIGEST_SIZE/4];
	uint8				au8CurrentBlock[SHA_BLOCK_SIZE];
	uint32				u32TotalLength;
	tpfHashProcessBlock	fpHash;
	uint8				u8BlockSize;
	uint8				u8DigestSize;
}tstrHashContext;


/*!
@typedef
@brief
*/
typedef tstrHashContext	tstrMd5Context;


/*!
@typedef
@brief
*/
typedef tstrHashContext	tstrSha1Context;


/*!
@typedef
@brief
*/
typedef tstrHashContext	tstrSha256Context;


/*!
@typedef
@brief
*/
typedef void (*tpfHash)
(
tstrHashContext	*pstrHashContext,		
uint8			u8Flags,
uint8			*pu8Data, 
uint32 		u32DataLength, 
uint8			*pu8Digest
);

#if 0
/*!
@struct
@brief
*/
typedef struct{
	uint32 	keys[60];
	uint32 	ikeys[60];
	uint32 	nrounds;
} aes_context_t;
#endif

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
FUNCTION PROTOTYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

void DES_EncryptBlock
(
uint8 	*pu8PlainTxt,
uint8	*pu8CipherKey,
uint8	*pu8CipherTxt
);



#ifndef __ROM_FIRMWARE__

/*======*======*======*
	HASH ALGORITHMS APIs
*======*======*======*/

void hmac_md5
(	
uint8		*pu8Data, 
uint32 	u32DataLength, 
uint8		*pu8Key, 
uint32 	u32KeyLength,
uint8		*pu8Digest
);
void MD5_Hash
(
tstrHashContext 	*pstrMD5Context, 
uint8 		u8Flags, 
uint8 		*pu8Data, 
uint32 		u32DataLength,
uint8 		*pu8Digest
);

void DRV_HMAC_Vector
(
tpfHash			fpHash,
uint8			*pu8Key,
uint32 		u32KeyLength, 
tstrBuffer			*pstrInData,
uint8			u8NumInputs,
uint8			*pu8Out
);


void DRV_SHA1_Hash
(
tstrHashContext	*pstrSha1Context,
uint8			u8Flags,
uint8 		*pu8Data, 
uint32 		u32DataLength, 
uint8 		*pu8Digest
);

void DRV_hmac_sha1
(	
uint8		*pu8Key, 
uint32 	u32KeyLength,
uint8		*pu8Data, 
uint32 	u32DataLength, 
uint8		*pu8Digest
);

void SHA256_Hash
(
tstrHashContext		*pstrSha256Context,
uint8				u8Flags,
uint8 			*pu8Data, 
uint32 			u32DataLength, 
uint8 			*pu8Digest
);


void HMAC_SHA256
(
uint8 	*pu8Data, 
uint32 	u32DataLength, 
uint8 	*pu8Key, 
uint32 	u32KeyLength, 
uint8 	*pu8Out
);


uint8 pbkdf2_sha1(uint8 *key, uint8 key_len,
                          uint8 *data, uint8 data_len,
                     uint8 *digest);


/*======*======*======*======*
	AES ENCRYPTION/DECRYPTION
*======*======*======*======*/




/*!
@fn				void AES_CbcDec(uint8 *pu8IV,uint8 *pu8Data,uint16 u16DataSize,uint8	 *pu8Key,uint8 u8KeySize)

@brief
				Decrypts a stream of data (in place) using AES algorithm in the Cipher Block Chain (CBC) mode.

@param [in]		pu8IV
					Initialization vector.

@param [in/out]	pu8Data
					Data to be decrypted.

@param [in]		u16DataSize
					Size of data in bytes.

@param [in]		pu8Key
					Decryption Key.

@param [in]		u8KeySize
					Size of the input key in bytes.

@author			Ahmed Ezzat

@date			11 Nov 2012

@version			1.0
*/
void AES_CbcDec
(
uint8 	*pu8IV,
uint8 	*pu8Data,
uint16	u16DataSize,
uint8	*pu8Key,
uint8	u8KeySize
);



/*!
@fn				void AES_CbcEnc(uint8 *pu8IV,uint8 *pu8Data,uint16 u16DataSize,uint8	 *pu8Key,uint8 u8KeySize)

@brief			Encrypts a stream of data (in place) using AES algorithm in the Cipher Block Chain (CBC) mode.

@param [in]		pu8IV
					Initialization vector.

@param [in/out]	pu8Data
					Data to be encrypted.

@param [in]		u16DataSize
					Size of data in bytes.

@param [in]		pu8Key
					Encryption Key.

@param [in]		u8KeySize
					Size of the input key in bytes.

@author			Ahmed Ezzat
@date			11 Nov 2012
@version			1.0
*/
void AES_CbcEnc
(
uint8 	*pu8IV,
uint8 	*pu8Data,
uint16	u16DataSize,
uint8	*pu8Key,
uint8	u8KeySize
);
#endif

#ifndef __ROM_FIRMWARE_V2__


void MD4_Hash
(
uint8		*pu8Data, 
uint32		u32DataLength, 
uint8		*pu8Digest
);



/*======*======*======*======*
		RSA CRYPTOGRAPHIC APIs
*======*======*======*======*/


/*!
@fn				uint16 HS_MessageBuild(uint8 u8MsgType, void * pvMsgParam,uint8 * pu8MsgBuffer)
*/
uint16 RSA_Encrypt
(
tstrRSAPublicKey 	*pstrKey , 
uint8 			*pu8Plain , 
uint16 			u16PlainSize , 
uint8 			*pu8Cipher
);


/*!
@fn				sint8 RSA_SignVerify(tstrRSAKey * pstrKey,uint8 *pu8Msg,uint16 u16MsgLen,uint8* pu8Signature,uint16 u16SignLen)
*/
sint8 RSA_SignVerify
(
tstrRSAPublicKey	*pstrKey,
uint8			*pu8Signature,
uint16 			u16SignLen,
uint8			*pu8Hash,
uint16 			u16HashLength
);



/*======*======*======*
	BIGINT LIBRARY APIs
*======*======*======*/


/*!
@fn	\			
	sint32 BI_ModExp(uint8	*pu8X, uint32 	u32XSize, uint8 *pu8E, uint32 u32ESize, uint8 *pu8N, uint32 u32NSize, uint8 *pu8Y, uint32 u32YSize);

@brief			
	Computes the modular exponentiation of big integers for the expression:
		Result = (Base ^ Power) % Mod.

@param [in]	pu32Base
				A big integer representing the base digit..

@param [in]	u32BaseSize
				Size of the base digit in 32-bit words.

@param [in]	pu32Power
				A big integer representing the exponent digit..

@param [in]	u32PowerSize
				Size of the exponent digit in 32-bit words.

@param [in]	pu32Mod
				A big integer representing the modulo digit..

@param [in]	u32ModSize
				Size of the modulo digit in 32-bit words.

@param [out]	pu32Result
				The resultant big integer.

@param [in]	u32ResultSize
				Size of the result digit in 32-bit words.

@author			
	Ahmed Ezzat

@date			
	27 Nov 2012
*/
sint32 BI_ModExp
(	
uint8	*pu8X,
uint32 	u32XSize,
uint8	*pu8E,
uint32 	u32ESize,
uint8	*pu8N,
uint32 	u32NSize,
uint8	*pu8Y,
uint32 	u32YSize
);

#endif

#endif /* CRYPTO_H_INCLUDED */
