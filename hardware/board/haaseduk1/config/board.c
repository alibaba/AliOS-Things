#include "board.h"
#include "aos/hal/uart.h"
#include "aos/hal/i2c.h"
#include "cmsis.h"
#include "cmsis_os.h"
#include "hal_gpio.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include <k_api.h>
#if (RHINO_CONFIG_HW_COUNT > 0)
#include "haas1000.h"
#endif
#define SYSTICK_USE_FASTTIMER

uart_dev_t uart_0;
uint8_t g_haasedu_boardname = HAAS_EDU_K1C;

extern void haas_wifi_init();

#if (RHINO_CONFIG_HW_COUNT > 0)
void soc_hw_timer_init(void)
{
    /* Enable DWT */
    CoreDebug->DEMCR |= ((1 << CoreDebug_DEMCR_TRCENA_Pos));
    DWT->CYCCNT = 0;
    /* Enable CPU cycle counter */
    DWT->CTRL |= ((1 << DWT_CTRL_CYCCNTENA_Pos));
}

hr_timer_t soc_hr_hw_cnt_get(void)
{
    return DWT->CYCCNT;
}

lr_timer_t soc_lr_hw_cnt_get(void)
{
    return (lr_timer_t)hal_sys_timer_get();
}

float soc_hr_hw_freq_mhz(void)
{
    return hal_sys_timer_calc_cpu_freq(5, 0) / 1000000;
}

#endif /* RHINO_CONFIG_HW_COUNT */

#if (RHINO_CONFIG_INTRPT_STACK_OVF_CHECK > 0)
void soc_intrpt_stack_ovf_check(void) {}
#endif

#if (RHINO_CONFIG_DYNTICKLESS > 0)
void soc_tick_interrupt_set(tick_t next_ticks, tick_t elapsed_ticks) {}

tick_t soc_elapsed_ticks_get(void)
{
    return 0;
}
#endif

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
#define RHINO_HEAP_BLK_SIZE (0x680000)
__SRAM_EXT_BSS uint32_t heap_blk0[RHINO_HEAP_BLK_SIZE / sizeof(uint32_t)] = {0};
// __SRAM_EXT_BSS uint32_t heap_blk1[RHINO_HEAP_BLK_SIZE/sizeof(uint32_t)] = {0};
k_mm_region_t g_mm_region[] = {
    {(uint8_t *)heap_blk0, RHINO_HEAP_BLK_SIZE}
    // {(uint8_t *)heap_blk1, RHINO_HEAP_BLK_SIZE}
};
int g_region_num = sizeof(g_mm_region) / sizeof(k_mm_region_t);

void soc_sys_mem_init(void) {}
#endif

void soc_err_proc(kstat_t err) {}

krhino_err_proc_t g_err_proc = soc_err_proc;

#define OS_CLOCK OS_CLOCK_NOMINAL

#define SYS_TICK_LOAD                                                          \
    ((uint32_t)((((float)OS_CLOCK * (float)RHINO_CONFIG_TICKS_PER_SECOND)) /   \
                    (float)1E6 +                                               \
                0.5f))

__STATIC_INLINE uint32_t SysTick_Config_Alt(uint32_t ticks)
{
    if ((ticks - 1UL) > SysTick_LOAD_RELOAD_Msk) {
        return (1UL); /* Reload value impossible */
    }

    SysTick->LOAD = (uint32_t)(ticks - 1UL); /* set reload register */
    NVIC_SetPriority(SysTick_IRQn,
                     (1UL << __NVIC_PRIO_BITS) -
                     1UL); /* set Priority for Systick Interrupt */
    SysTick->VAL = 0UL;        /* Load the SysTick Counter Value */
    SysTick->CTRL =
        SysTick_CTRL_TICKINT_Msk |
        SysTick_CTRL_ENABLE_Msk; /* Enable SysTick IRQ and SysTick Timer */
    return (0UL);                /* Function successful */
}

void SysTick_Handler(void)
{
    krhino_intrpt_enter();
    krhino_tick_proc();
    krhino_intrpt_exit();
}

void soc_systick_init(void)
{
#ifdef SYSTICK_USE_FASTTIMER
    hal_systick_timer_open(RHINO_CONFIG_TICKS_PER_SECOND, SysTick_Handler);
#else
    SysTick_Config_Alt(SYS_TICK_LOAD);
#endif
}

void soc_systick_start(void)
{
#ifdef SYSTICK_USE_FASTTIMER
    hal_systick_timer_start();
#else
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
#endif
}

void soc_systick_stop(void)
{
#ifdef SYSTICK_USE_FASTTIMER
    hal_systick_timer_stop();
#else
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
#endif
}

void board_stduart_init()
{
    uart_0.port                = 0;
    uart_0.config.baud_rate    = 1500000;
    uart_0.config.data_width   = DATA_WIDTH_8BIT;
    uart_0.config.flow_control = FLOW_CONTROL_DISABLED;
    uart_0.config.mode         = MODE_TX_RX;
    uart_0.config.parity       = NO_PARITY;
    uart_0.config.stop_bits    = STOP_BITS_1;

    hal_uart_init(&uart_0);
}

int haasedu_is_k1c()
{
    int value = 0;
    int len = 4;

    if (0 == aos_kv_get("HAASEDU_NAME", &value, &len)) {
        printf("HAASEDU_NAME is %d\r\n", value);
        if (value == 1) {
            printf("3333 is %d\r\n", value);
            return 1;
        } else {
            return 0;
        }
    } else {
        printf("HAASEDU_NAME is avild %d!\r\n", value);
        return 0;
    }
}

void board_detect()
{
    int re_value = 0;
    int len = 4;
    uint8_t chip_id = 0x00;

    int32_t ret = sensor_i2c_open(1, 0x6b, I2C_BUS_BIT_RATES_100K, 0);
    if (ret) {
        printf("sensor i2c open failed, ret:%d\n", ret);
        return;
    }

    FisImu_read_reg(0, &chip_id, 1);
    aos_msleep(100);
    sensor_i2c_close(1);

    if (chip_id == 0xfc) {
        g_haasedu_boardname = HAAS_EDU_K1C;
        re_value = 1;
        aos_kv_set("HAASEDU_NAME", &re_value, len, 1);
        printf("haasedu name is K1C\n");
    } else {
        g_haasedu_boardname = HAAS_EDU_K1;
        re_value = 0;
        aos_kv_set("HAASEDU_NAME", &re_value, len, 1);
        printf("haasedu name is K1\n");
    }
    printf("haasedu detected is %d\n", g_haasedu_boardname);

    return;
}

void board_dma_init() {}

void board_gpio_init() {}

void board_tick_init() {}

void board_kinit_init() {}

void board_flash_init() {}

void board_network_init()
{
    haas_wifi_init();
}
