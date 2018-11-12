/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <hal/soc/uart.h>

mb_status_t mb_serial_init(mb_handler_t * handler, uint8_t port, uint32_t baud_rate, uint8_t data_width, mb_parity_t parity);
mb_status_t mb_serial_frame_send(mb_handler_t * handler);
mb_status_t mb_serial_frame_recv(mb_handler_t * handler);
mb_status_t mb_serial_finalize(mb_handler_t * handler);
