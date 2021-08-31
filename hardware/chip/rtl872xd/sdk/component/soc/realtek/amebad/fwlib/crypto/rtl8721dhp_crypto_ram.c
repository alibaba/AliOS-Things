/**
  ******************************************************************************
  * @file    rtl8721dhp_crypto_ram.c
  * @author
  * @version V1.0.0
  * @date    2017-10-10
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the HW crypto:
  *           - Crypto initialization
  *           - Auth and cipher initialization
  *           - Descriptor setting
  *           - key/IV/PAD setting
  *           - Auth process/update/final
  *           - Cipher encrypt/decrypt
  *
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#include "ameba_soc.h"
#include "osdep_service.h"

ALIGNMTO(32) static u8 ipsec_padding[64]  = { 0x0 };

extern HAL_CRYPTO_ADAPTER crypto_engine ALIGNMTO(32);

/**
  * @brief  Sequential hash process.
  * @param  pDigest: Sequential hash result. 
  * @note  There exist two buffers : last_message and seq_buf;
  *             last_message : store the previous message pointer.
  *             seq_buf : store the data less than 64.
  * @retval Process status.
  */
IMAGE2_RAM_TEXT_SECTION
int CRYPTO_SendSeqBuf(u8 *pDigest)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;
	int ret = SUCCESS;
	const u8* pIV 	= NULL;
	const u32 ivlen = 0;
	const u32 a2eo 	= 0;

	int total_len = pIE->hmac_seq_last_msglen;
	int buf_pos = pIE->hmac_seq_buf_is_used_bytes;
	int rest_bytes = 64 - buf_pos;
	int bodylen, restlen;

	if ( total_len < rest_bytes ) {	//store into seq buf
		_memcpy((void*)(&pIE->hmac_seq_buf[buf_pos]), (const void*)(pIE->hmac_seq_last_message), total_len);
		pIE->hmac_seq_buf_is_used_bytes += total_len;
	} else {
		// send out a seq buf if the seq buf is the last and length is 64-byte
		_memcpy((void*)(&pIE->hmac_seq_buf[buf_pos]), (const void*)(pIE->hmac_seq_last_message), rest_bytes);
		ret =  CRYPTO_ProcessAD(pIE, (u8*)(pIE->hmac_seq_buf), 64, pIV, ivlen, NULL, a2eo, (total_len == rest_bytes)?pDigest:NULL, NULL);
		if ( ret != SUCCESS ) return ret;

		pIE->hmac_seq_buf_is_used_bytes = 0;
		buf_pos = 0;

		total_len -= rest_bytes;
		pIE->hmac_seq_last_msglen = total_len;
		pIE->hmac_seq_last_message += rest_bytes;

		//send out all 64-byte align message
		restlen = total_len & 0x3F;
		bodylen = total_len - restlen;
		if ( bodylen > 0 ) { // there are 64x messages
			ret =  CRYPTO_ProcessAD(pIE, pIE->hmac_seq_last_message, bodylen, pIV, ivlen, NULL, a2eo, (restlen==0)?pDigest:NULL, NULL);
			if ( ret != SUCCESS ) return ret;
			pIE->hmac_seq_last_message += bodylen;
		}

		// backup the rest
		if ( restlen > 0 ) {
			_memcpy((void*)(&pIE->hmac_seq_buf[0]), (const void*)(pIE->hmac_seq_last_message), restlen);
		}
		pIE->hmac_seq_buf_is_used_bytes = restlen;
	}

 	if ( (pDigest != NULL) &&(pIE->hmac_seq_buf_is_used_bytes > 0) ) { // last one
		ret =  CRYPTO_ProcessAD(pIE, (u8*)(pIE->hmac_seq_buf), pIE->hmac_seq_buf_is_used_bytes, pIV, ivlen, NULL, a2eo, pDigest, NULL);
	}

	return ret;
}

/**
  * @brief  Crypto memory dump.
  * @param  start: Point to start address of memory to be dumped. 
  * @param  size: The memory length.
  * @param  strHeader: String at the begining of dump.
  * @retval None
  */
IMAGE2_RAM_TEXT_SECTION
void CRYPTO_MemDump(const u8 *start, u32 size, char * strHeader)
{
	int row, column, index, index2, max;
	u8 *buf, *line;

	if(!start ||(size==0))
		return;

	line = (u8*)start;

	/*
	16 bytes per line
	*/
	if (strHeader)
		DiagPrintf ("%s", strHeader);

	column = size % 16;
	row = (size / 16) + 1;
	for (index = 0; index < row; index++, line += 16) 
	{
		buf = (u8*)line;

		max = (index == row - 1) ? column : 16;
		if ( max==0 )
			break; /* If we need not dump this line, break it. */

		DiagPrintf ("\n[%08x] ", line);

		//Hex
		for (index2 = 0; index2 < max; index2++)
		{
			if (index2 == 8)
				DiagPrintf ("  ");
			DiagPrintf ("%02x ", (u8) buf[index2]);
		}

		if (max != 16)
		{
			if (max < 8)
				DiagPrintf ("  ");
			for (index2 = 16 - max; index2 > 0; index2--)
				DiagPrintf ("   ");
		}

	}

	DiagPrintf ("\n");
}

/**
  * @brief  dump crypto engine register for debug.
  * @param  None
  * @retval None
  */
