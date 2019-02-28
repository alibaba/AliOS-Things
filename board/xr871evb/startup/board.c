/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "driver/chip/hal_def.h"
#include "driver/chip/hal_adc.h"
#include "driver/chip/hal_chip.h"
#include "aos/hal/adc_priv.h"

#include <k_api.h>
#include <k_soc.h>

#include "aos/hal/gpio.h"
#include "aos/hal/adc.h"
#include "aos/kernel.h"
#include "aos/yloop.h"

#define BOARD_AKEY   (1) //adc key ch 1
#define BOARD_DKEY0  (6) //dkey 0
#define BOARD_DKEY1  (7) //dkey 1

struct board_dkey {
    gpio_dev_t gpio;
    uint64_t   time;
};
static struct board_dkey board_dkeys[2];

static void board_dkey_irq_callback(void* arg)
{
    struct board_dkey *key = arg;
    uint32_t level;
    uint64_t diff;
    uint16_t key_code;

    hal_gpio_input_get(key, &level);
    //printf("key[%d] event: %d\n", key->gpio.port, level);

    if (level == 0 && key->time == 0) {
        key->time = aos_now_ms();
    } else {
        diff = aos_now_ms() - key->time;
        if (key->gpio.port == BOARD_DKEY0)
            key_code = CODE_BOOT;
        if (key->gpio.port == BOARD_DKEY1)
            key_code = CODE_VOLUME;

        if (diff > 6000) { /*long long press */
            key->time = 0;
            aos_post_event(EV_KEY, key_code, VALUE_KEY_LLTCLICK);
        } else if (diff > 2000) { /* long press */
            key->time = 0;
            aos_post_event(EV_KEY, key_code, VALUE_KEY_LTCLICK);
        } else if (diff > 40) { /* short press */
            key->time = 0;
            aos_post_event(EV_KEY, key_code, VALUE_KEY_CLICK);
        }
    }
}

void dkey_init(void)
{
    printf("gpio key init...\n");
    memset(&board_dkeys[0], 0, sizeof(struct board_dkey));
    board_dkeys[0].gpio.port = BOARD_DKEY0;
    hal_gpio_enable_irq(&board_dkeys[0].gpio, IRQ_TRIGGER_BOTH_EDGES, board_dkey_irq_callback, &board_dkeys[0]);

    memset(&board_dkeys[1], 0, sizeof(struct board_dkey));
    board_dkeys[1].gpio.port = BOARD_DKEY1;
    hal_gpio_enable_irq(&board_dkeys[1].gpio, IRQ_TRIGGER_BOTH_EDGES, board_dkey_irq_callback, &board_dkeys[1]);
}

struct board_akey {
    adc_dev_t adc;
    uint64_t   time;
    uint32_t   sample_count;
    uint32_t   avg_val;
};
static struct board_akey board_adc_key;
#define AKEY12_DOWN_TOGETHER_VOL    (300)
#define AKEY1_DOWN_VOL    (1000)  // 0.5V
#define AKEY2_DOWN_VOL    (2000) // 1.05V
#define AKEY3_DOWN_VOL    (3000) // 1.65V
#define AKEY_UP_VOL       (3800)

