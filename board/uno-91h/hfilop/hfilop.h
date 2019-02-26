
#ifndef HFILOP_HEAD_H
#define HFILOP_HEAD_H

#include <stdlib.h>
#include <stdarg.h>
#include "hal/wifi.h"

#define HAL_PARTITION_SYS_DATA      HAL_PARTITION_CUSTOM_1
#define HAL_PARTITION_HFILOP        HAL_PARTITION_CUSTOM_2

#define HFILOP_VERSION "3.01-20181121"


#define HFILOP_PRINTF(...)  \
    do {                                                     \
        printf("\e[0;32m%s@line%d:\t", __FUNCTION__, __LINE__);  \
        printf(__VA_ARGS__);                                 \
        printf("\e[0m");                                   \
    } while (0)


enum HFILOP_MODULE_TYPE
{
	HFILOP_MODULE_LPB130=1,
	HFILOP_MODULE_LPT130=2,
	HFILOP_MODULE_LPT230,
	HFILOP_MODULE_LPT330,
	HFILOP_MODULE_LPB135,
	HFILOP_MODULE_LPT130B
};


typedef int (*data_callback_t)(void *data, uint32_t len);

typedef struct _at_session
{
}at_session_t,*pat_session_t;

typedef struct _at_cmd
{
	const char * name;
	int	(*func)(pat_session_t,int, char** ,char *,int);
	const char * doc;
	int	(*callhook)(pat_session_t,int, char** ,char *,int);
} hfat_cmd_t,*phfat_cmd_t;


/*
 * Please define correct module type, refer enum HFILOP_MODULE_TYPE
 */
#define MODULE_TYPE HFILOP_MODULE_LPT230

/**
 * @brief init RF type.
 *
 * @param[in] type: module type, refer enum HFILOP_MODULE_TYPE
 * @return[out] none
 * @see None.
 * @note None.
 */
void hfilop_init_rf_type(enum HFILOP_MODULE_TYPE type);

/**
 * @brief start uart task.
 *
 * @param[in] cb: uart data callback
 *			 cmd: cmd table
 * @return[out] none
 * @see None.
 * @note None.
 */
void hfilop_uart_task_start(data_callback_t cb, phfat_cmd_t cmd);

/**
 * @brief send uart data.
 *
 * @param[in] data: send data
 *			 len: the length of data, in bytes
 * @return[out] the data length of send success
 * @see None.
 * @note None.
 */
int hfilop_uart_send_data(unsigned char *data, int len);

/**
 * @brief recv uart data.
 *
 * @param[in] data: a pointer to a buffer to recv data
 *			 len: the length, in bytes, of the data pointed to by the data parameter
 *			 timeouts: recv timeout, in millisecond
 * @return[out] the data length of send success
 * @see None.
 * @note None.
 */
int hfilop_uart_recv_data(unsigned char *data, int len, int timeout);

/**
 * @brief check is in cmd mode.
 *
 * @param[in] none
 * @return[out] 1-in cmd mode, 0-not in cmd mode
 * @see None.
 * @note None.
 */
int hfilop_uart_in_cmd_mode(void);

/**
 * @brief start assis thread.
 *
 * @param[in] none
 * @return[out] none
 * @see None.
 * @note None.
 */
void hfilop_assis_task_start(void);

/**
 * @brief check local OTA state.
 *
 * @param[in] none
 * @return[out] none
 * @see None.
 * @note None.
 */
void hfilop_check_ota_state(void);

/**
 * @brief start local OTA.
 *
 * @param[in] none
 * @return[out] flag
 * @see None.
 * @note None.
 */
void hfilop_ota_task_start(int flag);

/**
 * @brief automatically upgrade after connect to specified router.
 *
 * @param[in] none
 * @return[out] ssid: the ssid of specified router
 *			 key: the key of specified router, "" or NULL means no key
 * @see if ssid==NULL and pwd==NULL, will use function 'hfilop_ota_auto_upgrade_is_valid'
 *                   and 'hfilop_ota_auto_upgrade_get_ssid' and 'hfilop_ota_auto_upgrade_get_pwd'.
 * @note will block in this function forever.
 */
void hfilop_ota_auto_upgrade(char *ssid, char *pwd);

/**
 * @brief OTA auto upgrade flag.
 *
 * @param[in] none
 * @return[out] 1-need auto upgrade, 0-not need, default is 0
 * @see None.
 * @note if you need this function, please define it yourself.
 */
int hfilop_ota_auto_upgrade_is_valid(void);

/**
 * @brief OTA auto upgrade ssid name.
 *
 * @param[in] none
 * @return[out] ssid name
 * @see None.
 * @note if you need this function, please define it yourself.
 */
