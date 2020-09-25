/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "cmsis.h"
#include "cmsis_os.h"
#define BES_HAL_DEBUG 0
#include "bes_hal.h"
#include "aos/errno.h"
#include "aos/hal/uart.h"
#include "hal_uart.h"
#include "hal_trace.h"
#include "plat_types.h"
#include "aos/kernel.h"
#include "kfifo.h"

#define UART_ID_0      HAL_UART_ID_0

#if (CHIP_HAS_UART > 1)
#define UART_ID_1      HAL_UART_ID_1
#else
#define UART_ID_1      HAL_UART_ID_0
#endif

#if (CHIP_HAS_UART > 2)
#define UART_ID_2      HAL_UART_ID_2
#else
#define UART_ID_2      HAL_UART_ID_0
#endif

#define UART_FIFO_MAX_BUFFER   2048
#define _CLI_RING_BUFFER_SIZE  512// mast be 2^n
static __SRAMBSS unsigned char _hal_uart_buf[_CLI_RING_BUFFER_SIZE];
static __SRAMBSS unsigned char _hal_uart1_buf[_CLI_RING_BUFFER_SIZE];
static __SRAMBSS unsigned char _hal_uart2_buf[_CLI_RING_BUFFER_SIZE];

static int _hal_uart_size = 0;
static int _hal_uart1_size = 0;
static int _hal_uart2_size = 0;
static int _hal_uart_read_offset = 0;
static int _hal_uart1_read_offset = 0;
static int _hal_uart2_read_offset = 0;

typedef struct console_sema {
    osSemaphoreId sema_id;
    osSemaphoreDef_t def;
    ksem_t	sem;
} _cli_sema_t;

static _cli_sema_t cli_sema;
static _cli_sema_t cli1_sema_rx;
static _cli_sema_t cli1_sema_tx;
static _cli_sema_t cli2_sema_rx;
static _cli_sema_t cli2_sema_tx;

typedef struct
{
    uint32_t uart_port;
    char *uart_buffer;
    struct kfifo *uart_fifo;
    void *uart_fifo_buff;
    void (*uart_dma_rx_handler)(uint32_t xfer_size, int dma_error, union HAL_UART_IRQ_T status);
    void (*uart_dma_tx_handler)(uint32_t xfer_size, int dma_error);
}uart_ctx_obj_t;

static void hal_uart_rx_start(uint32_t uart_id);
static void _uart_dma_rx_handler(uint32_t xfer_size, int dma_error, union HAL_UART_IRQ_T status);
static void _uart1_dma_rx_handler(uint32_t xfer_size, int dma_error, union HAL_UART_IRQ_T status);
static void _uart2_dma_rx_handler(uint32_t xfer_size, int dma_error, union HAL_UART_IRQ_T status);
//static osSemaphoreId write_sem = NULL;
//static osSemaphoreId read_sem = NULL;
static struct HAL_UART_CFG_T high_uart_cfg = { // used for trace/tgdb pull/push
    .parity = HAL_UART_PARITY_NONE,
    .stop = HAL_UART_STOP_BITS_1,
    .data = HAL_UART_DATA_BITS_8,
    .flow = HAL_UART_FLOW_CONTROL_NONE,
    .tx_level = HAL_UART_FIFO_LEVEL_1_2,
    .rx_level = HAL_UART_FIFO_LEVEL_1_2,
    .baud = 0,
    .dma_rx = true,
    .dma_tx = true,
    .dma_rx_stop_on_err = false,
};
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

//static int uart2_id = -1;
//static int uart1_id = -1;

static void hal_set_uart(uint32_t uart_id)
{
    if (uart_id == UART_ID_0) {
        hal_iomux_set_uart0();
    } else if (uart_id == UART_ID_1) {
        hal_iomux_set_uart1();
    } else {
        hal_iomux_set_uart2();
    }
}

extern void panicNmiInputFilter(uint8_t ch);

