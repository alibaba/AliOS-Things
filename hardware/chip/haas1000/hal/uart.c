/*

 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
   */

#include "cmsis.h"
#include "cmsis_os.h"
#define BES_HAL_DEBUG 0
#include "aos/kernel.h"
#include "k_api.h"
#include "app_hal.h"
#include "aos/errno.h"
#include "aos/hal/uart.h"
#include "ulog/ulog.h"
#include "hal_uart.h"
#include "hal_trace.h"
#include "plat_types.h"

#define UART_FIFO_MAX_BUFFER   2048
#define UART_DMA_RING_BUFFER_SIZE  256// mast be 2^n

static __SRAMBSS unsigned char _hal_uart_buf[UART_DMA_RING_BUFFER_SIZE];
static __SRAMBSS unsigned char _hal_uart1_buf[UART_DMA_RING_BUFFER_SIZE];
static __SRAMBSS unsigned char _hal_uart2_buf[UART_DMA_RING_BUFFER_SIZE];

typedef struct
{
    uint32_t init_flag;
    uint8_t *uart_buffer;
    uint8_t *rx_ringbuf;
    int32_t rxring_size;
    int32_t rxbuf_in;
    int32_t rxbuf_out;
    ksem_t rx_sem;
    ksem_t tx_sem;
    ksem_t rx_irq_bottom_sem;
    kmutex_t rx_cb_mutex;
    uart_rx_cb rx_cb;
    uart_dev_t *rx_cb_arg;
    ktask_t *rx_irq_bottom_task;
    void (*uart_dma_rx_handler)(uint32_t xfer_size, int dma_error, union HAL_UART_IRQ_T status);
    void (*uart_dma_tx_handler)(uint32_t xfer_size, int dma_error);
}uart_ctx_obj_t;

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
    krhino_sem_give(&(uart_ctx[uartid].rx_sem));
    krhino_sem_give(&(uart_ctx[uartid].rx_irq_bottom_sem));
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
    krhino_sem_give(&(uart_ctx[uartid].rx_sem));
    krhino_sem_give(&(uart_ctx[uartid].rx_irq_bottom_sem));

    hal_uart_rx_start(uartid);

}

static void _uart1_dma_tx_handler(uint32_t xfer_size, int dma_error)
{
    krhino_sem_give(&(uart_ctx[1].tx_sem));
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

    krhino_sem_give(&(uart_ctx[uartid].rx_sem));
    krhino_sem_give(&(uart_ctx[uartid].rx_irq_bottom_sem));

    hal_uart_rx_start(uartid);

}

static void _uart2_dma_tx_handler(uint32_t xfer_size, int dma_error)
{
    krhino_sem_give(&(uart_ctx[2].tx_sem));
}

