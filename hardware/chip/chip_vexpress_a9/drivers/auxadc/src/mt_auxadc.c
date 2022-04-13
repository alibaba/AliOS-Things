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
#include "freertos_to_aos.h"
#include <arch_timer.h>
#include <mt_auxadc.h>
#include <mt_reg_base.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define dsb() __asm__ __volatile__ ("dsb" : : : "memory")

#define mb()    \
    do {    \
        dsb();    \
    } while (0)

#define DRV_Reg32(addr)             (*(volatile unsigned int *)(addr))
#define DRV_WriteReg32(addr, data)    \
    do {    \
        ((*(volatile unsigned int *)(addr)) = (unsigned int)data);    \
        mb();    \
    } while(0)

#define DRV_Reg16(addr)             (*(volatile unsigned short *)(addr))
#define DRV_WriteReg16(addr, data)    \
    do {    \
        ((*(volatile unsigned short *)(addr)) = (unsigned short)data);    \
        mb();    \
    } while(0)
/* step1 check con2 if auxadc is busy
 * step2 clear bit
 * step3 read channle and make sure old ready bit ==0
 * step4 set bit to trigger sample
 * step5 read channle and make sure  ready bit ==1
 * step6 read data
 */
int32_t IMM_GetOneChannelValue(int32_t dwChannel,
		int32_t data[4], int32_t* rawdata)
{
	uint32_t channel[16] = {0};
	int32_t idle_count = 0;
	int32_t data_ready_count = 0;
	/* open clk */
	DRV_WriteReg32(AUXADC_CLR_CLOCK, 1 << 10);
	/* step1 check con2 if auxadc is busy */
	while (DRV_Reg16(AUXADC_CON2) & 0x01) {
		printf("[adc_api]: wait for module idle\n");
		udelay(10000);
		idle_count++;
		if (idle_count > 30) {
			/* wait for idle time out */
			printf("[adc_api]: wait for aux/adc idle time out\n");
			return -1;
		}
	}
	/* step2 clear bit */
	DRV_WriteReg16(AUXADC_CON1_CLR, 1 << dwChannel);
	/* step3  read channle and make sure old ready bit ==0 */
	while (DRV_Reg16(AUXADC_DAT0 + dwChannel * 0x04) & (1 << 12)) {
		printf("wait for channel[%d] ready bit clear\n", dwChannel);
		udelay(10000);
		data_ready_count++;
		if (data_ready_count > 30) {
			/* wait for idle time out */
			printf("wait for channel%d ready bit clear time out\n",
				dwChannel);
			return -2;
		}
	}
	/* step4 set bit  to trigger sample */
	DRV_WriteReg16(AUXADC_CON1_SET, 1 << dwChannel);
	/* step5  read channle and make sure  ready bit ==1 */
	udelay(25);/* we must dealay here for hw sample channel data */
	while (0 == (DRV_Reg16(AUXADC_DAT0 + dwChannel * 0x04) & (1 << 12))) {
		printf("wait for channel[%d] ready bit ==1\n", dwChannel);
		udelay(10000);
		data_ready_count++;
		if (data_ready_count > 30) {
			/* wait for idle time out */
			printf("wait for channel[%d] data ready time out\n",
				dwChannel);
			return -3;
		}
	}
	/* step6 read data */
	channel[dwChannel] =
		DRV_Reg16(AUXADC_DAT0 + dwChannel * 0x04) & 0x0FFF;
	if (rawdata) {
		*rawdata = channel[dwChannel];
	}
	data[0] = (channel[dwChannel] * 150 / 4096 / 100);
	data[1] = ((channel[dwChannel] * 150 / 4096) % 100);
	return 0;
}
/* 1v == 1000 mv
   this function voltage Unit is mv */
int32_t IMM_GetOneChannelValue_Cali(int32_t Channel, int32_t* voltage)
{
	int32_t ret = 0, data[4], rawvalue;
	ret = IMM_GetOneChannelValue(Channel, data, &rawvalue);
	if (ret) {
		ret = IMM_GetOneChannelValue(Channel, data, &rawvalue);
		if (ret) {
			printf("GetOneChannelValue_Cali get value error %d\n",
				ret);
			return -1;
		}
	}
	*voltage = rawvalue * 1500 / 4096;

	return 0;
}

int32_t auxadc_test(void)
{
	int32_t i = 0, data[4] = {0, 0, 0, 0};
	int32_t res = 0;
	int32_t rawdata = 0;
	for (i = 0; i < 16; i++) {
		res = IMM_GetOneChannelValue(i, data, &rawdata);
		if (res < 0) {
			printf("[adc_lk]: get data error\n");
			break;
		} else {
			printf("[adc_lk]: channel[%d]raw =%d\n", i, rawdata);
		}
	}
	return 0;
}

