/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include "bootloader.h"

/* uart port for debug */
uart_dev_t uart0;

/* uart port for download */
uart_dev_t uart1;

static wdg_dev_t wdg;

int uart_init(void)
{
    int ret = 0;

    memset(&uart0, 0, sizeof(uart0));
    memset(&uart1, 0, sizeof(uart1));

    uart0.port = 0;
    uart0.config.baud_rate = 115200;
    uart0.config.data_width = DATA_WIDTH_8BIT;
    uart0.config.flow_control = FLOW_CONTROL_DISABLED;
    uart0.config.mode = MODE_TX_RX;
    uart0.config.parity = NO_PARITY;
    uart0.config.stop_bits = STOP_BITS_1;
    ret = hal_uart_init(&uart0);
    if (ret != 0) {
        return -1;
    }

    uart1.port = 1;
    uart1.config.baud_rate = 115200;
    uart1.config.data_width = DATA_WIDTH_8BIT;
    uart1.config.flow_control = FLOW_CONTROL_DISABLED;
    uart1.config.mode = MODE_TX_RX;
    uart1.config.parity = NO_PARITY;
    uart1.config.stop_bits = STOP_BITS_1;
    ret = hal_uart_init(&uart1);
    if (ret != 0) {
        return -1;
    }

    return 0;
}

int uart_recv_byte(unsigned char *c)
{
    int      ret = 0;
    uint32_t size = 0;
    uint32_t expect_size = 1;
    unsigned char tc = 0;

    if (c == NULL) {
        return 0;
    }

    ret = hal_uart_recv_II(&uart1, &tc, expect_size, &size, UART_DEFAULT_TIMEOUT);
    if ((ret == 0) && (size == expect_size)) {
        *c = tc;
        return 1;
    }

    return 0;
}

void uart_send_byte(unsigned char c)
{
    (void)hal_uart_send(&uart1, &c, 1, UART_DEFAULT_TIMEOUT);
}

void uart_send_string(char *buff)
{
    while (*buff != '\0') {
        (void)hal_uart_send(&uart0, buff++, 1, UART_DEFAULT_TIMEOUT);
    }
}

int flash_get_partition(uint32_t addr, uint32_t len, hal_partition_t* no, uint32_t* offset)
{
    hal_partition_t index = 0;
    hal_logic_partition_t *partition;

    for (index = 0; index < HAL_PARTITION_MAX; index++) {
        partition = flash_get_info(index);
        if (partition == NULL) {
            continue;
        }

        if ((addr >= partition->partition_start_addr) &&
            (addr < (partition->partition_start_addr + partition->partition_length)) &&
            ((addr + len) >= partition->partition_start_addr) &&
            ((addr + len) < (partition->partition_start_addr + partition->partition_length))) {

            *no = index;
            *offset = addr - partition->partition_start_addr;
            return 0;
        }
    }

    return -1;
}

int flash_read_data(unsigned int addr, unsigned char *buffer, unsigned int len)
{
    int      ret = 0;
    uint32_t offset = 0;

    hal_partition_t index;

    ret = flash_get_partition(addr, len, &index, &offset);
    if (ret != 0) {
        return -1;
    }

    return hal_flash_read(index, &offset, (void *)buffer, len);
}

int flash_write_data(unsigned int addr, unsigned char *buffer, unsigned int len)
{
    int      ret = 0;
    uint32_t offset = 0;

    hal_partition_t index;

    ret = flash_get_partition(addr, len, &index, &offset);
    if (ret != 0) {
        return -1;
    }

    return hal_flash_write(index, &offset, (const void *)buffer, len);
}

int flash_erase(unsigned int addr,  unsigned int size)
{
    int      ret = 0;
    uint32_t offset = 0;

    hal_partition_t index;

    ret = flash_get_partition(addr, size, &index, &offset);
    if (ret != 0) {
        return -1;
    }

    return hal_flash_erase(index, offset, size);
}

void boot_wdg_init(unsigned int ms)
{
    int ret = 0;

    wdg.config.timeout = ms;
    ret = hal_wdg_init(&wdg);
    if (ret != 0) {
        printf("hal_wdg_init fail timeout = %d\n\r", wdg.config.timeout);
    }
}

void boot_wdg_reload(void)
{
    hal_wdg_reload(&wdg);
}

void boot_wdg_finalize(void)
{
    int ret = hal_wdg_finalize(&wdg);
    if (ret != 0) {
        printf("hal_wdg_finalize fail\n\r");
    }
}

void boot_reset(void)
{
    hal_sys_reboot();
}

void boot_delay(unsigned int ms)
{
    hal_sys_delay(ms);
}
