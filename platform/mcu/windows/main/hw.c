/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

 /**
  *                      caution
  * linuxhost hw.c won't use any lwip functionalities,
  * disable CONFIG_AOS_LWIP to avoid close() -> lwip_close()
  */
#undef CONFIG_AOS_LWIP

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "aos/kernel.h"

#include "aos/hal/timer.h"
#include "network/hal/base.h"
#include <hal/wifi.h>
#include <arg_options.h>

#define TAG "hw"

static bool per_pid_flash;
static FILE* open_flash(int pno, bool w)
{
	char fn[64];

	FILE *flash_fd = NULL;

	if (per_pid_flash)
		snprintf(fn, sizeof(fn), "./aos_partition_%d_%d.bin", GetCurrentProcessId(), pno);
	else
		snprintf(fn, sizeof(fn), "./aos_partition_%d.bin", pno);

	if (w)
		flash_fd = fopen(fn, "r+");
	else
		flash_fd = fopen(fn, "r");

	if (flash_fd != NULL)
	{
		goto out;
	}


	flash_fd = fopen(fn, "w+");
	if (flash_fd != NULL)//Create & Open Sucessfully
	{
		goto out;
	}

	fclose(flash_fd);
	flash_fd = fopen(fn, "w+");
out:
	return flash_fd;
}

int32_t hal_flash_write(hal_partition_t pno, uint32_t* poff, const void* buf, uint32_t buf_size)
{
	int ret = -1;
	char *tmp, *new_val;
	char *origin = NULL;
	FILE* flash_fd = open_flash(pno, true);

	if (flash_fd != NULL)
	{
		char *origin = (char *)malloc(buf_size);
		if (NULL != origin)
		{
			memset(origin, -1, buf_size);
			if (fseek(flash_fd, *poff, 0) == 0)
			{
				ret = fread(origin,sizeof(char), buf_size, flash_fd);
				if (ret < 0)
				{
					perror("error reading flash:");
				}
				else
				{
					tmp = origin;
					new_val = (char *)buf;
					for (uint32_t i = 0; i < buf_size; i++) {
						(*tmp) &= (*new_val);
						new_val++;
						tmp++;
					}
					if (fseek(flash_fd, *poff, 0) == 0)
					{
						ret = fwrite(origin, sizeof(char), buf_size, flash_fd);

						if (ret < 0)
							perror("error writing flash:");
						else
							*poff += ret;
					}
				}
			}
			free(origin);
		}
		fclose(flash_fd);
	}
	return ret < 0 ? ret : 0;
}

int32_t hal_flash_read(hal_partition_t pno, uint32_t* poff, void* buf, uint32_t buf_size)
{
	int ret = -1;

	if (poff != NULL)
	{
		FILE* flash_fd;
		flash_fd = open_flash(pno, false);
		if (flash_fd != NULL)
		{
			if (fseek(flash_fd, *poff, 0) == 0)
			{
				ret = fread(buf, sizeof(char), buf_size, flash_fd);
				if (ret < 0)
					perror("error reading flash:");
				else
					*poff += ret;
			}
			fclose(flash_fd);
		}
	}
	return ret < 0 ? ret : 0;
}

int32_t hal_flash_erase(hal_partition_t in_partition, uint32_t off_set,
	uint32_t size)
{
	int ret = -1;
	FILE* flash_fd = open_flash(in_partition, true);
	if (NULL != flash_fd)
	{
		char *buf = (char *)malloc(size);
		if (buf != NULL)
		{
			if (fseek(flash_fd, off_set, 0) == 0)
			{
				memset(buf, -1, size);
				ret = fwrite(buf, sizeof(char), size, flash_fd);
				if (ret < 0)
					perror("error erase flash:");
			}
			free(buf);
		}

		fclose(flash_fd);
	}

	return ret < 0 ? ret : 0;
}

void hal_reboot(void)
{

}

#ifdef OSAL_RHINO
#include <k_api.h>
#define us2tick(us) \
    ((us * RHINO_CONFIG_TICKS_PER_SECOND + 999999) / 1000000)

static void _timer_cb(void *timer, void *arg)
{
	timer_dev_t *tmr = arg;
	tmr->config.cb(tmr->config.arg);
}

int32_t hal_timer_init(timer_dev_t *tim)
{
	if (tim->config.reload_mode == TIMER_RELOAD_AUTO) {
		krhino_timer_dyn_create((ktimer_t **)&tim->priv, "hwtmr", _timer_cb,
			us2tick(tim->config.period), us2tick(tim->config.period), tim, 0);
	}
	else {
		krhino_timer_dyn_create((ktimer_t **)&tim->priv, "hwtmr", _timer_cb,
			us2tick(tim->config.period), 0, tim, 0);
	}
}

int hal_timer_start(timer_dev_t *tmr)
{
	return krhino_timer_start(tmr->priv);
}

void hal_timer_stop(timer_dev_t *tmr)
{
	krhino_timer_stop(tmr->priv);
	krhino_timer_dyn_del(tmr->priv);
	tmr->priv = NULL;
}

int csp_printf(const char *fmt, ...)
{
	CPSR_ALLOC();

	va_list args;
	int ret;

	RHINO_CRITICAL_ENTER();

	va_start(args, fmt);
	ret = vprintf(fmt, args);
	va_end(args);

	fflush(stdout);

	RHINO_CRITICAL_EXIT();

	return ret;
}
#else

int csp_printf(const char *fmt, ...)
{
	va_list args;
	int ret;

	va_start(args, fmt);
	ret = vprintf(fmt, args);
	va_end(args);

	fflush(stdout);

	return ret;
}

#endif

#if defined(DEV_SAL_MK3060)
extern hal_wifi_module_t aos_wifi_module_mk3060;
#else
extern hal_wifi_module_t sim_aos_wifi_linux;
#endif
uart_dev_t uart_0;

void linux_wifi_register(void);
void hw_start_hal(options_t *poptions)
{
	uart_0.port = 0;
	uart_0.config.baud_rate = 921600;
	uart_0.config.data_width = DATA_WIDTH_8BIT;
	uart_0.config.parity = NO_PARITY;
	uart_0.config.stop_bits = STOP_BITS_1;
	uart_0.config.flow_control = FLOW_CONTROL_DISABLED;

	per_pid_flash = poptions->flash.per_pid;

#ifdef AOS_COMP_CLI
	if (poptions->cli.enable)
		hal_uart_init(&uart_0);
#endif

#ifdef AOS_HAL
#if defined(DEV_SAL_MK3060)
	hal_wifi_register_module(&aos_wifi_module_mk3060);
#else
	hal_wifi_register_module(&sim_aos_wifi_linux);
#endif
#endif
}
