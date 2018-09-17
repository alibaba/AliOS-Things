/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "aos/aos.h"
#include "os.h"
#include "crc.h"
#include "alink_protocol.h"
#include "alink_export_internal.h"
#include "devmgr.h"

#ifdef ROUTER_SDK
#include "devmgr_router.h"
#endif

#ifdef GATEWAY_SDK
#include "devmgr_cache.h"
#include "devmgr_alink.h"
/* #include "stdd_datatype.h" */
#include "kvmgr.h"
/* #include "attrs_profile_mgr.h" */
/* #include "stdd.h" */
#endif

#define MODULE_NAME             MODULE_NAME_DEVMGR
#define DEVMGR_MAX_SUBDEVICE    128
#define IEEE_ADDR_STRING_FMT    "%02x%02x%02x%02x%02x%02x%02x%02x"
#define DEVICE_KEY_PREFIX       "device_%02x"
#define DEVBASE_LEN              ((long)(&((dev_base_t  *)0)->crc))

/*子设备序号bit map*/
static uint8_t devidx_bit_map[DEVMGR_MAX_SUBDEVICE / 8 + 1] = {0};
/*待删除设备链表*/
static AOS_DLIST_HEAD(delele_dev_head);
/*有效设备链表*/
AOS_DLIST_HEAD(dev_head);
/*未知设备链表*/
AOS_DLIST_HEAD(unknown_dev_head);

void *devlist_lock = NULL;

#if (defined(GATEWAY_SDK) || defined (ROUTER_SDK))  //public functions for router and gateway
static void __hold_devinfo_ref(dev_info_t *devinfo)
{
    devinfo->ref_cnt++;
}

static void __free_devinfo(dev_info_t *devinfo)
{
    LOGW(MODULE_NAME_DEVMGR, "__free_devinfo, devid:%s", devinfo->dev_base.dev_id);

#ifdef GATEWAY_SDK
    //释放属性缓存链表
    devmgr_free_device_cache(devinfo);  //for gateway only
#endif
    //从delete_dev_head链表中删除设备节点
    if (!dlist_empty(&devinfo->list_node)) {
        dlist_del(&devinfo->list_node);
    }

    if (devinfo->dev_mutex) {
        os_mutex_destroy(devinfo->dev_mutex);
        devinfo->dev_mutex = NULL;
    }

    os_free(devinfo);
}

/*释放devinfo对象引用计数*/
void devmgr_put_devinfo_ref(dev_info_t *devinfo)
{
    os_mutex_lock(devlist_lock);
    devinfo->ref_cnt--;

    /*引用计数为0，释放节点*/
    if (devinfo->ref_cnt == 0) {
        __free_devinfo(devinfo);
    }

    os_mutex_unlock(devlist_lock);
    return;
}

// modify by wukong 2017-4-17 devid_or_uuid --> devid_or_uuid_or_mac
static dev_info_t *__get_devinfo(const char *devid_or_uuid_or_mac)
{
    dev_info_t *pos, *devinfo = NULL;

    dlist_for_each_entry(&dev_head, pos, dev_info_t, list_node) {
        if (strcmp(pos->dev_base.dev_id, devid_or_uuid_or_mac) == 0 ||
            strcmp(pos->dev_base.uuid, devid_or_uuid_or_mac) == 0 ||
            strcmp(pos->dev_base.u.ether_mac, devid_or_uuid_or_mac) == 0
           ) {
            devinfo = pos;
            break;
        }
    }

    return devinfo;
}
// modify by wukong 2017-4-17 end

static dev_info_t *devmgr_new_devinfo()
{
    dev_info_t *devinfo = NULL;

    devinfo = os_malloc(sizeof(dev_info_t));
    if (devinfo == NULL) {
        return NULL;
    }

    memset(devinfo, 0, sizeof(dev_info_t));

    //链表节点初始化，锁初始化
    dlist_init(&devinfo->attr_head);
    dlist_init(&devinfo->list_node);
    devinfo->device_idx = 0xff;
    devinfo->cloud_state = DEVICE_STATE_INITIAL;
    devinfo->link_state = LINK_STATE_OFFLINE;
    devinfo->ref_cnt = 1;
    devinfo->dev_mutex = os_mutex_init();

    return devinfo;
}

// modify by wukong 2017-4-17
dev_info_t *devmgr_get_devinfo(const char *devid_or_uuid_or_mac)
{
    os_mutex_lock(devlist_lock);
    dev_info_t *devinfo = __get_devinfo(devid_or_uuid_or_mac);

    if (devinfo) {
        __hold_devinfo_ref(devinfo);
    }
    os_mutex_unlock(devlist_lock);

    return devinfo;
}

int get_mac_by_uuid(const char *uuid, char *mac)
{
    dev_info_t *devinfo = devmgr_get_devinfo(uuid);
    PTR_RETURN(devinfo, SERVICE_RESULT_ERR, "device not found, uuid:%s", uuid);

    memcpy(mac, devinfo->dev_base.u.ether_mac, ETHER_ADDR_BYTES);

    devmgr_put_devinfo_ref(devinfo);

    return 0;
}

