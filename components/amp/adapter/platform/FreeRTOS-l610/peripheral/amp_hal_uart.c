/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "amp_defines.h"
#include "osi_api.h"
#include "drv_uart.h"
#include "amp_hal_uart.h"
#include "osi_log.h"
#include <stdbool.h>

#define UART_INSTANCE_NUM (4)
#define UART_WAIT_TX_DONE_TIMEOUT (500)
#define UART_WATI_RX_DONE_TIMEOUT (10)
#define DRV_UART_EVENT_RX_TIMEOUT DRV_UART_EVENT_TX_COMPLETE << 1
#define DRV_UART_EVENT_RX_OVER    DRV_UART_EVENT_RX_TIMEOUT << 1
#define HAL_WAIT_FOREVER 0xFFFFFFFFU
#define UART_RX_BUF_SIZE (512)
#define UART_TX_BUF_SIZE (256)

/**
* @brief Callback for recv uart data
* 
 */

enum hal_uart_data_bits_s
{
    HAL_UART_DATA_BITS_7 = 7,
    HAL_UART_DATA_BITS_8 = 8
};

enum hal_uart_stop_bits_s
{
    HAL_UART_STOP_BITS_1 = 1,
    HAL_UART_STOP_BITS_2 = 2
};

typedef int hal_uart_port_t;
typedef struct hal_uart_config_s hal_uart_config_t;
typedef enum hal_uart_data_bits_s hal_uart_data_bits_t;
typedef enum hal_uart_stop_bits_s fibo_hal_uart_stop_bits_t;
typedef void (*uart_input_callback_t)(hal_uart_port_t uart_port, uint8_t *data, uint16_t len, void *arg);

struct hal_uart_config_s
{
    uint32_t baud;                       ///< baudrate, 0 for auto baud
    hal_uart_data_bits_t data_bits;      ///< data bits
    fibo_hal_uart_stop_bits_t stop_bits; ///< stop bits
    bool cts_enable;                     ///< enable cts or not
    bool rts_enable;                     ///< enable rts or not
    size_t rx_buf_size;                  ///< rx buffer size
    size_t tx_buf_size;                  ///< tx buffer size
    uint32_t recv_timeout;               //ms
};

/**
* @brief 
* 
* @param uart_port 
* @param uart_config 
* @param recv_cb 
* @param arg 
* @return INT32 
 */
int32_t aliyun_hal_uart_init(hal_uart_port_t uart_port, hal_uart_config_t *uart_config, uart_input_callback_t recv_cb, void *arg);

/**
* @brief 
* 
* @param uart_port 
* @param buff 
* @param len 
* @return INT32 
 */
int32_t aliyun_hal_uart_put(hal_uart_port_t uart_port, uint8_t *buff, uint32_t len);

/**
* @brief 
* 
* @param uart_port 
* @return INT32 
 */
int32_t aliyun_hal_uart_deinit(hal_uart_port_t uart_port);

void aliyun_uart_recv(hal_uart_port_t uart_port, uint8_t *data, uint16_t len, void *arg);