IMAGE2_RAM_TEXT_SECTION
void CRYPTO_RegDump(void)
{
	IPSEC_TypeDef* IPSEC = (IPSEC_TypeDef*) CRYPTO_REG_BASE;

	if (TrustZone_IsSecure()) {
		IPSEC =  (IPSEC_TypeDef*) CRYPTOS_REG_BASE;
	}
	
	DiagPrintf("=========================================\n");
	DiagPrintf("Crypto Engine Registers\n");
	DiagPrintf("=========================================\n");

	DiagPrintf(" SDSR : %08x\n", IPSEC->SDSR);
	DiagPrintf(" SDFW : %08x\n", IPSEC->SDFW);
	DiagPrintf(" SDSW : %08x\n",  IPSEC->SDSW);
	DiagPrintf(" RST_ISR_CON : %08x\n",  IPSEC->RST_ISR_CON);
	DiagPrintf(" IMR : %08x\n", IPSEC->IMR);
	DiagPrintf(" DEBUG : %08x\n", IPSEC->DEBUG);
	DiagPrintf(" ERR_STAT : %08x\n",  IPSEC->ERR_STAT);
	DiagPrintf(" A2EO_SUM : %08x\n",  IPSEC->A2EO_SUM);
	DiagPrintf(" ENL_SUM : %08x\n", IPSEC->ENL_SUM);
	DiagPrintf(" APL_SUM : %08x\n", IPSEC->APL_SUM);
	DiagPrintf(" EPL_SUM : %08x\n",  IPSEC->EPL_SUM);
	DiagPrintf(" SWAP_BURST : %08x\n",  IPSEC->SWAP_BURST);
	DiagPrintf(" ENG_DBG : %08x\n", IPSEC->ENG_DBG);
	DiagPrintf(" DMA_LX_DBG : %08x\n", IPSEC->DMA_LX_DBG);
	DiagPrintf(" DMA_RX_DBG : %08x\n",  IPSEC->DMA_RX_DBG);
	DiagPrintf(" DMA_TX_DBG : %08x\n",  IPSEC->DMA_TX_DBG);
	DiagPrintf(" STA_SDES_CFG : %08x\n", IPSEC->STA_SDES_CFG);
	DiagPrintf(" STA_SDES_PTR : %08x\n", IPSEC->STA_SDES_PTR);
	DiagPrintf(" STA_SDES_CMD1 : %08x\n",  IPSEC->STA_SDES_CMD1);
	DiagPrintf(" STA_SDES_CMD2 : %08x\n",  IPSEC->STA_SDES_CMD2);
	DiagPrintf(" STA_SDES_CMD3 : %08x\n",  IPSEC->STA_SDES_CMD3);
	DiagPrintf(" STA_DDES_CFG : %08x\n",  IPSEC->STA_DDES_CFG);
	DiagPrintf(" STA_DDES_PTR : %08x\n", IPSEC->STA_DDES_PTR);
	DiagPrintf(" DBG_VERSION : %08x\n", IPSEC->DBG_VERSION);
	DiagPrintf(" DDSR : %08x\n",  IPSEC->DDSR);
	DiagPrintf(" DDFW : %08x\n",  IPSEC->DDFW);
	DiagPrintf(" DDSW : %08x\n", IPSEC->DDSW);
	DiagPrintf(" DESC_PKT_CONF : %08x\n", IPSEC->DESC_PKT_CONF);
	DiagPrintf(" DBG_SD : %08x\n",  IPSEC->DBG_SD);
	DiagPrintf(" DBG_DD : %08x\n",  IPSEC->DBG_DD);
}

/**
  * @brief  Set source descriptor.
  * @param  sd1: Source descriptor first word.
  * @param  sdpr: Source descriptor second word.
  * @retval None
  */
static inline  
void CRYPTO_SetSrcDesc(u32 sd1, u32 sdpr)
{
	u32 timeout;
	IPSEC_TypeDef* IPSEC = (IPSEC_TypeDef*) CRYPTO_REG_BASE;

	if (TrustZone_IsSecure()) {
		IPSEC =  (IPSEC_TypeDef*) CRYPTOS_REG_BASE;
	}

	timeout = FIFOCNT_TIMEOUT;
	while (1) {
		if ( (IPSEC->SDSR & SFIFO_EMPTY_CNT) > 0 ) {
			DBG_PRINTF(MODULE_IPSEC, LEVEL_INFO, "sd1=0x%x , sdpr=0x%x \r\n", sd1, sdpr);
			IPSEC->SDFW = sd1;
			IPSEC->SDSW = sdpr;
			break;
		}
		timeout--;
		if (timeout==0) {
			DBG_PRINTF(MODULE_IPSEC, LEVEL_ERROR, "Timeout, src fifo is FULL \r\n");
			break;
		}
	}
}

/**
  * @brief  Set destination descriptor.
  * @param  dd1: Destination descriptor first word.
  * @param  ddpr: Destination descriptor second word.
  * @retval None
  */
static inline  
void CRYPTO_SetDstDesc(u32 dd1, u32 ddpr)
{
	IPSEC_TypeDef* IPSEC = (IPSEC_TypeDef*) CRYPTO_REG_BASE;

	if (TrustZone_IsSecure()) {
		IPSEC =  (IPSEC_TypeDef*) CRYPTOS_REG_BASE;
	}

	if ( (IPSEC->DDSR & DFIFO_EMPTY_CNT) > 0  ) {
		DBG_PRINTF(MODULE_IPSEC, LEVEL_INFO, "dd1=0x%x , ddpr=0x%x \r\n", dd1, ddpr);
		IPSEC->DDFW = dd1;
		IPSEC->DDSW = ddpr;
	} else {
		DBG_PRINTF(MODULE_IPSEC, LEVEL_ERROR, "dst fifo_cnt is not correct: %d \r\n", (IPSEC->DDSR & DFIFO_EMPTY_CNT));
	}
}

/**
  * @brief  Clear crypto engine command OK interrupt.
  * @retval None
  */
IMAGE2_RAM_TEXT_SECTION
void CRYPTO_CleanCmdOk(void)
{
	IPSEC_TypeDef* IPSEC = (IPSEC_TypeDef*) CRYPTO_REG_BASE;

	if (TrustZone_IsSecure()) {
		IPSEC =  (IPSEC_TypeDef*) CRYPTOS_REG_BASE;
	}

	u32 ok_intr_cnt = 0;
	ok_intr_cnt = (IPSEC->RST_ISR_CON & OK_INTR_CNT) >> 8;
	IPSEC->RST_ISR_CON |= ok_intr_cnt << 16;
	IPSEC->RST_ISR_CON |= CMD_OK;
}

