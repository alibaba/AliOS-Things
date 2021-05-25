/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <aos/hal/wifi.h>
#include <vfsdev/wifi_dev.h>
#include <devicevfs/devicevfs.h>

#include "wifi_inter.h"

#define PLATFORM_wifi_NUM  4
// wifi device node will be named with "/dev/wifi<x>", where <x> is wifi port id
#define WIFI_DEV_NAME_FORMAT "wifi%d"

#ifndef BUG_ON_MSG
#define BUG_ON_MSG(condition, fmt, ...) do { \
	if(condition) { \
		printf("BUG at %s:%d! "fmt"\r\n", __func__, __LINE__, ##__VA_ARGS__); \
	} \
} while (0)
#endif

typedef struct vfs_wifi {
    netdev_t dev;
} vfs_wifi_t;

int wifi_device_ioctl (file_t *f, int cmd, unsigned long arg)
{
    int ret = 0;
    vfs_wifi_t *vd = (vfs_wifi_t *)f->node->i_arg;
    netdev_t *wifi_dev = &vd->dev;

    // arg is channel info
    ddkc_info("cmd:0x%x, arg:0x%lx\r\n", cmd, arg);

    switch (cmd) {
    case WIFI_DEV_CMD_GET_BASE_INFO: {
        wifi_base_info_t *info = (wifi_base_info_t*)(arg);

        ret = hal_wifi_get_base_info(wifi_dev, info);
        break;
    }
    case WIFI_DEV_CMD_SET_MODE: {
        wifi_mode_t mode = (wifi_mode_t)(arg);

        ret = hal_wifi_set_mode(wifi_dev, mode);
        break;
    }
    case WIFI_DEV_CMD_GET_MODE: {
        wifi_mode_t *mode = (wifi_mode_t*)(arg);

        ret = hal_wifi_get_mode(wifi_dev, mode);
        break;
    }
    case WIFI_DEV_CMD_INSTALL_EVENT_CB: {
        wifi_ip_stat_t *out_net_para = (wifi_ip_stat_t*)(arg);

        ret = hal_wifi_notify_ip_state2drv(wifi_dev, out_net_para, WIFI_MODE_STA);
        break;
    }
    case WIFI_DEV_CMD_CANCEL_CONNECT: {

        ret = hal_wifi_cancel_connect(wifi_dev);
        break;
    }
    case WIFI_DEV_CMD_SET_MAC: {
        const uint8_t *mac = (const uint8_t *)(arg);

        ret = hal_wifi_set_mac_addr(wifi_dev, mac);
        break;
    }
    case WIFI_DEV_CMD_GET_MAC: {

        ret = hal_wifi_get_mac_addr(wifi_dev, (uint8_t *)(arg));
        break;
    }
    case WIFI_DEV_CMD_SET_LPM: {
        wifi_lpm_mode_t mode = (wifi_lpm_mode_t )arg;
        ret = hal_wifi_set_lpm(wifi_dev, mode);
        break;
    }
    case WIFI_DEV_CMD_GET_LPM: {
        wifi_lpm_mode_t *mode = (wifi_lpm_mode_t *)arg;

        ret = hal_wifi_get_lpm(wifi_dev, mode);
        break;
    }

    case WIFI_DEV_CMD_NOTIFY_IP_STATE2DRV: {
        wifi_ip_stat_t *out_net_para = (wifi_ip_stat_t*)(arg);

        ret = hal_wifi_notify_ip_state2drv(wifi_dev, out_net_para, WIFI_MODE_STA);
        break;
    }

    case WIFI_DEV_CMD_START_SCAN:
    {
        wifi_scan_config_t *wifi_config = NULL;
        wifi_config = (wifi_scan_config_t *)arg;

        ret = hal_wifi_start_scan(wifi_dev, wifi_config, false);

        ddkc_info("hal wifi scan:ret:%d\n", ret);

        break;
    }

    case WIFI_DEV_CMD_CONNECT:
    {
        wifi_config_t *wifi_config = NULL;
        wifi_config = (wifi_config_t *)arg;
        BUG_ON_MSG(!wifi_config, "wifi config - arg should not be NULL\r\n");

        ret = hal_wifi_connect(wifi_dev, wifi_config);
        ddkc_info("wifi ssid:%s, psw:%s ret:%d\n",wifi_config->ssid, wifi_config->password, ret);

        break;
    }

    case WIFI_DEV_CMD_DISCONNECT: {

        ret = hal_wifi_disconnect(wifi_dev);

        break;
    }
    case WIFI_DEV_CMD_STA_GET_LINK_STATUS: {
        wifi_ap_record_t *ap_info = (wifi_ap_record_t *)arg;

        ret = hal_wifi_sta_get_link_status(wifi_dev, ap_info);
        break;
    }
    case WIFI_DEV_CMD_AP_GET_STA_LIST: {

        wifi_sta_list_t *sta = (wifi_sta_list_t *)arg;
        ret = hal_wifi_ap_get_sta_list(wifi_dev, sta);
        break;
    }
    case WIFI_DEV_CMD_START_MONITOR: {

        wifi_promiscuous_cb_t cb = (wifi_promiscuous_cb_t)arg;
        ret = hal_wifi_start_monitor(wifi_dev, cb);
        break;
    }
    case WIFI_DEV_CMD_STOP_MONITOR: {

        ret = hal_wifi_stop_monitor(wifi_dev);
        break;
    }
    case WIFI_DEV_CMD_SEND_80211_RAW_FRAME: {
        raw_frame_t *raw = (raw_frame_t*)arg;

        ret = hal_wifi_send_80211_raw_frame(wifi_dev, raw->data, raw->len);
        break;
    }
    case WIFI_DEV_CMD_SET_CHANNEL: {
        int channel = (int )(arg);

        ret = hal_wifi_set_channel(wifi_dev, channel);
        break;
    }
    case WIFI_DEV_CMD_GET_CHANNEL: {
        int *channel = (int *)(arg);

        ret = hal_wifi_get_channel(wifi_dev, channel);
        break;
    }
    case WIFI_DEV_CMD_START_SPECIFIED_SCAN: {
        scan_ap_list_t *scan = (scan_ap_list_t*)arg;

        ret = hal_wifi_start_specified_scan(wifi_dev, scan->ap_list, scan->ap_num);
        break;
    }
    case WIFI_DEV_CMD_REGISTER_MONITOR_CB: {

        monitor_data_cb_t fn = (monitor_data_cb_t)arg;
        ret = hal_wifi_register_monitor_cb(wifi_dev, fn);
        break;
    }
    case WIFI_DEV_CMD_START_MGNT_MONITOR: {

        ret = hal_wifi_start_mgnt_monitor(wifi_dev);
        break;
    }
    case WIFI_DEV_CMD_STOP_MGNT_MONITOR: {

        ret = hal_wifi_stop_mgnt_monitor(wifi_dev);
        break;
    }
    case WIFI_DEV_CMD_REGISTER_MGNT_MONITOR_CB: {

        monitor_data_cb_t fn = (monitor_data_cb_t)arg;
        ret = hal_wifi_register_mgnt_monitor_cb(wifi_dev, fn);

        break;
    }
    case WIFI_DEV_CMD_SET_SMARTCFG: {

        int enable = (int)arg;
        ret = hal_wifi_set_smartcfg(wifi_dev, enable);

        break;
    }
    default:
        ret = -EINVAL;
        ddkc_err("invalid cmd:%d\r\n", cmd);
        break;
    }

    return ret;
}