int get_uuid_by_mac(const char *mac, char *uuid)
{
    dev_info_t *devinfo = devmgr_get_devinfo(mac);
    PTR_RETURN(devinfo, SERVICE_RESULT_ERR, "device not found, mac:%s", mac);

    memcpy(uuid, devinfo->dev_base.uuid, STR_UUID_LEN + 1);

    devmgr_put_devinfo_ref(devinfo);

    return 0;
}
// add by wukong  end 2017-4-17

int devmgr_get_dev_type(const char *devid_or_uuid, uint8_t *dev_type)
{
    int ret = SERVICE_RESULT_ERR;

    os_mutex_lock(devlist_lock);
    dev_info_t *devinfo = __get_devinfo(devid_or_uuid);
    if (devinfo) {
        *dev_type = devinfo->dev_base.dev_type;
        ret = SERVICE_RESULT_OK;
    }
    os_mutex_unlock(devlist_lock);

    return ret;
}

static void __release_devidx(uint8_t devidx)
{
    uint8_t n = devidx / 8;
    uint8_t m = devidx % 8;

    if (devidx_bit_map[n] & (0x01 << m)) {
        devidx_bit_map[n] &= ~(0x01 << m);
    }
}

static int __new_devidx(uint8_t *devidx)
{
    uint8_t n, m, index = 0;

    while (index < DEVMGR_MAX_SUBDEVICE) {
        n = index / 8;
        m = index % 8;

        if (!(devidx_bit_map[n] & (0x01 << m))) {
            devidx_bit_map[n] |= (0x01 << m);
            break;
        }
        index++;
    }

    if (index == DEVMGR_MAX_SUBDEVICE) {
        return SERVICE_RESULT_ERR;
    }

    *devidx = index;
    return SERVICE_RESULT_OK;
}

static void devmgr_hold_devidx(uint8_t devidx)
{
    uint8_t n = devidx / 8;
    uint8_t m = devidx % 8;

    if (!(devidx_bit_map[n] & (0x01 << m))) {
        devidx_bit_map[n] |= (0x01 << m);
    }
}

#endif

#ifdef GATEWAY_SDK
void __dump_devbase(dev_base_t *dev_base)
{
    char ieee_addr[32] = {0};
    char rand_hexstr[33] = {0};
    get_ieeeaddr_string_by_extaddr(dev_base->u.ieee_addr, ieee_addr,
                                   sizeof(ieee_addr));

    bytes_2_hexstr(dev_base->rand, sizeof(dev_base->rand), rand_hexstr,
                   sizeof(rand_hexstr));

    os_printf("\tdevice base:\n");
    os_printf("\t\tdev_type: %d, model_id: 0x%08x, rand_hex: %s, sign: %s, \n"
              "\t\tdev_id: %s, uuid: %s, ieee_addr:%s,\n"
              "\t\ttoken: %s",
              dev_base->dev_type, dev_base->model_id, rand_hexstr,
              dev_base->sign, dev_base->dev_id, dev_base->uuid, ieee_addr,
              dev_base->token);
}

void __dump_devinfo(dev_info_t *devinfo)
{
    os_printf("device info:\n");
    __dump_devbase(&devinfo->dev_base);
    os_printf("\tdevice state:\n\t\tdev_idx: %02x, cloud_state: %02x, link_state: %02x, ref_cnt: %02x\n",
              devinfo->device_idx, devinfo->cloud_state, devinfo->link_state,
              devinfo->ref_cnt);
    __dump_attr_cache(&devinfo->attr_head);
}


void __dump_device_list()
{
    dev_info_t *pos;
    dlist_for_each_entry(&dev_head, pos, dev_info_t, list_node) {
        __dump_devinfo(pos);
    }
}

static void __get_devkey_name(uint8_t dev_index, char *key_buf, int buf_len)
{
    snprintf(key_buf, buf_len - 1, DEVICE_KEY_PREFIX, dev_index);
}

static void __remove_devinfo(dev_info_t *devinfo)
{
    LOGW(MODULE_NAME_DEVMGR, "__clean device, devid:%s, devidx:%02x, ref:%d",
         devinfo->dev_base.dev_id,
         devinfo->device_idx, devinfo->ref_cnt);

    devinfo->device_idx = 0xff;
    //5.释放节点
    dlist_del(&devinfo->list_node);
    dlist_add(&devinfo->list_node, &delele_dev_head);
    devinfo->ref_cnt--;

    /*引用计数为0，释放节点*/
    if (devinfo->ref_cnt == 0) {
        __free_devinfo(devinfo);
    }
}

static void __delete_devinfo(dev_info_t *devinfo)
{
    char key[16] = {0};

    //4.释放dev idx
    if (devinfo->device_idx != 0xff) {
        __get_devkey_name(devinfo->device_idx, key, sizeof(key));
        /* remove_kv(key); */
        __release_devidx(devinfo->device_idx);
    }

    __remove_devinfo(devinfo);
}

