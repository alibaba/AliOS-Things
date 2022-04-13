/*

 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
   */

#include "cmsis.h"
#include "cmsis_os.h"
#define BES_HAL_DEBUG 0
#include "aos/kernel.h"
#include "bes_hal.h"
#include "aos/errno.h"
#include "aos/hal/uart.h"
#include "ulog/ulog.h"
#include "hal_uart.h"
#include "hal_trace.h"
#include "plat_types.h"
#include "hal_iomux.h"

#define UART_FIFO_MAX_BUFFER   2048
#define UART_DMA_RING_BUFFER_SIZE  256// mast be 2^n

static __SRAMBSS unsigned char _hal_uart_buf[UART_DMA_RING_BUFFER_SIZE];
static __SRAMBSS unsigned char _hal_uart1_buf[UART_DMA_RING_BUFFER_SIZE];
static __SRAMBSS unsigned char _hal_uart2_buf[UART_DMA_RING_BUFFER_SIZE];

osSemaphoreDef(rx_sem0);
osSemaphoreDef(rx_sem1);
osSemaphoreDef(rx_sem2);
osSemaphoreDef(tx_sem0);
osSemaphoreDef(tx_sem1);
osSemaphoreDef(tx_sem2);
typedef struct
{
    uint32_t init_flag;
    uint8_t use_dma;
    uint8_t *uart_buffer;
    uint8_t *rx_ringbuf;
    int32_t rxring_size;
    int32_t rxbuf_in;
    int32_t rxbuf_out;
    osSemaphoreId rx_sem;
    osSemaphoreId tx_sem;
    void (*uart_dma_rx_handler)(uint32_t xfer_size, int dma_error, union HAL_UART_IRQ_T status);
    void (*uart_dma_tx_handler)(uint32_t xfer_size, int dma_error);
}uart_ctx_obj_t;

bool uart0_dma_onoff = true;

static uart_ctx_obj_t uart_ctx[3] = {0};

struct HAL_UART_CFG_T low_uart_cfg = { // used for tgdb cli console
    .parity = HAL_UART_PARITY_NONE,
    .stop = HAL_UART_STOP_BITS_1,
    .data = HAL_UART_DATA_BITS_8,
    .flow = HAL_UART_FLOW_CONTROL_NONE,
    .tx_level = HAL_UART_FIFO_LEVEL_7_8,
    .rx_level = HAL_UART_FIFO_LEVEL_1_8,
    .baud = 0,
    .dma_rx = false,
    .dma_tx = false,
    .dma_rx_stop_on_err = false,
};

static void hal_set_uart_iomux(uint32_t uart_id)
{
    if (uart_id == HAL_UART_ID_0) {
        hal_iomux_set_uart0();
    } else if (uart_id == HAL_UART_ID_1) {
        hal_iomux_set_uart1();
    } else {
        hal_iomux_set_uart2();
    }
}

static void hal_uart_rx_start(uint32_t uart_id)
{
    struct HAL_DMA_DESC_T dma_desc_rx;
    unsigned int desc_cnt = 1;
    union HAL_UART_IRQ_T mask;

    mask.reg = 0;
    mask.BE = 0;
    mask.FE = 0;
    mask.OE = 0;
    mask.PE = 0;
    mask.RT = 1;

    hal_uart_dma_recv_mask(uart_id, uart_ctx[uart_id].uart_buffer, UART_DMA_RING_BUFFER_SIZE, &dma_desc_rx, &desc_cnt, &mask);

}

static int32_t _uart_ringbuffer_push(uint32_t uart_id, uint8_t *buf, int32_t len);

static void _uart_rx_handler(enum HAL_UART_ID_T id, union HAL_UART_IRQ_T status)
{
    unsigned char wakeup = 0;
    char data[128], c;

    int i = 0;
    while (hal_uart_readable(id)) {
        c = hal_uart_getc(id);
        if (c == '\n') {
            wakeup++;
        }
        data[i++] = c;
        if (i == 128) {
            _uart_ringbuffer_push(id, data, i);
            i = 0;
        }
    }
    if (i) {
        data[i] = 0;
        _uart_ringbuffer_push(id, data, i);
    }
    if (wakeup) {
        osSemaphoreRelease(uart_ctx[id].rx_sem);
    }
}