int wifi_device_open (inode_t *node, file_t *f) {

    ddkc_dbg("device:%s open succeed\r\n", node->i_name);

    return 0;
}

int wifi_device_close (file_t *f) {

    ddkc_dbg("device:%s close succeed\r\n", f->node->i_name);

    return 0;
}

subsys_file_ops_t wifi_device_fops = {
    .open = wifi_device_open,
    .close = wifi_device_close,
    .read = NULL,
    .write = NULL,
    .ioctl = wifi_device_ioctl,
    .poll = NULL,
};

int wifi_device_init (struct u_platform_device *pdev) {
    // make sure 0 is returned if init operation success
    // or aos_dev_reg procedure will break and no device node will be registered
    // pdev->
    vfs_wifi_t *vd = (vfs_wifi_t *)u_platform_get_user_data(pdev);
    netdev_t *wifi_dev = &vd->dev;

    ddkc_info("wifi_dev=%p drv_ops=%p\r\n", wifi_dev, wifi_dev->drv_ops);
    hal_wifi_init(wifi_dev);
    return 0;
}

int wifi_device_deinit (struct u_platform_device *pdev) {
    ddkc_info("enter\r\n");
    return 0;
}

int wifi_device_pm (struct u_platform_device *pdev, u_pm_ops_t state) {
    ddkc_info("enter\r\n");
    return 0;
}

