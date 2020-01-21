/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef BLE_CONFIG_H
#define BLE_CONFIG_H

#define CONFIG_BT_HCI_TX_STACK_SIZE 512

#define CONFIG_BT_HCI_CMD_COUNT 16

#ifdef CONFIG_BLE_LINK_PARAMETERS
#define  CONFIG_SUP_TO_LIMIT 400
#define  CONFIG_CONN_SUP_TIMEOUT 400
#define  CONFIG_CONN_INTERVAL_MIN 24
#define  CONFIG_CONN_INTERVAL_MAX 40
#endif

#if defined(__cplusplus)
}
#endif

#endif /* BLE_DEF_CONFIG_H */