static dev_info_t *__get_devinfo_by_ieeeaddr(char ieee_addr[8])
{
    dev_info_t *pos, *devinfo = NULL;

    dlist_for_each_entry(&dev_head, pos, dev_info_t, list_node) {
        //add by wukong 2017-4-17
        if (pos->dev_base.dev_type != DEV_TYPE_ZIGBEE) {
            continue;
        }
        //add by wukong end 2017-4-17

        if (memcmp(pos->dev_base.u.ieee_addr, ieee_addr,
                   sizeof(pos->dev_base.u.ieee_addr)) == 0) {
            devinfo = pos;
            break;
        }
    }

    return devinfo;
}

static dev_info_t *__get_unknown_devinfo(const char *devid_or_uuid)
{
    dev_info_t *pos, *devinfo = NULL;

    dlist_for_each_entry(&unknown_dev_head, pos, dev_info_t, list_node) {
        if (strcmp(pos->dev_base.dev_id, devid_or_uuid) == 0 ||
            strcmp(pos->dev_base.uuid, devid_or_uuid) == 0) {
            devinfo = pos;
            break;
        }
    }

    return devinfo;
}

static int devmgr_read_devbase(uint8_t dexidx, dev_base_t *dev_base)
{
    int ret = SERVICE_RESULT_ERR;
    char key[16] = {0};
    int len = sizeof(dev_base_t);
    uint16_t crc = 0;

    __get_devkey_name(dexidx, key, sizeof(key));
    //ret = get_kv(key, (char *)dev_base, &len);
    //if(ret != SERVICE_RESULT_OK){
    //    //LOGW(MODULE_NAME_DEVMGR, "read devbase fail, key:%s", key);
    //    return ret;
    //}

    //crc校验
    crc = utils_crc16((uint8_t *)dev_base, DEVBASE_LEN);
    if (crc != dev_base->crc) {
        LOGE(MODULE_NAME_DEVMGR, "check crc fail, key:%s, crc:%04x, new_crc:%04x", key,
             dev_base->crc, crc);
        return SERVICE_RESULT_ERR;
    }

    return ret;
}

//保存设备信息到flash
static int devmgr_save_devinfo(dev_info_t *devinfo)
{
    char key[16] = {0};

    if (devinfo->device_idx == 0xff) {
        LOGE(MODULE_NAME_DEVMGR, "invalid device index:0x%02x", devinfo->device_idx);
        return SERVICE_RESULT_ERR;
    }

    __get_devkey_name(devinfo->device_idx, key, sizeof(key));

    //计算crc值
    devinfo->dev_base.crc = utils_crc16((uint8_t *)(&devinfo->dev_base),
                                        DEVBASE_LEN);
    int ret = 0;
    //ret = set_kv_in_flash(key, &devinfo->dev_base, sizeof(devinfo->dev_base), 1);
    //if(ret != SERVICE_RESULT_OK)
    //    LOGE(MODULE_NAME_DEVMGR, "save devbase fail, key:%s, crc:%04x", key, devinfo->dev_base.crc);

    return ret;
}

static void devmgr_delete_devinfo(dev_info_t *devinfo)
{
    LOGW(MODULE_NAME_DEVMGR, "delete device, devid:%s", devinfo->dev_base.dev_id);
    os_mutex_lock(devlist_lock);
    __delete_devinfo(devinfo);
    os_mutex_unlock(devlist_lock);
}

dev_info_t *devmgr_get_devinfo_by_ieeeaddr(unsigned char ieee_addr[8])
{
    os_mutex_lock(devlist_lock);
    dev_info_t *devinfo = __get_devinfo_by_ieeeaddr(ieee_addr);

    if (devinfo) {
        __hold_devinfo_ref(devinfo);
    }
    os_mutex_unlock(devlist_lock);

    return devinfo;
}

//从flash中load设备信息
static int devmgr_load_devinfo()
{
    int ret = SERVICE_RESULT_ERR;
    uint8_t devidx = 0;
    dev_base_t dev_base;

    //遍历读取设备信息
    for (devidx = 0; devidx < DEVMGR_MAX_SUBDEVICE; devidx++) {
        ret = devmgr_read_devbase(devidx, &dev_base);
        if (ret != SERVICE_RESULT_OK) {
            continue;
        }

        dev_info_t *devinfo = devmgr_new_devinfo();
        if (devinfo == NULL) {
            continue;
        }

        memcpy(&devinfo->dev_base, &dev_base, sizeof(devinfo->dev_base));
        devmgr_hold_devidx(devidx);

        //有uuid表示已经注册
        if (devinfo->dev_base.uuid[0] != '\0') {
            devinfo->cloud_state = DEVICE_STATE_REGISTERED;
        } else {
            devinfo->cloud_state = DEVICE_STATE_AUTHORISED;
        }

        devinfo->device_idx = devidx;

        os_mutex_lock(devlist_lock);
        dlist_add_tail(&devinfo->list_node, &dev_head);
        os_mutex_unlock(devlist_lock);
    }

    return SERVICE_RESULT_OK;
}

