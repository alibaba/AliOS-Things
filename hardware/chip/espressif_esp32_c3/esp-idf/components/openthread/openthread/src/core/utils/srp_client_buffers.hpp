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
 *   This file includes definitions for the SRP client buffers and service pool.
 */

#ifndef SRP_CLIENT_BUFFERS_HPP_
#define SRP_CLIENT_BUFFERS_HPP_

#include "openthread-core-config.h"

#include <openthread/srp_client_buffers.h>

#include "common/clearable.hpp"
#include "common/locator.hpp"
#include "common/non_copyable.hpp"
#include "common/pool.hpp"
#include "net/srp_client.hpp"

namespace ot {
namespace Utils {

#if OPENTHREAD_CONFIG_SRP_CLIENT_BUFFERS_ENABLE

#if !OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE
#error "OPENTHREAD_CONFIG_SRP_CLIENT_BUFFERS_ENABLE requires OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE feature."
#endif

/**
 * This class represents the SRP client buffers and service pool.
 *
 */
class SrpClientBuffers : public InstanceLocator, private NonCopyable
{
public:
    /**
     * Maximum number of service entries in the pool.
     *
     */
    static constexpr uint16_t kMaxServices = OPENTHREAD_CONFIG_SRP_CLIENT_BUFFERS_MAX_SERVICES;

    /**
     * Max number of host address entries.
     *
     */
    static constexpr uint16_t kMaxHostAddresses = OPENTHREAD_CONFIG_SRP_CLIENT_BUFFERS_MAX_HOST_ADDRESSES;

    /**
     * Size (number of char) of host name string (includes null `\0` termination char).
     *
     */
    static constexpr uint16_t kHostNameSize = OPENTHREAD_CONFIG_SRP_CLIENT_BUFFERS_HOST_NAME_SIZE;

    /**
     * Size (number of char) of service name string (includes null `\0` termination char).
     *
     */
    static constexpr uint16_t kServiceNameSize = OPENTHREAD_CONFIG_SRP_CLIENT_BUFFERS_SERVICE_NAME_SIZE;

    /**
     * Array length for service subtype label.
     *
     */
    static constexpr uint16_t kServiceMaxSubTypes = OPENTHREAD_CONFIG_SRP_CLIENT_BUFFERS_SERVICE_MAX_SUB_TYPES;

    /**
     * Size (number of char) of service instance name string (includes null `\0` termination char).
     *
     */
    static constexpr uint16_t kInstanceNameSize = OPENTHREAD_CONFIG_SRP_CLIENT_BUFFERS_SERVICE_INSTANCE_NAME_SIZE;

    /**
     * Size (number of bytes) of TXT record buffer.
     *
     */
    static constexpr uint16_t kTxtBufferSize = OPENTHREAD_CONFIG_SRP_CLIENT_BUFFERS_TXT_BUFFER_SIZE;

    /**
     * This class represents a SRP client service entry from the pool.
     *
     */
    class ServiceEntry : public otSrpClientBuffersServiceEntry, public Clearable<ServiceEntry>
    {
        friend class SrpClientBuffers;
        friend class LinkedList<ServiceEntry>;

    public:
        /**
         * This method gets the string buffer for the service name from the service entry.
         *
         * @param[out] aSize    Reference to a variable to return the size (number of bytes) of the string buffer.
         *
         * @returns A pointer to the string buffer.
         *
         */
        char *GetServiceNameString(uint16_t &aSize)
        {
            aSize = sizeof(mServiceName);
            return mServiceName;
        }

        /**
         * This method gets the string buffer for the instance name from the service entry.
         *
         * @param[out] aSize    Reference to a variable to return the size (number of bytes) of the string buffer.
         *
         * @returns A pointer to the string buffer.
         *
         */
        char *GetInstanceNameString(uint16_t &aSize)
        {
            aSize = sizeof(mInstanceName);
            return mInstanceName;
        }

        /**
         * This method gets the buffer for the TXT value from the service entry.
         *
         * @param[out] aSize    Reference to a variable to return the size (number of bytes) of the buffer.
         *
         * @returns A pointer to the buffer.
         *
         */
        uint8_t *GetTxtBuffer(uint16_t &aSize)
        {
            aSize = sizeof(mTxtBuffer);
            return mTxtBuffer;
        }

