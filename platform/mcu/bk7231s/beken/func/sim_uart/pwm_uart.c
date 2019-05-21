#include <stdio.h>
#include "include.h"
#include "arm_arch.h"
#include "uart_pub.h"
#include "uart.h"
#include "pwm_pub.h"
#include "drv_model_pub.h"
#include "sys_ctrl_pub.h"
#include "mem_pub.h"
#include "icu_pub.h"
#include "gpio_pub.h"
#include "mem_pub.h"
#include "intc_pub.h"
#include "pwm_uart.h"

uint8_t uart_io_start ;

#ifdef INCLUDE_RX_MODE
volatile BOOL uart_io_sim_rx_done = false;
unsigned char uart_io_sim_rx_buf[UART_IO_SIM_RX_FIFO_MAX_COUNT];
volatile unsigned char uart_io_sim_rx_buf_current_cnt;
#ifdef INTERRUPT_METHOD
LOCAL volatile unsigned char uart_io_sim_rx_buf_current_bit;
LOCAL volatile unsigned int uart_io_sim_rx_int_count = 0;
LOCAL volatile unsigned char uart_io_sim_rx_firt_start_bit = 0;

#endif
#endif
LOCAL unsigned char *p_uart_io_sim_tx_buf = NULL;

#ifdef INCLUDE_TX_MODE
unsigned char uart_io_sim_tx_buf[UART_IO_SIM_TX_FIFO_MAX_COUNT];
#ifdef INTERRUPT_METHOD
LOCAL int uart_io_sim_tx_buf_size;
LOCAL unsigned char uart_io_sim_tx_buf_current_bit;

LOCAL unsigned char uart_io_sim_tx_first_start = 0;

LOCAL unsigned long uart_io_sim_tx_buf_current_cnt;
#endif
#endif

#if defined GPIO_7231_STYLE
extern UART_S uart[2];
extern void (*p_PWM_Int_Handler[])(UINT8);

void GPIO_UART_io_sim_tx_function_enable(void)
{
    *((volatile UINT32 *)(GPIO_BASE_ADDR + GPIO_UART_IO_SIM_TX_PIN * 4)) = 0x0;
}

void UARTCALL uart_io_sim_set_tx(unsigned long ulValue)
{
    if(ulValue)
    {
        *((volatile UINT32 *)(GPIO_BASE_ADDR + GPIO_UART_IO_SIM_TX_PIN * 4)) = \
                *((volatile UINT32 *)(GPIO_BASE_ADDR + GPIO_UART_IO_SIM_TX_PIN * 4))\
                | (0x01 << GCFG_OUTPUT_POS);
    }
    else
    {
        *((volatile UINT32 *)(GPIO_BASE_ADDR + GPIO_UART_IO_SIM_TX_PIN * 4)) = \
                *((volatile UINT32 *)(GPIO_BASE_ADDR + GPIO_UART_IO_SIM_TX_PIN * 4))\
                & ~(0x01 << GCFG_OUTPUT_POS);
    }
}

void GPIO_UART_io_sim_rx_function_enable(void)
{
    *((volatile UINT32 *)(GPIO_BASE_ADDR + GPIO_UART_IO_SIM_RX_PIN * 4)) = 0x3C;
}

unsigned long uart_io_sim_get_rx(void)
{
    return *((volatile UINT32 *)(GPIO_BASE_ADDR + GPIO_UART_IO_SIM_RX_PIN * 4))& GCFG_INPUT_BIT;
}
#endif

#ifdef INCLUDE_RX_MODE
LOCAL void uart_io_sim_pwm_cap_neg_init_for_rx(void);
extern void UARTCALL uart_io_sim_receive(void);
LOCAL void uart_io_sim_pwm_cap_neg_init_for_rx(void);
#endif

#ifdef INCLUDE_TX_MODE
extern void uart_io_sim_send_string(char *);
#endif
static  void  uart_io_sim_delay(volatile unsigned long times)
{
    while (times != 0)
    {
        times --;
    }
}

