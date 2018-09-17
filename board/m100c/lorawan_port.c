/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <hal/soc/soc.h>

#include "em_device.h"
#include "em_cmu.h"
#include "em_rtc.h"
#include "em_common.h"
#include "em_gpio.h"

#include "lorawan_port.h"
#include "timer.h"
#include "debug.h"
#include "low_power.h"

// Assume 32kHz RTC/RTCC clock, cmuClkDiv_2 prescaler, 16 ticks per millisecond
#define RTC_DIVIDER                     ( cmuClkDiv_1 )
#define RTC_CLOCK                       ( 32768U )
#define MSEC_TO_TICKS_DIVIDER           ( 1000U * RTC_DIVIDER )
#define MSEC_TO_TICKS_ROUNDING_FACTOR   ( MSEC_TO_TICKS_DIVIDER / 2 )
#define MSEC_TO_TICKS( ms )             ( ( ( (uint64_t)(ms) * RTC_CLOCK )    \
                                            + MSEC_TO_TICKS_ROUNDING_FACTOR ) \
                                          / MSEC_TO_TICKS_DIVIDER )

#define TICKS_TO_MSEC_ROUNDING_FACTOR   ( RTC_CLOCK / 2 )
#define TICKS_TO_MSEC( ticks )          ( ( ( (uint64_t)(ticks)               \
                                              * RTC_DIVIDER * 1000U )         \
                                            + TICKS_TO_MSEC_ROUNDING_FACTOR ) \
                                          / RTC_CLOCK )

#define TIMEDIFF( a, b )              ((( (a)<<8) - ((b)<<8) ) >> 8 )
#define RTC_COUNTERGET()              RTC_CounterGet()
#define RTC_MAX_VALUE                 (_RTC_CNT_MASK)
#define RTC_CLOSE_TO_MAX_VALUE        (RTC_MAX_VALUE-100UL)

#define RTC_COMP_INT                  RTC_IF_COMP0
#define RTC_INTCLEAR(x)               RTC_IntClear(x)
#define RTC_INTENABLE(x)              RTC_IntEnable(x)
#define RTC_COMPARESET( x )           RTC_CompareSet(0, (x) & _RTC_COMP0_MASK)

#define RTC_MIN_TIMEOUT               3

static uint32_t g_rtc_ticks;

static void enter_stop_mode(void)
{
}

static void exit_stop_mode(void)
{
}

static void enter_sleep_mode(void)
{
    /* enter EM2 Mode for low power */
    EMU_EnterEM2(false);
}

static void delay_ticks(uint32_t ticks)
{
    uint32_t start_time;
    volatile uint32_t now;

    if (ticks) {
        start_time = RTC_COUNTERGET();
        do {
        now = RTC_COUNTERGET();
        } while (TIMEDIFF(now, start_time) < ticks);
    }
}

static void delay_ms(time_ms_t delay)
{
    uint32_t total_ticks;

    total_ticks = MSEC_TO_TICKS(delay);

    while (total_ticks > RTC_CLOSE_TO_MAX_VALUE) {
        delay_ticks(RTC_CLOSE_TO_MAX_VALUE);
        total_ticks -= RTC_CLOSE_TO_MAX_VALUE;
    }

    delay_ticks(total_ticks);
}

static uint32_t set_timer_context(void)
{
    g_rtc_ticks = RTC_COUNTERGET();
    return g_rtc_ticks;
}

static uint32_t get_timer_context(void)
{
    return g_rtc_ticks;
}

static uint32_t get_delta_context(uint32_t now, uint32_t old)
{
    uint32_t delta_ticks;

    if (now >= old) {
        delta_ticks = now - old;
    } else {
        delta_ticks = (now + (RTC_MAX_VALUE + 1)) - old;
    }

    return delta_ticks;
}

static uint32_t get_timer_elapsed_time(void)
{
    uint32_t ticks = RTC_COUNTERGET();
    uint32_t elapsed_time;

    if (ticks >= g_rtc_ticks) {
        elapsed_time = ticks - g_rtc_ticks;
    } else {
        elapsed_time = (ticks + 0x1000000 - g_rtc_ticks);
    }

    return elapsed_time;
}

static void set_uc_wakeup_time(void)
{
}

static void set_alarm(uint32_t timeout)
{
    uint32_t comp_val;
    uint32_t cnt;
    uint32_t ticks = timeout;

    RTC_INTCLEAR(RTC_COMP_INT);

    cnt = get_timer_context();

    cnt += ticks;

    if (cnt > RTC_MAX_VALUE) {
        cnt -= (RTC_MAX_VALUE + 1);
    }

    RTC_COMPARESET(cnt);

    RTC_INTENABLE(RTC_COMP_INT);

    LowPower_Disable(e_LOW_POWER_RTC);
}

static void stop_alarm(void)
{
    RTC_IntDisable(RTC_COMP_INT);
}