static void _uart1_dma_tx_handler(uint32_t xfer_size, int dma_error)
{
    osSemaphoreRelease(cli1_sema_tx.sema_id);
}

static void _uart2_dma_tx_handler(uint32_t xfer_size, int dma_error)
{
    osSemaphoreRelease(cli2_sema_tx.sema_id);
}

static uart_ctx_obj_t uart_ctx[3] =
{
    {
        .uart_fifo = NULL,
        .uart_buffer = &_hal_uart_buf,
        .uart_fifo_buff = NULL,
        .uart_dma_rx_handler = _uart_dma_rx_handler,
        .uart_dma_tx_handler = NULL,
    },
    {
        .uart_fifo = NULL,
        .uart_buffer = &_hal_uart1_buf,
        .uart_fifo_buff = NULL,
        .uart_dma_rx_handler = _uart1_dma_rx_handler,
        .uart_dma_rx_handler = _uart1_dma_tx_handler,
    },
    {
        .uart_fifo = NULL,
        .uart_buffer = &_hal_uart2_buf,
        .uart_fifo_buff = NULL,
        .uart_dma_rx_handler = _uart2_dma_rx_handler,
        .uart_dma_tx_handler = _uart2_dma_tx_handler,
    }
};
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

    hal_uart_dma_recv_mask(uart_id, uart_ctx[uart_id].uart_buffer, _CLI_RING_BUFFER_SIZE, &dma_desc_rx, &desc_cnt, &mask);
}

static void _uart_dma_rx_handler(uint32_t xfer_size, int dma_error, union HAL_UART_IRQ_T status)
{
    //_hal_uart_read_offset = 0;
    //_hal_uart_size = xfer_size;
    uint32_t len = 0;
    if (xfer_size == 5) {
        panicNmiInputFilter(_hal_uart_buf[0]);
        panicNmiInputFilter(_hal_uart_buf[1]);
        panicNmiInputFilter(_hal_uart_buf[2]);
        panicNmiInputFilter(_hal_uart_buf[3]);
    } else if (xfer_size == 2) {
        panicNmiInputFilter(_hal_uart_buf[0]);
    }
    len = kfifo_put(uart_ctx[UART_ID_0].uart_fifo,_hal_uart_buf,xfer_size);
    if (len < xfer_size)
    {
        TRACE("%s kfifo no more buff! have %d need %d\r",__FUNCTION__,len,xfer_size);
        return;
    }
    hal_uart_rx_start(UART_ID_0);
}

static void _uart1_dma_rx_handler(uint32_t xfer_size, int dma_error, union HAL_UART_IRQ_T status)
{
    //_hal_uart1_read_offset = 0;
    //_hal_uart1_size = xfer_size;
    uint32_t len = 0;
    TRACE_IMM("%s line %d", __func__,__LINE__);
    if (xfer_size == 5) {
        panicNmiInputFilter(_hal_uart1_buf[0]);
        panicNmiInputFilter(_hal_uart1_buf[1]);
        panicNmiInputFilter(_hal_uart1_buf[2]);
        panicNmiInputFilter(_hal_uart1_buf[3]);
    } else if (xfer_size == 2) {
        panicNmiInputFilter(_hal_uart1_buf[0]);
    }
    if (_hal_uart1_buf[xfer_size-2] == '\r' && _hal_uart1_buf[xfer_size-1] == '\n')
        xfer_size--;
    else if (_hal_uart1_buf[xfer_size-2] == '\n' && _hal_uart1_buf[xfer_size-1] == '\r')
        xfer_size--;

    len = kfifo_put(uart_ctx[UART_ID_1].uart_fifo,_hal_uart1_buf,xfer_size);
    if (len < xfer_size)
    {
        TRACE("%s kfifo no more buff! have %d need %d\r",__FUNCTION__,len,xfer_size);
        return;
    }
    hal_uart_rx_start(UART_ID_1);
}

