/*
 *  Copyright (c) 2020, The OpenThread Authors.
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

#include "platform-simulation.h"

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <openthread/config.h>
#include <openthread/platform/flash.h>

#include "core/common/logging.hpp"
#include "lib/platform/exit_code.h"

static int sFlashFd = -1;

enum
{
    SWAP_SIZE = 2048,
    SWAP_NUM  = 2,
};

void otPlatFlashInit(otInstance *aInstance)
{
    const char *path = OPENTHREAD_CONFIG_POSIX_SETTINGS_PATH;
    char        fileName[sizeof(OPENTHREAD_CONFIG_POSIX_SETTINGS_PATH) + 32];
    struct stat st;
    bool        create = false;
    const char *offset = getenv("PORT_OFFSET");

    memset(&st, 0, sizeof(st));

    if (stat(path, &st) == -1)
    {
        mkdir(path, 0777);
    }

    if (offset == NULL)
    {
        offset = "0";
    }

    snprintf(fileName, sizeof(fileName), "%s/%s_%d.flash", path, offset, gNodeId);

    if (access(fileName, 0))
    {
        create = true;
    }

    sFlashFd = open(fileName, O_RDWR | O_CREAT | O_CLOEXEC, 0600);
    VerifyOrDie(sFlashFd >= 0, OT_EXIT_ERROR_ERRNO);

    lseek(sFlashFd, 0, SEEK_SET);

    if (create)
    {
        for (uint8_t index = 0; index < SWAP_NUM; index++)
        {
            otPlatFlashErase(aInstance, index);
        }
    }
}

uint32_t otPlatFlashGetSwapSize(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return SWAP_SIZE;
}

void otPlatFlashErase(otInstance *aInstance, uint8_t aSwapIndex)
{
    OT_UNUSED_VARIABLE(aInstance);

    uint8_t  buffer[SWAP_SIZE];
    uint32_t address;
    ssize_t  rval;

    assert((sFlashFd >= 0) && (aSwapIndex < SWAP_NUM));

    address = aSwapIndex ? SWAP_SIZE : 0;
    memset(buffer, 0xff, sizeof(buffer));

    rval = pwrite(sFlashFd, buffer, sizeof(buffer), (off_t)address);
    VerifyOrDie(rval == SWAP_SIZE, OT_EXIT_ERROR_ERRNO);
}

void otPlatFlashRead(otInstance *aInstance, uint8_t aSwapIndex, uint32_t aOffset, void *aData, uint32_t aSize)
{
    OT_UNUSED_VARIABLE(aInstance);

    uint32_t address;
    ssize_t  rval;

    assert((sFlashFd >= 0) && (aSwapIndex < SWAP_NUM) && (aSize <= SWAP_SIZE) && (aOffset <= (SWAP_SIZE - aSize)));

    address = aSwapIndex ? SWAP_SIZE : 0;

    rval = pread(sFlashFd, aData, aSize, (off_t)(address + aOffset));
    VerifyOrDie((uint32_t)rval == aSize, OT_EXIT_ERROR_ERRNO);
}

void otPlatFlashWrite(otInstance *aInstance, uint8_t aSwapIndex, uint32_t aOffset, const void *aData, uint32_t aSize)
{
    OT_UNUSED_VARIABLE(aInstance);

    uint32_t address;
    uint8_t  byte;
    ssize_t  rval;

    assert((sFlashFd >= 0) && (aSwapIndex < SWAP_NUM) && (aSize <= SWAP_SIZE) && (aOffset <= (SWAP_SIZE - aSize)));

    address = aSwapIndex ? SWAP_SIZE : 0;
    address += aOffset;

    for (uint32_t offset = 0; offset < aSize; offset++)
    {
        rval = pread(sFlashFd, &byte, sizeof(byte), (off_t)(address + offset));
        VerifyOrDie(rval == sizeof(byte), OT_EXIT_ERROR_ERRNO);

        // Use bitwise AND to emulate the behavior of flash memory
        byte &= ((uint8_t *)aData)[offset];

        rval = pwrite(sFlashFd, &byte, sizeof(byte), (off_t)(address + offset));
        VerifyOrDie(rval == sizeof(byte), OT_EXIT_ERROR_ERRNO);
    }
}
