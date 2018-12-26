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

#include "sys/xr_debug.h"
#include "driver/chip/ir_nec.h"
#include "hal_base.h"

#define HAL_DBG_IRNEC  0

#if (HAL_DBG_IRNEC == 1)
#define IRNEC_DBG(fmt, arg...) HAL_LOG(HAL_DBG_ON && HAL_DBG_IRNEC, "[IRNEC] "fmt, ##arg)
#define irnec_hex_dump_bytes(...) print_hex_dump_bytes(__VA_ARGS__)
#define IRNEC_INF IRNEC_DBG
#define IRNEC_WRN HAL_WRN
#define IRNEC_ERR HAL_ERR
#else
#define irnec_hex_dump_bytes(...)
#define IRNEC_INF(...)
#define IRNEC_WRN(...)
#define IRNEC_ERR(...)
#endif

/**
 * @brief DePacket code by NEC protocal.
 * @param buf:
 *        @arg buf->[in]Raw code buffer.
 * @param dcnt:
 *        @arg dcnt->[in] Num of Raw code.
 * @retval  DePacket code if success or 0xffffffff if depacket failed.
 */
uint32_t IRRX_NECPacket_DeCode(uint8_t *buf, uint32_t dcnt)
{
	uint32_t len;
	uint8_t val = 0, last;
	uint32_t code = 0;
	int32_t bitCnt = 0;
	uint32_t i = 0;
	uint32_t active_delay = 0;
	uint32_t irrx_l1_min = 0, irrx_l0_min = 0, irrx_pmax = 0, irrx_dmid = 0, irrx_dmax = 0;
#if !defined (IR_CLK_32K_USED)
	uint32_t clk = HAL_GetHFClock();
#endif

	irnec_hex_dump_bytes(buf, dcnt);

	IRNEC_INF("dcnt = %d\n", dcnt);

	/* Find Lead '1' */
#if defined (IR_CLK_32K_USED)
	active_delay = (IRRX_32K_ACTIVE_T + 1) * (IRRX_32K_ACTIVE_T_C ? 128 : 1);
	irrx_l1_min = IRRX_32K_L1_MIN;
	irrx_l0_min = IRRX_32K_L0_MIN;
	irrx_pmax = IRRX_32K_PMAX;
	irrx_dmid = IRRX_32K_DMID;
	irrx_dmax = IRRX_32K_DMAX;
#else
	if (clk == HOSC_CLOCK_26M) {
		active_delay = (IRRX_26M_ACTIVE_T + 1) * (IRRX_26M_ACTIVE_T_C ? 128 : 1);
		irrx_l1_min = IRRX_26M_L1_MIN;
		irrx_l0_min = IRRX_26M_L0_MIN;
		irrx_pmax = IRRX_26M_PMAX;
		irrx_dmid = IRRX_26M_DMID;
		irrx_dmax = IRRX_26M_DMAX;
	} else if (clk == HOSC_CLOCK_24M) {
		active_delay = (IRRX_24M_ACTIVE_T + 1) * (IRRX_24M_ACTIVE_T_C ? 128 : 1);
		irrx_l1_min = IRRX_24M_L1_MIN;
		irrx_l0_min = IRRX_24M_L0_MIN;
		irrx_pmax = IRRX_24M_PMAX;
		irrx_dmid = IRRX_24M_DMID;
		irrx_dmax = IRRX_24M_DMAX;
	}
#endif
	//IRNEC_INF("active_delay = %d\n", active_delay);
	len = 0;
	len += (active_delay >> 1);
	for (i = 0; i < dcnt; i++) {
		val = buf[i];
		if (val & 0x80) {
			len += val & 0x7f;
		} else {
			if (len > irrx_l1_min)
				break;
			len = 0;
		}
	}

	//IRNEC_INF("'1' len = %d\n", len);

	if ((val & 0x80) || (len <= irrx_l1_min)){
		IRNEC_INF("start 1 error code %d\n", len);
		goto error_code;
	}

	/* Find Lead '0' */
	len = 0;
	for (; i < dcnt; i++) {
		val = buf[i];
		if (val & 0x80) {
			if (len > irrx_l0_min)
				break;
			len = 0;
		} else {
			len += val & 0x7f;
		}
	}

	//IRNEC_INF("'0' len = %d\n", len);

	if ((!(val & 0x80)) || (len <= irrx_l0_min)){
		IRNEC_INF("start 0 error code %d\n", len);
		goto error_code;
	}

	/* go decoding */
	code = 0; /* 0 for Repeat Code */
	bitCnt = 0;
	last = 1;
	len = 0;
	for (; i < dcnt; i++) {
		val = buf[i];
		if (last) {
			if (val & 0x80) {
				len += val & 0x7f;
			} else {
				if (len > irrx_pmax) { /* Error Pulse */
					IRNEC_INF("len:%d > IRRX_PMAX @%d\n", len, i);
					goto error_code;
				}
				last = 0;
				len = val & 0x7f;
			}
		} else {
			if (val & 0x80) {
				if (len > irrx_dmax){ /* Error Distant */
					IRNEC_INF("len:%d > IRRX_DMAX @%d\n", len, i);
					goto error_code;
				} else {
					if (len > irrx_dmid)  {
						/* data '1'*/
						code |= 1 << bitCnt;
					}
					bitCnt++;
					if (bitCnt == 32)
						break; /* decode over */
				}
				last = 1;
				len = val & 0x7f;
			} else {
				len += val & 0x7f;
			}
		}
	}
	return code;

error_code:
	irnec_hex_dump_bytes(buf, dcnt);

	return (uint32_t)-1;
}

