/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef YLOOP_H
#define YLOOP_H

/* set loop's event fd */
void aos_loop_set_eventfd(int fd);

/* get loop's event fd */
int aos_loop_get_eventfd(void *loop);

/* init per-loop event service */
int aos_event_service_init(void);

/* deinit per-loop event service */
void aos_event_service_deinit(int fd);

#endif /* YLOOP_H */

