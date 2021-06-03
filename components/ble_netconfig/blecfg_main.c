/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "blecfg_pub.h"

#include "netmgr.h"

#define AP_INFO_MAX                     8
#define BLE_UP_MTU                      20

/*
使用event进行任务同步，约定如下：
event_flag bit0, 表示ssid与passwd已经设定，但还未确认是否正确
event_flag bit1, 表示ssid与passwd正确，已经接入ap
event_flag bit2, 表示三元组已经设定，但还未确认是否正确
event_flag bit3, 表示三元组正确，已经链接云端
*/
#define LINK_EVT_WIFI_AVAIL             0x1
#define LINK_EVT_WIFI_CONNECTED         0x2
#define LINK_EVT_LINKSDK_AVAIL          0x4
#define LINK_EVT_LINKSDK_CONNECTED      0x8

static aos_event_t g_blecfg_event;


static void BleCfg_main_task(void *arg)
{
    BLECFG_STATE ret_ble;
    aos_status_t ret_aos;
    uint32_t     event_flag;
    int32_t      wifi_ok = 0;
    int32_t      linksdk_ok = 0;

    (void)arg;

    while ( 1 ) {
        event_flag = 0;
        ret_aos = aos_event_get(&g_blecfg_event, 0xffffffff, AOS_EVENT_OR_CLEAR, &event_flag, AOS_WAIT_FOREVER);
        if ( ret_aos != 0 ) {
            BLECFG_LOG_ERROR("%s: blecfg evnet error, ret %d\r\n", __func__, ret_aos);
            break;
        }
        BLECFG_LOG_INFO("%s: Event recieve，flag = %d \r\n", __func__, event_flag);

        if ( event_flag & LINK_EVT_WIFI_AVAIL ) {
            wifi_ok = 0;
            BLECFG_LOG_INFO("%s: Wifi connecting...\r\n", __func__);
            ret_ble = BleCfg_wifi_connect();
            if ( ret_ble != BLECFG_SUCCESS ) {
                continue;
            }
            BLECFG_LOG_INFO("%s: Wifi connect success.\r\n", __func__);
        }

        if ( event_flag & LINK_EVT_WIFI_CONNECTED ) {
            wifi_ok = 1;
            BLECFG_LOG_INFO("%s: Wifi connected.\r\n", __func__);
        }

        if ( event_flag & LINK_EVT_LINKSDK_AVAIL ) {
            BLECFG_LOG_INFO("%s: Dev info available.\r\n", __func__);
            if ( wifi_ok == 1 && linksdk_ok != 1 ) {
                BLECFG_LOG_INFO("%s: Link to cloud...\r\n", __func__);
                linksdk_ok = 0;
                BleCfg_dev_connect();
            }
        }

        if ( event_flag & LINK_EVT_LINKSDK_CONNECTED ) {
            linksdk_ok = 1;
            BLECFG_LOG_INFO("%s: Link to cloud success.\r\n", __func__);
        }
    }
}