struct subsys_drv wifi_device_drv = {
    .drv_name = "wifi",
    .init = wifi_device_init,
    .deinit = wifi_device_deinit,
    .pm = wifi_device_pm,
};

int vfs_wifi_dev_register(wifi_driver_t *drv, int id)
{
    int ret = 0;
    int node_name_len = 0;
    struct subsys_dev *ppsdev = NULL;

    ddkc_info("wifi vfs driver init starts\r\n");

    node_name_len = strlen(WIFI_DEV_NAME_FORMAT) + 1;
    ddkc_dbg("node_name_len:%d\r\n", node_name_len);

    vfs_wifi_t *vd = malloc(sizeof(vfs_wifi_t));

    ppsdev = malloc(sizeof(struct subsys_dev) + node_name_len);
    if (!(ppsdev) || !vd) {
        ddkc_info("malloc failed, ppsdev:%p, vd:%p\r\n", ppsdev, vd);

        if (ppsdev) {
            free(ppsdev);
            ppsdev = NULL;
        }

        if (vd)
            free(vd);

        goto err;
    }

    memset(vd, 0x0, sizeof(vfs_wifi_t));
    memset(ppsdev, 0x0, sizeof(struct subsys_dev));

    vd->dev.drv_ops = drv;

    // vfs_wifi_t's port should be remained during the whole driver life
    (ppsdev)->node_name = (char *)((ppsdev) + 1);
    snprintf((ppsdev)->node_name, node_name_len, WIFI_DEV_NAME_FORMAT, id);
    ddkc_dbg("ppsdev:%p, node_name:%s, (ppsdev) + 1:%p, sizeof(struct subsys_dev):%d\r\n",
            ppsdev, (ppsdev)->node_name, (ppsdev) + 1, sizeof(struct subsys_dev));

    (ppsdev)->permission = 0;
    // please refer to definitions in enum SUBSYS_BUS_TYPE
    (ppsdev)->type = BUS_TYPE_SDIO;
    // user_data will be passed to open operation via node->i_arg
    (ppsdev)->user_data = vd;

    ret = aos_dev_reg(ppsdev, &wifi_device_fops, &wifi_device_drv);
    if (ret) {
        ddkc_err("aos_dev_reg for wifi%d failed, ret:%d\r\n", id, ret);
        goto err;
    }

    free(ppsdev);

    ddkc_info("wifi vfs driver init finish, ret:%d\r\n", ret);
    return 0;

err:
    // shall uninstall wifi devices who are already registered

    if (ppsdev) {
        aos_dev_unreg(ppsdev);
        ddkc_info("free memory for wifi%d\r\n", id);

        if ((ppsdev)->user_data)
            free((ppsdev)->user_data);

        free(ppsdev);
        ppsdev = NULL;
    }

    ddkc_err("wifi vfs driver init failed, ret:%d\r\n", ret);

    return ret;
}