dev_info_t *devmgr_get_unknown_devinfo(const char *devid_or_uuid)
{
    dev_info_t *devinfo = NULL;

    os_mutex_lock(devlist_lock);
    devinfo = __get_unknown_devinfo(devid_or_uuid);
    if (devinfo) {
        __hold_devinfo_ref(devinfo);
    }
    os_mutex_unlock(devlist_lock);

    return devinfo;
}

void devmgr_remove_device(char ieee_addr[8])
{
    int ret = SERVICE_RESULT_ERR;

    //从zigbee协议栈中移除设备
    if (alink_cb_func[_ALINK_ZIGBEE_REMOVE_DEVICE]) {
        ret = ((zigbee_remove_device_cb_t)alink_cb_func[_ALINK_ZIGBEE_REMOVE_DEVICE])(
                  ieee_addr);
        RET_LOG(ret, "zigbee remove fail");
    } else {
        LOGE(MODULE_NAME_DEVMGR, "remove_device callback is NULL");
    }

    return ;
}

void devmgr_free_unknown_devlist()
{
    dev_info_t *pos;
    dlist_t *next = NULL;

    LOGI(MODULE_NAME_DEVMGR, "free unknown devices");

    os_mutex_lock(devlist_lock);
    dlist_for_each_entry_safe(&unknown_dev_head, next, pos, dev_info_t, list_node) {
        //从zigbee协议栈中移除设备
        devmgr_remove_device(pos->dev_base.u.ieee_addr);

        __remove_devinfo(pos);
    }
    os_mutex_unlock(devlist_lock);
}

int devmgr_delete_device(const char *devid)
{
    int ret = SERVICE_RESULT_ERR;

    LOGI(MODULE_NAME_DEVMGR, "remove device, devid:%s", devid);

    //查找未知设备列表
    dev_info_t *devinfo = devmgr_get_unknown_devinfo(devid);
    if (devinfo) {
        //从zigbee协议栈中移除设备
        devmgr_remove_device(devinfo->dev_base.u.ieee_addr);

        devmgr_delete_devinfo(devinfo);
        devmgr_put_devinfo_ref(devinfo);
    }

    //查找已知设备列表
    devinfo = devmgr_get_devinfo(devid);
    if (devinfo) {
        //从zigbee协议栈中移除设备
        devmgr_remove_device(devinfo->dev_base.u.ieee_addr);

        //设备解绑
        ret = devmgr_network_leave_event_handler(devinfo);
        RET_LOG(ret, CALL_FUCTION_FAILED, "devmgr_network_leave_event_handler");

        //删除节点
        devmgr_delete_devinfo(devinfo);
        devmgr_put_devinfo_ref(devinfo);
    }

    return SERVICE_RESULT_OK;
}

int devmgr_delete_device_by_devinfo(dev_info_t *devinfo)
{
    int ret = SERVICE_RESULT_ERR;

    if (NULL == devinfo) {
        LOGE(MODULE_NAME_DEVMGR, "Invalid parameter!");
        return SERVICE_RESULT_ERR;
    }
    /*remove the device from zigbee protocol*/
    devmgr_remove_device(devinfo->dev_base.u.ieee_addr);

    ret = devmgr_network_leave_event_handler(devinfo);
    RET_LOG(ret, CALL_FUCTION_FAILED, "devmgr_network_leave_event_handler");

    /*The devlist is locked on function devmgr_network_event_cb*/
    __delete_devinfo(devinfo);

    return ret;
}

int devmgr_authorise_device(char *devid)
{
    int ret = SERVICE_RESULT_OK;

    //查找设备是否已经存在
    dev_info_t *devinfo = devmgr_get_devinfo(devid);
    if (devinfo) {
        devmgr_put_devinfo_ref(devinfo);
        return SERVICE_RESULT_OK;
    }

    //查找未知设备
    devinfo = devmgr_get_unknown_devinfo(devid);
    PTR_RETURN(devinfo, SERVICE_RESULT_ERR, "the device does not exist, devid:%s",
               devid);

    devinfo->cloud_state = DEVICE_STATE_AUTHORISED;
    //2.云端注册,根据注册结果设置云端链接状态,认证失败直接剔除设备直接退出
    ret = devmgr_network_up_event_handler(devinfo);
    //if(ALINK_CODE_ERROR_SIGN == ret)
    //devmgr_network_up_event_handler 只会返回SERVICE_RESULT_OK 或者SERVICE_RESULT_ERR
    if (SERVICE_RESULT_OK != ret) {
        LOGE(MODULE_NAME_DEVMGR, "register subdevice error!");
        goto err;
    }

    ret = __new_devidx(&devinfo->device_idx);
    if (ret < 0) { //设备列表达到上限
        goto err;
    }

    os_mutex_lock(devlist_lock);
    dlist_del(&devinfo->list_node);
    dlist_add_tail(&devinfo->list_node, &dev_head);
    os_mutex_unlock(devlist_lock);

    //保存设备信息到flash
    devmgr_save_devinfo(devinfo);

    //获取device profile
    ret = devmgr_sync_device_profile(DEV_TYPE_ZIGBEE, devinfo->dev_base.model_id);
    if (SERVICE_RESULT_OK != ret) {
        LOGW(MODULE_NAME_DEVMGR, "sync device profile fail, model id:0x%08x",
             (unsigned int)devinfo->dev_base.model_id);
    }

    //释放引用计数
    devmgr_put_devinfo_ref(devinfo);

    return SERVICE_RESULT_OK;

err:
    devmgr_delete_devinfo(devinfo);
    devmgr_put_devinfo_ref(devinfo);

    return SERVICE_RESULT_ERR;
}


