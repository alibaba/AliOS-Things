/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef YUNOS_BSP_POW_H
#define YUNOS_BSP_POW_H

#include <stdint.h>

typedef enum
{
    FLAG_USB = 0,
	FLAG_MODEM,
    FLAG_I2C,
    FLAG_UART,
    FLAG_SD,
    FLAG_MAX
}drv_sleep_flag;  


/*AP睡眠模式 */
typedef enum
{

    USR_AP_DEF_MODE = 0,    
    USR_AP_RUN_MODE,    
    USR_AP_WFI_MODE,    
    USR_AP_SLEEP_MODE,
    USR_AP_DRX1_MODE,
    USR_AP_DRX2_MODE,
    USR_AP_DRX3_MODE,
    USR_AP_PSM_MODE
}AP_SLEEP_MODE;

uint32_t drv_sleep_flag_check(void);//驱动休眠标记判断, 驱动提供  todo
uint32_t sys_standby_mode_check(void);
uint32_t pow_kernel_sleep(void); //其它模式都不满足进入到内核省电
void pow_set_lpm_wakeup_timer(uint64_t SleepTime); //lpm睡眠时间设置
uint32_t drv_pow_sleep_func(AP_SLEEP_MODE sleep_mode, uint32_t sleep_time,uint32_t ap_sleep_start);
uint32_t read_cnt_by_lpm(void);

void set_rtc_wakeup_time(uint32_t sleep_time);

void drv_sleep_flag_set(drv_sleep_flag dev_id);

void drv_sleep_flag_clr(drv_sleep_flag dev_id);

uint32_t get_rtc_sleeptime(void);

void set_rtc_wakeup_time(uint32_t sleep_time);

#endif /* YUNOS_BSP_POW_H */