static void set_timer_val(TimerEvent_t *obj, uint32_t value)
{
    uint32_t min_value = 0;
    uint32_t ticks = MSEC_TO_TICKS(value);

    min_value = RTC_MIN_TIMEOUT;

    if (ticks < min_value) {
        ticks = min_value;
    }

    obj->Timestamp = ticks;
    obj->ReloadValue = ticks;
}

static TimerTime_t get_current_time(void)
{
    uint32_t ticks = RTC_COUNTERGET();
    return TICKS_TO_MSEC(ticks);
}

static TimerTime_t compute_elapsed_time(TimerTime_t time)
{
    uint32_t now_in_ticks = RTC_COUNTERGET();
    uint32_t past_in_ticks = MSEC_TO_TICKS((uint32_t)time);
    return TICKS_TO_MSEC(now_in_ticks - past_in_ticks);
}

static void set_timeout(TimerEvent_t *obj)
{
    int32_t minTicks = RTC_MIN_TIMEOUT;

    obj->IsRunning = true;

    //in case deadline too soon
    if(obj->Timestamp  < (aos_lrwan_time_itf.get_timer_elapsed_time() + minTicks)) {
        obj->Timestamp = aos_lrwan_time_itf.get_timer_elapsed_time() + minTicks;
    }

    aos_lrwan_time_itf.set_alarm(obj->Timestamp);
}

static void radio_reset(void)
{
    GPIO_PinModeSet(gpioPortF, 2, gpioModePushPull, 0);    //rx
    GPIO_PinOutClear(gpioPortF, 2);
}

static void radio_reset_cfg_input(void)
{
    GPIO_PinModeSet(gpioPortF, 2, gpioModeInput, 0);    //rx
}

static void radio_rw_en(void)
{
    GPIO_PinModeSet(gpioPortE, 13, gpioModePushPull, 1);
    GPIO_PinOutClear(gpioPortE, 13);
}

static void radio_rw_dis(void)
{
    GPIO_PinModeSet(gpioPortE, 13, gpioModePushPull, 1);
    GPIO_PinOutSet(gpioPortE, 13);
}

static uint16_t radio_rw(uint16_t tx_data)
{
    return lora_spi_inout(tx_data);
}

static uint8_t get_battery_level(void)
{
    return 0;
}

static void get_unique_id(uint8_t *id)
{
    *id = 0;
}

static uint32_t get_random_seed(void)
{
    return 0;
}

static uint32_t get_mft_id(void)
{
    return 0x1234;
}

static uint32_t get_mft_model(void)
{
    return 0x4321;
}

static uint32_t get_mft_rev(void)
{
    return 0x0001;
}

static uint32_t get_mft_sn(void)
{
    return 0xFFFF;
}

static bool set_mft_baud(uint32_t baud)
{
    return false;
}

static uint32_t get_mft_baud(void)
{
    return 115200;
}

extern uart_dev_t uart_1;
int linkwan_serial_output(uint8_t *buffer, int len)
{
    hal_uart_send(&uart_1, buffer, len, 1000);
    return len;
}

/* the struct is for changing the device working mode */
hal_lrwan_dev_chg_mode_t aos_lrwan_chg_mode = {
    .enter_stop_mode  = enter_stop_mode,
    .exit_stop_mode   = exit_stop_mode, 
    .enter_sleep_mode = enter_sleep_mode,
};

/* LoRaWan time and timer interface */
hal_lrwan_time_itf_t aos_lrwan_time_itf = {
    .delay_ms = delay_ms,
    .set_timer_context = set_timer_context,
    .get_timer_context = get_timer_context,
    .get_delta_context = get_delta_context,
    .get_timer_elapsed_time = get_timer_elapsed_time,

    .stop_alarm = stop_alarm,
    .set_alarm = set_alarm,
    .set_uc_wakeup_time = set_uc_wakeup_time,

    .set_timeout = set_timeout,
    .compute_elapsed_time = compute_elapsed_time,
    .get_current_time = get_current_time,
    .set_timer_val = set_timer_val,
};

/* LoRaWan radio control */
hal_lrwan_radio_ctrl_t aos_lrwan_radio_ctrl = {
    .radio_reset = radio_reset,
    .radio_reset_cfg_input = radio_reset_cfg_input,
    .radio_rw_en = radio_rw_en,
    .radio_rw_dis = radio_rw_dis,
    .radio_rw = radio_rw,
};

hal_lrwan_sys_t aos_lrwan_sys = {
    .get_battery_level = get_battery_level,
    .get_unique_id = get_unique_id,
    .get_random_seed = get_random_seed,
};

/* LoraWan manufactory interface*/
hal_manufactory_itf_t aos_mft_itf = {
    .get_mft_id = get_mft_id,
    .get_mft_model = get_mft_model,
    .get_mft_rev = get_mft_rev,
    .get_mft_sn = get_mft_sn,
    .set_mft_baud = set_mft_baud,
    .get_mft_baud = get_mft_baud,
};

uint32_t get_rtc_counter(void)
{
    return RTC_COUNTERGET();
}
