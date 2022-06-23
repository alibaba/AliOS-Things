#include <stdio.h>
#include <aos/kernel.h>

#include "aos_hal_uart.h"
#include "string.h"
#include "uart_frame.h"

#define UART_NUM 1
#define UART_QUEUE_LENGTH 10
#define TX_BUF_SIZE 0
#define FRAME_MAX_SIZE (16 * 1024)
#define RX_BUF_SIZE 4 * 1024

#define PACK_FIRST_BYTE 0xAA
#define PACK_SECOND_BYTE 0x55

#define UART_TIMEOUT (0xFFFFFF)

volatile frame_state_n frame_state;
#ifndef DISABLE_USE_UART_QUEUE
//static QueueHandle_t uart_queue = NULL;
static aos_queue_t *uart_queue = NULL;
#endif
//static QueueHandle_t &uart_buffer_queue = NULL;

/* queue resource */
#define MESSAGE_MAX_LENGTH 10 /* maximum message length */

uart_dev_t g_uart_device = {
    .port = 2,
    .config = {
        .baud_rate = 1500000,
        .data_width = DATA_WIDTH_8BIT,
        .parity = NO_PARITY,
        .stop_bits = STOP_BITS_1,
        .flow_control = FLOW_CONTROL_DISABLED,
        .mode = MODE_TX_RX
    },
    .priv = NULL,
    .cb = NULL,
    .userdata = NULL

};

static uint32_t g_frame_state = 0;

static aos_task_t uart_frame_send_task_handle;

static void uart_frame_send_task(void *arg);
static void uart_base_uninit(void);

typedef struct _UartFrame_t {
    bool free_buffer;
    uint8_t* buffer;
    uint32_t len;
} UartFrame_t;

/* Static memory for static creation */
static char        queue_buffer[sizeof(UartFrame_t) * 2]; /* for the internal buffer of the queue */
static aos_queue_t uart_buffer_queue;

void __attribute__((weak)) frame_post_callback(uint8_t cmd) {

}

void __attribute__((weak)) frame_recv_callback(int cmd, const uint8_t*buf, int len) {

}

static void uart_base_init(uint32_t baud) {
    int ret = -1;

    g_uart_device.config.baud_rate = baud;

    ret = aos_hal_uart_init(&g_uart_device);

    if (0 != ret) {
        printf("%s: aos_hal_uart_init failed ret = %d;\n", __func__, ret);
    }

    return;
} 

void uart_frame_init(int32_t rx, int32_t tx, uint32_t baud) {

    aos_status_t status = -1;

    baud;
    rx;
    tx;

    if (g_frame_state) {
        printf("[%s]already initialized\n", __func__);
        return 0;
    }

    status = aos_queue_new(&uart_buffer_queue, (void *)queue_buffer, sizeof(queue_buffer), sizeof(queue_buffer));
    if (status != 0) {
        printf("[%s]create queue error\n", __func__);
        return;
    } else {
        printf("[%s]create queue success\n", __func__);
    }

    uart_base_init(baud);

    status = aos_task_create(&uart_frame_send_task_handle, "uart_frame_send_task", uart_frame_send_task, NULL, NULL, 2 * 1024, 50, AOS_TASK_AUTORUN);
    if (status != 0) {
        printf("[%s]create %s error\n", __func__, "uart_frame_send_task");
        uart_base_uninit();
        aos_queue_free(&uart_buffer_queue);
        return;
    }
    g_frame_state = 1;
}

static void uart_base_uninit(void) {
    int ret = -1;

    g_frame_state = 0;

    ret = aos_hal_uart_finalize(&g_uart_device);

    if (0 != ret) {
        printf("%s: aos_hal_uart_finalize failed ret = %d;\n", __func__, ret);
    }

    return;
}

int32_t uart_frame_uninit() {

    aos_task_delete(&uart_frame_send_task_handle);

    uart_base_uninit();

    aos_queue_free(&uart_buffer_queue);
    return 0;
}

typedef enum {
    kWaitStart = 0x00, 
    kRecvLenght,
    kLenghtCheck,
    kRecvCMD,
    kRecvData,
    kCrcCheck,
    kSuccess,
} frame_state_t;

static void uart_frame_send_task(void *arg) {
    aos_status_t  status;
    UartFrame_t frame;
    int rev_size = 0;
    int ret = 0;

    for (;;) {

        status = aos_queue_recv(&uart_buffer_queue, AOS_WAIT_FOREVER, (void *)&frame, &rev_size);
        if (status == 0) {
            /* show message data */
            printf("[%s]%d recv message\r\n", __func__, rev_size);
        } else {
            printf("[%s]recv buf queue error\n", __func__);
        }

        ret = aos_hal_uart_send(&g_uart_device, frame.buffer, frame.len, AOS_WAIT_FOREVER);
        if (ret != 0) {
            printf("aos_hal_uart_send failed, ret:%d\n", ret);
        }

        frame_post_callback(frame.buffer[7]);
        if (frame.free_buffer) {
            free(frame.buffer);
        }
        if (!g_frame_state)
            break;
    }

    aos_task_exit(0);
}

