/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos/aos.h"
#include "device.h"
#include "git_version.h"
#include "os.h"
const char *online_server = "alink.tcp.aliyun.com";
#if defined(SUPPORT_ID2)
#define ALINK_PROTOCOL_VERSION  "1.0.SE"
#elif defined(CONFIG_SDS)
#define ALINK_PROTOCOL_VERSION  "1.0.1"
#else
#define ALINK_PROTOCOL_VERSION  "1.0"
#endif

#define MODULE_NAME "alink.device"

static device_info_t main_devinfo;
static const device_t main_dev = {&main_devinfo, &main_config};

const device_t *get_main_dev(void)
{
    return &main_dev;
}

static void devinfo_dump(void)
{
    char *version, *module_name;
    version = (char *)os_zalloc(STR_SHORT_LEN);
    module_name = (char *)os_zalloc(STR_SHORT_LEN);
    OS_CHECK_MALLOC(version && module_name);

    os_get_version(version);
    os_get_module_name(module_name);

    LOGI(MODULE_NAME, "~~~~~dump device info~~~~~");
    LOGI(MODULE_NAME, "sn: %s", main_devinfo.sn);
    LOGI(MODULE_NAME, "cid: %s", main_devinfo.cid);
    LOGI(MODULE_NAME, "mac: %s", main_devinfo.mac);
    LOGI(MODULE_NAME, "model: %s", main_devinfo.model);
    LOGI(MODULE_NAME, "os_version: %s", main_devinfo.os_version);
    LOGI(MODULE_NAME, "alink_version: %s", main_devinfo.alink_version);
    LOGI(MODULE_NAME, "firmware_version: %s", main_devinfo.firmware_version);
    LOGI(MODULE_NAME, "key: %s", main_devinfo.key);
    LOGI(MODULE_NAME, "secret: %s", main_devinfo.secret);
    LOGI(MODULE_NAME, "debug_key: %s", main_devinfo.debug_key);
    LOGI(MODULE_NAME, "debug_secret: %s", main_devinfo.debug_secret);
#ifdef CONFIG_SDS
    LOGI(MODULE_NAME, "device key: %s", main_devinfo.device_key);
    LOGI(MODULE_NAME, "device secret: %s", main_devinfo.device_secret);
#endif
    LOGI(MODULE_NAME, "A[%s|%s|%04x]OS[%s]T[%s.%s]", main_devinfo.alink_version,
         ALINK_AGENT_GIT_VERSION, 0x0000, version, module_name, ALINK_AGENT_BUILD_TIME);
    LOGI(MODULE_NAME, "~~~~~~~~~~~~~~~~~~~~~~~~~~");

    os_free(version);
    os_free(module_name);
}

char *devinfo_get_secret(void)
{
    if (strstr(main_config.alinkserver, online_server)) {
        return main_devinfo.secret;
    } else {
        return main_devinfo.debug_secret;
    }
}

char *devinfo_get_key(void)
{
    if (strstr(main_config.alinkserver, online_server)) {
        return main_devinfo.key;
    } else {
        return main_devinfo.debug_key;
    }
}

char *devinfo_get_version(void)
{
    return main_devinfo.firmware_version;
}


#ifdef CONFIG_SDS
char *devinfo_get_device_key(void)
{
    return main_devinfo.device_key;
}

char *devinfo_get_device_secret(void)
{
    return main_devinfo.device_secret;
}
#endif

static int devinfo_init(void)
{
    product_init();
    memset(&main_devinfo, 0, sizeof(main_devinfo));

    os_get_chipid(main_devinfo.cid);
    os_wifi_get_mac_str(main_devinfo.mac);
    os_product_get_sn(main_devinfo.sn);
    os_product_get_model(main_devinfo.model);

#ifdef CONFIG_SDS
    os_get_device_key(main_devinfo.device_key);
    os_get_device_secret(main_devinfo.device_secret);
#endif
    os_get_version(main_devinfo.os_version);
    strcpy(main_devinfo.alink_version, ALINK_PROTOCOL_VERSION);
    os_product_get_version(main_devinfo.firmware_version);

#ifdef SUPPORT_ID2
    {
        uint8_t id2[TFS_ID2_LEN + 1] = { 0 };
        uint32_t len = TFS_ID2_LEN;
        tfs_get_ID2(id2, &len);
        OS_ASSERT(id2[0], "get ID2 failed");

        strncpy(main_devinfo.key, id2, PRODUCT_KEY_LEN);
        /* debug_key is useless when ID2 enabled */
        strncpy(main_devinfo.debug_key, id2, PRODUCT_KEY_LEN);
    }
#else
    os_product_get_key(main_devinfo.key);
    os_product_get_secret(main_devinfo.secret);
    os_product_get_debug_key(main_devinfo.debug_key);
    os_product_get_debug_secret(main_devinfo.debug_secret);
#endif

    devinfo_dump();

    return 0;
}

void device_init(void)
{
    devinfo_init();
    config_init();
}

void device_exit(void)
{
}
