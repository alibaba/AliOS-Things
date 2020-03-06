#include <stdio.h>
#include "bootloader.h"
#include "include.h"
#include "uart_pub.h"
#include "sys_ctrl_pub.h"
#include "arm_arch.h"
#include "uart.h"

#define IRQ_UART2               (1)
#define IRQ_UART1               (0)
#define PRI_IRQ_UART2           (25)
#define PRI_IRQ_UART1           (26)

#define CLKGATE_UART2_APB_BIT   (1 << 2)
#define CLKGATE_UART1_APB_BIT   (1 << 1)
#define IRQ_UART2_BIT           (1 << 1)
#define IRQ_UART1_BIT           (1 << 0)
#define UART2_ARM_WAKEUP_EN_BIT (1 << 1)
#define UART1_ARM_WAKEUP_EN_BIT (1 << 0)

UART_S uart[2] =
{
    {0, 0, 0},
    {0, 0, 0}
};
uart_config_t uart_0;

#if CFG_BACKGROUND_PRINT
INT32 uart_printf(const char *fmt, ...)
{
    INT32 rc;
    char buf[TX_RB_LENGTH];

    va_list args;
    va_start(args, fmt);
    rc = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    buf[sizeof(buf) - 1] = '\0';
    if((rc < sizeof(buf) - 2)
            && (buf[rc - 1] == '\n')
            && (buf[rc - 2] != '\r'))
    {
        buf[rc - 1] = '\r';
        buf[rc] = '\n';
        buf[rc + 1] = 0;

        rc += 1;
    }

    return kfifo_put(uart[1].tx, (UINT8 *)&buf[0], rc);
}
#endif // CFG_BACKGROUND_PRINT

void fatal_print(const char *fmt, ...)
{
    os_printf(fmt);

    DEAD_WHILE();
}

void uart_hw_init(UINT8 uport)
{
    UINT32 reg, baud_div;
    UINT32 conf_reg_addr, fifo_conf_reg_addr;
    UINT32 flow_conf_reg_addr, wake_conf_reg_addr, intr_reg_addr;

    baud_div = UART_CLOCK / UART_BAUD_RATE;
    baud_div = baud_div - 1;

    if(UART1_PORT == uport)
    {
        conf_reg_addr = REG_UART1_CONFIG;
        fifo_conf_reg_addr = REG_UART1_FIFO_CONFIG;
        flow_conf_reg_addr = REG_UART1_FLOW_CONFIG;
        wake_conf_reg_addr = REG_UART1_WAKE_CONFIG;
        intr_reg_addr = REG_UART1_INTR_ENABLE;
    }
    else
    {
        conf_reg_addr = REG_UART2_CONFIG;
        fifo_conf_reg_addr = REG_UART2_FIFO_CONFIG;
        flow_conf_reg_addr = REG_UART2_FLOW_CONFIG;
        wake_conf_reg_addr = REG_UART2_WAKE_CONFIG;
        intr_reg_addr = REG_UART2_INTR_ENABLE;
    }

    reg = UART_TX_ENABLE
          | UART_RX_ENABLE
          & (~UART_IRDA)
          | ((DEF_DATA_LEN & UART_DATA_LEN_MASK) << UART_DATA_LEN_POSI)
          & (~UART_PAR_EN)
          & (~UART_PAR_ODD_MODE)
          & (~UART_STOP_LEN_2)
          | ((baud_div & UART_CLK_DIVID_MASK) << UART_CLK_DIVID_POSI);
    REG_WRITE(conf_reg_addr, reg);

    reg = ((TX_FIFO_THRD & TX_FIFO_THRESHOLD_MASK) << TX_FIFO_THRESHOLD_POSI)
          | ((RX_FIFO_THRD & RX_FIFO_THRESHOLD_MASK) << RX_FIFO_THRESHOLD_POSI)
          | ((RX_STOP_DETECT_TIME32 & RX_STOP_DETECT_TIME_MASK) << RX_STOP_DETECT_TIME_POSI);
    REG_WRITE(fifo_conf_reg_addr, reg);

    REG_WRITE(flow_conf_reg_addr, 0);
    REG_WRITE(wake_conf_reg_addr, 0);

    reg = RX_FIFO_NEED_READ_EN | UART_RX_STOP_END_EN;
    REG_WRITE(intr_reg_addr, reg);

    return;
}

