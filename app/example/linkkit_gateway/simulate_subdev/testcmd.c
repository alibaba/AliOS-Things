/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <aos/aos.h>
#include "testcmd_lock.h"
#include "linkkit_gateway_export.h"
#include "iot_import.h"

#define PRODUCT_KEY_MAXLEN          (20 + 1)
#define DEVICE_NAME_MAXLEN          (32 + 1)
#define DEVICE_SECRET_MAXLEN        (64 + 1)
typedef struct device_s {
    char productKey[PRODUCT_KEY_MAXLEN];
    char deviceName[DEVICE_NAME_MAXLEN];
    char deviceSecret[DEVICE_SECRET_MAXLEN];

    int devid;
    int subdev;

    int login: 1,
        disabled: 1,
        devtype: 4;
    void *priv_data;
    struct device_s *next;
} device_t;

extern int linkkit_gateway_testcmd_post_properties(const char *properties);

extern int linkkit_gateway_testcmd_post_event(const char *identifier, const char *events);

extern int linkkit_gateway_check_permit(int devtype, const char *productKey);

static device_t *first_device = NULL;
static int gbl_devcnt = 1;

#define SUBDEV_MAX_NUMBER 3


static int get_subdev_cnt()
{
    device_t *node;
    int count = 0;

    for (node = first_device; node; node = node->next) {
        count++;
    }

    return count;
}

static int device_get_property(char *in, char *out, int out_len, void *ctx)
{
    LOG("get property, in: %s\n", in);
    device_t *subdev = ctx;

	testcmd_lock_get_propery(in, out, out_len, subdev->priv_data);

    return 0;
}

static int device_set_property(char *in, void *ctx)
{
    device_t *subdev = ctx;
    device_t *node;

    LOG("set property, in: %s\n", in);
    for (node = first_device; node; node = node->next) {
        if (node->devid == subdev->devid) {
            break;
        }
    }
    if (!node) {
        LOG("subdevice is not existed\n");
        return -1;
    }

    testcmd_lock_set_property(in, subdev->priv_data);

	linkkit_gateway_post_property_json_sync(node->subdev, in, 10000);

    return 0;
}

static int device_call_service(char *identifier, char *in, char *out, int out_len, void *ctx)
{
    device_t *subdev = (device_t *)ctx;
    device_t *node;

    LOG("call service, id: %s, in: %s\n", identifier, in);
    for (node = first_device; node; node = node->next) {
        if (node->devid == subdev->devid) {
            break;
        }
    }
    if (!node) {
        LOG("subdevice is not existed\n");
        return -1;
    }

    if (testcmd_lock_call_service(subdev->subdev, identifier, in, out, out_len, subdev->priv_data)) {
        LOG("call service %s failed, in: %s!\n", identifier, in);
        return -1;
    }

    return 0;
}

static ssize_t device_down_rawdata(const void *in, int in_len, void *out, int out_len, void *ctx)
{
    LOG("down rawdata!\n");
    return 0;
}

static int device_post_rawdata_reply(const void *data, int len, void *ctx)
{
    LOG("post rawdata reply\n");
    return 0;
}

static linkkit_cbs_t device_cbs = {
    .get_property = device_get_property,
    .set_property = device_set_property,

    .call_service = device_call_service,

    .down_rawdata = device_down_rawdata,

    .post_rawdata_reply = device_post_rawdata_reply,
};

static device_t *find_device(char *productKey, char *deviceName)
{
    device_t *dev = NULL;
    for (dev = first_device; dev; dev = dev->next) {
        if (strcmp(dev->productKey, productKey) == 0 &&
            strcmp(dev->deviceName, deviceName) == 0)
            return dev;
    }

    return NULL;
}