int32_t uart_frame_recv(uint8_t **data, uint32_t *len)
{
    uint8_t *buf = calloc(1, FRAME_MAX_SIZE);
    uint8_t *recv_buf = calloc(1, RX_BUF_SIZE);
    uint8_t *ptr = recv_buf;
    uint32_t packet_pos = 0;
    uint8_t packet_ahead[] = {PACK_FIRST_BYTE, PACK_SECOND_BYTE};
    uint32_t frame_length = 0;
    uint8_t xor = 0x00;
    uint8_t cmd = 0x00;
    uint16_t size;
    buf[0] = PACK_FIRST_BYTE;
    buf[1] = PACK_SECOND_BYTE;
    frame_state_t frame_state = kWaitStart;
    int32_t ret = 0;

    if (!buf)
        return -1;

    if (!recv_buf) {
        free(buf);
        return -1;
    }

    for(;;) {
        memset(recv_buf, 0, RX_BUF_SIZE);
        size = 0;
        
        ret = aos_hal_uart_recv_II(&g_uart_device, recv_buf, RX_BUF_SIZE, &size, 40);
        if (ret != 0) {
            free(recv_buf);
            recv_buf = NULL;
            return -1;
        }

        ptr = recv_buf;
        while (size) {
            switch (frame_state) {
                case kWaitStart:
                    buf[packet_pos] = *ptr;
                    ptr += 1;
                    size -= 1;

                    if (buf[packet_pos] == packet_ahead[packet_pos]) {
                        packet_pos += 1;
                        if (packet_pos == 2) {
                            frame_length = 0;
                            frame_state = kRecvLenght;

                        }
                    } else {
                        packet_pos = 0;
                    }
                    break;

                case kRecvLenght:
                    buf[packet_pos] = *ptr;
                    ptr += 1;
                    size -= 1;
                    packet_pos += 1;
                    if (packet_pos == (2 + 4)) {
                        xor = 0x000;
                        frame_state = kLenghtCheck;
                    }
                    break;

                case kLenghtCheck:
                    buf[packet_pos] = *ptr;
                    ptr += 1;
                    size -= 1;
                    xor = 0x00 ^ buf[2] ^ buf[3] ^ buf[4] ^ buf[5];
                    if (xor == buf[packet_pos]) {
                        packet_pos += 1;
                        frame_length = (buf[2] << 24) | (buf[3] << 16) | (buf[4] << 8) | (buf[5] << 0);
                        if (frame_length > FRAME_MAX_SIZE - 9) {
                            packet_pos = 0;
                            frame_state = kWaitStart;
                        } else {
                            frame_state = kRecvCMD;
                        }
                    } else {
                        packet_pos = 0;
                        frame_state = kWaitStart;
                    }
                    break;

                case kRecvCMD:
                    buf[packet_pos] = *ptr;
                    cmd = buf[packet_pos];
                    ptr += 1;
                    size -= 1;
                    packet_pos += 1;
                    frame_state = kRecvData;
                    break;
                
                case kRecvData:
                    while(size > 0) {
                        buf[packet_pos] = *ptr;
                        ptr += 1;
                        size -= 1;
                        packet_pos += 1;
                        if (packet_pos == (frame_length + 6)) {
                            frame_state = kCrcCheck;
                            break;
                        }
                    }
                    break;

                case kCrcCheck:
                    buf[packet_pos] = *ptr;
                    ptr += 1;
                    size -= 1;
                    xor = 0x00;

                    for (uint16_t i = 0; i < packet_pos; i++) {
                        xor = buf[i] ^ xor;
                    }

                    if (xor != buf[packet_pos]) {
                        packet_pos = 0;
                        frame_state = kWaitStart;
                        break;
                    }

                case kSuccess:
                    packet_pos = 0;
                    frame_state = kWaitStart;
                    *data = buf;
                    *len = frame_length - 2;
                    free(recv_buf);
                    recv_buf = NULL;
                    return 0;

                default:
                    break;
            }
        }
    }

    free(recv_buf);
    recv_buf = NULL;
    return -1;
}


void uart_frame_send(uint8_t cmd, const uint8_t* frame, uint32_t len, bool wait_finish) {
    aos_status_t status = -1;
    uint32_t out_len = 9 + len;
    uint8_t* out_buf = (uint8_t *)malloc(sizeof(uint8_t) * out_len);

    out_buf[0] = PACK_FIRST_BYTE;
    out_buf[1] = PACK_SECOND_BYTE;
    out_buf[2] = (out_len - 7) >> 24;
    out_buf[3] = (out_len - 7) >> 16;
    out_buf[4] = (out_len - 7) >> 8;
    out_buf[5] = (out_len - 7);
    out_buf[6] = 0x00 ^ out_buf[2] ^ out_buf[3] ^ out_buf[4] ^ out_buf[5];
    out_buf[7] = cmd;
    memcpy(&out_buf[8], frame, len);
    int xor_resilt = 0x00;
    for (uint32_t i = 0; i < out_len - 1; i++) {
        xor_resilt = out_buf[i] ^ xor_resilt;
    }
    out_buf[out_len - 1] = xor_resilt;

    if (wait_finish) {
        //while (uxQueueMessagesWaiting(&uart_buffer_queue)) {
        while (aos_queue_get_count(&uart_buffer_queue) > 0) {
            //vTaskDelay(pdMS_TO_TICKS(10));
            aos_msleep(10);
        }
    }

    UartFrame_t uart_frame;
    uart_frame.buffer = out_buf;
    uart_frame.len = out_len;
    uart_frame.free_buffer = true;

    //xQueueSend(&uart_buffer_queue, &uart_frame, portMAX_DELAY);
    
    status = aos_queue_send(&uart_buffer_queue, (void *)&uart_frame, sizeof(uart_frame));
    if (status != 0) {
        printf("[%s]send buf queue error, status:%d\n", __func__, status);
    } else
        printf("[%s]send buf queue success\n", __func__);
}