/**
  * @brief  Clear command ok and error interrupts.
  * @retval None
  */
IMAGE2_RAM_TEXT_SECTION
void CRYPTO_ClearAllINT(void)
{
	IPSEC_TypeDef* IPSEC = (IPSEC_TypeDef*) CRYPTO_REG_BASE;

	if (TrustZone_IsSecure()) {
		IPSEC =  (IPSEC_TypeDef*) CRYPTOS_REG_BASE;
	}

	IPSEC->ERR_STAT = 0xFFFF;
	//IPSEC->RST_ISR_CON |= CMD_OK;
	CRYPTO_CleanCmdOk();
}

/**
  * @brief  Set source descriptor command buffer.
  * @param  pIE: Point to crypto adapter. 
  * @retval None
  */
static inline
void CRYPTO_SetCmdBuf(HAL_CRYPTO_ADAPTER *pIE)
{
	rtl_crypto_cl_t *pCL;
	u32 a2eo;
	u32 enl;

	a2eo = pIE->a2eo;
	enl = pIE->enl;

	pCL = (rtl_crypto_cl_t *)pIE->cl_buffer;

	_memset((u8*)pCL, 0, sizeof(pIE->cl_buffer));

	if ( pIE->auth_type != AUTH_TYPE_NO_AUTH ) {	//authentication
		pCL->engine_mode = 1; //hash only
		// Sequential hash setting
		pCL->hmac_seq_hash=1;
		pCL->hmac_seq_hash_first=0;
		if ( pIE->hmac_seq_hash_first == 1 ) {
			pCL->hmac_seq_hash_first=1;
			pIE->hmac_seq_hash_first=0;
		}
		//Swap settting
		if ( pIE->isMD5 ) {
			pCL->habs=1;
			pCL->hibs=1;
			pCL->hobs=1;
			pCL->hkbs=1;
		} else if ( pIE->isSHA1 ) {
			pCL->hmac_mode = 1;
			pCL->habs=1;
		} else if ( pIE->sha2type != SHA2_NONE ) {
			pCL->hmac_mode = (pIE->sha2type == SHA2_224 )? 2 : 3 ; // currently only support SHA2_224 / SHA2_256
			pCL->habs=1;
		}

		if ( pIE->isHMAC ) {
			pCL->hmac_en = 1;
		}
		// Use auto padding and sequential hash III
		if ( pIE->hmac_seq_hash_last == 1 ) {	//The last block autopadding: 16-byte
			// always using auto padding
			pCL->enl = (enl + 15)/16 ;
			pCL->enc_last_data_size = pIE->enc_last_data_size;
			pCL->ap0 = pIE->hmac_seq_hash_total_len*8;
			if ( pIE->isHMAC ) pCL->ap0 += 64*8;
		} else {	//MD5/SHA1/SHA224/SHA256: 64-byte
			pCL->enl = enl/64;
		}

		pCL->hmac_seq_hash_last=0;
		pCL->hmac_seq_hash_no_wb=1;
		if ( pIE->hmac_seq_hash_last == 1 ) {
			pCL->hmac_seq_hash_last=1;
			pCL->hmac_seq_hash_no_wb=0;
		}
	} else { // cipher - encryption / decryption
		u32 cipher_type;
		u32 block_mode;
		//cipher mode: 0x0 : ECB, 0x1: CBC, 0x2: CFB , 0x3 : OFB , 0x4 : CTR, 0x5 : GCTR, 0x6: GMAC, 0x7: GHASH, 0x8: GCM
		//0 - chacha20+poly1305, 1 - chacha20, 2 - poly1305 mode 1, 3 - poly1305 mode 2
		cipher_type = pIE->cipher_type;
		block_mode = cipher_type & CIPHER_TYPE_MASK_BLOCK;
		pCL->cipher_mode = block_mode;
		//AES key length
		if ( pIE->aes ) {
			pCL->cipher_eng_sel = 0;
			switch ( pIE->lenCipherKey ) {
				case 128/8 :
					pCL->aes_key_sel = 0;
					break;
				case 192/8 :
					pCL->aes_key_sel = 1;
					break;
				case 256/8 :
					pCL->aes_key_sel = 2;
					break;
				default:
					break;
			}

			pCL->enl = (enl+15)/16;
			pCL->enc_last_data_size = pIE->enc_last_data_size;

			if ( block_mode ==  CIPHER_TYPE_BLOCK_GCM  ) {
				pCL->header_total_len = (a2eo+15)/16;
				pCL->aad_last_data_size = pIE->aad_last_data_size;
			}
		} else if ( pIE->des ) {//DES setting
			pCL->cipher_eng_sel = 1;
			pCL->des3_en = 0;
			pCL->enl = (enl+7)/8;
		} else if ( pIE->trides ) {
			pCL->cipher_eng_sel = 1;
			pCL->des3_en = 1;
			pCL->enl = (enl+7)/8;
		} else if ( pIE->chacha ) {
			pCL->cipher_eng_sel = 2;
			pCL->enl = (enl+15)/16;
			pCL->enc_last_data_size = pIE->enc_last_data_size;

			pCL->header_total_len = (a2eo+15)/16;
			pCL->aad_last_data_size = pIE->aad_last_data_size;
		}

		if ( pIE->isDecrypt == 0 )
			pCL->cipher_encrypt = 1;
		//Swap settting
		if ( pIE->chacha ) {
			pCL->ckws = 1;
			pCL->ckbs = 1;
			pCL->cabs = 1;
			pCL->ciws = 1;
			pCL->cibs = 1;
			pCL->cows = 1;
			pCL->cobs = 1;
			pCL->codws = 1;
			pCL->cidws = 1;
		} else {
			pCL->cabs = 1;
		}
	}

	pCL->icv_total_length = 0x40; // for mix mode, but need to set a value 0x40
}

/**
  * @brief  Set source descriptor command, key, IV, pad array.
  * @param  pIE: Point to crypto adapter.
  * @param  p_iv: IV(initialization vector).
  * @param  ivLen: IV length.
  * @retval None
  */
