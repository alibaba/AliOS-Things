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

#ifdef CONFIG_PM
/* pm config l=<Test_Level> d=<Delay_ms>
 *  <Test_Level>: TEST_NONE ~ TEST_DEVICES.
 *  <Delay_ms>: based on ms.
 */
static enum cmd_status cmd_pm_config_exec(char *cmd)
{
	int32_t cnt;
	uint32_t level, delayms;

	cnt = cmd_sscanf(cmd, "l=%d d=%d", &level, &delayms);
	if (cnt != 2 || level > __TEST_AFTER_LAST || delayms > 100) {
		CMD_ERR("err cmd:%s, expect: l=<Test_Level> d=<Delay_ms>\n", cmd);
		return CMD_STATUS_INVALID_ARG;
	}

	pm_set_test_level(level);
	pm_set_debug_delay_ms(delayms);

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

	HAL_Wakeup_SetTimer_mS(wakeup_time * 1000);

	return CMD_STATUS_OK;
}

/* pm wk_io p=<Port_Num> m=<Mode>
 *  <Port_Num>: 0 ~ 9
 *  <Mode>: 0: negative edge, 1: positive edge, 2: disable this port as wakeup io.
 */
static enum cmd_status cmd_pm_wakeupio_exec(char *cmd)
{
	int32_t cnt;
	uint32_t io_num, mode;

	cnt = cmd_sscanf(cmd, "p=%d m=%d", &io_num, &mode);
	if (cnt != 2 || io_num > 9 || mode > 2) {
		CMD_ERR("err cmd:%s, expect: p=<Port_Num> m=<Mode>\n", cmd);
		return CMD_STATUS_INVALID_ARG;
	}

	if (mode < 2) {
		HAL_Wakeup_SetIO(io_num, mode);
	} else {
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

static struct cmd_data g_pm_cmds[] = {
	{ "config",      cmd_pm_config_exec },
	{ "wk_timer",    cmd_pm_wakeuptimer_exec },
	{ "wk_io",       cmd_pm_wakeupio_exec },
	{ "wk_event",    cmd_pm_wakeupevent_exec },
	{ "sleep",       cmd_pm_sleep_exec },
	{ "standby",     cmd_pm_standby_exec },
	{ "hibernation", cmd_pm_hibernation_exec },
	{ "poweroff",    cmd_pm_poweroff_exec },
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
