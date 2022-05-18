/*
 *  Copyright (c) 2016-21, The OpenThread Authors.
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
 *   This file includes definitions for settings driver.
 */

#ifndef SETTINGS_DRIVER_HPP_
#define SETTINGS_DRIVER_HPP_

#include "openthread-core-config.h"

#include <openthread/platform/settings.h>

#include "common/encoding.hpp"
#include "common/error.hpp"
#include "common/locator.hpp"
#include "common/non_copyable.hpp"
#include "utils/flash.hpp"

namespace ot {

class SettingsDriver : public InstanceLocator, private NonCopyable
{
public:
    /**
     * This constructor initializes the `SettingsDriver`.
     *
     * @param[in]  aInstance     A reference to the OpenThread instance.
     *
     */
    explicit SettingsDriver(Instance &aInstance)
        : InstanceLocator(aInstance)
#if OPENTHREAD_CONFIG_PLATFORM_FLASH_API_ENABLE
        , mFlash(aInstance)
#endif
    {
    }

    /**
     * This method initializes the settings storage driver.
     *
     */
    void Init(void)
    {
#if OPENTHREAD_CONFIG_PLATFORM_FLASH_API_ENABLE
        mFlash.Init();
#else
        otPlatSettingsInit(GetInstancePtr());
#endif
    }

    /**
     * This method deinitializes the settings driver.
     *
     */
    void Deinit(void)
    {
#if !OPENTHREAD_CONFIG_PLATFORM_FLASH_API_ENABLE
        otPlatSettingsDeinit(GetInstancePtr());
#endif
    }

    /**
     * This method sets the critical keys that should be stored in a secure area.
     *
     * @param[in]  aKeys        A pointer to an array containing the list of critical keys.
     * @param[in]  aKeysLength  The number of entries in the @p aKeys array.
     *
     */
    void SetCriticalKeys(const uint16_t *aKeys, uint16_t aKeysLength)
    {
#if OPENTHREAD_CONFIG_PLATFORM_FLASH_API_ENABLE
        OT_UNUSED_VARIABLE(aKeys);
        OT_UNUSED_VARIABLE(aKeysLength);
#else
        otPlatSettingsSetCriticalKeys(GetInstancePtr(), aKeys, aKeysLength);
#endif
    }

    /**
     * This method adds a value to @p aKey.
     *
     * @param[in]  aKey          The key associated with the value.
     * @param[in]  aValue        A pointer to where the new value of the setting should be read from.
     *                           MUST NOT be nullptr if @p aValueLength is non-zero.
     * @param[in]  aValueLength  The length of the data pointed to by @p aValue. May be zero.
     *
     * @retval kErrorNone     The value was added.
     * @retval kErrorNoBufs   Not enough space to store the value.
     *
     */
    Error Add(uint16_t aKey, const void *aValue, uint16_t aValueLength)
    {
        Error          error;
        const uint8_t *value = reinterpret_cast<const uint8_t *>(aValue);

#if OPENTHREAD_CONFIG_PLATFORM_FLASH_API_ENABLE
        error = mFlash.Add(aKey, value, aValueLength);
#else
        error = otPlatSettingsAdd(GetInstancePtr(), aKey, value, aValueLength);
#endif
        return error;
    }

    /**
     * This method removes a value from @p aKey.
     *
     * @param[in] aKey    The key associated with the value.
     * @param[in] aIndex  The index of the value to be removed.
     *                    If set to -1, all values for @p aKey will be removed.
     *
     * @retval kErrorNone       The given key and index was found and removed successfully.
     * @retval kErrorNotFound   The given key or index was not found.
     *
     */
    Error Delete(uint16_t aKey, int aIndex = -1)
    {
        Error error;

#if OPENTHREAD_CONFIG_PLATFORM_FLASH_API_ENABLE
        error = mFlash.Delete(aKey, aIndex);
#else
        error = otPlatSettingsDelete(GetInstancePtr(), aKey, aIndex);
#endif
        return error;
    }

