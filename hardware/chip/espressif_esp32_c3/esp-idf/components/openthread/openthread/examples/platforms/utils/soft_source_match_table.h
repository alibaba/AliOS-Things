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
 * @brief
 *   This file defines the software source match table interfaces used by
 *   soft_source_match_table.c.
 */

#ifndef SOFT_SOURCE_MATCH_TABLE_H
#define SOFT_SOURCE_MATCH_TABLE_H

#include "openthread-core-config.h"
#include <openthread/platform/radio.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef RADIO_CONFIG_SRC_MATCH_SHORT_ENTRY_NUM
#define RADIO_CONFIG_SRC_MATCH_SHORT_ENTRY_NUM OPENTHREAD_CONFIG_MLE_MAX_CHILDREN
#endif

#ifndef RADIO_CONFIG_SRC_MATCH_EXT_ENTRY_NUM
#define RADIO_CONFIG_SRC_MATCH_EXT_ENTRY_NUM OPENTHREAD_CONFIG_MLE_MAX_CHILDREN
#endif

#if RADIO_CONFIG_SRC_MATCH_SHORT_ENTRY_NUM || RADIO_CONFIG_SRC_MATCH_EXT_ENTRY_NUM
void utilsSoftSrcMatchSetPanId(uint16_t aPanId);
#endif // RADIO_CONFIG_SRC_MATCH_SHORT_ENTRY_NUM || RADIO_CONFIG_SRC_MATCH_EXT_ENTRY_NUM

#if RADIO_CONFIG_SRC_MATCH_SHORT_ENTRY_NUM
int16_t utilsSoftSrcMatchShortFindEntry(uint16_t aShortAddress);
#endif // RADIO_CONFIG_SRC_MATCH_SHORT_ENTRY_NUM

#if RADIO_CONFIG_SRC_MATCH_EXT_ENTRY_NUM
int16_t utilsSoftSrcMatchExtFindEntry(const otExtAddress *aExtAddress);
#endif // RADIO_CONFIG_SRC_MATCH_EXT_ENTRY_NUM

#ifdef __cplusplus
} // extern "C"
#endif

#endif // SOFT_SOURCE_MATCH_TABLE_H