char *hfilop_ota_auto_upgrade_get_ssid(void);

/**
 * @brief OTA auto upgrade password.
 *
 * @param[in] none
 * @return[out] password
 * @see None.
 * @note if you need this function, please define it yourself.
 */
char *hfilop_ota_auto_upgrade_get_pwd(void);

/**
 * @brief OTA success callback function.
 *
 * @param[in] none
 * @return[out] none
 * @see None.
 * @note if you need this function, please define it yourself.
 */
void hfilop_ota_success_callback(void);

/**
 * @brief OTA fail callback function.
 *
 * @param[in] none
 * @return[out] none
 * @see None.
 * @note if you need this function, please define it yourself.
 */
void hfilop_ota_fail_callback(void);


typedef int (*wifi_scan_callback_t)(ap_list_adv_t *);

/**
 * @brief wifi scan.
 *
 * @param[in] cb: pass ssid info(scan result) to this callback one by one
 * @return[out] the number of wifi
 * @see None.
 * @note None.
 */
int hfilop_wifi_scan(wifi_scan_callback_t cb);

/**
 * @brief get MAC address in STA mode.
 *
 * @param[in] none
 * @return[out] MAC address
 * @see None.
 * @note None.
 */
unsigned char *hfilop_layer_get_mac(void);

/**
 * @brief get hostname(MID).
 *
 * @param[in] none
 * @return[out] hostname
 * @see None.
 * @note None.
 */
char *hfilop_layer_get_hostname(void);

/**
 * @brief get ILOP product key.
 *
 * @param[in] none
 * @return[out] product key
 * @see None.
 * @note None.
 */
char *hfilop_layer_get_product_key(void);

/**
 * @brief get ILOP product secret.
 *
 * @param[in] none
 * @return[out] product secret
 * @see None.
 * @note None.
 */
char *hfilop_layer_get_product_secret(void);

/**
 * @brief get ILOP device name.
 *
 * @param[in] none
 * @return[out] device name
 * @see None.
 * @note None.
 */
char *hfilop_layer_get_device_name(void);

/**
 * @brief get ILOP device secret.
 *
 * @param[in] none
 * @return[out] device secret
 * @see None.
 * @note None.
 */
char *hfilop_layer_get_device_secret(void);

/**
 * @brief set hostname.
 *
 * @param[in] hostname, maxnum is 20 bytes
 * @return[out] 0-successfully, other value is failed
 * @see None.
 * @note None.
 */
int hfilop_layer_set_hostname(char * hostname);

/**
 * @brief set product key.
 *
 * @param[in] product key, maxnum is 64 bytes
 * @return[out] 0-successfully, other value is failed
 * @see None.
 * @note None.
 */
int hfilop_layer_set_product_key(char * product_key);

/**
 * @brief set product secret.
 *
 * @param[in] product secret, maxnum is 64 bytes
 * @return[out] 0-successfully, other value is failed
 * @see None.
 * @note None.
 */
int hfilop_layer_set_product_secret(char * product_secret);

/**
 * @brief set device name.
 *
 * @param[in] device name, maxnum is 64 bytes
 * @return[out] 0-successfully, other value is failed
 * @see None.
 * @note None.
 */
int hfilop_layer_set_device_name(char * device_name);

/**
 * @brief set device secret.
 *
 * @param[in] device secret, maxnum is 64 bytes
 * @return[out] 0-successfully, other value is failed
 * @see None.
 * @note None.
 */
int hfilop_layer_set_device_secret(char * device_secret);

/**
 * @brief set uart baud.
 *
 * @param[in] baud
 * @return[out] 0-successfully, other value is failed
 * @see None.
 * @note None.
 */
int hf_uart_config(int baud);

/**
 * @brief set wifi connect wifi when no have product secreta and device name device secret.
 *
 * @param[in] ssid and key
 * @return[out] 0-successfully, other value is failed
 * @see None.
 * @note None.
 */
int hf_wificonn(char *ssid, char *key);

/**
 * @brief set wifi connect wifi when have product secreta and device name device secret.
 *
 * @param[in] ssid and key
 * @return[out] 0-successfully, other value is failed
 * @see None.
 * @note None.
 */
int hf_set_wsssid(char *ssid, char *key);

/**
 * @brief get aliserver type.
 *
 * @param[in] none
 * @return[out] refer to enum linkkit_cloud_domain_type_t
 * @see None.
 * @note None.
 */
int hf_get_aliserver(void);

/**
 * @brief get ali key register type.
 *
 * @param[in] none
 * @return[out] 0: one machine one key, 1:one model one key
 * @see None.
 * @note None.
 */
int hf_get_alikeytype(void);


#endif /* HFILOP_HEAD_H */

