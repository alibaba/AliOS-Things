/*
 *  Copyright (c) 2016, The OpenThread Authors.
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
 *   This file implements the OpenThread platform abstraction for non-volatile storage of settings.
 *
 */

#include "openthread-posix-config.h"
#include "platform-posix.h"

#include <assert.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <openthread/platform/misc.h>
#include <openthread/platform/radio.h>
#include <openthread/platform/settings.h>
#if OPENTHREAD_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
#include <openthread/platform/secure_settings.h>
#endif

#include "common/code_utils.hpp"
#include "common/encoding.hpp"

static const size_t kMaxFileNameSize = sizeof(OPENTHREAD_CONFIG_POSIX_SETTINGS_PATH) + 32;

static int sSettingsFd = -1;

static otError platformSettingsDelete(otInstance *aInstance, uint16_t aKey, int aIndex, int *aSwapFd);

#if OPENTHREAD_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
static const uint16_t *sKeys       = nullptr;
static uint16_t        sKeysLength = 0;

void otPlatSettingsSetCriticalKeys(otInstance *aInstance, const uint16_t *aKeys, uint16_t aKeysLength)
{
    OT_UNUSED_VARIABLE(aInstance);

    sKeys       = aKeys;
    sKeysLength = aKeysLength;
}

static bool isCriticalKey(uint16_t aKey)
{
    bool ret = false;

    VerifyOrExit(sKeys != nullptr);

    for (uint16_t i = 0; i < sKeysLength; i++)
    {
        VerifyOrExit(aKey != sKeys[i], ret = true);
    }

exit:
    return ret;
}
#endif

static void getSettingsFileName(otInstance *aInstance, char aFileName[kMaxFileNameSize], bool aSwap)
{
    const char *offset = getenv("PORT_OFFSET");
    uint64_t    nodeId;

    otPlatRadioGetIeeeEui64(aInstance, reinterpret_cast<uint8_t *>(&nodeId));
    nodeId = ot::Encoding::BigEndian::HostSwap64(nodeId);
    snprintf(aFileName, kMaxFileNameSize, OPENTHREAD_CONFIG_POSIX_SETTINGS_PATH "/%s_%" PRIx64 ".%s",
             offset == nullptr ? "0" : offset, nodeId, (aSwap ? "swap" : "data"));
}

static int swapOpen(otInstance *aInstance)
{
    char fileName[kMaxFileNameSize];
    int  fd;

    getSettingsFileName(aInstance, fileName, true);

    fd = open(fileName, O_RDWR | O_CREAT | O_TRUNC | O_CLOEXEC, 0600);
    VerifyOrDie(fd != -1, OT_EXIT_ERROR_ERRNO);

    return fd;
}

/**
 * This function reads @p aLength bytes from the data file and appends to the swap file.
 *
 * @param[in]   aFd     The file descriptor of the current swap file.
 * @param[in]   aLength Number of bytes to copy.
 *
 */
static void swapWrite(otInstance *aInstance, int aFd, uint16_t aLength)
{
    OT_UNUSED_VARIABLE(aInstance);

    const size_t kBlockSize = 512;
    uint8_t      buffer[kBlockSize];

    while (aLength > 0)
    {
        uint16_t count = aLength >= sizeof(buffer) ? sizeof(buffer) : aLength;
        ssize_t  rval  = read(sSettingsFd, buffer, count);

        VerifyOrDie(rval > 0, OT_EXIT_FAILURE);
        count = static_cast<uint16_t>(rval);
        rval  = write(aFd, buffer, count);
        assert(rval == count);
        VerifyOrDie(rval == count, OT_EXIT_FAILURE);
        aLength -= count;
    }
}

static void swapPersist(otInstance *aInstance, int aFd)
{
    char swapFile[kMaxFileNameSize];
    char dataFile[kMaxFileNameSize];

    getSettingsFileName(aInstance, swapFile, true);
    getSettingsFileName(aInstance, dataFile, false);

    VerifyOrDie(0 == close(sSettingsFd), OT_EXIT_ERROR_ERRNO);
    VerifyOrDie(0 == fsync(aFd), OT_EXIT_ERROR_ERRNO);
    VerifyOrDie(0 == rename(swapFile, dataFile), OT_EXIT_ERROR_ERRNO);

    sSettingsFd = aFd;
}

