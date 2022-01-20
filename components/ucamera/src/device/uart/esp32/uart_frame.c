#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "string.h"
#include "esp_heap_caps.h"
#include "uart_frame.h"

#define UART_NUM UART_NUM_1
#define UART_QUEUE_LENGTH 10
#define TX_BUF_SIZE 0
#define FRAME_MAX_SIZE (16 * 1024)
#define RX_BUF_SIZE 2 * 1024

#define PACK_FIRST_BYTE 0xAA
#define PACK_SECOND_BYTE 0x55

volatile frame_state_n frame_state;
#ifndef DISABLE_USE_UART_QUEUE
static QueueHandle_t uart_queue = NULL;
#endif
static QueueHandle_t uart_buffer_queue = NULL;

#ifndef DISABLE_USE_UART_QUEUE
static void uart_frame_task(void *arg);
#endif
static void uart_frame_send_task(void *arg);

typedef struct _UartFrame_t {
    bool free_buffer;
    uint8_t* buffer;
    uint32_t len;
} UartFrame_t;

uint32_t baud_rate = 1500000;
uint16_t uart_tx_pin = 26;
uint16_t uart_rx_pin = 36;

void __attribute__((weak)) frame_post_callback(uint8_t cmd) {

}

void __attribute__((weak)) frame_recv_callback(int cmd, const uint8_t*buf, int len) {

}

static void uart_base_init() {
    uart_driver_delete(UART_NUM);
    const uart_config_t uart_config = {
        .baud_rate = baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM, uart_tx_pin, uart_rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
#ifdef DISABLE_USE_UART_QUEUE
    uart_driver_install(UART_NUM, RX_BUF_SIZE, TX_BUF_SIZE, UART_QUEUE_LENGTH, NULL, 0);
#else
    uart_driver_install(UART_NUM, RX_BUF_SIZE, TX_BUF_SIZE, UART_QUEUE_LENGTH, &uart_queue, ESP_INTR_FLAG_LOWMED);
#endif
    uart_set_rx_timeout(UART_NUM, 50);
} 

void uart_frame_init(int32_t rx, int32_t tx, uint32_t baud) {
    baud_rate = baud;
    uart_rx_pin = rx;
    uart_tx_pin = tx;
    uart_buffer_queue = xQueueCreate(1, sizeof(UartFrame_t));
    uart_base_init();
#ifndef DISABLE_USE_UART_QUEUE
    xTaskCreatePinnedToCore(uart_frame_task, "uart_queue_task", 4 * 1024, NULL, 3, NULL, 0);
#endif
    xTaskCreatePinnedToCore(uart_frame_send_task, "uart_frame_send_task", 2 * 1024, NULL, 1, NULL, 0);
}

void uart_update_pin(int32_t rx, int32_t tx) {
    uart_set_pin(UART_NUM, tx, rx, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
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
    UartFrame_t frame;
    
    for (;;) {
        xQueueReceive(uart_buffer_queue, &frame, portMAX_DELAY);
        uart_wait_tx_done(UART_NUM, portMAX_DELAY);
        uart_write_bytes(UART_NUM, (const char *)frame.buffer, frame.len);
        uart_wait_tx_done(UART_NUM, portMAX_DELAY);
        frame_post_callback(frame.buffer[7]);
        if (frame.free_buffer) {
            free(frame.buffer);
        }
    }
    vTaskDelete(NULL);
}

#ifdef DISABLE_USE_UART_QUEUE
int32_t uart_frame_recv(uint8_t **data, uint32_t *len)
{
    uint8_t *buf = calloc(1, FRAME_MAX_SIZE); //heap_caps_calloc(FRAME_MAX_SIZE, sizeof(uint8_t),  MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL);
    uint8_t recv_buf[1024];
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

    for(;;) {
        memset(recv_buf, 0, 1024);
        size = 1024;
        ret = uart_read_bytes(UART_NUM, recv_buf, size, portMAX_DELAY);
        if (ret < 0) {
            printf("uart_read_bytes fail\n");
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
                    return 0;

                default:
                    break;
            }
        }
    }
    return -1;
}
#else
static void uart_frame_task(void *arg) {
    uart_event_t xEvent;
    // uint32_t buf_pos = 0;

    uint8_t *buf = calloc(1, FRAME_MAX_SIZE); //heap_caps_calloc(FRAME_MAX_SIZE, sizeof(uint8_t),  MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL);
    uint8_t *recv_buf = (uint8_t *)malloc(1024 * sizeof(uint8_t));
    uint8_t* ptr = recv_buf;
    uint32_t packet_pos = 0;
    uint8_t packet_ahead[] = {PACK_FIRST_BYTE, PACK_SECOND_BYTE};
    uint32_t frame_length = 0;
    uint8_t xor = 0x00;
    uint8_t cmd = 0x00;
    uint16_t size;
    buf[0] = PACK_FIRST_BYTE;
    buf[1] = PACK_SECOND_BYTE;
    frame_state_t frame_state = kWaitStart;

    for(;;) {
        if (xQueueReceive(uart_queue, (void*)&xEvent, portMAX_DELAY) == pdTRUE) {
            switch(xEvent.type) {
                case UART_DATA: {
                    uart_read_bytes(UART_NUM, recv_buf, xEvent.size, portMAX_DELAY);
                    ptr = recv_buf;
                    size = xEvent.size;
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
                                frame_recv_callback(buf[7], &buf[8], frame_length - 2);
                                break;

                            default:
                                break;
                        }
                    }
                    break;
                }
                
                case UART_FIFO_OVF: {
                    xQueueReset(uart_queue);
                    break;
                }

                case UART_BUFFER_FULL: {
                    uart_flush_input(UART_NUM);
                    xQueueReset(uart_queue);
                    break;
                }

                case UART_BREAK: {
                    break;
                }

                case UART_PARITY_ERR: {
                    break;
                }

                case UART_FRAME_ERR: {
                    break;
                }

                default: {
                    break;
                }
            }
        }
    }
    vTaskDelete(NULL);
}
#endif

void uart_frame_send(uint8_t cmd, const uint8_t* frame, uint32_t len, bool wait_finish) {
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
        while (uxQueueMessagesWaiting(uart_buffer_queue)) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }

    UartFrame_t uart_frame;
    uart_frame.buffer = out_buf;
    uart_frame.len = out_len;
    uart_frame.free_buffer = true;

    xQueueSend(uart_buffer_queue, &uart_frame, portMAX_DELAY);
}