    /**
     * This method fetches the value identified by @p aKey at a given @p aIndex.
     *
     * @param[in]     aKey          The key associated with the requested value.
     * @param[in]     aIndex        The index of the specific item to get.
     * @param[out]    aValue        A pointer to where the value of the setting should be written.
     *                              May be nullptr if just testing for the presence or length of a key.
     * @param[inout]  aValueLength  A pointer to the length of the value.
     *                              When called, this should point to an integer containing the maximum bytes that
     *                              can be written to @p aValue.
     *                              At return, the actual length of the setting is written.
     *                              May be nullptr if performing a presence check.
     *
     * @retval kErrorNone        The value was fetched successfully.
     * @retval kErrorNotFound    The key was not found.
     *
     */
    Error Get(uint16_t aKey, int aIndex, void *aValue, uint16_t *aValueLength) const
    {
        Error    error;
        uint8_t *value = reinterpret_cast<uint8_t *>(aValue);

#if OPENTHREAD_CONFIG_PLATFORM_FLASH_API_ENABLE
        error = mFlash.Get(aKey, aIndex, value, aValueLength);
#else
        error = otPlatSettingsGet(GetInstancePtr(), aKey, aIndex, value, aValueLength);
#endif
        return error;
    }

    /**
     * This method fetches the value identified by @p aKey.
     *
     * @param[in]     aKey          The key associated with the requested value.
     * @param[out]    aValue        A pointer to where the value of the setting should be written.
     *                              May be nullptr if just testing for the presence or length of a key.
     * @param[inout]  aValueLength  A pointer to the length of the value.
     *                              When called, this should point to an integer containing the maximum bytes that
     *                              can be written to @p aValue.
     *                              At return, the actual length of the setting is written.
     *                              May be nullptr if performing a presence check.
     *
     * @retval kErrorNone        The value was fetched successfully.
     * @retval kErrorNotFound    The key was not found.
     *
     */
    Error Get(uint16_t aKey, void *aValue, uint16_t *aValueLength) const { return Get(aKey, 0, aValue, aValueLength); }

    /**
     * This method sets or replaces the value identified by @p aKey.
     *
     * If there was more than one value previously associated with @p aKey, then they are all deleted and replaced with
     * this single entry.
     *
     * @param[in]  aKey          The key associated with the value.
     * @param[in]  aValue        A pointer to where the new value of the setting should be read from.
     *                           MUST NOT be nullptr if @p aValueLength is non-zero.
     * @param[in]  aValueLength  The length of the data pointed to by @p aValue. May be zero.
     *
     * @retval kErrorNone     The value was changed.
     * @retval kErrorNoBufs   Not enough space to store the value.
     *
     */
    Error Set(uint16_t aKey, const void *aValue, uint16_t aValueLength)
    {
        Error          error;
        const uint8_t *value = reinterpret_cast<const uint8_t *>(aValue);

#if OPENTHREAD_CONFIG_PLATFORM_FLASH_API_ENABLE
        error = mFlash.Set(aKey, value, aValueLength);
#else
        error = otPlatSettingsSet(GetInstancePtr(), aKey, value, aValueLength);
#endif
        return error;
    }

    /**
     * This method removes all values.
     *
     */
    void Wipe(void)
    {
#if OPENTHREAD_CONFIG_PLATFORM_FLASH_API_ENABLE
        mFlash.Wipe();
#else
        otPlatSettingsWipe(GetInstancePtr());
#endif
    }

private:
    otInstance *GetInstancePtr(void) const { return reinterpret_cast<otInstance *>(&InstanceLocator::GetInstance()); }

#if OPENTHREAD_CONFIG_PLATFORM_FLASH_API_ENABLE
    Flash mFlash;
#endif
};

} // namespace ot

#endif // SETTINGS_DRIVER_HPP_
