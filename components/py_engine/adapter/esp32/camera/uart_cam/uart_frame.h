#ifndef _UART_FRAME_H
#define _UART_FRAME_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define DISABLE_USE_UART_QUEUE

typedef enum {
    IDLE = 0x00,
    WAIT_FINISH,
    FINISH,
} frame_state_n;

/*
 * frame_state is volatile type
 */
extern volatile frame_state_n frame_state;

void uart_frame_init(int32_t rx, int32_t tx, uint32_t baud);
void uart_frame_send(uint8_t cmd, const uint8_t *frame, uint32_t len, bool wait_finish);
void uart_update_pin(int32_t rx, int32_t tx);
int32_t uart_frame_recv(uint8_t **data, uint32_t *len);
#endif