/*
解析APP小程序下发的命令，目前命令格式如下
1. 配网命令：  标识位（2B）FFA0 + SSID len（1B）+ pwd len（1B）+ SSID str + pwd str，没有str结束符
2. 三元组设置命令：  标识位（2B）FFB0 + PK len（1B）+ DN len（1B）+ DS len（1B）+ PK str + DN str + DS str，没有str结束符
3. WIFI热点查询命令：  标识位（2B）FFC0
4. WIFI热点查询结果返回：  标识位（2B）FFD0
*/
static void BleCfg_cmd_parse(uint8_t *buf, int32_t len)
{
    enum {
        PKT_TYPE_NETCFG,        /* 配网命令的首个packet */
        PKT_TYPE_DEVINFO,       /* 配置设备三元组命令的首个packet */
        PKT_TYPE_WIFIINFO,      /* 设备扫描热点信息并上报的命令 */
        PKT_TYPE_FOLLOWED       /* 命令的后续packet，即需要多次buf合并成一个完整命令 */
    } pkt_type;
    static uint8_t *cmd_merge = NULL;
    static int32_t recv_len = 0;   /* 已经接收到的命令长度 */
    static int32_t total_len = 0;  /* 完整命令的长度 */
    int8_t ssid_len, pwd_len;
    int8_t pk_len, dn_len, ds_len;
    int32_t ret, loop, ap_len, ap_offset, payload_len;
    char   *ap_buf, *ssid, *passwd, *pk, *dn, *ds;
    char    ble_note[BLE_UP_MTU + 1];
    netmgr_wifi_ap_list_t *ap_info;

    /*
    BLECFG_LOG_INFO("%s payload: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
        __func__, buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7], buf[8], buf[9]);
    BLECFG_LOG_INFO("%s payload: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
        __func__, buf[10], buf[11], buf[12], buf[13], buf[14], buf[15], buf[16], buf[17], buf[18], buf[19]);
    */

    /* 1. 一条命令可能分拆成多个ble包，所以先进行包合并 */
    /* 1.1 判断包类型。
       由于包内容都是字符串不会出现0xff。所以首字节0xff表示多个包组成命令的第一个包，非0xff表示后续包 */
    if ( buf[0] == 0xff && buf[1] == 0xa0 ) {
        pkt_type = PKT_TYPE_NETCFG;
    }
    else if( buf[0] == 0xff && buf[1] == 0xb0 ) {
        pkt_type = PKT_TYPE_DEVINFO;
    }
    else if( buf[0] == 0xff && buf[1] == 0xc0 ) {
        pkt_type = PKT_TYPE_WIFIINFO;
    }
    else if(cmd_merge != NULL) {
        pkt_type = PKT_TYPE_FOLLOWED;
    }
    else {
        BLECFG_LOG_ERROR("%s: NetConfig not config data recv\r\n", __func__);
        return;
    }

    /* 1.2 判断是否为命令的第一个包，可能需要包合并，申请相关内存 */
    if ( pkt_type != PKT_TYPE_FOLLOWED ) {
        if ( cmd_merge != NULL ) {
            /* 发生丢包了，抛弃前一次buffer */
            BLECFG_LOG_ERROR("%s: NetConfig packet loss\r\n", __func__);
            free(cmd_merge);
            cmd_merge = NULL;
        }

        /* 申请buf_merge */
        if ( pkt_type == PKT_TYPE_NETCFG ) {
            /* APP下发的配网命令：  标识位（2B）FFA0 + SSID len（1B）+ pwd len（1B）+ SSID str + pwd str，没有str结束符 */
            total_len = buf[2] + buf[3] + 4;
        } else if ( pkt_type == PKT_TYPE_DEVINFO ) {
            /* APP下发的三元组设置命令：  标识位（2B）FFA0 + PK len（1B）+ DN len（1B）+ DS len（1B）+ PK str + DN str + DS str，没有str结束符 */
            total_len = buf[2] + buf[3] + buf[4] + 5;
        } else {
            /* 不需要merge流程 */
            total_len = len;
        }
        cmd_merge = malloc(total_len);
        if ( cmd_merge == NULL ) {
            BLECFG_LOG_ERROR("%s: NetConfig malloc fail for cmd_merge\r\n", __func__);
            return;
        }
        recv_len = 0;
    }

    /* 1.3 缓存包内容，本次buf copy写入 cmd_merge */
    if ( len > total_len - recv_len) {
        /* 发生丢包或错包，抛弃前一次buffer */
        BLECFG_LOG_ERROR("%s: NetConfig packet error\r\n", __func__);
        free(cmd_merge);
        cmd_merge = NULL;
        return;
    }
    memcpy(cmd_merge + recv_len, buf, len);
    recv_len += len;
    if (recv_len < total_len) {
        /* 包不完整，等待后续重新进入本流程 */
        BLECFG_LOG_INFO("%s: Waiting for next packet\r\n", __func__);
        return;
    }

    /* 2. 命令已经完整接收，进入后续命令处理流程 */
    if ( cmd_merge[0] == 0xff && cmd_merge[1] == 0xa0 ) {
        /* 2.1 配网命令处理流程 */
        /* APP下发的配网命令：  标识位（2B）FFA0 + SSID len（1B）+ pwd len（1B）+ SSID str + pwd str，没有str结束符 */
        ssid_len = cmd_merge[2];
        pwd_len  = cmd_merge[3];

        ssid = malloc(ssid_len + pwd_len + 2);
        if ( ssid == NULL ) {
            BLECFG_LOG_ERROR("%s: NetConfig malloc fail for ssid\r\n", __func__);
            free(cmd_merge);
            cmd_merge = NULL;
        }
        passwd = ssid + ssid_len + 1;

        memcpy(ssid, cmd_merge + 4, ssid_len);
        ssid[ssid_len] = 0;

        memcpy(passwd, cmd_merge + 4 + ssid_len, pwd_len);
        passwd[pwd_len] = 0;

        ret = BleCfg_wifi_set(ssid, passwd);
        if ( ret != BLECFG_SUCCESS ) {
            BLECFG_LOG_ERROR("%s: NetConfig wifi set error\r\n", __func__);
        }

        aos_event_set(&g_blecfg_event, LINK_EVT_WIFI_AVAIL, AOS_EVENT_OR);

        /* 消息接收完毕 */
        BLECFG_LOG_INFO("%s: NetConf SSID = %s, PWD = %s\r\n", __func__, ssid, passwd);
        free(ssid);
    } else if ( cmd_merge[0] == 0xff && cmd_merge[1] == 0xb0 ) {
        /* 2.2 三元组设置命令处理流程 */
        /* APP下发的三元组设置命令：  标识位（2B）FFB0 + PK len（1B）+ DN len（1B）+ DS len（1B）+ PK str + DN str + DS str，没有str结束符 */
        pk_len = cmd_merge[2];
        dn_len = cmd_merge[3];
        ds_len = cmd_merge[4];

        pk = malloc(pk_len + dn_len + ds_len + 3);
        if ( pk == NULL ) {
            BLECFG_LOG_ERROR("%s: NetConfig malloc fail for pk\r\n", __func__);
            free(cmd_merge);
            cmd_merge = NULL;
        }
        dn = pk + pk_len + 1;
        ds = dn + dn_len + 1;

        memcpy(pk, cmd_merge + 5, pk_len);
        pk[pk_len] = 0;

        memcpy(dn, cmd_merge + 5 + pk_len, dn_len);
        dn[dn_len] = 0;

        memcpy(ds, cmd_merge + 5 + pk_len + dn_len, ds_len);
        ds[ds_len] = 0;

        ret = BleCfg_dev_set(pk, dn, ds);
        if ( ret != BLECFG_SUCCESS ) {
            BLECFG_LOG_ERROR("%s: NetConfig wifi set error\r\n", __func__);
        }

        aos_event_set(&g_blecfg_event, LINK_EVT_LINKSDK_AVAIL, AOS_EVENT_OR);

        /* 消息接收完毕 */
        BLECFG_LOG_INFO("%s: pk = %d, dn = %d, ds = %d\r\n", __func__, pk, dn, ds);
        free(pk);
    } else if ( cmd_merge[0] == 0xff && cmd_merge[1] == 0xc0 ) {
        /* 2.3 WIFI热点查询命令处理流程 */
        ap_info = malloc(AP_INFO_MAX*sizeof(*ap_info));
        if ( ap_info == NULL ) {
            BLECFG_LOG_ERROR("%s: Not enough memory, %d\r\n", __func__, AP_INFO_MAX*sizeof(*ap_info));
            free(cmd_merge);
            cmd_merge = NULL;
            return;
        }
        memset(ap_info, 0, AP_INFO_MAX*sizeof(*ap_info));

        ret = netmgr_wifi_scan_result(ap_info, AP_INFO_MAX, NETMGR_WIFI_SCAN_TYPE_FULL_MERGE);

        for (loop = 0, ap_len = 0; loop < AP_INFO_MAX; loop++) {
            if ( ap_info[loop].ssid == NULL || ap_info[loop].ssid[0] == 0 ) {
                continue;
            }
            BLECFG_LOG_INFO("%s: ssid=%s ap_power=%d\r\n", __func__, ap_info[loop].ssid, ap_info[loop].ap_power);
            ap_len += strlen(ap_info[loop].ssid) + 2; //+2 for '()'
        }
        ap_len += 1; //str end
        BLECFG_LOG_INFO("%s: len of SSIDs is %d\r\n", __func__, ap_len);

        ap_buf = malloc(ap_len);
        if ( ap_buf == NULL ) {
            BLECFG_LOG_ERROR("%s: Not enough memory, %d\r\n", __func__, ap_len);
            free(cmd_merge);
            cmd_merge = NULL;
            free(ap_info);
            return;
        }

        for (loop = 0, ap_offset = 0; loop < AP_INFO_MAX; loop++) {
            if ( ap_info[loop].ssid == NULL || ap_info[loop].ssid[0] == 0 ) {
                continue;
            }
            ap_buf[ap_offset++] = '(';
            memcpy(&ap_buf[ap_offset], ap_info[loop].ssid, strlen(ap_info[loop].ssid));
            ap_offset += strlen(ap_info[loop].ssid);
            ap_buf[ap_offset++] = ')';
        }
        ap_buf[ap_offset] = 0;
        BLECFG_LOG_INFO("%s: SSIDs Total = '%s'\r\n", __func__, ap_buf);

        for (loop = 0, ap_offset = 0; loop < 16; loop++) {
            memset(ble_note, 0, sizeof(ble_note));
            ble_note[0] = 0xff;
            ble_note[1] = 0xd0 + loop;

            payload_len = ap_len - ap_offset > BLE_UP_MTU - 2 ? BLE_UP_MTU - 2 : ap_len - ap_offset;
            if (payload_len == 0 || loop == 15) {
                BleCfg_notificate(ble_note, 2);
                BLECFG_LOG_INFO("%s: SSIDs End = 0x%x%x\r\n", __func__, ble_note[0], ble_note[1]);
                break;
            }
            memcpy(&ble_note[2], &ap_buf[ap_offset], payload_len);
            ap_offset += payload_len;
            BleCfg_notificate(ble_note, BLE_UP_MTU);
            BLECFG_LOG_INFO("%s: SSIDs in one MTU = 0x%x%x'%s'\r\n", __func__, ble_note[0], ble_note[1], &ble_note[2]);
        }

        /* 消息接收完毕 */
        free(ap_info);
        free(ap_buf);
    }

    /* 3. 命令交互完毕，释放命令缓存 */
    free(cmd_merge);
    cmd_merge = NULL;
}

