/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef BLE_CONFIG_H
#define BLE_CONFIG_H

/**
 * CONFIG_BT: Tx thread stack size
 */

#ifndef CONFIG_BT_HCI_TX_STACK_SIZE
#define CONFIG_BT_HCI_TX_STACK_SIZE 256
#endif

#ifndef CONFIG_BT_L2CAP_TX_BUF_COUNT
#define CONFIG_BT_L2CAP_TX_BUF_COUNT 2
#endif

#ifndef CONFIG_BT_RX_BUF_COUNT
#define CONFIG_BT_RX_BUF_COUNT 2
#endif

#ifndef CONFIG_BT_MAX_CONN
#define CONFIG_BT_MAX_CONN 1
#endif

#if defined(__cplusplus)
}
#endif

#endif /* BLE_DEF_CONFIG_H */