static int32_t _get_uart_ringbuf_freesize(uint32_t uart_id)
{
    int32_t size = 0;

    /*if uart haven't init free size is zero*/
    if (uart_ctx[uart_id].init_flag == 0) {
        return 0;
    }

    /*now input index equals output index means fifo empty*/
    if (uart_ctx[uart_id].rxbuf_in == uart_ctx[uart_id].rxbuf_out) {
        size = uart_ctx[uart_id].rxring_size;
    } else if (uart_ctx[uart_id].rxbuf_in > uart_ctx[uart_id].rxbuf_out) {
        size = uart_ctx[uart_id].rxring_size - uart_ctx[uart_id].rxbuf_in + uart_ctx[uart_id].rxbuf_out;
    } else {
        size = uart_ctx[uart_id].rxbuf_out - uart_ctx[uart_id].rxbuf_in;
    }

    return size;
}

static int32_t _get_uart_ringbuf_available_read_size(uint32_t uart_id)
{
    uint32_t size = 0;

    /*if uart haven't init free size is zero*/
    if (uart_ctx[uart_id].init_flag == 0) {
        return 0;
    }
    size = uart_ctx[uart_id].rxring_size - _get_uart_ringbuf_freesize(uart_id);

    return size;
}

/*return value is push in data size */
static int32_t _uart_ringbuffer_push(uint32_t uart_id, uint8_t *buf, int32_t len)
{
    int32_t free_size = 0;
    int32_t write_size = 0;
    int32_t spilt_len = 0;

    if (uart_id > HAL_UART_ID_2 || uart_ctx[uart_id].init_flag == 0) {
        return 0;
    }

    free_size = _get_uart_ringbuf_freesize(uart_id);

    /*get real read_size */
    if (free_size > len) {
        write_size = len;
    } else {
        write_size = free_size;
    }

    if (write_size != 0 ) {
        spilt_len = uart_ctx[uart_id].rxring_size - uart_ctx[uart_id].rxbuf_in;
        if (spilt_len >= write_size) {
            memcpy(uart_ctx[uart_id].rx_ringbuf + uart_ctx[uart_id].rxbuf_in, buf, write_size);
        } else {
            memcpy(uart_ctx[uart_id].rx_ringbuf + uart_ctx[uart_id].rxbuf_in, buf, spilt_len);
            memcpy(uart_ctx[uart_id].rx_ringbuf, buf + spilt_len, write_size - spilt_len);
        }
        uart_ctx[uart_id].rxbuf_in = (uart_ctx[uart_id].rxbuf_in + write_size) % uart_ctx[uart_id].rxring_size;
    }

    return write_size;
}

/*return value is pop out data size */
static int32_t _uart_ringbuffer_pop(uint32_t uart_id, uint8_t *buf, int32_t len)
{
    int32_t available_size = 0;
    int32_t read_size = 0;
    int32_t spilt_len = 0;

    if (uart_id > HAL_UART_ID_2 || uart_ctx[uart_id].init_flag == 0) {
        return 0;
    }

    available_size = _get_uart_ringbuf_available_read_size(uart_id);

    /*get real read_size */
    if (available_size > len) {
        read_size = len;
    } else {
        read_size = available_size;
    }

    spilt_len = uart_ctx[uart_id].rxring_size - uart_ctx[uart_id].rxbuf_out;
    if (spilt_len >= read_size) {
        memcpy(buf, uart_ctx[uart_id].rx_ringbuf + uart_ctx[uart_id].rxbuf_out, read_size);
    } else {
        memcpy(buf, uart_ctx[uart_id].rx_ringbuf + uart_ctx[uart_id].rxbuf_out, spilt_len);
        memcpy(buf + spilt_len, uart_ctx[uart_id].rx_ringbuf, read_size - spilt_len);
    }

    uart_ctx[uart_id].rxbuf_out = (uart_ctx[uart_id].rxbuf_out + read_size) % uart_ctx[uart_id].rxring_size;

    return read_size;
}

extern void panicNmiInputFilter(uint8_t ch);
static void _uart_dma_rx_handler(uint32_t xfer_size, int dma_error, union HAL_UART_IRQ_T status)
{
    uint32_t len = 0;
    uint32_t uartid = 0;

    len = _uart_ringbuffer_push(uartid, uart_ctx[uartid].uart_buffer, xfer_size);
    if (len < xfer_size) {
        printf("%s ringbuf is full have %d need %d\r", __FUNCTION__, len, xfer_size);
        return;
    }

    memset(uart_ctx[uartid].uart_buffer, 0, UART_DMA_RING_BUFFER_SIZE);
    osSemaphoreRelease(uart_ctx[uartid].rx_sem);

    hal_uart_rx_start(uartid);
}