#ifdef INCLUDE_TX_MODE
void uart_io_sim_tx_buf_init(void)
{
    os_memset(uart_io_sim_tx_buf, 0, sizeof(uart_io_sim_tx_buf));
#if defined INTERRUPT_METHOD
    uart_io_sim_tx_buf_size = 0;
    p_uart_io_sim_tx_buf = NULL;
    uart_io_sim_tx_buf_current_bit = 0;
    uart_io_sim_tx_buf_current_cnt = 0;
#endif
}
#endif		/* #ifdef INCLUDE_TX_MODE */

#ifdef INCLUDE_RX_MODE
void uart_io_sim_rx_buf_init(void)
{
    os_memset(uart_io_sim_rx_buf, 0, sizeof(uart_io_sim_rx_buf));

    uart_io_sim_rx_done = false;
    uart_io_sim_rx_buf_current_cnt = 0;
#if defined INTERRUPT_METHOD
    uart_io_sim_rx_buf_current_bit = 0;
#endif
}
#endif		/* #ifdef INCLUDE_RX_MODE */


#if defined INTERRUPT_METHOD
#ifdef INCLUDE_TX_MODE

void UARTCALL uart_io_sim_timer_int_handle_tx_data(unsigned char ucChannel)
{
    if(uart_io_sim_tx_first_start)
    {
        uart_io_sim_tx_first_start = 0;
        uart_io_sim_set_tx(0x00UL);
        return;
    }

    if (p_uart_io_sim_tx_buf == NULL)
    {
        uart_io_sim_set_tx(0x01UL);
        *((volatile UINT32 *)(PWM_CTL)) = (*(volatile UINT32 *)(PWM_CTL))& ~(2 << (UART_IO_SIM_PWM_TIMER_TX_CHN * 4));
        return;
    }

    if (uart_io_sim_tx_buf_current_bit < 8)
    {
        uart_io_sim_set_tx((p_uart_io_sim_tx_buf[uart_io_sim_tx_buf_current_cnt] >> uart_io_sim_tx_buf_current_bit)
                           & 0x01UL);           // data bit
        uart_io_sim_tx_buf_current_bit ++;
    }
    else if (uart_io_sim_tx_buf_current_bit == 8 || uart_io_sim_tx_buf_current_bit == 9)
    {
        uart_io_sim_set_tx(0x01UL);             // STOP bit
        uart_io_sim_tx_buf_current_bit ++;
    }
    else
    {
        uart_io_sim_tx_buf_current_bit = 0;
        uart_io_sim_tx_buf_current_cnt ++;
        if(uart_io_sim_tx_buf_current_cnt == UART_IO_SIM_TX_FIFO_MAX_COUNT)
            uart_io_sim_tx_buf_current_cnt = 0;
        if (uart_io_sim_tx_buf_current_cnt == uart_io_sim_tx_buf_size)
        {
            p_uart_io_sim_tx_buf = NULL;        // data send over
        }
        else
        {
            uart_io_sim_set_tx(0x00UL);         // START bit
        }
    }
}

void uart_io_sim_pwm_timer_init_for_tx(void)
{
    UINT32 ret;
    pwm_param_t param;

    /*init pwm*/
    param.channel         = UART_IO_SIM_PWM_TIMER_TX_CHN;
    param.cfg.bits.en     = PWM_ENABLE;
    param.cfg.bits.int_en = PWM_INT_DIS;
    param.cfg.bits.mode   = PMODE_TIMER;

#if(CFG_RUNNING_PLATFORM == FPGA_PLATFORM)  // FPGA:PWM0-2-32kCLK, pwm3-5-24CLK
    param.cfg.bits.clk    = PWM_CLK_32K;
#else
    param.cfg.bits.clk    = PWM_CLK_26M;
#endif

    param.p_Int_Handler   = uart_io_sim_timer_int_handle_tx_data;
    param.duty_cycle      = 0;
    param.end_value       = UART_IO_SIM_CLK_DIVID_SET_TX;

    ret = sddev_control(PWM_DEV_NAME, CMD_PWM_INIT_PARAM, &param);
    ASSERT(PWM_SUCCESS == ret);
}
#endif

#ifdef INCLUDE_RX_MODE
extern void uart_io_sim_pwm_timer_init_for_rx(UINT16 end);