static inline
void CRYPTO_SrcDescKeyIv(HAL_CRYPTO_ADAPTER *pIE, const u8* p_iv, u32 ivLen)
{
	rtl_crypto_srcdesc_t src_desc;

	src_desc.w = 0;

	// FS=1, CL=3
	src_desc.b.rs = 1;
	src_desc.b.fs = 1;
	src_desc.b.cl= 3;

	// auto padding
	if ( pIE->hmac_seq_hash_last )
		src_desc.b.ap = 0x01;

	//Set command buffer array
	CRYPTO_SetCmdBuf(pIE);
	DCache_CleanInvalidate((u32)(&(pIE->cl_buffer)[0]), sizeof(pIE->cl_buffer));
	crypto_dbg_mem_dump((u32)(&(pIE->cl_buffer)[0]), sizeof(pIE->cl_buffer), "Command Setting: ");
	CRYPTO_SetSrcDesc(src_desc.w, (u32)(&(pIE->cl_buffer)[0]));

	// Set key array
	if ( pIE->cipher_type != CIPHER_TYPE_NO_CIPHER) {	//cipher
		u32 lenCipherKey;
		u8  *pCipherKey;

		lenCipherKey = (u32) pIE->lenCipherKey;
		pCipherKey = (u8*) pIE->pCipherKey;

		src_desc.w = 0;
		src_desc.b.rs = 1;
		src_desc.b.fs = 1;
		src_desc.b.key_len = lenCipherKey/4;
		DCache_CleanInvalidate((u32)(pCipherKey), lenCipherKey);
		crypto_dbg_mem_dump((u32)(pCipherKey), lenCipherKey, "KEY: ");
		CRYPTO_SetSrcDesc(src_desc.w, (u32)(pCipherKey));
	}

	// Set IV array
	if ( p_iv != NULL && ivLen > 0 ) {
		src_desc.w = 0;
		src_desc.b.rs = 1;
		src_desc.b.fs = 1;
		src_desc.b.iv_len = ivLen/4;
		DCache_CleanInvalidate((u32)(p_iv), ivLen);
		crypto_dbg_mem_dump((u32)(p_iv), ivLen, "IV: ");
		CRYPTO_SetSrcDesc(src_desc.w, (u32)(p_iv));
	}

	// Set Pad array
	if ( pIE->isHMAC ) {
		src_desc.w = 0;
		src_desc.b.rs = 1;
		src_desc.b.fs = 1;
		src_desc.b.keypad_len = 128;
		DCache_CleanInvalidate((u32)(&(pIE->g_IOPAD[0])), sizeof(pIE->g_IOPAD));
		crypto_dbg_mem_dump((u32)(&(pIE->g_IOPAD[0])), 128, "PAD: ");
		CRYPTO_SetSrcDesc(src_desc.w, (u32)(&(pIE->g_IOPAD[0])));
	}
}

/**
  * @brief  Crypto engine reset.
  * @param  pIE: Point to crypto adapter. 
  * @note pIE is no need in Ameba-D reset api, but in order to compatible with AmebaZ reset api, pIE is added. 
  * @retval None
  */
IMAGE2_RAM_TEXT_SECTION
void CRYPTO_Reset(UNUSED_WARN_DIS HAL_CRYPTO_ADAPTER *pIE)
{
	IPSEC_TypeDef* IPSEC = (IPSEC_TypeDef*) CRYPTO_REG_BASE;

	if (TrustZone_IsSecure()) {
		IPSEC =  (IPSEC_TypeDef*) CRYPTOS_REG_BASE;
	}

	// Crypto engine : Software Reset
	IPSEC->RST_ISR_CON = SOFT_RST;
	// register offset 0x030 
	IPSEC->SWAP_BURST = 16<<16;
	IPSEC->SWAP_BURST |= (KEY_IV_SWAP | KEY_PAD_SWAP | HASH_INITIAL_VALUE_SWAP);
	IPSEC->SWAP_BURST |= (DMA_IN_LITTLE_ENDIAN | DATA_OUT_LITTLE_ENDIAN | MAC_OUT_LITTLE_ENDIAN);

	// ++++ register offset 0x018 +++++
	// Crypto Engine : DMA arbiter(detect fifo wasted level) , clock enable
	IPSEC->DEBUG = (ARBITER_MODE | ENGINE_CLK_EN);
}

/**
  * @brief  Set crypto adapter parameter.
  * @param  pIE: Point to crypto adapter. 
  * @param  cipher_type: Cipher type.
  *		 @arg CIPHER_TYPE_DES_ECB: DES_ECB
  *		 @arg CIPHER_TYPE_DES_CBC: DES_CBC
  *		 @arg CIPHER_TYPE_3DES_ECB: 3DES_ECB
  *		 @arg CIPHER_TYPE_3DES_CBC: 3DES_CBC
  *		 @arg CIPHER_TYPE_AES_ECB: AES_ECB
  *		 @arg CIPHER_TYPE_AES_CBC: AES_CBC
  *		 @arg CIPHER_TYPE_AES_CFB: AES_CFB
  *		 @arg CIPHER_TYPE_AES_OFB: AES_OFB
  *		 @arg CIPHER_TYPE_AES_CTR: AES_CTR
  *		 @arg CIPHER_TYPE_AES_GCTR: AES_GCTR
  *		 @arg CIPHER_TYPE_AES_GMAC: AES_GMAC
  *		 @arg CIPHER_TYPE_AES_GHASH: AES_GHASH
  *		 @arg CIPHER_TYPE_AES_GCM: AES_GCM
  *		 @arg CIPHER_TYPE_CHACHA_POLY1305: ChaCha+poly1305
  *		 @arg CIPHER_TYPE_CHACHA: ChaCha
  *		 @arg CIPHER_TYPE_POLY1305: Poly 1305
  * @param  auth_type: Authentication algorithm type.
  *		 @arg AUTH_TYPE_MD5: MD5
  *		 @arg AUTH_TYPE_SHA1: SHA1
  *		 @arg AUTH_TYPE_SHA2_224_ALL: SHA2_224
  *		 @arg AUTH_TYPE_SHA2_256_ALL: SHA2_256
  *		 @arg AUTH_TYPE_HMAC_MD5: HMAC MD5
  *		 @arg AUTH_TYPE_HMAC_SHA1: HMAC SHA1
  *		 @arg AUTH_TYPE_HMAC_SHA2_224_ALL: HMAC SHA2_224
  *		 @arg AUTH_TYPE_HMAC_SHA2_256_ALL: HMAC SHA2_256
  * @param  pCipherKey: Point to cipher key.
  * @param  lenCipherKey: Cipher key length.
  * @param  pAuthKey: Point to authentication key.
  * @param  lenAuthKey: Authentication key length.
  * @retval SUCCESS or error status
  */
