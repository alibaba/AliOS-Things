/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef MODULE_UDP_H
#define MODULE_UDP_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * register the add-on of UDP module
 *
 * @return  void
 */

void module_udp_register(void);

#define MAX_UDP_RECV_LEN 256

#ifdef __cplusplus
}
#endif

#endif /* MODULE_UDP_H */