static int create_device(int devtype, char *productKey, char *deviceName, char *deviceSecret, uint32_t *devid)
{
    device_t *dev = NULL;
    int new = 0;

    if (SUBDEV_MAX_NUMBER <= get_subdev_cnt()) {
        LOG("It's allowed to create %d subdevs at most!\n", SUBDEV_MAX_NUMBER);
        return -1;
    }

    if (!linkkit_gateway_check_permit(devtype, productKey)) {
        LOG("%s is not permitted to join for now!\n", productKey);
        return -1;
    }

    dev = find_device(productKey, deviceName);
    if (!dev) {
        dev = malloc(sizeof(device_t));
        if (!dev) {
            LOG("malloc device_t failed!\n");
            return -1;
        }
        memset(dev, 0, sizeof(device_t));

        strncpy(dev->productKey, productKey, sizeof(dev->productKey) - 1);
        strncpy(dev->deviceName, deviceName, sizeof(dev->deviceName) - 1);
        dev->devtype = devtype;

        dev->priv_data = testcmd_lock_alloc_property();
        if (!dev->priv_data) {
            HAL_Free(dev);
            LOG("malloc device private data failed!\n");
            return -1;
        }

        dev->subdev = linkkit_gateway_subdev_create(productKey, deviceName, &device_cbs, dev);
        if (dev->subdev < 0) {
            //linkkit_gateway_subdev_unregister(productKey, deviceName);
            testcmd_lock_destroy_property(&dev->priv_data);
            HAL_Free(dev);
            LOG("linkkit_gateway_subdev_create %s<%s> failed\n", deviceName, productKey);
            return -1;
        }

        if (linkkit_gateway_subdev_register(productKey, deviceName, deviceSecret) < 0) {
            linkkit_gateway_subdev_destroy(dev->subdev);
            testcmd_lock_destroy_property(&dev->priv_data);
            HAL_Free(dev);
            LOG("subdev register failed!\n");
            return -1;
        }

        new = 1;
    }
    if (!dev->login) {
        if (linkkit_gateway_subdev_login(dev->subdev) < 0) {
            LOG("linkkit_gateway_subdev_login %s<%s> failed\n", deviceName, productKey);
            linkkit_gateway_subdev_destroy(dev->subdev);
            linkkit_gateway_subdev_unregister(productKey, deviceName);
            testcmd_lock_destroy_property(&dev->priv_data);
            HAL_Free(dev);
            return -1;
        }
        dev->login = 1;
    }

    if (new) {
        dev->devid = gbl_devcnt++;

        /* add device to list */
        dev->next = first_device;
        first_device = dev;
    }

    *devid = dev->devid;
    return 0;
}

static device_t *find_device_by_devid(int devid)
{
    device_t *dev = NULL;
    for (dev = first_device; dev; dev = dev->next) {
        if (dev->devid == devid)
            return dev;
    }

    return NULL;
}

static int device_logout(int devid)
{
    device_t *dev = find_device_by_devid(devid);
    if (!dev)
        return -1;

    linkkit_gateway_subdev_logout(dev->subdev);

    dev->login = 0;

    return 0;
}

static int device_login(int devid)
{
    device_t *dev = find_device_by_devid(devid);
    if (!dev)
        return -1;

    if (linkkit_gateway_subdev_login(dev->subdev) < 0)
        return -1;

    dev->login = 1;

    return 0;
}

static int destroy_device(int devid)
{
    LOG("in %s\n", __func__);
    device_t *dev = find_device_by_devid(devid);
    if (!dev)
        return -1;

    device_t **dp, *d1;

    /* remove device from list */
    dp = &first_device;
    while ((*dp) != NULL) {
        d1 = *dp;
        if (d1 == dev)
            *dp = dev->next;
        else
            dp = &d1->next;
    }

    if (linkkit_gateway_subdev_logout(dev->subdev) < 0) {
        LOG("linkkit_gateway_subdev_logout %s<%s> failed\n", dev->deviceName, dev->productKey);
        return -1;
    }

    linkkit_gateway_subdev_destroy(dev->subdev);

    if (linkkit_gateway_subdev_unregister(dev->productKey, dev->deviceName) < 0) {
        LOG("linkkit_gateway_subdev_unregister %s<%s> failed\n", dev->deviceName, dev->productKey);
        return -1;
    }
    testcmd_lock_destroy_property(&dev->priv_data);
    HAL_Free(dev);

    return 0;
}

static int post_event(int devid, char *identifier, char *events)
{
    device_t *dev = find_device_by_devid(devid);
    if (!dev)
        return -1;

    if (linkkit_gateway_trigger_event_json_sync(dev->subdev, identifier, events, 10000) < 0)
        return -1;

    return 0;
}

//linkkit2.2.0 do not support linkkit_gateway_reset
static int reset(int devid)
{
    // LOG("in %s\n", __func__);
    // device_t *dev = find_device_by_devid(devid);
    // if (!dev)
    //     return -1;

    // if (linkkit_gateway_reset(dev->subdev) < 0)
    //     return -1;

    // device_t **dp, *d1;

    // /* remove device from list */
    // dp = &first_device;
    // while ((*dp) != NULL) {
    //     d1 = *dp;
    //     if (d1 == dev)
    //         *dp = dev->next;
    //     else
    //         dp = &d1->next;
    // }

    // HAL_Free(dev);

    return 0;
}

static int post_property(int devid, char *properties)
{
    device_t *dev = find_device_by_devid(devid);
    if (!dev)
        return -1;

    if (linkkit_gateway_post_property_json_sync(dev->subdev, properties, 10000) < 0)
        return -1;

    return 0;
}

static int post_rawdata(int devid, void *data, int len)
{
    device_t *dev = find_device_by_devid(devid);
    if (!dev)
        return -1;

    if (linkkit_gateway_post_rawdata(dev->subdev, data, len) < 0)
        return -1;

    return 0;
}