/*uart2*/
static void _uart2_dma_rx_handler(uint32_t xfer_size, int dma_error, union HAL_UART_IRQ_T status)
{
    //_hal_uart2_read_offset = 0;
    //_hal_uart2_size = xfer_size;
    int32_t len = 0;
    if (xfer_size == 5) {
        panicNmiInputFilter(_hal_uart2_buf[0]);
        panicNmiInputFilter(_hal_uart2_buf[1]);
        panicNmiInputFilter(_hal_uart2_buf[2]);
        panicNmiInputFilter(_hal_uart2_buf[3]);
    } else if (xfer_size == 2) {
        panicNmiInputFilter(_hal_uart2_buf[0]);
    }
    if (_hal_uart2_buf[xfer_size-2] == '\r' && _hal_uart2_buf[xfer_size-1] == '\n')
        xfer_size--;
    else if (_hal_uart2_buf[xfer_size-2] == '\n' && _hal_uart2_buf[xfer_size-1] == '\r')
        xfer_size--;
    len = kfifo_put(uart_ctx[UART_ID_2].uart_fifo,_hal_uart2_buf,xfer_size);
    if (len < xfer_size)
    {
        TRACE("%s kfifo no more buff! have %d need %d\r",__FUNCTION__,len,xfer_size);
        return;
    }
    hal_uart_rx_start(UART_ID_2);
}

void _hal_uart_init(uart_dev_t *uart, int reinit)
{
    enum HAL_UART_ID_T uart_id = uart->port;
    TRACE_IMM("%s line %d", __func__,__LINE__);
    hal_set_uart(uart_id);
    if (uart_id == UART_ID_0) {
        if (reinit){
            osSemaphoreDelete(cli_sema.sema_id);
        }
        high_uart_cfg.baud = uart->config.baud_rate;
        hal_uart_reopen(uart_id, &high_uart_cfg);
    } else {
        struct HAL_UART_CFG_T uart_cfg;
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
        hal_uart_open(uart_id, &uart_cfg);
    }
    //struct kfifo *fifo = NULL;
#ifdef RTOS
    uart_ctx[uart_id].uart_fifo = malloc(sizeof(struct kfifo));
#endif
    if (!uart_ctx[uart_id].uart_fifo)
    {
        TRACE("%s fail since no memory for kfifo!\n", __FUNCTION__);
        return;
    }
#ifdef RTOS
    uart_ctx[uart_id].uart_fifo_buff = malloc(UART_FIFO_MAX_BUFFER);
#endif
    if (!uart_ctx[uart_id].uart_fifo_buff)
    {
        TRACE("%s fail since no memory for kfifo buffer!\n", __FUNCTION__);
        return;
    }
    kfifo_init(uart_ctx[uart_id].uart_fifo, uart_ctx[uart_id].uart_fifo_buff, UART_FIFO_MAX_BUFFER);

    hal_uart_irq_set_dma_handler(uart_id, uart_ctx[uart_id].uart_dma_rx_handler, uart_ctx[uart_id].uart_dma_tx_handler, NULL);
    hal_uart_rx_start(uart_id);
    if (uart_id == UART_ID_0)
    {
        cli_sema.def.sem = &cli_sema.sem;
        cli_sema.def.name = "cli_uart";
        cli_sema.sema_id = osSemaphoreCreate(&cli_sema.def, 0);
    }else if (uart_id == UART_ID_1) {
        cli1_sema_rx.def.sem = &cli1_sema_rx.sem;
        cli1_sema_rx.def.name = "cli_uart1_rx";
        cli1_sema_rx.sema_id = osSemaphoreCreate(&cli1_sema_rx.def, 0);
        cli1_sema_tx.def.sem = &cli1_sema_tx.sem;
        cli1_sema_tx.def.name = "cli_uart1_tx";
        cli1_sema_tx.sema_id = osSemaphoreCreate(&cli1_sema_tx.def, 0);
    } else {
        cli2_sema_rx.def.sem = &cli2_sema_rx.sem;
        cli2_sema_rx.def.name = "cli_uart2_rx";
        cli2_sema_rx.sema_id = osSemaphoreCreate(&cli2_sema_rx.def, 0);
        cli2_sema_tx.def.sem = &cli2_sema_tx.sem;
        cli2_sema_tx.def.name = "cli_uart2_tx";
        cli2_sema_tx.sema_id = osSemaphoreCreate(&cli2_sema_tx.def, 0);
    }
}

