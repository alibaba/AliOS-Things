#ifndef __CUSTOM_PROFILE_H__
#define __CUSTOM_PROFILE_H__

const char* alipay_box_sdk_get_mqtt_product_key(void);

int alipay_box_sdk_set_mqtt_product_key(const char* mqtt_product_key);

const char* alipay_box_sdk_get_supplier_id(void);

int alipay_box_sdk_set_supplier_id(const char* supplier_id);

const char* alipay_box_sdk_get_item_id(void);

int alipay_box_sdk_set_item_id(const char* item_id);

const char* alipay_box_sdk_get_box_model(void);

int alipay_box_sdk_set_box_model(const char* box_model);

const char* alipay_box_sdk_get_box_version(void);

int alipay_box_sdk_set_box_version(const char* box_version);

#ifdef USE_SERVER_XP
const char* alipay_box_sdk_get_xp_product_key(void);

int alipay_box_sdk_set_xp_product_key(const char* xp_product_key);

const char* alipay_box_sdk_get_xp_product_secret(void);

int alipay_box_sdk_set_xp_product_secret(const char* xp_product_secret);
#endif //USE_SERVER_XP

#endif //__CUSTOM_PROFILE_H__