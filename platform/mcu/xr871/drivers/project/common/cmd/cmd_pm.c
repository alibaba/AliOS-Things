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

#include "cmd_util.h"
#include "cmd_pm.h"
#include "pm/pm.h"
#include "driver/chip/hal_wakeup.h"
#include "driver/chip/hal_prcm.h"

#ifdef CONFIG_PM
extern void uart_set_suspend_record(unsigned int len);

/* pm config l=<Test_Level> d=<Delay_ms> u=<Buffer_len>
 *  <Test_Level>: TEST_NONE ~ TEST_DEVICES.
 *  <Delay_ms>: based on ms.
 *  <Buffer_len>: buffer data len when uart suspend.
 *
 * eg. pm config l=0 d=0 u=1024
 */
static enum cmd_status cmd_pm_config_exec(char *cmd)
{
	int32_t cnt;
	uint32_t level, delayms, len = 0;

	cnt = cmd_sscanf(cmd, "l=%d d=%d u=%d", &level, &delayms, &len);
	if (cnt != 3 || level > __TEST_AFTER_LAST || delayms > 100) {
		CMD_ERR("err cmd:%s, expect: l=<Test_Level> d=<Delay_ms> "
		        "u=<Buffer_len>\n", cmd);
		return CMD_STATUS_INVALID_ARG;
	}

	pm_set_test_level(level);
	pm_set_debug_delay_ms(delayms);
	uart_set_suspend_record(len);

	return CMD_STATUS_OK;
}

/* pm dump <0xAddr> <Len> <Idx>
 *  <Addr>: addres to dump.
 *  <Len>: length to dump.
 *  <Idx>: idx to dump.
 *
 * eg. pm dump 0x40040000 64 0
 */
static enum cmd_status cmd_pm_dump_exec(char *cmd)
{
	int32_t cnt;
	uint32_t addr, len, idx;

	cnt = cmd_sscanf(cmd, "0x%x %d %d", &addr, &len, &idx);
	if (cnt != 3) {
		CMD_ERR("err cmd:%s, expect: <0xAddr> <Len> <Idx>\n", cmd);
		return CMD_STATUS_INVALID_ARG;
	}

	pm_set_dump_addr(addr, len, idx);

	return CMD_STATUS_OK;
}

/* pm check
 */
static enum cmd_status cmd_pm_check_exec(char *cmd)
{
	if (!HAL_Wakeup_CheckIOMode()) {
		CMD_LOG(1, "some wakeup io not EINT mode\n");
	} else {
		CMD_LOG(1, "all wakeup io is EINT mode\n");
	}

	return CMD_STATUS_OK;
}

/* pm wk_timer <Seconds>
 *  <Seconds>: seconds.
 */
static enum cmd_status cmd_pm_wakeuptimer_exec(char *cmd)
{
	int32_t cnt;
	uint32_t wakeup_time;

	cnt = cmd_sscanf(cmd, "%d", &wakeup_time);
	if (cnt != 1 || wakeup_time < 1) {
		CMD_ERR("err cmd:%s, expect: <Seconds>\n", cmd);
		return CMD_STATUS_INVALID_ARG;
	}

	HAL_Wakeup_SetTimer_Sec(wakeup_time);

	return CMD_STATUS_OK;
}

static void key_IrqCb(void *arg)
{
	printf("%s,%d\n", __func__, __LINE__);
}

/* pm wk_io p=<Port_Num> m=<Mode> p=<Pull>
 *  <Port_Num>: 0 ~ 9
 *  <Mode>: 0: negative edge, 1: positive edge, 2: disable this port as wakeup io.
 *  <Pull>: 0: no pull, 1: pull up, 2: pull down.
 */
static enum cmd_status cmd_pm_wakeupio_exec(char *cmd)
{
	int32_t cnt;
	uint32_t io_num, mode, pull;

