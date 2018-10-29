#include "gd32f4xx.h"
#include "hal/soc/soc.h"
#include "ringbuf.h"
#include <aos/aos.h>
#include <k_api.h>
#include <stdio.h>
#define MAX_UART_NUM 4


typedef enum 
{
    _UART_STATUS_CLOSED = 0,
    _UART_STATUS_OPENED,
}_uart_status_e;

typedef struct
{
    _uart_status_e      status;
    uint8_t             *rx_buf;
    uint32_t            rx_size;
    ring_buffer_t       rx_ringbuf;
    aos_sem_t           rx_semphr;
    aos_mutex_t         tx_mutex;
} _uart_drv_t;

extern uart_dev_t  uart_0;
_uart_drv_t _uart_drv[MAX_UART_NUM];
static const uint32_t  s_uart_buffersize_table[] = {128, 4096, 128, 128};
static const uint32_t s_uart_periph_table[] = {USART0, USART1, USART2, UART3};
static const rcu_periph_enum s_uart_clk_table[] = {RCU_USART0, RCU_USART1, RCU_USART2, RCU_UART3};
static const uint32_t s_uart_txpin_table[] = {GPIO_PIN_9, GPIO_PIN_2, GPIO_PIN_10, GPIO_PIN_10};
static const uint32_t s_uart_gpio_txaf_table[] = {GPIO_AF_7, GPIO_AF_7, GPIO_AF_7, GPIO_AF_8};
static const uint32_t s_uart_rxpin_table[] = {GPIO_PIN_10, GPIO_PIN_3, GPIO_PIN_11, GPIO_PIN_11};
static const uint32_t s_uart_gpio_rxaf_table[] = {GPIO_AF_7, GPIO_AF_7, GPIO_AF_7, GPIO_AF_8};
static const uint32_t s_uart_gpio_table[] = {GPIOA, GPIOA, GPIOB, GPIOC};
static const rcu_periph_enum s_uart_gpio_clk_table[] = {RCU_GPIOA, RCU_GPIOA, RCU_GPIOB, RCU_GPIOC};
static const IRQn_Type s_uart_irqn[] = {USART0_IRQn, USART1_IRQn, USART2_IRQn, UART3_IRQn};

void dev_uart_init(uart_dev_t *uart);

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE int __io_getchar(void)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif /* __GNUC__ */

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
    while(RESET == usart_flag_get(s_uart_periph_table[uart_0.port], USART_FLAG_TBE));
    usart_data_transmit(s_uart_periph_table[uart_0.port], (uint8_t)ch);
    
    return ch;
}

/**
  * @brief  Retargets the C library scanf function to the USART.
  * @param  None
  * @retval None
  */
GETCHAR_PROTOTYPE
{
    /* Place your implementation of fgetc here */
    /* e.g. readwrite a character to the USART2 and Loop until the end of transmission */
    uint8_t ch = 0;
    uint32_t recved_size;
    int32_t ret = 0;
    
    ret = hal_uart_recv_II(&uart_0, &ch, 1, &recved_size, HAL_WAIT_FOREVER);

    if (ret == 0) {
        return ch;
    } else {
        return -1;
    }
}

int32_t hal_uart_init(uart_dev_t *uart)
{
    _uart_drv_t *pdrv = &_uart_drv[uart->port];
    int reval;
    
    if(pdrv->status == _UART_STATUS_CLOSED)
    {
        pdrv->rx_buf = (uint8_t *)aos_malloc(s_uart_buffersize_table[uart->port]);
        if(NULL == pdrv->rx_buf){
            while(1);
        }
        
        ring_buffer_init(&pdrv->rx_ringbuf, pdrv->rx_buf, s_uart_buffersize_table[uart->port]);
        reval = aos_sem_new(&pdrv->rx_semphr, 0);
        if(reval != 0)
        {
            while(1);
        }

        reval = aos_mutex_new(&pdrv->tx_mutex);
        if(reval != 0)
        {
            while(1);
        }
        
        dev_uart_init(uart);

        pdrv->status = _UART_STATUS_OPENED;
    }
    else{
        while(1);    
    }

    return 0;
}



