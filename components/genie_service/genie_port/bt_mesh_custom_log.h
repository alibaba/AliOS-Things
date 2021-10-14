#ifndef _BT_MESH_CUSTOM_LOG_H_
#define _BT_MESH_CUSTOM_LOG_H_

#ifdef USE_BT_MESH_CUSTOM_LOG

#ifdef BT_DBG
#undef BT_DBG
#define BT_DBG(f, ...) printf(f "\n", ##__VA_ARGS__)
#endif

#ifdef BT_INFO
#undef BT_INFO
#define BT_INFO(f, ...) printf(f "\n", ##__VA_ARGS__)
#endif

#endif
#if defined(USE_BT_MESH_CUSTOM_LOG) || defined(USE_BT_MESH_CUSTOM_ERR_LOG)
#ifdef BT_WARN
#undef BT_WARN
#define BT_WARN(f, ...) printf("[%s]" f "\n", __func__, ##__VA_ARGS__)
#endif

#ifdef BT_ERR
#undef BT_ERR
#define BT_ERR(f, ...) printf("[%s]" f "\n", __func__, ##__VA_ARGS__)
#endif

#endif

#endif
