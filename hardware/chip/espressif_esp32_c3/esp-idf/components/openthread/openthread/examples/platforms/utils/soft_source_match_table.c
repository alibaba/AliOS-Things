/*
 *  Copyright (c) 2019, The OpenThread Authors.
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
 *   This file implements a software Source Match table, for radios that don't have
 *   such hardware acceleration. It supports only the single-instance build of
 *   OpenThread.
 *
 */

#include "utils/soft_source_match_table.h"

#include <stdlib.h>
#include <string.h>

#include "common/logging.hpp"
#include "utils/code_utils.h"

#if RADIO_CONFIG_SRC_MATCH_SHORT_ENTRY_NUM || RADIO_CONFIG_SRC_MATCH_EXT_ENTRY_NUM
static uint16_t sPanId = 0;

void utilsSoftSrcMatchSetPanId(uint16_t aPanId)
{
    sPanId = aPanId;
}
#endif // RADIO_CONFIG_SRC_MATCH_SHORT_ENTRY_NUM || RADIO_CONFIG_SRC_MATCH_EXT_ENTRY_NUM

#if RADIO_CONFIG_SRC_MATCH_SHORT_ENTRY_NUM
typedef struct srcMatchShortEntry
{
    uint16_t checksum;
    bool     allocated;
} sSrcMatchShortEntry;

static sSrcMatchShortEntry srcMatchShortEntry[RADIO_CONFIG_SRC_MATCH_SHORT_ENTRY_NUM];

int16_t utilsSoftSrcMatchShortFindEntry(uint16_t aShortAddress)
{
    int16_t  entry    = -1;
    uint16_t checksum = aShortAddress + sPanId;

    for (int16_t i = 0; i < RADIO_CONFIG_SRC_MATCH_SHORT_ENTRY_NUM; i++)
    {
        if (checksum == srcMatchShortEntry[i].checksum && srcMatchShortEntry[i].allocated)
        {
            entry = i;
            break;
        }
    }

    return entry;
}

static int16_t findSrcMatchShortAvailEntry(void)
{
    int16_t entry = -1;

    for (int16_t i = 0; i < RADIO_CONFIG_SRC_MATCH_SHORT_ENTRY_NUM; i++)
    {
        if (!srcMatchShortEntry[i].allocated)
        {
            entry = i;
            break;
        }
    }

    return entry;
}

static inline void addToSrcMatchShortIndirect(uint16_t entry, uint16_t aShortAddress)
{
    uint16_t checksum = aShortAddress + sPanId;

    srcMatchShortEntry[entry].checksum  = checksum;
    srcMatchShortEntry[entry].allocated = true;
}

static inline void removeFromSrcMatchShortIndirect(uint16_t entry)
{
    srcMatchShortEntry[entry].allocated = false;
    srcMatchShortEntry[entry].checksum  = 0;
}

otError otPlatRadioAddSrcMatchShortEntry(otInstance *aInstance, uint16_t aShortAddress)
{
    OT_UNUSED_VARIABLE(aInstance);

    otError error = OT_ERROR_NONE;
    int16_t entry = -1;

    entry = findSrcMatchShortAvailEntry();
    otLogDebgPlat("Add ShortAddr entry: %d", entry);

    otEXPECT_ACTION(entry >= 0 && entry < RADIO_CONFIG_SRC_MATCH_SHORT_ENTRY_NUM, error = OT_ERROR_NO_BUFS);

    addToSrcMatchShortIndirect((uint16_t)entry, aShortAddress);

exit:
    return error;
}

otError otPlatRadioClearSrcMatchShortEntry(otInstance *aInstance, uint16_t aShortAddress)
{
    OT_UNUSED_VARIABLE(aInstance);

    otError error = OT_ERROR_NONE;
    int16_t entry = -1;

    entry = utilsSoftSrcMatchShortFindEntry(aShortAddress);
    otLogDebgPlat("Clear ShortAddr entry: %d", entry);

    otEXPECT_ACTION(entry >= 0 && entry < RADIO_CONFIG_SRC_MATCH_SHORT_ENTRY_NUM, error = OT_ERROR_NO_ADDRESS);

    removeFromSrcMatchShortIndirect((uint16_t)entry);

exit:
    return error;
}

void otPlatRadioClearSrcMatchShortEntries(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    otLogDebgPlat("Clear ShortAddr entries", NULL);

    memset(srcMatchShortEntry, 0, sizeof(srcMatchShortEntry));
}
#endif // RADIO_CONFIG_SRC_MATCH_SHORT_ENTRY_NUM

