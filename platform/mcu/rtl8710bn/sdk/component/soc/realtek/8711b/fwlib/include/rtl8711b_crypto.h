/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef __HAL_CRYPTO_RAM_H__
#define __HAL_CRYPTO_RAM_H__

// IPSecEngine
#define CRYPTO_MAX_DESP				8
#define CRYPTO_MAX_MSG_LENGTH		32000
#define CRYPTO_MD5_DIGEST_LENGTH		16
#define CRYPTO_SHA1_DIGEST_LENGTH	20
#define CRYPTO_SHA2_DIGEST_LENGTH	32

#define CRYPTO_MAX_DIGEST_LENGTH		32  // SHA256 Digest length : 32
#define CRYPTO_MAX_KEY_LENGTH			32  // MAX  is  AES-256 : 32 byte,  3DES : 24 byte
#define CRYPTO_PADSIZE					64
#define CRYPTO_AUTH_PADDING			64

typedef enum _SHA2_TYPE_ {
	SHA2_NONE 	= 0,
	SHA2_224 	= 224/8,
	SHA2_256 	= 256/8,
	SHA2_384 	= 384/8,
	SHA2_512 	= 512/8
} SHA2_TYPE;


/**************************************************************************
 * Definition for Parameters
 **************************************************************************/


#define HAL_CRYPTO_WRITE32(addr, value32)  HAL_WRITE32(CRYPTO_REG_BASE, (addr), (value32))
#define HAL_CRYPTO_WRITE16(addr, value16)  HAL_WRITE16(CRYPTO_REG_BASE, (addr), (value16))
#define HAL_CRYPTO_WRITE8(addr, value8)    HAL_WRITE8(CRYPTO_REG_BASE, (addr), (value8))
#define HAL_CRYPTO_READ32(addr)            HAL_READ32(CRYPTO_REG_BASE, (addr))
#define HAL_CRYPTO_READ16(addr)            HAL_READ16(CRYPTO_REG_BASE, (addr))
#define HAL_CRYPTO_READ8(addr)             HAL_READ8(CRYPTO_REG_BASE, (addr))


#define REG_IPSSDAR			(0x00)	// IPSec Source Descriptor Starting Address Register 
#define REG_IPSDDAR			(0x04)	// IPSec Destination Descriptor Starting Address Register 
#define REG_IPSCSR			(0x08)	// IPSec Command/Status Register 
#define REG_IPSCSDD			(0x10)	// IPSec CheckSum of Decrypted Data
#define REG_IPSCTR			(0x0C)	// IPSec Control Register 


/**************************************************************************
 * Type definition  for AES KEY
 **************************************************************************/

//
// For AES key calculation
// 

#define CIPHER_AES_MAXNR 14
#define CIPHER_AES_BLOCK_SIZE 16


enum _CIPHER_AES_KEYlEN
{
	_CIPHER_AES_KEYLEN_NONE = 0,
	_CIPHER_AES_KEYLEN_128BIT = 1,
	_CIPHER_AES_KEYLEN_192BIT = 2,
	_CIPHER_AES_KEYLEN_256BIT = 3,
};


typedef struct {
	u32 rd_key[4 *(CIPHER_AES_MAXNR + 1)];
	int rounds;
} RTL_SW_AES_KEY;

/**************************************************************************
 * Data Structure for Descriptor
 **************************************************************************/
#define _LITTLE_ENDIAN

#if (SYSTEM_ENDIAN==PLATFORM_LITTLE_ENDIAN)

typedef struct __rtl_crypto_source_s
{
	// offset 0 : 
	u32 sbl:15;
//	u32 resv2:1;
	u32 isSHA2:1;
	u32 md5:1;
	u32 hmac:1;
	//u32 auth_type:3; // hmac:1 + md5:2
	u32 ctr:1;
	u32 cbc:1;
	u32 trides:1;
	u32 aeskl:2;
	u32 kam:3;
	u32 ms:2;
	u32 resv1:1;
	u32 fs:1;
	u32 eor:1;
	u32 own:1;

	// offset 1 :
	u32 enl:15;
	u32 resv4:1;
	u32 a2eo:8;
	u32 resv3:5;
	u32 sha2:3;

	// offset 2:
	u32 resv6:16;
	u32 apl:8;
	u32 resv5:8;

	// offset 3:
	u32 sdbp;
}RTL_CRYPTO_SOURCE_T;