#define FIBO_LOG(format, ...)                                                  \
    do                                                                         \
    {                                                                          \
        OSI_PRINTFI("[%s:%d]-" format, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define FIBO_CHECK(condition, err_code, format, ...) \
    do                                               \
    {                                                \
        if (condition)                               \
        {                                            \
            FIBO_LOG(format, ##__VA_ARGS__);         \
            ret = err_code;                          \
            goto error;                              \
        }                                            \
    } while (0);

#define UART_LOG(format, ...) FIBO_LOG("[uartapi]" format, ##__VA_ARGS__)

enum fibo_result_s
{
    FIBO_R_FAILED = -1,
    FIBO_R_SUCCESS = 0,
};

typedef struct uart_instance_s uart_instance_t;
struct uart_instance_s
{
    int uart_port;
    int name;
    drvUart_t *drv;
    uint32_t pending_event;
    drvUartCfg_t drvcfg;
    osiWork_t *work;
    bool init;
    //    bool receivfinished;
    uint8_t *recv_buf;
    uint32_t recv_len;
    uint32_t recv_buf_len;
    uart_input_callback_t recv_cb;
    osiTimer_t *recv_timer;
    uint32_t recv_timeout;
    void *arg;
};

static int uart_name[UART_INSTANCE_NUM] = {DRV_NAME_UART1, DRV_NAME_UART2, DRV_NAME_UART3, DRV_NAME_UART4};
static uart_instance_t aliyun_uart_instace[UART_INSTANCE_NUM] = {0};

typedef struct uarttmp
{
    int writelen;
    char recvbuf[4096];
    int readlen;
} uarttmp;

static uarttmp uartrecover;

static void drv_evt_callback(void *param, uint32_t evt)
{
    uart_instance_t *ins = (uart_instance_t *)param;
    ins->pending_event |= evt;
    UART_LOG("recv drv evt:0x%x pending evt:0x%x", evt, ins->pending_event);
    osiWorkEnqueue(ins->work, osiSysWorkQueueHighPriority());
}

static void recv_timer_callback(void *arg)
{
    uart_instance_t *ins = (uart_instance_t *)arg;
    //UART_LOG("recv timer reached");
    ins->pending_event |= DRV_UART_EVENT_RX_TIMEOUT;
    osiWorkEnqueue(ins->work, osiSysWorkQueueHighPriority());
}

void aliyun_uart_recv(hal_uart_port_t uart_port, uint8_t *data, uint16_t len, void *arg)
{
    //    uart_instance_t *ins = &aliyun_uart_instace[uart_port];
    //uint32_t critical = osiEnterCritical();
    // ins->pending_event |= DRV_UART_EVENT_RX_OVER;
    // osiExitCritical(critical);
    memcpy(&uartrecover.recvbuf[uartrecover.writelen], data, len);
    uartrecover.writelen += len;
    UART_LOG("uartapi recv uart_port=%d len=%d  uartrecover.writelen = %d\n", uart_port, len, uartrecover.writelen);
}

static void aliyun_recv_data(void *param)
{
    uart_instance_t *ins = (uart_instance_t *)param;
    uint32_t critical = osiEnterCritical();
    unsigned event = ins->pending_event;
    ins->pending_event = 0;
    osiExitCritical(critical);
    //UART_LOG("fibo uart recv work start!\n");

    if (event & DRV_UART_EVENT_RX_ARRIVED)
    {
        //UART_LOG("drvUartReadAvail start!\n");
        int avail_len = drvUartReadAvail(ins->drv);
        if (avail_len > 0)
        {
            //UART_LOG("fibo uart recv work avail_len = %d!\n", avail_len);
            int trans = drvUartReceive(ins->drv, ins->recv_buf + ins->recv_len, ins->recv_buf_len - ins->recv_len);
            if (trans > 0)
            {
                ins->recv_len += trans;
                ins->recv_buf[ins->recv_len] = 0;
                if (ins->recv_len >= ins->recv_buf_len)
                {
                    ins->recv_cb(ins->uart_port, ins->recv_buf, ins->recv_len, ins->arg);
                    UART_LOG("fibo uart total recv %lu bytes, call user cb", ins->recv_len);
                    ins->recv_len = 0;
                    osiTimerStop(ins->recv_timer);
                }
                else
                {
                    UART_LOG("start timer for recv more data cur_len=%lu timeout=%lu", ins->recv_len, ins->recv_timeout);
                    osiTimerStop(ins->recv_timer);
                    osiTimerStart(ins->recv_timer, ins->recv_timeout);
                }
                UART_LOG("fibo uart recv %d bytes", trans);
            }
            else
            {
                UART_LOG("fibo uart output error %d", trans);
            }
        }
        else
        {
            if (ins->recv_len > 0)
            {
                UART_LOG("fibo uart should call user", ins->recv_len);
                osiTimerStop(ins->recv_timer);
                osiTimerStart(ins->recv_timer, ins->recv_timeout);
            }
            UART_LOG("avail len is 0");
        }
    }
    else if (event & DRV_UART_EVENT_RX_TIMEOUT)
    {
        UART_LOG("fibo uart total recv %d bytes, timerout call user cb", ins->recv_len);
        if (ins->recv_len > 0)
        {
            ins->recv_cb(ins->uart_port, ins->recv_buf, ins->recv_len, ins->arg);
        }
        ins->recv_len = 0;
    }
}

int32_t aliyun_hal_uart_init(hal_uart_port_t uart_port, hal_uart_config_t *uart_config, uart_input_callback_t recv_cb, void *arg)
{
    int ret = FIBO_R_SUCCESS;
    uart_instance_t *ins = NULL;
    drvUartCfg_t drvcfg = {0};

    FIBO_CHECK(uart_port >= UART_INSTANCE_NUM, FIBO_R_FAILED, "fibo uart port out of range:%d", uart_port);
    FIBO_CHECK(uart_config == NULL, FIBO_R_FAILED, "uart_config is null");
    FIBO_CHECK(recv_cb == NULL, FIBO_R_FAILED, "recv_cb is null");

    aliyun_hal_uart_deinit(uart_port);

    ins = &aliyun_uart_instace[uart_port];

    if (!ins->init)
    {
        memset(ins, 0, sizeof(*ins));
        ins->uart_port = uart_port;
        ins->name = uart_name[uart_port];
        drvcfg.baud = uart_config->baud;
        drvcfg.auto_baud_lc = 0;
        drvcfg.data_bits = uart_config->data_bits;
        drvcfg.stop_bits = uart_config->stop_bits;
        drvcfg.parity = DRV_UART_NO_PARITY;
        drvcfg.rx_buf_size = uart_config->rx_buf_size;
        drvcfg.tx_buf_size = uart_config->tx_buf_size;
        drvcfg.event_mask = DRV_UART_EVENT_RX_ARRIVED | DRV_UART_EVENT_RX_OVERFLOW | DRV_UART_EVENT_TX_COMPLETE;
        drvcfg.event_cb = drv_evt_callback;
        drvcfg.event_cb_ctx = ins;

        ins->recv_buf = (uint8_t *)malloc(uart_config->rx_buf_size + 1);
        FIBO_CHECK(ins->recv_buf == NULL, FIBO_R_FAILED, "memory not enough");
        ins->recv_buf_len = uart_config->rx_buf_size;
        ins->recv_len = 0;
        ins->drv = drvUartCreate(ins->name, &drvcfg);
        FIBO_CHECK(ins->drv == NULL, FIBO_R_FAILED, "create uart device failed");
        FIBO_CHECK(!drvUartOpen(ins->drv), FIBO_R_FAILED, "open uart failed");
        ins->work = osiWorkCreate(aliyun_recv_data, NULL, ins);
        ins->recv_timer = osiTimerCreate(NULL, recv_timer_callback, ins);
        ins->recv_timeout = uart_config->recv_timeout == 0 ? UART_WATI_RX_DONE_TIMEOUT : uart_config->recv_timeout;
        ins->init = true;
        ins->pending_event = 0;
        ins->recv_cb = recv_cb;
        ins->arg = arg;
    }

    return ret;

error:
    if (NULL != ins && NULL != ins->drv)
    {
        drvUartDestroy(ins->drv);
    }
    return ret;
}

int32_t aliyun_hal_uart_deinit(hal_uart_port_t uart_port)
{
    int ret = FIBO_R_SUCCESS;
    uart_instance_t *ins = NULL;

    FIBO_CHECK(uart_port >= UART_INSTANCE_NUM, FIBO_R_FAILED, "fibo uart port out of range:%d", uart_port);

    ins = &aliyun_uart_instace[uart_port];
    FIBO_CHECK(!ins->init, FIBO_R_FAILED, "fibo uart is not open");

    drvUartWaitTxFinish(ins->drv, UART_WAIT_TX_DONE_TIMEOUT);
    drvUartDestroy(ins->drv);
    osiWorkDelete(ins->work);
    osiTimerDelete(ins->recv_timer);
    free(ins->recv_buf);
    ins->drv = NULL;
    ins->recv_buf = NULL;
    ins->recv_buf_len = 0;
    ins->init = false;

error:
    return ret;
}

int32_t aliyun_hal_uart_put(int uart_port, uint8_t *buff, uint32_t len)
{
    int ret = FIBO_R_SUCCESS;
    uart_instance_t *ins = NULL;

    FIBO_CHECK(uart_port >= UART_INSTANCE_NUM, FIBO_R_FAILED, "fibo uart port out of range:%d", uart_port);
    ins = &aliyun_uart_instace[uart_port];
    FIBO_CHECK(!ins->init, FIBO_R_FAILED, "fibo uart is not open");

    OSI_LOGI(0, "fibo uart %4c write: len=%d", ins->name, len);

#ifdef CONFIG_TWOLINE_WAKEUP_ENABLE
    srv2LineWakeUpCheckWakeUpOutState();
#endif
    int trans = 0;
    if (drvUartWriteAvail(ins->drv) > 0)
    {
        trans = drvUartSendAll(ins->drv, buff, len, UART_WAIT_TX_DONE_TIMEOUT);
        if (trans < 0)
        {
            ret = FIBO_R_FAILED;
            UART_LOG("fibo uart output error %d", trans);
        }
        else if (trans < len)
        {
            ret = FIBO_R_FAILED;
            UART_LOG("fibo uart output overflow, drop %d bytes", len - trans);
        }
        else
        {
            UART_LOG("fibo uart send finish");
        }
    }

error:
    return (ret == FIBO_R_SUCCESS) ? trans : ret;
}
int32_t amp_hal_uart_init(uart_dev_t *uart)
{
    UART_LOG("aliyun enter hal_uart_init uart port:%d\n", uart->port);
    hal_uart_config_t drvcfg = {0};
    if (uart->port == 0)
    {
        UART_LOG("invalide uart prot %d\n", uart->port);
        return -1;
    }

    switch (uart->config.data_width)
    {
    case DATA_WIDTH_7BIT:
        uart->config.data_width = DRV_UART_DATA_BITS_7;
        break;
    case DATA_WIDTH_8BIT:
        uart->config.data_width = DRV_UART_DATA_BITS_8;
        break;
    default:
        break;
    }

    switch (uart->config.stop_bits)
    {
    case STOP_BITS_1:
        uart->config.stop_bits = DRV_UART_STOP_BITS_1;
        break;
    case STOP_BITS_2:
        uart->config.stop_bits = DRV_UART_STOP_BITS_2;
        break;
    default:
        break;
    }

    drvcfg.baud = uart->config.baud_rate;
    drvcfg.data_bits = uart->config.data_width;
    drvcfg.stop_bits = uart->config.stop_bits;
    drvcfg.rx_buf_size = UART_RX_BUF_SIZE;
    drvcfg.tx_buf_size = UART_TX_BUF_SIZE;
    drvcfg.recv_timeout = UART_WATI_RX_DONE_TIMEOUT;
    aliyun_hal_uart_init(uart->port, &drvcfg, aliyun_uart_recv, NULL);
    return 0;
}

int32_t amp_hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    int ret = -1;
    if (uart->port == 0)
    {
        UART_LOG("invalide uart prot %d\n", uart->port);
        return -1;
    }
    ret = aliyun_hal_uart_put(uart->port, (uint8_t *)data, size);
    return ret;
}

int32_t amp_hal_uart_recv(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t timeout)
{
    return 0;
}

int32_t amp_hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                             uint32_t *recv_size, uint32_t timeout)
{
    uint8_t *pdata = (uint8_t *)data;
    uint32_t rx_count = 0;
    uint32_t sleep_ms = 1;
    int32_t ret = -1;

    if (uart->port == AMP_REPL_STDIO) {
        sleep_ms = 10;
    }

    while(rx_count < expect_size)
    {
        while (uartrecover.readlen != uartrecover.writelen)
        {
            pdata[rx_count++] = uartrecover.recvbuf[uartrecover.readlen++];
        
            if (uartrecover.readlen == UART_RX_BUF_SIZE) {
                uartrecover.readlen = 0;
            }

            if (rx_count == expect_size) {
                break;
            }
        }

        uint32_t critical = osiEnterCritical();
        if (uartrecover.writelen == uartrecover.readlen)
        {
            uartrecover.writelen = 0;
            uartrecover.readlen = 0;
            memset(uartrecover.recvbuf, 0x00, 4096);
        }
        osiExitCritical(critical);

        if (rx_count == expect_size) {
            break;
        }

        if (timeout >= sleep_ms) {
            HAL_SleepMs(sleep_ms);
            timeout -= sleep_ms;
        } else if (timeout > 0) {
            HAL_SleepMs(timeout);
            timeout = 0;
        }
    }

    if (recv_size != NULL) {
        *recv_size = 1;
    }

    if (rx_count != 0) {
        ret = 0;
    } else {
        ret = -1;
    }
    return ret;
}

int32_t amp_hal_uart_finalize(uart_dev_t *uart)
{
    aliyun_hal_uart_deinit(uart->port);
    return 0;
}

int32_t amp_hal_uart_callback(uart_dev_t *uart, void (*cb)(int, void *, uint16_t, void *), void *args)
{
    uart_instance_t *ins = &aliyun_uart_instace[uart->port];
    ins->recv_cb = (uart_input_callback_t)cb;
    return 0;
}
