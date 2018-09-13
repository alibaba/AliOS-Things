/**
 * @file console.c
 * @author XRADIO IOT WLAN Team
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

#include <string.h>
#include <ctype.h>
#include "kernel/os/os.h"
#include "console/console.h"
#include "console_debug.h"


#define CONSOLE_EMPTY_CMD_SUPPORT   1

#define CONSOLE_CMD_LINE_MAX_LEN    256
#define CONSOLE_CMD_LINE_BUF_NUM    2

typedef enum {
	CONSOLE_STATE_STOP = 0,
	CONSOLE_STATE_START,
	CONSOLE_STATE_TERMINATE,	/* to be terminated */
} console_state;

static uint8_t g_console_buf[CONSOLE_CMD_LINE_BUF_NUM * CONSOLE_CMD_LINE_MAX_LEN];

typedef struct console_priv {
    UART_ID         uart_id;
    console_state   state;

    uint8_t         free_buf_bitmap[CONSOLE_CMD_LINE_BUF_NUM];
    uint8_t         ready_buf_bitmap[CONSOLE_CMD_LINE_BUF_NUM];
    uint8_t         last_cmd_buf_idx;
    uint8_t         rx_buf_idx;
    uint32_t        rx_data_cnt;

    uint8_t         *buf[CONSOLE_CMD_LINE_BUF_NUM];

    OS_Semaphore_t   cmd_sem;

    console_cmd_exec_func cmd_exec;
} console_priv_t;

static console_priv_t g_console;

#define CONSOLE_SET_BUF_BITMAP_VALID(bitmap, idx)       ((bitmap)[idx] = 1)
#define CONSOLE_SET_BUF_BITMAP_INVALID(bitmap, idx)     ((bitmap)[idx] = 0)
#define CONSOLE_IS_BUF_BITMAP_VALID(bitmap, idx)        ((bitmap)[idx] == 1)

#define CONSOLE_INVALID_BUF_IDX         CONSOLE_CMD_LINE_BUF_NUM

#define CONSOLE_BUF(console, buf_idx)   ((console)->buf[buf_idx])


static uint8_t console_get_valid_buf_idx(uint8_t *bitmap, uint8_t last_idx)
{
	uint8_t loop = CONSOLE_CMD_LINE_BUF_NUM;
	uint8_t idx = last_idx;

	while (loop > 0) {
		idx = (idx + 1) % CONSOLE_CMD_LINE_BUF_NUM;
		if (CONSOLE_IS_BUF_BITMAP_VALID(bitmap, idx)) {
			CONSOLE_SET_BUF_BITMAP_INVALID(bitmap, idx);
			return idx;
		}
		--loop;
	}

	return CONSOLE_INVALID_BUF_IDX;
}

/* called after receive a valid command line */
static __inline void console_rx_cmdline(console_priv_t *console)
{
	/* buf state change: rx --> ready */
	CONSOLE_SET_BUF_BITMAP_VALID(console->ready_buf_bitmap, console->rx_buf_idx);

	/* try to get a new rx buf from free list */
	console->rx_buf_idx = console_get_valid_buf_idx(console->free_buf_bitmap,
							                        console->rx_buf_idx);
	console->rx_data_cnt = 0;

	/* notify console task that a new command is ready */
	OS_SemaphoreRelease(&console->cmd_sem);
}

