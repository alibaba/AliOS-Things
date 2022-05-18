/*
 *  Copyright (c) 2016-2017, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 * @brief
 *  This file defines the OpenThread Server API.
 */

#ifndef OPENTHREAD_SERVER_H_
#define OPENTHREAD_SERVER_H_

#include <openthread/netdata.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup api-server
 *
 * @brief
 *  This module includes functions to manage local network data with the OpenThread Server.
 *
 * @{
 *
 */

/**
 * This method provides a full or stable copy of the local Thread Network Data.
 *
 * @param[in]     aInstance    A pointer to an OpenThread instance.
 * @param[in]     aStable      TRUE when copying the stable version, FALSE when copying the full version.
 * @param[out]    aData        A pointer to the data buffer.
 * @param[inout]  aDataLength  On entry, size of the data buffer pointed to by @p aData.
 *                             On exit, number of copied bytes.
 *
 */
otError otServerGetNetDataLocal(otInstance *aInstance, bool aStable, uint8_t *aData, uint8_t *aDataLength);

/**
 * Add a service configuration to the local network data.
 *
 * @param[in]  aInstance A pointer to an OpenThread instance.
 * @param[in]  aConfig   A pointer to the service configuration.
 *
 * @retval OT_ERROR_NONE          Successfully added the configuration to the local network data.
 * @retval OT_ERROR_INVALID_ARGS  One or more configuration parameters were invalid.
 * @retval OT_ERROR_NO_BUFS       Not enough room is available to add the configuration to the local network data.
 *
 * @sa otServerRemoveService
 * @sa otServerRegister
 *
 */
otError otServerAddService(otInstance *aInstance, const otServiceConfig *aConfig);

/**
 * Remove a service configuration from the local network data.
 *
 * @param[in]  aInstance          A pointer to an OpenThread instance.
 * @param[in]  aEnterpriseNumber  Enterprise Number of the service entry to be deleted.
 * @param[in]  aServiceData       A pointer to an Service Data to look for during deletion.
 * @param[in]  aServiceDataLength The length of @p aServiceData in bytes.
 *
 * @retval OT_ERROR_NONE       Successfully removed the configuration from the local network data.
 * @retval OT_ERROR_NOT_FOUND  Could not find the Border Router entry.
 *
 * @sa otServerAddService
 * @sa otServerRegister
 *
 */
otError otServerRemoveService(otInstance *   aInstance,
                              uint32_t       aEnterpriseNumber,
                              const uint8_t *aServiceData,
                              uint8_t        aServiceDataLength);

/**
 * This function gets the next service in the local Network Data.
 *
 * @param[in]     aInstance  A pointer to an OpenThread instance.
 * @param[inout]  aIterator  A pointer to the Network Data iterator context. To get the first service entry
                             it should be set to OT_NETWORK_DATA_ITERATOR_INIT.
 * @param[out]    aConfig    A pointer to where the service information will be placed.
 *
 * @retval OT_ERROR_NONE       Successfully found the next service.
 * @retval OT_ERROR_NOT_FOUND  No subsequent service exists in the Thread Network Data.
 *
 */
otError otServerGetNextService(otInstance *aInstance, otNetworkDataIterator *aIterator, otServiceConfig *aConfig);

/**
 * Immediately register the local network data with the Leader.
 *
 * @param[in]  aInstance A pointer to an OpenThread instance.
 *
 * @retval OT_ERROR_NONE  Successfully queued a Server Data Request message for delivery.
 *
 * @sa otServerAddService
 * @sa otServerRemoveService
 *
 */
otError otServerRegister(otInstance *aInstance);

/**
 * @}
 *
 */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OPENTHREAD_SERVER_H_