        /**
         * This method gets the array for service subtype labels from the service entry.
         *
         * @param[out] aArrayLength    Reference to a variable to return the array length.
         *
         * @returns A pointer to the array.
         *
         */
        const char **GetSubTypeLabelsArray(uint16_t &aArrayLength)
        {
            aArrayLength = OT_ARRAY_LENGTH(mSubTypeLabels);
            return mSubTypeLabels;
        }

    private:
        ServiceEntry *      GetNext(void) { return reinterpret_cast<ServiceEntry *>(mService.mNext); }
        const ServiceEntry *GetNext(void) const { return reinterpret_cast<const ServiceEntry *>(mService.mNext); }
        void SetNext(ServiceEntry *aEntry) { mService.mNext = reinterpret_cast<Srp::Client::Service *>(aEntry); }

        char        mServiceName[kServiceNameSize];
        char        mInstanceName[kInstanceNameSize];
        uint8_t     mTxtBuffer[kTxtBufferSize];
        const char *mSubTypeLabels[kServiceMaxSubTypes + 1];
    };

    /**
     * This constructor initializes the `SrpClientBuffers` object.
     *
     * @param[in]  aInstance  A reference to the OpenThread instance.
     *
     */
    explicit SrpClientBuffers(Instance &aInstance);

    /**
     * This method gets the string buffer to use for SRP client host name.
     *
     * @param[out] aSize      Reference to a variable to return the size (number of bytes) of the string buffer.
     *
     * @returns A pointer to char buffer to use for SRP client host name.
     *
     */
    char *GetHostNameString(uint16_t &aSize)
    {
        aSize = sizeof(mHostName);
        return mHostName;
    }

    /**
     * This method gets the array of IPv6 address entries to use as SRP client host address list.
     *
     * @param[out] aArrayLength  Reference to a variable to return the array length (number of IPv6 address entries in
     * *                         the array).
     *
     * @returns A pointer to an array of `Ip6::Address` entries (number of entries is returned in @p aArrayLength).
     *
     */
    Ip6::Address *GetHostAddressesArray(uint8_t &aArrayLength)
    {
        aArrayLength = OT_ARRAY_LENGTH(mHostAddresses);
        return &mHostAddresses[0];
    }

    /**
     * This method allocates a new service entry from the pool.
     *
     * The returned service entry instance will be initialized as follows:
     *
     *  - `mService.mName` points to a string buffer which can be retrieved using `GetServiceNameString()`.
     *  - `mService.mInstanceName` points to a string buffer which can be retrieved using `GetInstanceNameString()`.
     *  - `mService.mSubTypeLabels` points to array which can be retrieved using `GetSubTypeLabelsArray()`.
     *  - `mService.mTxtEntries` points to `mTxtEntry`.
     *  - `mService.mNumTxtEntries` is set to one (one entry in the list).
     *  - Other `mService` fields (port, priority, weight) are set to zero.
     *  - `mTxtEntry.mKey` is set to `nullptr` (value is treated as already encoded data).
     *  - `mTxtEntry.mValue` points to a buffer which can be retrieved using `GetTxtBuffer()`
     *  - `mTxtEntry.mValueLength` is set to zero.
     *  - All related data/string buffers and arrays are cleared to all zero.
     *
     * @returns A pointer to the newly allocated service entry or `nullptr` if not more entry available in the pool.
     *
     */
    ServiceEntry *AllocateService(void);

    /**
     * This method frees a previously allocated service entry.
     *
     * The @p aService MUST be previously allocated using `AllocateService()` and not yet freed. Otherwise the behavior
     * of this method is undefined.
     *
     * @param[in] aServiceEntry     A service entry to free.
     *
     */
    void FreeService(ServiceEntry &aServiceEntry) { mServicePool.Free(aServiceEntry); }

    /**
     * This method frees all previously allocated service entries.
     *
     */
    void FreeAllServices(void) { mServicePool.FreeAll(); }

private:
    char                             mHostName[kHostNameSize];
    Ip6::Address                     mHostAddresses[kMaxHostAddresses];
    Pool<ServiceEntry, kMaxServices> mServicePool;
};

#endif // OPENTHREAD_CONFIG_SRP_CLIENT_BUFFERS_ENABLE

} // namespace Utils
} // namespace ot

#endif // SRP_CLIENT_BUFFERS_HPP_
