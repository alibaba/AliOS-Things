/**
  * @file  hal_crypto.c
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

#include <stdbool.h>
#include "driver/chip/hal_crypto.h"
#include "hal_base.h"
#include "sys/endian.h"
#include "pm/pm.h"


/*************************************** Debug *****************************************/
#define CE_MODULE (DBG_OFF | XR_LEVEL_DEBUG)
#define CE_REG_DEBUG

#define CE_ASSERT(condition) XR_ASSERT(condition, CE_MODULE, #condition " failed\n")

#define CE_DEBUG(msg, arg...) XR_DEBUG(CE_MODULE, NOEXPAND, "[CE Debug] " msg, ##arg)

#define CE_ALERT(msg, arg...) XR_ALERT(CE_MODULE, NOEXPAND, "[CE Alert] " msg, ##arg)

#define CE_ENTRY() XR_ENTRY(CE_MODULE, "[CE Entry]")

#define CE_EXIT(val) XR_RET(CE_MODULE, "[CE Exit]", val)

extern bool reg_show;

#ifdef CE_REG_DEBUG
#define CE_REG(reg) { \
		if (reg_show) \
			CE_DEBUG("register " #reg ": 0x%x.\n", reg); \
	}

#define CE_REG_ALL(ce) { \
		CE_REG(ce->CTL); \
		CE_REG(ce->KEY[0]); \
		CE_REG(ce->KEY[1]); \
		CE_REG(ce->KEY[2]); \
		CE_REG(ce->KEY[3]); \
		CE_REG(ce->KEY[4]); \
		CE_REG(ce->KEY[5]); \
		CE_REG(ce->KEY[6]); \
		CE_REG(ce->KEY[7]); \
		CE_REG(ce->IV[0]); \
		CE_REG(ce->IV[1]); \
		CE_REG(ce->IV[2]); \
		CE_REG(ce->IV[3]); \
		CE_REG(ce->CNT[0]); \
		CE_REG(ce->CNT[1]); \
		CE_REG(ce->CNT[2]); \
		CE_REG(ce->CNT[3]); \
		CE_REG(ce->FCSR); \
		CE_REG(ce->ICSR); \
		CE_REG(ce->MD0); \
		CE_REG(ce->MD1); \
		CE_REG(ce->MD2); \
		CE_REG(ce->MD3); \
		CE_REG(ce->MD4); \
		CE_REG(ce->CTS_LEN); \
		CE_REG(ce->CRC_POLY); \
		CE_REG(ce->CRC_RESULT); \
		CE_REG(ce->MD5); \
		CE_REG(ce->MD6); \
		CE_REG(ce->MD7); \
	}

#else
#define CE_REG_ALL(ce)

#define CE_REG(reg)

#endif


#define __CE_STATIC_INLINE__ static inline

bool reg_show;

/*
 * @brief CE
 */
__CE_STATIC_INLINE__
void CE_Enable(CE_T *ce)
{
	HAL_SET_BIT(ce->CTL, CE_CTL_ENABLE_MASK);
}

__CE_STATIC_INLINE__
void CE_Disable(CE_T *ce)
{
	HAL_CLR_BIT(ce->CTL, CE_CTL_ENABLE_MASK);
}

__CE_STATIC_INLINE__
uint8_t CE_GetInputAvailCnt(CE_T *ce)
{
	return HAL_GET_BIT_VAL(ce->FCSR, CE_FCSR_RXFIFO_EMP_CNT_SHIFT, CE_FCSR_RXFIFO_EMP_CNT_VMASK);
}

__CE_STATIC_INLINE__
uint8_t CE_GetOutputAvailCnt(CE_T *ce)
{
	return HAL_GET_BIT_VAL(ce->FCSR, CE_FCSR_TXFIFO_AVA_CNT_SHIFT, CE_FCSR_TXFIFO_AVA_CNT_VMASK);
}

__CE_STATIC_INLINE__
void CE_SetInputThreshold(CE_T *ce, uint8_t threshold)
{
	CE_ASSERT((threshold & ~CE_FCSR_TXFIFO_INT_TRIG_LEVEL_MASK) == 0); //rx vmask = tx vmask = tx mask
	HAL_MODIFY_REG(ce->FCSR, CE_FCSR_RXFIFO_INT_TRIG_LEVEL_MASK, threshold << CE_FCSR_RXFIFO_INT_TRIG_LEVEL_SHIFT);
}

__CE_STATIC_INLINE__
void CE_SetOutputThreshold(CE_T *ce, uint8_t threshold)
{
	CE_ASSERT((threshold & ~CE_FCSR_TXFIFO_INT_TRIG_LEVEL_MASK) == 0);
	HAL_MODIFY_REG(ce->FCSR, CE_FCSR_TXFIFO_INT_TRIG_LEVEL_MASK, threshold << CE_FCSR_TXFIFO_INT_TRIG_LEVEL_SHIFT);
}

typedef enum {
	CE_INT_TPYE_TXFIFO_AVA = CE_ICSR_TXFIFO_AVA_INT_EN_SHIFT,
	CE_INT_TPYE_RXFIFO_EMP = CE_ICSR_RXFIFO_EMP_INT_EN_SHIFT,
	CE_INT_TPYE_HASH_CRC_END = CE_ICSR_HASH_CRC_END_INT_EN_SHIFT
} CE_Int_Type;

__CE_STATIC_INLINE__
void CE_EnableInt(CE_T *ce, CE_Int_Type type)
{
	HAL_SET_BIT(ce->ICSR, 1 << type);
}

__CE_STATIC_INLINE__
void CE_DisableInt(CE_T *ce, CE_Int_Type type)
{
	HAL_CLR_BIT(ce->ICSR, 1 << type);
}

__CE_STATIC_INLINE__
bool CE_Status(CE_T *ce, CE_Int_Type type)
{
	return (bool)HAL_GET_BIT_VAL(ce->ICSR, CE_ICSR_TXFIFO_AVA_SHIFT + type, 1);
}

__CE_STATIC_INLINE__
void CE_EnableDMA(CE_T *ce)
{
	HAL_SET_BIT(ce->ICSR, CE_ICSR_DRQ_ENABLE_MASK);
}

__CE_STATIC_INLINE__
void CE_DisableDMA(CE_T *ce)
{
	HAL_CLR_BIT(ce->ICSR, CE_ICSR_DRQ_ENABLE_MASK);
}

__CE_STATIC_INLINE__
void CE_SetLength(CE_T *ce, uint32_t len)
{
	ce->CTS_LEN = len;
}

/*
 * @brief CRC
 */
__CE_STATIC_INLINE__
HAL_Status CE_CRC_Init(CE_T *ce,
					   bool init_val,
					   bool xor_out,
					   bool ref_in,
					   bool ref_out,
					   CE_CTL_CRC_Width width,
					   uint32_t poly)
{
	bool last_pckt = 0;

	HAL_CLR_BIT(ce->CTL, CE_CTL_END_BIT_MASK);
	HAL_MODIFY_REG(ce->CTL,
				   CE_CTL_CRC_INIT_MASK
				   | CE_CTL_CRC_REF_IN_MASK
				   | CE_CTL_CRC_REF_OUT_MASK
				   | CE_CTL_CRC_XOR_OUT_MASK
				   | CE_CTL_CRC_WIDTH_MASK
				   | CE_CTL_CRC_CONT_MASK
				   | CE_CTL_OP_DIR_MASK
				   | CE_CTL_METHOD_MASK,
				   (init_val << CE_CTL_CRC_INIT_SHIFT)
				   | (ref_in << CE_CTL_CRC_REF_IN_SHIFT)
				   | (ref_out << CE_CTL_CRC_REF_OUT_SHIFT)
				   | (xor_out << CE_CTL_CRC_XOR_OUT_SHIFT)
				   | (width)
				   | (last_pckt << CE_CTL_CRC_CONT_SHIFT) /* not the last packet */
				   | CE_CRYPT_OP_ENCRYPTION
				   | CE_CTL_METHOD_CRC);
	ce->CRC_POLY = poly;
	return HAL_OK;
}

__CE_STATIC_INLINE__
void CE_CRC_Fill(CE_T *ce, uint32_t data)
{
	while (CE_GetInputAvailCnt(ce) == 0);
	ce->RXFIFO = data;
}

__CE_STATIC_INLINE__
void CE_CRC_Finish(CE_T *ce)
{
	HAL_SET_BIT(ce->CTL, CE_CTL_END_BIT_MASK);
}

__CE_STATIC_INLINE__
void CE_CRC_Calc(CE_T *ce, uint32_t *crc)	//_____________16bit mode is it generate a 16bits crc?
{
	*crc = ce->CRC_RESULT;
}

__CE_STATIC_INLINE__
void CE_CRC_Deinit(CE_T *ce)
{
	/*CE_Disable(ce);*/
}

__CE_STATIC_INLINE__
void CE_CRC_SetResult(CE_T *ce, uint32_t crc)
{
	ce->CRC_RESULT = crc;
}

/*
void CE_CRC_SetLength(CE_T *ce, uint32_t len)
{
	CE_SetLength(ce, len);
}
*/


/*
 * @brief AES/DES/3DES
 */
__CE_STATIC_INLINE__
void CE_Crypto_Init(CE_T *ce, CE_CTL_Method algo, CE_CTL_Crypto_Mode mode, CE_Crypto_Op op)
{
	CE_ASSERT(algo > CE_CTL_METHOD_3DES);

	HAL_MODIFY_REG(ce->CTL,
				   CE_CTL_METHOD_MASK
				   | CE_CTL_OP_MODE_MASK
				   | CE_CTL_OP_DIR_MASK,
				   algo
				   | mode
				   | op); /* ce enable */
}

