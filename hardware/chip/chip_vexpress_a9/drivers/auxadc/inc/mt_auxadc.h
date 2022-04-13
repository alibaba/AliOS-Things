/*
 * Copyright (c) 2019 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _MT_AUXADC_H
#define _MT_AUXADC_H
#include <stdint.h>
#include <mt_reg_base.h>
#include <stdio.h>

#define AUXADC_SET_CLOCK        (INFRACFG_AO_BASE + 0x88)
#define AUXADC_CLR_CLOCK        (INFRACFG_AO_BASE + 0x8C)
#define AUXADC_STA_CLOCK        (INFRACFG_AO_BASE + 0x94)
#define AUXADC_CON0             (AUXADC_BASE + 0x000)
#define AUXADC_CON1             (AUXADC_BASE + 0x004)
#define AUXADC_CON1_SET         (AUXADC_BASE + 0x008)
#define AUXADC_CON1_CLR         (AUXADC_BASE + 0x00C)
#define AUXADC_CON2             (AUXADC_BASE + 0x010)
#define AUXADC_DAT0             (AUXADC_BASE + 0x014)
#define AUXADC_TP_CMD           (AUXADC_BASE + 0x005c)
#define AUXADC_TP_ADDR          (AUXADC_BASE + 0x0060)
#define AUXADC_TP_CON0          (AUXADC_BASE + 0x0064)
#define AUXADC_TP_DATA0         (AUXADC_BASE + 0x0074)
#define AUXADC_DET_VOLT         (AUXADC_BASE + 0x084)
#define AUXADC_DET_SEL          (AUXADC_BASE + 0x088)
#define AUXADC_DET_PERIOD       (AUXADC_BASE + 0x08C)
#define AUXADC_DET_DEBT         (AUXADC_BASE + 0x090)
#define AUXADC_MISC             (AUXADC_BASE + 0x094)
#define AUXADC_ECC              (AUXADC_BASE + 0x098)
#define AUXADC_SAMPLE_LIST      (AUXADC_BASE + 0x09c)
#define AUXADC_ABIST_PERIOD     (AUXADC_BASE + 0x0A0)
extern  int32_t auxadc_test(void);
extern int32_t IMM_GetOneChannelValue(int32_t dwChannel,
	int32_t data[4], int32_t* rawdata);
extern int32_t IMM_GetOneChannelValue_Cali(int32_t Channel,
	int32_t* voltage);
#endif   /*_MT_AUXADC_H*/
