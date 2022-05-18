/*
 *  Copyright (c) 2021, The OpenThread Authors.
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
 *  This file defines the OpenThread SRP (Service Registration Protocol) client buffers and service pool.
 */

#ifndef OPENTHREAD_SRP_CLIENT_BUFFERS_H_
#define OPENTHREAD_SRP_CLIENT_BUFFERS_H_

#include <openthread/dns.h>
#include <openthread/srp_client.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup api-srp
 *
 * @brief
 *   This module includes functions for SRP client buffers and service pool.
 *
 * @{
 *
 * Functions in this module are only available when feature OPENTHREAD_CONFIG_SRP_CLIENT_BUFFERS_ENABLE is enabled.
 *
 */

/**
 * This struct represents a SRP client service pool entry.
 *
 */
typedef struct otSrpClientBuffersServiceEntry
{
    otSrpClientService mService;  ///< The SRP client service structure.
    otDnsTxtEntry      mTxtEntry; ///< The SRP client TXT entry.
} otSrpClientBuffersServiceEntry;

/**
 * This function gets the string buffer to use for SRP client host name.
 *
 * @param[in]  aInstance  A pointer to the OpenThread instance.
 * @param[out] aSize      Pointer to a variable to return the size (number of bytes) of the string buffer (MUST NOT be
 *                        NULL).
 *
 * @returns A pointer to char buffer to use for SRP client host name.
 *
 */
char *otSrpClientBuffersGetHostNameString(otInstance *aInstance, uint16_t *aSize);

/**
 * This function gets the array of IPv6 address entries to use as SRP client host address list.
 *
 * @param[in]  aInstance     A pointer to the OpenThread instance.
 * @param[out] aArrayLength  Pointer to a variable to return the array length i.e., number of IPv6 address entries in
 *                           the array (MUST NOT be NULL).
 *
 * @returns A pointer to an array of `otIp6Address` entries (number of entries is returned in @p aArrayLength).
 *
 */
otIp6Address *otSrpClientBuffersGetHostAddressesArray(otInstance *aInstance, uint8_t *aArrayLength);

/**
 * This function allocates a new service entry from the pool.
 *
 * The returned service entry instance will be initialized as follows:
 *
 *  - `mService.mName` will point to an allocated string buffer which can be retrieved using the function
 *    `otSrpClientBuffersGetServiceEntryServiceNameString()`.
 *  - `mService.mInstanceName` will point to an allocated string buffer which can be retrieved using the function
 *    `otSrpClientBuffersGetServiceEntryInstanceNameString()`.
 *  - `mService.mSubTypeLabels` points to an array that is returned from `otSrpClientBuffersGetSubTypeLabelsArray()`.
 *  - `mService.mTxtEntries` will point to `mTxtEntry`.
 *  - `mService.mNumTxtEntries` will be set to one.
 *  - Other `mService` fields (port, priority, weight) are set to zero.
 *  - `mTxtEntry.mKey` is set to NULL (value is treated as already encoded).
 *  - `mTxtEntry.mValue` will point to an allocated buffer which can be retrieved using the function
 *    `otSrpClientBuffersGetServiceEntryTxtBuffer()`.
 *  - `mTxtEntry.mValueLength` is set to zero.
 *  - All related data/string buffers and arrays are cleared to all zero.
 *
 * @param[in] aInstance   A pointer to the OpenThread instance.
 *
 * @returns A pointer to the newly allocated service entry or NULL if not more entry available in the pool.
 *
 */
otSrpClientBuffersServiceEntry *otSrpClientBuffersAllocateService(otInstance *aInstance);

/**
 * This function frees a previously allocated service entry.
 *
 * The @p aService MUST be previously allocated using `otSrpClientBuffersAllocateService()` and not yet freed. Otherwise
 * the behavior of this function is undefined.
 *
 * @param[in] aInstance   A pointer to the OpenThread instance.
 * @param[in] aService    A pointer to the service entry to free (MUST NOT be NULL).
 *
 */
void otSrpClientBuffersFreeService(otInstance *aInstance, otSrpClientBuffersServiceEntry *aService);

/**
 * This function frees all previously allocated service entries.
 *
 * @param[in] aInstance   A pointer to the OpenThread instance.
 *
 */
void otSrpClientBuffersFreeAllServices(otInstance *aInstance);

/**
 * This function gets the string buffer for service name from a service entry.
 *
 * @param[in]  aEntry   A pointer to a previously allocated service entry (MUST NOT be NULL).
 * @param[out] aSize    A pointer to a variable to return the size (number of bytes) of the string buffer (MUST NOT be
 *                      NULL).
 *
 * @returns A pointer to the string buffer.
 *
 */
char *otSrpClientBuffersGetServiceEntryServiceNameString(otSrpClientBuffersServiceEntry *aEntry, uint16_t *aSize);

/**
 * This function gets the string buffer for service instance name from a service entry.
 *
 * @param[in]  aEntry   A pointer to a previously allocated service entry (MUST NOT be NULL).
 * @param[out] aSize    A pointer to a variable to return the size (number of bytes) of the string buffer (MUST NOT be
 *                      NULL).
 *
 * @returns A pointer to the string buffer.
 *
 */
char *otSrpClientBuffersGetServiceEntryInstanceNameString(otSrpClientBuffersServiceEntry *aEntry, uint16_t *aSize);

/**
 * This function gets the buffer for TXT record from a service entry.
 *
 * @param[in]  aEntry   A pointer to a previously allocated service entry (MUST NOT be NULL).
 * @param[out] aSize    A pointer to a variable to return the size (number of bytes) of the buffer (MUST NOT be NULL).
 *
 * @returns A pointer to the buffer.
 *
 */
uint8_t *otSrpClientBuffersGetServiceEntryTxtBuffer(otSrpClientBuffersServiceEntry *aEntry, uint16_t *aSize);

/**
 * This function gets the array for service subtype labels from the service entry.
 *
 * @param[in]  aEntry          A pointer to a previously allocated service entry (MUST NOT be NULL).
 * @param[out] aArrayLength    A pointer to a variable to return the array length (MUST NOT be NULL).
 *
 * @returns A pointer to the array.
 *
 */
const char **otSrpClientBuffersGetSubTypeLabelsArray(otSrpClientBuffersServiceEntry *aEntry, uint16_t *aArrayLength);

/**
 * @}
 *
 */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OPENTHREAD_SRP_CLIENT_BUFFERS_H_
