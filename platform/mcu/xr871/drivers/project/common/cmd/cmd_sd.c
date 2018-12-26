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

#include "sys/xr_debug.h"

#include "cmd_util.h"
#include "cmd_sd.h"

#include "driver/chip/hal_def.h"
#include "driver/chip/sdmmc/hal_sdhost.h"
#include "driver/chip/sdmmc/sdmmc.h"

static struct mmc_card *card;

extern int32_t mmc_test(uint32_t cd_mode);
extern int32_t mmc_test_init(void);
extern int32_t mmc_test_exit(void);
struct mmc_card *mmc_scan_init(void);

/*
 * drv sd init
 */
static enum cmd_status cmd_sd_init_exec(char *cmd)
{
	mmc_test_init();

	return CMD_STATUS_OK;
}

/*
 * drv sd scan
 */
static enum cmd_status cmd_sd_scan_exec(char *cmd)
{
	card = mmc_scan_init();
	if (!card) {
		CMD_ERR("scan card failed!\n");
		return CMD_STATUS_OK;;
	}

	return CMD_STATUS_OK;
}

/*
 * drv sd read s=<Start_Sector> n=<sector_num>
 */
static enum cmd_status cmd_sd_read_exec(char *cmd)
{
	int32_t err;
	uint32_t cnt;
	uint32_t start_sector, r_secnum;
	char *buf;

	cnt = cmd_sscanf(cmd, "s=%d n=%d", &start_sector, &r_secnum);
	if (cnt != 2) {
		CMD_ERR("invalid argument %s\n", cmd);
		return CMD_STATUS_FAIL;
	}

	buf = cmd_malloc(r_secnum*512);
	if (!buf)
		return CMD_STATUS_OK;;

	err = mmc_block_read(card, (uint8_t *)buf, start_sector, r_secnum);
	if (err) {
		CMD_ERR("mmc mult blocks read err!\n");
		return CMD_STATUS_FAIL;
	}

	CMD_DBG("read from %d sector, %d sectors\n", start_sector, r_secnum);
	print_hex_dump_bytes(buf, r_secnum*512);

	return CMD_STATUS_OK;
}

/*
 * drv sd test
 */
static enum cmd_status cmd_sd_test_exec(char *cmd)
{
	mmc_test(0);

	return CMD_STATUS_OK;
}

/*
 * drv sd deinit
 */
static enum cmd_status cmd_sd_deinit_exec(char *cmd)
{
	mmc_test_exit();

	return CMD_STATUS_OK;
}

#define CMD_ARG_LEN  64

struct sd_test_param {
	uint8_t task_idx;
	uint8_t random;
	uint8_t task_num;
	uint16_t time_sec;
	uint32_t start_sector;
	uint32_t sector_num;
};