static void UARTCALL init_pwm_param_2(pwm_param_t *pwm_param)
{
    UINT32 value;

    *((volatile UINT32 *)(GPIO_BASE_ADDR + PWM2_GPIO_PIN * 4)) = 0x40;
    *((volatile UINT32 *)(REG_GPIO_FUNC_CFG)) = *((volatile UINT32 *)(REG_GPIO_FUNC_CFG)) | BIT(PWM2_GPIO_PIN);

    value = REG_READ(PWM_CTL);
    value = (value & ~(0x0F << (0x04 *  pwm_param->channel)))
            | ((pwm_param->cfg.val & 0x0F) << (0x04 * pwm_param->channel));
    REG_WRITE(PWM_CTL, value);

    value = (((UINT32)pwm_param->duty_cycle & 0x0000FFFF) << 16)
            + ((UINT32)pwm_param->end_value & 0x0000FFFF);

	#if (CFG_SOC_NAME == SOC_BK7231)
    REG_WRITE(REG_APB_BK_PWMn_CNT_ADDR(pwm_param->channel), value);
	#else
	REG_WRITE(REG_APB_BK_PWMn_END_ADDR(pwm_param->channel), value);
	#endif

    p_PWM_Int_Handler[pwm_param->channel] = pwm_param->p_Int_Handler;

    *((volatile UINT32 *)(ICU_PERI_CLK_PWD)) = *((volatile UINT32 *)(ICU_PERI_CLK_PWD))& ~(PWD_PWM2_CLK_BIT);
    *((volatile UINT32 *)(ICU_PWM_CLK_MUX)) = *((volatile UINT32 *)(ICU_PWM_CLK_MUX))& ~(1 << (pwm_param->channel));
    *((volatile UINT32 *)(ICU_INTERRUPT_ENABLE)) = *((volatile UINT32 *)(ICU_INTERRUPT_ENABLE)) | (IRQ_PWM_BIT);
}

static void UARTCALL init_pwm_param_5(pwm_param_t *pwm_param)
{
    UINT32 value;

    *((volatile UINT32 *)(GPIO_BASE_ADDR + PWM5_GPIO_PIN * 4)) = 0x40;
    *((volatile UINT32 *)(REG_GPIO_FUNC_CFG)) = *((volatile UINT32 *)(REG_GPIO_FUNC_CFG)) | BIT(PWM5_GPIO_PIN);

    value = REG_READ(PWM_CTL);
    value = (value & ~(0x0F << (0x04 *  pwm_param->channel)))
            | ((pwm_param->cfg.val & 0x0F) << (0x04 * pwm_param->channel));
    REG_WRITE(PWM_CTL, value);

    value = (((UINT32)pwm_param->duty_cycle & 0x0000FFFF) << 16)
            + ((UINT32)pwm_param->end_value & 0x0000FFFF);
	#if (CFG_SOC_NAME == SOC_BK7231)
    REG_WRITE(REG_APB_BK_PWMn_CNT_ADDR(pwm_param->channel), value);
	#else
	REG_WRITE(REG_APB_BK_PWMn_END_ADDR(pwm_param->channel), value);
	#endif

    p_PWM_Int_Handler[pwm_param->channel] = pwm_param->p_Int_Handler;

    *((volatile UINT32 *)(ICU_PERI_CLK_PWD)) = *((volatile UINT32 *)(ICU_PERI_CLK_PWD))& ~(PWD_PWM5_CLK_BIT);
    *((volatile UINT32 *)(ICU_PWM_CLK_MUX)) = *((volatile UINT32 *)(ICU_PWM_CLK_MUX))& ~(1 << (pwm_param->channel));
    *((volatile UINT32 *)(ICU_INTERRUPT_ENABLE)) = *((volatile UINT32 *)(ICU_INTERRUPT_ENABLE)) | (IRQ_PWM_BIT);
}

