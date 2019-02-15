/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef BLE_CTLR_CONFIG_H
#define BLE_CTLR_CONFIG_H

#ifndef CONFIG_BT_CTLR_WORKER_PRIO
#define CONFIG_BT_CTLR_WORKER_PRIO 0
#endif

#ifndef CONFIG_BT_CTLR_JOB_PRIO
#define CONFIG_BT_CTLR_JOB_PRIO 0
#endif

#ifndef CONFIG_BT_CTLR_RX_BUFFERS
#define CONFIG_BT_CTLR_RX_BUFFERS 1
#endif

#ifndef CONFIG_BT_CTLR_XTAL_THRESHOLD
#define CONFIG_BT_CTLR_XTAL_THRESHOLD 5168
#endif

#define CONFIG_BT_RECV_IS_RX_THREAD

#if defined(__cplusplus)
}
#endif

#endif /* BLE_CTLR_CONFIG_H */