static void cmd_sd_bench_task(void *arg)
{
	int32_t err;
	char *cmd = (char *)arg;
	uint32_t start_sector;
	uint32_t cnt;
	uint32_t throuth_mb, throuth_kb;
	OS_Time_t tick_use;
	uint32_t bench_sector;

	cnt = cmd_sscanf(cmd, "s=%d", &start_sector);
	if (cnt != 1) {
		CMD_ERR("invalid argument %s\n", cmd);
		goto out;
	}

	mmc_test_init();
	card = mmc_scan_init();
	if (!card) {
		CMD_ERR("scan card failed!\n");
		goto out;
	}

	bench_sector = start_sector - 1;

	for (int i = 0; i < 1000; i++) {
		uint32_t bench_size = 512 * (1 << i);
		uint8_t *buf = cmd_malloc(bench_size);
		if (!buf) {
			CMD_DBG("%s test end for malloc buff failed!\n", __func__);
			goto out;
		}

		for (int j = 0; j < bench_size/4; j++)
			((unsigned int *)buf)[j] = j;

		bench_sector += bench_size/512;

		tick_use = OS_GetTicks();
		err = mmc_block_write(card, buf, bench_sector, bench_size/512);
		tick_use = OS_GetTicks() - tick_use;
		if (!tick_use)
			tick_use = 1;
		if (err) {
			CMD_ERR("mmc mult blocks write err!\n");
			goto next;
		} else {
			throuth_kb = bench_size*1000/1024/(uint32_t)OS_TicksToMSecs(tick_use);
			throuth_mb = throuth_kb/1000;
			CMD_DBG("%s mult blocks write ok, ", __func__);
			if (bench_size <= 512)
				CMD_LOG(CMD_DBG_ON, "0.5");
			else
				CMD_LOG(CMD_DBG_ON, "%3d", bench_size/1024);
			CMD_LOG(CMD_DBG_ON, " KB use:%3d ms, throughput:%d.%d MB/S\n",
			        (uint32_t)OS_TicksToMSecs(tick_use),
			        throuth_mb, throuth_kb - throuth_mb);
		}

		for (int j = 0; j < bench_size/4; j++)
			((unsigned int *)buf)[j] = 0;

		tick_use = OS_GetTicks();
		err = mmc_block_read(card, buf, bench_sector, bench_size/512);
		tick_use = OS_GetTicks() - tick_use;
		if (!tick_use)
			tick_use = 1;
		if (err) {
			CMD_ERR("mmc mult blocks read err!\n");
			goto next;
		} else {
			throuth_kb = bench_size*1000/1024/(uint32_t)OS_TicksToMSecs(tick_use);
			throuth_mb = throuth_kb/1000;
			CMD_DBG("%s mult blocks read  ok, ", __func__);
			if (bench_size <= 512)
				CMD_LOG(CMD_DBG_ON, "0.5");
			else
				CMD_LOG(CMD_DBG_ON, "%3d", bench_size/1024);
			CMD_LOG(CMD_DBG_ON, " KB use:%3d ms, throughput:%d.%d MB/S\n",
			        (uint32_t)OS_TicksToMSecs(tick_use),
			        throuth_mb, throuth_kb - throuth_mb);
		}

		err = 0;
		for (int j = 0; j < bench_size/4; j++) {
			if (((unsigned int *)buf)[j] != j) {
				err = -1;
				break;
			}
		}
		if (err) {
			CMD_ERR("mmc %d blocks write data err!\n", bench_size/512);
			goto next;
		}
next:
		cmd_free(buf);
		if (err)
			break;
	}

out:
	CMD_DBG("%s test end\n", __func__);
	mmc_test_exit();
	cmd_free(arg);
	OS_ThreadDelete(NULL);
}

#define PRESS_FILE_SIZE (20 * 1024 * 1024) /* 20M */
#define PRESS_READ_SIZE 2048
#define PRESS_WRITE_SIZE 8192
static OS_Semaphore_t sem_wait;

static void cmd_sd_press_read_task(void *arg)
{
	int32_t err;
	struct sd_test_param *param = (struct sd_test_param *)arg;
	OS_Time_t tick_now = 0, tick_print = 0;
	OS_Time_t tick_end = OS_GetTicks() + OS_MSecsToTicks(param->time_sec * 1000);
	uint32_t random_sleep = param->random;
	char *buf;
	uint32_t start_sector = param->start_sector;

	if (param->task_idx == 0) {
		buf = cmd_malloc(4096);
		for (int i = 0; i < 512/4; i++) {
			((unsigned int *)buf)[i] = i;
		}
		memcpy(buf+512, buf, 512);
		memcpy(buf+1024, buf, 1024);
		memcpy(buf+2048, buf, 2048);
		CMD_DBG("%s do nothing until sd card prepare ok!\n", __func__);
		for (int i = 0; i < (PRESS_FILE_SIZE+4095)/4096; i++) { /* 10M */
			int32_t err;
			err = mmc_block_write(card, (uint8_t *)buf, start_sector + (i*(4096/512)), 4096/512);
			if (err)
				goto out;
		}
		CMD_DBG("%s sd card prepared ok!\n", __func__);

		for (int j = 0; j < param->task_num - 1; j++)
			OS_SemaphoreRelease(&sem_wait);
		cmd_free(buf);
	} else {
		OS_SemaphoreWait(&sem_wait, OS_WAIT_FOREVER);
	}

	buf = cmd_malloc(param->sector_num*512);
	if (!buf)
		goto exit;

	if (!random_sleep)
		random_sleep = 2;
	OS_MSleep(random_sleep);
	CMD_DBG("%s id:%d random:%d start_sector:%d\n", __func__, param->task_idx,
	        random_sleep, start_sector);

	while (tick_now < tick_end) {
		int j;
		err = mmc_block_read(card, (uint8_t *)buf, start_sector, param->sector_num);
		if (err) {
			CMD_ERR("mmc mult blocks read err!\n");
			goto out;
		}
		err = 0;
		for (j = 0; j < param->sector_num*(512/4); j++) {
			if (((unsigned int *)buf)[j] != (j & 0x07f)) {
				err = -1;
				CMD_ERR("%x %x\n", j, ((unsigned int *)buf)[j]);
				break;
			}
		}
		if (err) {
			CMD_ERR("mmc %d blocks read data err! at sector:%d count:%d\n",
			        param->sector_num, start_sector, j);
			goto out;
		} else
			;//CMD_DBG("%s mmc blocks read data ok! at sector:%d\n",
			//         __func__, start_sector);
		start_sector += param->sector_num;
		if (start_sector >= param->start_sector + PRESS_FILE_SIZE/512) {
			start_sector = param->start_sector;
		}
		OS_MSleep(random_sleep);
		tick_now = OS_GetTicks();
		if (tick_now >= tick_print + 5000) {
			CMD_DBG("%s id:%d testing... at sector:%d\n", __func__,
			        param->task_idx, start_sector);
			tick_print = tick_now;
		}
	}

out:
	CMD_DBG("%s id:%d test end\n", __func__, param->task_idx);
	cmd_free(buf);
exit:
	cmd_free(param);
	if (param->task_idx == 0)
		OS_SemaphoreDelete(&sem_wait);
	OS_ThreadDelete(NULL);
}

