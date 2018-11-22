#ifndef _MESH_CONFIG_H_
#define _MESH_CONFIG_H_

#ifdef CONFIG_BT_MESH

#ifndef CONFIG_BT_MESH_MODEL_KEY_COUNT
#define CONFIG_BT_MESH_MODEL_KEY_COUNT 2
#endif

#ifndef CONFIG_BT_MESH_MODEL_GROUP_COUNT
#define CONFIG_BT_MESH_MODEL_GROUP_COUNT 2
#endif

#ifndef CONFIG_BT_MESH_APP_KEY_COUNT
#define CONFIG_BT_MESH_APP_KEY_COUNT 1
#endif

#ifndef CONFIG_BT_MESH_SUBNET_COUNT
#define CONFIG_BT_MESH_SUBNET_COUNT 2
#endif

#ifndef CONFIG_BT_MESH_CRPL
#define CONFIG_BT_MESH_CRPL 5
#endif

#ifndef CONFIG_BT_MESH_ADV_BUF_COUNT
#define CONFIG_BT_MESH_ADV_BUF_COUNT 6
#endif

#ifndef CONFIG_BT_MESH_LABEL_COUNT
#define CONFIG_BT_MESH_LABEL_COUNT 1
#endif

#ifndef CONFIG_BT_MESH_MSG_CACHE_SIZE
#define CONFIG_BT_MESH_MSG_CACHE_SIZE 2
#endif

#ifndef CONFIG_BT_MESH_TX_SEG_MSG_COUNT
#define CONFIG_BT_MESH_TX_SEG_MSG_COUNT 2
#endif

#ifndef CONFIG_BT_MESH_RX_SDU_MAX
#define CONFIG_BT_MESH_RX_SDU_MAX 36
#endif

#ifndef CONFIG_BT_MESH_RX_SEG_MSG_COUNT
#define CONFIG_BT_MESH_RX_SEG_MSG_COUNT 2
#endif

#ifndef CONFIG_BT_MESH_ADV_PRIO
#define CONFIG_BT_MESH_ADV_PRIO 41
#endif

#ifndef CONFIG_BT_MESH_PROXY_FILTER_SIZE
#define CONFIG_BT_MESH_PROXY_FILTER_SIZE 1
#endif

#ifndef CONFIG_BT_MESH_NODE_ID_TIMEOUT
#define CONFIG_BT_MESH_NODE_ID_TIMEOUT 60
#endif

#endif /* endof CONFIG_BT_MESH */

#endif