int hal_tgdb_uart_open(enum HAL_UART_ID_T uart_id)
{
    if (uart_id == hal_trace_get_id()) {
        platform_trace_disable();
    }
    hal_set_uart(uart_id);
    high_uart_cfg.baud = hal_trace_get_baudrate();
    hal_uart_open(uart_id, &high_uart_cfg);
}

void hal_tgdb_uart_close(enum HAL_UART_ID_T uart_id)
{
    union HAL_UART_FLAG_T flag;

    hal_uart_stop_dma_recv(uart_id);
    do {
        flag = hal_uart_get_flag(uart_id);
        if (!flag.BUSY) {
            break;
        }
        osDelay(1);
    } while (1);
    if (uart_id == hal_trace_get_id()) {
        hal_uart_close(uart_id);
        platform_trace_enable();
        //_hal_uart_init(uart_id, 1);
    } else {
        hal_uart_close(uart_id);
        high_uart_cfg.baud = hal_trace_get_baudrate();
        //uart2_id = uart_id;
        hal_uart_open(uart_id, &high_uart_cfg);
        hal_uart_irq_set_dma_handler(uart_id, uart_ctx[uart_id].uart_dma_rx_handler, uart_ctx[uart_id].uart_dma_tx_handler, NULL);
        hal_uart_rx_start(uart_id);
    }
}

static int panic_uart_open = 0;
int32_t g_cli_direct_read;
int hal_panic_uart_open(void)
{
    enum HAL_UART_ID_T uart_id = hal_trace_get_id();
    TRACE_FLUSH();
    hal_uart_close(uart_id);
    low_uart_cfg.baud = hal_trace_get_baudrate();
    hal_uart_open(uart_id, &low_uart_cfg);
    panic_uart_open = 1;
    g_cli_direct_read = 1;
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
    _hal_uart_init(uart,0);
    return 0;
}