__CE_STATIC_INLINE__
void CE_AES_SetKey(CE_T *ce, CE_CTL_KeySource src, CE_CTL_AES_KeySize size, uint32_t *key)
{
	uint8_t key_cnt;
	uint8_t i = 0;
	HAL_MODIFY_REG(ce->CTL, CE_CTL_KEY_SEL_MASK | CE_CTL_AES_KEY_SIZE_MASK, src | size);
	if (size == CE_CTL_AES_KEYSIZE_128BITS)
		key_cnt = 4;
	else if (size == CE_CTL_AES_KEYSIZE_192BITS)
		key_cnt = 6;
	else if (size == CE_CTL_AES_KEYSIZE_256BITS)
		key_cnt = 8;
	else {
		CE_ALERT("param is out of range\n");
		return;
	}

	while (i < key_cnt) {
		ce->KEY[i] = *(key++);
		i++;
	}
}

__CE_STATIC_INLINE__
void CE_DES_SetKey(CE_T *ce, CE_CTL_KeySource src, uint32_t key[2]) //56bit key? 64bit key? = 2 * 32bit reg?
{
	HAL_MODIFY_REG(ce->CTL, CE_CTL_KEY_SEL_MASK, src);
	ce->KEY[0] = key[0];
	ce->KEY[1] = key[1];
}

__CE_STATIC_INLINE__
void CE_3DES_SetKey(CE_T *ce, CE_CTL_KeySource src, uint32_t key[6]) //3 * 64bit key? = 6 * 32bit reg?
{
	uint8_t i = 0;
	HAL_MODIFY_REG(ce->CTL, CE_CTL_KEY_SEL_MASK, src);

	while (i < CE_3DES_KEY_COUNT) {
		ce->KEY[i] = *(key++);
		i++;
	}
}

#ifdef CE_CTR_MODE
__CE_STATIC_INLINE__
HAL_Status CE_Crypto_SetCounter(CE_T *ce, uint32_t *cnt, CE_CTL_CtrWidth size)
{
	HAL_MODIFY_REG(ce->CTL, CE_CTL_CTR_WIDTH_MASK, size);
	switch (size) {
	case CE_CTL_CTRWITDH_128BITS:
		ce->CNT[3] = *(cnt + 3);
		ce->CNT[2] = *(cnt + 2);
	case CE_CTL_CTRWITDH_64BITS:
		ce->CNT[1] = *(cnt + 1);
	case CE_CTL_CTRWITDH_32BITS:
		ce->CNT[0] = *(cnt + 0);
		break;
	case CE_CTL_CTRWITDH_16BITS:
		ce->CNT[0] = *(cnt + 0) & 0xFFFF;
		break;
	default:
		return HAL_INVALID;
	}
	return HAL_OK;
}
#endif

__CE_STATIC_INLINE__
void CE_Crypto_SetLength(CE_T *ce, uint32_t len)
{
	CE_SetLength(ce, len);
}

__CE_STATIC_INLINE__
void CE_Crypto_SetIV(CE_T *ce, uint32_t iv[4], uint8_t size)
{
	uint8_t i = size / 4;
	while (i--)
		ce->IV[i] = iv[i];
}

__CE_STATIC_INLINE__
void CE_Crypto_Input(CE_T *ce, uint32_t data)
{
	while (CE_GetInputAvailCnt(ce) == 0);
	ce->RXFIFO = data;
}

__CE_STATIC_INLINE__
void CE_Crypto_Output(CE_T *ce, uint32_t *data)
{
	while (CE_GetOutputAvailCnt(ce) == 0);
	*data = ce->TXFIFO;
}

__CE_STATIC_INLINE__
void CE_Crypto_Deinit(CE_T *ce)
{
	CE_Disable(ce);
}

__CE_STATIC_INLINE__
volatile uint32_t *CE_Crypto_GetInputAddr(CE_T *ce)
{
	return &ce->RXFIFO;
}

__CE_STATIC_INLINE__
volatile const uint32_t *CE_Crypto_GetOutputAddr(CE_T *ce)
{
	return &ce->TXFIFO;
}

/*HAL_Status CE_Crypto_GetOutput(CE_T *ce, uint32_t *data)
{}

HAL_Status CE_Crypto_GetInput(CE_T *ce, uint32_t *data)
{}*/

__CE_STATIC_INLINE__
void CE_Crypto_GetIV(CE_T *ce, uint32_t iv[4], uint8_t size)
{
	uint8_t i = size / 4;
	while (i--)
		iv[i] = ce->IV[i];
}

#ifdef CE_CTR_MODE
__CE_STATIC_INLINE__
HAL_Status CE_Crypto_GetCounter(CE_T *ce, uint32_t cnt[4], CE_CTL_CtrWidth size)
{
	switch (size) {
	case CE_CTL_CTRWITDH_128BITS:
		cnt[2] = ce->CNT[2];
		cnt[3] = ce->CNT[3];
	case CE_CTL_CTRWITDH_64BITS:
		cnt[1] = ce->CNT[1];
	case CE_CTL_CTRWITDH_32BITS:
	case CE_CTL_CTRWITDH_16BITS:
		cnt[0] = ce->CNT[0];
		break;
	default:
		return HAL_INVALID;
	}
	return HAL_OK;
}
#endif

/*
 * @brief MD5/sha1/sha256
 */
void CE_Hash_Init(CE_T *ce, CE_CTL_Method algo)
{
	CE_ASSERT((algo != CE_CTL_METHOD_SHA1) && (algo != CE_CTL_METHOD_MD5) && (algo != CE_CTL_METHOD_SHA256));

	HAL_CLR_BIT(ce->CTL, CE_CTL_END_BIT_MASK);
	HAL_MODIFY_REG(ce->CTL,
				   CE_CTL_METHOD_MASK | CE_CTL_ENABLE_MASK | CE_CTL_OP_DIR_MASK,
				   algo | (1 << CE_CTL_ENABLE_SHIFT) | CE_CRYPT_OP_ENCRYPTION);
}

void CE_Hash_SetIV(CE_T *ce, CE_CTL_IvMode_SHA_MD5 iv_src, uint32_t *iv, uint32_t iv_size)
{
	HAL_MODIFY_REG(ce->CTL, CE_CTL_IV_MODE_MASK, iv_src);

	if (iv_src == CE_CTL_IVMODE_SHA_MD5_FIPS180) {	//necessary?
		ce->IV[0] = 0;
		ce->IV[1] = 0;
		ce->IV[2] = 0;
		ce->IV[3] = 0;
		return;
	}

	ce->IV[0] = iv[0];
	ce->IV[1] = iv[1];
	ce->IV[2] = iv[2];
	ce->IV[3] = iv[3];
	if (iv_size == 5)
		ce->CNT[0] = iv[4];
	else if (iv_size == 8) {
		ce->CNT[0] = iv[4];
		ce->CNT[1] = iv[5];
		ce->CNT[2] = iv[6];
		ce->CNT[3] = iv[7];
	}

}

__CE_STATIC_INLINE__
void CE_Hash_Finish(CE_T *ce)
{
	HAL_SET_BIT(ce->CTL, CE_CTL_END_BIT_MASK);
}

__CE_STATIC_INLINE__
void CE_Hash_Fill(CE_T *ce, uint32_t data)
{
	while (CE_GetInputAvailCnt(ce) == 0);
	ce->RXFIFO = data;
}

void CE_Hash_Calc(CE_T *ce, CE_CTL_Method algo, uint32_t *digest)	// md5[5] sha1[5] sha256[8]
{
	if (algo == CE_CTL_METHOD_SHA256) {
		*(digest + 7) = ce->MD7;
		*(digest + 6) = ce->MD6;
		*(digest + 5) = ce->MD5;
		*(digest + 4) = ce->MD4;
	} else if (algo == CE_CTL_METHOD_SHA1)
		*(digest + 4) = ce->MD4;
	*(digest + 3) = ce->MD3;
	*(digest + 2) = ce->MD2;
	*(digest + 1) = ce->MD1;
	*(digest + 0) = ce->MD0;
}

__CE_STATIC_INLINE__
void CE_Hash_Deinit(CE_T *ce)
{
	CE_Disable(ce);
}


/*
 * @brief PRNG
 */
__CE_STATIC_INLINE__
void CE_PRNG_Init(CE_T *ce, bool continue_mode)
{
	HAL_MODIFY_REG(ce->CTL,
				   CE_CTL_PRNG_MODE_MASK | CE_CTL_METHOD_MASK | CE_CTL_ENABLE_MASK,
				   (continue_mode << CE_CTL_PRNG_MODE_SHIFT) | CE_CTL_METHOD_PRNG | (1 << CE_CTL_ENABLE_SHIFT));

}

__CE_STATIC_INLINE__
void CE_PRNG_Seed(CE_T *ce, uint32_t seed[6])
{
	ce->KEY[0] = seed[0];
	ce->KEY[1] = seed[1];
	ce->KEY[2] = seed[2];
	ce->KEY[3] = seed[3];
	ce->KEY[4] = seed[4];
	ce->KEY[5] = seed[5];
}

__CE_STATIC_INLINE__
void CE_PRNG_Generate(CE_T *ce, uint32_t random[5])
{
	HAL_SET_BIT(ce->CTL, CE_CTL_PRNG_START_MASK);

	while (HAL_GET_BIT(CE->CTL, CE_CTL_PRNG_START_MASK) != 0);

	random[0] = ce->MD0;
	random[1] = ce->MD1;
	random[2] = ce->MD2;
	random[3] = ce->MD3;
	random[4] = ce->MD4;
}

