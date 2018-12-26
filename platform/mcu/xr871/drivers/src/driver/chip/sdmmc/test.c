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

#include <string.h>
#include <stdio.h>

#include "sys/io.h"
#include "sys/xr_debug.h"

#include "kernel/os/os_time.h"
#include "kernel/os/os_semaphore.h"

#include "driver/chip/hal_def.h"
#include "driver/chip/hal_ccm.h"
#include "driver/chip/sdmmc/hal_sdhost.h"
#include "driver/chip/sdmmc/sdmmc.h"

#define TEST_SD
#define TEST_SD_WRITE

#ifdef TEST_SD

#define SIZE_1K 	(1024)
#define SIZE_1M         (SIZE_1K*SIZE_1K)

#define READ_WRITE_SINGLE_SIZE  (16*SIZE_1K)
#define READ_WRITE_TOTAL_SIZE   (8*SIZE_1M)

struct sdmmc_tester {
	struct mmc_card card;
	uint8_t wbuf[READ_WRITE_SINGLE_SIZE];
	uint8_t rbuf[READ_WRITE_SINGLE_SIZE];
#ifdef CONFIG_DETECT_CARD
	OS_Semaphore_t card_present_sem;
	uint32_t card_exist;
#endif
};

static struct sdmmc_tester *sdmmc_test = NULL;

void print_hex_dump_words(const void *addr, unsigned int len);

#ifdef CONFIG_DETECT_CARD
void card_detect(uint32_t present)
{
	if (present) {
		printf("%s insert\n", __func__);
		OS_MSleep(500); /* wait voltage stable */

		if (mmc_rescan(&sdmmc_test->card, 0)) {
			printf("Initial card failed!!\n");
			return ;
		} else {
			printf("Initial card success\n");
			OS_SemaphoreRelease(&sdmmc_test->card_present_sem);
		}
	} else {
		printf("%s removed\n", __func__);
	}
}
#endif

int32_t mmc_test_init(void)
{
	SDC_InitTypeDef sdc_param;

	if (!sdmmc_test)
		sdmmc_test = malloc(sizeof(struct sdmmc_tester));
	if (!sdmmc_test) {
		printf("malloc faild!\n");
		return -1;
	}

#ifdef CONFIG_DETECT_CARD
	uint32_t cd_mode = CARD_DETECT_BY_GPIO_IRQ;
	sdmmc_test->card_exist = 1;

	OS_SemaphoreCreate(&sdmmc_test->card_present_sem, 0, OS_SEMAPHORE_MAX_COUNT);

	XR_ASSERT((cd_mode != CARD_ALWAYS_PRESENT) && (cd_mode != CARD_DETECT_BY_GPIO_IRQ) && \
	          (cd_mode != CARD_DETECT_BY_D3), 1, " cd_mode config err!\n");

	sdc_param.cd_mode = cd_mode;
	sdc_param.cd_cb = &card_detect;
#endif
	HAL_SDC_Init(0, &sdc_param);

	return 0;
}

int32_t mmc_test_exit(void)
{
	if (!sdmmc_test->card.host)
		return 0;

	mmc_card_deinit(&sdmmc_test->card);
	HAL_SDC_Deinit(0);

#ifdef CONFIG_DETECT_CARD
	OS_SemaphoreDelete(&sdmmc_test->card_present_sem);
#endif

	if (sdmmc_test) {
		free(sdmmc_test);
		sdmmc_test = NULL;
	}

	return 0;
}

struct mmc_card *mmc_scan_init(void)
{
	if (!mmc_rescan(&sdmmc_test->card, 0))
		return &sdmmc_test->card;
	else
		return NULL;
}