void UARTCALL uart_io_sim_timer_int_handle_rx_data(unsigned char ucChannel)
{
    volatile UINT8 val = 0 ;
    val = *((volatile UINT32 *)(GPIO_BASE_ADDR + GPIO_UART_IO_SIM_RX_PIN * 4))& GCFG_INPUT_BIT;

    ++uart_io_sim_rx_int_count;
    if( uart_io_sim_rx_int_count % 5 != 2 || uart_io_sim_rx_int_count == 2)
        return;

    if (uart_io_sim_rx_buf_current_bit < 8)             // data bit
    {
        uart_io_sim_rx_buf[uart_io_sim_rx_buf_current_cnt] |= (val << uart_io_sim_rx_buf_current_bit);
        uart_io_sim_rx_buf_current_bit ++;
        if(uart_io_sim_rx_buf_current_bit == 8)
        {
            uart_io_sim_rx_buf_current_bit = 0;
            uart_io_sim_rx_int_count = 0;
            *((volatile UINT32 *)(PWM_CTL)) = *((volatile UINT32 *)(PWM_CTL))& ~(2 << (ucChannel * 4));
            *((volatile UINT32 *)(GPIO_BASE_ADDR + GPIO_UART_IO_SIM_RX_PIN * 4)) = 0x40;
            *((volatile UINT32 *)(PWM_CTL)) = (*(volatile UINT32 *)(PWM_CTL)) | (2 << (UART_IO_SIM_PWM_CAP_NEG_RX_CHN * 4));

            kfifo_put(uart[1].rx, (UINT8 *) & (uart_io_sim_rx_buf[uart_io_sim_rx_buf_current_cnt]), 1); //get char c
            uart_io_sim_rx_buf_current_cnt ++;
            if(uart_io_sim_rx_buf_current_cnt == UART_IO_SIM_RX_FIFO_MAX_COUNT)
            {
                uart_io_sim_rx_buf_current_cnt = 0;
                os_memset(uart_io_sim_rx_buf, 0, UART_IO_SIM_RX_FIFO_MAX_COUNT);
            }
        }
    }
}

void UARTCALL uart_io_sim_pwm_timer_init_for_rx(UINT16 end)
{
    pwm_param_t param;

    /*init pwm*/
    param.channel         = UART_IO_SIM_PWM_TIMER_RX_CHN;
    param.cfg.bits.en     = PWM_ENABLE;
    param.cfg.bits.int_en = PWM_INT_DIS;
    param.cfg.bits.mode   = PMODE_TIMER;
#if(CFG_RUNNING_PLATFORM == FPGA_PLATFORM)
    param.cfg.bits.clk    = PWM_CLK_32K;
#else
    param.cfg.bits.clk    = PWM_CLK_26M;
#endif
    param.p_Int_Handler   = uart_io_sim_timer_int_handle_rx_data;
    param.duty_cycle      = 0;
    param.end_value       = end;

    init_pwm_param_2(&param);

}

void UARTCALL uart_io_sim_int_handle_rx_cap_neg(unsigned char ucChannel)
{
    // capture START bit, prepare to receive data bits
    *((volatile UINT32 *)(GPIO_BASE_ADDR + GPIO_UART_IO_SIM_RX_PIN * 4)) = 0x3C;

#if defined INTERRUPT_METHOD
    *((volatile UINT32 *)(PWM_CTL)) = (*(volatile UINT32 *)(PWM_CTL))&~(2 << (ucChannel * 4));

    uart_io_sim_rx_int_count = 0;
    uart_io_sim_rx_firt_start_bit = 0;
    uart_io_sim_rx_buf_current_bit = 0;

    *((volatile UINT32 *)(PWM_CTL)) = *((volatile UINT32 *)(PWM_CTL)) | (2 << (UART_IO_SIM_PWM_TIMER_RX_CHN * 4));
#elif defined POLLING_METHOD
    uart_io_sim_receive();
#endif
}

void UARTCALL uart_io_sim_pwm_cap_neg_init_for_rx(void)
{
    pwm_param_t param;
    /*init pwm*/
    param.channel         = UART_IO_SIM_PWM_CAP_NEG_RX_CHN;
    param.cfg.bits.en     = PWM_ENABLE;
    param.cfg.bits.int_en = PWM_INT_DIS;
    param.cfg.bits.mode   = PMODE_CAP_NEG;
#if(CFG_RUNNING_PLATFORM == FPGA_PLATFORM)
    param.cfg.bits.clk    = PWM_CLK_32K;
#else
    param.cfg.bits.clk    = PWM_CLK_26M;
#endif
    param.p_Int_Handler   = uart_io_sim_int_handle_rx_cap_neg;
    param.duty_cycle      = 0;
    param.end_value       = 0xFFFF;

    init_pwm_param_5(&param);
}
#endif
#endif