//app端扫码添加设备
int devmgr_add_authorise_device(const char *devid, uint16_t model_id,
                                uint8_t dev_type)
{
    int ret = SERVICE_RESULT_ERR;

    LOGI(MODULE_NAME_DEVMGR, "authorise device, devid:%s, model_id:%d", devid,
         model_id);

    //查找设备是否已经存在
    dev_info_t *devinfo = devmgr_get_devinfo(devid);
    if (devinfo) {
        devmgr_put_devinfo_ref(devinfo);
        return SERVICE_RESULT_ERR;
    }


    //直接添加到设备列表
    devinfo = devmgr_new_devinfo();
    if (devinfo == NULL) {
        return SERVICE_RESULT_ERR;
    }

    devinfo->dev_base.dev_type = dev_type;
    devinfo->dev_base.model_id = model_id;
    //devinfo->dev_base.u.ieee_addr
    //devinfo->dev_base.rand
    //devinfo->dev_base.sign
    devinfo->cloud_state = DEVICE_STATE_AUTHORISED;

    ret = __new_devidx(&devinfo->device_idx);
    RET_LOG(ret, CALL_FUCTION_FAILED, "__new_devidx")
    if (ret != SERVICE_RESULT_OK) {
        __free_devinfo(devinfo);
        return SERVICE_RESULT_ERR;
    }

    //添加到设备链表
    os_mutex_lock(devlist_lock);
    dlist_add_tail(&devinfo->list_node, &dev_head);
    os_mutex_unlock(devlist_lock);

    devmgr_save_devinfo(devinfo);

    return ret;
}

static void devmgr_free_authorise_devlist()
{
    dev_info_t *pos;
    dlist_t *next = NULL;

    LOGI(MODULE_NAME_DEVMGR, "free authorise devices");

    os_mutex_lock(devlist_lock);
    dlist_for_each_entry_safe(&dev_head, next, pos, dev_info_t, list_node) {
        __remove_devinfo(pos);
    }
    os_mutex_unlock(devlist_lock);
}

static void devmgr_delete_authorise_devlist()
{
    dev_info_t *pos;
    dlist_t *next = NULL;

    LOGI(MODULE_NAME_DEVMGR, "delete authorise devices");

    os_mutex_lock(devlist_lock);
    dlist_for_each_entry_safe(&dev_head, next, pos, dev_info_t, list_node) {
        //从zigbee协议栈中移除设备
        devmgr_remove_device(pos->dev_base.u.ieee_addr);

        //unregister device,网关复位服务端会完成所有子设备unregister操作
        //devmgr_network_leave_event_handler(pos);
        __delete_devinfo(pos);
    }
    os_mutex_unlock(devlist_lock);
}


/*获取全部设备model_id,提供给文件下载管理模块使用*/
int devmgr_get_all_device_modelid(uint32_t model_id[], int *num)
{
    uint16_t model_map = 0;
    dev_info_t *pos, *devinfo = NULL;
    int index = 0;

    os_mutex_lock(devlist_lock);
    dlist_for_each_entry(&dev_head, pos, dev_info_t, list_node) {
        /*只关注zigbee设备*/
        if (pos->dev_base.dev_type != DEV_TYPE_ZIGBEE) {
            continue;
        }

        int i = 0;
        int exist = 0;
        while (i < *num && i < index) {
            if (model_id[i++] == pos->dev_base.model_id) {
                exist = 1;
                break;
            }
        }
        if (exist) {
            continue;
        }

        model_id[index++] = pos->dev_base.model_id;

        //达到数组上限
        if (index == *num) {
            break;
        }
    }

    os_mutex_unlock(devlist_lock);
    *num = index;

    return SERVICE_RESULT_OK;
}

int devmgr_relogin_device(const char *devid)
{
    int ret = SERVICE_RESULT_OK;

    LOGI(MODULE_NAME_DEVMGR, "relogin device, devid:%s", devid);

    dev_info_t *devinfo = devmgr_get_devinfo(devid);
    PTR_RETURN(devinfo, SERVICE_RESULT_ERR, "device not found, devid:%s", devid);

    devinfo->link_state = LINK_STATE_ONLINE;
    devinfo->cloud_state = DEVICE_STATE_LOGOUTED;

    ret = devmgr_link_state_event_handler(devinfo, LINK_STATE_ONLINE);
    RET_LOG(ret, CALL_FUCTION_FAILED, "devmgr_link_state_event_handler");

    devmgr_put_devinfo_ref(devinfo);

    return ret;
}