static void cmd_sd_press_write_task(void *arg)
{
	int32_t err;
	struct sd_test_param *param = (struct sd_test_param *)arg;
	OS_Time_t tick_now = 0, tick_print = 0;
	OS_Time_t tick_end = OS_GetTicks() + OS_MSecsToTicks(param->time_sec * 1000);
	uint32_t random_sleep = param->random;
	char *buf;
	uint32_t start_sector = param->start_sector;
	uint32_t round = 0;
	uint32_t total_num;

	buf = cmd_malloc(param->sector_num*512);
	if (!buf)
		goto out;

	for (int i = 0; i < 512/4; i++)
		((unsigned int *)buf)[i] = i;

	for (int i = 1; i < param->sector_num; i++)
		memcpy(buf + i * 512, buf, 512);

	if (!random_sleep)
		random_sleep = 2;
	OS_MSleep(random_sleep);
	CMD_DBG("%s id:%d random:%d\n", __func__, param->task_idx, random_sleep);

	while (tick_now < tick_end) {
		err = mmc_block_write(card, (uint8_t *)buf, start_sector, param->sector_num);
		if (err) {
			CMD_ERR("mmc mult blocks write err!\n");
			goto out;
		}
		start_sector += param->sector_num;
		if (start_sector >= param->start_sector + PRESS_FILE_SIZE/512) {
			start_sector = param->start_sector;
			round = 1;
		}
		OS_MSleep(random_sleep);
		tick_now = OS_GetTicks();
		if (tick_now >= tick_print + 5000) {
			CMD_DBG("%s id:%d testing... at sector:%d\n", __func__,
			        param->task_idx, start_sector);
			tick_print = tick_now;
		}
	}
	CMD_DBG("%s test end. round:%d start_sector:%d end_sector:%d\n", __func__,
	        round, param->start_sector, start_sector);

	if (round) {
		total_num = PRESS_FILE_SIZE/512;
	} else {
		// BUG_ON(start_sector <= param->start_sector);
		total_num = start_sector - param->start_sector;
	}
	total_num /= param->sector_num;

	start_sector = param->start_sector;
	CMD_DBG("%s test checking... start_sector:%d total_num:%d\n", __func__,
	        start_sector, total_num);
	for (int i = 0; i < total_num; i++) {
		int j;

		memset(buf, 0, param->sector_num*512);
		err = mmc_block_read(card, (uint8_t *)buf, start_sector, param->sector_num);
		if (err) {
			CMD_ERR("mmc mult blocks read err!\n");
			goto out;
		}
		err = 0;
		for (j = 0; j < param->sector_num*(512/4); j++) {
			if (((unsigned int *)buf)[j] != (j & 0x07f)) {
				err = -1;
				break;
			}
		}
		if (err) {
			CMD_ERR("%x %x\n", ((unsigned int *)buf)[j], (j & 0x07f));
			CMD_ERR("mmc %d blocks write data err! at sector:%d count:%d\n",
			        param->sector_num, start_sector, j);
			goto out;
		} else
			;//CMD_DBG("%s mmc blocks write data ok! at sector:%d\n",
			//         __func__, start_sector);
		start_sector += param->sector_num;
	}

out:
	CMD_DBG("%s id:%d test end\n", __func__, param->task_idx);
	cmd_free(buf);
	cmd_free(param);
	OS_ThreadDelete(NULL);
}

/*
 * drv sd bench s=<Start_Sector>
 */
static enum cmd_status cmd_sd_bench_exec(char *cmd)
{
	OS_Thread_t thread;
	char *param;
	uint32_t len;

