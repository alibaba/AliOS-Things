/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _ALINK_EXPORT_ZIGBEE_H_
#define _ALINK_EXPORT_ZIGBEE_H_

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

#define STR_NAME_LEN                (32)
#define IEEE_ADDR_BYTES             (8)
#define SUBDEV_RAND_BYTES           (16)

/** @defgroup zigbee_device zigbee API
 *  @{
 */

/** @defgroup zigbee_struct struct
 *  @{
 */


/**
 * command argument
 */
typedef struct cmd_argument {
    char arg_name[STR_NAME_LEN];
    unsigned char data_type;
    unsigned int mask_code; /*only used for argument of bitmap type*/
    struct cmd_argument *next_arg;
    struct cmd_argument *child_argument;/*only used for argument of bitmap type*/
} cmd_argument_t;

/**
 * attr element
 */
typedef struct attr_element {
    char element_name[STR_NAME_LEN];
    unsigned char data_type;/**< defined by zigbee protocol */
    unsigned int mask_code; /*only used for attribute of bitmap type*/
    struct attr_element *next_element;
    struct attr_element *child_element;
} attr_element_t;


/**
 * alink attribute profile description
 */
typedef struct attr_profile {
    char attr_name[STR_NAME_LEN];
    unsigned char data_type;
    unsigned short cluster_id;
    unsigned short attribute_id;
    attr_element_t *attr_element;
} attr_profile_t;

/**
 * alink command profile description
 */
typedef struct cmd_profile {
    char cmd_name[STR_NAME_LEN];
    unsigned short cluster_id;
    unsigned char cmd_id;
    unsigned char direction; /**< 0:c to s; 1:s to c */
    cmd_argument_t *args;
} cmd_profile_t;

/** @} */ //end of key struct

/** @defgroup zigbee_status register & report status
 *  @{
 */

/**
 * @brief register and authentication zigbee device to aliyun server
 *
 * @param[in] ieee_addr: 8 bytes zigbee device long address
 * @param[in] model_id: 2 bytes device model identification
 * @param[in] rand: 16 bytes random binary array
 * @param[in] sign: 16 bytes characters of signature, end with '\0', md5(rand|secret)
 * @retval 0 on success, -1 when params invalid
 * @see None.
 * @note when joined zigbee network, invoke this function to register sub device
 */
int alink_zigbee_register_device(unsigned char ieee_addr[IEEE_ADDR_BYTES],
                                 unsigned int model_id, const char rand[SUBDEV_RAND_BYTES], const char *sign);


/**
 * @brief unregister zigbee device to aliyun server
 *
 * @param[in] ieee_addr: 8 bytes zigbee device long address
 * @retval 0 on success, -1 when params invalid
 * @see None.
 * @note when factory reset, invoke this function to unregister sub device
 */
int alink_zigbee_unregister_device(unsigned char ieee_addr[IEEE_ADDR_BYTES]);

/**
 * @brief update zigbee device online status, online or offline
 *
 * @param[in] ieee_addr: 8 bytes zigbee device long address
 * @param[in] online_or_not: 1 online; 0 offline
 *
 * @retval 0 on success, -1 when params invalid
 * @see None.
 * @note alink sdk will keep syncing subdev status with aliyun server
 */
int alink_zigbee_update_online_status(unsigned char ieee_addr[IEEE_ADDR_BYTES],
                                      char online_or_not);

/**
 * @brief report zigbee device attribute list to server, at least 1 attribute should be include
 *
 * @param[in] ieee_addr: zigbee device's long address
 * @param[in] endpoint_id: zigbee device's endpoint
 * @param[in] attr_name[]: NULL-terminated array which hold the attr name list
 * @param[in] attr_value[]: NULL-terminated array which hold the attr value list,
 *            attr_name[x] & attr_value[x] is a key-value pair.
 * @retval 0 when successfully got response from cloud,
 *          otherwise this func will block until timeout and -1 will return
 * @see None.
 * @note when connection with server is unstable, this func will block
 *      until got response from server or timeout.
 */
