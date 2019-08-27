#include <stdio.h>
#include "lega_cm4.h"
#include "system_cm4.h"
#include "lega_common.h"
#include "lega_peripheral_reg.h"
#include "lega_uart.h"

lega_uart_callback_func g_lega_uart_callback_handler[LEGA_UART_NUM];

void UART_SendData(UART_TypeDef* UARTx, unsigned char Data)
{
    UARTx->THR = Data;
    while((UARTx->LSR & (DW_UART_LSR_TX_THRE|DW_UART_LSR_TX_TEMT)) != DW_UART_TX_TC);
}

uint8_t UART_ReceiveData(UART_TypeDef* UARTx)
{
    while((UARTx->LSR & DW_UART_LSR_DR) == RESET);

    return UARTx->RBR;
}

ITStatus UART_Get_Data_Ready_Status(UART_TypeDef* UARTx, unsigned int UART_DATA_READY)
{
    ITStatus bitstatus = RESET;

    if(((UARTx->LSR) & UART_DATA_READY) != (uint16_t)RESET)
        bitstatus=SET;
    else
        bitstatus=RESET;

    return bitstatus;
}

void UART_IRQHandler(UART_TypeDef * uart)
{
    uint8_t res;
    char ch;
    uint8_t uart_port;
    if(UART0 == uart)
    {
        uart_port = LEGA_UART0_INDEX;
    }
    else if(UART1 == uart)
    {
        uart_port = LEGA_UART1_INDEX;
    }
    else if(UART2 == uart)
    {
        uart_port = LEGA_UART2_INDEX;
    }
    else
    {
        return;
    }
    if(UART_Get_Data_Ready_Status(uart, DW_UART_LSR_DR) != RESET)
    {
        res = UART_ReceiveData(uart);
        ch = (char)res;
        if(g_lega_uart_callback_handler[uart_port])
        {
            g_lega_uart_callback_handler[uart_port](ch);
        }
    }
}

void UART0_IRQHandler(void)
{
    lega_intrpt_enter();
    UART_IRQHandler(UART0);
    lega_intrpt_exit();
}

void UART1_IRQHandler(void)
{
    lega_intrpt_enter();
    UART_IRQHandler(UART1);
    lega_intrpt_exit();
}

void UART2_IRQHandler(void)
{
    lega_intrpt_enter();
    UART_IRQHandler(UART2);
    lega_intrpt_exit();
}