static void swapDiscard(otInstance *aInstance, int aFd)
{
    char swapFileName[kMaxFileNameSize];

    VerifyOrDie(0 == close(aFd), OT_EXIT_ERROR_ERRNO);
    getSettingsFileName(aInstance, swapFileName, true);
    VerifyOrDie(0 == unlink(swapFileName), OT_EXIT_ERROR_ERRNO);
}

void otPlatSettingsInit(otInstance *aInstance)
{
    otError error = OT_ERROR_NONE;

#if OPENTHREAD_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
    otPosixSecureSettingsInit(aInstance);
#endif

    {
        struct stat st;

        if (stat(OPENTHREAD_CONFIG_POSIX_SETTINGS_PATH, &st) == -1)
        {
            mkdir(OPENTHREAD_CONFIG_POSIX_SETTINGS_PATH, 0755);
        }
    }

    {
        char fileName[kMaxFileNameSize];

        getSettingsFileName(aInstance, fileName, false);
        sSettingsFd = open(fileName, O_RDWR | O_CREAT | O_CLOEXEC, 0600);
    }

    VerifyOrDie(sSettingsFd != -1, OT_EXIT_ERROR_ERRNO);

    for (off_t size = lseek(sSettingsFd, 0, SEEK_END), offset = lseek(sSettingsFd, 0, SEEK_SET); offset < size;)
    {
        uint16_t key;
        uint16_t length;
        ssize_t  rval;

        rval = read(sSettingsFd, &key, sizeof(key));
        VerifyOrExit(rval == sizeof(key), error = OT_ERROR_PARSE);

        rval = read(sSettingsFd, &length, sizeof(length));
        VerifyOrExit(rval == sizeof(length), error = OT_ERROR_PARSE);

        offset += sizeof(key) + sizeof(length) + length;
        VerifyOrExit(offset == lseek(sSettingsFd, length, SEEK_CUR), error = OT_ERROR_PARSE);
    }

exit:
    if (error == OT_ERROR_PARSE)
    {
        VerifyOrDie(ftruncate(sSettingsFd, 0) == 0, OT_EXIT_ERROR_ERRNO);
    }
}

void otPlatSettingsDeinit(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

#if OPENTHREAD_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
    otPosixSecureSettingsDeinit(aInstance);
#endif

    assert(sSettingsFd != -1);
    VerifyOrDie(close(sSettingsFd) == 0, OT_EXIT_ERROR_ERRNO);
}

otError otPlatSettingsGet(otInstance *aInstance, uint16_t aKey, int aIndex, uint8_t *aValue, uint16_t *aValueLength)
{
    OT_UNUSED_VARIABLE(aInstance);

    otError     error  = OT_ERROR_NOT_FOUND;
    const off_t size   = lseek(sSettingsFd, 0, SEEK_END);
    off_t       offset = lseek(sSettingsFd, 0, SEEK_SET);

#if OPENTHREAD_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
    if (isCriticalKey(aKey))
    {
        ExitNow(error = otPosixSecureSettingsGet(aInstance, aKey, aIndex, aValue, aValueLength));
    }
#endif

    VerifyOrExit(offset == 0 && size >= 0, error = OT_ERROR_PARSE);

    while (offset < size)
    {
        uint16_t key;
        uint16_t length;
        ssize_t  rval;

        rval = read(sSettingsFd, &key, sizeof(key));
        VerifyOrExit(rval == sizeof(key), error = OT_ERROR_PARSE);

        rval = read(sSettingsFd, &length, sizeof(length));
        VerifyOrExit(rval == sizeof(length), error = OT_ERROR_PARSE);

        if (key == aKey)
        {
            if (aIndex == 0)
            {
                error = OT_ERROR_NONE;

                if (aValueLength)
                {
                    if (aValue)
                    {
                        uint16_t readLength = (length <= *aValueLength ? length : *aValueLength);

                        VerifyOrExit(read(sSettingsFd, aValue, readLength) == readLength, error = OT_ERROR_PARSE);
                    }

                    *aValueLength = length;
                }

                break;
            }
            else
            {
                --aIndex;
            }
        }

        offset += sizeof(key) + sizeof(length) + length;
        VerifyOrExit(offset == lseek(sSettingsFd, length, SEEK_CUR), error = OT_ERROR_PARSE);
    }

exit:
    VerifyOrDie(error != OT_ERROR_PARSE, OT_EXIT_FAILURE);
    return error;
}