static void _uart1_dma_rx_handler(uint32_t xfer_size, int dma_error, union HAL_UART_IRQ_T status)
{
    uint32_t len = 0;
    uint32_t uartid = 1;

    len = _uart_ringbuffer_push(uartid, uart_ctx[uartid].uart_buffer, xfer_size);
    if (len < xfer_size) {
        printf("%s ringbuf is full have %d need %d\r", __FUNCTION__, len, xfer_size);
        return;
    }

    memset(uart_ctx[uartid].uart_buffer, 0, UART_DMA_RING_BUFFER_SIZE);
    osSemaphoreRelease(uart_ctx[uartid].rx_sem);

    hal_uart_rx_start(uartid);
}

static void _uart1_dma_tx_handler(uint32_t xfer_size, int dma_error)
{
    osSemaphoreRelease(uart_ctx[1].tx_sem);
}

/*uart2*/
static void _uart2_dma_rx_handler(uint32_t xfer_size, int dma_error, union HAL_UART_IRQ_T status)
{
    uint32_t len = 0;
    uint32_t uartid = 2;

    len = _uart_ringbuffer_push(uartid, uart_ctx[uartid].uart_buffer, xfer_size);
    if (len < xfer_size) {
        printf("%s ringbuf is full have %d need %d\r", __FUNCTION__, len, xfer_size);
        return;
    }

    memset(uart_ctx[uartid].uart_buffer, 0, UART_DMA_RING_BUFFER_SIZE);

    osSemaphoreRelease(uart_ctx[uartid].rx_sem);

    hal_uart_rx_start(uartid);
}

static void _uart2_dma_tx_handler(uint32_t xfer_size, int dma_error)
{
    osSemaphoreRelease(uart_ctx[2].tx_sem);
}

static int panic_uart_open = 0;
extern int32_t g_cli_direct_read;
int32_t hal_panic_uart_open(void)
{
    enum HAL_UART_ID_T uart_id = hal_trace_get_id();
    TRACE_FLUSH();
    hal_uart_close(uart_id);
    low_uart_cfg.baud = hal_trace_get_baudrate();
    hal_uart_open(uart_id, &low_uart_cfg);
    panic_uart_open = 1;
    g_cli_direct_read = 1;

    return 0;
}
/**

 * Initialises a UART interface
   *
    *
 * @param[in]  uart  the interface which should be initialised
   *
 * @return  0 : on success, EIO : if an error occurred with any step
   */