typedef struct __rtl_crypto_dest_s
{
	u32 dbl:15;
	u32 resv1:15;
	u32 eor:1;
	u32 own:1;

	u32 ddbp;

	u32 resv2;

	u32 icv[5+4]; // also set with IPSCTR's DDLEN 
} RTL_CRYPTO_DEST_T;

#else // SYSTEM_ENDIAN -> BIG_ENDIAN

typedef struct __rtl_crypto_source_s
{
	u32 own:1;
	u32 eor:1;
	u32 fs:1;
	u32 resv1:1;
	u32 ms:2;
	u32 kam:3;
	u32 aeskl:2;
	u32 trides:1;
	u32 cbc:1;
	u32 ctr:1;
	u32 auth_type:3; // hmac:1 + md5:2
//	u32 resv2:1;
	u32 sbl:15;

	u32 resv3:8;
	u32 a2eo:8;
	u32 resv4:1;
	u32 enl:15;

	u32 resv5:8;
	u32 apl:8;
	u32 resv6:16;

	u32 sdbp;
}  RTL_CRYPTO_SOURCE_T; 

typedef struct __rtl_crypto_dest_s
{
	u32 own:1;
	u32 eor:1;
	u32 resv1:15;
	u32 dbl:15;
	u32 ddbp;
	u32 resv2;
	u32 icv[5]; // changint with DDLEN
} RTL_CRYPTO_DEST_T;
#endif // SYSTEM_ENDIAN

/**************************************************************************
 * Data Structure for Cipher Adapter
 **************************************************************************/
typedef struct _HAL_CRYPTO_ADAPTER_
{
	u8 isInit;  // 0: not init, 1: init
	u8 dma_mode; // 2: 64 bytes;
	u8 sawb_mode;  // 0: disable sawb, 1:enable sawb
	u8 desc_num;  // total number of source/destination description

	u8 mode_select;

	u32 cipher_type;
	u8 des;
	u8 trides;
	u8 aes;
	u8 isDecrypt;

	u32 auth_type;
	u8 isHMAC;
	u8 isMD5;
	u8 isSHA1;
	u8 isSHA2;
	SHA2_TYPE sha2type;


	u32 lenAuthKey;
	const u8*  pAuthKey;
	u8  authKey[CRYPTO_MAX_DIGEST_LENGTH];
	u32 digestlen;

	// AES calculation
	RTL_SW_AES_KEY aes_key;

	u32 	lenCipherKey;
	const u8* pCipherKey;

	u32 a2eo, enl, apl; 


	u8 decryptKey[CRYPTO_MAX_KEY_LENGTH];

	u8 pad_to_align_1[16];	//pad to make g_IOPAD align 32
	u8 g_IOPAD[CRYPTO_PADSIZE*2+32];
	u8 *ipad;
	u8 *opad;

	u8 pad_to_align_2[24];	//pad to make g_authPadding align 32
	u8 g_authPadding[CRYPTO_AUTH_PADDING*2+32];

	// Source Descriptor
	u8 idx_srcdesc;
	RTL_CRYPTO_SOURCE_T *g_ipssdar;
	RTL_CRYPTO_SOURCE_T g_src_buf[CRYPTO_MAX_DESP];

	// Destination Descriptor
	u8 idx_dstdesc;
	RTL_CRYPTO_DEST_T *g_ipsddar;
	RTL_CRYPTO_DEST_T g_dst_buf[CRYPTO_MAX_DESP];
	u8 pad_to_align_3[16];	//pad to make _HAL_CRYPTO_ADAPTER_ size 1280
} HAL_CRYPTO_ADAPTER, *PHAL_CRYPTO_ADAPTER ;



/**************************************************************************
 * Definition for value and type
 **************************************************************************/

/* IPSec Command/Status Register */
#define IPS_SDUEIP		(1<<15)				/* Source Descriptor Unavailable Error Interrupt Pending */
#define IPS_SDLEIP		(1<<14)				/* Source Descriptor Length Error Interrupt Pending */
#define IPS_DDUEIP		(1<<13)				/* Destination Descriptor Unavailable Error Interrupt Pending */
#define IPS_DDOKIP		(1<<12)				/* Destination Descriptor OK Interrupt Pending */
#define IPS_DABFIP		(1<<11)				/* Data Address Buffer Interrupt Pending */
#define IPS_FIX_DMA_FAI	(1<<3)
#define IPS_RDMA_IDLE	(1<<2)
#define IPS_POLL		(1<<1)				/* Descriptor Polling. Set 1 to kick crypto engine to fetch source descriptor. */
#define IPS_SRST		(1<<0)				/* Software reset, write 1 to reset */