void dev_uart_init(uart_dev_t *uart)
{
    /* USART interrupt configuration */
    nvic_irq_enable(s_uart_irqn[uart->port], 0, 0);

    /* enable GPIO clock */
    rcu_periph_clock_enable(s_uart_gpio_clk_table[uart->port]);

    /* enable USART clock */
    rcu_periph_clock_enable(s_uart_clk_table[uart->port]);

    /* connect port to USARTx_Tx */
    gpio_af_set(s_uart_gpio_table[uart->port], s_uart_gpio_txaf_table[uart->port], s_uart_txpin_table[uart->port]);

    /* connect port to USARTx_Rx */
    gpio_af_set(s_uart_gpio_table[uart->port], s_uart_gpio_rxaf_table[uart->port], s_uart_rxpin_table[uart->port]);
    
        /* configure USART Tx as alternate function push-pull */
    gpio_mode_set(s_uart_gpio_table[uart->port], GPIO_MODE_AF, GPIO_PUPD_NONE, s_uart_txpin_table[uart->port]);
    gpio_output_options_set(s_uart_gpio_table[uart->port], GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, s_uart_txpin_table[uart->port]);

    /* configure USART Rx as alternate function push-pull */
    gpio_mode_set(s_uart_gpio_table[uart->port], GPIO_MODE_AF, GPIO_PUPD_NONE, s_uart_rxpin_table[uart->port]);
    gpio_output_options_set(s_uart_gpio_table[uart->port], GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, s_uart_rxpin_table[uart->port]);

    /* USART configure */
    usart_deinit(s_uart_periph_table[uart->port]);
    usart_baudrate_set(s_uart_periph_table[uart->port], uart->config.baud_rate);
    usart_receive_config(s_uart_periph_table[uart->port], USART_RECEIVE_ENABLE);
    usart_transmit_config(s_uart_periph_table[uart->port], USART_TRANSMIT_ENABLE);

    switch(uart->config.data_width){
        case DATA_WIDTH_9BIT:
            usart_word_length_set(s_uart_periph_table[uart->port], USART_WL_9BIT);
            break;
        case DATA_WIDTH_8BIT:
            usart_word_length_set(s_uart_periph_table[uart->port], USART_WL_8BIT);
            break;
        default:
            while(1);
    }

    switch(uart->config.parity){
        case NO_PARITY:
            usart_parity_config(s_uart_periph_table[uart->port], USART_PM_NONE);
            break;

        case ODD_PARITY:
            usart_parity_config(s_uart_periph_table[uart->port], USART_PM_ODD);
            break;

        case EVEN_PARITY:
            usart_parity_config(s_uart_periph_table[uart->port], USART_PM_EVEN);
            break;

        default:
            while(1);
    }

    switch(uart->config.stop_bits){
        case STOP_BITS_1:
            usart_stop_bit_set(s_uart_periph_table[uart->port], USART_STB_1BIT);
            break;

        case STOP_BITS_2:
            usart_stop_bit_set(s_uart_periph_table[uart->port], USART_STB_2BIT);
            break;

        default:
            break;
    }

    switch(uart->config.flow_control){
        case FLOW_CONTROL_DISABLED:
            usart_hardware_flow_rts_config(s_uart_periph_table[uart->port], USART_RTS_DISABLE);
            usart_hardware_flow_cts_config(s_uart_periph_table[uart->port], USART_CTS_DISABLE);
            
            break;

        case FLOW_CONTROL_CTS:
            usart_hardware_flow_rts_config(s_uart_periph_table[uart->port], USART_RTS_DISABLE);
            usart_hardware_flow_cts_config(s_uart_periph_table[uart->port], USART_CTS_ENABLE);
            break;

        case FLOW_CONTROL_RTS:
            usart_hardware_flow_rts_config(s_uart_periph_table[uart->port], USART_RTS_ENABLE);
            usart_hardware_flow_cts_config(s_uart_periph_table[uart->port], USART_CTS_DISABLE);
            break;

        case FLOW_CONTROL_CTS_RTS:
            usart_hardware_flow_rts_config(s_uart_periph_table[uart->port], USART_RTS_ENABLE);
            usart_hardware_flow_cts_config(s_uart_periph_table[uart->port], USART_CTS_ENABLE);
            break;

        default:
            break;
    }
    
    usart_enable(s_uart_periph_table[uart->port]);
    usart_interrupt_enable(s_uart_periph_table[uart->port], USART_INTEN_RBNEIE);
}

