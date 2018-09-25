/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef ALI_CORE_H__
#define ALI_CORE_H__

#include <stdint.h>
#include "ali_common.h"
#include "ble_ais.h"
#include "ali_transport.h"
#include "ali_auth.h"
#include "ali_ext.h"
#include "bzopt.h"

#ifdef __cplusplus
extern "C"
{
#endif


#define ALI_COMPANY_ID 0x01A8
#define ALI_PROTOCOL_ID 0x05

#define ALI_SDK_VERSION "2.0.4" /**< Alibaba SDK version. */

    /**
     * @brief Types of commands for control interface.
     */
    typedef enum
    {
        ALI_CTRL_SET_MTU = 0,
    } ali_ctrl_t;


    /**
     * @brief Types of events.
     */
    typedef enum
    {
        ALI_EVT_CONNECTED,     /**< Connection established. */
        ALI_EVT_DISCONNECTED,  /**< Disconnected from peer. */
        ALI_EVT_AUTHENTICATED, /**< Authentication has been successful. */
        ALI_EVT_TX_DONE, /**< Command has been sent to central successfully. */
        ALI_EVT_CTRL_RECEIVED,  /**< Command ALI_CMD_CTRL has been received from
                                   the central. */
        ALI_EVT_QUERY_RECEIVED, /**< Command ALI_CMD_QUERY has been received
                                   from the central. */
        ALI_EVT_APINFO,       /**< AP info has been received. */
	ALI_EVT_OTA_CMD,     /*< encapsulate OTA cmd, p_data[0] = ota_cmd, p_data[1] = num_frame>,...,remaining the reset */
        ALI_EVT_ERROR,        /**< Error reported by lower layers. */
    } ali_evt_type_t;


    /**@brief Structure for generic data.
     */
    typedef struct
    {
        uint8_t *p_data; /**< Pointer to data. */
        uint16_t length; /**< Length of data, excluding the '\0' at the end if
                            it exists. */
    } ali_data_t;


    /**@brief Structure for Rx data event @ref ALI_EVT_CTRL_RECEIVED and @ref
     * ALI_EVT_QUERY_RECEIVED.
     * @note  The referred data will be invalidated on function exit.
     */
    typedef ali_data_t ali_rx_data_evt_t;


    /**@brief Structure for error event. */
    typedef struct
    {
        uint32_t source;   /**< The location which caused the error. */
        uint32_t err_code; /**< Error code which has been raised. */
    } ali_error_evt_t;


    /**@brief Structure for core module event. */
    typedef struct
    {
        ali_evt_type_t type; /**< Event type. */
        union
        {
            ali_rx_data_evt_t rx_data; /**< Data provided for rx-data event. */
            ali_error_evt_t   error;   /**< Data provided for error event. */
        } data;
    } ali_event_t;


    /**
     * @brief Event handler.
     *
     * @param[in] p_context   Context passed to event handler, set on
     * initialization.
     * @param[in] p_event     Pointer to event structure. Event is allocated on
     * the stack so it is available only within the context of the event
     * handler.
     */
    typedef void (*ali_event_handler_t)(void *p_context, ali_event_t *p_event);


    /**@brief Structure for core module configuration. */
    typedef struct
    {
        uint16_t            context_size;  /**< Size of context. */
        ali_event_handler_t event_handler; /**< Pointer to event handler. */
        void *p_evt_context; /**< Pointer to context which will be passed as a
                                parameter of event_handler. */
        uint32_t   model_id; /**< Model ID. */
        ali_data_t mac;      /**< MAC, format: "xx:xx:xx:xx:xx:xx". */
        ali_data_t secret;   /**< Secret (ASCII, from 16 to 40 bytes). */
        ali_data_t product_secret; /**< Secret (ASCII, from 16 to 40 bytes). */
        ali_data_t
                   product_key; /**< Product key (ASCII, from 11 to 20 bytes). */
        ali_data_t device_key; /**< Device key (ASCII, from 20 to 32 bytes). For
                                  V2 network, this is device name. */
        ali_data_t sw_ver;     /**< Software version. */
        uint32_t   timer_prescaler; /**< Prescaler of timers. */
        uint32_t   transport_timeout; /**< Timeout of Tx/Rx, in number of ms. Fill
                                       0 if not used. */
        bool     enable_auth;       /**< Enable authentication. */
        bool     enable_ota;        /**< Enable OTA firmware upgrade. */
        uint16_t max_mtu;           /**< Maximum MTU. */
        uint8_t  *user_adv_data;    /**< User's adv data, if any. */
        uint32_t user_adv_len;      /**< User's adv data length */
    } ali_init_t;

#define MAX_ADV_DATA_LEN 16
#define TX_BUFF_LEN                                                           \
    (BZ_MAX_SUPPORTED_MTU - 3) /**< Transport layer: Tx buffer size (see \ \ \
                                   \ \ \ \ \ specification v1.0.4, ch.5.9). */
#define RX_BUFF_LEN                                                           \
    ALI_TRANSPORT_MAX_RX_DATA_LEN /**< Transport layer: Rx buffer size (see \ \
                                     \                                        \
                                     \ \ \ \ \ \ specification v1.0.4,                                                \
                                     ch.5.9). */

    /**@brief Core module structure. */
    typedef struct
    {
        ble_ais_t       ais;            /**< ble_ais structure. */
        ali_transport_t transport;      /**< Transport layer structure. */
        ali_auth_t      auth;           /**< Authentication module structure. */
        ali_ext_t       ext;            /**< Extend module structure */
        bool            is_initialized; /**< Whether module has been initialized
                                           successfully. */
        bool is_auth_enabled; /**< Whether ali_auth module has been enabled. */
        ali_event_handler_t event_handler; /**< Pointer to event handler. */
        void *p_evt_context;   /**< Pointer to context which will be passed as a
                                  parameter of event_handler. */
        bool is_authenticated; /**< flag which tells whether authentication has
                                  been successful. */
        uint16_t conn_handle;  /**< Connection handle. */
        uint8_t  manuf_spec_adv_data[MAX_ADV_DATA_LEN]; /**< Payload of
                                                           manufacturer specific
                                                           advertising data. */
        uint16_t manuf_spec_adv_data_len; /**< Length of manufacturer specific
                                             advertising data. */
        uint8_t  tx_buff[TX_BUFF_LEN];    /**< Tx buffer for transport layer. */
        uint32_t rx_buff[RX_BUFF_LEN / sizeof(uint32_t)]; /**< Rx buffer for
                                                             transport layer. */
    } ali_t;

    extern ali_t *g_ali;

    /**
     * @brief Function for initializing the core module.
     *
     * This function configures and enables the core module.
     *
     * @param[in] p_ali     Pointer to core module context.
     * @param[in] p_init    Initial configuration.
     *
     * @retval    BREEZE_SUCCESS            If initialization was successful.
     * @retval    BREEZE_ERROR_NULL         If NULL pointers are provided.
     */
    ret_code_t ali_init(void *p_ali, ali_init_t const *p_init);


    /**
     * @brief Function for resetting the core module.
     *
     * @param[in] p_ali     Pointer to core module context.
     */
    void ali_reset(void *p_ali);


    /**@brief Function for sending commands to central through notification.
     *
     * @param[in] p_ali     Core module structure.
     * @param[in] cmd       cmd, 0: default reply; other: internal cmd
     * @param[in] p_data    Pointer to data.
     * @param[in] length    Length of data.
     *
     * @retval    BREEZE_SUCCESS            If the process was successful.
     * @retval    BREEZE_ERROR_NULL         If NULL pointers are provided.
     * @retval    BREEZE_ERROR_BUSY         If Tx process is already on-going.
     * @retval    BREEZE_ERROR_DATA_SIZE    If data size is zero.
     * @retval    BREEZE_ERROR_INVALID_DATA If invalid parameters have been
     * provided.
     */
    ret_code_t ali_send_notify(void *p_ali, uint8_t cmd, uint8_t *p_data, uint16_t length);


    /**@brief Function for sending commands to central through indication.
     *
     * @param[in] p_ali     Core module structure.
     * @param[in] cmd       cmd type 
     * @param[in] p_data    Pointer to data.
     * @param[in] length    Length of data.
     *
     * @retval    BREEZE_SUCCESS            If the process was successful.
     * @retval    BREEZE_ERROR_NULL         If NULL pointers are provided.
     * @retval    BREEZE_ERROR_BUSY         If Tx process is already on-going.
     * @retval    BREEZE_ERROR_DATA_SIZE    If data size is zero.
     * @retval    BREEZE_ERROR_INVALID_DATA If invalid parameters have been
     * provided.
     */
    ret_code_t ali_send_indicate(void *p_ali, uint8_t cmd, uint8_t *p_data, uint16_t length);


    /**@brief Function for getting manufacturer-specific advertising data.
     *
     * @param[in]    p_ali      Core module structure.
     * @param[in]    p_data     Pointer to data.
     * @param[inout] length     Length of data.
     *
     * @retval    BREEZE_SUCCESS            If the process was successful.
     * @retval    BREEZE_ERROR_NULL         If NULL pointers are provided.
     * @retval    BREEZE_ERROR_BUSY         If Tx process is already on-going.
     * @retval    BREEZE_ERROR_DATA_SIZE    If data size is zero.
     * @retval    BREEZE_ERROR_INVALID_DATA If invalid parameters have been
     * provided.
     */
    ret_code_t ali_get_manuf_spec_adv_data(void *p_ali, uint8_t *p_data,
                                           uint16_t *length);

    /**@brief Control interface.
     *
     * @param[in] p_ali       Core module structure.
     * @param[in] ctrl_word   Control word.
     * @param[in] p_data      Pointer to data provided, dependent on ctrl_word.
     *
     * @retval    BREEZE_SUCCESS If the process was successful.
     */
    ret_code_t ali_ctrl(void *p_ali, ali_ctrl_t ctrl_word, void *p_data);

    void notify_evt_no_data(ali_t *p_ali, ali_evt_type_t evt_type);

#ifdef __cplusplus
}
#endif

#endif // ALI_CORE_H__