/* IPSec Control Register */
#define	IPS_CHKSUM_EN	(1<<28)				/* Enable check sum calculation of decryption data*/
//#define IPS_DDLEN_72	(7<<24)				/* Destination Descriptor Length : address offset to 72 : for SHA-512 */
#define IPS_DDLEN_20	(3<<24)				/* Destination Descriptor Length : Length is 20*DW : for SHA-1/MD5 */
#define IPS_DDLEN_16	(2<<24)				/* Destination Descriptor Length : Length is 16*DW : for SHA-1/MD5 */
#define IPS_DDLEN_12	(1<<24)				/* Destination Descriptor Length : Length is 12*DW : for SHA-1/MD5 */
#define IPS_DDLEN_8		(0<<24)				/* Destination Descriptor Length : Length is 8*DW : for SHA-1/MD5 */

#define IPS_SDUEIE		(1<<15)				/* Source Descriptor Unavailable Error Interrupt Enable */
#define IPS_SDLEIE		(1<<14)				/* Source Descriptor Length Error Interrupt Enable */
#define IPS_DDUEIE		(1<<13)				/* Destination Descriptor Unavailable Error Interrupt Enable */
#define IPS_DDOKIE		(1<<12)				/* Destination Descriptor OK Interrupt Enable */
#define IPS_DABFIE		(1<<11)				/* Data Address Buffer Interrupt Enable */
#define IPS_LXBIG		(1<<9)				/* Lx bus data endian*/
#define IPS_LBKM		(1<<8)				/* Loopback mode enable */
#define IPS_SAWB		(1<<7)				/* Source Address Write Back */
#define IPS_CKE			(1<<6)				/* Clock enable */
#define IPS_DMBS_MASK	(0x7<<3)			/* Mask for Destination DMA Maximum Burst Size */
#define IPS_DMBS_16		(0x0<<3)			/* 16 Bytes */
#define IPS_DMBS_32		(0x1<<3)			/* 32 Bytes */
#define IPS_DMBS_64		(0x2<<3)			/* 64 Bytes */
#define IPS_DMBS_128	(0x3<<3)			/* 128 Bytes */
#define IPS_SMBS_MASK	(0x7<<0)			/* Mask for SourceDMA Maximum Burst Size */
#define IPS_SMBS_16		(0x0<<0)			/* 16 Bytes */
#define IPS_SMBS_32		(0x1<<0)			/* 32 Bytes */
#define IPS_SMBS_64		(0x2<<0)			/* 64 Bytes */
#define IPS_SMBS_128	(0x3<<0)			/* 128 Bytes */

// MS(0:27-26) Mode Select
#define _MS_TYPE_CRYPT                       (0)
#define _MS_TYPE_AUTH                        (1)
#define _MS_TYPE_AUTH_THEN_CRYPT  (2)
#define _MS_TYPE_CRYPT_THEN_AUTH  (3)


// modeCrypto
#define _MD_NOCRYPTO 			((u32)-1)
#define _MD_CBC					(u32)(0)
#define _MD_ECB					(u32)(2)
#define _MD_CTR					(u32)(3)
#define _MASK_CRYPTOECRYPT		(u32)(1<<7)
#define _MASK_CRYPTOTHENAUTH	(u32)(1<<6)
#define _MASK_CRYPTOAES			(u32)(1<<5)
#define _MASK_CRYPTO3DESDES		(u32)(1<<4)
#define _MASK_CBCECBCTR			((u32)((1<<0)|(1<<1)))
//#define _MASK_ECBCBC			(1<<1)

// modeAuth
#define _MD_NOAUTH				((u32)-1)
#define _MASK_AUTHSHA2			(1<<0)
#define _MASK_AUTHSHA1MD5		(1<<1)
#define _MASK_AUTHHMAC			(1<<2)
#define _MASK_AUTH_IOPAD_READY	(1<<3)

#define MAX_PKTLEN (1<<14)