otError otPlatSettingsSet(otInstance *aInstance, uint16_t aKey, const uint8_t *aValue, uint16_t aValueLength)
{
    int     swapFd = -1;
    otError error  = OT_ERROR_NONE;

#if OPENTHREAD_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
    if (isCriticalKey(aKey))
    {
        ExitNow(error = otPosixSecureSettingsSet(aInstance, aKey, aValue, aValueLength));
    }
#endif

    switch (platformSettingsDelete(aInstance, aKey, -1, &swapFd))
    {
    case OT_ERROR_NONE:
    case OT_ERROR_NOT_FOUND:
        break;

    default:
        assert(false);
        break;
    }

    VerifyOrDie(write(swapFd, &aKey, sizeof(aKey)) == sizeof(aKey) &&
                    write(swapFd, &aValueLength, sizeof(aValueLength)) == sizeof(aValueLength) &&
                    write(swapFd, aValue, aValueLength) == aValueLength,
                OT_EXIT_FAILURE);

    swapPersist(aInstance, swapFd);

#if OPENTHREAD_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
exit:
#endif
    return error;
}

otError otPlatSettingsAdd(otInstance *aInstance, uint16_t aKey, const uint8_t *aValue, uint16_t aValueLength)
{
    OT_UNUSED_VARIABLE(aInstance);

    otError error  = OT_ERROR_NONE;
    off_t   size   = lseek(sSettingsFd, 0, SEEK_END);
    int     swapFd = swapOpen(aInstance);

#if OPENTHREAD_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
    if (isCriticalKey(aKey))
    {
        ExitNow(error = otPosixSecureSettingsAdd(aInstance, aKey, aValue, aValueLength));
    }
#endif

    if (size > 0)
    {
        VerifyOrDie(0 == lseek(sSettingsFd, 0, SEEK_SET), OT_EXIT_ERROR_ERRNO);
        swapWrite(aInstance, swapFd, static_cast<uint16_t>(size));
    }

    VerifyOrDie(write(swapFd, &aKey, sizeof(aKey)) == sizeof(aKey) &&
                    write(swapFd, &aValueLength, sizeof(aValueLength)) == sizeof(aValueLength) &&
                    write(swapFd, aValue, aValueLength) == aValueLength,
                OT_EXIT_FAILURE);

    swapPersist(aInstance, swapFd);

#if OPENTHREAD_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
exit:
#endif
    return error;
}

otError otPlatSettingsDelete(otInstance *aInstance, uint16_t aKey, int aIndex)
{
    otError error;

#if OPENTHREAD_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
    if (isCriticalKey(aKey))
    {
        error = otPosixSecureSettingsDelete(aInstance, aKey, aIndex);
    }
    else
#endif
    {
        error = platformSettingsDelete(aInstance, aKey, aIndex, nullptr);
    }

    return error;
}

/**
 * This function removes a setting either from swap file or persisted file.
 *
 * @param[in]  aInstance  The OpenThread instance structure.
 * @param[in]  aKey       The key associated with the requested setting.
 * @param[in]  aIndex     The index of the value to be removed. If set to -1, all values for this aKey will be removed.
 * @param[out] aSwapFd    A optional pointer to receive file descriptor of the generated swap file descriptor.
 *
 * @note
 *   If @p aSwapFd is null, operate deleting on the setting file.
 *   If @p aSwapFd is not null, operate on the swap file, and aSwapFd will point to the swap file descriptor.
 *
 * @retval OT_ERROR_NONE        The given key and index was found and removed successfully.
 * @retval OT_ERROR_NOT_FOUND   The given key or index was not found in the setting store.
 *
 */