int devmgr_update_device_online_status(const char *devid, link_state_t state)
{
    int ret = SERVICE_RESULT_OK;

    LOGI(MODULE_NAME_DEVMGR, "update device online status, devid:%s, state:%d",
         devid, state);

    dev_info_t *devinfo = devmgr_get_devinfo(devid);
    PTR_RETURN(devinfo, SERVICE_RESULT_ERR, "device not found, devid:%s", devid);

    ret = devmgr_link_state_event_handler(devinfo, state);
    RET_LOG(ret, CALL_FUCTION_FAILED, "devmgr_link_state_event_handler");

    devmgr_put_devinfo_ref(devinfo);

    return ret;
}

//app -> gw,扫码添加和删除设备
//payload:{"mac":"","shortModel":"","model":"","devType":"","name":""}
int devmgr_add_device(const char *devid, const char *payload)
{
    return 0;
}


int devmgr_update_zigbee_device_online_status(uint8_t
                                              ieee_addr[IEEE_ADDR_BYTES], uint8_t online)
{
    char *addr_ptr;
    char devid[17] = {0};

    addr_ptr = get_ieeeaddr_string_by_extaddr(ieee_addr, devid, sizeof(devid));
    PTR_RETURN(addr_ptr, SERVICE_RESULT_ERR, "invalid ieee address");

    return devmgr_update_device_online_status(devid, online);
}


//lmns依赖接口
//device -> gw,wifi设备上下线接口
//payload:{"uuid":"","mac":"","model":"","dev_type":"","name":""}
int devmgr_update_wifi_device_online_status(const char *devid, uint8_t online,
                                            const char *payload)
{
    return 0;
}


//同步device profile
int devmgr_sync_device_profile(uint8_t dev_type, unsigned int model_id)
{
    int ret = SERVICE_RESULT_OK;
    char file[128] = {0};

    LOGW(MODULE_NAME_DEVMGR, "devtype:%d, model id:0x%08x", dev_type,
         (unsigned int)model_id);
    //ret = get_device_profile_file(dev_type, model_id, file, sizeof(file));
    //if(ret == SERVICE_RESULT_OK && strlen(file))
    //    ret = stdd_add_device_profile(dev_type, model_id, file);

    return ret;
}