IMAGE2_RAM_TEXT_SECTION
int CRYPTO_SetSecurityModeAD(HAL_CRYPTO_ADAPTER *pIE,
        IN const u32 cipher_type, IN const u32 auth_type,
        IN const void* pCipherKey, IN const u32 lenCipherKey,
        IN const void* pAuthKey, IN const u32 lenAuthKey)
{
	assert_param(pIE != NULL);
	assert_param(pIE->isInit == _TRUE);

	if ( lenAuthKey > 0 ) { // Authentication
		assert_param(pAuthKey != NULL);
		assert_param(lenAuthKey <= CRYPTO_AUTH_PADDING);
	}

	// Cipher
	pIE->trides  = 0;
	pIE->aes	 = 0;
	pIE->des	 = 0;
	pIE->chacha = 0;
	pIE->isDecrypt	 = 0;
	pIE->cipher_type = cipher_type;

	if ( cipher_type != CIPHER_TYPE_NO_CIPHER) {
		switch (cipher_type & CIPHER_TYPE_MASK_FUNC ) {
			case CIPHER_TYPE_FUNC_DES :
				pIE->des = 1;
				break;
			case CIPHER_TYPE_FUNC_3DES :
				pIE->trides = 1;
				break;
			case CIPHER_TYPE_FUNC_AES :
				pIE->aes = 1;
				break;
			case CIPHER_TYPE_FUNC_CHACHA :
				pIE->chacha = 1;
				break;
		}
		pIE->isDecrypt  = ( cipher_type & CIPHER_TYPE_MODE_ENCRYPT )? 0: 1;
	}

	// Auth
	pIE->auth_type = auth_type;
	if ( auth_type != AUTH_TYPE_NO_AUTH) {
		pIE->isHMAC = (auth_type & AUTH_TYPE_MASK_HMAC) ? 1 : 0;
		pIE->isMD5 = ( (auth_type & AUTH_TYPE_MASK_FUNC) == AUTH_TYPE_MD5 )? 1 : 0;
		pIE->isSHA1 = ( (auth_type & AUTH_TYPE_MASK_FUNC) == AUTH_TYPE_SHA1 )? 1 : 0;
		pIE->isSHA2 = ( (auth_type & AUTH_TYPE_MASK_FUNC) == AUTH_TYPE_SHA2 )? 1 : 0;
		if ( pIE->isSHA2 ) {
			switch ( auth_type & AUTH_TYPE_MASK_SHA2 ) {
				case AUTH_TYPE_SHA2_224 :
					pIE->sha2type = SHA2_224;
					break;
				case AUTH_TYPE_SHA2_256 :
					pIE->sha2type = SHA2_256;
					break;
				default :
					DBG_PRINTF(MODULE_IPSEC, LEVEL_ERROR, "No this auth_type(%d) for SHA2\n", auth_type);
					pIE->sha2type = SHA2_NONE;
					break;
			}
		} else {
			pIE->sha2type = SHA2_NONE;
		}
	} else {
		pIE->isMD5 = 0;
		pIE->isHMAC = 0;
		pIE->isSHA1=0;
		pIE->isSHA2=0;
		pIE->sha2type = SHA2_NONE;
	}

	//Set hash algorithm digest length.
	switch (auth_type &  AUTH_TYPE_MASK_FUNC) {
		case AUTH_TYPE_MD5:
			pIE->digestlen = CRYPTO_MD5_DIGEST_LENGTH;
			break;
		case AUTH_TYPE_SHA1:
			pIE->digestlen = CRYPTO_SHA1_DIGEST_LENGTH;
			break;
		case AUTH_TYPE_SHA2:
			pIE->digestlen = pIE->sha2type;
			break;
		default:
			pIE->digestlen = 0;
			break;
	}

	// Set auth key and cipher key.
	pIE->lenCipherKey = lenCipherKey;
	pIE->pCipherKey = pCipherKey;

	pIE->pAuthKey = pAuthKey;
	pIE->lenAuthKey = lenAuthKey;
	
	//Authkey is always used in HMAC algorithm to generate auth ipad and opad.
	if ( lenAuthKey > 0 ) {
		u32 i;
		
		pIE->ipad = (u8 *)(&(pIE->g_IOPAD[0]));
		pIE->opad = (u8 *)(&(pIE->g_IOPAD[CRYPTO_PADSIZE]));

		_memset(pIE->ipad, 0x36, CRYPTO_PADSIZE);
		_memset(pIE->opad, 0x5c, CRYPTO_PADSIZE);

		for (i=0; i< lenAuthKey; i++) {
			pIE->ipad[i] ^= ((u8*) pIE->pAuthKey)[i];
			pIE->opad[i] ^= ((u8*) pIE->pAuthKey)[i];
	    }
	} else {
		pIE->ipad = 0;
		pIE->opad = 0;
	}
	// Sequential hash parameter init
	if ( auth_type != AUTH_TYPE_NO_AUTH) {
		pIE->hmac_seq_hash_first = 1;
		pIE->hmac_seq_hash_last = 0;
		pIE->hmac_seq_hash_total_len = 0;
	}
	return SUCCESS;
}