/**
 * Transmit data on a UART interface
 *
 * @param[in]  uart     the UART interface
 * @param[in]  data     pointer to the start of data
 * @param[in]  size     number of bytes to transmit
 * @param[in]  timeout  timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                      if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    int32_t ret;
    uint8_t uart_id = uart->port;
    if (uart_id == hal_trace_get_id()) {
        if (panic_uart_open) {
            for (int i = 0; i < size; i++)
                hal_uart_blocked_putc(uart_id, *((char *)data + i));
        } else {
            hal_trace_output_block(data, size);
        }
    } else if(uart_id == UART_ID_1){
        hal_uart_dma_send_sync_cache(uart_id, data, size, NULL, NULL);
        osSemaphoreWait(cli1_sema_tx.sema_id,timeout);
    }else {
        hal_uart_dma_send_sync_cache(uart_id, data, size, NULL, NULL);
        osSemaphoreWait(cli2_sema_tx.sema_id,timeout);
    }

    return 0;
}

char uart_input_read(uart_dev_t *uart)
{
    return hal_uart_blocked_getc(uart->port);
}

/**
 * Receive data on a UART interface
 *
 * @param[in]   uart         the UART interface
 * @param[out]  data         pointer to the buffer which will store incoming data
 * @param[in]   expect_size  number of bytes to receive
 * @param[in]   timeout      timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                           if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_uart_recv(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t timeout)
{
    FAIL_FUNCTION();
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
 *                           if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                         uint32_t *recv_size, uint32_t timeout)
{
    int32_t ret = 0;
    uint8_t uart_id = uart->port;
    int rsize = 0;
    int32_t begin_time =0;
    int32_t now_time = 0;
    int32_t len = 0;
    //ENTER_FUNCTION();
    if (NULL == data || 0 == expect_size || recv_size == 0)
    {
        FAIL_FUNCTION();
        ret = EIO;
        goto RETURN;
    }

    *recv_size = 0;
        begin_time = krhino_sys_tick_get();
        do
        {
            len = kfifo_len(uart_ctx[uart_id].uart_fifo);
            if (len>0)
            {
                *recv_size = (len>expect_size)?expect_size:len;
                kfifo_get(uart_ctx[uart_id].uart_fifo,data,*recv_size);
            }
            now_time = krhino_sys_tick_get();
            if((now_time-begin_time)>=timeout){
                TRACE("%s time out! line %d",__FUNCTION__,__LINE__);
                break;
            }
        } while (len==0);
    ret = 0;

RETURN:
    //LEAVE_FUNCTION();
    return ret;
}

void _hal_uart1_recv_simulate(char ch)
{
    _hal_uart1_buf[_hal_uart1_read_offset] = ch;
    osSemaphoreRelease(cli1_sema_rx.sema_id);
}

void _hal_uart2_recv_simulate(char ch)
{
    _hal_uart2_buf[_hal_uart2_read_offset] = ch;
    osSemaphoreRelease(cli2_sema_rx.sema_id);
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
    int32_t ret = 0;
    ENTER_FUNCTION();
    uint8_t uart_id = uart->port;
    if (uart_id == hal_trace_get_id()) {
        //hal_uart_close(uart->port);
        TRACEME("do nothings since we need uart!\n");
    } else {
        hal_uart_close(uart_id);
        if (uart_id == UART_ID_1){
            memset(&cli1_sema_rx,0,sizeof(_cli_sema_t));
            memset(&cli1_sema_tx,0,sizeof(_cli_sema_t));
        }else{
            memset(&cli2_sema_rx,0,sizeof(_cli_sema_t));
            memset(&cli2_sema_rx,0,sizeof(_cli_sema_t));
        }
    }
    free(uart_ctx[uart_id].uart_fifo);
    free(uart_ctx[uart_id].uart_fifo_buff);
    LEAVE_FUNCTION();
    return ret;
}

#define TEST_DATA   "hello"
void _hal_uart_test()
{
    TRACE("%s", __func__);
    int32_t ret = 0;
    int32_t i = 0;
    uint32_t recv_size = 0;
    uint8_t recv_data[100] = {0};
    uart_dev_t uart_ttl = {0};

    uart_ttl.port = 2;  /* uart1 be used as rs485 */
    uart_ttl.config.baud_rate = 19200;
    uart_ttl.config.data_width = DATA_WIDTH_8BIT;
    uart_ttl.config.flow_control = FLOW_CONTROL_DISABLED;
    uart_ttl.config.mode = MODE_TX_RX;
    uart_ttl.config.parity = NO_PARITY;
    uart_ttl.config.stop_bits = STOP_BITS_1;

    /*printf("\r\n ====start uart %d test baudrate %d ====\r\n", port, baud);*/

    ret = hal_uart_init(&uart_ttl);
    if (ret) {
        printf("=====uart test : uart 1 dev init fail =====\r\n");
        return -1;

    }

    for(i=0; i < 100; i++)
    {
        recv_size = 0;
        memset(recv_data, 0, sizeof(recv_data));
        ret = hal_uart_recv_II(&uart_ttl, recv_data, sizeof(recv_data), &recv_size, 2000);

        osDelay(100);
        ret = hal_uart_send(&uart_ttl, recv_data, recv_size, 2000);
        /*if (ret) {*/
        /*printf("=====uart test : uart %d fail to send test data=====\r\n", port);*/
        /*continue;*/
        /*}*/
        osDelay(100);

    }

    hal_uart_finalize(&uart_ttl);

}