int devmgr_join_zigbee_device(unsigned char ieee_addr[IEEE_ADDR_BYTES],
                              uint32_t model_id, const char rand[SUBDEV_RAND_BYTES], const char *sign)
{
    int ret = SERVICE_RESULT_OK;
    uint32_t permitjoin_model_id = 0;
    permitjoin_model_id = devmgr_get_permitjoin_short_model();

    /*设置disable入网延后任务*/
    devmgr_delay_disable_join(DEVMGR_DEFAULT_PERMITJOIN_DURATION);

    if (permitjoin_model_id != DEVMGR_PERMITJOIN_ANY_MODEL &&
        permitjoin_model_id != model_id) {
        LOGE(MODULE_NAME_DEVMGR, "unkown model_id:0x%08x, permitjoin model_id:0x%08x",
             (unsigned int)model_id, (unsigned int)permitjoin_model_id);

        return SERVICE_RESULT_ERR;
    }

    //TODO:remove hardcode
    if (DEVMGR_PERMITJOIN_ANY_MODEL == permitjoin_model_id) {
        permitjoin_model_id = model_id;
    }

    char addr[20] = {0};
    get_ieeeaddr_string_by_extaddr(ieee_addr, addr, sizeof(addr));
    LOGW(MODULE_NAME_DEVMGR, "join device, ieee addr:%s, model id:0x%08x", addr,
         model_id);

    //1.判断设备是否已经存在
    dev_info_t *devinfo = devmgr_get_devinfo_by_ieeeaddr(ieee_addr);
    if (devinfo) {
        //未注册则再次发起注册流程，更新rand和sign
        devinfo->dev_base.uuid[0] = '\0';
        devinfo->cloud_state = DEVICE_STATE_AUTHORISED;
        devinfo->link_state = LINK_STATE_ONLINE;
        devinfo->dev_base.model_id = model_id;
        devinfo->dev_base.dev_type = DEV_TYPE_ZIGBEE;
        memcpy(devinfo->dev_base.rand, rand, sizeof(devinfo->dev_base.rand));
        strncpy(devinfo->dev_base.sign, sign, sizeof(devinfo->dev_base.sign));
        devinfo->dev_base.sign[STR_SIGN_LEN] = '\0';

        ret = devmgr_network_up_event_handler(devinfo);
        RET_LOG(ret, CALL_FUCTION_FAILED, "devmgr_network_up_event_handler");
        devmgr_put_devinfo_ref(devinfo);

        //保存设备信息到flash
        devmgr_save_devinfo(devinfo);
        return ret;
    }

    //信息填充
    devinfo = devmgr_new_devinfo();
    PTR_RETURN(devinfo, SERVICE_RESULT_ERR, "new device info fail");

    memcpy(devinfo->dev_base.u.ieee_addr, ieee_addr, 8);
    get_ieeeaddr_string_by_extaddr(ieee_addr, devinfo->dev_base.dev_id,
                                   sizeof(devinfo->dev_base.dev_id));
    devinfo->dev_base.model_id = model_id;
    memcpy(devinfo->dev_base.rand, rand, sizeof(devinfo->dev_base.rand));
    strncpy(devinfo->dev_base.sign, sign, sizeof(devinfo->dev_base.sign));
    devinfo->dev_base.sign[STR_SIGN_LEN] = '\0';
    devinfo->dev_base.dev_type = DEV_TYPE_ZIGBEE;

    //设置状态
    devinfo->link_state = LINK_STATE_ONLINE;
    devinfo->cloud_state = DEVICE_STATE_INITIAL;
    if (permitjoin_model_id == model_id) {
        ret = __new_devidx(&devinfo->device_idx);
        if (ret < 0) { //设备列表达到上限, 不允许添加设备
            devmgr_put_devinfo_ref(devinfo);
            LOGE(MODULE_NAME_DEVMGR, "new device index fail, devid:%s",
                 devinfo->dev_base.dev_id);
            return ret;
        }

        LOGI(MODULE_NAME_DEVMGR, "authorise device, devid:%s",
             devinfo->dev_base.dev_id);
        devinfo->cloud_state = DEVICE_STATE_AUTHORISED;
        ret = devmgr_network_up_event_handler(devinfo);
        RET_LOG(ret, CALL_FUCTION_FAILED, "devmgr_network_up_event_handler");
        if (SERVICE_RESULT_OK != ret) {
            __release_devidx(devinfo->device_idx);
            devmgr_put_devinfo_ref(devinfo);
            return ret;
        }

        //同步设备profile
        ret = devmgr_sync_device_profile(DEV_TYPE_ZIGBEE, model_id);
        if (SERVICE_RESULT_OK != ret) {
            LOGW(MODULE_NAME_DEVMGR, "sync device profile fail, model id:0x%08x",
                 (unsigned int)model_id);
        }

        //添加确认设备列表
        os_mutex_lock(devlist_lock);
        dlist_add_tail(&devinfo->list_node, &dev_head);
        os_mutex_unlock(devlist_lock);

        //4.保存设备信息到flash
        devmgr_save_devinfo(devinfo);
    } else {
        LOGI(MODULE_NAME_DEVMGR, "unknonw device, devid:%s", devinfo->dev_base.dev_id);
        //添加到未知设备列表
        os_mutex_lock(devlist_lock);
        dlist_add_tail(&devinfo->list_node, &unknown_dev_head);
        os_mutex_unlock(devlist_lock);
    }

    return SERVICE_RESULT_OK;
}


/*设备端主动离开网络*/
int devmgr_leave_zigbee_device(unsigned char ieee_addr[IEEE_ADDR_BYTES])
{
    int ret = SERVICE_RESULT_ERR;
    char addr_buff[17] = {0};
    char *addr_ptr = NULL;

    addr_ptr = get_ieeeaddr_string_by_extaddr(ieee_addr, addr_buff,
                                              sizeof(addr_buff));
    PTR_RETURN(addr_ptr, SERVICE_RESULT_ERR, "invalid ieee address");
    LOGI(MODULE_NAME_DEVMGR, "leave device, devid:%s", addr_buff);

    //查找未知设备列表
    dev_info_t *devinfo = devmgr_get_unknown_devinfo(addr_ptr);
    if (devinfo) {
        LOGI(MODULE_NAME_DEVMGR, "exist unknonw device, devid:%s",
             devinfo->dev_base.dev_id);
        devmgr_delete_devinfo(devinfo);
        devmgr_put_devinfo_ref(devinfo);
    }

    //查找已知设备列表
    devinfo = devmgr_get_devinfo(addr_ptr);
    if (devinfo) {
        LOGI(MODULE_NAME_DEVMGR, "exist authorise device, devid:%s",
             devinfo->dev_base.dev_id);
        //设备解绑
        ret = devmgr_network_leave_event_handler(devinfo);
        RET_LOG(ret, CALL_FUCTION_FAILED, "devmgr_network_leave_event_handler");

        //删除节点
        devmgr_delete_devinfo(devinfo);
        devmgr_put_devinfo_ref(devinfo);
    }

    return SERVICE_RESULT_OK;
}


void devmgr_reset_all_device(void)
{
    devmgr_free_unknown_devlist();
    devmgr_delete_authorise_devlist();
}
#endif



#ifdef ROUTER_SDK

