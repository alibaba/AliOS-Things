/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef YLOOP_H
#define YLOOP_H

typedef struct {
    int              sock;
    void            *private_data;
    aos_poll_call_t  cb;
} yloop_sock_t;

typedef struct {
    dlist_t          timeouts;
    struct pollfd   *pollfds;
    yloop_sock_t    *readers;
    int              eventfd;
    /* socket Value may exceed 255 */
    uint16_t         max_sock;
    uint8_t          reader_count;
    bool             pending_terminate;
    bool             terminate;
} yloop_ctx_t;
/* set loop's event fd */
void aos_loop_set_eventfd(int fd);

/* get loop's event fd */
int aos_loop_get_eventfd(void *loop);

/* init per-loop event service */
int aos_event_service_init(void);

/* deinit per-loop event service */
void aos_event_service_deinit(int fd);

#endif /* YLOOP_H */