static void BleCfg_dhcp_success()
{
    aos_status_t ret;
    aos_task_t task_main;
    uint8_t res = BLECFG_DHCP_SUCCESS;
    char    ipaddr[64] = {'0'};

    BLECFG_LOG_INFO("Got IP\r\n");
    BleCfg_notificate(&res, 1);

    netmgr_wifi_get_ip_addr(ipaddr);
    BleCfg_notificate(ipaddr, strlen(ipaddr));

    /* 跳过ping步骤 */
    res = BLECFG_PING_SUCCESS;
    BleCfg_notificate(&res, 1);

    /* 保存wifi信息 */
    BleCfg_wifi_save();

    aos_msleep(500);

    /* ble断连，协议栈自动回到adv广播模式 */
    //BleCfg_stack_disconnect();

    aos_event_set(&g_blecfg_event, LINK_EVT_WIFI_CONNECTED, AOS_EVENT_OR);

}

static void BleCfg_link_success()
{
    BLECFG_LOG_INFO("LINK SUCCESS\r\n");

    /* 保存三元组信息 */
    BleCfg_dev_save();

    BleCfg_notificate("DEVSETOK", sizeof("DEVSETOK"));

    aos_event_set(&g_blecfg_event, LINK_EVT_LINKSDK_CONNECTED, AOS_EVENT_OR);

}