static int _dev_uart_putchar(uart_dev_t *uart, int ch)
{
    volatile uint32_t i;
    while(RESET == usart_flag_get(s_uart_periph_table[uart->port], USART_FLAG_TBE));
    for(i = 0; i < 200; i++);
    usart_data_transmit(s_uart_periph_table[uart->port], (uint8_t)ch);
    
    
    return ch;
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout) 
{

    uint8_t* pTmp = (uint8_t*)data;
    _uart_drv_t *pdrv = &_uart_drv[uart->port];
    
    aos_mutex_lock(&pdrv->tx_mutex, AOS_WAIT_FOREVER);
    
    for (int i = 0; i < size; i++) {
        _dev_uart_putchar(uart, *pTmp++);
    }
    
    aos_mutex_unlock(&pdrv->tx_mutex);
    
    return 0;
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t *recv_size, uint32_t timeout)
{
    uint32_t read_size, actual_size, tmp;
    uint32_t ringbuf_size;
    uint32_t start_time, expired_time;
    _uart_drv_t *pdrv = &_uart_drv[uart->port];
    uint8_t *p_data;
    
    p_data = data;

    recv_size = recv_size == NULL ? &actual_size : recv_size;

    start_time = aos_now_ms();
    expired_time = 0;

    *recv_size = 0;
    ringbuf_size = pdrv->rx_ringbuf.size - 1;

    for(;;)
    {
        read_size = expect_size > ringbuf_size ? ringbuf_size : expect_size;

        if(read_size > ring_buffer_used_space( &pdrv->rx_ringbuf ))
        {
            pdrv->rx_size = read_size;

            if ( aos_sem_wait( &pdrv->rx_semphr, timeout - expired_time) != 0)
            {
                pdrv->rx_size = 0;
                return -1;
            }
        }

        ring_buffer_read(&pdrv->rx_ringbuf, p_data, read_size, &tmp);
        p_data += read_size;
        *recv_size += read_size;
        expect_size -= read_size;

        if(expect_size == 0)
        {
            break;
        }

        expired_time = aos_now_ms() - start_time;
        if(expired_time > timeout)
        {
            return -1;
        }
    }

    return 0;
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
    _uart_drv_t *pdrv = &_uart_drv[uart->port];

    while(usart_flag_get(s_uart_periph_table[uart->port], USART_FLAG_TBE) != SET);

    usart_deinit(s_uart_clk_table[uart->port]);
    /* disable USART clock */
    rcu_periph_clock_disable(s_uart_clk_table[uart->port]);

    ring_buffer_deinit(&pdrv->rx_ringbuf);
    aos_free(pdrv->rx_buf);
    aos_sem_free(&pdrv->rx_semphr);
    aos_mutex_free(&pdrv->tx_mutex);
    
    pdrv->status = _UART_STATUS_CLOSED;
    
    return 0;
}


void uart_rx_cb(uint8_t port)
{
    uint8_t rx_byte;
    volatile uint32_t val;
    _uart_drv_t *pdrv = &_uart_drv[port];

    if(RESET != usart_flag_get(s_uart_periph_table[port], USART_FLAG_ORERR)){
        val = USART_STAT0(s_uart_periph_table[port]); 
        val = USART_DATA(s_uart_periph_table[port]);    
    }
    
    if(RESET != usart_flag_get(s_uart_periph_table[port], USART_FLAG_RBNE)){
        /* receive data */
        rx_byte = usart_data_receive(s_uart_periph_table[port]);
        if(0 == ring_buffer_write(&pdrv->rx_ringbuf, &rx_byte, 1)){
            printf("ring buffer overflow!! \r\n");
            while(1);
        }

        if ( pdrv->rx_size > 0 && ring_buffer_used_space( &pdrv->rx_ringbuf ) >= pdrv->rx_size )
        {
            aos_sem_signal( &pdrv->rx_semphr );
            pdrv->rx_size = 0;
        }
    }
}

void USART0_IRQHandler(void)
{
    krhino_intrpt_enter();
    uart_rx_cb(0);
    krhino_intrpt_exit();
}

void USART1_IRQHandler(void)
{
    krhino_intrpt_enter();
    uart_rx_cb(1);
    krhino_intrpt_exit();
}

void USART2_IRQHandler(void)
{
    krhino_intrpt_enter();
    uart_rx_cb(2);
    krhino_intrpt_exit();
}

void UART3_IRQHandler(void)
{
    krhino_intrpt_enter();
    uart_rx_cb(3);
    krhino_intrpt_exit();
}

void UART4_IRQHandler(void)
{
    krhino_intrpt_enter();
    uart_rx_cb(4);  
    krhino_intrpt_exit();    
}