/**
  * @brief  Crypto engine initialize.
  * @param  pIE: Point to crypto adapter, Ameba-D should be NULL. 
  * @retval SUCCESS
  */
IMAGE2_RAM_TEXT_SECTION
int CRYPTO_Init(HAL_CRYPTO_ADAPTER *pIE_I)
{
	HAL_CRYPTO_ADAPTER *pIE = &crypto_engine;

	if (pIE_I != NULL) {
		pIE = pIE_I;
	}

	CRYPTO_Reset(pIE);

	pIE->isInit = 1;
	pIE->isIntMode = 0;
	return SUCCESS;
}

IMAGE2_RAM_TEXT_SECTION
void CRYPTO_Cache_Sync(u8 *p_buf, u32 len)
{
	u32 start_addr = (u32)(p_buf);
	u32 end_addr = start_addr + len;
	u8 cache_buf[CACHE_LINE_SIZE] = {0};
	u8 header_offset = 0, tail_offset = 0;
	//S1: enter critical, should be initialized before use
	//if(driver_call_os_func_map.driver_enter_critical) {
	//	driver_call_os_func_map.driver_enter_critical();
	//}
	rtw_enter_critical(NULL,NULL);

	header_offset = (start_addr & (~CACHE_LINE_ADDR_MSK));
	tail_offset = (end_addr & (~CACHE_LINE_ADDR_MSK));
	
	if(!IS_CACHE_LINE_ALIGNED_ADDR(start_addr))
	{
		//S2: cache store
		memcpy(cache_buf, (u8 *)(start_addr & CACHE_LINE_ADDR_MSK), CACHE_LINE_SIZE);
		//S3: cache flush
		DCache_Invalidate((start_addr & CACHE_LINE_ADDR_MSK), CACHE_LINE_SIZE);
		//S4: memory store
		if((end_addr & CACHE_LINE_ADDR_MSK) == (start_addr & CACHE_LINE_ADDR_MSK))
		{
			memcpy((cache_buf + header_offset), (u8 *)(start_addr), (tail_offset - header_offset));
		}
		else
		{
			memcpy((cache_buf + header_offset), (u8 *)(start_addr), (CACHE_LINE_SIZE - header_offset));
		}
		//S5: write back
		memcpy((u8 *)(start_addr & CACHE_LINE_ADDR_MSK), cache_buf, CACHE_LINE_SIZE);
		//S6: cache clean
		DCache_Clean((start_addr & CACHE_LINE_ADDR_MSK), CACHE_LINE_SIZE);
	}

	if((!IS_CACHE_LINE_ALIGNED_ADDR(end_addr)) && ((end_addr & CACHE_LINE_ADDR_MSK) != (start_addr & CACHE_LINE_ADDR_MSK)))
	{
		//S2: cache store
		memcpy(cache_buf, (u8 *)(end_addr & CACHE_LINE_ADDR_MSK), CACHE_LINE_SIZE);
		//S3: cache flush
		DCache_Invalidate((end_addr & CACHE_LINE_ADDR_MSK), CACHE_LINE_SIZE);
		//S4: memory store
		memcpy(cache_buf, (u8 *)(end_addr & CACHE_LINE_ADDR_MSK), tail_offset);
		//S5: write back
		memcpy((u8 *)(end_addr & CACHE_LINE_ADDR_MSK), cache_buf, CACHE_LINE_SIZE);
		//S6: cache clean
		DCache_Clean((end_addr & CACHE_LINE_ADDR_MSK), CACHE_LINE_SIZE);
	}

	//S7: cache flush
	DCache_Invalidate(start_addr, len);
	
	//S8: exit critical
	//if(driver_call_os_func_map.driver_exit_critical) {
	//	driver_call_os_func_map.driver_exit_critical();
	//}
	rtw_exit_critical(NULL,NULL);
}

/**
  * @brief  Crypto engine process.
  * @param  pIE: Point to crypto adapter. 
  * @param  message: Point to message. 
  * @param  msglen: Message length. 
  * @param  pIv: Point to IV(initialize vector). 
  * @param  ivlen: IV length. 
  * @param  paad: Point to AAD(additional authentication data). 
  * @param  aadlen: AAD length. 
  * @param  pResult: Point to auth or cipher result. 
  * @param  pTag: Point to MAC(Message Authentication Code) . 
  * @retval Result of process.
  */
