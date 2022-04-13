/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __SPP_TASK_H__
#define __SPP_TASK_H__

#include "cmsis_os.h"
#include "cqueue.h"
#include "rfcomm_api.h"
#include "sdp_api.h"

#ifdef __cplusplus
extern "C" {
#endif

osThreadId create_spp_read_thread(void);

void close_spp_read_thread(void);

int spp_mailbox_put(struct spp_device *dev, uint8_t spp_dev_num, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /* __SPP_TASK_H__ */