static void rx_irq_bottom(void *arg)
{
    uart_ctx_obj_t *uart_ctx = arg;

    while (1) {
        krhino_sem_take(&uart_ctx->rx_irq_bottom_sem, RHINO_WAIT_FOREVER);
        krhino_mutex_lock(&uart_ctx->rx_cb_mutex, RHINO_WAIT_FOREVER);
        if (uart_ctx->rx_cb)
            uart_ctx->rx_cb(uart_ctx->rx_cb_arg);
        krhino_mutex_unlock(&uart_ctx->rx_cb_mutex);
    }
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

    uart_id = uart->port;

    if (uart_ctx[uart_id].init_flag) {
        /*have already inited*/
        return EIO;
    }

    if (uart_id == 0) {
        uart_ctx[uart_id].uart_dma_rx_handler = _uart_dma_rx_handler;
        uart_ctx[uart_id].uart_buffer = _hal_uart_buf;
    }
    if (uart_id == HAL_UART_ID_1) {
        uart_ctx[uart_id].uart_dma_rx_handler = _uart1_dma_rx_handler;
        uart_ctx[uart_id].uart_dma_tx_handler = _uart1_dma_tx_handler;
        uart_ctx[uart_id].uart_buffer = _hal_uart1_buf;
    }
    if (uart_id == HAL_UART_ID_2) {
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
    uart_cfg.dma_rx = true;
    uart_cfg.dma_tx = true;
    uart_cfg.dma_rx_stop_on_err = false;

    /*means it have already opened*/
    if (uart_id == trace_port) {
        hal_uart_close(uart_id);
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

    ret = krhino_sem_create(&uart_ctx[uart_id].rx_sem, "aos", 0);
    if (ret != RHINO_SUCCESS) {
        aos_free(uart_ctx[uart_id].rx_ringbuf);
        return EIO;
    }

    ret = krhino_sem_create(&uart_ctx[uart_id].tx_sem, "aos", 0);
    if (ret != RHINO_SUCCESS) {
        aos_free(uart_ctx[uart_id].rx_ringbuf);
        krhino_sem_del(&uart_ctx[uart_id].rx_sem);
        return EIO;
    }

    ret = krhino_sem_create(&uart_ctx[uart_id].rx_irq_bottom_sem, "aos", 0);
    if (ret != RHINO_SUCCESS) {
        aos_free(uart_ctx[uart_id].rx_ringbuf);
        krhino_sem_del(&uart_ctx[uart_id].tx_sem);
        krhino_sem_del(&uart_ctx[uart_id].rx_sem);
        return EIO;
    }

    krhino_mutex_create(&uart_ctx[uart_id].rx_cb_mutex, "uart_rx_cb");
    uart_ctx[uart_id].rx_cb = NULL;
    uart_ctx[uart_id].rx_cb_arg = NULL;
    ret = krhino_task_dyn_create(&uart_ctx[uart_id].rx_irq_bottom_task,
                                 "uart_rx_irq_bottom", &uart_ctx[uart_id],
                                 20, 0, 1024, rx_irq_bottom, 1);
    if (ret != RHINO_SUCCESS) {
        aos_free(uart_ctx[uart_id].rx_ringbuf);
        krhino_sem_del(&uart_ctx[uart_id].tx_sem);
        krhino_sem_del(&uart_ctx[uart_id].rx_sem);
        krhino_sem_del(&uart_ctx[uart_id].rx_irq_bottom_sem);
        krhino_mutex_del(&uart_ctx[uart_id].rx_cb_mutex);
        return EIO;
    }

    uart_ctx[uart_id].init_flag = 1;

    hal_uart_irq_set_dma_handler(uart_id, uart_ctx[uart_id].uart_dma_rx_handler,
                    uart_ctx[uart_id].uart_dma_tx_handler, NULL);

    hal_uart_rx_start(uart_id);

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
        printf("%s %d uart %d haven't init yet \r\n", __FILE__, __LINE__, uart_id);
        return ret;
    }

    if (uart_id == hal_trace_get_id()) {
        if (panic_uart_open) {
            for (int i = 0; i < size; i++)
                hal_uart_blocked_putc(uart_id, *((char *)data + i));
        } else {
            hal_trace_output_block(data, size);
        }
    } else {
        hal_uart_dma_send_sync_cache(uart_id, data, size, NULL, NULL);
        krhino_sem_take(&uart_ctx[uart_id].tx_sem, krhino_ms_to_ticks(timeout));
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

    begin_time = krhino_sys_tick_get();
    do
    {
        fifo_pop_len = _uart_ringbuffer_pop(uart_id, (uint8_t *)data + recved_len, expect_len);
        recved_len += fifo_pop_len;
        expect_len -= fifo_pop_len;

        if (recved_len >= expect_size) {
            break;
        }

        /*if reaches here, it means need to wait for more data come*/
        krhino_sem_take(&uart_ctx[uart_id].rx_sem, krhino_ms_to_ticks(timeout));
        /*time out break*/
        now_time = krhino_sys_tick_get();
        if((uint32_t)(now_time - begin_time) >= timeout){
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

 * release sem for uart rx port
   *

 * @param[in]   uartid    index of uart
    *

 * @return  0 : on success, Others : if an error occurred with any step
   */
kstat_t hal_uart_rx_sem_give(int uartid)
{
    return krhino_sem_give(&(uart_ctx[uartid].rx_sem));
}

/**

 * take sem for uart rx port
   *

 * @param[in]  uartid  index of uart

* @param[in]   timeout      timeout in milisecond, set this value to HAL_WAIT_FOREVER

 * if you want to wait forever
   *

 * @return  0 : on success, Others : if an error occurred with any step
   */
kstat_t hal_uart_rx_sem_take(int uartid, int timeout)
{
    return krhino_sem_take(&uart_ctx[uartid].rx_sem, krhino_ms_to_ticks(timeout));
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

    hal_uart_close(uart->port);
    aos_free(uart_ctx[uart_id].rx_ringbuf);
    krhino_sem_del(&uart_ctx[uart_id].rx_sem);
    krhino_sem_del(&uart_ctx[uart_id].tx_sem);
    krhino_task_dyn_del(uart_ctx[uart_id].rx_irq_bottom_task);
    uart_ctx[uart_id].rx_irq_bottom_task = NULL;
    krhino_sem_del(&uart_ctx[uart_id].rx_irq_bottom_sem);
    krhino_mutex_del(&uart_ctx[uart_id].rx_cb_mutex);
    uart_ctx[uart_id].rx_cb = NULL;
    uart_ctx[uart_id].rx_cb_arg = NULL;
    memset(&uart_ctx[uart_id], 0, sizeof(uart_ctx_obj_t));

    ret = 0;

    return ret;
}

int32_t hal_uart_recv_cb_reg(uart_dev_t *uart, uart_rx_cb cb)
{
    int id;

    if (!uart || uart->port > HAL_UART_ID_2)
        return -1;

    id = uart->port;
    krhino_mutex_lock(&uart_ctx[id].rx_cb_mutex, RHINO_WAIT_FOREVER);
    uart_ctx[id].rx_cb = cb;
    uart_ctx[id].rx_cb_arg = cb ? uart : NULL;
    krhino_mutex_unlock(&uart_ctx[id].rx_cb_mutex);

    return 0;
}