static otError platformSettingsDelete(otInstance *aInstance, uint16_t aKey, int aIndex, int *aSwapFd)
{
    OT_UNUSED_VARIABLE(aInstance);

    otError error  = OT_ERROR_NOT_FOUND;
    off_t   size   = lseek(sSettingsFd, 0, SEEK_END);
    off_t   offset = lseek(sSettingsFd, 0, SEEK_SET);
    int     swapFd = swapOpen(aInstance);

    assert(swapFd != -1);
    assert(offset == 0);
    VerifyOrExit(offset == 0 && size >= 0, error = OT_ERROR_PARSE);

    while (offset < size)
    {
        uint16_t key;
        uint16_t length;
        ssize_t  rval;

        rval = read(sSettingsFd, &key, sizeof(key));
        VerifyOrExit(rval == sizeof(key), error = OT_ERROR_PARSE);

        rval = read(sSettingsFd, &length, sizeof(length));
        VerifyOrExit(rval == sizeof(length), error = OT_ERROR_PARSE);

        offset += sizeof(key) + sizeof(length) + length;

        if (aKey == key)
        {
            if (aIndex == 0)
            {
                VerifyOrExit(offset == lseek(sSettingsFd, length, SEEK_CUR), error = OT_ERROR_PARSE);
                swapWrite(aInstance, swapFd, static_cast<uint16_t>(size - offset));
                error = OT_ERROR_NONE;
                break;
            }
            else if (aIndex == -1)
            {
                VerifyOrExit(offset == lseek(sSettingsFd, length, SEEK_CUR), error = OT_ERROR_PARSE);
                error = OT_ERROR_NONE;
                continue;
            }
            else
            {
                --aIndex;
            }
        }

        rval = write(swapFd, &key, sizeof(key));
        assert(rval == sizeof(key));
        VerifyOrDie(rval == sizeof(key), OT_EXIT_FAILURE);

        rval = write(swapFd, &length, sizeof(length));
        assert(rval == sizeof(length));
        VerifyOrDie(rval == sizeof(length), OT_EXIT_FAILURE);

        swapWrite(aInstance, swapFd, length);
    }

exit:
    VerifyOrDie(error != OT_ERROR_PARSE, OT_EXIT_FAILURE);

    if (aSwapFd != nullptr)
    {
        *aSwapFd = swapFd;
    }
    else if (error == OT_ERROR_NONE)
    {
        swapPersist(aInstance, swapFd);
    }
    else if (error == OT_ERROR_NOT_FOUND)
    {
        swapDiscard(aInstance, swapFd);
    }

    return error;
}

void otPlatSettingsWipe(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
#if OPENTHREAD_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
    otPosixSecureSettingsWipe(aInstance);
#endif

    VerifyOrDie(0 == ftruncate(sSettingsFd, 0), OT_EXIT_ERROR_ERRNO);
}

#ifndef SELF_TEST
#define SELF_TEST 0
#endif

#if SELF_TEST

void otPlatRadioGetIeeeEui64(otInstance *aInstance, uint8_t *aIeeeEui64)
{
    OT_UNUSED_VARIABLE(aInstance);

    memset(aIeeeEui64, 0, sizeof(uint64_t));
}

