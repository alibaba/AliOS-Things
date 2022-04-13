/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef YMODEM_ADAPT_H
#define YMODEM_ADAPT_H

#define ROUND_DOWN(a, b) (((a) / (b)) * (b))

/* system APIs */
void sys_reboot(void);

/* uart APIs */
void uart_init(void);
void uart_send(unsigned char *buf, int len);
unsigned char uart_recv_byte(unsigned char *c);
void uart_send_byte(unsigned char buf);

/* watchdog APIs */
void wdg_init(unsigned int ms);
void wdg_feed(void);
void wdg_finish(void);


#endif /* YMODEM_ADAPT_H */