	cnt = cmd_sscanf(cmd, "p=%d m=%d p=%d", &io_num, &mode, &pull);
	if (cnt != 3 || io_num >= WAKEUP_IO_MAX || mode > 2 || pull > GPIO_CTRL_PULL_MAX) {
		CMD_ERR("err cmd:%s, expect: p=<Port_Num> m=<Mode> p=<Pull>\n", cmd);
		return CMD_STATUS_INVALID_ARG;
	}

	if (mode < 2) {
		GPIO_InitParam param;
		GPIO_IrqParam Irq_param;

		param.mode = GPIOx_Pn_F6_EINT;
		param.driving = GPIO_DRIVING_LEVEL_1;
		param.pull = pull;
		HAL_GPIO_Init(GPIO_PORT_A, WakeIo_To_Gpio(io_num), &param);

		Irq_param.event = GPIO_IRQ_EVT_BOTH_EDGE;
		Irq_param.callback = key_IrqCb;
		Irq_param.arg = (void *)0;
		HAL_GPIO_EnableIRQ(GPIO_PORT_A, WakeIo_To_Gpio(io_num), &Irq_param);
		HAL_Wakeup_SetIO(io_num, mode, pull);
	} else {
		HAL_GPIO_DeInit(GPIO_PORT_A, WakeIo_To_Gpio(io_num));
		HAL_GPIO_DisableIRQ(GPIO_PORT_A, WakeIo_To_Gpio(io_num));
		HAL_Wakeup_ClrIO(io_num);
	}

	return CMD_STATUS_OK;
}

/* pm wk_event
 */
static enum cmd_status cmd_pm_wakeupevent_exec(char *cmd)
{
	uint32_t event;

	event = HAL_Wakeup_GetEvent();
	CMD_LOG(1, "wakeup event:%x\n", event);

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_pm_wakeupread_exec(char *cmd)
{
	CMD_LOG(1, "wakeup io:%x timer:%x\n", HAL_Wakeup_ReadIO(),
	        HAL_Wakeup_ReadTimerPending());

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_pm_sleep_exec(char *cmd)
{
	pm_enter_mode(PM_MODE_SLEEP);

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_pm_standby_exec(char *cmd)
{
	pm_enter_mode(PM_MODE_STANDBY);

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_pm_hibernation_exec(char *cmd)
{
	pm_enter_mode(PM_MODE_HIBERNATION);

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_pm_poweroff_exec(char *cmd)
{
	pm_enter_mode(PM_MODE_POWEROFF);

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_pm_net_prepare_exec(char *cmd)
{
	pm_set_sync_magic();
	HAL_PRCM_AllowCPUNDeepSleep();
	return CMD_STATUS_OK;
}

static struct cmd_data g_pm_cmds[] = {
	{ "config",      cmd_pm_config_exec },
	{ "dump",        cmd_pm_dump_exec },
	{ "wk_check",    cmd_pm_check_exec },
	{ "wk_timer",    cmd_pm_wakeuptimer_exec },
	{ "wk_io",       cmd_pm_wakeupio_exec },
	{ "wk_event",    cmd_pm_wakeupevent_exec },
	{ "wk_read",     cmd_pm_wakeupread_exec },
	{ "sleep",       cmd_pm_sleep_exec },
	{ "standby",     cmd_pm_standby_exec },
	{ "hibernation", cmd_pm_hibernation_exec },
	{ "poweroff",    cmd_pm_poweroff_exec },
	{ "net_prepare", cmd_pm_net_prepare_exec },
	{ "shutdown",    cmd_pm_poweroff_exec },
};

enum cmd_status cmd_pm_exec(char *cmd)
{
	return cmd_exec(cmd, g_pm_cmds, cmd_nitems(g_pm_cmds));
}
#else /* CONFIG_PM */
enum cmd_status cmd_pm_exec(char *cmd)
{
	return CMD_STATUS_FAIL;
}
#endif /* CONFIG_PM */