void uart_hw_set_change(UINT8 uport, uart_config_t *uart_config)
{
    UINT32 reg, baud_div, width;

    hal_uart_parity_t       parity_en;
    hal_uart_stop_bits_t    stop_bits;
    hal_uart_flow_control_t flow_control;
    UINT8 parity_mode = 0;
    UINT32 intr_ena_reg_addr, conf_reg_addr, fifi_conf_reg_addr;
    UINT32 flow_conf_reg_addr, wake_reg_addr;

    if(UART1_PORT == uport)
    {
        intr_ena_reg_addr = REG_UART1_INTR_ENABLE;
        conf_reg_addr = REG_UART1_CONFIG;
        fifi_conf_reg_addr = REG_UART1_FIFO_CONFIG;
        flow_conf_reg_addr = REG_UART1_FLOW_CONFIG;
        wake_reg_addr = REG_UART1_WAKE_CONFIG;
    }
    else
    {
        intr_ena_reg_addr = REG_UART2_INTR_ENABLE;
        conf_reg_addr = REG_UART2_CONFIG;
        fifi_conf_reg_addr = REG_UART2_FIFO_CONFIG;
        flow_conf_reg_addr = REG_UART2_FLOW_CONFIG;
        wake_reg_addr = REG_UART2_WAKE_CONFIG;
    }
    REG_WRITE(intr_ena_reg_addr, 0);//disable int

    baud_div = UART_CLOCK / uart_config->baud_rate;
    baud_div = baud_div - 1;
    width = uart_config->data_width;
    parity_en = uart_config->parity;
    stop_bits = uart_config->stop_bits;
    flow_control = uart_config->flow_control;

    if(parity_en)
    {

        if(parity_en == ODD_PARITY)
            parity_mode = 1;
        else
            parity_mode = 0;
        parity_en = 1;
    }

    reg = UART_TX_ENABLE
          | UART_RX_ENABLE
          & (~UART_IRDA)
          | ((width & UART_DATA_LEN_MASK) << UART_DATA_LEN_POSI)
          | (parity_en << 5)
          | (parity_mode << 6)
          | (stop_bits << 7)
          | ((baud_div & UART_CLK_DIVID_MASK) << UART_CLK_DIVID_POSI);

    width = ((width & UART_DATA_LEN_MASK) << UART_DATA_LEN_POSI);
    REG_WRITE(conf_reg_addr, reg);

    reg = ((TX_FIFO_THRD & TX_FIFO_THRESHOLD_MASK) << TX_FIFO_THRESHOLD_POSI)
          | ((RX_FIFO_THRD & RX_FIFO_THRESHOLD_MASK) << RX_FIFO_THRESHOLD_POSI)
          | ((RX_STOP_DETECT_TIME32 & RX_STOP_DETECT_TIME_MASK) << RX_STOP_DETECT_TIME_POSI);
    REG_WRITE(fifi_conf_reg_addr, reg);

    REG_WRITE(flow_conf_reg_addr, 0);
    REG_WRITE(wake_reg_addr, 0);

    reg = RX_FIFO_NEED_READ_EN | UART_RX_STOP_END_EN;
    REG_WRITE(intr_ena_reg_addr, reg);
}

INT32 os_null_printf(const char *fmt, ...)
{
    return 0;
}

int uart_read_byte(int uport)
{
    int val = -1;
    UINT32 fifo_status_reg;

    if (UART1_PORT == uport)
        fifo_status_reg = REG_UART1_FIFO_STATUS;
    else
        fifo_status_reg = REG_UART2_FIFO_STATUS;

    if (REG_READ(fifo_status_reg) & FIFO_RD_READY)
        UART_READ_BYTE(uport, val);

    return val;
}

int uart_write_byte(int uport, char c)
{
    if (UART1_PORT == uport)
        while(!UART1_TX_WRITE_READY);
    else
        while(!UART2_TX_WRITE_READY);

    UART_WRITE_BYTE(uport, c);

    return (1);
}


int uart_try_read_byte(int uport, unsigned char *c)
{
    unsigned char val;
    UINT32 fifo_status_reg;

    if (UART1_PORT == uport)
        fifo_status_reg = REG_UART1_FIFO_STATUS;
    else
        fifo_status_reg = REG_UART2_FIFO_STATUS;

    if (REG_READ(fifo_status_reg) & FIFO_RD_READY) {
        UART_READ_BYTE(uport, val);
        *c = val;
        return 1;
    }
    return 0;
}

int uart_try_write_byte(int uport, char c)
{
    if (UART1_PORT == uport)
        while(!UART1_TX_WRITE_READY);
    else
        while(!UART2_TX_WRITE_READY);

    UART_WRITE_BYTE(uport, c);

    return 1;
}

int32_t hal_uart_init(uart_dev_t *uart)
{
    uart_0.baud_rate    = 115200;
    uart_0.data_width   = DATA_WIDTH_8BIT;
    uart_0.parity       = NO_PARITY;
    uart_0.stop_bits    = STOP_BITS_1;
    uart_0.flow_control = FLOW_CONTROL_DISABLED;

    uart_hw_set_change(UART2_PORT, &uart_0);

    return 0;
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    int i = 0;
    char* buff = NULL;

    if (data == NULL) {
        return -1;
    }

    if (size == 0) {
        return -1;
    }

    buff = (char*)data;

    for (i = 0; i < size; i++) {
        uart_try_write_byte(UART2_PORT, *(buff + i));
    }

    return 0;
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t *recv_size, uint32_t timeout)
{
    char         *buff = NULL;
    uint32_t     size = 0;

    if ((data == NULL) || (recv_size == NULL)) {
        return -1;
    }

    if (expect_size == 0) {
        return -1;
    }

    buff = data;

    while (size != expect_size) {
        if(0 == uart_try_read_byte(UART2_PORT, buff)) {
            break;
        }

        size++;
        buff++;
    }

    *recv_size = size;
    return 0;
}


