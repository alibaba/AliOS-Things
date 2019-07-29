/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UR_LOGGING_H
#define UR_LOGGING_H

typedef enum {
    UR_LOG_LEVEL_ERROR,
    UR_LOG_LEVEL_WARN,
    UR_LOG_LEVEL_INFO,
    UR_LOG_LEVEL_DEBUG,
} ur_log_level_t;

#define lvl2str(l) ({ \
    const char *_s = "unknown"; \
    switch (l) { \
    case UR_LOG_LEVEL_ERROR: _s = "error"; break; \
    case UR_LOG_LEVEL_WARN: _s = "warning"; break; \
    case UR_LOG_LEVEL_INFO: _s = "information"; break; \
    case UR_LOG_LEVEL_DEBUG: _s = "debug"; break; \
    } \
    _s; })

#define str2lvl(s) ({ \
    int _l; \
    if (strcmp(s, "error") == 0) _l = UR_LOG_LEVEL_ERROR; \
    else if (strcmp(s, "warning") == 0) _l = UR_LOG_LEVEL_WARN; \
    else if (strcmp(s, "information") == 0) _l = UR_LOG_LEVEL_INFO; \
    else if (strcmp(s, "debug") == 0) _l = UR_LOG_LEVEL_DEBUG; \
    else _l = UR_LOG_LEVEL_INFO; \
    _l; })

#define EXT_ADDR_FMT "%02x%02x%02x%02x%02x%02x%02x%02x"
#define EXT_ADDR_DATA(addr) \
                      addr[0],addr[1],addr[2],addr[3],\
                      addr[4],addr[5],addr[6],addr[7]

#define EXT_NETID_FMT "%02x:%02x:%02x:%02x:%02x:%02x"
#define EXT_NETID_DATA(addr) \
                      addr[0],addr[1],addr[2],\
                      addr[3],addr[4],addr[5]

#define IP6_ADDR_FMT "%x:%x:%x:%x:%x:%x:%x:%x"
#define IP6_ADDR_DATA(dest) \
                        ntohs(dest->m16[0]), \
                        ntohs(dest->m16[1]), \
                        ntohs(dest->m16[2]), \
                        ntohs(dest->m16[3]), \
                        ntohs(dest->m16[4]), \
                        ntohs(dest->m16[5]), \
                        ntohs(dest->m16[6]), \
                        ntohs(dest->m16[7])

#define IP4_ADDR_FMT "%d.%d.%d.%d"
#define IP4_ADDR_DATA(dest) \
                        dest->m8[0], dest->m8[1], \
                        dest->m8[2], dest->m8[3]

#define format_ip6_str(ip6_addr, buf, len) \
    snprintf(buf, len, IP6_ADDR_FMT, IP6_ADDR_DATA(ip6_addr))

void umesh_log(const char *fmt, ...);

#ifndef CONFIG_AOS_MESH_DEBUG
#define MESH_LOG_DEBUG(format, ...)
#define MESH_LOG_INFO(format, ...) umesh_log(format, ##__VA_ARGS__)
#define MESH_LOG_WARN(format, ...)
#define MESH_LOG_ERROR(format, ...)
#else
ur_log_level_t ur_log_get_level(void);
void ur_log_set_level(ur_log_level_t level);
extern int dda_log(char *str, ...);

#define MESH_LOG_LEVEL ur_log_get_level()

#ifdef CONFIG_AOS_DDA
#define LOG_OUTPUT(format, ...) \
    dda_log(format, ##__VA_ARGS__); \
    umesh_log(format, ##__VA_ARGS__);
#else
#define LOG_OUTPUT(format, ...) \
    umesh_log(format, ##__VA_ARGS__);
#endif

#define MESH_LOG_DEBUG(format, ...) \
    if (MESH_LOG_LEVEL >= UR_LOG_LEVEL_DEBUG) { \
        LOG_OUTPUT(format, ##__VA_ARGS__); \
    }
#define MESH_LOG_INFO(format, ...) \
    if (MESH_LOG_LEVEL >= UR_LOG_LEVEL_INFO) { \
        LOG_OUTPUT(format, ##__VA_ARGS__); \
    }
#define MESH_LOG_WARN(format, ...) \
    if (MESH_LOG_LEVEL >= UR_LOG_LEVEL_WARN) { \
        LOG_OUTPU(format, ##__VA_ARGS__); \
    }
#define MESH_LOG_ERROR(format, ...) \
    if (MESH_LOG_LEVEL >= UR_LOG_LEVEL_ERROR) { \
        LOG_OUTPUT(format, ##__VA_ARGS__); \
    }
#endif

#endif  /* UR_LOGGING_H */
