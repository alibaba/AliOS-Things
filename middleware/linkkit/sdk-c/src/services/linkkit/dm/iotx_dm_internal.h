/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _IOTX_DM_INTERNAL_H_
#define _IOTX_DM_INTERNAL_H_

#ifndef _IN_
    #define _IN_
#endif

#ifndef _OU_
    #define _OU_
#endif

#ifndef DM_READ_ONLY
    #define DM_READ_ONLY
#endif

#define _BSD_SOURCE

#include <stdlib.h>

#include "iot_import.h"
#include "iot_export.h"
#include "iotx_utils.h"
#include "iotx_system.h"

#if defined(OTA_ENABLED) && !defined(BUILD_AOS)
    #include "iotx_ota.h"
#endif

/* CM Header File */
#include "iotx_cm.h"

/* ALCS Header File */
#ifdef ALCS_ENABLED
    #include "CoAPExport.h"
    #include "iotx_alcs.h"
#endif

/* DM Header File */
#include "iotx_dm_config.h"
#include "iotx_dm.h"
#include "dm_utils.h"
#include "dm_shadow.h"
#include "dm_tsl_alink.h"
#include "dm_message_cache.h"
#include "dm_opt.h"
#include "dm_ota.h"
#include "dm_cota.h"
#include "dm_fota.h"
#include "dm_ipc.h"
#include "dm_message.h"
#include "dm_msg_process.h"
#include "dm_manager.h"
#include "dm_client_adapter.h"
#include "dm_client.h"
#include "dm_server_adapter.h"
#include "dm_server.h"
#include "dm_api.h"

#define DM_SUPPORT_MEMORY_MAGIC
#ifdef DM_SUPPORT_MEMORY_MAGIC
    #define DM_malloc(size) LITE_malloc(size, MEM_MAGIC, "dm")
#else
    #define DM_malloc(size) LITE_malloc(size)
#endif
#define DM_free(ptr)   {LITE_free(ptr);ptr = NULL;}

#if defined(COAP_COMM_ENABLED) && !defined(MQTT_COMM_ENABLED)
    #define DM_URI_OFFSET 1
#else
    #define DM_URI_OFFSET 0
#endif

#if 1
    #define dm_log_emerg(...)     log_emerg("DM", __VA_ARGS__)
    #define dm_log_crit(...)      log_crit("DM", __VA_ARGS__)
    #define dm_log_err(...)       log_err("DM", __VA_ARGS__)
    #define dm_log_warning(...)   log_warning("DM", __VA_ARGS__)
    #define dm_log_info(...)      log_info("DM", __VA_ARGS__)
    #define dm_log_debug(...)     log_debug("DM", __VA_ARGS__)
#else
    #define dm_log_emerg(...)
    #define dm_log_crit(...)
    #define dm_log_err(...)
    #define dm_log_warning(...)
    #define dm_log_info(...)
    #define dm_log_debug(...)
#endif

#endif