/* 启动ble配置，等待用户通过小程序输入（可能是配网，也可能是配置三元组） */
BLECFG_STATE BleCfg_run()
{
    BLECFG_STATE ret_ble;
    aos_status_t ret_aos;

    /* ble stack init */
    ret_ble = BleCfg_stack_init(BleCfg_cmd_parse);
    if (ret_ble) {
        BLECFG_LOG_ERROR("%s: Ble config stack init error %d\r\n", __func__, ret_ble);
        return ret_ble;
    }

    /* netmgr init */
    ret_ble = BleCfg_netmgr_init(BleCfg_dhcp_success);
    if (ret_ble) {
        BLECFG_LOG_ERROR("%s: Ble config netmgr init error %d\r\n", __func__, ret_ble);
        return ret_ble;
    }

    /* linksdk init */
    ret_ble = BleCfg_linksdk_init(BleCfg_link_success);
    if (ret_ble) {
        BLECFG_LOG_ERROR("%s: Ble config linksdk init error %d\r\n", __func__, ret_ble);
        return ret_ble;
    }

    ret_aos = aos_event_new(&g_blecfg_event, 0);
    if(ret_aos != 0) {
        BLECFG_LOG_ERROR("%s: event new failed, err=%d\r\n", __func__, ret_aos);
        return -1;
    }

    /* netmgr init */
    ret_aos = aos_task_new("blecfg_task", BleCfg_main_task, NULL, 6*1024);
    if(ret_aos != 0) {
        BLECFG_LOG_ERROR("%s: blecfg task new failed, err=%d\r\n", __func__, ret_aos);
        return -1;
    }

    ret_ble = BleCfg_stack_start();
    if (ret_ble) {
        BLECFG_LOG_ERROR("%s: Ble config start error %d\r\n", __func__, ret_ble);
        return ret_ble;
    }

    return BLECFG_SUCCESS;
}

/* 恢复上一次成功连接WiFi的ssid与password配置，若存在则直接完成配网，若不存在则报错 */
BLECFG_STATE BleCfg_recovery_wifi()
{
    BLECFG_STATE ret;

    ret = BleCfg_wifi_restore();
    if (ret == BLECFG_SUCCESS) {
        BLECFG_LOG_INFO("%s: Wifi info get from kv\r\n", __func__);
        aos_event_set(&g_blecfg_event, LINK_EVT_WIFI_AVAIL, AOS_EVENT_OR);
    }

    return ret;
}

/* 恢复上一次成功连接云端的三元组信息，若存在则直接连云，若不存在则报错 */
BLECFG_STATE BleCfg_recovery_devinfo()
{
    BLECFG_STATE ret;

    ret = BleCfg_dev_restore();
    if (ret == BLECFG_SUCCESS) {
        BLECFG_LOG_INFO("%s: Device information get from kv\r\n", __func__);
        aos_event_set(&g_blecfg_event, LINK_EVT_LINKSDK_AVAIL, AOS_EVENT_OR);
    }

    return ret;
}


