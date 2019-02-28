/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef BLE_CONFIG_H
#define BLE_CONFIG_H

/**
 * CONFIG_BT: Tx thread stack size
 */

#ifndef CONFIG_BT_HCI_TX_STACK_SIZE
#define CONFIG_BT_HCI_TX_STACK_SIZE 512
#endif

#ifndef CONFIG_BT_HCI_RX_STACK_SIZE
#define CONFIG_BT_HCI_RX_STACK_SIZE 1024
#endif

#define CONFIG_BT_HOST_RX_THREAD

#if defined(__cplusplus)
}
#endif

#endif /* BLE_DEF_CONFIG_H */