__CE_STATIC_INLINE__
void CE_PRNG_Deinit(CE_T *ce)
{
	CE_Disable(ce);
}


/************************ AES DES 3DES private **************************************/
static HAL_Mutex ce_lock;
static HAL_Semaphore ce_block;

__CE_STATIC_INLINE__
void HAL_CE_EnableCCMU()
{
	HAL_CCM_BusEnablePeriphClock(CCM_BUS_PERIPH_BIT_CE);
	HAL_CCM_CE_EnableMClock();
}

__CE_STATIC_INLINE__
void HAL_CE_DisableCCMU()
{
	HAL_CCM_CE_DisableMClock();
	HAL_CCM_BusDisablePeriphClock(CCM_BUS_PERIPH_BIT_CE);
}

/*static void HAL_CE_InputCmpl(void *arg)
{
	CE_DEBUG("input by dma had been finished.\n");
}

static void HAL_CE_OutputCmpl(void *arg)
{
	HAL_SemaphoreRelease(&ce_block);
	CE_DEBUG("output by dma had been finished.\n");
}*/

static void HAL_CE_DMACmpl(void *arg)
{
	if (arg != NULL)
		HAL_SemaphoreRelease(&ce_block);
	CE_DEBUG("Transfer by dma had been finished.\n");
}

static HAL_Status HAL_Crypto_InitDMA(DMA_Channel *input, DMA_Channel *output)
{
	HAL_Status ret = HAL_OK;
	DMA_ChannelInitParam Output_param;
	DMA_ChannelInitParam Input_param;
	memset(&Output_param, 0, sizeof(Output_param));
	memset(&Input_param, 0, sizeof(Input_param));
	CE_ENTRY();

	if ((*output = HAL_DMA_Request()) == DMA_CHANNEL_INVALID) {
		CE_ALERT("DMA request failed \n");
		ret = HAL_INVALID;
		goto out;
	}
	if ((*input = HAL_DMA_Request()) == DMA_CHANNEL_INVALID) {
		CE_ALERT("DMA request failed \n");
		HAL_DMA_Release(*output);
		ret = HAL_INVALID;
		goto out;
	}

	Output_param.cfg = HAL_DMA_MakeChannelInitCfg(DMA_WORK_MODE_SINGLE,
											  DMA_WAIT_CYCLE_16,
											  DMA_BYTE_CNT_MODE_REMAIN,
											  DMA_DATA_WIDTH_8BIT,
											  DMA_BURST_LEN_1,
											  DMA_ADDR_MODE_INC,
											  DMA_PERIPH_SRAM,
											  DMA_DATA_WIDTH_32BIT,
											  DMA_BURST_LEN_1,
											  DMA_ADDR_MODE_FIXED,
											  DMA_PERIPH_CE);
	Output_param.irqType = DMA_IRQ_TYPE_END;
	Output_param.endCallback = HAL_CE_DMACmpl;
	Output_param.endArg = &ce_block;

	Input_param.cfg = HAL_DMA_MakeChannelInitCfg(DMA_WORK_MODE_SINGLE,
											  DMA_WAIT_CYCLE_16,
											  DMA_BYTE_CNT_MODE_REMAIN,
											  DMA_DATA_WIDTH_32BIT,
											  DMA_BURST_LEN_1,
											  DMA_ADDR_MODE_FIXED,
											  DMA_PERIPH_CE,
											  DMA_DATA_WIDTH_8BIT,
											  DMA_BURST_LEN_1,
											  DMA_ADDR_MODE_INC,
											  DMA_PERIPH_SRAM);
	Input_param.irqType = DMA_IRQ_TYPE_END;
	Input_param.endCallback = HAL_CE_DMACmpl;
	Input_param.endArg = NULL;

	HAL_DMA_Init(*input, &Input_param);
	HAL_DMA_Init(*output, &Output_param);

out:
	CE_EXIT(ret);
	return ret;
}

static inline void HAL_Crypto_DenitDMA(DMA_Channel input, DMA_Channel output)
{
	HAL_DMA_DeInit(input);
	HAL_DMA_DeInit(output);
	HAL_DMA_Release(input);
	HAL_DMA_Release(output);
}

static HAL_Status HAL_Crypto_Convey(uint8_t *input, uint8_t *output, uint32_t size, uint8_t block_size)
{
	HAL_Status ret = HAL_OK;
	uint32_t align_len = size & (~(block_size - 1));
	uint32_t remain_len = size & (block_size - 1);
	uint8_t padding[16] = {0};
	CE_ENTRY();
//	CE_DEBUG("align_len = %d, remain_len = %d, len = %d\n", align_len, remain_len, remain_len ? (align_len + block_size) : align_len);

	DMA_Channel Output_channel = DMA_CHANNEL_INVALID;
	DMA_Channel Input_channel = DMA_CHANNEL_INVALID;

	/* data preprocess */
	memcpy(padding, &input[align_len], remain_len);
	CE_Crypto_SetLength(CE, remain_len ? (align_len + block_size) : align_len);
	CE_REG_ALL(CE);

	/* data transfer by DMA */
	if ((ret = HAL_Crypto_InitDMA(&Input_channel, &Output_channel)) != HAL_OK){
		CE_ALERT("DMA Request failed\n");
		goto out;
	}

	CE_SetInputThreshold(CE, 0);
	CE_SetOutputThreshold(CE, 0);
	CE_EnableDMA(CE);
	CE_Enable(CE);

	CE_REG_ALL(CE);

	if (align_len != 0) {
		HAL_DMA_Start(Input_channel, (uint32_t)input, (uint32_t)CE_Crypto_GetInputAddr(CE), align_len);
		HAL_DMA_Start(Output_channel, (uint32_t)CE_Crypto_GetOutputAddr(CE), (uint32_t)output, align_len);
		if ((ret = HAL_SemaphoreWait(&ce_block, CE_WAIT_TIME)) != HAL_OK) {
			CE_ALERT("DMA 1st transfer failed\n");
			HAL_DMA_Stop(Input_channel);
			HAL_DMA_Stop(Output_channel);
			goto failed;
		}
		HAL_DMA_Stop(Input_channel);
		HAL_DMA_Stop(Output_channel);
	}

	if (remain_len != 0) {
		HAL_DMA_Start(Input_channel, (uint32_t)padding, (uint32_t)CE_Crypto_GetInputAddr(CE), block_size);
		CE_REG_ALL(CE);

		HAL_DMA_Start(Output_channel, (uint32_t)CE_Crypto_GetOutputAddr(CE), (uint32_t)&output[align_len], block_size);
		if ((ret = HAL_SemaphoreWait(&ce_block, CE_WAIT_TIME)) != HAL_OK) {
			CE_ALERT("DMA 2nd transfer failed\n");
			CE_REG_ALL(CE);
			HAL_DMA_Stop(Input_channel);
			HAL_DMA_Stop(Output_channel);
			goto failed;
		}
		CE_REG_ALL(CE);
		HAL_DMA_Stop(Input_channel);
		HAL_DMA_Stop(Output_channel);
	}

failed:
	CE_Disable(CE);
	CE_DisableDMA(CE);
	HAL_Crypto_DenitDMA(Input_channel, Output_channel);

out:
	CE_EXIT(ret);
	return ret;
}

int ce_running = 0;
#ifdef CONFIG_PM
static int hal_ce_suspending = 0;

static int ce_suspend(struct soc_device *dev, enum suspend_state_t state)
{
	if (ce_running)
		return -1;
	hal_ce_suspending = 1;

	return 0;
}

static int ce_resume(struct soc_device *dev, enum suspend_state_t state)
{
	switch (state) {
	case PM_MODE_SLEEP:
		break;
	case PM_MODE_STANDBY:
	case PM_MODE_HIBERNATION:
		HAL_CE_Init();
		break;
	default:
		break;
	}

	hal_ce_suspending = 0;

	return 0;
}

static struct soc_device_driver ce_drv = {
	.name = "ce",
	.suspend_noirq = ce_suspend,
	.resume_noirq = ce_resume,
};

static struct soc_device ce_dev = {
	.name = "ce",
	.driver = &ce_drv,
};

#define CE_DEV (&ce_dev)
#else
#define CE_DEV NULL
#endif


/************************ public **************************************/

/**
  * @brief Initialize the Crypto Engine Module.
  * @param None
  * @retval HAL_Status:  The result status of initializtion
  */
