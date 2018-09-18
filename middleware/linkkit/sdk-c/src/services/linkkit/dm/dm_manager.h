/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef _DM_MANAGER_H_
#define _DM_MANAGER_H_

#include "iotx_dm_internal.h"
#include "dm_shadow.h"

#define DM_MGR_DEV_SUB_TIMEOUT_MS (5000)

#define DM_MGR_DEV_SUB_START      (-1)
#define DM_MGR_DEV_SUB_END        (-2)

typedef struct {
    int generic_index;
    int service_event_index;
    int service_event_number;
    char **service_event;
    uint64_t ctime;
} dm_mgr_dev_sub_t;

typedef struct {
    int devid;
    int dev_type;
    dm_shw_t *dev_shadow;
    char product_key[PRODUCT_KEY_MAXLEN];
    char device_name[DEVICE_NAME_MAXLEN];
    char device_secret[DEVICE_SECRET_MAXLEN];
    iotx_dm_tsl_source_t tsl_source;
    iotx_dm_dev_avail_t status;
    iotx_dm_dev_status_t dev_status;
    dm_mgr_dev_sub_t sub_status;
    struct list_head linked_list;
} dm_mgr_dev_node_t;

typedef struct {
    void *mutex;
    int global_devid;
    struct list_head dev_list;
} dm_mgr_ctx;

int dm_mgr_init(void);
int dm_mgr_deinit(void);
int dm_mgr_device_create(_IN_ int dev_type, _IN_ char product_key[PRODUCT_KEY_MAXLEN],
                         _IN_ char device_name[DEVICE_NAME_MAXLEN], _OU_ int *devid);
int dm_mgr_device_destroy(_IN_ int devid);
int dm_mgr_device_number(void);
int dm_mgr_get_devid_by_index(_IN_ int index, _OU_ int *devid);
int dm_mgr_get_next_devid(_IN_ int devid, _OU_ int *devid_next);
int dm_mgr_search_device_by_devid(_IN_ int devid, _OU_ char product_key[PRODUCT_KEY_MAXLEN],
                                  _OU_ char device_name[DEVICE_NAME_MAXLEN], _OU_ char device_secret[DEVICE_SECRET_MAXLEN]);
int dm_mgr_search_device_by_pkdn(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN],
                                 _OU_ int *devid);
int dm_mgr_search_device_node_by_devid(_IN_ int devid, _OU_ void **node);

int dm_mgr_get_dev_type(_IN_ int devid, _OU_ int *dev_type);
int dm_mgr_set_dev_enable(_IN_ int devid);
int dm_mgr_set_dev_disable(_IN_ int devid);
int dm_mgr_get_dev_avail(_IN_ char product_key[PRODUCT_KEY_MAXLEN], _IN_ char device_name[DEVICE_NAME_MAXLEN],
                         _OU_ iotx_dm_dev_avail_t *status);
int dm_mgr_set_dev_status(_IN_ int devid, _IN_ iotx_dm_dev_status_t status);
int dm_mgr_get_dev_status(_IN_ int devid, _OU_ iotx_dm_dev_status_t *status);
int dm_mgr_set_dev_sub_generic_index(_IN_ int devid, _IN_ int index);
int dm_mgr_get_dev_sub_generic_index(_IN_ int devid, _OU_ int *index);
int dm_mgr_set_dev_sub_service_event(_IN_ int devid, _IN_ int number, _IN_ char **service_event);
int dm_mgr_set_dev_sub_service_event_index(_IN_ int devid, _IN_ int index);
int dm_mgr_get_dev_sub_service_event_number(_IN_ int devid, _OU_ int *number);
int dm_mgr_get_dev_sub_service_event_index(_IN_ int devid, _OU_ int *index);
int dm_mgr_get_dev_sub_service_event(_IN_ int devid, _IN_ int index, _OU_ char **service_event);
int dm_mgr_clear_dev_sub_service_event(_IN_ int devid);
void dm_mgr_dev_sub_status_check(void);
int dm_mgr_set_device_secret(_IN_ int devid, _IN_ char device_secret[DEVICE_SECRET_MAXLEN]);

int dm_mgr_deprecated_assemble_property(_IN_ int devid, _IN_ char *identifier, _IN_ int identifier_len,
                                        _IN_ lite_cjson_item_t *lite);
#ifdef CONFIG_DM_DEVTYPE_GATEWAY
int dm_mgr_upstream_thing_sub_register(_IN_ int devid);
int dm_mgr_upstream_thing_sub_unregister(_IN_ int devid);
int dm_mgr_upstream_thing_topo_add(_IN_ int devid);
int dm_mgr_upstream_thing_topo_delete(_IN_ int devid);
int dm_mgr_upstream_thing_topo_get(void);
int dm_mgr_upstream_thing_list_found(_IN_ int devid);
int dm_mgr_upstream_combine_login(_IN_ int devid);
int dm_mgr_upstream_combine_logout(_IN_ int devid);
#endif
int dm_mgr_upstream_thing_property_post(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len);
int dm_mgr_upstream_thing_event_post(_IN_ int devid, _IN_ char *identifier, _IN_ int identifier_len, _IN_ char *method,
                                     _IN_ char *payload, _IN_ int payload_len);
