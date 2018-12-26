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

#ifndef _DRIVER_CHIP_IR_NEC_H_
#define _DRIVER_CHIP_IR_NEC_H_

#include "driver/chip/hal_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * NEC protocal
 *  8 bits addr, 8 bits cmd code.
 *  send addr and cmd code twice every time to improve reliable.
 *  pluse modulation time is long or short.
 *  38KHz carrier frequency.
 *  bit time is 1.12ms or 2.25ms.
 *  send time is 68ms, send cycle is 110ms.
 * ------------------------------------------------------------------------------
 * |         Logical '1'          |      Logical '0'      |     Logical 'x'     |
 * ------------------------------------------------------------------------------
 * | 1(pulses) |         0        | 1(pulses) |     0     | 1(pulses) |    -    |
 * ------------------------------------------------------------------------------
 * |   560uS   |      1690uS      |   560uS   |   560uS   |   560uS   |         |
 * |           2.25mS             |         1.12mS        |           |         |
 * ------------------------------------------------------------------------------
 * 1(pulses): f=38KHz, T=26.3us, N=560/26.3=21
 *
 * example: send 0x59(10011010 LSB first) addr, 0x16(01101000 LSB first) cmd code.
 * ---------------------------------------------------------------------------------------------------------------------
 * |  S1   | S0  |L1|L0|L0|L1|L1|L0|L1|L0|L0|L1|L1|L0|L0|L1|L0|L1|L0|L1|L1|L0|L1|L0|L0|L0|L1|L0|L0|L1|L0|L1|L1|L1| E1  |
 * |  9mS  |4.5mS|LSB                 MSB|LSB                 MSB|LSB                 MSB|LSB                 MSB|560uS|
 * |    Start    |     0x59 Address      |     ~0x59 Address     |     0x16 Command      |    ~0x16 Command      | End |
 * ---------------------------------------------------------------------------------------------------------------------
 * the S0 of repeat code is 2.25mS.
 */

#if defined (IR_CLK_32K_USED)
#define IRRX_32K_ACTIVE_T       (0)             /* ActionTreshold, Active Threshold */
#define IRRX_32K_ACTIVE_T_C     (0)             /* SampleClock, Active Threshold Control, same with IRRX_ATHC_CLOCKUNIT_1 */
#define IRRX_32K_L1_MIN         (112)           /* 112*30.5 = ~3.4ms, Lead1(9.0ms) Lead1R(4.5ms) > IR_L1_MIN */
#define IRRX_32K_L0_MIN         (56)            /* 56*30.5 = ~1.7ms, Lead0(4.5ms) Lead0R(2.25ms) > IR_L0_MIN */
#define IRRX_32K_PMAX           (36)            /* 36*30.5 = ~1098us ~= 549*2, Pluse < IRRX_PMAX */
#define IRRX_32K_DMID           (36)            /* 36*30.5 = ~1098us ~= 549*2, D1 > IRRX_DMID, D0 =< IRRX_DMID */
#define IRRX_32K_DMAX           (74)            /* 74*30.5 = ~2257us ~= 564*4, D < IRRX_DMAX */

#define IRTX_32K_TS_US          (61)            /* Ts frome IRTX_SAMPLE */
                                                /* IRTX_9MS_NUM 0x94, 148*Ts=9028uS */
                                                /* IRTX_9MS_NUM1 127*Ts=7747uS */
                                                /* IRTX_9MS_NUM2 21*Ts=1281uS */
                                                /* IRTX_4P5MS_NUM 0x4A, 74*Ts=4514uS */
                                                /* IRTX_560US_NUM 0x09, 9*Ts=549uS */
                                                /* IRTX_1680US_NUM 0x1C, 28*Ts=1708uS */
