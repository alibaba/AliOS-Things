/**
  ******************************************************************************
  * @file    rtl8721d_crypto.h
  * @author
  * @version V1.0.0
  * @date    2017-09-15
  * @brief   This file contains all the functions prototypes for the IPsec firmware
  *          library
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#ifndef _RTL8721D_CRYPTO_H_
#define _RTL8721D_CRYPTO_H_

/** @addtogroup AmebaD_Periph_Driver
  * @{
  */

/** @defgroup CRYPTO
  * @brief CRYPTO driver modules
  * @{
  */

/** @addtogroup CRYPTO
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  *	-Authentication or Cipher use hardware to improve data process performance
  *	-Two mode: secure mode and non-secure mode
  *	-Register base address: Secure mode -0x5007xxxx, non-secure mode -0x4007xxxx
  *	-IPclk: 100MHz
  *****************************************************************************************
  * Authentication
  *****************************************************************************************
  *	-MD5
  *	-SHA1
  *	-SHA2
  *	-HMAC-MD5
  *	-HMAC-SHA1
  *	-HMAC-SHA2
  *	-Poly1305
  *	-Sequential hash
  *
  *****************************************************************************************
  * Cipher
  *****************************************************************************************   
  *	-AES-CBC
  *	-AES-ECB
  *	-AES-CFB
  *	-AES-OFB
  *	-AES-CTR
  *	-AES-GCM
  *	-3DES-CBC
  *	-3DES-ECB
  *	-3DES-CFB
  *	-3DES-OFB
  *	-3DES-CTR
  *	-DES-CBC
  *	-DES-ECB
  *	-DES-CFB
  *	-DES-OFB
  *	-DES-CTR
  *
  *****************************************************************************************    
  * How to use crypto
  *****************************************************************************************    
  * Method 1 (use the lowest level API)
  *****************************************************************************************    
  *	-call CRYPTO_Init to open IPSEC function & clock
  *
  *	-call following API for set key:
  *		-CRYPTO_SetSecurityModeAD
  *
  *	-call following API for authentication/encrypt/decrypt:
  *		-CRYPTO_ProcessAD
  *
  *****************************************************************************************
  * @endverbatim
  */
  
/* Exported Types --------------------------------------------------------*/

/** @defgroup CRYPTO_Exported_Types CRYPTO Exported Types
  * @{
  */

/** 
  * @brief  CRYPTO source descriptor structure definition
  */
typedef union {
	struct {
		u32 key_len:4;		/*Offset 0, Bit[3:0], key length				*/
		u32 iv_len:4;			/*Offset 0, Bit[7:3], IV length				*/
		u32 keypad_len:8;		/*Offset 0, Bit[15:8], pad length			*/
		u32 hash_iv_len:6;		/*Offset 0, Bit[21:16], Hash initial value length	*/
		u32 ap:2;				/*Offset 0, Bit[23:22], auto-padding		*/
		u32 cl:2;				/*Offset 0, Bit[25:24], Command length		*/
		u32 priv_key:1;		/*Offset 0, Bit[26], Secure Key				*/
		u32 otp_key:1;		/*Offset 0, Bit[27], Secure Key				*/
		u32 ls:1;				/*Offset 0, Bit[28], Last segment descriptor	*/
		u32 fs:1;				/*Offset 0, Bit[29], First segment descriptor	*/
		u32 rs:1;				/*Offset 0, Bit[30], Read data source 		*/
		u32 rsvd:1;			/*Offset 0, Bit[31], Reserved				*/
	} b;

	struct {
		u32 apl:8;			/*Offset 0, Bit[7:0], Auth padding length		*/
		u32 a2eo:5;			/*Offset 0, Bit[12:8], Auth to encryption offset	*/
		u32 zero:1;			/*Offset 0, Bit[13], 0/1					*/
		u32 enl:14;			/*Offset 0, Bit[27:14], Encryption data length	*/
		u32 ls:1;				/*Offset 0, Bit[28], Last segment descriptor	*/
		u32 fs:1;				/*Offset 0, Bit[29], First segment descriptor	*/
		u32 rs:1;				/*Offset 0, Bit[30], Read data source		*/
		u32 rsvd:1;			/*Offset 0, Bit[31], Reserved				*/
	} d;

	u32 w;
} rtl_crypto_srcdesc_t;

/** 
  * @brief  CRYPTO destination descriptor structure definition
  */