int32_t hal_uart_init(uart_dev_t *uart)
{
    int32_t ret = 0;
    uint32_t trace_port = hal_trace_get_id();
    enum HAL_UART_ID_T uart_id;
    struct HAL_UART_CFG_T uart_cfg = {0};

    if(NULL == uart || uart->port > HAL_UART_ID_2) {
        return EIO;
    }

#ifdef AOS_COMP_IRQ
    // DONOT do any operation on all DMA channels
    //hal_gpdma_open();
#endif

    uart_id = uart->port;

    if (uart_ctx[uart_id].init_flag) {
        /*have already inited*/
        return EIO;
    }

    if (uart_id == 0) {
        uart_ctx[uart_id].use_dma = uart0_dma_onoff;
        uart_ctx[uart_id].uart_dma_rx_handler = _uart_dma_rx_handler;
        uart_ctx[uart_id].uart_buffer = _hal_uart_buf;
    }
    if (uart_id == HAL_UART_ID_1) {
        uart_ctx[uart_id].use_dma = 0;
        uart_ctx[uart_id].uart_dma_rx_handler = _uart1_dma_rx_handler;
        uart_ctx[uart_id].uart_dma_tx_handler = _uart1_dma_tx_handler;
        uart_ctx[uart_id].uart_buffer = _hal_uart1_buf;
    }
    if (uart_id == HAL_UART_ID_2) {
        uart_ctx[uart_id].use_dma = 0;
        uart_ctx[uart_id].uart_dma_rx_handler = _uart2_dma_rx_handler;
        uart_ctx[uart_id].uart_dma_tx_handler = _uart2_dma_tx_handler;
        uart_ctx[uart_id].uart_buffer = _hal_uart2_buf;
    }
    memset(uart_ctx[uart_id].uart_buffer, 0, UART_DMA_RING_BUFFER_SIZE);

    uart_cfg.baud = uart->config.baud_rate;
    uart_cfg.parity = uart->config.parity;
    uart_cfg.stop = uart->config.stop_bits;
    uart_cfg.data = uart->config.data_width;
    uart_cfg.flow = uart->config.flow_control;
    uart_cfg.tx_level = HAL_UART_FIFO_LEVEL_1_2;
    uart_cfg.rx_level = HAL_UART_FIFO_LEVEL_1_2;

    uart_cfg.dma_rx = uart_ctx[uart_id].use_dma;
    uart_cfg.dma_tx = uart_ctx[uart_id].use_dma;
    uart_cfg.dma_rx_stop_on_err = false;

    /*means it have already opened*/
    if (uart_id == trace_port) {
        if (uart_cfg.dma_rx) {
            hal_uart_close(uart_id);
        } else {
            hal_trace_close();
        }
        ret = hal_uart_open(uart_id, &uart_cfg);
    } else {
        ret = hal_uart_open(uart_id, &uart_cfg);
    }
    if (ret) {
        printf("%s %d trace port %d uart %d open fail ret %d\r\n", __FILE__, __LINE__, trace_port, uart_id, ret);
        return EIO;
    }
    hal_set_uart_iomux(uart_id);

    /*rx fifo buffer, for now fix length 2048*/
    uart_ctx[uart_id].rx_ringbuf = aos_malloc(UART_FIFO_MAX_BUFFER);
    if (NULL == uart_ctx[uart_id].rx_ringbuf) {
        printf("%s %d uart %d fail to malloc rx fifo buffer\r\n", __FILE__, __LINE__, uart_id);
        return EIO;
    }
    uart_ctx[uart_id].rxring_size = UART_FIFO_MAX_BUFFER;
    uart_ctx[uart_id].rxbuf_in = 0;
    uart_ctx[uart_id].rxbuf_out = 0;
    memset(uart_ctx[uart_id].rx_ringbuf, 0, UART_FIFO_MAX_BUFFER);

    uart_ctx[uart_id].rx_sem = osSemaphoreCreate((uart_id==0)?osSemaphore(rx_sem0):
                            ((uart_id==1)?osSemaphore(rx_sem1):osSemaphore(rx_sem2)), 0);
    if (NULL == uart_ctx[uart_id].rx_sem) {
        aos_free(uart_ctx[uart_id].rx_ringbuf);
        return EIO;
    }

    uart_ctx[uart_id].tx_sem = osSemaphoreCreate((uart_id==0)?osSemaphore(tx_sem0):
                            ((uart_id==1)?osSemaphore(tx_sem1):osSemaphore(tx_sem2)), 0);
    if (NULL == uart_ctx[uart_id].tx_sem) {
        aos_free(uart_ctx[uart_id].rx_ringbuf);
        osSemaphoreDelete(uart_ctx[uart_id].rx_sem);
        return EIO;
    }

    uart_ctx[uart_id].init_flag = 1;
    if (uart_ctx[uart_id].use_dma) {
        hal_uart_irq_set_dma_handler(uart_id, uart_ctx[uart_id].uart_dma_rx_handler,
                    uart_ctx[uart_id].uart_dma_tx_handler, NULL);

        hal_uart_rx_start(uart_id);
    } else {
        hal_uart_irq_set_handler(uart_id, _uart_rx_handler);
        union HAL_UART_IRQ_T mask;
        mask.reg = 0;
        mask.RT = 1;
        mask.RX = 1;
        hal_uart_clear_irq(uart_id, mask);
        hal_uart_irq_set_mask(uart_id, mask);
    }

    return 0;
}

/**

 * Transmit data on a UART interface
   *

 * @param[in]  uart     the UART interface

 * @param[in]  data     pointer to the start of data

 * @param[in]  size     number of bytes to transmit

 * @param[in]  timeout  timeout in milisecond, set this value to HAL_WAIT_FOREVER

 * if you want to wait forever
    *

 * @return  0 : on success, EIO : if an error occurred with any step
   */
