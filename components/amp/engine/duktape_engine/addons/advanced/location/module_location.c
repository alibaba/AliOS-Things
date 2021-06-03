/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdarg.h>

#include "amp_config.h"
#include "aos_system.h"
#include "amp_defines.h"
#include "be_inl.h"
#include "amp_location.h"
#include "aos_network.h"


#define MOD_STR        "LOCATION"

#define WIFI_SCAN_INFO_LEN		2048
#define WIFI_ACCESS_INFO_LEN	128

#define LBS_SCAN_INFO_LEN		2048
#define LBS_ACCESS_INFO_LEN	    128

static char wifi_scan_info[WIFI_SCAN_INFO_LEN];
static char wifi_access_info[WIFI_ACCESS_INFO_LEN];

static char lbs_scan_info[LBS_SCAN_INFO_LEN];
static char lbs_access_info[LBS_ACCESS_INFO_LEN];

static duk_ret_t native_location_access_wifi_info(duk_context *ctx)
{
#if defined(AMP_N58)
     int ret     = -1;
     amp_wifi_info_t wifi_info;

     memset(&wifi_info, 0, sizeof(wifi_info));
     ret = amp_location_access_wifi_info(&wifi_info);
     if (ret != 0) {
         amp_debug(MOD_STR, "get location wifi info failed");
         goto out;
     }

 	memset(wifi_access_info, 0, sizeof(wifi_access_info));

 	snprintf(wifi_access_info, sizeof(wifi_access_info),
 			"%02x:%02x:%02x:%02x:%02x:%02x,%d,%s",
 			wifi_info.mac[0], wifi_info.mac[1],
 			wifi_info.mac[2], wifi_info.mac[3],
 			wifi_info.mac[4], wifi_info.mac[5],
 			wifi_info.rssi, strlen(wifi_info.ssid) > 0 ? wifi_info.ssid : "TPLink");

     duk_push_lstring(ctx, wifi_access_info, strlen(wifi_access_info));
     return 1;

out:
    duk_push_int(ctx, ret);
#endif
    return 1;
}

static duk_ret_t native_location_scaned_wifi_info(duk_context *ctx)
{
#if defined(AMP_N58)
    int ret     = -1;
    scanned_wifi_info_t scaned_info;
 	amp_wifi_info_t *wifi_info;
 	int len = 0;
 	int i;

    memset(&scaned_info, 0, sizeof(scaned_info));
    ret = amp_location_scaned_wifi_info(&scaned_info);
    if (ret != 0) {
        amp_debug(MOD_STR, "get location wifi info failed");
        goto out;
    }

    memset(wifi_scan_info, 0, sizeof(wifi_scan_info));

 	for (i = 0; i < scaned_info.num; i++) {
 		wifi_info = &scaned_info.wifi_info[i];
 		if (i == 0) {
 			len += snprintf(wifi_scan_info + len,
 				WIFI_SCAN_INFO_LEN - len,
 				"%02x:%02x:%02x:%02x:%02x:%02x,%d,%s",
 				wifi_info->mac[0], wifi_info->mac[1],
 				wifi_info->mac[2], wifi_info->mac[3],
 				wifi_info->mac[4], wifi_info->mac[5],
 				wifi_info->rssi, strlen(wifi_info->ssid) > 0 ? wifi_info->ssid : "TPLink");
 		} else {
 			len += snprintf(wifi_scan_info + len,
 				WIFI_SCAN_INFO_LEN - len,
 				"|%02x:%02x:%02x:%02x:%02x:%02x,%d,%s",
 				wifi_info->mac[0], wifi_info->mac[1],
 				wifi_info->mac[2], wifi_info->mac[3],
 				wifi_info->mac[4], wifi_info->mac[5],
 				wifi_info->rssi, strlen(wifi_info->ssid) > 0 ? wifi_info->ssid : "TPLink");
 		}
 	}

    duk_push_lstring(ctx, wifi_scan_info, strlen(wifi_scan_info));
    return 1;

out:
    duk_push_int(ctx, ret);
#endif
    return 1;
}

static duk_ret_t native_location_access_lbs_info(duk_context *ctx)
{
    int ret     = -1;
    amp_locator_info_t locator_info;
	int i;

    ret = amp_get_locator_info(&locator_info);
    if (ret != 0) {
        amp_debug(MOD_STR, "get location wifi lbs failed");
        goto out;
    }

	duk_push_object(ctx);

    duk_push_number(ctx, locator_info.cellid);
    duk_put_prop_string(ctx, -2, "cellid");

    duk_push_number(ctx, locator_info.lac);
    duk_put_prop_string(ctx, -2, "lac");

    duk_push_string(ctx, locator_info.mcc);
    duk_put_prop_string(ctx, -2, "mcc");

    duk_push_string(ctx, locator_info.mnc);
    duk_put_prop_string(ctx, -2, "mnc");

    duk_push_number(ctx, locator_info.signal);
    duk_put_prop_string(ctx, -2, "signal");

    return 1;

out:
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_location_near_lbs_info(duk_context *ctx)
{
//     int ret     = -1;
//     scanned_locator_info_t nearbts;
// 	int i;

//     ret = HAL_Get_Scanned_LocatorInfo(&nearbts);
//     if (ret != 0) {
//         amp_debug(MOD_STR, "get location wifi lbs failed");
//         goto out;
//     }

//     duk_idx_t arr_idx = duk_push_array(ctx);

//     for (i = 0; i < nearbts.num; i++) {

//         duk_push_object(ctx);

//         duk_push_number(ctx, nearbts.locator_info[i].cellid);
//         duk_put_prop_string(ctx, -2, "cellid");

//         duk_push_number(ctx, nearbts.locator_info[i].lac);
//         duk_put_prop_string(ctx, -2, "lac");

//         duk_push_string(ctx, nearbts.locator_info[i].mcc);
//         duk_put_prop_string(ctx, -2, "mcc");

//         duk_push_string(ctx, nearbts.locator_info[i].mnc);
//         duk_put_prop_string(ctx, -2, "mnc");

//         duk_push_number(ctx, nearbts.locator_info[i].rsrp);
//         duk_put_prop_string(ctx, -2, "rsrp");
//     }

//     return 1;

// out:
//     duk_push_int(ctx, ret);
//     return 1;
}

void module_location_register(void)
{
    duk_context *ctx = be_get_context();

    duk_push_object(ctx);

    AMP_ADD_FUNCTION("accessedWifi", native_location_access_wifi_info, 0);
    AMP_ADD_FUNCTION("scannedWifi",  native_location_scaned_wifi_info, 0);
    AMP_ADD_FUNCTION("accessedLbs",  native_location_access_lbs_info, 0);
    AMP_ADD_FUNCTION("nearbts",      native_location_near_lbs_info, 0);

    duk_put_prop_string(ctx, -2, "LOCATION");
}