int dm_mgr_upstream_thing_deviceinfo_update(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len);
int dm_mgr_upstream_thing_deviceinfo_delete(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len);
int dm_mgr_upstream_thing_dsltemplate_get(_IN_ int devid);
int dm_mgr_upstream_thing_dynamictsl_get(_IN_ int devid);
int dm_mgr_upstream_thing_model_up_raw(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len);
int dm_mgr_upstream_ntp_request(void);
int dm_mgr_deprecated_upstream_thing_service_response(_IN_ int devid, _IN_ int msgid, _IN_ iotx_dm_error_code_t code,
        _IN_ char *identifier, _IN_ int identifier_len, _IN_ char *payload, _IN_ int payload_len);
int dm_mgr_upstream_thing_service_response(_IN_ int devid, _IN_ char *msgid, _IN_ int msgid_len,
        _IN_ iotx_dm_error_code_t code,
        _IN_ char *identifier, _IN_ int identifier_len, _IN_ char *payload, _IN_ int payload_len);
int dm_mgr_upstream_thing_property_get_response(_IN_ int devid, _IN_ char *msgid, _IN_ int msgid_len,
        _IN_ iotx_dm_error_code_t code,
        _IN_ char *payload, _IN_ int payload_len, _IN_ void *ctx);
int dm_mgr_upstream_rrpc_response(_IN_ int devid, _IN_ char *msgid, _IN_ int msgid_len, _IN_ iotx_dm_error_code_t code,
                                  _IN_ char *rrpcid, _IN_ int rrpcid_len, _IN_ char *payload, _IN_ int payload_len);
int dm_mgr_upstream_thing_lan_prefix_get(_IN_ int devid);
#ifdef DEPRECATED_LINKKIT
int dm_mgr_deprecated_set_tsl_source(_IN_ int devid, _IN_ iotx_dm_tsl_source_t tsl_source);
int dm_mgr_deprecated_get_tsl_source(_IN_ int devid, _IN_ iotx_dm_tsl_source_t *tsl_source);
int dm_mgr_deprecated_get_shadow(_IN_ int devid, void **shadow);
int dm_mgr_deprecated_search_devid_by_device_node(_IN_ void *node, _OU_ int *devid);
int dm_mgr_deprecated_set_tsl(int devid, iotx_dm_tsl_type_t tsl_type, const char *tsl, int tsl_len);
int dm_mgr_deprecated_get_product_key(_IN_ int devid, _OU_ char product_key[PRODUCT_KEY_MAXLEN]);
int dm_mgr_deprecated_get_device_name(_IN_ int devid, _OU_ char device_name[DEVICE_NAME_MAXLEN]);
int dm_mgr_deprecated_get_property_data(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _OU_ void **data);
int dm_mgr_deprecated_get_service_input_data(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _OU_ void **data);
int dm_mgr_deprecated_get_service_output_data(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _OU_ void **data);
int dm_mgr_deprecated_get_event_output_data(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _OU_ void **data);
int dm_mgr_deprecated_get_data_type(_IN_ void *property, _OU_ dm_shw_data_type_e *type);
int dm_mgr_deprecated_get_property_number(_IN_ int devid, _OU_ int *number);
int dm_mgr_deprecated_get_service_number(_IN_ int devid, _OU_ int *number);
int dm_mgr_deprecated_get_event_number(_IN_ int devid, _OU_ int *number);
int dm_mgr_deprecated_get_property_by_index(_IN_ int devid, _IN_ int index, _OU_ void **property);
int dm_mgr_deprecated_get_service_by_index(_IN_ int devid, _IN_ int index, _OU_ void **service);
int dm_mgr_deprecated_get_event_by_index(_IN_ int devid, _IN_ int index, _OU_ void **event);
int dm_mgr_deprecated_get_service_by_identifier(_IN_ int devid, _IN_ char *identifier, _OU_ void **service);
int dm_mgr_deprecated_get_event_by_identifier(_IN_ int devid, _IN_ char *identifier, _OU_ void **event);
int dm_mgr_deprecated_get_property_identifier(_IN_ void *property, _OU_ char **identifier);
int dm_mgr_deprecated_get_service_method(_IN_ void *service, _OU_ char **method);
int dm_mgr_deprecated_get_event_method(_IN_ void *event, _OU_ char **method);
int dm_mgr_deprecated_set_property_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value,
        _IN_ int value_len);
int dm_mgr_deprecated_get_property_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value);
int dm_mgr_deprecated_set_event_output_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value,
        _IN_ int value_len);
int dm_mgr_deprecated_get_event_output_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value);
int dm_mgr_deprecated_set_service_input_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value,
        _IN_ int value_len);
int dm_mgr_deprecated_get_service_input_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value);
int dm_mgr_deprecated_set_service_output_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value,
        _IN_ int value_len);
int dm_mgr_deprecated_get_service_output_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value);
int dm_mgr_deprecated_assemble_event_output(_IN_ int devid, _IN_ char *identifier, _IN_ int identifier_len,
        _IN_ lite_cjson_item_t *lite);
int dm_mgr_deprecated_assemble_service_output(_IN_ int devid, _IN_ char *identifier, _IN_ int identifier_len,
        _IN_ lite_cjson_item_t *lite);
#endif
#endif