int alink_zigbee_report_attrs(unsigned char ieee_addr[IEEE_ADDR_BYTES],
                              unsigned char endpoint_id,
                              const char *attr_name[], const char *attr_value[]);

/**
 * @brief report zigbee device event of device to coo
 *
 * @param[in] ieee_addr: zigbee device's long address
 * @param[in] endpoint_id: zigbee device's endpoint
 * @param[in] event_name: event name
 * @param[in] event_args: event params, json format
 * @retval 0 when successfully got response from cloud,
 *          otherwise this func will block until timeout and -1 will return
 * @see None.
 * @note when connection with server is unstable, this func will block
 *      until got response from server or timeout.
 */
int alink_zigbee_report_event(unsigned char ieee_addr[IEEE_ADDR_BYTES],
                              unsigned char endpoint_id,
                              const char *event_name, const char *event_args);



/** @} */ //end of register & report status

/** @defgroup zigbee_callback callback
 *  @{
 */

/**
 * zigbee event, used by alink_register_callback function
 */
enum ALINK_ZIGBEE_CALLBACK {
    /**
     * int callback_zigbee_get_attr(unsigned char ieee_addr[IEEE_ADDR_BYTES], unsigned char endpoint_id,
     *                  const char *attr_name);
     */
    ALINK_ZIGBEE_GET_DEVICE_STATUS = 30,

    /**
     * int callback_zigbee_set_attr(unsigned char ieee_addr[IEEE_ADDR_BYTES], unsigned char endpoint_id,
     *                  const char *attr_name, const char *attr_value);
     */
    ALINK_ZIGBEE_SET_DEVICE_STATUS,

    /**
     * int callback_zigbee_execute_cmd(unsigned char ieee_addr[IEEE_ADDR_BYTES],
     *                  unsigned char endpoint_id, const char *cmd_name,
     *                  const char *cmd_args);
     *
     * @brief execute zigbee device cmd(service)
     *
     * @param[in] ieee_addr: zigbee ieee address
     * @param[in] endpoint_id: zigbee port
     * @param[in] cmd_name: command name
     * @param[in] cmd_args: command params, json format
     * @retval  0 on success, otherwise -1 will return
     */
    ALINK_ZIGBEE_EXECUTE_DEVICE_CMD,

    /**
     * int callback_zigbee_update_attr_profile(st_attr_profile *profile[]);
     *
     * @brief update gateway zigbee sub device's attr profile
     * @param[in] profile: pointer array of device attr profile struct,pointer array end with NULL.
     * @retval  0 on success, otherwise -1 will return
     * @see None.
     * @note None.
     */
    ALINK_ZIGBEE_UPDATE_ATTR_PROFILE,

    /**
     * int callback_zigbee_update_cmd_profile(st_cmd_profile *profile[]);
     *
     * @brief update gateway zigbee sub device's cmd profile
     * @param[in] profile: pointer array of device cmd profile struct,pointer array end with NULL.
     * @retval  0 on success, otherwise -1 will return
     * @see None.
     * @note None.
     */
    ALINK_ZIGBEE_UPDATE_CMD_PROFILE,

    /**
     * int callback_zigbee_remove_device(unsigned char ieee_addr[IEEE_ADDR_BYTES]);
     *
     * @brief remove zigbee sub device's from zigbee network
     * @param[in] ieee_addr: zigbee ieee address
     * @retval  0 on success, otherwise -1 will return
     * @see None.
     * @note None.
     */
    ALINK_ZIGBEE_REMOVE_DEVICE,

    /**
     * int callback_zigbee_permit_join(uint8_t duration);
     *
     * @brief remove zigbee sub device's from zigbee network
     * @param[in] duration: permit join duration time, unit: second
     *                  duration: 0, disable join; 255, enable join forever
     * @retval  0 on success, otherwise -1 will return
     * @see None.
     * @note None.
     */
    ALINK_ZIGBEE_PERMIT_JOIN,

};

/** @} */ //end of callback
/** @} */ //end of zigbee export api

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
}
#endif

#endif
