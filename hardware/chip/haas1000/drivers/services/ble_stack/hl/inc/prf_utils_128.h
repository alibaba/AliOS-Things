/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */



#ifndef _PRF_UTILS_128_H_
#define _PRF_UTILS_128_H_

/**
 ****************************************************************************************
 * @addtogroup PRF_UTILS
 * @ingroup PROFILE
 *
 * @brief Definitions of shared profiles functions that can be used by several profiles
 *
 * @{
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_CLIENT_PRF)
#include "ke_msg.h"
#include "prf.h"
#include "prf_types.h"
#include "gattc_task.h"
#include "gapc.h"
#include "gapc_task.h"
#include "attm_db.h"
#endif /* (BLE_CLIENT_PRF) */

/*
 * MACROS
 ****************************************************************************************
 */

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
 

#if (BLE_CLIENT_PRF)

/// Characteristic definition
struct prf_char_def_128
{
    /// Characteristic UUID
    uint8_t uuid[ATT_UUID_128_LEN];
    /// Requirement Attribute Flag
    uint8_t req_flag;
    /// Mandatory Properties
    uint8_t prop_mand;
};

/**
 ****************************************************************************************
 * @brief Request service discovery with 128-bit UUID on peer device.
 *
 * This request will be used to retrieve start and end handles of the service.
 *
 * @param con_info Pointer to connection information (connection handle, app task id,
 *                 profile task id)
 *
 * @param uuid_128 128-bit service UUID
 ****************************************************************************************
 */
void prf_disc_svc_send_128(prf_env_t *prf_env, uint8_t conidx, uint8_t *uuid_128);

/**
 ****************************************************************************************
 * @brief Check validity for service characteristic with 128-bit UUID
 *
 * For each characteristic in service it verifies handles.
 *
 * If some handles are not present, it checks if they shall be present or they are optional.
 *
 * @param nb_chars   Number of Characteristics in the service
 * @param chars      Characteristics values (char handles, val handles, properties)
 * @param chars_req  Characteristics requirements.
 *
 * @return 0x1 if service is valid, 0x00 else.
 ****************************************************************************************
 */																		
uint8_t prf_check_svc_char_validity_128(uint8_t nb_chars,
                                    const struct prf_char_inf* chars,
                                    const struct prf_char_def_128* chars_req);

void prf_extract_svc_info_128(const struct gattc_sdp_svc_ind* param,
        uint8_t nb_chars, const struct prf_char_def_128* chars_req, struct prf_char_inf* chars,
        uint8_t nb_descs, const struct prf_char_desc_def* descs_req, struct prf_char_desc_inf* descs);

#endif //(BLE_CLIENT_PRF)


/// @} prf_utils

#endif /* _PRF_UTILS_128_H_ */