/* Note: only support line end with "\r\n" or "\n", not support "\r" */
static void console_rx_callback(void *arg)
{
	console_priv_t *console;
	UART_T *uart;
	uint8_t *rx_buf;
	uint32_t cnt;
	uint8_t data;

	uart = (UART_T *)arg;
	console = &g_console;

	if (console->rx_buf_idx != CONSOLE_INVALID_BUF_IDX) {
retry:
		rx_buf = CONSOLE_BUF(console, console->rx_buf_idx);
		cnt = console->rx_data_cnt;
		rx_buf += cnt;

		while (cnt < CONSOLE_CMD_LINE_MAX_LEN) {
			if (HAL_UART_IsRxReady(uart)) {
				data = HAL_UART_GetRxData(uart);
				if (data == '\n') { /* command line end */
					if (cnt > 0 && (*(rx_buf - 1)) == '\r') { /* skip last '\r' */
						--rx_buf;
						--cnt;
					}
#if (!CONSOLE_EMPTY_CMD_SUPPORT)
					if (cnt > 0)
#endif
					{ /* valid command */
						*rx_buf = '\0'; /* C style string */
						CONS_DBG("rx cmd (%u char): '%s'\n", cnt,
						         CONSOLE_BUF(console, console->rx_buf_idx));
#if CONS_CHECK_OVERFLOW
						if (rx_buf - CONSOLE_BUF(console, console->rx_buf_idx)
							>= CONSOLE_CMD_LINE_MAX_LEN) {
							CONS_ERR("rx buf %d overflow\n", console->rx_buf_idx);
						}
#endif
						console_rx_cmdline(console);
						return;
					}
				} else {
					if (isprint(data) || (data == '\r')) { /* valid char */
						*rx_buf = data;
#if CONS_CHECK_OVERFLOW
						if (rx_buf - CONSOLE_BUF(console, console->rx_buf_idx)
							>= CONSOLE_CMD_LINE_MAX_LEN) {
							CONS_ERR("rx buf %d overflow\n", console->rx_buf_idx);
						}
#endif
						++rx_buf;
						++cnt;
					} else { /* invalid char */
						CONS_DBG("rx illegal char 0x%x\n", data);
						console->rx_data_cnt = 0; /* reset rx buffer */
						goto retry;
					}
				}
			} else {
				break; /* no more data */
			}
		}

		if (cnt >= CONSOLE_CMD_LINE_MAX_LEN) {
			/* rx buffer full but no valid command */
			console->rx_data_cnt = 0; /* reset rx buffer */
			CONS_DBG("cmd too long, max %d\n", CONSOLE_CMD_LINE_MAX_LEN - 1);
			goto retry;
		}
		console->rx_data_cnt = cnt;
	} else {
		CONS_WARN("no buf for rx, discard received data\n");
		while (HAL_UART_IsRxReady(uart)) {
			HAL_UART_GetRxData(uart);
		}
	}
}

#define CONSOLE_THREAD_STACK_SIZE	(2 * 1024)
static OS_Thread_t g_console_thread;

static void console_task(void *arg)
{
	uint8_t cmd_buf_idx;
	uint8_t *cmd_buf;
	console_priv_t *console;

	CONS_DBG("%s() start...\n", __func__);

	console = &g_console;

	while (1) {
		if (OS_SemaphoreWait(&console->cmd_sem, OS_WAIT_FOREVER) != OS_OK)
			continue;

		if (console->state != CONSOLE_STATE_START)
			break;

		arch_irq_disable();
		cmd_buf_idx = console_get_valid_buf_idx(console->ready_buf_bitmap,
		                                        console->last_cmd_buf_idx);
		arch_irq_enable();

		if (cmd_buf_idx != CONSOLE_INVALID_BUF_IDX) {
			cmd_buf = CONSOLE_BUF(console, cmd_buf_idx);

			CONS_DBG("exec cmd (idx %d): '%s'\n", cmd_buf_idx, cmd_buf);
			if (console->cmd_exec)
				console->cmd_exec((char *)cmd_buf);

			console->last_cmd_buf_idx = cmd_buf_idx;

			arch_irq_disable();
			if (console->rx_buf_idx == CONSOLE_INVALID_BUF_IDX) {
				/* no buf for rx, set the used one for rx directly */
				console->rx_buf_idx = cmd_buf_idx;
				console->rx_data_cnt = 0;
			} else {
				/* buf state change: ready --> free */
				CONSOLE_SET_BUF_BITMAP_VALID(console->free_buf_bitmap, cmd_buf_idx);
			}
			arch_irq_enable();

		} else {
			CONS_WARN("no valid command\n");
		}
	}

	CONS_DBG("%s() exit\n", __func__);
	OS_ThreadDelete(&g_console_thread);
}

/**
 * @brief Start the console according to the specified parameters
 * @param[in] param Pointer to console_param_t structure
 * @return 0 on success, -1 on failure
 *
 * @note Before starting the console, the related UART specified by
 *       console_param_t::uart_id MUST be inited.
 * @note After starting the console, all UART's input data are read and
 *       processed by the console.
 */