#if RADIO_CONFIG_SRC_MATCH_EXT_ENTRY_NUM
typedef struct srcMatchExtEntry
{
    uint16_t checksum;
    bool     allocated;
} sSrcMatchExtEntry;

static sSrcMatchExtEntry srcMatchExtEntry[RADIO_CONFIG_SRC_MATCH_EXT_ENTRY_NUM];

int16_t utilsSoftSrcMatchExtFindEntry(const otExtAddress *aExtAddress)
{
    int16_t  entry    = -1;
    uint16_t checksum = sPanId;

    checksum += (uint16_t)aExtAddress->m8[0] | (uint16_t)(aExtAddress->m8[1] << 8);
    checksum += (uint16_t)aExtAddress->m8[2] | (uint16_t)(aExtAddress->m8[3] << 8);
    checksum += (uint16_t)aExtAddress->m8[4] | (uint16_t)(aExtAddress->m8[5] << 8);
    checksum += (uint16_t)aExtAddress->m8[6] | (uint16_t)(aExtAddress->m8[7] << 8);

    for (int16_t i = 0; i < RADIO_CONFIG_SRC_MATCH_EXT_ENTRY_NUM; i++)
    {
        if (checksum == srcMatchExtEntry[i].checksum && srcMatchExtEntry[i].allocated)
        {
            entry = i;
            break;
        }
    }

    return entry;
}

static int16_t findSrcMatchExtAvailEntry(void)
{
    int16_t entry = -1;

    for (int16_t i = 0; i < RADIO_CONFIG_SRC_MATCH_EXT_ENTRY_NUM; i++)
    {
        if (!srcMatchExtEntry[i].allocated)
        {
            entry = i;
            break;
        }
    }

    return entry;
}

static inline void addToSrcMatchExtIndirect(uint16_t entry, const otExtAddress *aExtAddress)
{
    uint16_t checksum = sPanId;

    checksum += (uint16_t)aExtAddress->m8[0] | (uint16_t)(aExtAddress->m8[1] << 8);
    checksum += (uint16_t)aExtAddress->m8[2] | (uint16_t)(aExtAddress->m8[3] << 8);
    checksum += (uint16_t)aExtAddress->m8[4] | (uint16_t)(aExtAddress->m8[5] << 8);
    checksum += (uint16_t)aExtAddress->m8[6] | (uint16_t)(aExtAddress->m8[7] << 8);

    srcMatchExtEntry[entry].checksum  = checksum;
    srcMatchExtEntry[entry].allocated = true;
}

static inline void removeFromSrcMatchExtIndirect(uint16_t entry)
{
    srcMatchExtEntry[entry].allocated = false;
    srcMatchExtEntry[entry].checksum  = 0;
}

otError otPlatRadioAddSrcMatchExtEntry(otInstance *aInstance, const otExtAddress *aExtAddress)
{
    OT_UNUSED_VARIABLE(aInstance);

    otError error = OT_ERROR_NONE;
    int16_t entry = -1;

    entry = findSrcMatchExtAvailEntry();
    otLogDebgPlat("Add ExtAddr entry: %d", entry);

    otEXPECT_ACTION(entry >= 0 && entry < RADIO_CONFIG_SRC_MATCH_EXT_ENTRY_NUM, error = OT_ERROR_NO_BUFS);

    addToSrcMatchExtIndirect((uint16_t)entry, aExtAddress);

exit:
    return error;
}

otError otPlatRadioClearSrcMatchExtEntry(otInstance *aInstance, const otExtAddress *aExtAddress)
{
    OT_UNUSED_VARIABLE(aInstance);

    otError error = OT_ERROR_NONE;
    int16_t entry = -1;

    entry = utilsSoftSrcMatchExtFindEntry(aExtAddress);
    otLogDebgPlat("Clear ExtAddr entry: %d", entry);

    otEXPECT_ACTION(entry >= 0 && entry < RADIO_CONFIG_SRC_MATCH_EXT_ENTRY_NUM, error = OT_ERROR_NO_ADDRESS);

    removeFromSrcMatchExtIndirect((uint16_t)entry);

exit:
    return error;
}

void otPlatRadioClearSrcMatchExtEntries(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    otLogDebgPlat("Clear ExtAddr entries", NULL);

    memset(srcMatchExtEntry, 0, sizeof(srcMatchExtEntry));
}
#endif // RADIO_CONFIG_SRC_MATCH_EXT_ENTRY_NUM
