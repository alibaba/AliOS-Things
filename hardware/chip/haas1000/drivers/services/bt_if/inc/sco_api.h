/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef _SCO_API_H
#define _SCO_API_H
#include "btlib_type.h"

/* notify upper layer */
enum sco_event_enum {
    SCO_OPENED,
    SCO_CLOSED
};

#ifdef __cplusplus
extern "C" {
#endif
    int8 sco_register_link(struct bdaddr_t *bdaddr,
                           void (*sco_notify_callback) (enum sco_event_enum event, void *pdata));
    int8 sco_open_link(struct bdaddr_t *bdaddr,
                       void (*sco_notify_callback) (enum sco_event_enum event, void *pdata));
    int8 sco_close_link(struct bdaddr_t *bdaddr1, uint8 reason);
    int8 sco_unregister_link(struct bdaddr_t *bdaddr);
    int8 sco_init(void);
    int8 sco_exit(void);
    void sco_conn_opened_ind(struct bdaddr_t *bdaddr_remote);
    void sco_conn_closed_ind(struct bdaddr_t *bdaddr_remote);
#ifdef __cplusplus
}
#endif

#endif                          /**/