HAL_Status HAL_CE_Init()
{
	HAL_Status ret = HAL_OK;
	uint32_t clk;
	uint32_t div;
	CE_ENTRY();

	HAL_CE_DisableCCMU();
	HAL_CCM_BusForcePeriphReset(CCM_BUS_PERIPH_BIT_CE);
	HAL_CCM_BusReleasePeriphReset(CCM_BUS_PERIPH_BIT_CE);
	clk = HAL_GetDevClock();
	div = (clk - 1) / (80 * 1000 * 1000) + 1;
	CE_DEBUG("CCMU src clk is %d, div is %d\n", clk, div);

	if (div > (8 * 16))
		return HAL_ERROR;
	else if (div > (4 * 16))
		HAL_CCM_CE_SetMClock(CCM_AHB_PERIPH_CLK_SRC_DEVCLK, CCM_PERIPH_CLK_DIV_N_8, (CCM_PeriphClkDivM)((div >> 3) - 1));
	else if (div > (2 * 16))
		HAL_CCM_CE_SetMClock(CCM_AHB_PERIPH_CLK_SRC_DEVCLK, CCM_PERIPH_CLK_DIV_N_4, (CCM_PeriphClkDivM)((div >> 2) - 1));
	else if (div > (1 & 16))
		HAL_CCM_CE_SetMClock(CCM_AHB_PERIPH_CLK_SRC_DEVCLK, CCM_PERIPH_CLK_DIV_N_2, (CCM_PeriphClkDivM)((div >> 1) - 1));
	else
		HAL_CCM_CE_SetMClock(CCM_AHB_PERIPH_CLK_SRC_DEVCLK, CCM_PERIPH_CLK_DIV_N_1, (CCM_PeriphClkDivM)((div >> 0) - 1));

#ifdef CONFIG_PM
	if (!hal_ce_suspending) {
#endif

	if ((ret = HAL_MutexInit(&ce_lock)) != HAL_OK)
		goto out;
	if ((ret = HAL_SemaphoreInit(&ce_block, 0, 1)) != HAL_OK) {
		HAL_MutexDeinit(&ce_lock);
		goto out;
	}

	ce_running = 0;

#ifdef CONFIG_PM
	pm_register_ops(CE_DEV);
	}
#endif

out:
	CE_EXIT(ret);
	return ret;
}

/**
  * @brief Deinitializes the Crypto Engine Module.
  * @param None
  * @retval HAL_Status:  The status of driver
  */
HAL_Status HAL_CE_Deinit()
{
	HAL_Status ret = HAL_OK;
	CE_ENTRY();

#ifdef CONFIG_PM
	if (!hal_ce_suspending)
		pm_unregister_ops(CE_DEV);
#endif

	if ((ret = HAL_MutexDeinit(&ce_lock)) != HAL_OK)
		goto out;
	if ((ret = HAL_SemaphoreDeinit(&ce_block)) != HAL_OK)
		goto out;
out:
	CE_EXIT(ret);
	return ret;
}


/**
  * @brief Encrypt data by AES.
  * @note AES128 & AES192 & AES256 limit the key size, oversize of key will be cut
  *       off. AES block size is 16 bytes.
  * @param aes:
  *        @arg aes->mode: CE_CRYPT_MODE_ECB or CE_CRYPT_MODE_CBC
  *        @arg aes->iv[16]: initialization value, is same size as block size.
  *                          CE_CRYPT_MODE_CBC mode need this param.
  *        @arg aes->src: CE_CTL_KEYSOURCE_INPUT: key is set from aes->key.
  *                       CE_CTL_KEYSOURCE_SID: is not support for now.
  *                       CE_CTL_KEYSOURCE_INTERNAL0~7: is not support for now.
  *        @arg aes->key[32]: store the key. please notice the size should same as
  *                           the algorithm. example: AES128 should only use
  *                           key[0]~key[15].
  *        @arg aes->keysize: CE_CTL_AES_KEYSIZE_128BITS: algorithm of AES128.
  *                           CE_CTL_AES_KEYSIZE_192BITS: algorithm of AES192.
  *                           CE_CTL_AES_KEYSIZE_256BITS: algorithm of AES256.
  * @param plain: plain data which is need to be encrypt.
  * @param cipher: cipher data which is store the encrpyted data. the cipher data
  *                size is multiple of 16 bytes and no less than plain data size.
  * @param size: size of plain data. if encrypt a segment of data, size should
  *              be multiple of 16 bytes.
  * @retval HAL_Status:  The status of driver
  */
HAL_Status HAL_AES_Encrypt(CE_AES_Config *aes, uint8_t *plain, uint8_t *cipher, uint32_t size)
{
	HAL_Status ret = HAL_OK;
	CE_ENTRY();

	if (size == 0) {
		ret = HAL_INVALID;
		goto out;
	}

	ce_running = 1;
	if ((ret = HAL_MutexLock(&ce_lock, CE_WAIT_TIME)) != HAL_OK)
		goto out;

	HAL_CE_EnableCCMU();

	/* CE config */
	CE_Crypto_Init(CE, CE_CTL_METHOD_AES, aes->mode, CE_CRYPT_OP_ENCRYPTION);
	CE_AES_SetKey(CE, aes->src, aes->keysize, (uint32_t *)aes->key);
	if ((CE_CTL_Crypto_Mode)aes->mode == CE_CTL_CRYPT_MODE_CBC)
		CE_Crypto_SetIV(CE, (uint32_t *)aes->iv, 16);
#ifdef CE_CTR_MODE
	else if ((CE_CTL_Crypto_Mode)aes->mode == CE_CTL_CRYPT_MODE_CTR)
		CE_Crypto_SetCounter(CE, (uint32_t *)aes->cnt, aes->width);
#endif
#ifdef CE_CTS_MODE
	else if ((CE_CTL_Crypto_Mode)aes->mode == CE_CTL_CRYPT_MODE_CTS)
		CE_Crypto_SetIV(CE, (uint32_t *)aes->iv, 16);
#endif

	CE_REG_ALL(CE);

	/* plain data convey to CE and cipher data convey from CE */
	HAL_Crypto_Convey(plain, cipher, size, AES_BLOCK_SIZE);

	CE_REG_ALL(CE);
	/* CE state storage */
	if ((CE_CTL_Crypto_Mode)aes->mode == CE_CTL_CRYPT_MODE_CBC)
		CE_Crypto_GetIV(CE, (uint32_t *)aes->iv, 16);
#ifdef CE_CTR_MODE
	else if ((CE_CTL_Crypto_Mode)aes->mode == CE_CTL_CRYPT_MODE_CTR) {
		CE_Crypto_GetCounter(CE, (uint32_t *)aes->cnt, aes->width);
		aes->cnt[15] += (size + 15) / 16;
	}
#endif
#ifdef CE_CTS_MODE
	else if ((CE_CTL_Crypto_Mode)aes->mode == CE_CTL_CRYPT_MODE_CTS)
		CE_Crypto_GetIV(CE, (uint32_t *)aes->iv, 16);
#endif

	CE_Crypto_Deinit(CE);
	HAL_CE_DisableCCMU();
	HAL_MutexUnlock(&ce_lock);
out:
	ce_running = 0;
	CE_EXIT(ret);
	return ret;
}

/**
  * @brief Decrypt data by AES.
  * @note AES128 & AES192 & AES256 limit the key size, oversize of key will be cut
  *       off. AES block size is 16 bytes. and please reset the aes config if doing
  *       a loopback(HAL_AES_Encrypt then HAL_AES_Decrypt).
  * @param aes:
  *        @arg aes->mode: CE_CRYPT_MODE_ECB or CE_CRYPT_MODE_CBC
  *        @arg aes->iv[16]: initialization value, is same size as block size.
  *                          CE_CRYPT_MODE_CBC mode need this param, and same as
  *                          the encrypt side.
  *        @arg aes->src: CE_CTL_KEYSOURCE_INPUT: key is set from aes->key.
  *                       CE_CTL_KEYSOURCE_SID: is not support for now.
  *                       CE_CTL_KEYSOURCE_INTERNAL0~7: is not support for now.
  *        @arg aes->key[32]: store the key. please notice the size should same as
  *                           the algorithm. example: AES128 should only use
  *                           key[0]~key[15].
  *        @arg aes->keysize: CE_CTL_AES_KEYSIZE_128BITS: algorithm of AES128.
  *                           CE_CTL_AES_KEYSIZE_192BITS: algorithm of AES192.
  *                           CE_CTL_AES_KEYSIZE_256BITS: algorithm of AES256.
  * @param cipher: cipher data which is needed to be decrypted. the cipher data
  *                size must be multiple of 16 bytes.
  * @param plain: plain data which store the decrypted data. the plain data size
  *               will be same as cipher data.
  * @param size: size of cipher data. size must be multiple of 16 bytes.
  * @retval HAL_Status:  The status of driver
  */
HAL_Status HAL_AES_Decrypt(CE_AES_Config *aes, uint8_t *cipher, uint8_t *plain, uint32_t size)
{
	HAL_Status ret = HAL_OK;
	CE_ENTRY();

	if (size == 0) {
		ret = HAL_INVALID;
		goto out;
	}

	ce_running = 1;
	if ((ret = HAL_MutexLock(&ce_lock, CE_WAIT_TIME)) != HAL_OK)
		goto out;

	HAL_CE_EnableCCMU();

	/* CE config */
	CE_Crypto_Init(CE, CE_CTL_METHOD_AES, aes->mode, CE_CRYPT_OP_DECRYPTION);
	CE_AES_SetKey(CE, aes->src, aes->keysize, (uint32_t *)aes->key);
	if ((CE_CTL_Crypto_Mode)aes->mode == CE_CTL_CRYPT_MODE_CBC)
		CE_Crypto_SetIV(CE, (uint32_t *)aes->iv, 16);
#ifdef CE_CTR_MODE
	else if ((CE_CTL_Crypto_Mode)aes->mode == CE_CTL_CRYPT_MODE_CTR)
		CE_Crypto_SetCounter(CE, (uint32_t *)aes->cnt, aes->width);
#endif
#ifdef CE_CTS_MODE
	else if ((CE_CTL_Crypto_Mode)aes->mode == CE_CTL_CRYPT_MODE_CTS)
		CE_Crypto_SetIV(CE, (uint32_t *)aes->iv, 16);
#endif

	CE_REG_ALL(CE);

	/* plain data convey to CE and cipher data convey from CE */
	HAL_Crypto_Convey(cipher, plain, size, AES_BLOCK_SIZE);

	CE_REG_ALL(CE);

	/* CE state storage */
	if ((CE_CTL_Crypto_Mode)aes->mode == CE_CTL_CRYPT_MODE_CBC)
		CE_Crypto_GetIV(CE, (uint32_t *)aes->iv, 16);
#ifdef CE_CTR_MODE
	else if ((CE_CTL_Crypto_Mode)aes->mode == CE_CTL_CRYPT_MODE_CTR) {
		CE_Crypto_GetCounter(CE, (uint32_t *)aes->cnt, aes->width);
		aes->cnt[15] += (size + 15) / 16;
	}
#endif
#ifdef CE_CTS_MODE
	else if ((CE_CTL_Crypto_Mode)aes->mode == CE_CTL_CRYPT_MODE_CTS)
		CE_Crypto_GetIV(CE, (uint32_t *)aes->iv, 16);
#endif

	CE_Crypto_Deinit(CE);
	HAL_CE_DisableCCMU();
	HAL_MutexUnlock(&ce_lock);
out:
	ce_running = 0;
	CE_EXIT(ret);
	return ret;
}