int devmgr_attach_sub_device(const char *name,
                             const char *type,
                             const char *category,
                             const char *manufacturer,
                             const char *mac,
                             const char *model)
{
    int ret = SERVICE_RESULT_OK;
    dev_info_t *devinfo;

    devinfo = devmgr_get_devinfo(mac);
    if (devinfo) {
        memcpy(devinfo->router_base.name, name, STR_NAME_LEN);
        memcpy(devinfo->router_base.ostype, type, STR_NAME_LEN);
        memcpy(devinfo->router_base.category, category, STR_NAME_LEN);
        memcpy(devinfo->router_base.manufacturer, manufacturer, STR_NAME_LEN);
        memcpy(devinfo->router_base.model, model, STR_MODEL_LEN);

        devinfo->dev_base.dev_type = DEV_TYPE_WIFI;

        //设置状态
        devinfo->link_state = LINK_STATE_ONLINE;
        devinfo->cloud_state = DEVICE_STATE_INITIAL;

        //register and login
        ret = router_network_up_event_handler(devinfo);
        RET_LOG(ret, CALL_FUCTION_FAILED, "router_network_up_event_handler");


        devmgr_put_devinfo_ref(devinfo);

        return ret;
    }

    devinfo = devmgr_new_devinfo();
    PTR_RETURN(devinfo, SERVICE_RESULT_ERR, "new device info fail");

    memcpy(devinfo->router_base.name, name, STR_NAME_LEN);
    memcpy(devinfo->router_base.ostype, type, STR_NAME_LEN);
    memcpy(devinfo->router_base.category, category, STR_NAME_LEN);
    memcpy(devinfo->router_base.manufacturer, manufacturer, STR_NAME_LEN);
    memcpy(devinfo->dev_base.u.ether_mac, mac, ETHER_ADDR_BYTES);
    memcpy(devinfo->router_base.model, model, STR_MODEL_LEN);
    devinfo->dev_base.dev_type = DEV_TYPE_WIFI;

    ret = __new_devidx(&devinfo->device_idx);
    if (ret < 0) { //设备列表达到上限, 不允许添加设备
        devmgr_put_devinfo_ref(devinfo);
        LOGE(MODULE_NAME_DEVMGR, "new device index fail, mac:%s",
             devinfo->dev_base.u.ether_mac);
        return ret;
    }

    //设置状态
    devinfo->link_state = LINK_STATE_ONLINE;
    devinfo->cloud_state = DEVICE_STATE_INITIAL;

    //register and login
    ret = router_network_up_event_handler(devinfo);
    RET_LOG(ret, CALL_FUCTION_FAILED, "router_network_up_event_handler");
    if (SERVICE_RESULT_OK != ret) {
        __release_devidx(devinfo->device_idx);
        devmgr_put_devinfo_ref(devinfo);
        return ret;
    }

    //添加到未知设备列表
    os_mutex_lock(devlist_lock);
    dlist_add_tail(&devinfo->list_node, &dev_head);
    os_mutex_unlock(devlist_lock);

    return ret;
}

int32_t devmgr_detach_sub_device(const char *mac)
{
    int ret = SERVICE_RESULT_OK;

    LOGI(MODULE_NAME_DEVMGR, "detach sub device, mac:%s", mac);

    dev_info_t *devinfo = devmgr_get_devinfo(mac);
    PTR_RETURN(devinfo, SERVICE_RESULT_ERR, "device not found, mac:%s", mac);

    ret = router_network_down_event_handler(devinfo);
    RET_LOG(ret, CALL_FUCTION_FAILED, "router_network_up_event_handler");

    devmgr_put_devinfo_ref(devinfo);

    return ret;
}

#endif

int devmgr_init()
{
    int ret = SERVICE_RESULT_OK;
    uint8_t network_up = NETWORK_EVENT_UP;

    devlist_lock = os_mutex_init();
    if (NULL == devlist_lock) {
        return SERVICE_RESULT_ERR;
    }

#ifdef GATEWAY_SDK
    //device attr cache init
    /* ret = devmgr_cache_init(); */
    /* RET_RETURN(ret, "devmgr attribute cache init fail"); */

    //load devicelist
    /* ret = devmgr_load_devinfo(); */
    /* RET_LOG(ret, "devmgr load device info fail"); */
    /* __dump_device_list(); */

    //网络ready情况下，全部设备上线
    //if(network_up == NETWORK_EVENT_UP)
    //    devmgr_network_event_cb(network_up);

    //注册网关属性和服务
    ret = devmgr_alink_init();
    RET_LOG(ret, "devmgr alink init fail");
#endif

#ifdef ROUTER_SDK
    devmgr_router_init();
#endif

    LOGD(MODULE_NAME_DEVMGR, "devmgr init success");
    return ret;
}

void devmgr_exit()
{
#ifdef GATEWAY_SDK
    devmgr_alink_exit();
    devmgr_free_unknown_devlist();
    devmgr_free_authorise_devlist();
    devmgr_cache_exit();
#endif

#ifdef ROUTER_SDK
    devmgr_router_exit();
#endif

    os_mutex_destroy(devlist_lock);
    devlist_lock = NULL;
    LOGW(MODULE_NAME_DEVMGR, "devmgr exit");
}