typedef union {
	struct {
		u32 adl:8;			/*Offset 0, Bit[7:0], Auth data length		*/
		u32 rsvd1:19;			/*Offset 0, Bit[26:8], Reserved				*/
		u32 enc:1;			/*Offset 0, Bit[27], Cipher or auth			*/
		u32 ls:1;				/*Offset 0, Bit[28], Last segment descriptor	*/
		u32 fs:1;				/*Offset 0, Bit[29], First segment descriptor	*/
		u32 ws:1;			/*Offset 0, Bit[30], Write data source		*/
		u32 rsvd2:1;			/*Offset 0, Bit[31], Reserved				*/
	} auth;

	struct {
		u32 enl:24;			/*Offset 0, Bit[23:0], Auth padding length		*/
		u32 rsvd1:3;			/*Offset 0, Bit[26:24], Reserved			*/
		u32 enc:1;			/*Offset 0, Bit[27], Cipher or auth			*/
		u32 ls:1;				/*Offset 0, Bit[28], Last segment descriptor	*/
		u32 fs:1;				/*Offset 0, Bit[29], First segment descriptor	*/
		u32 ws:1;			/*Offset 0, Bit[30], Write data source		*/
		u32 rsvd2:1;			/*Offset 0, Bit[31], Reserved				*/
	} cipher;

	u32 w;
} rtl_crypto_dstdesc_t;

/** 
  * @brief  CRYPTO command buffer structure definition
  */
typedef struct rtl_crypto_cl_struct_s {
	// offset 0 :
	u32 cipher_mode:4;			/*Offset 0, Bit[3:0], Cipher mode			*/
	u32 cipher_eng_sel:2;			/*Offset 0, Bit[5:4], Cipher algorithm selected	*/
	u32 rsvd1:1;					/*Offset 0, Bit[6], Reserved				*/
	u32 cipher_encrypt:1;			/*Offset 0, Bit[7], Encryption or decryption	*/
	u32 aes_key_sel:2;			/*Offset 0, Bit[9:8], AES key length			*/
	u32 des3_en:1;				/*Offset 0, Bit[10], 3DES					*/
	u32 des3_type:1;				/*Offset 0, Bit[11], 3DES type				*/
	u32 ckbs:1;					/*Offset 0, Bit[12], Cipher key byte swap		*/
	u32 hmac_en:1;				/*Offset 0, Bit[13], HMAC 				*/
	u32 hmac_mode:3;				/*Offset 0, Bit[16:14], Hash algorithm		*/
	u32 hmac_seq_hash_last:1;		/*Offset 0, Bit[17], the last payload(seq hash)	*/
	u32 engine_mode:3;			/*Offset 0, Bit[20:18], engine mode			*/
	u32 hmac_seq_hash_first:1;		/*Offset 0, Bit[21], the first payload(seq hash)	*/
	u32 hmac_seq_hash:1;			/*Offset 0, Bit[22], Seqential hash			*/
	u32 hmac_seq_hash_no_wb:1;	/*Offset 0, Bit[23], Result hash output		*/
	u32 icv_total_length:8;			/*Offset 0, Bit[31:24], icv length			*/

	// offset 4
	u32 aad_last_data_size:4;		/*Offset 4, Bit[3:0], AAD last block data size				*/
	u32 enc_last_data_size:4;		/*Offset 4, Bit[7:4], Message last block data size			*/
	u32 pad_last_data_size:3;		/*Offset 4, Bit[10:8], Hash padding last block data size		*/
	u32 ckws:1;					/*Offset 4, Bit[11], Cipher key word swap					*/
	u32 enc_pad_last_data_size:3;	/*Offset 4, Bit[14:12], Encryption padding last block data size	*/
	u32 hsibs:1;					/*Offset 4, Bit[15], Hash sequential initial value byte swap		*/
	u32 caws:1;					/*Offset 4, Bit[16], Cipher align word swap				*/
	u32 cabs:1;					/*Offset 4, Bit[17], Cipher align byte swap				*/
	u32 ciws:1;					/*Offset 4, Bit[18], Cipher input word swap				*/
	u32 cibs:1;					/*Offset 4, Bit[19], Cipher input byte swap				*/
	u32 cows:1;					/*Offset 4, Bit[20], Cipher output word swap				*/
	u32 cobs:1;					/*Offset 4, Bit[21], Cipher output byte swap				*/
	u32 codws:1;					/*Offset 4, Bit[22], Cipher output double word swap			*/
	u32 cidws:1;					/*Offset 4, Bit[23], Cipher input double word swap			*/
	u32 haws:1;					/*Offset 4, Bit[24], Hash align word swap					*/
	u32 habs:1;					/*Offset 4, Bit[25], Hash align byte swap					*/
	u32 hiws:1;					/*Offset 4, Bit[26], Hash input word swap					*/
	u32 hibs:1;					/*Offset 4, Bit[27], Hash input byte swap					*/
	u32 hows:1;					/*Offset 4, Bit[28], Hash output word swap				*/
	u32 hobs:1;					/*Offset 4, Bit[29], Hash output byte swap				*/
	u32 hkws:1;					/*Offset 4, Bit[30], Hash key word swap					*/
	u32 hkbs:1;					/*Offset 4, Bit[31], Hash key byte swap					*/

	// offset 8
	u32 hash_pad_len:8;			/*Offset 8, Bit[7:0], Hash padding total length		*/
	u32 header_total_len:6;			/*Offset 8, Bit[13:8], A2EO total length			*/
	u32 apl:2;					/*Offset 8, Bit[15:14], Encryption padding total length*/
	u32 enl:16;					/*Offset 8, Bit[15:14], Message total length		*/

	// offset
	u32 ap0;						/*Offset 12, Bit[31:0], Message length[31:0]		*/
	u32 ap1;						/*Offset 16, Bit[31:0], Message length[63:32]		*/
	u32 ap2;						/*Offset 20, Bit[31:0], Message length[95:64]		*/
	u32 ap3;						/*Offset 24, Bit[31:0], Message length[127:96]	*/
} rtl_crypto_cl_t;