/**
  * @brief Encrypt data by DES.
  * @note DES key size is 8 bytes. DES block size is 8 bytes.
  * @param des:
  *        @arg des->mode: CE_CRYPT_MODE_ECB or CE_CRYPT_MODE_CBC
  *        @arg des->iv[8]: initialization value, is same size as block size.
  *                         CE_CRYPT_MODE_CBC mode need this param.
  *        @arg des->src: CE_CTL_KEYSOURCE_INPUT: key is set from des->key.
  *                       CE_CTL_KEYSOURCE_SID: is not support for now.
  *                       CE_CTL_KEYSOURCE_INTERNAL0~7: is not support for now.
  *        @arg des->key[8]: store the key.
  * @param plain: plain data which is need to be encrypt.
  * @param cipher: cipher data which is store the encrpyted data. the cipher data
  *                size is multiple of 8 bytes and no less than plain data size.
  * @param size: size of plain data.
  * @retval HAL_Status:  The status of driver
  */
HAL_Status HAL_DES_Encrypt(CE_DES_Config *des, uint8_t *plain, uint8_t *cipher, uint32_t size)
{
	HAL_Status ret = HAL_OK;
	CE_ENTRY();

	if (size == 0) {
		ret = HAL_INVALID;
		goto out;
	}

	ce_running = 1;
	if ((ret = HAL_MutexLock(&ce_lock, CE_WAIT_TIME)) != HAL_OK)
		goto out;

	HAL_CE_EnableCCMU();

	/* CE config */
	CE_Crypto_Init(CE, CE_CTL_METHOD_DES, des->mode, CE_CRYPT_OP_ENCRYPTION);
	CE_DES_SetKey(CE, des->src, (uint32_t *)des->key);
	if ((CE_CTL_Crypto_Mode)des->mode == CE_CTL_CRYPT_MODE_CBC)
		CE_Crypto_SetIV(CE, (uint32_t *)des->iv, 8);
#ifdef CE_CTR_MODE
	else if ((CE_CTL_Crypto_Mode)des->mode == CE_CTL_CRYPT_MODE_CTR)
		CE_Crypto_SetCounter(CE, (uint32_t *)des->cnt, des->width);
#endif
#ifdef CE_CTS_MODE
	else if ((CE_CTL_Crypto_Mode)des->mode == CE_CTL_CRYPT_MODE_CTS)
		CE_Crypto_SetIV(CE, (uint32_t *)des->iv, 8);
#endif

	/* plain data convey to CE and cipher data convey from CE */
	HAL_Crypto_Convey(plain, cipher, size, DES_BLOCK_SIZE);

	/* CE state storage */
	if ((CE_CTL_Crypto_Mode)des->mode == CE_CTL_CRYPT_MODE_CBC)
		CE_Crypto_GetIV(CE, (uint32_t *)des->iv, 8);
#ifdef CE_CTR_MODE
	else if ((CE_CTL_Crypto_Mode)des->mode == CE_CTL_CRYPT_MODE_CTR)
		CE_Crypto_GetCounter(CE, (uint32_t *)des->cnt, des->width);
#endif
#ifdef CE_CTS_MODE
	else if ((CE_CTL_Crypto_Mode)des->mode == CE_CTL_CRYPT_MODE_CTS)
		CE_Crypto_GetIV(CE, (uint32_t *)des->iv, 8);
#endif

	CE_Crypto_Deinit(CE);
	HAL_CE_DisableCCMU();
	HAL_MutexUnlock(&ce_lock);
out:
	ce_running = 0;
	CE_EXIT(ret);
	return ret;
}

/**
  * @brief Encrypt data by DES.
  * @note DES key size is 8 bytes. DES block size is 8 bytes.
  * @param des:
  *        @arg aes->mode: CE_CRYPT_MODE_ECB or CE_CRYPT_MODE_CBC
  *        @arg aes->iv[8]: initialization value, is same size as block size.
  *                         CE_CRYPT_MODE_CBC mode need this param, and same as
  *                         the encrypt side.
  *        @arg aes->src: CE_CTL_KEYSOURCE_INPUT: key is set from aes->key.
  *                       CE_CTL_KEYSOURCE_SID: is not support for now.
  *                       CE_CTL_KEYSOURCE_INTERNAL0~7: is not support for now.
  *        @arg aes->key[8]: store the key.
  * @param cipher: cipher data which is needed to be decrypted. the cipher data
  *                size must be multiple of 8 bytes.
  * @param plain: plain data which store the decrypted data. the plain data size
  *               will be same as cipher data.
  * @param size: size of cipher data. size must be multiple of 8 bytes.
  * @retval HAL_Status:  The status of driver
  */
HAL_Status HAL_DES_Decrypt(CE_DES_Config *des, uint8_t *cipher, uint8_t *plain, uint32_t size)
{
	HAL_Status ret = HAL_OK;
	CE_ENTRY();

	if (size == 0) {
		ret = HAL_INVALID;
		goto out;
	}

	ce_running = 1;
	if ((ret = HAL_MutexLock(&ce_lock, CE_WAIT_TIME)) != HAL_OK)
		goto out;

	HAL_CE_EnableCCMU();

	/* CE config */
	CE_Crypto_Init(CE, CE_CTL_METHOD_DES, des->mode, CE_CRYPT_OP_DECRYPTION);
	CE_DES_SetKey(CE, des->src, (uint32_t *)des->key);
	if ((CE_CTL_Crypto_Mode)des->mode == CE_CTL_CRYPT_MODE_CBC)
		CE_Crypto_SetIV(CE, (uint32_t *)des->iv, 8);
#ifdef CE_CTR_MODE
	else if ((CE_CTL_Crypto_Mode)des->mode == CE_CTL_CRYPT_MODE_CTR)
		CE_Crypto_SetCounter(CE, (uint32_t *)des->cnt, des->width);
#endif
#ifdef CE_CTS_MODE
	else if ((CE_CTL_Crypto_Mode)des->mode == CE_CTL_CRYPT_MODE_CTS)
		CE_Crypto_SetIV(CE, (uint32_t *)des->iv, 8);
#endif

	/* plain data convey to CE and cipher data convey from CE */
	HAL_Crypto_Convey(cipher, plain, size, DES_BLOCK_SIZE);

	/* CE state storage */
	if ((CE_CTL_Crypto_Mode)des->mode == CE_CTL_CRYPT_MODE_CBC)
		CE_Crypto_GetIV(CE, (uint32_t *)des->iv, 8);
#ifdef CE_CTR_MODE
	else if ((CE_CTL_Crypto_Mode)des->mode == CE_CTL_CRYPT_MODE_CTR)
		CE_Crypto_GetCounter(CE, (uint32_t *)des->cnt, des->width);
#endif
#ifdef CE_CTS_MODE
	else if ((CE_CTL_Crypto_Mode)des->mode == CE_CTL_CRYPT_MODE_CTS)
		CE_Crypto_GetIV(CE, (uint32_t *)des->iv, 8);
#endif

	CE_Crypto_Deinit(CE);
	HAL_CE_DisableCCMU();
	HAL_MutexUnlock(&ce_lock);
out:
	ce_running = 0;
	CE_EXIT(ret);
	return ret;
}

/**
  * @brief Encrypt data by 3DES.
  * @note 3DES key size is 24 bytes, or 3 * 8bytes(8bytes key). 3DES block size is 8 bytes.
  * @param des:
  *        @arg des->mode: CE_CRYPT_MODE_ECB or CE_CRYPT_MODE_CBC
  *        @arg des->iv[8]: initialization value, is same size as block size.
  *                         CE_CRYPT_MODE_CBC mode need this param.
  *        @arg des->src: CE_CTL_KEYSOURCE_INPUT: key is set from des->key.
  *                       CE_CTL_KEYSOURCE_SID: is not support for now.
  *                       CE_CTL_KEYSOURCE_INTERNAL0~7: is not support for now.
  *        @arg des->key[24]: store the key. some 3DES tool key size may be 8 bytes, is 3
  *                           copy of the key.
  * @param plain: plain data which is need to be encrypt.
  * @param cipher: cipher data which is store the encrpyted data. the cipher data
  *                size is multiple of 8 bytes and no less than plain data size.
  * @param size: size of plain data.
  * @retval HAL_Status:  The status of driver
  */