int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    int32_t ret = EIO;
    uint8_t uart_id;

    if (NULL == uart || NULL == data || 0 == size) {
        printf("%s %d Invalid input \r\n", __FILE__, __LINE__);
        return ret;
    }

    uart_id = uart->port;

    if (uart_id > HAL_UART_ID_2) {
        printf("%s %d Invalid input \r\n", __FILE__, __LINE__);
        return ret;
    }

    if (uart_ctx[uart_id].init_flag == 0) {
        hal_trace_output_block(data, size);
        //printf("%s %d uart %d haven't init yet \r\n", __FILE__, __LINE__, uart_id);
        //ASSERT(0, "haven't init yet");
        return ret;
    }

    if (uart_id == hal_trace_get_id()) {
        if (panic_uart_open || !uart_ctx[uart_id].use_dma) {
            for (int i = 0; i < size; i++)
                hal_uart_blocked_putc(uart_id, *((char *)data + i));
        } else {
            hal_trace_output_block(data, size);
        }
    } else {
        if (uart_ctx[uart_id].use_dma) {
            hal_uart_dma_send_sync_cache(uart_id, data, size, NULL, NULL);
            osSemaphoreWait(uart_ctx[uart_id].tx_sem, osWaitForever);
        } else {
           for (int i = 0; i < size; i++)
                hal_uart_blocked_putc(uart_id, *((char *)data + i));
        }
    }

    return 0;
}

/**

 * Receive data on a UART interface
   *
 * @param[in]   uart         the UART interface
 * @param[out]  data         pointer to the buffer which will store incoming data
 * @param[in]   expect_size  number of bytes to receive
 * @param[in]   timeout      timeout in milisecond, set this value to HAL_WAIT_FOREVER
 * if you want to wait forever
    *
 * @return  0 : on success, EIO : if an error occurred with any step
   */
int32_t hal_uart_recv(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t timeout)
{
    printf("%s %d is a stub function \r\n", __FILE__, __LINE__);
    return EIO;
}

/**

 * Receive data on a UART interface
   *

 * @param[in]   uart         the UART interface

 * @param[out]  data         pointer to the buffer which will store incoming data

 * @param[in]   expect_size  number of bytes to receive

 * @param[out]  recv_size    number of bytes received

 * @param[in]   timeout      timeout in milisecond, set this value to HAL_WAIT_FOREVER

 * if you want to wait forever
    *

 * @return  0 : on success, EIO : if an error occurred with any step
   */
int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                        uint32_t *recv_size, uint32_t timeout)
{
    int32_t  ret = EIO;
    uint8_t  uart_id = 0;
    tick_t begin_time =0;
    tick_t now_time = 0;
    uint32_t fifo_pop_len = 0;
    uint32_t recved_len = 0;
    uint32_t expect_len = expect_size;

    if (NULL == uart || NULL == data || expect_size == 0) {
        printf("%s %d Invalid input \r\n", __FILE__, __LINE__);
        return ret;
    }

    uart_id = uart->port;

    if (uart_id > HAL_UART_ID_2) {
        printf("%s %d Invalid input \r\n", __FILE__, __LINE__);
        return ret;
    }

    if (uart_ctx[uart_id].init_flag == 0) {
        printf("%s %d uart %d haven't init yet \r\n", __FILE__, __LINE__, uart_id);
        return ret;
    }

    begin_time = osKernelSysTick();
    do
    {
        fifo_pop_len = _uart_ringbuffer_pop(uart_id, (uint8_t *)data + recved_len, expect_len);
        recved_len += fifo_pop_len;
        expect_len -= fifo_pop_len;

        if (recved_len >= expect_size) {
            break;
        }

        /*if reaches here, it means need to wait for more data come*/
        osSemaphoreWait(uart_ctx[uart_id].rx_sem, osWaitForever);
        /*time out break*/
        now_time = osKernelSysTick();
        if((uint32_t)(now_time - begin_time) >= timeout) {
            break;
        }

    } while (1);

    /*haven't get any data from fifo */
    if (recved_len == 0) {
        return EIO;
    }

    if (recv_size != NULL) {
        *recv_size = recved_len;
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
int32_t hal_uart_finalize(uart_dev_t *uart)
{
    int32_t ret = EIO;
    uint8_t uart_id;

    if (NULL == uart) {
        printf("%s %d Invalid input \r\n", __FILE__, __LINE__);
        return ret;
    }

    uart_id = uart->port;

    if (uart_id > HAL_UART_ID_2) {
        printf("%s %d Invalid input \r\n", __FILE__, __LINE__);
        return ret;
    }

    if (uart_ctx[uart_id].init_flag == 0) {
        return 0;
    }

    if (uart_id == hal_trace_get_id()) {
        //hal_uart_close(uart->port);
        printf("do nothings since we need uart!\n");
        return 0;
    }

    aos_free(uart_ctx[uart_id].rx_ringbuf);
    osSemaphoreDelete(uart_ctx[uart_id].rx_sem);
    osSemaphoreDelete(uart_ctx[uart_id].tx_sem);
    memset(&uart_ctx[uart_id], 0, sizeof(uart_ctx_obj_t));

    return 0;
}