	len = strlen(cmd);
	if (len >= CMD_ARG_LEN) {
		CMD_ERR("should adjust CMD_ARG_LEN to %d\n", len);
		return CMD_STATUS_FAIL;
	}

	param = cmd_malloc(CMD_ARG_LEN);
	if (!param)
		return CMD_STATUS_FAIL;
	memcpy(param, cmd, len);
	param[len+1] = 0;

	OS_ThreadSetInvalid(&thread);
	if (OS_ThreadCreate(&thread,
	                    "",
	                    cmd_sd_bench_task,
	                    param,
	                    OS_THREAD_PRIO_APP,
	                    2 * 1024) != OS_OK) {
		CMD_ERR("create sd bench test task failed\n");
		return CMD_STATUS_FAIL;
	}

	return CMD_STATUS_OK;
}

/*
 * drv sd press r=<threads_num> s=<Start_Sector> n=<sector_num> w=<threads_num>
 *              s=<Start_Sector> n=<sector_num> t=<secons>
 */
static enum cmd_status cmd_sd_press_exec(char *cmd)
{
	OS_Thread_t thread;
	struct sd_test_param *param;
	uint32_t r_threads, w_threads;
	uint32_t r_secnum, w_secnum;
	uint32_t cnt;
	uint32_t time_sec;
	uint32_t start_rsector, start_wsector;

	cnt = cmd_sscanf(cmd, "r=%d s=%d n=%d w=%d s=%d n=%d t=%d",
	                 &r_threads, &start_rsector, &r_secnum,
	                 &w_threads, &start_wsector, &w_secnum, &time_sec);
	if (cnt != 7) {
		CMD_ERR("invalid argument %s\n", cmd);
		return CMD_STATUS_FAIL;
	}

	OS_SemaphoreCreate(&sem_wait, 0, OS_SEMAPHORE_MAX_COUNT);

	OS_MSleep(5);

	for (uint32_t i = 0; i < r_threads; i++) {
		param = cmd_malloc(sizeof(struct sd_test_param));
		if (!param)
			return CMD_STATUS_FAIL;
		param->task_idx = i;
		param->task_num = r_threads;
		param->start_sector = start_rsector;
		param->sector_num = r_secnum;
		param->time_sec = time_sec;
		param->random = rand() % 8 + i;
		if (!r_secnum || time_sec < 2) {
			CMD_ERR("%s read n=<sector_num> should not 0 !\n", __func__);
			cmd_free(param);
			goto out;
		}
		OS_ThreadSetInvalid(&thread);
		if (OS_ThreadCreate(&thread,
		                    "",
		                    cmd_sd_press_read_task,
		                    param,
		                    OS_THREAD_PRIO_APP,
		                    2 * 1024) != OS_OK) {
			CMD_ERR("create sd bench test task failed\n");
			return CMD_STATUS_FAIL;
		}
		(void)cmd_sd_press_read_task;
		OS_MSleep(2);
	}

	for (uint32_t i = 0; i < w_threads; i++) {
		param = cmd_malloc(sizeof(struct sd_test_param));
		if (!param)
			return CMD_STATUS_FAIL;
		param->task_idx = i;
		param->task_num = w_threads;
		param->start_sector = start_wsector;
		param->sector_num = w_secnum;
		param->time_sec = time_sec;
		param->random = rand() % 20 + i;
		if (!w_secnum || time_sec < 2) {
			CMD_ERR("%s write n=<sector_num> should not 0 !\n", __func__);
			cmd_free(param);
			goto out;
		}
		OS_ThreadSetInvalid(&thread);
		if (OS_ThreadCreate(&thread,
				    "",
				    cmd_sd_press_write_task,
				    param,
				    OS_THREAD_PRIO_APP,
				    2 * 1024) != OS_OK) {
			CMD_ERR("create sd bench test task failed\n");
			return CMD_STATUS_FAIL;
		}
		OS_MSleep(2);
	}

out:
	return CMD_STATUS_OK;
}

static struct cmd_data g_sd_cmds[] = {
	{ "init",     cmd_sd_init_exec },
	{ "deinit",   cmd_sd_deinit_exec },
	{ "scan",     cmd_sd_scan_exec },
	{ "read",     cmd_sd_read_exec },
	{ "test",     cmd_sd_test_exec },
	{ "bench",    cmd_sd_bench_exec },
	{ "press",    cmd_sd_press_exec },
};

enum cmd_status cmd_sd_exec(char *cmd)
{
	return cmd_exec(cmd, g_sd_cmds, cmd_nitems(g_sd_cmds));
}