//Bit 0: 0:DES 1:3DES
//Bit 1: 0:CBC 1:ECB
//Bit 2: 0:Decrypt 1:Encrypt
#define DECRYPT_CBC_DES					0x00
#define DECRYPT_CBC_3DES				0x01
#define DECRYPT_ECB_DES					0x02
#define DECRYPT_ECB_3DES				0x03
#define ENCRYPT_CBC_DES					0x04
#define ENCRYPT_CBC_3DES				0x05
#define ENCRYPT_ECB_DES					0x06
#define ENCRYPT_ECB_3DES				0x07
#define RTL8651_CRYPTO_NON_BLOCKING		0x08
#define RTL8651_CRYPTO_GENERIC_DMA		0x10
#define DECRYPT_CBC_AES					0x20
#define DECRYPT_ECB_AES					0x22
#define DECRYPT_CTR_AES					0x23
#define ENCRYPT_CBC_AES					0x24
#define ENCRYPT_ECB_AES					0x26
#define ENCRYPT_CTR_AES					0x27

//Bit 0: 0:MD5 1:SHA1
//Bit 1: 0:Hash 1:HMAC
#define HASH_MD5		0x00
#define HASH_SHA1		0x01
#define HMAC_MD5		0x02
#define HMAC_SHA1		0x03

#define REG32(reg)				(*((volatile u32 *)(reg)))
#define READ_MEM32(reg)			REG32(reg)
#define WRITE_MEM32(reg,val)	REG32(reg)=(val)

// address macro
#define KUSEG           0x00000000
#define KSEG0           0x80000000
#define KSEG1           0xa0000000
#define KSEG2           0xc0000000
#define KSEG3           0xe0000000

#define CPHYSADDR(a)        ((int)(a) & 0x1fffffff)
#define CKSEG0ADDR(a)       (CPHYSADDR(a) | KSEG0)
#define CKSEG1ADDR(a)       (CPHYSADDR(a) | KSEG1)
#define CKSEG2ADDR(a)       (CPHYSADDR(a) | KSEG2)
#define CKSEG3ADDR(a)       (CPHYSADDR(a) | KSEG3)

#define PHYSICAL_ADDRESS(x) (x)//CPHYSADDR(x)
#define UNCACHED_ADDRESS(x) (x)//CKSEG1ADDR(x)
#define CACHED_ADDRESS(x)	(x)//CKSEG0ADDR(x)



enum _MODE_SELECT
{
	_MS_CRYPTO = 0,
	_MS_AUTH = 1,
	_MS_AUTH_THEN_DECRYPT = 2,
	_MS_ENCRYPT_THEN_AUTH = 3,
};


//
// AUTH Type
//

#define AUTH_TYPE_NO_AUTH 		((u32)-1)

#define AUTH_TYPE_MASK_FUNC		0x3	// bit 0, bit 1
#define AUTH_TYPE_MD5			0x2
#define AUTH_TYPE_SHA1			0x0
#define AUTH_TYPE_SHA2			0x1
	
#define AUTH_TYPE_MASK_HMAC 	0x4	// bit 2
#define AUTH_TYPE_HMAC_MD5 		(AUTH_TYPE_MD5 | AUTH_TYPE_MASK_HMAC)
#define AUTH_TYPE_HMAC_SHA1 	(AUTH_TYPE_SHA1 | AUTH_TYPE_MASK_HMAC)
#define AUTH_TYPE_HMAC_SHA2 	(AUTH_TYPE_SHA2 | AUTH_TYPE_MASK_HMAC)

#define AUTH_TYPE_MASK_SHA2 	0x30 // bit 3,4
#define AUTH_TYPE_SHA2_224		0x10
#define AUTH_TYPE_SHA2_256  	0x20 



// 
// Cipher Type
//

#define CIPHER_TYPE_NO_CIPHER 		((u32)-1)

#define CIPHER_TYPE_DES_CBC 		0x0
#define CIPHER_TYPE_DES_ECB			0x2
#define CIPHER_TYPE_3DES_CBC		0x10
#define CIPHER_TYPE_3DES_ECB		0x12
#define CIPHER_TYPE_AES_CBC			0x20
#define CIPHER_TYPE_AES_ECB			0x22
#define CIPHER_TYPE_AES_CTR			0x23

#define CIPHER_TYPE_MODE_ENCRYPT 	0x80