#define IRTX_32K_9MS_NUM        (9000/IRTX_32K_TS_US)
#define IRTX_32K_9MS_NUM1       (0x7f)
#define IRTX_32K_9MS_NUM2       (IRTX_32K_9MS_NUM-IRTX_32K_9MS_NUM1)
#define IRTX_32K_4P5MS_NUM      (4500/IRTX_32K_TS_US)
#define IRTX_32K_4P5MS_NUM1     (IRTX_32K_4P5MS_NUM)
#define IRTX_32K_2P25MS_NUM     (IRTX_32K_4P5MS_NUM/2)
#define IRTX_32K_560US_NUM      (560/IRTX_32K_TS_US)
#define IRTX_32K_1680US_NUM     (1680/IRTX_32K_TS_US)

#define IRTX_32K_NEC_CYCLE_MS   (110)
#define IRTX_32K_NEC_IDC_VALUE  ((IRTX_32K_NEC_CYCLE_MS*1000)/128/IRTX_32K_TS_US + 1)
#else
#define IRRX_26M_ACTIVE_T       (0)             /* ActionTreshold, Active Threshold */
#define IRRX_26M_ACTIVE_T_C     (1)             /* SampleClock, Active Threshold Control, same with IRRX_ATHC_CLOCKUNIT_128 */
#define IRRX_26M_L1_MIN         (80)            /* 80*39.4 = ~3.2ms, Lead1(9.0ms) Lead1R(4.5ms) > IR_L1_MIN */
#define IRRX_26M_L0_MIN         (41)            /* 41*39.4 = ~1.6ms, Lead0(4.5ms) Lead0R(2.25ms)> IR_L0_MIN */
#define IRRX_26M_PMAX           (27)            /* 27*39.4 = ~1064us ~= 532*2, Pluse < IRRX_PMAX */
#define IRRX_26M_DMID           (27)            /* 27*39.4 = ~1064us ~= 532*2, D1 > IRRX_DMID, D0 =< IRRX_DMID */
#define IRRX_26M_DMAX           (55)            /* 55*39.4 = ~2167us ~= 542*4, D < IRRX_DMAX */

#define IRTX_26M_TS_US          (39)            /* Ts frome IRTX_SAMPLE */
                                                /* IRTX_9MS_NUM 0xE4, 228*Ts=8994.6uS */
                                                /* IRTX_9MS_NUM1 127*Ts=5003.8uS */
                                                /* IRTX_9MS_NUM2 101*Ts=3979.4uS */
                                                /* IRTX_4P5MS_NUM 0x72, 114*Ts=4491.6uS */
                                                /* IRTX_560US_NUM 0x0E, 14*Ts=551.6uS */
                                                /* IRTX_1680US_NUM 0x2B, 43*Ts=1694.2uS */
#define IRTX_26M_9MS_NUM        (9000/IRTX_26M_TS_US)
#define IRTX_26M_9MS_NUM1       (0x7f)
#define IRTX_26M_9MS_NUM2       (IRTX_26M_9MS_NUM-IRTX_26M_9MS_NUM1)
#define IRTX_26M_4P5MS_NUM      (4500/IRTX_26M_TS_US)
#define IRTX_26M_4P5MS_NUM1     (IRTX_26M_4P5MS_NUM)
#define IRTX_26M_2P25MS_NUM     (IRTX_26M_4P5MS_NUM/2)
#define IRTX_26M_560US_NUM      (560/IRTX_26M_TS_US)
#define IRTX_26M_1680US_NUM     (1680/IRTX_26M_TS_US)

#define IRTX_26M_NEC_CYCLE_MS   (110)
#define IRTX_26M_NEC_IDC_VALUE  ((IRTX_26M_NEC_CYCLE_MS*1000)/128/IRTX_26M_TS_US + 1)

