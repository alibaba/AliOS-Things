/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef ALI_CORE_H__
#define ALI_CORE_H__

#include <stdint.h>
#include "common.h"
#include "ble_service.h"
#include "transport.h"
#include "auth.h"
#include "extcmd.h"
#include "bzopt.h"

#ifdef __cplusplus
extern "C"
{
#endif


#define ALI_COMPANY_ID 0x01A8
#define ALI_PROTOCOL_ID 0x05

#define ALI_SDK_VERSION "2.0.4" /**< Alibaba SDK version. */

#define MAX_ADV_DATA_LEN 16

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
        uint16_t manuf_spec_adv_data_len;
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

    ret_code_t transport_packet(ali_transport_tx_type_t type, void *p_ali_ext, uint8_t cmd,
                                uint8_t *p_data, uint16_t length);

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

    void notify_evt_no_data(ali_t *p_ali, ali_evt_type_t evt_type);

#ifdef __cplusplus
}
#endif

#endif // ALI_CORE_H__