//#ifdef (INCLUDE_TX_MODE || INCLUDE_RX_MODE)
#if ((defined INCLUDE_TX_MODE) || (defined INCLUDE_RX_MODE))
void uart_io_sim_init(void)
{
    //gpio_config(SIMU_UART_GPIOCLK,GMODE_OUTPUT);
#ifdef INCLUDE_TX_MODE
    GPIO_UART_io_sim_tx_function_enable();
    uart_io_sim_tx_buf_init();
    uart_io_sim_set_tx(0x01UL);
    uart_io_sim_delay(1000);
    uart_io_sim_pwm_timer_init_for_tx();
#endif

#ifdef INCLUDE_RX_MODE
    uart_io_sim_rx_buf_init();
    uart_io_sim_pwm_timer_init_for_rx(UART_IO_SIM_CLK_DIVID_SET);
    uart_io_sim_pwm_cap_neg_init_for_rx();
    *((volatile UINT32 *)(PWM_CTL)) = *((volatile UINT32 *)(PWM_CTL)) | (2 << (UART_IO_SIM_PWM_CAP_NEG_RX_CHN * 4));

#endif

#ifdef INCLUDE_TX_MODE
    uart_io_start = 1;
#endif
}

void uart_io_sim_disable(void)
{
#if defined INTERRUPT_METHOD
    UINT32 param;
    UINT32 ret;

    param = UART_IO_SIM_PWM_CAP_NEG_RX_CHN;
    ret = sddev_control(PWM_DEV_NAME, CMD_PWM_UINT_DISABLE, &param);
    ASSERT(PWM_SUCCESS == ret);

    param = UART_IO_SIM_PWM_TIMER_TX_CHN;
    ret = sddev_control(PWM_DEV_NAME, CMD_PWM_UINT_DISABLE, &param);
    ASSERT(PWM_SUCCESS == ret);
    p_uart_io_sim_tx_buf = NULL;

    param = UART_IO_SIM_PWM_TIMER_RX_CHN;
    ret = sddev_control(PWM_DEV_NAME, CMD_PWM_UINT_DISABLE, &param);
    ASSERT(PWM_SUCCESS == ret);
#ifdef INCLUDE_TX_MODE
    uart_io_start = 0;
#endif

#endif
}
#endif

//----------------------------------------------
// UART_IO_SIM Tx/Rx character driver
//----------------------------------------------
#ifdef INCLUDE_TX_MODE
void UARTCALL uart_io_sim_send_byte(u8 data)
{
#if defined INTERRUPT_METHOD
    if(!uart_io_start)
        return;

    if(p_uart_io_sim_tx_buf)
    {
        p_uart_io_sim_tx_buf[uart_io_sim_tx_buf_size++] = data;
        if(uart_io_sim_tx_buf_size == UART_IO_SIM_TX_FIFO_MAX_COUNT)
        {
            uart_io_sim_tx_buf_size = 0;
        }
    }
    else
    {
        p_uart_io_sim_tx_buf = uart_io_sim_tx_buf;
        uart_io_sim_tx_buf_size = 0;
        uart_io_sim_tx_buf_current_cnt = 0;
        uart_io_sim_tx_first_start = 1;
        os_memset(p_uart_io_sim_tx_buf, 0, UART_IO_SIM_TX_FIFO_MAX_COUNT);
        p_uart_io_sim_tx_buf[uart_io_sim_tx_buf_size++] = data;
        if(uart_io_sim_tx_buf_size == UART_IO_SIM_TX_FIFO_MAX_COUNT)
            uart_io_sim_tx_buf_size = 0;

        *((volatile UINT32 *)(PWM_CTL)) = (*(volatile UINT32 *)(PWM_CTL)) | (2 << (UART_IO_SIM_PWM_TIMER_TX_CHN * 4));
    }

#elif defined POLLING_METHOD
    int i;
    uart_io_sim_set_tx(0x00UL);         // START bit
    uart_io_sim_delay(SEND_BYTE_START_DELAY);
    for (i = 0; i < 8; i++)
    {
        uart_io_sim_set_tx((data >> i) & 0x01UL);
        uart_io_sim_delay(DELAY_NOPS_COUNT);
    }
    uart_io_sim_set_tx(0x01UL);         // STOP bit
    uart_io_sim_delay(DELAY_NOPS_COUNT * 3);
#endif
}

