
#ifndef __DRIVER_UART_IO_SIM_H__

#define __DRIVER_UART_IO_SIM_H__


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */


#define UART_IO_SIM_RX_FIFO_MAX_COUNT           128
#define UART_IO_SIM_TX_FIFO_MAX_COUNT           512

#define UARTCALL   __attribute__((section("uartcall")))

#define LOCAL           static
#define CHIP_7231           6
#define CHIP_ID             CHIP_7231

#if (CHIP_ID == CHIP_7231)

#define MCU_CLK_120MHz      120000000
#define MCU_CLK             MCU_CLK_120MHz

#define PER_CLK_120MHz      120000000
#endif      /* #if (CHIP_ID == CHIP_7211) */

    //#define POLLING_METHOD
#define INTERRUPT_METHOD

//#define INCLUDE_TX_MODE
//#define INCLUDE_RX_MODE
//#define GPIO_7231_STYLE 1


#if defined POLLING_METHOD
#if (MCU_CLK == MCU_CLK_120MHz)
#define DELAY_NOPS_COUNT                102
#define SEND_BYTE_START_DELAY           (DELAY_NOPS_COUNT - 2)
#define RECEIVE_BYTE_START_DELAY        (DELAY_NOPS_COUNT - 11)
#define RECEIVE_BYTE_MIDDLE_DELAY       (DELAY_NOPS_COUNT + 1)
#define RECEIVE_BYTE_CALI_LAST_DELAY    (1)
#define RECEIVE_BYTE_LAST_DELAY         (DELAY_NOPS_COUNT - 2 - RECEIVE_BYTE_CALI_LAST_DELAY)
#define RECEIVE_BYTE_STOP_DELAY         (DELAY_NOPS_COUNT + 6)
#define WATCH_SAMPLE_POINT_RCV_MID_DLY  (DELAY_NOPS_COUNT - 4)
#define WATCH_SAMPLE_POINT_RCV_LST_DLY  (DELAY_NOPS_COUNT - 6)
#endif
#endif


#ifdef INTERRUPT_METHOD
#define UART_IO_SIM_DEFAULT_CLK         MCU_CLK
#define UART_IO_SIM_BAUD_115200         60000000    /*115200*/
#define UART_IO_SIM_DEFAULT_BAUD        UART_IO_SIM_BAUD_115200
#define UART_IO_SIM_CLK_DIVID_SET       271
#define UART_IO_SIM_CLK_DIVID_SET_TX       2710
#define UART_IO_SIM_CLK_FIRST_DELAY_SET       4000
#endif


#ifdef INTERRUPT_METHOD
#define UART_IO_SIM_PWM_TIMER_TX_CHN    PWM1
#define UART_IO_SIM_PWM_TIMER_RX_CHN    PWM2
#endif



#if defined  GPIO_7231_STYLE
#define GPIO_UART_IO_SIM_TX_PIN                 18
#define GPIO_UART_IO_SIM_RX_PIN                 19
#define UART_IO_SIM_PWM_CAP_NEG_RX_CHN          PWM5
#define SIMU_UART_GPIO_TEST                     15
#define PWM5_GPIO_PIN                           GPIO_UART_IO_SIM_RX_PIN

#endif

#if 0//defined GPIO_7231_STYLE
#define GCFG_OUTPUT_POS                              1
#define GCFG_INPUT_BIT                               (1 << 0)
#define GPIO_BASE_ADDR                               (0x0802800)
#define PWM2_GPIO_PIN 8

#define PWM_CTL                                      (PWM_BASE + 0 * 4)
#define ICU_BASE                                     (0x00802000)
#define ICU_PERI_CLK_PWD                             (ICU_BASE + 2 * 4)
#define ICU_PWM_CLK_MUX                              (ICU_BASE + 1 * 4)
#define ICU_INTERRUPT_ENABLE                         (ICU_BASE + 16 * 4)
#define REG_GPIO_FUNC_CFG                            (GPIO_BASE_ADDR + 32*4)
#if (CFG_SOC_NAME == SOC_BK7231)
#define PWM_BASE                                     (0x00802A00)
#define REG_APB_BK_PWMn_CNT_ADDR(n)                  (PWM_BASE + 0x08 + 2 * 0x04 * (n))
#else
#define PWM_NEW_BASE                                 (0x00802A00)
#define PWM_BASE                                     (PWM_NEW_BASE + 0x20 * 4 )
#define REG_APB_BK_PWMn_END_ADDR(n)                  (PWM_BASE + 0x08 + 2 * 0x04 * (n))
#endif
#endif

extern void uart_io_sim_init(void);
extern void uart_io_sim_disable(void);
extern void uart_io_sim_send_byte(u8 data);


#ifdef __cplusplus
}
#endif  /* __cplusplus */


#endif      /* __DRIVER_UART_IO_SIM_H__ */