int console_start(console_param_t *param)
{
	int i;
	UART_T *uart;
	console_priv_t *console;

	console = &g_console;
	if (console->state != CONSOLE_STATE_STOP) {
		CONS_ERR("console state %d\n", console->state);
		return -1;
	}

	memset(console, 0, sizeof(*console));
	console->uart_id = param->uart_id;
	console->cmd_exec = param->cmd_exec;

	for (i = 0; i < CONSOLE_CMD_LINE_BUF_NUM; ++i) {
		console->buf[i] = &g_console_buf[i * CONSOLE_CMD_LINE_MAX_LEN];
		CONSOLE_SET_BUF_BITMAP_VALID(console->free_buf_bitmap, i);
		CONSOLE_SET_BUF_BITMAP_INVALID(console->ready_buf_bitmap, i);
	}

	console->last_cmd_buf_idx = CONSOLE_CMD_LINE_BUF_NUM - 1;

	/* set rx buf */
	console->rx_buf_idx = 0;
	console->rx_data_cnt = 0;
	CONSOLE_SET_BUF_BITMAP_INVALID(console->free_buf_bitmap, console->rx_buf_idx);

	if (OS_SemaphoreCreate(&console->cmd_sem, 0, OS_SEMAPHORE_MAX_COUNT) != OS_OK) {
		CONS_ERR("create semaphore failed\n");
		return -1;
	}

	/* start console task */
	if (OS_ThreadCreate(&g_console_thread,
		                "",
		                console_task,
		                NULL,
		                OS_THREAD_PRIO_CONSOLE,
		                CONSOLE_THREAD_STACK_SIZE) != OS_OK) {
		CONS_ERR("create console task failed\n");
		return -1;
	}


	uart = HAL_UART_GetInstance(console->uart_id);
	HAL_UART_EnableRxCallback(console->uart_id, console_rx_callback, uart);
	console->state = CONSOLE_STATE_START;

	return 0;
}

/**
 * @brief Stop the console
 * @return None
 */
void console_stop(void)
{
	console_priv_t *console;

	console = &g_console;
	HAL_UART_DisableRxCallback(console->uart_id);
	console->state = CONSOLE_STATE_TERMINATE;
	OS_SemaphoreRelease(&console->cmd_sem);

	while (OS_ThreadIsValid(&g_console_thread)) {
		OS_MSleep(1); /* wait for thread termination */
	}

	OS_SemaphoreDelete(&console->cmd_sem);
	console->state = CONSOLE_STATE_STOP;
}

/**
 * @brief Write out an amount of data through the console's UART
 * @param[in] buf Pointer to the data buffer
 * @param[in] len Number of bytes to be written out
 * @return Number of bytes written out, -1 on error
 */
int console_write(uint8_t *buf, int32_t len)
{
	console_priv_t *console;

	console = &g_console;
	return HAL_UART_Transmit_Poll(console->uart_id, buf, len);
}

/**
 * @brief Disable the console's input function
 * @note This function is used to disable the console's input function
 *       temporarily when the console is running. When the console is diable,
 *       all UART's input data are no long read or processed by the console.
 * @return None
 */
void console_disable(void)
{
	console_priv_t *console;

	console = &g_console;
	if (console->state == CONSOLE_STATE_START) {
		HAL_UART_DisableRxCallback(console->uart_id);
	}
}

/**
 * @brief Enable the console's input function
 * @note This function is used to enable the console's input function if it is
 *       disabled before. The console's input function is enable by default
 *       after starting.
 * @return None
 */
void console_enable(void)
{
	console_priv_t *console;
	UART_T *uart;

	console = &g_console;
	if (console->state == CONSOLE_STATE_START) {
		uart = HAL_UART_GetInstance(console->uart_id);
		HAL_UART_EnableRxCallback(console->uart_id, console_rx_callback, uart);
	}
}

/**
 * @brief Get the console's related UART ID
 * @retval UART_ID, UART_INVALID_ID on failure
 */
UART_ID console_get_uart_id(void)
{
	console_priv_t *console;

	console = &g_console;
	if (console->state == CONSOLE_STATE_START) {
		return console->uart_id;
	} else {
		return UART_INVALID_ID;
	}
}