static void handle_gw_test_list_cmd()
{
    device_t *dev;
    for (dev = first_device; dev; dev = dev->next) {
        LOG("       devid: %d\n", dev->devid);
        LOG("     devtype: %d\n", dev->devtype);
        LOG("  productKey: %s\n", dev->productKey);
        LOG("  deviceName: %s\n", dev->deviceName);
        LOG("deviceSecret: %s\n", dev->deviceSecret);
    }
    return ;
}

static void handle_gw_test_create_cmd(char *str_devtype, char *productKey, char *deviceName, char *deviceSecret)
{
    uint32_t devid = 0;
    int devtype = str_devtype[0] - '0';

    if (0 != create_device(devtype, productKey, deviceName, deviceSecret, &devid)) {
        LOG("create device failed!\n");
        return ;
    }
    LOG("Sub device created and devid is %u\n", devid);
}

static void gw_test_action_login(uint32_t devid)
{
    if (0 != device_login(devid))
        LOG("login failed!\n");

    return ;
}

static void gw_test_action_logout(uint32_t devid)
{
    if (0 != device_logout(devid))
        LOG("logout failed!\n");

    return ;
}

static void gw_test_action_delete(uint32_t devid)
{
    if (0 != destroy_device(devid))
        LOG("logout failed!\n");

    return ;
}

static void gw_test_action_reset(uint32_t devid)
{
    if (0 != reset(devid))
        LOG("dev reset failed!\n");

    return ;
}

static void gw_test_action_post_prop(uint32_t devid, const char *prop_in_json)
{
    post_property(devid, (char *)prop_in_json);

    return ;
}

static void gw_test_action_post_event(uint32_t devid, const char *identifier, const char *events)
{
    post_event(devid, (char *)identifier, (char *)events);

    return ;
}

static void gw_test_action_post_rawdata(uint32_t devid, const char *rawdata)
{
    post_rawdata(devid, (void *)rawdata, strlen(rawdata));

    return ;
}

static void handle_gw_test_dev_action(int argc, char *argv[])
{
    uint32_t devid;

    if (argc <= 0)
        return;

    devid = (uint32_t)atoi(argv[0]);
    LOG("devid is %u\n", devid);

    if (!strcmp(argv[1], "login")) {
        gw_test_action_login(devid);
    } else if (!strcmp(argv[1], "logout")) {
        gw_test_action_logout(devid);
    } else if (!strcmp(argv[1], "delete")) {
        gw_test_action_delete(devid);
    } else if (!strcmp(argv[1], "reset")) {
        gw_test_action_reset(devid);
    } else if (!strcmp(argv[1], "post_prop")) {
        gw_test_action_post_prop(devid, argv[2]);
    } else if (!strcmp(argv[1], "post_event")) {
        gw_test_action_post_event(devid, argv[2], argv[3]);
    } else if (!strcmp(argv[1], "post_rawdata")) {
        gw_test_action_post_rawdata(devid, argv[2]);
    } else {
        LOG("Unsupported action: %s\n", argv[1]);
    }
    return ;
}

static void handle_gw_testcmd(int argc, char *argv[])
{
    if (argc <= 0)
        return ;

    if (!strcmp(argv[0], "post_prop")) {
        linkkit_gateway_testcmd_post_properties(argv[1]);
    } else if (!strcmp(argv[0], "post_event")) {
        linkkit_gateway_testcmd_post_event(argv[1], argv[2]);
    } else {
        LOG("Unsupported gateway command: %s\n", argv[0]);
    }
    return ;
}

static void handle_gw_test_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    if (!strcmp(argv[1], "list")) {
        if (argc > 2) {
            LOG("invalid command arguments!\n");
            goto out;
        }
        handle_gw_test_list_cmd();
    } else if (!strcmp(argv[1], "create")) {
        if (argc != 6) {
            LOG("invalid command arguments!\n");
            goto out;
        }
        handle_gw_test_create_cmd(argv[2], argv[3], argv[4], argv[5]);
    } else if (!strcmp(argv[1], "devid")) {
        if (argc < 3)
            goto out;
        handle_gw_test_dev_action(argc - 2, &argv[2]);
    } else if (!strcmp(argv[1], "gateway")) {
        handle_gw_testcmd(argc - 2, &argv[2]);
    } else {
        LOG("invalid command arguments!\n");
        goto out;
    }
out:
    return ;
}
struct cli_command gw_test_cmd = {
    .name = "testcmd",
    .help = "testcmd list\ntestcmd create [type PK DN PS]\ntestcmd devid <id> [login|logout|delete|reset|post_prop <property>|post_event <identifier> <event>|post_rawdata <rawdata>]\ntestcmd gateway [post_prop <property>|post_event <identifier> <event>]",
    .function = handle_gw_test_cmd
};

int testcmd_delete_device(const char *productKey, const char *deviceName)
{
    device_t *subdev;

    subdev = find_device((char *)productKey, (char *)deviceName);
    if (!subdev) {
        LOG("subdev %s<%s> not existed!\n", productKey, deviceName);
        return -1;
    }

    destroy_device(subdev->devid);

    return 0;
}