IMAGE2_RAM_TEXT_SECTION
int CRYPTO_ProcessAD(HAL_CRYPTO_ADAPTER *pIE,
                       IN const u8 *message, 	IN const u32 msglen,
                       IN const u8 *pIv, 		IN const u32 ivlen,
                       IN const u8 *paad, 		IN const u32 aadlen,
                       OUT u8 *pResult, OUT u8 *pTag)
{
	rtl_crypto_srcdesc_t srcdesc;
	u32 a2eo;
	u32 enl;
	u32 digestlen;
	volatile u32 ips_err;
	u32 loopWait;
	IPSEC_TypeDef* IPSEC = (IPSEC_TypeDef*) CRYPTO_REG_BASE;

	if (TrustZone_IsSecure()) {
		IPSEC =  (IPSEC_TypeDef*) CRYPTOS_REG_BASE;
	}

	assert_param(message != NULL);
	//assert_param(msglen >= 0 && msglen <= CRYPTO_MAX_MSG_LENGTH);
	assert_param(pIE != NULL);
	assert_param(pIE->isInit == _TRUE);

	// Use only one scatter
	a2eo = aadlen;
	enl = msglen;

	CRYPTO_ClearAllINT();

	//   Set relative length data
	/* ** Calculate message and a2eo auto padding length ** */
	pIE->a2eo = a2eo;
	pIE->enl = enl;

	// DES / 3DES : 8 byte alignment
	if ( (pIE->des == 1) || (pIE->trides == 1) ) {
		pIE->enc_last_data_size = enl % 8;
		pIE->apl = (8-pIE->enc_last_data_size) % 8;

		pIE->aad_last_data_size = a2eo % 8;
		pIE->apl_aad = (8-pIE->aad_last_data_size) % 8;
	} else {
		pIE->enc_last_data_size = enl % 16;
		pIE->apl = (16-pIE->enc_last_data_size) % 16;

		pIE->aad_last_data_size = a2eo % 16;
		pIE->apl_aad = (16-pIE->aad_last_data_size) % 16;
	}

	digestlen = pIE->digestlen;

	if ( pIE->auth_type != AUTH_TYPE_NO_AUTH) {
		pIE->hmac_seq_hash_total_len += msglen;

		if ( pResult != NULL ) {
			pIE->hmac_seq_hash_last = 1;
		}
	}

	/********************************************
	 * step 1: Setup desitination descriptor
	 * Auth descriptor (1):
	 	-----------------
		|digest			|
		-----------------
	 * Cipher descriptor:
		-----------------
		|Data			|
		-----------------
		|Hash Data(Tag)	|
		-----------------
	 ********************************************/
	if ( pIE->auth_type != AUTH_TYPE_NO_AUTH ) {	//auth
		rtl_crypto_dstdesc_t dst_desc;

		if ( pResult != NULL ) {
			dst_desc.w = 0;
			dst_desc.auth.ws = 1;
			dst_desc.auth.fs = 1;
			dst_desc.auth.ls = 1;
			dst_desc.auth.adl = digestlen;
			DCache_CleanInvalidate((u32)pResult, digestlen);
			CRYPTO_SetDstDesc(dst_desc.w, (u32)pResult);
		}
	} else { // cipher
		rtl_crypto_dstdesc_t  dst_auth_desc;
		rtl_crypto_dstdesc_t  dst_cipher_desc;

		if ( pResult != NULL ) {
			dst_cipher_desc.w = 0;
			dst_cipher_desc.cipher.ws = 1;
			dst_cipher_desc.cipher.fs = 1;
			dst_cipher_desc.cipher.enc = 1;
			dst_cipher_desc.cipher.enl = pIE->enl;
			if ( pTag == NULL ) {
				dst_cipher_desc.cipher.ls = 1;
			}
			DCache_CleanInvalidate(((u32)pResult), pIE->enl);
			CRYPTO_SetDstDesc(dst_cipher_desc.w, (u32)pResult);// Data
		}

		if ( pTag != NULL ) {	// Tag
			dst_auth_desc.w = 0;
			dst_auth_desc.auth.ws = 1;
			dst_auth_desc.auth.enc = 0;
			dst_auth_desc.auth.adl = 16;
			dst_auth_desc.auth.ls = 1;
			if ( pResult == NULL ) {
				dst_auth_desc.auth.fs = 1;
			}
			DCache_CleanInvalidate((u32)pTag, 16);
			CRYPTO_SetDstDesc(dst_auth_desc.w, (u32)pTag);
		}
	}

	/********************************************
	 * step 2: Setup source descriptor
			 ----------
			|Cmd buffer |
			----------
			|Key array   |
			----------
			|IV array     |
			----------
			|PAD array  |
			----------
			|Data 1       |
			|   .             |
			|   .             |
			|   .             |
			|Data N       |
			----------
			|HMAC APL  |
			----------
	 ********************************************/
	/********************************************
	  * step 2-1: prepare Cmd & Key & IV array & Pad array:
	  ********************************************/

	CRYPTO_SrcDescKeyIv(pIE, pIv, ivlen);

	/********************************************
	 * step 2-2: prepare Data1 ~ DataN
	 ********************************************/
	srcdesc.w = 0;
	srcdesc.d.rs = 1;

	if ( paad != NULL ) {
		DCache_CleanInvalidate((u32)paad, aadlen);
		while ( a2eo > 16 ) {	//AAD, 16-byte per src desc
			srcdesc.d.a2eo = 16;

			crypto_dbg_mem_dump(paad, 16, "AAD: ");
			CRYPTO_SetSrcDesc(srcdesc.w, (u32)paad);

			paad += 16;
			a2eo -= 16;
			srcdesc.w = 0;
			srcdesc.d.rs = 1;
		}

		if ( a2eo > 0 ) {	//AAD, last block
			srcdesc.d.a2eo = a2eo;
			crypto_dbg_mem_dump(paad, a2eo, "AAD: ");
			CRYPTO_SetSrcDesc(srcdesc.w, (u32)paad);
			srcdesc.w = 0;
			srcdesc.d.rs = 1;
		}

		if ( pIE->apl_aad > 0 ) {	//AAD padding, last block
			srcdesc.d.a2eo = pIE->apl_aad;
			DCache_CleanInvalidate((u32)ipsec_padding, sizeof(ipsec_padding));
			crypto_dbg_mem_dump(ipsec_padding, pIE->apl_aad, "AAD padding ");
			CRYPTO_SetSrcDesc(srcdesc.w, (u32)ipsec_padding);
			srcdesc.w = 0;
			srcdesc.d.rs = 1;
		}
	}

	if(message != NULL)
		DCache_CleanInvalidate(((u32)message), enl);

	while ( enl > 16368 ) {	// Data body 16352-byte per src desc
		srcdesc.d.enl = 16368;
		CRYPTO_SetSrcDesc(srcdesc.w, (u32)message);

		message += 16368;
		enl -= 16368;
		srcdesc.w = 0;
		srcdesc.d.rs = 1;
	}

	// Data body msglen < 16352
	if ( pIE->apl == 0 )
		srcdesc.d.ls = 1;
	srcdesc.d.enl = enl;
	CRYPTO_SetSrcDesc(srcdesc.w, (u32)message);
	crypto_dbg_mem_dump(message, enl, "message: ");
	
	// data padding, regard as enl
	if ( pIE->apl != 0 ) {
		srcdesc.w = 0;
		srcdesc.d.rs = 1;
		srcdesc.d.enl = pIE->apl;
		srcdesc.d.ls = 1;
		DCache_CleanInvalidate((u32)ipsec_padding, sizeof(ipsec_padding));
		crypto_dbg_mem_dump(ipsec_padding, srcdesc.d.enl, "data padding ");
		CRYPTO_SetSrcDesc(srcdesc.w, (u32)ipsec_padding);
	}	

	/********************************************
	 * step 3: Wait until ipsec engine stop
	 *Polling mode, intr_mode = 0
	 ********************************************/
	loopWait = 1000000; /* hope long enough */
	while (1) {
		if ( IPSEC->RST_ISR_CON & CMD_OK ) break;

		ips_err = IPSEC->ERR_STAT;
		if ( ips_err ) {
			DBG_PRINTF(MODULE_IPSEC, LEVEL_ERROR, "ips 0x1C err = 0x%08x\r\n", ips_err);
			return FAIL;
		}

		loopWait--;
		if (loopWait == 0) {
			DBG_PRINTF(MODULE_IPSEC, LEVEL_ERROR, "Wait Timeout\r\n");
			return FAIL; /* error occurs */
		}
	}

	if(pResult != NULL) {
		if ( pIE->auth_type != AUTH_TYPE_NO_AUTH ) {	//auth
			CRYPTO_Cache_Sync(pResult, digestlen);
		} else {
			CRYPTO_Cache_Sync(pResult, pIE->enl);
		}
	}

	if ( pTag != NULL ) {
		CRYPTO_Cache_Sync(pTag, 16);
	}

	//CRYPTO_CleanCmdOk();
	return SUCCESS;
}