/**
 * Initialises a UART interface
 *
 *
 * @param[in]  uart  the interface which should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t lega_uart_init(lega_uart_dev_t *uart)
{
    UART_TypeDef* UARTx;

    uint32_t baudrate;
    int flow_control;
    uint32_t data_width;
    int stop_bit;
    int parity_flag;
    uint32_t dll, tmp_value;

    if(NULL == uart)
    {
        return -1;
    }

    if(LEGA_UART0_INDEX == uart->port)
    {
        UARTx = UART0;
    }
    else if(LEGA_UART1_INDEX == uart->port)
    {
        UARTx = UART1;
    }
    else if(LEGA_UART2_INDEX == uart->port)
    {
        UARTx = UART2;
    }
    else
    {
        return -1;
    }

    if((0 == uart->config.baud_rate)
        || (uart->config.data_width >= DATA_9BIT)
        || (uart->config.parity > PARITY_EVEN)
        || (uart->config.stop_bits > STOP_2BITS)
        || (uart->config.flow_control > FLOW_CTRL_CTS_RTS)
        || (uart->config.mode > TX_RX_MODE))
    {
        return -1;
    }

    baudrate = uart->config.baud_rate;
    data_width = uart->config.data_width;
    stop_bit = uart->config.stop_bits << 2;

    if(uart->config.parity == PARITY_NO)
    {
        parity_flag = uart->config.parity << 3;
    }
    else
    {
        parity_flag = ((uart->config.parity-1)<<4) | (1<<3);
    }

    if(FLOW_CTRL_DISABLED == uart->config.flow_control)
    {
        flow_control = uart->config.flow_control<<5;
    }
    else
    {
        flow_control = 1 << 5;
    }

    //uart pin mux configure
    if(UARTx == UART0)
    {
        tmp_value = REG_RD(UART0_PIN_MUX_REG) & (~DW_UART0_GPIO_MASK);
        REG_WR(UART0_PIN_MUX_REG, (tmp_value | DW_UART0_TX | DW_UART0_RX));//gpio0/gpio1
        tmp_value = REG_RD(HW_CTRL_PE_PS);
        REG_WR(HW_CTRL_PE_PS, (tmp_value & (~(UART0_PAD_MASK))));
        tmp_value = REG_RD(PAD_PE_REG);
        REG_WR(PAD_PE_REG, (tmp_value & (~(UART0_PAD_MASK))));
    }
    else if(UARTx == UART1)
    {
        tmp_value = REG_RD(UART1_PIN_MUX_REG) & (~DW_UART1_GPIO_MASK);
        REG_WR(UART1_PIN_MUX_REG, (tmp_value | DW_UART1_TX | DW_UART1_RX));//gpio2/gpio3
        tmp_value = REG_RD(HW_CTRL_PE_PS);
        REG_WR(HW_CTRL_PE_PS, (tmp_value & (~(UART1_PAD_MASK))));
        tmp_value = REG_RD(PAD_PE_REG);
        REG_WR(PAD_PE_REG, (tmp_value & (~(UART1_PAD_MASK))));
    }
    else
    {
        tmp_value = REG_RD(UART2_PIN_MUX_REG) & (~DW_UART2_GPIO_MASK);
        REG_WR(UART2_PIN_MUX_REG, (tmp_value | DW_UART2_TX | DW_UART2_RX));//gpio6/gpio7 for a0v1; gpio12/gpio13 for a0v2
    }

    //uart sclk enable and sclk root clk setting (XTAL)
    //fpga no effect, soc need
#ifdef LEGA_A0V1
    if(UARTx == UART0)
    {
        tmp_value = REG_RD(UART_CLK_REG_ADDR);
        REG_WR(UART_CLK_REG_ADDR, tmp_value | UART0_SCLK_EN | UART_SCLK_ROOT_SEL_XTAL);
    }
    else if(UARTx == UART1)
    {
        tmp_value = REG_RD(UART_CLK_REG_ADDR);
        REG_WR(UART_CLK_REG_ADDR, tmp_value | UART1_SCLK_EN | UART_SCLK_ROOT_SEL_XTAL);
    }
    else
    {
        tmp_value = REG_RD(UART_CLK_REG_ADDR);
        REG_WR(UART_CLK_REG_ADDR, tmp_value | UART2_SCLK_EN | UART_SCLK_ROOT_SEL_XTAL);
    }
#else //LEGA_A0V2
    if(UARTx==UART0)
    {
        REG_WR(UART_CLK_REG_ADDR,REG_RD(UART_CLK_REG_ADDR)|UART0_SCLK_EN);
    }
    else if(UARTx==UART1)
    {
        REG_WR(UART_CLK_REG_ADDR,REG_RD(UART_CLK_REG_ADDR)|UART1_SCLK_EN);
    }
    else
    {
        REG_WR(UART_CLK_REG_ADDR,REG_RD(UART_CLK_REG_ADDR)|UART2_SCLK_EN);
    }
#endif

#ifdef LEGA_A0V1
    //uart clk rst
    if(UARTx == UART1)
    {
        REG_WR(UART1_CLK_RST_REG_ADDR, (UART1_HCLK_RESET | UART1_SCLK_RESET));
        delay(20);
        REG_WR(UART1_CLK_RST_REG_ADDR, 0x00);
    }
    else
    {
        //SRR soft reset
        UARTx->SRR = DW_UART_SRR_UR;
    }
#else
    //SRR soft reset
    UARTx->SRR = DW_UART_SRR_UR;
#endif

    //disable all uart interrupt
    UARTx->IER=DW_UART_DISABLE_ALL_IRQ;

    //check busy
    while(UARTx->USR & DW_UART_USR_BUSY);

    dll= UART_BASE_CLOCK/(baudrate*16);
    dll= ((UART_BASE_CLOCK - dll*(baudrate*16)) < (baudrate*16)/2) ? dll : (dll+1);  //minus four , add five

    //set latch for dll,set baud
    UARTx->LCR |= DW_UART_LCR_DLAB;
    //for baudrate=115200, we set it to 1 because hw has done pll divider
    UARTx->DLL = dll&0xFF;
    UARTx->DLH = (dll>>8)&0xFF;
    //clear bit 7 of dll latch
    UARTx->LCR &=~(DW_UART_LCR_DLAB);

    //set dps
    UARTx->LCR &=(~DW_UART_LCR_DPS_MASK);
    //bit[0:1]=11 means 8 bits data size and bit[2]=0 means 1 bit stop
    UARTx->LCR |= data_width|stop_bit|parity_flag;

    UARTx->MCR |= flow_control;
    //enable  FIFO
    UARTx->FCR |= DW_UART_FCR_FIFO_ENABLE;
    //enable rx interrupt
    UARTx->IER = DW_UART_IER_RX_INT_EN;

    //enable cm4 interrupt
    if(UARTx==UART0)
    {
        NVIC_EnableIRQ(UART0_IRQn);
    }
    else if(UARTx==UART1)
    {
        NVIC_EnableIRQ(UART1_IRQn);
    }
    else
    {
        NVIC_EnableIRQ(UART2_IRQn);
    }

    if(uart->priv)
    {
        g_lega_uart_callback_handler[uart->port] = (lega_uart_callback_func)(uart->priv);
    }
    return 0;
}

/**
 * Transmit data on a UART interface
 *
 * @param[in]  uart  the UART interface
 * @param[in]  data  pointer to the start of data
 * @param[in]  size  number of bytes to transmit
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t lega_uart_send(lega_uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    UART_TypeDef* UARTx;
    int i = 0;

    if((NULL == uart) || (NULL == data))
    {
        return -1;
    }

    if(LEGA_UART0_INDEX == uart->port)
    {
        UARTx = UART0;
    }
    else if(LEGA_UART1_INDEX == uart->port)
    {
        UARTx = UART1;
    }
    else if(LEGA_UART2_INDEX == uart->port)
    {
        UARTx = UART2;
    }
    else
    {
        return -1;
    }

    for(i = 0; i < size; i++)
    {
        UART_SendData(UARTx,((uint8_t*)data)[i]);
    }

    return 0;
}

/**
 * Deinitialises a UART interface
 *
 * @param[in]  uart  the interface which should be deinitialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t lega_uart_finalize(lega_uart_dev_t *uart)
{
    UART_TypeDef* UARTx;
    unsigned int tmp_value;

    if(NULL == uart)
    {
        return -1;
    }

    if(LEGA_UART0_INDEX == uart->port)
    {
        UARTx = UART0;
    }
    else if(LEGA_UART1_INDEX == uart->port)
    {
        UARTx = UART1;
    }
    else if(LEGA_UART2_INDEX == uart->port)
    {
        UARTx = UART2;
    }
    else
    {
        return -1;
    }

    //disable all uart interrupt
    UARTx->IER = DW_UART_DISABLE_ALL_IRQ;

    //disable cm4 interrupt
    if(UART0 == UARTx)
    {
        NVIC_DisableIRQ(UART0_IRQn);
    }
    else if(UART1 == UARTx)
    {
        NVIC_DisableIRQ(UART1_IRQn);
    }
    else
    {
        NVIC_DisableIRQ(UART2_IRQn);
    }

    //uart sclk disable, fpga no effect, soc need
    if(UART0 == UARTx)
    {
        REG_WR(UART_CLK_REG_ADDR, REG_RD(UART_CLK_REG_ADDR)&(~UART0_SCLK_EN));
    }
    else if(UART1 == UARTx)
    {
        REG_WR(UART_CLK_REG_ADDR, REG_RD(UART_CLK_REG_ADDR)&(~UART1_SCLK_EN));
    }
    else
    {
        REG_WR(UART_CLK_REG_ADDR, REG_RD(UART_CLK_REG_ADDR)&(~UART2_SCLK_EN));
    }

    //uart pin mux configure to default gpio
    if(UARTx == UART0)
    {
        tmp_value = REG_RD(UART0_PIN_MUX_REG) & (~DW_UART0_GPIO_MASK);
        REG_WR(UART0_PIN_MUX_REG, tmp_value);//gpio0/gpio1
    }
    else if(UARTx == UART1)
    {
        tmp_value = REG_RD(UART1_PIN_MUX_REG) & (~DW_UART1_GPIO_MASK);
        REG_WR(UART1_PIN_MUX_REG, tmp_value);//gpio2/gpio3
    }
    else
    {
        tmp_value = REG_RD(UART2_PIN_MUX_REG) & (~DW_UART2_GPIO_MASK);
        REG_WR(UART2_PIN_MUX_REG, tmp_value);//gpio6/gpio7 for a0v1; gpio12/gpio13 for a0v2
    }

    g_lega_uart_callback_handler[uart->port] = NULL;
    return 0;
}