/** 
  * @brief  CRYPTO adapter definition
  */
typedef struct _HAL_CRYPTO_ADAPTER_ {
	u8 isInit;						/*0: not init, 1: init							*/
	u8 isIntMode;					/*0: disable interrupt mode, 1: enable interrupt mode*/

	u32 cipher_type;				/*cipher type								*/
	u8 des;						/*DES									*/
	u8 trides;						/*3DES									*/
	u8 aes;						/*AES									*/
	u8 chacha;					/*ChaCha									*/
	u8 isDecrypt;					/*0: encryption, 1: decryption					*/

	u32 auth_type;				/*auth type								*/
	u8 isHMAC;					/*0: not HMAC, 1: HMAC						*/
	u8 isMD5;					/*MD5									*/
	u8 isSHA1;					/*SHA1									*/
	u8 isSHA2;					/*SHA2									*/
	u8 sha2type;					/*SHA2 type: SHA2_224/SHA2_256			*/

	u32 enc_last_data_size;		/*message last data size						*/
	u32 aad_last_data_size;		/*AAD last data size							*/

	u32 lenAuthKey;				/*Auth key length							*/
	const u8*  pAuthKey;			/*Auth key								*/
	u32 digestlen;					/*digest									*/

	// sequential hash
	u8 hmac_seq_hash_first;		/* seq hash the first message payload block		*/
	u8 hmac_seq_hash_last;		/* seq hash the last message payload block		*/
	u32 hmac_seq_hash_total_len;	/* seq hash message total length 				*/
	u8 hmac_seq_is_recorded;		/* enter seq hash or not						*/
	u8 *hmac_seq_last_message;	/* previous message payload					*/
	u32 hmac_seq_last_msglen;		/* previous message payload length				*/
	u8 hmac_seq_buf_is_used_bytes;/* seq buf used bytes(total 64-bytes)			*/

	u32 	lenCipherKey;				/*Cipher key length							*/
	const u8*  	pCipherKey;		/*Cipher key								*/

	u32 a2eo, apl_aad, enl, apl;		/*length									*/

	u8 *ipad;						/*HMAC ipad								*/
	u8 *opad;					/*HMAC opad								*/
	// crc
	//int crc_order;					/*CRC order								*/
	
	volatile u8 g_IOPAD[64*2+4] __attribute__((aligned(4)));
	volatile u8  gcm_iv[32] __attribute__((aligned(4)));
	volatile u8  cl_buffer[32] __attribute__((aligned(4)));
	volatile u8  hmac_seq_buf[64] __attribute__((aligned(4)));
} HAL_CRYPTO_ADAPTER, *PHAL_CRYPTO_ADAPTER ;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup CRYPTO_Exported_Constants CRYPTO Exported Constants
  * @{
  */

/** @defgroup CRYPTO_LENGTH_definitions 
  * @{
  */ 
#define CRYPTO_MAX_DIGEST_LENGTH	32  /* SHA256 Digest length : 32 */
#define CRYPTO_MAX_KEY_LENGTH		32  /* MAX  is  AES-256 : 32 byte,  3DES : 24 byte */
#define CRYPTO_PADSIZE 				64
#define CRYPTO_AUTH_PADDING			64
#define CRYPTO_MD5_DIGEST_LENGTH 	16
#define CRYPTO_SHA1_DIGEST_LENGTH 	20
#define CRYPTO_SHA2_DIGEST_LENGTH 	32
/**
  * @}
  */

/** @defgroup CRYPTO_ATHENTICATION_TYPE_definitions 
  * @{
  */
#define AUTH_TYPE_NO_AUTH 				((u32)-1)

#define AUTH_TYPE_MASK_FUNC				0x3	/* bit 0, bit 1*/
#define AUTH_TYPE_MD5					0x2
#define AUTH_TYPE_SHA1					0x0
#define AUTH_TYPE_SHA2					0x1

#define AUTH_TYPE_MASK_HMAC 			0x4	/* bit 2 */
#define AUTH_TYPE_HMAC_MD5 				(AUTH_TYPE_MD5 | AUTH_TYPE_MASK_HMAC)
#define AUTH_TYPE_HMAC_SHA1 			(AUTH_TYPE_SHA1 | AUTH_TYPE_MASK_HMAC)
#define AUTH_TYPE_HMAC_SHA2 			(AUTH_TYPE_SHA2 | AUTH_TYPE_MASK_HMAC)

#define AUTH_TYPE_MASK_FUNC_ALL			(AUTH_TYPE_MASK_FUNC| AUTH_TYPE_MASK_HMAC)

// SHA2
#define AUTH_TYPE_MASK_SHA2 			0x30 /* bit 3,4 */
#define AUTH_TYPE_SHA2_224				0x10
#define AUTH_TYPE_SHA2_256  				0x20

#define AUTH_TYPE_SHA2_224_ALL  			(AUTH_TYPE_SHA2_224|AUTH_TYPE_SHA2)
#define AUTH_TYPE_SHA2_256_ALL  			(AUTH_TYPE_SHA2_256|AUTH_TYPE_SHA2)
#define AUTH_TYPE_HMAC_SHA2_224_ALL	(AUTH_TYPE_SHA2_224|AUTH_TYPE_HMAC_SHA2)
#define AUTH_TYPE_HMAC_SHA2_256_ALL	(AUTH_TYPE_SHA2_256|AUTH_TYPE_HMAC_SHA2)

typedef enum _SHA2_TYPE_ {
	SHA2_NONE 	= 0,
	SHA2_224 	= 224/8,
	SHA2_256 	= 256/8,
	SHA2_384 	= 384/8,
	SHA2_512 	= 512/8
} SHA2_TYPE;
/**
  * @}
  */

/** @defgroup CRYPTO_CIPHER_TYPE_definitions 
  * @{
  */
#define CIPHER_TYPE_NO_CIPHER 		((u32)-1)

#define CIPHER_TYPE_MODE_ENCRYPT 	0x80

#define CIPHER_TYPE_MASK_FUNC 		0x30 /* 0x00 : DES, 0x10: 3DES, 0x20: AES */
#define CIPHER_TYPE_FUNC_DES			0x00
#define CIPHER_TYPE_FUNC_3DES 		0x10
#define CIPHER_TYPE_FUNC_AES 		0x20
#define CIPHER_TYPE_FUNC_CHACHA 	0x30

#define CIPHER_TYPE_MASK_BLOCK 		0xF 	/*0x0 : ECB, 0x1: CBC, 0x2: CFB , 0x3 : OFB , 0x4 : CTR, 0x5 : GCTR, 0x6: GMAC, 0x7: GHASH, 0x8: GCM*/
#define CIPHER_TYPE_BLOCK_ECB 		0x0
#define CIPHER_TYPE_BLOCK_CBC 		0x1
#define CIPHER_TYPE_BLOCK_CFB 		0x2
#define CIPHER_TYPE_BLOCK_OFB 		0x3
#define CIPHER_TYPE_BLOCK_CTR 		0x4
#define CIPHER_TYPE_BLOCK_GCTR 		0x5
#define CIPHER_TYPE_BLOCK_GMAC 		0x6
#define CIPHER_TYPE_BLOCK_GHASH 	0x7
#define CIPHER_TYPE_BLOCK_GCM 		0x8
#define CIPHER_TYPE_BLOCK_CHACHA 	0x1
//
#define CIPHER_TYPE_DES_ECB			0x0
#define CIPHER_TYPE_DES_CBC 			0x1
#define CIPHER_TYPE_DES_CFB			0x2
#define CIPHER_TYPE_DES_OFB			0x3
#define CIPHER_TYPE_DES_CTR			0x4
#define CIPHER_TYPE_3DES_ECB			0x10
#define CIPHER_TYPE_3DES_CBC			0x11
#define CIPHER_TYPE_3DES_CFB			0x12
#define CIPHER_TYPE_3DES_OFB			0x13
#define CIPHER_TYPE_3DES_CTR			0x14
#define CIPHER_TYPE_AES_ECB			0x20
#define CIPHER_TYPE_AES_CBC			0x21
#define CIPHER_TYPE_AES_CFB			0x22
#define CIPHER_TYPE_AES_OFB			0x23
#define CIPHER_TYPE_AES_CTR			0x24  // 0x29
#define CIPHER_TYPE_AES_GCTR			0x25
#define CIPHER_TYPE_AES_GMAC		0x26
#define CIPHER_TYPE_AES_GHASH		0x27
#define CIPHER_TYPE_AES_GCM			0x28

#define CIPHER_TYPE_CHACHA_POLY1305	0x30 // chacha+poly1305
#define CIPHER_TYPE_CHACHA			0x31
#define CIPHER_TYPE_POLY1305			0x33 // poly1305 - mode 2

/**
  * @}
  */
  
/**
  * @}
  */ 

/* Exported functions --------------------------------------------------------*/
/**************************************************************************//**
 * @defgroup CRYPTO_Exported_Functions CRYPTO Exported Functions
 * @{
 *****************************************************************************/

/** @defgroup Crypto_Normal_Functions
  * @{
  */
_LONG_CALL_ void CRYPTO_MemDump(const u8 *start, u32 size, char * strHeader);
_LONG_CALL_ void CRYPTO_RegDump(void);
_LONG_CALL_ void CRYPTO_CleanCmdOk(void);
_LONG_CALL_ void CRYPTO_ClearAllINT(void);
_LONG_CALL_ void CRYPTO_Reset(HAL_CRYPTO_ADAPTER *pIE);
_LONG_CALL_ int CRYPTO_SetSecurityModeAD(HAL_CRYPTO_ADAPTER *pIE, IN const u32 cipher_type, IN const u32 auth_type,IN const void* pCipherKey, IN const u32 lenCipherKey, IN const void* pAuthKey, IN const u32 lenAuthKey);
_LONG_CALL_ int CRYPTO_Init(HAL_CRYPTO_ADAPTER *pIE);
_LONG_CALL_ int CRYPTO_ProcessAD(HAL_CRYPTO_ADAPTER *pIE, IN const u8 *message, IN const u32 msglen, IN const u8 *pIv, IN const u32 ivlen, IN const u8 *paad, IN const u32 aadlen, OUT u8 *pResult, OUT u8 *pTag);
_LONG_CALL_ int CRYPTO_CipherInit(HAL_CRYPTO_ADAPTER *pIE, IN const u32 cipher_type, IN const u8 *key, IN const u32 keylen);
_LONG_CALL_ int CRYPTO_CipherEncryptAD(HAL_CRYPTO_ADAPTER *pIE, IN const u8* message, IN const u32 msglen, IN const u8* piv, IN const u32 ivlen, IN const u8* paad, IN const u32 aadlen, OUT u8* pResult, OUT u8* pTag);
_LONG_CALL_ int CRYPTO_CipherDecryptAD(HAL_CRYPTO_ADAPTER *pIE, IN const u8* message, IN const u32 msglen, IN const u8* piv, IN const u32 ivlen, IN const u8* paad, IN const u32 aadlen, OUT u8* pResult, OUT u8* pTag);
 _LONG_CALL_ int CRYPTO_SendSeqBuf(u8 *pDigest);
/**
  * @}
  */

/**
  * @}
  */

/* Registers Definitions --------------------------------------------------------*/
/**************************************************************************//**
 * @defgroup CRYPTO_Register_Definitions CRYPTO Register Definitions
 * @{
 *****************************************************************************/
 
/**************************************************************************//**
 * @defgroup CRYPTO_SDSR
 * @{
 *****************************************************************************/
#define SFIFO_EMPTY_CNT  						((u32)0x000000FF)		/* Bit[7:0], Source Descriptor FIFO empty counter         	 */
#define SWPTR								((u32)0x0000FF00)		/* Bit[15:8], Source Descriptor FIFO write pointer         	*/
#define SRPTR									((u32)0x00FF0000)		/* Bit[23:16], Source Descriptor FIFO read pointer      	*/
#define SRC_FAIL								((u32)0x00000001<<24)	/* Bit[24], Source Descriptor fail interrupt                   	*/
#define SRC_FAIL_STATUS						((u32)0x00000003<<25)	/* Bit[26:25], Source Descriptor fail status                  	*/
#define SRC_FAIL_M							((u32)0x00000001<<27)	/* Bit[27], Source Descriptor fail interrupt mask            	*/
#define PK_UP									((u32)0x00000001<<30)	/* Bit[30], packet base update wptr to engine                 	*/
#define SRC_RST								((u32)0x00000001<<31)	/* Bit[31], Source Descriptor reset(only for pk_up=1'b1)	*/
/** @} */

/**************************************************************************//**
 * @defgroup CRYPTO_RST_ISR_CON
 * @{
 *****************************************************************************/
#define SOFT_RST								((u32)0x00000001)		/* Bit[0], Software Reset write 1 to reset									*/
#define DMA_BUSY								((u32)0x00000001<<3)	/* Bit[3], Ipsec dma busy												*/
#define CMD_OK								((u32)0x00000001<<4)	/* Bit[4], Command OK interrupt										*/
#define INTR_MODE							((u32)0x00000001<<7)	/* Bit[7], Select ok interrupt mode										*/
#define INTR_NORMAL_MODE					((u32)0x00000000<<7)	/* Bit[7], interrupt normal mode										*/
#define INTR_COUNT_MODE						((u32)0x00000001<<7)	/* Bit[7], interrupt counter mode										*/
#define OK_INTR_CNT							((u32)0x000000FF<<8)	/* Bit[15:8], OK interrupt counter										*/
#define CLEAR_OK_INTR_NUM					((u32)0x000000FF<<16)	/* Bit[23:16], Clear OK interrupt number									*/
#define IPSEC_RST								((u32)0x00000001<<31)	/* Bit[31], Ipsec engine Reset Write 1 to reset the crypto engine and DMA engine	*/
/** @} */

/**************************************************************************//**
 * @defgroup CRYPTO_IMR
 * @{
 *****************************************************************************/
#define CMD_OK_M								((u32)0x00000001)		/* Bit[0], Command OK interrupt Mask				*/
#define SRC_ERR0_M							((u32)0x00000001<<3)	/* Bit[3], Source Descriptor Error 0 Interrupt Mask		*/
#define SRC_ERR1_M							((u32)0x00000001<<4)	/* Bit[4], Source Descriptor Error 1 Interrupt Mask		*/
#define SRC_ERR2_M							((u32)0x00000001<<5)	/* Bit[5], Source Descriptor Error 2 Interrupt Mask		*/
#define SRC_ERR3_M							((u32)0x00000001<<6)	/* Bit[6], Source Descriptor Error 3 Interrupt Mask		*/
#define SRC_ERR4_M							((u32)0x00000001<<7)	/* Bit[7], Source Descriptor Error 4 Interrupt Mask		*/
#define SRC_ERR5_M							((u32)0x00000001<<8)	/* Bit[8], Source Descriptor Error 5 Interrupt Mask		*/
#define SRC_ERR6_M							((u32)0x00000001<<9)	/* Bit[9], Source Descriptor Error 6 Interrupt Mask		*/
#define SRC_ERR7_M 							((u32)0x00000001<<10)	/* Bit[10], Source Descriptor Error 7 Interrupt Mask		*/
#define SRC_ERR8_M							((u32)0x00000001<<11)	/* Bit[11], Source Descriptor Error 8 Interrupt Mask		*/
#define SRC_ERR9_M 							((u32)0x00000001<<12)	/* Bit[12], Source Descriptor Error 9 Interrupt Mask		*/
#define DST_ERR1_M							((u32)0x00000001<<13)	/* Bit[13], Destination Descriptor Error 1 Interrupt Mask	*/
#define DST_ERR2_M							((u32)0x00000001<<14)	/* Bit[14], Destination Descriptor Error 2 Interrupt Mask	*/
#define DST_ERR3_M 							((u32)0x00000001<<15)	/* Bit[15], Destination Descriptor Error 3 Interrupt Mask	*/
#define DST_ERR4_M							((u32)0x00000001<<16)	/* Bit[16], Destination Descriptor Error 4 Interrupt Mask	*/
#define DST_ERR5_M							((u32)0x00000001<<17)	/* Bit[17], Destination Descriptor Error 5 Interrupt Mask	*/
#define DST_ERR6_M							((u32)0x00000001<<18)	/* Bit[18], Destination Descriptor Error 6 Interrupt Mask	*/
/** @} */

/**************************************************************************//**
 * @defgroup CRYPTO_DEBUG
 * @{
 *****************************************************************************/								
#define DMA_WAIT_CYCLE						((u32)0x0000FFFF)		/* Bit[15:0], Wait dma_wait_cycle to assert next dma request	*/
#define ARBITER_MODE							((u32)0x00000001<<16)	/* Bit[16], dma arbiter mode							*/
#define DEBUG_PORT_SEL						((u32)0x0000000F<<20)	/* Bit[23:20], dma arbiter mode						*/
#define ENGINE_CLK_EN						((u32)0x00000001<<24)	/* Bit[24], Ipsec Engine clock enable						*/
#define DEBUG_WB								((u32)0x00000001<<31)	/* Bit[31], Debug : write back mode						*/
/** @} */
/**************************************************************************//**
 * @defgroup CRYPTO_ERR_STAT
 * @{
 *****************************************************************************/
#define SRC_ERR0								((u32)0x00000001)		/* Bit[0], Source Descriptor Error 0 interrupt		*/
#define SRC_ERR1								((u32)0x00000001<<1)	/* Bit[1], Source Descriptor Error 1 interrupt		*/
#define SRC_ERR2								((u32)0x00000001<<2)	/* Bit[2], Source Descriptor Error 2 interrupt		*/
#define SRC_ERR3								((u32)0x00000001<<3)	/* Bit[3], Source Descriptor Error 3 interrupt		*/
#define SRC_ERR4								((u32)0x00000001<<4)	/* Bit[4], Source Descriptor Error 4 interrupt		*/
#define SRC_ERR5								((u32)0x00000001<<5)	/* Bit[5], Source Descriptor Error 5 interrupt		*/
#define SRC_ERR6								((u32)0x00000001<<6)	/* Bit[6], Source Descriptor Error 6 interrupt		*/
#define SRC_ERR7								((u32)0x00000001<<7)	/* Bit[7], Source Descriptor Error 7 interrupt		*/
#define SRC_ERR8								((u32)0x00000001<<8)	/* Bit[8], Source Descriptor Error 8 interrupt		*/
#define SRC_ERR9								((u32)0x00000001<<9)	/* Bit[9], Source Descriptor Error 9 interrupt		*/
#define DST_ERR1								((u32)0x00000001<<10)	/* Bit[10], Destination Descriptor Error 1 interrupt	*/
#define DST_ERR2								((u32)0x00000001<<11)	/* Bit[11], Destination Descriptor Error 2 interrupt	*/
#define DST_ERR3								((u32)0x00000001<<12)	/* Bit[12], Destination Descriptor Error 2 interrupt	*/
#define DST_ERR4								((u32)0x00000001<<13)	/* Bit[13], Destination Descriptor Error 4 interrupt	*/
#define DST_ERR5								((u32)0x00000001<<14)	/* Bit[14], Destination Descriptor Error 5 interrupt	*/
#define DST_ERR6								((u32)0x00000001<<15)	/* Bit[15], Destination Descriptor Error 6 interrupt	*/
/** @} */

/**************************************************************************//**
 * @defgroup CRYPTO_SWAP_BURST
 * @{
 *****************************************************************************/
#define SET_SWAP								((u32)0x00000001)		/* Bit[0], Byte swap for command setting data		*/
#define KEY_IV_SWAP							((u32)0x00000001<<1)	/* Bit[1], Byte swap for key and iv				*/
#define KEY_PAD_SWAP						((u32)0x00000001<<2)	/* Bit[2], Byte swap for hmac key				*/
#define HASH_INITIAL_VALUE_SWAP				((u32)0x00000001<<3)	/* Bit[3], Byte swap for sequential hash initial value	*/
#define DMA_IN_LITTLE_ENDIAN					((u32)0x00000001<<4)	/* Bit[4], Input data is little endian				*/
#define TX_BYTE_SWAP							((u32)0x00000001<<8)	/* Bit[8], Byte swap for dma_tx engine input data	*/
#define DATA_OUT_LITTLE_ENDIAN				((u32)0x00000001<<9)	/* Bit[9], Output data is little endian				*/
#define MAC_OUT_LITTLE_ENDIAN				((u32)0x00000001<<10)	/* Bit[10], Output mac is little endian			*/
#define RX_WD_SWAP							((u32)0x00000001<<11)	/* Bit[11], Word swap for dma_rx engine input data	*/
#define TX_WD_SWAP							((u32)0x00000001<<12)	/* Bit[12], Word swap for dma_tx engine input data	*/
#define DMA_BURST_LENGTH					((u32)0x0000003F<<16)	/* Bit[21:16], dma burst length				*/
/** @} */

/**************************************************************************//**
 * @defgroup CRYPTO_DDSR
 * @{
 *****************************************************************************/
#define DFIFO_EMPTY_CNT  						((u32)0x000000FF)		/* Bit[7:0], Destination Descriptor FIFO empty counter         	*/
#define DWPTR								((u32)0x0000FF00)		/* Bit[15:8], Destination Descriptor FIFO write pointer         	*/
#define DRPTR									((u32)0x00FF0000)		/* Bit[23:16], Destination Descriptor FIFO read pointer      	*/
#define DST_FAIL								((u32)0x00000001<<24)	/* Bit[24], Destination Descriptor fail interrupt                   	*/
#define DST_FAIL_STATUS						((u32)0x00000003<<25)	/* Bit[26:25], Destination Descriptor fail status                  	*/
#define DST_FAIL_M							((u32)0x00000001<<27)	/* Bit[27], Destination Descriptor fail interrupt mask            	*/
#define DST_RST								((u32)0x00000001<<31)	/* Bit[31], Destination Descriptor reset(only for pk_up=1'b1)	*/
/** @} */

/**************************************************************************//**
 * @defgroup CRYPTO_DESC_PKT_CONF
 * @{
 *****************************************************************************/
/********************  Bits definition for  register  *******************/
#define DBG_SPTR								(u32)0x000000FF)		/* Bit[7:0], Source Descriptor FIFO empty counter		*/
#define DBG_DPTR								(u32)0x0000FF00)		/* Bit[15:8], Destination Descriptor FIFO write pointer	*/
#define PK_ARBITER							((u32)0x00000003<<16)	/* Bit[17:16], Packet arbiter						*/ 
#define BUS1_PRIORITY_TH						((u32)0x00000003<<24)	/* Bit[25:24], Bus1 priority threshold				*/
#define BUS2_PRIORITY_TH						((u32)0x00000003<<26)	/* Bit[27:26], Bus2 priority threshold				*/ 
#define PK_ARBITER_MODE						((u32)0x00000003<<30)	/* Bit[31:30], Packet arbiter mode					*/
/** @} */

/** @} */

/**
  * @}
  */

/**
  * @}
  */ 

/* Other Definitions --------------------------------------------------------*/
#define FIFOCNT_TIMEOUT		0x100000
extern HAL_CRYPTO_ADAPTER crypto_engine;

__STATIC_INLINE
int rtl_cryptoEngine_init(void)
{
	return CRYPTO_Init(NULL);
}

/* *** dump macro definition *** */
#undef crypto_dbg_mem_dump
#define crypto_dbg_mem_dump(start, size, str_header) \
	if (ConfigDebug[LEVEL_INFO] & BIT(MODULE_IPSEC)) { \
		DiagPrintf("%s(): memdump : address: %08x, size: %d\n", "rtl_cryptoEngine_dbg", start, size); \
		CRYPTO_MemDump((const u8*)(start), size, (char*)(str_header)); \
	}
#endif
/******************* (C) COPYRIGHT 2017 Realtek Semiconductor *****END OF FILE****/

