// Copyright 2010-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_log.h"
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/clk.h"
#include "esp32/ulp.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/clk.h"
#include "esp32s2/ulp.h"
#endif

#include "soc/soc.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"

#include "ulp_private.h"

typedef struct {
    uint32_t magic;
    uint16_t text_offset;
    uint16_t text_size;
    uint16_t data_size;
    uint16_t bss_size;
} ulp_binary_header_t;

#define ULP_BINARY_MAGIC_ESP32 (0x00706c75)

static const char* TAG = "ulp";

esp_err_t ulp_run(uint32_t entry_point)
{
#if CONFIG_IDF_TARGET_ESP32
    // disable ULP timer
    CLEAR_PERI_REG_MASK(RTC_CNTL_STATE0_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN);
    // wait for at least 1 RTC_SLOW_CLK cycle
    ets_delay_us(10);
    // set entry point
    REG_SET_FIELD(SENS_SAR_START_FORCE_REG, SENS_PC_INIT, entry_point);
    // disable force start
    CLEAR_PERI_REG_MASK(SENS_SAR_START_FORCE_REG, SENS_ULP_CP_FORCE_START_TOP_M);
    // set time until wakeup is allowed to the smallest possible
    REG_SET_FIELD(RTC_CNTL_TIMER5_REG, RTC_CNTL_MIN_SLP_VAL, RTC_CNTL_MIN_SLP_VAL_MIN);
    // make sure voltage is raised when RTC 8MCLK is enabled
    SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BIAS_I2C_FOLW_8M);
    SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BIAS_CORE_FOLW_8M);
    SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BIAS_SLEEP_FOLW_8M);
    // enable ULP timer
    SET_PERI_REG_MASK(RTC_CNTL_STATE0_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN);
#elif defined CONFIG_IDF_TARGET_ESP32S2
    // disable ULP timer
    CLEAR_PERI_REG_MASK(RTC_CNTL_ULP_CP_TIMER_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN);
    // wait for at least 1 RTC_SLOW_CLK cycle
    ets_delay_us(10);
    // set entry point  
    REG_SET_FIELD(RTC_CNTL_ULP_CP_TIMER_REG, RTC_CNTL_ULP_CP_PC_INIT, entry_point);
    
    SET_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_SEL);         // Select ULP_TIMER trigger target for ULP.
    CLEAR_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_DONE_FORCE);  // Select the value for ULP_TIMER sleep. 1: REG_COCPU_DONE;0: ULP END value.
    /* Set the number of cycles of ULP_TIMER sleep, the wait time required to start ULP */
    REG_SET_FIELD(RTC_CNTL_ULP_CP_TIMER_REG, RTC_CNTL_ULP_CP_TIMER_SLP_CYCLE, 100);
    /* Clear interrupt COCPU status */
    REG_WRITE(RTC_CNTL_INT_CLR_REG, RTC_CNTL_COCPU_INT_CLR | RTC_CNTL_COCPU_TRAP_INT_CLR | RTC_CNTL_ULP_CP_INT_CLR);
    // start ULP clock gate.
    SET_PERI_REG_MASK(RTC_CNTL_ULP_CP_CTRL_REG ,RTC_CNTL_ULP_CP_CLK_FO);
    // 1: start with timer. wait ULP_TIMER cnt timer.
    CLEAR_PERI_REG_MASK(RTC_CNTL_ULP_CP_CTRL_REG, RTC_CNTL_ULP_CP_FORCE_START_TOP); // Select ULP_TIMER timer as COCPU trigger source
    SET_PERI_REG_MASK(RTC_CNTL_ULP_CP_TIMER_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN);     // Software to turn on the ULP_TIMER timer
#endif
    return ESP_OK;
}

esp_err_t ulp_load_binary(uint32_t load_addr, const uint8_t* program_binary, size_t program_size)
{
    size_t program_size_bytes = program_size * sizeof(uint32_t);
    size_t load_addr_bytes = load_addr * sizeof(uint32_t);

    if (program_size_bytes < sizeof(ulp_binary_header_t)) {
        return ESP_ERR_INVALID_SIZE;
    }
    if (load_addr_bytes > ULP_RESERVE_MEM) {
        return ESP_ERR_INVALID_ARG;
    }
    if (load_addr_bytes + program_size_bytes > ULP_RESERVE_MEM) {
        return ESP_ERR_INVALID_SIZE;
    }

    // Make a copy of a header in case program_binary isn't aligned
    ulp_binary_header_t header;
    memcpy(&header, program_binary, sizeof(header));

    if (header.magic != ULP_BINARY_MAGIC_ESP32) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    size_t total_size = (size_t) header.text_offset + (size_t) header.text_size +
            (size_t) header.data_size;

    ESP_LOGD(TAG, "program_size_bytes: %d total_size: %d offset: %d .text: %d, .data: %d, .bss: %d",
            program_size_bytes, total_size, header.text_offset,
            header.text_size, header.data_size, header.bss_size);

    if (total_size != program_size_bytes) {
        return ESP_ERR_INVALID_SIZE;
    }

    size_t text_data_size = header.text_size + header.data_size;
    uint8_t* base = (uint8_t*) RTC_SLOW_MEM;

    memcpy(base + load_addr_bytes, program_binary + header.text_offset, text_data_size);
    memset(base + load_addr_bytes + text_data_size, 0, header.bss_size);

    return ESP_OK;
}

esp_err_t ulp_set_wakeup_period(size_t period_index, uint32_t period_us)
{
#if CONFIG_IDF_TARGET_ESP32
    if (period_index > 4) {
        return ESP_ERR_INVALID_ARG;
    }
    uint64_t period_us_64 = period_us;
    uint64_t period_cycles = (period_us_64 << RTC_CLK_CAL_FRACT) / esp_clk_slowclk_cal_get();
    uint64_t min_sleep_period_cycles = ULP_FSM_PREPARE_SLEEP_CYCLES
                                    + ULP_FSM_WAKEUP_SLEEP_CYCLES
                                    + REG_GET_FIELD(RTC_CNTL_TIMER2_REG, RTC_CNTL_ULPCP_TOUCH_START_WAIT);
    if (period_cycles < min_sleep_period_cycles) {
        period_cycles = 0;
        ESP_LOGW(TAG, "Sleep period clipped to minimum of %d cycles", (uint32_t) min_sleep_period_cycles);
    } else {
        period_cycles -= min_sleep_period_cycles;
    }
    REG_SET_FIELD(SENS_ULP_CP_SLEEP_CYC0_REG + period_index * sizeof(uint32_t),
            SENS_SLEEP_CYCLES_S0, (uint32_t) period_cycles);
#elif defined CONFIG_IDF_TARGET_ESP32S2
    if (period_index > 4) {
        return ESP_ERR_INVALID_ARG;
    }
    uint64_t period_us_64 = period_us;
    uint64_t period_cycles = (period_us_64 * 1000) / 90;  //COCPU sleep clock is 90KHZ.
    REG_SET_FIELD(RTC_CNTL_ULP_CP_TIMER_REG, RTC_CNTL_ULP_CP_TIMER_SLP_CYCLE, period_cycles);
#endif
    return ESP_OK;
}