static void board_akey_irq_callback(void* arg)
{
    struct board_akey *akey = arg;
    adc_priv_config_t *adc_priv_cfg = (adc_priv_config_t *)akey->adc.priv;
    uint32_t level;
    uint64_t diff;
    uint16_t key_code;

    hal_adc_value_get(&akey->adc, &level, 0xffffffff);

    if (level < AKEY3_DOWN_VOL) {
        if (akey->sample_count == 0 && akey->time == 0) {
            akey->avg_val = 0;
            akey->time = aos_now_ms();
        }
        akey->avg_val += level;
        akey->sample_count++;
        if (akey->sample_count == 10) {
            HAL_ADC_ConfigChannel(akey->adc.port, ADC_SELECT_ENABLE,
                        ADC_IRQMODE_HIGH, adc_priv_cfg->irqlow, adc_priv_cfg->irqhigh);
        }
    } else if (level > AKEY_UP_VOL) {
        HAL_ADC_ConfigChannel(akey->adc.port, ADC_SELECT_ENABLE,
                        ADC_IRQMODE_LOW, adc_priv_cfg->irqlow, adc_priv_cfg->irqhigh);
        diff = aos_now_ms() - akey->time;
        // akey up, calc the average voltage
        akey->avg_val = akey->avg_val / akey->sample_count;
        printf("avg %d, count %lu\n", akey->avg_val, akey->sample_count);
        if (akey->avg_val < AKEY12_DOWN_TOGETHER_VOL) {
            key_code = CODE_ELINK;
        } else if (akey->avg_val < AKEY1_DOWN_VOL) {
            key_code = CODE_NEXT;
        } else if (akey->avg_val < AKEY2_DOWN_VOL) {
            key_code = CODE_RECORD_PRE;
        } else if (akey->avg_val < AKEY3_DOWN_VOL) {
            key_code = CODE_PLAY_PAUSE;
        }

        if (diff > 6000) { /*long long press */
            akey->time = 0;
            akey->sample_count = 0;
            aos_post_event(EV_KEY, key_code, VALUE_KEY_LLTCLICK);
        } else if (diff > 2000) { /* long press */
            akey->time = 0;
            akey->sample_count = 0;
            aos_post_event(EV_KEY, key_code, VALUE_KEY_LTCLICK);
        } else if (diff > 40) { /* short press */
            akey->time = 0;
            akey->sample_count = 0;
            aos_post_event(EV_KEY, key_code, VALUE_KEY_CLICK);
        }
    }
}

int adc_key_init(void)
{
    HAL_Status status = HAL_ERROR;
    ADC_InitParam initParam;
    uint32_t akey_low_vol = 1650 + 100;  // 1650mV
    uint32_t akey_high_vol = 3000; // 3000mV
    static adc_priv_config_t adc_priv_cfg = {0};

    initParam.delay = 10;
    initParam.freq = 500000; // default 500K clock for adc controller
    status = HAL_ADC_Init(&initParam);
    if (status != HAL_OK) {
        printf("[adc_init]: adc init error\n");
        return -1;
    }

    memset(&board_adc_key, 0, sizeof(struct board_akey));
    board_adc_key.adc.port = BOARD_AKEY;
    board_adc_key.adc.config.sampling_cycle = 500000; // nothing
    board_adc_key.adc.priv = &adc_priv_cfg;
    adc_priv_cfg.irqmode = ADC_IRQMODE_LOW;
    adc_priv_cfg.irqlow = AKEY3_DOWN_VOL; // x/4096 == vol / 2.5
    adc_priv_cfg.irqhigh = AKEY_UP_VOL;
    adc_priv_cfg.irq_callback = board_akey_irq_callback;
    adc_priv_cfg.arg = &board_adc_key;
    hal_adc_init(&board_adc_key.adc);

    status = HAL_ADC_Start_Conv_IT();
    if (status != HAL_OK) {
        printf("[adc_init]: ADC it mode start error %d\n", status);
        return;
    }
    return 0;
}

void board_driver_init(void)
{
    printf("xr871evb board init...\n");
    dkey_init();
    adc_key_init();

    ssd1306_init();
    oled_clear_screen();
    oled_draw_string_1608(0, 0, "Hello World!!");
    oled_draw_string_1608(0, 2, "123456789");

    return;
}

void soc_driver_init(void)
{
    hal_init();

    printf("soc_driver_init done\n");
}

void soc_system_init(void)
{
    printf("%s %d\n", __func__, __LINE__);
    platform_init();
}

void soc_systick_init(void)
{
    /* enable system tick */
    extern uint32_t SystemCoreClock;
    SysTick_Config(SystemCoreClock/RHINO_CONFIG_TICKS_PER_SECOND);
}

void SysTick_Handler(void)
{
    krhino_intrpt_enter();
    krhino_tick_proc();
    krhino_intrpt_exit();
}

void board_init(void)
{
    soc_sys_mem_init();

    soc_systick_init();   
}