int32_t IRRX_NECCode_Valid(uint32_t code)
{
	uint32_t tmp1, tmp2;

#ifdef IRRX_CHECK_ADDR_CODE
	/* Check Address Value */
	if ((code & 0xffff) != (IRRX_ADDR_CODE & 0xffff))
		return 0; /* Address Error */

	tmp1 = code & 0x00ff0000;
	tmp2 = (code & 0xff000000) >> 8;

	return ((tmp1 ^ tmp2) == 0x00ff0000); /* Check User Code */
#else
	/* Do Not Check Address Value */
	tmp1 = code & 0x00ff00ff;
	tmp2 = (code & 0xff00ff00) >> 8;

	return (((tmp1 ^ tmp2) & 0x00ff0000) == 0x00ff0000);
#endif
}

/**
 * @brief Packet code by NEC protocal.
 * @param txBuff:
 *        @arg txBuff->[out] Raw code will put in.
 * @param ir_tx_code:
 *        @arg ir_tx_code->[in] The code will be Packeted.
 * @retval  Raw code num.
 */
uint32_t IRTX_NECPacket_Code(uint8_t *txBuff, uint32_t ir_tx_code)
{
	uint32_t i, j;
	uint32_t txCnt = 0;
	uint8_t tx_code[4];
#if !defined (IR_CLK_32K_USED)
	uint32_t clk = HAL_GetHFClock();
#endif
	uint32_t irtx_560us_num = 0, irtx_1680us_num = 0;

	tx_code[0] = (uint8_t)(ir_tx_code >> 24);
	tx_code[1] = (uint8_t)(ir_tx_code >> 16);
	tx_code[2] = (uint8_t)(ir_tx_code >> 8);
	tx_code[3] = (uint8_t)(ir_tx_code & 0xff);

	/* go encoding */
#if defined (IR_CLK_32K_USED)
	txBuff[txCnt++] = 0x80|IRTX_32K_9MS_NUM1;  /* S1:9ms */
	txBuff[txCnt++] = 0x80|IRTX_32K_9MS_NUM2;
	txBuff[txCnt++] = IRTX_32K_4P5MS_NUM1;     /* S0:4.5ms */
	irtx_560us_num = IRTX_32K_560US_NUM;
	irtx_1680us_num = IRTX_32K_1680US_NUM;
#else
	if (clk == HOSC_CLOCK_26M) {
		txBuff[txCnt++] = 0x80|IRTX_26M_9MS_NUM1;  /* S1:9ms */
		txBuff[txCnt++] = 0x80|IRTX_26M_9MS_NUM2;
		txBuff[txCnt++] = IRTX_26M_4P5MS_NUM1;     /* S0:4.5ms */
		irtx_560us_num = IRTX_26M_560US_NUM;
		irtx_1680us_num = IRTX_26M_1680US_NUM;
	} else if (clk == HOSC_CLOCK_24M) {
		txBuff[txCnt++] = 0x80|IRTX_24M_9MS_NUM1;  /* S1:9ms */
		txBuff[txCnt++] = 0x80|IRTX_24M_9MS_NUM2;
		txBuff[txCnt++] = IRTX_24M_4P5MS_NUM1;     /* S0:4.5ms */
		irtx_560us_num = IRTX_24M_560US_NUM;
		irtx_1680us_num = IRTX_24M_1680US_NUM;
	}
#endif

	for (j = 0; j < 4; j++) {
		for (i = 0; i < 8; i++ ) {
			if (tx_code[j] & 0x01) {
				/* L1:560uS, 1680uS */
				txBuff[txCnt++] = 0x80|irtx_560us_num;
				txBuff[txCnt++] = irtx_1680us_num;
			} else {
				/* L0:560uS, 560uS */
				txBuff[txCnt++] = 0x80|irtx_560us_num;
				txBuff[txCnt++] = irtx_560us_num;
			}
			tx_code[j] = tx_code[j] >> 1;
		}
	}
	txBuff[txCnt++] = 0x80|irtx_560us_num;
	txBuff[txCnt++] = irtx_560us_num; /* add for set gpio to 0 */
	txBuff[txCnt++] = 0x7f;
	txBuff[txCnt++] = 0x7f;
	txBuff[txCnt++] = 0x7f;
	txBuff[txCnt++] = 0x7f;

	IRNEC_INF("%s: tx_dcnt = %d\n", __func__, txCnt);
	irnec_hex_dump_bytes(txBuff, txCnt);

	return txCnt;
}

#undef IRNEC_INF
#undef IRNEC_WRN
#undef IRNEC_ERR