HAL_Status HAL_3DES_Encrypt(CE_3DES_Config *des, uint8_t *plain, uint8_t *cipher, uint32_t size)
{
	HAL_Status ret = HAL_OK;
	CE_ENTRY();

	if (size == 0) {
		ret = HAL_INVALID;
		goto out;
	}

	ce_running = 1;
	if ((ret = HAL_MutexLock(&ce_lock, CE_WAIT_TIME)) != HAL_OK)
		goto out;

	HAL_CE_EnableCCMU();

	/* CE config */
	CE_Crypto_Init(CE, CE_CTL_METHOD_3DES, des->mode, CE_CRYPT_OP_ENCRYPTION);
	CE_3DES_SetKey(CE, des->src, (uint32_t *)des->key);
	if ((CE_CTL_Crypto_Mode)des->mode == CE_CTL_CRYPT_MODE_CBC)
		CE_Crypto_SetIV(CE, (uint32_t *)des->iv, 8);
#ifdef CE_CTR_MODE
	else if ((CE_CTL_Crypto_Mode)des->mode == CE_CTL_CRYPT_MODE_CTR)
		CE_Crypto_SetCounter(CE, (uint32_t *)des->cnt, des->width);
#endif
#ifdef CE_CTS_MODE
	else if ((CE_CTL_Crypto_Mode)des->mode == CE_CTL_CRYPT_MODE_CTS)
		CE_Crypto_SetIV(CE, (uint32_t *)des->iv, 8);
#endif

	/* plain data convey to CE and cipher data convey from CE */
	HAL_Crypto_Convey(plain, cipher, size, DES3_BLOCK_SIZE);

	/* CE state storage */
	if ((CE_CTL_Crypto_Mode)des->mode == CE_CTL_CRYPT_MODE_CBC)
		CE_Crypto_GetIV(CE, (uint32_t *)des->iv, 8);
#ifdef CE_CTR_MODE
	else if ((CE_CTL_Crypto_Mode)des->mode == CE_CTL_CRYPT_MODE_CTR)
		CE_Crypto_GetCounter(CE, (uint32_t *)des->cnt, des->width);
#endif
#ifdef CE_CTS_MODE
	else if ((CE_CTL_Crypto_Mode)des->mode == CE_CTL_CRYPT_MODE_CTS)
		CE_Crypto_GetIV(CE, (uint32_t *)des->iv, 8);
#endif

	CE_Crypto_Deinit(CE);
	HAL_CE_DisableCCMU();
	HAL_MutexUnlock(&ce_lock);
out:
	ce_running = 0;
	CE_EXIT(ret);
	return ret;
}

/**
  * @brief Encrypt data by 3DES.
  * @note 3DES key size is 24 bytes, or 3 * 8bytes(8bytes key). 3DES block size
  *       is 8 bytes.
  * @param des:
  *        @arg aes->mode: CE_CRYPT_MODE_ECB or CE_CRYPT_MODE_CBC
  *        @arg aes->iv[8]: initialization value, is same size as block size.
  *                         CE_CRYPT_MODE_CBC mode need this param, and same as
  *                         the encrypt side.
  *        @arg aes->src: CE_CTL_KEYSOURCE_INPUT: key is set from aes->key.
  *                       CE_CTL_KEYSOURCE_SID: is not support for now.
  *                       CE_CTL_KEYSOURCE_INTERNAL0~7: is not support for now.
  *        @arg des->key[24]: store the key. some 3DES tool key size may be 8
  *                           bytes, is 3 copy of the key.
  * @param cipher: cipher data which is needed to be decrypted. the cipher data
  *                size must be multiple of 8 bytes.
  * @param plain: plain data which store the decrypted data. the plain data size
  *               will be same as cipher data.
  * @param size: size of cipher data. size must be multiple of 8 bytes.
  * @retval HAL_Status:  The status of driver
  */
HAL_Status HAL_3DES_Decrypt(CE_3DES_Config *des, uint8_t *cipher, uint8_t *plain, uint32_t size)
{
	HAL_Status ret = HAL_OK;
	CE_ENTRY();

	if (size == 0) {
		ret = HAL_INVALID;
		goto out;
	}

	ce_running = 1;
	if ((ret = HAL_MutexLock(&ce_lock, CE_WAIT_TIME)) != HAL_OK)
		goto out;

	HAL_CE_EnableCCMU();

	/* CE config */
	CE_Crypto_Init(CE, CE_CTL_METHOD_3DES, des->mode, CE_CRYPT_OP_DECRYPTION);
	CE_3DES_SetKey(CE, des->src, (uint32_t *)des->key);
	if ((CE_CTL_Crypto_Mode)des->mode == CE_CTL_CRYPT_MODE_CBC)
		CE_Crypto_SetIV(CE, (uint32_t *)des->iv, 8);
#ifdef CE_CTR_MODE
	else if ((CE_CTL_Crypto_Mode)des->mode == CE_CTL_CRYPT_MODE_CTR)
		CE_Crypto_SetCounter(CE, (uint32_t *)des->cnt, des->width);
#endif
#ifdef CE_CTS_MODE
	else if ((CE_CTL_Crypto_Mode)des->mode == CE_CTL_CRYPT_MODE_CTS)
		CE_Crypto_SetIV(CE, (uint32_t *)des->iv, 8);
#endif

	/* plain data convey to CE and cipher data convey from CE */
	HAL_Crypto_Convey(cipher, plain, size, 8);

	/* CE state storage */
	if ((CE_CTL_Crypto_Mode)des->mode == CE_CTL_CRYPT_MODE_CBC)
		CE_Crypto_GetIV(CE, (uint32_t *)des->iv, 8);
#ifdef CE_CTR_MODE
	else if ((CE_CTL_Crypto_Mode)des->mode == CE_CTL_CRYPT_MODE_CTR)
		CE_Crypto_GetCounter(CE, (uint32_t *)des->cnt, des->width);
#endif
#ifdef CE_CTS_MODE
	else if ((CE_CTL_Crypto_Mode)des->mode == CE_CTL_CRYPT_MODE_CTS)
		CE_Crypto_GetIV(CE, (uint32_t *)des->iv, 8);
#endif

	CE_Crypto_Deinit(CE);
	HAL_CE_DisableCCMU();
	HAL_MutexUnlock(&ce_lock);
out:
	ce_running = 0;
	CE_EXIT(ret);
	return ret;
}


/************************ CRC MD5 SHA1 SHA256 private **************************************/
typedef struct {
	CE_CTL_CRC_Width width;
	uint32_t poly;
	uint8_t init;
	uint8_t refin;
	uint8_t refout;
	uint8_t xorout;
} CE_CRC_Config;

static const CE_CRC_Config crc_cfg[] = {
	{CE_CTL_CRC_WIDTH_16BITS, 0x8005, 		0, 1, 1, 0},		/* CE_CRC16_IBM,    */
	{CE_CTL_CRC_WIDTH_16BITS, 0x8005, 		0, 1, 1, 1},		/* CE_CRC16_MAXIM,  */
	{CE_CTL_CRC_WIDTH_16BITS, 0x8005, 		1, 1, 1, 1},		/* CE_CRC16_USB,    */
	{CE_CTL_CRC_WIDTH_16BITS, 0x8005, 		1, 1, 1, 0},		/* CE_CRC16_MODBUS, */
	{CE_CTL_CRC_WIDTH_16BITS, 0x1021,		0, 1, 1, 0},		/* CE_CRC16_CCITT,  */
	{CE_CTL_CRC_WIDTH_16BITS, 0x1021, 		1, 0, 0, 0},		/* CE_CRC16_CCITT_1 */
	{CE_CTL_CRC_WIDTH_16BITS, 0x1021, 		1, 1, 1, 1},		/* CE_CRC16_X25,    */
	{CE_CTL_CRC_WIDTH_16BITS, 0x1021, 		0, 0, 0, 0},		/* CE_CRC16_XMODEM, */
	{CE_CTL_CRC_WIDTH_16BITS, 0x3d65, 		0, 1, 1, 1},		/* CE_CRC16_DNP,    */
	{CE_CTL_CRC_WIDTH_32BITS, 0x04c11db7, 	1, 1, 1, 1},		/* CE_CRC32,        */
	{CE_CTL_CRC_WIDTH_32BITS, 0x04c11db7, 	1, 0, 0, 0},		/* CE_CRC32_MPEG2,  */
};

static HAL_Status HAL_CRC_Hash_InitDMA(DMA_Channel *input)
{
	HAL_Status ret = HAL_OK;
	DMA_ChannelInitParam Input_param;
	memset(&Input_param, 0, sizeof(Input_param));
	CE_ENTRY();

	if ((*input = HAL_DMA_Request()) == DMA_CHANNEL_INVALID) {
		CE_ALERT("DMA request failed \n");
		goto out;
	}

	Input_param.cfg = HAL_DMA_MakeChannelInitCfg(DMA_WORK_MODE_SINGLE,
											  DMA_WAIT_CYCLE_16,
											  DMA_BYTE_CNT_MODE_REMAIN,
											  DMA_DATA_WIDTH_32BIT,
											  DMA_BURST_LEN_1,
											  DMA_ADDR_MODE_FIXED,
											  DMA_PERIPH_CE,
											  DMA_DATA_WIDTH_8BIT,
											  DMA_BURST_LEN_4,
											  DMA_ADDR_MODE_INC,
											  DMA_PERIPH_SRAM);
	Input_param.irqType = DMA_IRQ_TYPE_END;
	Input_param.endCallback = HAL_CE_DMACmpl;
	Input_param.endArg = &ce_block;

	HAL_DMA_Init(*input, &Input_param);
	CE_EnableDMA(CE);

out:
	CE_EXIT(ret);
	return ret;
}

static void HAL_CRC_Hash_DenitDMA(DMA_Channel input)
{
	CE_ENTRY();
	CE_DisableDMA(CE);
	HAL_DMA_DeInit(input);
	HAL_DMA_Release(input);
	CE_EXIT(0);
}