void uart_io_sim_send_string(char *buff)
{
    while (*buff)
        uart_io_sim_send_byte(*buff++);
}

void uart_io_sim_send(u8 *buff, int len)
{
    if (buff == NULL)
    {
        return;
    }

    if (len < 0)
    {
        return;
    }

#if defined INTERRUPT_METHOD
    p_uart_io_sim_tx_buf = buff;
    uart_io_sim_tx_buf_size = len;
    uart_io_sim_tx_buf_current_cnt = 0;
    uart_io_sim_tx_buf_current_bit = 0;

    *((volatile UINT32 *)(PWM_CTL)) = (*(volatile UINT32 *)(PWM_CTL)) | (2 << (UART_IO_SIM_PWM_TIMER_TX_CHN * 4));

#elif defined POLLING_METHOD
    REG_ICU_INT_GLOBAL_ENABLE = 0;
    while (len--)
    {
        uart_io_sim_send_byte(*buff ++);
    }
    REG_ICU_INT_GLOBAL_ENABLE = ICU_INT_GLOBAL_ENABLE_IRQ_SET + ICU_INT_GLOBAL_ENABLE_FIQ_SET;    // CPU IRQ enable
#endif
}
#endif		/* #ifdef INCLUDE_TX_MODE */

#ifdef INCLUDE_RX_MODE
unsigned char UARTCALL uart_io_sim_receive_byte(void)
{
    unsigned char ucTemp;
    ucTemp = 0;
#if defined INTERRUPT_METHOD
#elif defined POLLING_METHOD
#if (MCU_CLK > MCU_CLK_16MHz)
    uart_io_sim_delay(RECEIVE_BYTE_START_DELAY);
#endif
    int i;
    for (i = 0; i < 7; i++)
    {
        ucTemp |= (uart_io_sim_get_rx() << i);
        uart_io_sim_delay(RECEIVE_BYTE_MIDDLE_DELAY);
    }
    uart_io_sim_delay(RECEIVE_BYTE_CALI_LAST_DELAY);
    ucTemp |= (uart_io_sim_get_rx() << 7);
    uart_io_sim_delay(RECEIVE_BYTE_LAST_DELAY);
#endif
    return ucTemp;
}

void UARTCALL uart_io_sim_receive(void)
{
#if defined INTERRUPT_METHOD
#elif defined POLLING_METHOD
    int i;
    REG_ICU_INT_GLOBAL_ENABLE = 0;          // CPU IRQ disable
    while (uart_io_sim_rx_buf_current_cnt < UART_IO_SIM_RX_FIFO_MAX_COUNT)
    {
        uart_io_sim_rx_buf[uart_io_sim_rx_buf_current_cnt] = uart_io_sim_receive_byte();
        uart_io_sim_rx_buf_current_cnt ++;
        uart_io_sim_rx_done = TRUE;
        i = 0;
        while (i++ < (DELAY_NOPS_COUNT * 10))
        {
            if (uart_io_sim_get_rx() == 0x00)
            {
                uart_io_sim_delay(RECEIVE_BYTE_STOP_DELAY);
                break;
            }
        }
        if (i >= (DELAY_NOPS_COUNT * 10))
        {
            break;
        }
    }
    uart_io_sim_pwm_cap_neg_init_for_rx();
    REG_ICU_INT_GLOBAL_ENABLE = ICU_INT_GLOBAL_ENABLE_IRQ_SET + ICU_INT_GLOBAL_ENABLE_FIQ_SET;    // CPU IRQ enable
#endif
}
#endif		/* #ifdef INCLUDE_RX_MODE */