int main()
{
    otInstance *instance = nullptr;
    uint8_t     data[60];

    for (uint8_t i = 0; i < sizeof(data); ++i)
    {
        data[i] = i;
    }

    otPlatSettingsInit(instance);

    // verify empty situation
    otPlatSettingsWipe(instance);
    {
        uint8_t  value[sizeof(data)];
        uint16_t length = sizeof(value);

        assert(otPlatSettingsGet(instance, 0, 0, value, &length) == OT_ERROR_NOT_FOUND);
        assert(otPlatSettingsDelete(instance, 0, 0) == OT_ERROR_NOT_FOUND);
        assert(otPlatSettingsDelete(instance, 0, -1) == OT_ERROR_NOT_FOUND);
    }

    // verify write one record
    assert(otPlatSettingsSet(instance, 0, data, sizeof(data) / 2) == OT_ERROR_NONE);
    {
        uint8_t  value[sizeof(data)];
        uint16_t length = sizeof(value);

        assert(otPlatSettingsGet(instance, 0, 0, nullptr, nullptr) == OT_ERROR_NONE);
        assert(otPlatSettingsGet(instance, 0, 0, nullptr, &length) == OT_ERROR_NONE);
        assert(length == sizeof(data) / 2);

        length = sizeof(value);
        assert(otPlatSettingsGet(instance, 0, 0, value, &length) == OT_ERROR_NONE);
        assert(length == sizeof(data) / 2);
        assert(0 == memcmp(value, data, length));

        // insufficient buffer
        length -= 1;
        value[length] = 0;
        assert(otPlatSettingsGet(instance, 0, 0, value, &length) == OT_ERROR_NONE);
        // verify length becomes the actual length of the record
        assert(length == sizeof(data) / 2);
        // verify this byte is not changed
        assert(value[length] == 0);

        // wrong index
        assert(otPlatSettingsGet(instance, 0, 1, nullptr, nullptr) == OT_ERROR_NOT_FOUND);
        // wrong key
        assert(otPlatSettingsGet(instance, 1, 0, nullptr, nullptr) == OT_ERROR_NOT_FOUND);
    }
    otPlatSettingsWipe(instance);

    // verify write two records
    assert(otPlatSettingsSet(instance, 0, data, sizeof(data)) == OT_ERROR_NONE);
    assert(otPlatSettingsAdd(instance, 0, data, sizeof(data) / 2) == OT_ERROR_NONE);
    {
        uint8_t  value[sizeof(data)];
        uint16_t length = sizeof(value);

        assert(otPlatSettingsGet(instance, 0, 1, value, &length) == OT_ERROR_NONE);
        assert(length == sizeof(data) / 2);
        assert(0 == memcmp(value, data, length));

        length = sizeof(value);
        assert(otPlatSettingsGet(instance, 0, 0, value, &length) == OT_ERROR_NONE);
        assert(length == sizeof(data));
        assert(0 == memcmp(value, data, length));
    }
    otPlatSettingsWipe(instance);

    // verify write two records of different keys
    assert(otPlatSettingsSet(instance, 0, data, sizeof(data)) == OT_ERROR_NONE);
    assert(otPlatSettingsAdd(instance, 1, data, sizeof(data) / 2) == OT_ERROR_NONE);
    {
        uint8_t  value[sizeof(data)];
        uint16_t length = sizeof(value);

        assert(otPlatSettingsGet(instance, 1, 0, value, &length) == OT_ERROR_NONE);
        assert(length == sizeof(data) / 2);
        assert(0 == memcmp(value, data, length));

        length = sizeof(value);
        assert(otPlatSettingsGet(instance, 0, 0, value, &length) == OT_ERROR_NONE);
        assert(length == sizeof(data));
        assert(0 == memcmp(value, data, length));
    }
    otPlatSettingsWipe(instance);

    // verify delete record
    assert(otPlatSettingsAdd(instance, 0, data, sizeof(data)) == OT_ERROR_NONE);
    assert(otPlatSettingsAdd(instance, 0, data, sizeof(data) / 2) == OT_ERROR_NONE);
    assert(otPlatSettingsAdd(instance, 0, data, sizeof(data) / 3) == OT_ERROR_NONE);
    {
        uint8_t  value[sizeof(data)];
        uint16_t length = sizeof(value);

        // wrong key
        assert(otPlatSettingsDelete(instance, 1, 0) == OT_ERROR_NOT_FOUND);
        assert(otPlatSettingsDelete(instance, 1, -1) == OT_ERROR_NOT_FOUND);

        // wrong index
        assert(otPlatSettingsDelete(instance, 0, 3) == OT_ERROR_NOT_FOUND);

        // delete one record
        assert(otPlatSettingsDelete(instance, 0, 1) == OT_ERROR_NONE);
        assert(otPlatSettingsGet(instance, 0, 1, value, &length) == OT_ERROR_NONE);
        assert(length == sizeof(data) / 3);
        assert(0 == memcmp(value, data, length));

        // delete all records
        assert(otPlatSettingsDelete(instance, 0, -1) == OT_ERROR_NONE);
        assert(otPlatSettingsGet(instance, 0, 0, nullptr, nullptr) == OT_ERROR_NOT_FOUND);
    }
    otPlatSettingsWipe(instance);

    // verify delete all records of a type
    assert(otPlatSettingsAdd(instance, 0, data, sizeof(data)) == OT_ERROR_NONE);
    assert(otPlatSettingsAdd(instance, 1, data, sizeof(data) / 2) == OT_ERROR_NONE);
    assert(otPlatSettingsAdd(instance, 0, data, sizeof(data) / 3) == OT_ERROR_NONE);
    {
        uint8_t  value[sizeof(data)];
        uint16_t length = sizeof(value);

        assert(otPlatSettingsDelete(instance, 0, -1) == OT_ERROR_NONE);
        assert(otPlatSettingsGet(instance, 0, 0, value, &length) == OT_ERROR_NOT_FOUND);
        assert(otPlatSettingsGet(instance, 1, 0, value, &length) == OT_ERROR_NONE);
        assert(length == sizeof(data) / 2);
        assert(0 == memcmp(value, data, length));

        assert(otPlatSettingsDelete(instance, 0, 0) == OT_ERROR_NOT_FOUND);
        assert(otPlatSettingsGet(instance, 0, 0, nullptr, nullptr) == OT_ERROR_NOT_FOUND);
    }
    otPlatSettingsWipe(instance);
    otPlatSettingsDeinit(instance);

    return 0;
}
#endif