static HAL_Status HAL_CRC_Hash_Convey(DMA_Channel input, CE_Fifo_Align *align, uint8_t *data, uint32_t size)
{
	HAL_Status ret = HAL_OK;
	uint32_t align_len;
	uint32_t buf_left = 4 - align->word_size;
	CE_ENTRY();
	CE_DEBUG("input size = %d, buf_left = %d\n", size, buf_left);

	if (size < buf_left) {
		memcpy(align->word, data, size);
		align->word_size += size;
		CE_ASSERT(align->word_size < 4);
		goto out;
	}

	CE_SetInputThreshold(CE, 0);

	memcpy(&align->word[align->word_size], data, buf_left);
	HAL_DMA_Start(input, (uint32_t)align->word, (uint32_t)CE_Crypto_GetInputAddr(CE), 4);
	if ((ret = HAL_SemaphoreWait(&ce_block, CE_WAIT_TIME)) != HAL_OK) {
		CE_ALERT("DMA transfer 1st block failed\n");
		goto out;
	}
	HAL_DMA_Stop(input);

	align->word_size = (size - buf_left) & 0x3; // len % 4
	align_len = size - buf_left - align->word_size;
//	CE_DEBUG("align_len = %d, new buf_left should be = %d\n", align_len, align->word_size);

	if (align_len != 0) {
		HAL_DMA_Start(input, (uint32_t)(data + buf_left), (uint32_t)CE_Crypto_GetInputAddr(CE), align_len);
		if ((ret = HAL_SemaphoreWait(&ce_block, CE_WAIT_TIME)) != HAL_OK) {
			CE_ALERT("DMA transfer 2rd block failed\n");
			goto out;
		}
		HAL_DMA_Stop(input);
	}

//	CE_DEBUG("data copy from %d\n", size - align->word_size);
	memset(align->word, 0, sizeof(align->word));
	memcpy(align->word, data + size - align->word_size, align->word_size);

out:
	CE_EXIT(ret);
	return ret;
}

HAL_Status HAL_MD5_Append(CE_MD5_Handler *hdl, uint8_t *data, uint32_t size);

static void HAL_Hash_Finish(CE_MD5_Handler *hdl, uint64_t bit_size)
{
	uint32_t pad_size;
	uint32_t remain_size = hdl->total_size & 0x3f; // len % 64
	uint32_t total_size = hdl->total_size;
	uint8_t pad[128] = {0};

	CE_ENTRY();
//	CE_DEBUG("total size before padding = %d, remain size = %d.\n", (uint32_t)hdl->total_size, (uint32_t)remain_size);

	if (remain_size < 56)
		pad_size = 64 - remain_size - 8 - 1;
	else
		pad_size = 128 - remain_size - 8 - 1;
	pad[0] = 0x80;
	memcpy(&pad[1 + pad_size], &bit_size, 8);

//	CE_DEBUG("pad size = %d, buf size = %d, len copy to %d.\n", (uint32_t)pad_size, (uint32_t)hdl->word_size, (uint32_t)(1 + pad_size));

	HAL_MD5_Append(hdl, pad, pad_size + 1 + 8);

	CE_ASSERT(((total_size + pad_size + 1 + 8) & 0x3f) == 0);
//	CE_DEBUG("append size = %d, final total size = 0x%x.\n", (uint32_t)(pad_size + 1 + 8), (uint32_t)(total_size + pad_size + 1 + 8));

	CE_Hash_Finish(CE);
	CE_EXIT(0);
}
/************************ public **************************************/

/**
  * @brief Initialize CRC module.
  * @param hdl: It's a handler stored private info. created by user.
  * @param type: CRC algorithm.
  * @param total_size: the total size of data needed to calculate crc.
  * @retval HAL_Status:  The status of driver
  */
HAL_Status HAL_CRC_Init1(CE_CRC_Handler *hdl, CE_CRC_Types type, uint32_t total_size)
{
	HAL_Status ret = HAL_OK;
	const CE_CRC_Config *config = &crc_cfg[type];

	CE_ENTRY();

	if (total_size == 0) {
		ret = HAL_INVALID;
		goto out;
	}

	ce_running = 1;
	if ((ret = HAL_MutexLock(&ce_lock, CE_WAIT_TIME)) != HAL_OK)
		goto out;
	HAL_CE_EnableCCMU();

	hdl->type = type;
	hdl->crc = 0;
	hdl->word_size = 0;
	HAL_Memset(hdl->word, 0, sizeof(hdl->word));

	CE_CRC_Init(CE, config->init,
					config->xorout,
					config->refin,
					config->refout,
					config->width,
					config->poly);
	CE_CRC_SetResult(CE, 0);
	CE_SetLength(CE, total_size);
	CE_Enable(CE);

	CE_REG_ALL(CE);

out:
	CE_EXIT(ret);
	return ret;
}

HAL_Status HAL_CRC_Init(CE_CRC_Handler *hdl, CE_CRC_Types type, uint32_t total_size)
{
	uint32_t crc;

	/* temperarily bug fixed */
	HAL_CRC_Init1(hdl, CE_CRC32, 1);
	HAL_CRC_Append(hdl, (uint8_t *)hdl, 1);
	HAL_CRC_Finish(hdl, &crc);

	HAL_Memset(hdl, 0, sizeof(*hdl));
	return HAL_CRC_Init1(hdl, type, total_size);
}


/**
  * @brief Append data to calculate crc. it can be append several times before
  *        HAL_CRC_Finish.
  * @param hdl: It's a handler stored private info.
  * @param data: data needed to calculate crc.
  * @param size: size of data.
  * @retval HAL_Status:  The status of driver
  */
HAL_Status HAL_CRC_Append(CE_CRC_Handler *hdl, uint8_t *data, uint32_t size)
{
	HAL_Status ret = HAL_OK;
	DMA_Channel input;

	CE_ENTRY();

	if (size == 0) {
		ret = HAL_INVALID;
		goto out;
	}

	if ((ret = HAL_CRC_Hash_InitDMA(&input)) != HAL_OK){
		CE_ALERT("DMA Request failed\n");
		goto out;
	}
	HAL_CRC_Hash_Convey(input, (CE_Fifo_Align *)(&hdl->word[0]), data, size);
	HAL_CRC_Hash_DenitDMA(input);

	hdl->total_size += size;

out:
	CE_EXIT(ret);
	return ret;
}

/**
  * @brief Calculate the crc of all appended data.
  * @param hdl: It's a handler stored private info.
  * @param crc: Store the result of calculation of crc.
  * @retval HAL_Status:  The status of driver
  */
HAL_Status HAL_CRC_Finish(CE_CRC_Handler *hdl, uint32_t *crc)
{
	HAL_Status ret = HAL_OK;
	uint8_t pad[4] = {0};

	CE_ENTRY();

	HAL_CRC_Append(hdl, pad, (4 - hdl->word_size) & 0x03);

	CE_CRC_Finish(CE);
	while (CE_Status(CE, CE_INT_TPYE_HASH_CRC_END) == 0);		// use irq would be better
	CE_CRC_Calc(CE, crc);
	CE_REG_ALL(CE);
	CE_REG(CE->CRC_RESULT);

	CE_Disable(CE);
	CE_CRC_Deinit(CE);
	HAL_CE_DisableCCMU();
	HAL_MutexUnlock(&ce_lock);

	ce_running = 0;
	CE_EXIT(ret);
	return ret;
}

/**
  * @brief Initialize MD5 module.
  * @param hdl: It's a handler stored private info. created by user.
  * @param src: Selected IV source
  *        @arg CE_CTL_IVMODE_SHA_MD5_FIPS180: use FIPS180 IV.
  *        @arg CE_CTL_IVMODE_SHA_MD5_INPUT: IV from param iv[4].
  * @param iv: MD5 Initialization Value. size of iv must be 16 bytes.
  * @retval HAL_Status:  The status of driver
  */
HAL_Status HAL_MD5_Init(CE_MD5_Handler *hdl, CE_Hash_IVsrc src, uint32_t iv[4])
{
	HAL_Status ret = HAL_OK;

	CE_ENTRY();

	ce_running = 1;
	if ((ret = HAL_MutexLock(&ce_lock, CE_WAIT_TIME)) != HAL_OK)
		goto out;

	HAL_CE_EnableCCMU();
	CE_Hash_Init(CE, CE_CTL_METHOD_MD5);
	CE_Hash_SetIV(CE, src, iv, CE_MD5_IV_SIZE);

	hdl->total_size = 0;
	hdl->word_size = 0;
	memset(hdl->word, 0, sizeof(hdl->word));

	CE_REG_ALL(CE);

out:
	CE_EXIT(ret);
	return ret;
}

/**
  * @brief Append MD5 data to calculate.
  * @param hdl: It's a handler stored private info. created by user.
  * @param data: the data needed to calculate md5.
  * @param size: size of data.
  * @retval HAL_Status:  The status of driver
  */
HAL_Status HAL_MD5_Append(CE_MD5_Handler *hdl, uint8_t *data, uint32_t size)
{
	HAL_Status ret = HAL_OK;
	DMA_Channel input;

	CE_ENTRY();

	if (size == 0) {
		ret = HAL_INVALID;
		goto out;
	}

	if ((ret = HAL_CRC_Hash_InitDMA(&input)) != HAL_OK){
		CE_ALERT("DMA Request failed\n");
		goto out;
	}
	HAL_CRC_Hash_Convey(input, (CE_Fifo_Align *)(&hdl->word[0]), data, size);
	HAL_CRC_Hash_DenitDMA(input);

	hdl->total_size += size;

out:
	CE_EXIT(ret);
	return ret;
}