/**
  * @brief  Cipher initialize.
  * @param  pIE: Point to crypto adapter. 
  * @param  cipher_type: Cipher type.
  *		 @arg CIPHER_TYPE_DES_ECB: DES_ECB
  *		 @arg CIPHER_TYPE_DES_CBC: DES_CBC
  *		 @arg CIPHER_TYPE_3DES_ECB: 3DES_ECB
  *		 @arg CIPHER_TYPE_3DES_CBC: 3DES_CBC
  *		 @arg CIPHER_TYPE_AES_ECB: AES_ECB
  *		 @arg CIPHER_TYPE_AES_CBC: AES_CBC
  *		 @arg CIPHER_TYPE_AES_CFB: AES_CFB
  *		 @arg CIPHER_TYPE_AES_OFB: AES_OFB
  *		 @arg CIPHER_TYPE_AES_CTR: AES_CTR
  *		 @arg CIPHER_TYPE_AES_GCTR: AES_GCTR
  *		 @arg CIPHER_TYPE_AES_GMAC: AES_GMAC
  *		 @arg CIPHER_TYPE_AES_GHASH: AES_GHASH
  *		 @arg CIPHER_TYPE_AES_GCM: AES_GCM
  *		 @arg CIPHER_TYPE_CHACHA_POLY1305: ChaCha+poly1305
  *		 @arg CIPHER_TYPE_CHACHA: ChaCha
  *		 @arg CIPHER_TYPE_POLY1305: Poly 1305
  * @param  key: Cipher key. 
  * @param  keylen: Key length. 
  * @retval Function execute status.
  */
IMAGE2_RAM_TEXT_SECTION
int CRYPTO_CipherInit(HAL_CRYPTO_ADAPTER *pIE,
                             IN const u32 cipher_type,
                             IN const u8 *key, IN const u32 keylen)
{
	u8* pAuthKey = NULL;
	u32 lenAuthKey = 0;

	return CRYPTO_SetSecurityModeAD(pIE,
		cipher_type, AUTH_TYPE_NO_AUTH,
		key, keylen, pAuthKey, lenAuthKey);

}

/**
  * @brief  Cipher encryption.
  * @param  pIE: Point to crypto adapter. 
  * @param  message: Point to message. 
  * @param  msglen: Message length. 
  * @param  pIv: Point to IV(initialize vector). 
  * @param  ivlen: IV length. 
  * @param  paad: Point to AAD(additional authentication data). 
  * @param  aadlen: AAD length. 
  * @param  pResult: Point to auth or cipher result. 
  * @param  pTag: Point to MAC(Message Authentication Code) . 
  * @retval Result of process.
  */
IMAGE2_RAM_TEXT_SECTION
int CRYPTO_CipherEncryptAD(HAL_CRYPTO_ADAPTER *pIE,
                                IN const u8* message, 	IN const u32 msglen,
                                IN const u8* piv, 		IN const u32 ivlen,
                                IN const u8* paad, 		IN const u32 aadlen,
                                OUT u8* pResult, OUT u8* pTag)
{
	int ret;

	pIE->cipher_type |= CIPHER_TYPE_MODE_ENCRYPT;
	pIE->isDecrypt = _FALSE;

	ret = CRYPTO_ProcessAD(pIE, message, msglen,
			piv, ivlen, paad, aadlen, pResult, pTag);

	return ret;
}

/**
  * @brief  Cipher decryption.
  * @param  pIE: Point to crypto adapter. 
  * @param  message: Point to message. 
  * @param  msglen: Message length. 
  * @param  pIv: Point to IV(initialize vector). 
  * @param  ivlen: IV length. 
  * @param  paad: Point to AAD(additional authentication data). 
  * @param  aadlen: AAD length. 
  * @param  pResult: Point to auth or cipher result. 
  * @param  pTag: Point to MAC(Message Authentication Code) . 
  * @retval Result of process.
  */
IMAGE2_RAM_TEXT_SECTION
int CRYPTO_CipherDecryptAD(
    HAL_CRYPTO_ADAPTER *pIE,
    IN const u8* message, 	IN const u32 msglen,
    IN const u8* piv, 			IN const u32 ivlen,
    IN const u8* paad,			IN const u32 aadlen,
    OUT u8* pResult, OUT u8* pTag )
{
	int ret;

	pIE->cipher_type ^= CIPHER_TYPE_MODE_ENCRYPT;
	pIE->isDecrypt = _TRUE;
	
	ret = CRYPTO_ProcessAD(pIE, message, msglen, piv, ivlen, paad, aadlen, pResult, pTag);

	return ret;
}

/******************* (C) COPYRIGHT 2017 Realtek Semiconductor *****END OF FILE****/