#define IRRX_24M_ACTIVE_T       (0)             /* ActionTreshold, Active Threshold */
#define IRRX_24M_ACTIVE_T_C     (1)             /* SampleClock, Active Threshold Control, same with IRRX_ATHC_CLOCKUNIT_128 */
#define IRRX_24M_L1_MIN         (70)            /* 70*42.7 = ~3.0ms, Lead1(9.0ms) Lead1R(4.5ms) > IR_L1_MIN */
#define IRRX_24M_L0_MIN         (40)            /* 40*42.7 = ~1.7ms, Lead0(4.5ms) Lead0R(2.25ms)> IR_L0_MIN */
#define IRRX_24M_PMAX           (26)            /* 26*42.7 = ~1109us ~= 561*2, Pluse < IRRX_PMAX */
#define IRRX_24M_DMID           (26)            /* 26*42.7 = ~1109us ~= 561*2, D1 > IRRX_DMID, D0 =< IRRX_DMID */
#define IRRX_24M_DMAX           (53)            /* 53*42.7 = ~2263us ~= 561*4, D < IRRX_DMAX */

#define IRTX_24M_TS_US          (43)            /* Ts frome IRTX_SAMPLE */
                                                /* IRTX_9MS_NUM 0xD1, 209*Ts=8924.3uS */
                                                /* IRTX_9MS_NUM1 127*Ts=5422.9uS */
                                                /* IRTX_9MS_NUM2 81*Ts=3458.7uS */
                                                /* IRTX_4P5MS_NUM 0x69, 105*Ts=4483.5uS */
                                                /* IRTX_560US_NUM 0x0D, 13*Ts=555.1uS */
                                                /* IRTX_1680US_NUM 0x27, 39*Ts=1665.3uS */

#define IRTX_24M_9MS_NUM        (9000/IRTX_24M_TS_US)
#define IRTX_24M_9MS_NUM1       (0x7f)
#define IRTX_24M_9MS_NUM2       (IRTX_24M_9MS_NUM-IRTX_24M_9MS_NUM1)
#define IRTX_24M_4P5MS_NUM      (4500/IRTX_24M_TS_US)
#define IRTX_24M_4P5MS_NUM1     (IRTX_24M_4P5MS_NUM)
#define IRTX_24M_2P25MS_NUM     (IRTX_24M_4P5MS_NUM/2)
#define IRTX_24M_560US_NUM      (560/IRTX_24M_TS_US)
#define IRTX_24M_1680US_NUM     (1680/IRTX_24M_TS_US)

#define IRTX_24M_NEC_CYCLE_MS   (110)
#define IRTX_24M_NEC_IDC_VALUE  ((IRTX_24M_NEC_CYCLE_MS*1000)/128/IRTX_24M_TS_US + 1)
#endif

/**
 * @brief Format code by NEC protocal.
 * @param add:
 *        @arg add->[in] The address.
 * @param key:
 *        @arg key->[in] The key.
 * @retval  Formated code.
 */
static __inline uint32_t IR_NEC_CODE(uint8_t add, uint8_t key)
{
	uint32_t code = (add << 24) | (((~add) << 16) & 0x00ff0000);

	code |= (key << 8) | ((~key) & 0x00ff);

	return code;
}

/**
 * @brief Check code valied.
 * @param code:
 *        @arg code->[in] The code wanted be checked.
 * @retval  1 if valid or 0 if unvalid.
 */
extern int32_t IRRX_NECCode_Valid(uint32_t code);

/**
 * @brief DePacket code by NEC protocal.
 * @param buf:
 *        @arg buf->[in]Raw code buffer.
 * @param dcnt:
 *        @arg dcnt->[in] Num of Raw code.
 * @retval  DePacket code if success or 0xffffffff if depacket failed.
 */
extern uint32_t IRRX_NECPacket_DeCode(uint8_t *buf, uint32_t dcnt);

/**
 * @brief Packet code by NEC protocal.
 * @param txBuff:
 *        @arg txBuff->[out] Raw code will put in.
 * @param ir_tx_code:
 *        @arg ir_tx_code->[in] The code will be Packeted.
 * @retval  Raw code num.
 */
extern uint32_t IRTX_NECPacket_Code(uint8_t *txBuff, uint32_t ir_tx_code);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_CHIP_IR_NEC_H_ */
