/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __WIFI_PROVISION_INTERNAL_H__
#define __WIFI_PROVISION_INTERNAL_H__

#include "infra_config.h"
#include <string.h>
#include <stdio.h>
#include "aws_lib.h"
#include "zconfig_lib.h"
#include "zconfig_utils.h"
#include "zconfig_protocol.h"
#include "zconfig_ieee80211.h"
#include "awss_event.h"
#include "awss_timer.h"
#include "awss_main.h"
#include "os.h"
#include "infra_compat.h"
#include "awss_smartconfig.h"
#include "infra_sha1.h"
#include "passwd.h"
#include "awss_utils.h"
#include "awss_statis.h"
#include "awss_packet.h"
#include "awss_notify.h"
#include "awss_cmp.h"
#include "linkkit/wifi_provision_api.h"
#include "awss_cmp.h"
#include "awss_crypt.h"
#include <stdlib.h>
#include "infra_json_parser.h"
#include "linkkit/mqtt_api.h"
#include "awss_dev_reset_internal.h"
#include "awss_info.h"
#include "awss_bind_statis.h"
#include "awss_aplist.h"
#include "connect_ap.h"
#include "infra_aes.h"
#include "wrappers.h"

#ifdef AWSS_SUPPORT_SMARTCONFIG_WPS
    #include "awss_wps.h"
#endif

#ifdef AWSS_SUPPORT_HT40
    #include "awss_ht40.h"
#endif

#if defined(AWSS_SUPPORT_AHA)
    #include "awss_wifimgr.h"
#endif

#ifndef AWSS_DISABLE_ENROLLEE
    #include "awss_enrollee.h"
#endif

#if defined(AWSS_SUPPORT_AHA)
    #include "awss_aha.h"
#endif

#if defined(WIFI_PROVISION_ENABLED) || defined(DEV_BIND_ENABLED)
    #include "linkkit/coap_api.h"
    #include "iotx_coap.h"
#endif

#ifdef AWSS_SUPPORT_DISCOVER
    #include "awss_discover.h"
#endif

#endif