int32_t mmc_test(uint32_t cd_mode)
{
	int32_t err;
	uint32_t i, cnt = 0;

	if (mmc_test_init()) {  /* reinit is ok */
		return -1;
	}

	memset((void *)sdmmc_test->wbuf, 0x55, 128);
	memset((void *)&sdmmc_test->wbuf[128], 0xaa, 128);

	for (i = 0; i < 256; i ++)
		sdmmc_test->wbuf[256 + i] = i;

	memcpy((void *)&sdmmc_test->wbuf[512], (void *)sdmmc_test->wbuf, 512);

	/* scan card for detect card is exist? */
	if (mmc_rescan(&sdmmc_test->card, 0)) {
		printf("Initial card failed!!\n");
#ifdef CONFIG_DETECT_CARD
		sdmmc_test->card_exist = 0;
#else
		goto out;
#endif
	} else {
		printf("Initial card success\n");
	}

	while (cnt++ < 100) {
		uint32_t throuth_mb, throuth_kb;
		OS_Time_t tick_use;
#ifdef CONFIG_DETECT_CARD
		if (!sdmmc_test->card_exist || (cd_mode != CARD_ALWAYS_PRESENT))
			OS_SemaphoreWait(&sdmmc_test->card_present_sem, OS_WAIT_FOREVER);
#endif

		printf("%s,%d test count:%u\n", __func__, __LINE__, cnt);

#ifdef TEST_SD_WRITE
		tick_use = OS_GetTicks();
		err = mmc_block_write(&sdmmc_test->card, sdmmc_test->wbuf, 0, 1);
		tick_use = OS_GetTicks() - tick_use;
		if (err) {
			goto err_out;
		} else
			printf("%s 1 block write ok, 512B use:%u ms\n", __func__,
			       (uint32_t)OS_TicksToMSecs(tick_use));
#endif
		memset((void *)sdmmc_test->rbuf, 0, 512);
		tick_use = OS_GetTicks();
		err = mmc_block_read(&sdmmc_test->card, sdmmc_test->rbuf, 0, 1);
		tick_use = OS_GetTicks() - tick_use;
		if (err) {
			goto err_out;
		} else {
			printf("%s 1 block read ok, 512B use:%u ms\n", __func__,
			       (uint32_t)OS_TicksToMSecs(tick_use));
#ifndef TEST_SD_WRITE
			print_hex_dump_words(sdmmc_test->rbuf, 512);
#endif
		}
#ifdef TEST_SD_WRITE
		if (memcmp((void *)sdmmc_test->wbuf, (void *)sdmmc_test->rbuf, 512)) {
			goto err_out;
		} else
			printf("%s,%d mmc 1 block rw ok\n", __func__, __LINE__);

		tick_use = OS_GetTicks();
		for (i = 0; i < READ_WRITE_TOTAL_SIZE/READ_WRITE_SINGLE_SIZE; i++) {
			err = mmc_block_write(&sdmmc_test->card, sdmmc_test->wbuf, 3 + i * (READ_WRITE_SINGLE_SIZE/512),
			                      READ_WRITE_SINGLE_SIZE/512);
			if (err)
				break;
			if (i % 50 == 0)
				printf("%s, wirite cnt:%u\n", __func__, i);
		}
		tick_use = OS_GetTicks() - tick_use;
		if (err) {
			printf("%s,%d mmc mult blocks write err!\n", __func__, __LINE__);
			goto err_out;
		} else {
			throuth_kb = READ_WRITE_TOTAL_SIZE/SIZE_1K*1000/(uint32_t)OS_TicksToMSecs(tick_use);
			throuth_mb = throuth_kb/1000;
			printf("%s mult blocks write ok, %d MB use:%u ms, throughput:%u.%u MB/S\n",
			       __func__, READ_WRITE_TOTAL_SIZE/SIZE_1M, (uint32_t)OS_TicksToMSecs(tick_use),
			       throuth_mb, throuth_kb - throuth_mb);
		}
#endif

		tick_use = OS_GetTicks();
		for (i = 0; i < READ_WRITE_TOTAL_SIZE/READ_WRITE_SINGLE_SIZE; i++) {
			err = mmc_block_read(&sdmmc_test->card, sdmmc_test->rbuf, 3 + i * (READ_WRITE_SINGLE_SIZE/512),
			                     READ_WRITE_SINGLE_SIZE/512);
			if (err)
				break;
		}
		tick_use = OS_GetTicks() - tick_use;
		if (err) {
			printf("%s,%d mmc mult blocks read err!\n", __func__, __LINE__);
			goto err_out;
		} else {
			throuth_kb = READ_WRITE_TOTAL_SIZE/SIZE_1K*1000/(uint32_t)OS_TicksToMSecs(tick_use);
			throuth_mb = throuth_kb/1000;
			printf("%s mult blocks read ok, %d MB use:%u ms, throughput:%u.%u MB/S\n",
			       __func__, READ_WRITE_TOTAL_SIZE/SIZE_1M, (uint32_t)OS_TicksToMSecs(tick_use),
			       throuth_mb, throuth_kb - throuth_mb);
		}

		memset((void *)sdmmc_test->rbuf, 0, READ_WRITE_SINGLE_SIZE);
		err = mmc_block_read(&sdmmc_test->card, sdmmc_test->rbuf, 3, READ_WRITE_SINGLE_SIZE/512);
		if (err) {
			goto err_out;
		} else
			printf("%s %d blocks read ok\n", __func__, READ_WRITE_SINGLE_SIZE/512);

#ifdef TEST_SD_WRITE
		if (memcmp((void *)sdmmc_test->wbuf, (void *)sdmmc_test->rbuf, 1024)) { /* check 1024B */
			printf("%s %d mmc blocks rw failed\n", __func__, READ_WRITE_SINGLE_SIZE/512);
			goto err_out;
		} else
			printf("%s %d mmc blocks rw ok\n", __func__, READ_WRITE_SINGLE_SIZE/512);
#endif

		OS_MSleep(1000);
	}

	mmc_test_exit();

	return 0;

err_out:
#ifdef TEST_SD_WRITE
	printf("%s,%d mmc block rw failed\n", __func__, __LINE__);
	printf("rbuf:\n");
	print_hex_dump_words(sdmmc_test->rbuf, SIZE_1K);
	printf("wbuf:\n");
	print_hex_dump_words(sdmmc_test->wbuf, 512);
#endif
#ifndef CONFIG_DETECT_CARD
out:
#endif
	return -1;
}
#endif /* TEST_SD */