#define CIPHER_TYPE_MASK_FUNC 		0x30 	// 0x00 : DES, 0x10: 3DES, 0x20: AES
#define CIPHER_TYPE_FUNC_DES		0x00
#define CIPHER_TYPE_FUNC_3DES 		0x10
#define CIPHER_TYPE_FUNC_AES 		0x20
	
#define CIPHER_TYPE_MASK_BLOCK 		0x7 	// 0x0 : CBC, 0x2: ECB, 0x3: CTR 
#define CIPHER_TYPE_BLOCK_CBC 		0x0
#define CIPHER_TYPE_BLOCK_ECB 		0x2
#define CIPHER_TYPE_BLOCK_CTR 		0x3


//
// Data Structure
//
/**************************************************************************
 * Data Structure for IPSec Engine
 **************************************************************************/


/*
 *  ipsec engine supports scatter list: your data can be stored in several segments those are not continuous.
 *  Each scatter points to one segment of data.
 */
typedef struct rtl_ipsecScatter_s {
	u32 len;
	void* ptr;
} rtl_ipsecScatter_t;

// static inline function / macros

static inline
u32 __rtl_cpu_to_be32(u32 val)
{
	asm volatile(
		"rev %0, %0"
	  : "=r" (val) 
	  : "0" (val));
		
	return val;
}

//
// ROM global variables
extern const u32 __AES_Te4[];
extern const u32 __AES_rcon[];

//
// ROM Function mapping
//

#define __rtl_cryptoEngine_init				CRYPTO_Init
#define __rtl_cryptoEngine_exit				CRYPTO_DeInit
#define __rtl_cryptoEngine_reset				CRYPTO_Reset


#define __rtl_cryptoEngine_set_security_mode	CRYPTO_SetSecurityMode
#define __rtl_cryptoEngine					CRYPTO_Process

#define __rtl_crypto_cipher_init				CRYPTO_CipherInit
#define __rtl_crypto_cipher_encrypt			CRYPTO_CipherEncrypt
#define __rtl_crypto_cipher_decrypt			CRYPTO_CipherDecrypt


//
// ROM functions
//

extern _LONG_CALL_ void CRYPTO_AlignToBe32(u32 *pData, int bytes4_num );
extern _LONG_CALL_ void CRYPTO_MemDump(const u8 *start, u32 size, char * strHeader);
extern _LONG_CALL_ int CRYPTO_Init(HAL_CRYPTO_ADAPTER *pIE);
extern _LONG_CALL_ int CRYPTO_DeInit(HAL_CRYPTO_ADAPTER *pIE);
extern _LONG_CALL_ int CRYPTO_Reset(HAL_CRYPTO_ADAPTER *pIE);
extern _LONG_CALL_ int CRYPTO_SetSecurityMode(HAL_CRYPTO_ADAPTER *pIE, 
	IN const u32 mode_select, IN const u32 cipher_type, IN const u32 auth_type,
	IN const void* pCipherKey, IN const u32 lenCipherKey, 
	IN const void* pAuthKey, IN const u32 lenAuthKey);
extern _LONG_CALL_ int CRYPTO_Process(HAL_CRYPTO_ADAPTER *pIE, 
	IN const u8 *message, 	IN const u32 msglen, 
	IN const u8 *pIv, 		IN const u32 ivlen,
	IN const u32 a2eo, 		OUT void *pResult);
extern _LONG_CALL_ int CRYPTO_CipherInit(HAL_CRYPTO_ADAPTER *pIE, 
	IN const u32 cipher_type, 
	IN const u8* key, IN const u32 keylen);
extern _LONG_CALL_ int CRYPTO_CipherEncrypt(HAL_CRYPTO_ADAPTER *pIE, 
	IN const u32 cipher_type, 	
	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* piv, 		IN const u32 ivlen, 
	OUT u8* pResult);
extern _LONG_CALL_ int CRYPTO_CipherDecrypt(HAL_CRYPTO_ADAPTER *pIE, 
	IN const u32 cipher_type, 	IN const u8* message, 	IN const u32 msglen, 
	IN const u8* piv, 			IN const u32 ivlen, 	
	OUT u8* pResult );

extern _LONG_CALL_ void CRYPTO_SetCheckSumEn(u32 checksum_en);
extern _LONG_CALL_ void CRYPTO_GetCheckSumData(u32* pChecksum);
#endif  // __HAL_CRYPTO_RAM_H

