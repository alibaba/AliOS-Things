/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aos/errno.h>
#include "devmgr.h"
#include "devmgr_alink.h"
#include "devmgr_cache.h"

dev_base_t dev_base1;
dev_base_t dev_base2;
void __dump_device_list();


void *alink_cb_func[ALINK_CB_MAX_NUM] = {NULL};


int alink_unregister_service(const char *name)
{
    return 0;
}

int stdd_zbnet_remove_device_cb(uint8_t ieee_addr[8])
{
    log_trace("remove device: %02x%02x%02x%02x%02x%02x%02x%02x",
              ieee_addr[0], ieee_addr[1], ieee_addr[2], ieee_addr[3],
              ieee_addr[4], ieee_addr[5], ieee_addr[6], ieee_addr[7]);
    return 0;
}


int stdd_zigbee_permit_join(uint8_t duration)
{
    return 0;
}

int stdd_zbnet_supervision_interval_cb(uint8_t ieee_addr[8], uint8_t interval)
{
    return 0;
}


int get_device_profile_file(uint8_t dev_type, uint32_t model_id,
                            char file_name[], int max_name_length)
{
    strcpy(file_name, "test_attr_profile.lua");
    return 0;
}


int fmgr_get_attr_profile(char file_name[], int max_name_length)
{
    sprintf(file_name, "test_device_profile.lua");
    return 0;
}


int get_kv(const char *key, void *value_buf, int *buf_len)
{
    int ret = 0;

    if (strcmp(key, "device_01") == 0)    {
        memcpy(value_buf, &dev_base1, sizeof(dev_base1));
    } else if (strcmp(key, "device_02") == 0)    {
        memcpy(value_buf, &dev_base2, sizeof(dev_base2));
    } else {
        ret = -1;
    }

    return ret;
}

int set_kv_in_flash(const char *key, const void *value_buf, int buf_len,
                    int flag)
{
    return 0;
}
int remove_kv(const char *key)
{
    return 0;
}


int alink_register_attribute(const char *name,
                             void *get_cb,
                             void *set_cb)
{
    return 0;
}

int alink_register_service(const char *name, void *exec_cb)
{
    return 0;
}


void stdd_forward_device_status(dev_info_t *devinfo,
                                const char *attr_name, const char *attr_value)
{
    log_trace("forward device state, devid:%s, uuid:%s, attr_name:%s, attr_value:%s",
              devinfo->dev_base.dev_id, devinfo->dev_base.uuid, attr_name, attr_value);
}

int32_t stdd_add_device_profile(uint32_t model_id, const char *profile_path)
{
    log_trace("add device profile, profile_path:%s", profile_path);
    return 0;
}

#define UUID_1       "123412412312312312312sdfsdgdgdew"
#define UUID_2       "5465465465465464865465asdfa12312"
void dev_list_init()
{
    memset(&dev_base1, 0, sizeof(dev_base1));
    memset(&dev_base2, 0, sizeof(dev_base2));

    dev_base1.dev_type = DEV_TYPE_ZIGBEE;
    dev_base1.model_id = 0x01010101;
    memcpy(dev_base1.u.ieee_addr, "12345678", 8);
    strncpy(dev_base1.rand, "rand_01", strlen("rand_01"));
    strncpy(dev_base1.sign, "sign_01", strlen("sign_01"));
    strcpy(dev_base1.uuid, UUID_1);
    bytes_2_hexstr(dev_base1.u.ieee_addr, 8, dev_base1.dev_id,
                   sizeof(dev_base1.dev_id));
    //log_trace("dev_base1.devid:%s", dev_base1.dev_id);

    dev_base2.dev_type = DEV_TYPE_ZIGBEE;
    dev_base2.model_id = 0x02020202;
    memcpy(dev_base2.u.ieee_addr, "87654321", 8);
    strncpy(dev_base2.rand, "rand_02", strlen("rand_02"));
    strncpy(dev_base2.sign, "sign_02", strlen("sign_02"));
    strcpy(dev_base2.uuid, UUID_2);
    bytes_2_hexstr(dev_base2.u.ieee_addr, 8, dev_base2.dev_id,
                   sizeof(dev_base2.dev_id));
    //log_trace("dev_base2.devid:%s", dev_base2.dev_id);
}


void dev_list_ut()
{
    char ieee_addr[8];
    //devmgr_delete_device(UUID_1);
    //devmgr_delete_device(UUID_2);

    memcpy(ieee_addr, "12345678", 8);
    devmgr_leave_zigbee_device(ieee_addr);
    __dump_device_list();
}

#define ATTR_NAME_1     "attr_1"
#define ATTR_NAME_2     "attr_2"
#define ATTR_VALUE_1    "value_1"
#define ATTR_VALUE_2    "value_2"
void dev_cache_ut()
{
    int ret = SERVICE_RESULT_ERR;

    ret = devmgr_update_attr_cache(UUID_1, ATTR_NAME_1, ATTR_VALUE_1);
    RET_LOG(ret, "update attr cache fail, uuid:%s, attr_name:%s", UUID_1,
            ATTR_NAME_1);
    ret = devmgr_update_attr_cache(UUID_1, ATTR_NAME_2, ATTR_VALUE_2);
    RET_LOG(ret, "update attr cache fail, uuid:%s, attr_name:%s", UUID_1,
            ATTR_NAME_2);
    ret = devmgr_update_attr_cache(UUID_2, ATTR_NAME_1, ATTR_VALUE_1);
    RET_LOG(ret, "update attr cache fail, uuid:%s, attr_name:%s", UUID_1,
            ATTR_NAME_1);
    ret = devmgr_update_attr_cache(UUID_2, ATTR_NAME_2, ATTR_VALUE_2);
    RET_LOG(ret, "update attr cache fail, uuid:%s, attr_name:%s", UUID_1,
            ATTR_NAME_2);

    char *attr_value = NULL;
    ret = devmgr_read_attr_cache(UUID_1, ATTR_NAME_1, &attr_value);
    RET_LOG(ret, "read attr cache fail, uuid:%s, attr_name:%s", UUID_1,
            ATTR_NAME_1);
    if (attr_value) {
        log_trace("attr_name:%s, attr_value:%s", ATTR_NAME_1, attr_value);
    }
}

int main(int argc, char *argv[])
{
    int ret = SERVICE_RESULT_ERR;
    dev_list_init();

    ret = devmgr_init();
    RET_RETURN(ret, "devmgr_init fail");
    dev_cache_ut();

    dev_list_ut();

    return 0;
}