/**
  * @brief Calculate the MD5 of all appended data.
  * @param hdl: It's a handler stored private info.
  * @param digest: Store the result of calculation of MD5, result
  *                need 16 bytes space.
  * @retval HAL_Status:  The status of driver
  */
HAL_Status HAL_MD5_Finish(CE_MD5_Handler *hdl, uint32_t digest[4])
{
	HAL_Status ret = HAL_OK;
	uint64_t bit_size = htole64(hdl->total_size << 3);

	CE_ENTRY();

	HAL_Hash_Finish(hdl, bit_size);
	while (CE_Status(CE, CE_INT_TPYE_HASH_CRC_END) == 0);		// use irq would be better
	CE_REG_ALL(CE);
	CE_Hash_Calc(CE, CE_CTL_METHOD_MD5, digest);

	CE_Hash_Deinit(CE);
	HAL_CE_DisableCCMU();
	HAL_MutexUnlock(&ce_lock);

	ce_running = 0;
	CE_EXIT(ret);
	return ret;
}

/**
  * @brief Initialize SHA1 module.
  * @param hdl: It's a handler stored private info. created by user.
  * @param src: Selected IV source
  *        @arg CE_CTL_IVMODE_SHA_MD5_FIPS180: use FIPS180 IV.
  *        @arg CE_CTL_IVMODE_SHA_MD5_INPUT: IV from param iv[5].
  * @param iv: SHA1 Initialization Value. size of iv must be 20 bytes.
  * @retval HAL_Status:  The status of driver
  */
HAL_Status HAL_SHA1_Init(CE_SHA1_Handler *hdl, CE_Hash_IVsrc src, uint32_t iv[5])
{
	HAL_Status ret = HAL_OK;

	CE_ENTRY();

	ce_running = 1;
	if ((ret = HAL_MutexLock(&ce_lock, CE_WAIT_TIME)) != HAL_OK)
		goto out;

	HAL_CE_EnableCCMU();
	CE_Hash_Init(CE, CE_CTL_METHOD_SHA1);
	CE_Hash_SetIV(CE, src, iv, CE_SHA1_IV_SIZE);
	CE_REG_ALL(CE);

	hdl->total_size = 0;
	hdl->word_size = 0;
	memset(hdl->word, 0, sizeof(hdl->word));

out:
	CE_EXIT(ret);
	return ret;
}

/**
  * @brief Append SHA1 data to calculate.
  * @param hdl: It's a handler stored private info. created by user.
  * @param data: the data needed to calculate sha1.
  * @param size: size of data.
  * @retval HAL_Status:  The status of driver
  */
HAL_Status HAL_SHA1_Append(CE_SHA1_Handler *hdl, uint8_t *data, uint32_t size)
{
	HAL_Status ret;
	CE_ENTRY();

	ret = HAL_MD5_Append(hdl, data, size);

	CE_EXIT(ret);
	return ret;
}

/**
  * @brief Calculate the SHA1 of all appended data.
  * @param hdl: It's a handler stored private info.
  * @param digest: Store the result of calculation of SHA1, result
  *                need 20 bytes space.
  * @retval HAL_Status:  The status of driver
  */
HAL_Status HAL_SHA1_Finish(CE_SHA1_Handler *hdl, uint32_t digest[5])
{
	HAL_Status ret = HAL_OK;
	uint64_t bit_size = htobe64(hdl->total_size << 3);
	CE_ENTRY();

	HAL_Hash_Finish(hdl, bit_size);
	while (CE_Status(CE, CE_INT_TPYE_HASH_CRC_END) == 0);		// use irq
	CE_REG_ALL(CE);
	CE_Hash_Calc(CE, CE_CTL_METHOD_SHA1, digest);

	CE_Hash_Deinit(CE);
	HAL_CE_DisableCCMU();
	HAL_MutexUnlock(&ce_lock);

	ce_running = 0;
	CE_EXIT(ret);
	return ret;
}

/**
  * @brief Initialize SHA256 module.
  * @param hdl: It's a handler stored private info. created by user.
  * @param src: Selected IV source
  *        @arg CE_CTL_IVMODE_SHA_MD5_FIPS180: use FIPS180 IV.
  *        @arg CE_CTL_IVMODE_SHA_MD5_INPUT: IV from param iv[8].
  * @param iv: SHA256 Initialization Value. size of iv must be 32 bytes.
  * @retval HAL_Status:  The status of driver
  */
HAL_Status HAL_SHA256_Init(CE_SHA256_Handler *hdl, CE_Hash_IVsrc src, uint32_t iv[8])
{
	HAL_Status ret = HAL_OK;

	CE_ENTRY();

	ce_running = 1;
	if ((ret = HAL_MutexLock(&ce_lock, CE_WAIT_TIME)) != HAL_OK)
		goto out;

	HAL_CE_EnableCCMU();
	CE_Hash_Init(CE, CE_CTL_METHOD_SHA256);
	CE_Hash_SetIV(CE, src, iv, CE_SHA256_IV_SIZE);
	CE_REG_ALL(CE);

	hdl->total_size = 0;
	hdl->word_size = 0;
	memset(hdl->word, 0, sizeof(hdl->word));

out:
	CE_EXIT(ret);
	return ret;
}

/**
  * @brief Append SHA256 data to calculate.
  * @param hdl: It's a handler stored private info. created by user.
  * @param data: the data needed to calculate sha256.
  * @param size: size of data.
  * @retval HAL_Status:  The status of driver
  */
HAL_Status HAL_SHA256_Append(CE_SHA256_Handler *hdl, uint8_t *data, uint32_t size)
{
	HAL_Status ret;
	CE_ENTRY();

	ret = HAL_MD5_Append(hdl, data, size);

	CE_EXIT(ret);
	return ret;
}

/**
  * @brief Calculate the SHA256 of all appended data.
  * @param hdl: It's a handler stored private info.
  * @param digest: Store the result of calculation of SHA256, result
  *                need 32 bytes space.
  * @retval HAL_Status:  The status of driver
  */
HAL_Status HAL_SHA256_Finish(CE_SHA256_Handler *hdl, uint32_t digest[8])
{
	HAL_Status ret = HAL_OK;
	uint64_t bit_size = htobe64(hdl->total_size << 3);

	CE_ENTRY();

	HAL_Hash_Finish(hdl, bit_size);
	while (CE_Status(CE, CE_INT_TPYE_HASH_CRC_END) == 0);		// use irq would be better
	CE_REG_ALL(CE);
	CE_Hash_Calc(CE, CE_CTL_METHOD_SHA256, digest);

	CE_Hash_Deinit(CE);
	HAL_CE_DisableCCMU();
	HAL_MutexUnlock(&ce_lock);

	ce_running = 0;
	CE_EXIT(ret);
	return ret;
}

/**
  * @brief Generate some random numbers.
  * @param random: a buffer to store random number, create by user.
  * @param size: size of random number to generate.
  * @retval HAL_Status:  The status of driver
  */
HAL_Status HAL_PRNG_Generate(uint8_t *random, uint32_t size)
{
		HAL_Status ret = HAL_OK;
		uint32_t seed[6];
		uint32_t rand[5];
		uint8_t *p = random;
		uint32_t cpsize;

		CE_ENTRY();

		if (size == 0) {
			ret = HAL_INVALID;
			goto out;
		}

		ce_running = 1;
		if ((ret = HAL_MutexLock(&ce_lock, CE_WAIT_TIME)) != HAL_OK)
			goto out;
		HAL_CE_EnableCCMU();

		while (1) {

		seed[0] = HAL_Ticks();
		seed[1] = HAL_Ticks();
		seed[2] = HAL_Ticks();
		seed[3] = HAL_Ticks();
		seed[4] = HAL_Ticks();
		seed[5] = HAL_Ticks();

		CE_PRNG_Init(CE, 0);
		CE_PRNG_Seed(CE, seed);
		CE_REG_ALL(CE);
		CE_PRNG_Generate(CE, rand);
		cpsize = (size > 20) ? 20 : size;
		memcpy(p, (uint8_t *)rand, cpsize);
		if (size <= 20)
			break;
		p += cpsize;
		size -= cpsize;
		}

		CE_REG_ALL(CE);
		CE_PRNG_Deinit(CE);

		HAL_CE_DisableCCMU();
		HAL_MutexUnlock(&ce_lock);

	out:
		ce_running = 0;
		CE_EXIT(ret);
		return ret;

}
/*
HAL_Status HAL_PRNG_Generate(uint32_t random[5])
{
	HAL_Status ret = HAL_OK;
	uint32_t seed[6];
	uint32_t tick;

	CE_ENTRY();

	if ((ret = HAL_MutexLock(&ce_lock, CE_WAIT_TIME)) != HAL_OK)
		goto out;
	HAL_CE_EnableCCMU();

	tick = HAL_Ticks();

	seed[0] = tick;
	seed[2] = seed[2] ^ HAL_Ticks();
	seed[4] = seed[0] ^ seed[1] ^ seed[2] ^ seed[3] ^ HAL_Ticks();
	seed[5] = seed[0] ^ seed[4] ^ seed[2] ^ seed[3] ^ HAL_Ticks();

	CE_PRNG_Init(CE, 0);
	CE_PRNG_Seed(CE, seed);
	CE_REG_ALL(CE);
	CE_PRNG_Generate(CE, random);

	CE_REG_ALL(CE);
	CE_PRNG_Deinit(CE);

	HAL_CE_DisableCCMU();
	HAL_MutexUnlock(&ce_lock);

out:
	CE_EXIT(ret);
	return ret;
}
*/


