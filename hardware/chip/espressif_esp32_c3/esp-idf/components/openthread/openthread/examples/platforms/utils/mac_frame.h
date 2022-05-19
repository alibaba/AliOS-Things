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
 *   This file defines the mac frame interface for OpenThread platform radio drivers.
 *
 */

#ifndef OPENTHREAD_UTILS_MAC_FRAME_H
#define OPENTHREAD_UTILS_MAC_FRAME_H

#include <openthread/platform/radio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This enumeration specifies the IEEE 802.15.4 Address type.
 *
 */
typedef enum
{
    OT_MAC_ADDRESS_TYPE_NONE,     ///< No address.
    OT_MAC_ADDRESS_TYPE_SHORT,    ///< IEEE 802.15.4 Short Address.
    OT_MAC_ADDRESS_TYPE_EXTENDED, ///< IEEE 802.15.4 Extended Address.
} otMacAddressType;

/**
 * This structure represents an IEEE 802.15.4 short or extended Address.
 *
 */
typedef struct otMacAddress
{
    union
    {
        otShortAddress mShortAddress; ///< The IEEE 802.15.4 Short Address.
        otExtAddress   mExtAddress;   ///< The IEEE 802.15.4 Extended Address.
    } mAddress;

    otMacAddressType mType; ///< The address type (short, extended, or none).
} otMacAddress;

/**
 * Check if @p aFrame is an Ack frame.
 *
 * @param[in]   aFrame          A pointer to the frame.
 *
 * @retval  true    It is an ACK frame.
 * @retval  false   It is not an ACK frame.
 *
 */
bool otMacFrameIsAck(const otRadioFrame *aFrame);

/**
 * Check if @p aFrame is a Data frame.
 *
 * @param[in]   aFrame          A pointer to the frame.
 *
 * @retval  true    It is a Data frame.
 * @retval  false   It is not a Data frame.
 *
 */
bool otMacFrameIsData(const otRadioFrame *aFrame);

/**
 * Check if @p aFrame is a Command frame.
 *
 * @param[in]   aFrame          A pointer to the frame.
 *
 * @retval  true    It is a Command frame.
 * @retval  false   It is not a Command frame.
 *
 */
bool otMacFrameIsCommand(const otRadioFrame *aFrame);

/**
 * Check if @p aFrame is a Data Request Command.
 *
 * @param[in]   aFrame          A pointer to the frame. For 802.15.4-2015 and above frame,
 *                              the frame should be already decrypted.
 *
 * @retval  true    It is a Data Request Command frame.
 * @retval  false   It is not a Data Request Command frame.
 *
 */
bool otMacFrameIsDataRequest(const otRadioFrame *aFrame);

/**
 * Check if @p aFrame requests ACK.
 *
 * @param[in]   aFrame          A pointer to the frame.
 *
 * @retval  true    It requests ACK.
 * @retval  false   It does not request ACK.
 *
 */
bool otMacFrameIsAckRequested(const otRadioFrame *aFrame);

/**
 * Check if @p aFrame matches the @p aPandId and @p aShortAddress or @p aExtAddress.
 *
 * @param[in]   aFrame          A pointer to the frame.
 * @param[in]   aPanId          The PAN id to match with.
 * @param[in]   aShortAddress   The short address to match with.
 * @param[in]   aExtAddress     The extended address to match with.
 *
 * @retval  true    It is a broadcast or matches with the PAN id and one of the addresses.
 * @retval  false   It doesn't match.
 *
 */
bool otMacFrameDoesAddrMatch(const otRadioFrame *aFrame,
                             otPanId             aPanId,
                             otShortAddress      aShortAddress,
                             const otExtAddress *aExtAddress);

/**
 * Get source MAC address.
 *
 * @param[in]   aFrame          A pointer to the frame.
 * @param[out]  aMacAddress     A pointer to MAC address.
 *
 * @retval  OT_ERROR_NONE   Successfully got the source MAC address.
 * @retval  OT_ERROR_PARSE  Failed to parse the source MAC address.
 *
 */
otError otMacFrameGetSrcAddr(const otRadioFrame *aFrame, otMacAddress *aMacAddress);

/**
 * Get destination MAC address.
 *
 * @param[in]   aFrame          A pointer to the frame.
 * @param[out]  aMacAddress     A pointer to MAC address.
 *
 * @retval  OT_ERROR_NONE   Successfully got the destination MAC address.
 * @retval  OT_ERROR_PARSE  Failed to parse the destination MAC address.
 *
 */
otError otMacFrameGetDstAddr(const otRadioFrame *aFrame, otMacAddress *aMacAddress);

/**
 * Get the sequence of @p aFrame.
 *
 * @param[in]   aFrame          A pointer to the frame.
 *
 * @returns The sequence of the frame.
 *
 */
uint8_t otMacFrameGetSequence(const otRadioFrame *aFrame);

/**
 * This function performs AES CCM on the frame which is going to be sent.
 *
 * @param[in]  aFrame       A pointer to the MAC frame buffer that is going to be sent.
 * @param[in]  aExtAddress  A pointer to the extended address, which will be used to generate nonce
 *                          for AES CCM computation.
 *
 */
void otMacFrameProcessTransmitAesCcm(otRadioFrame *aFrame, const otExtAddress *aExtAddress);

/**
 * Tell if the version of @p aFrame is 2015.
 *
 * @param[in]   aFrame          A pointer to the frame.
 *
 * @retval  true    It is a version 2015 frame.
 * @retval  false   It is not a version 2015 frame.
 *
 */
bool otMacFrameIsVersion2015(const otRadioFrame *aFrame);

/**
 * Generate Imm-Ack for @p aFrame.
 *
 * @param[in]    aFrame             A pointer to the frame.
 * @param[in]    aIsFramePending    Value of the ACK's frame pending bit.
 * @param[out]   aAckFrame          A pointer to the ack frame to be generated.
 *
 */
void otMacFrameGenerateImmAck(const otRadioFrame *aFrame, bool aIsFramePending, otRadioFrame *aAckFrame);

/**
 * Generate Enh-Ack for @p aFrame.
 *
 * @param[in]    aFrame             A pointer to the frame.
 * @param[in]    aIsFramePending    Value of the ACK's frame pending bit.
 * @param[in]    aIeData            A pointer to the IE data portion of the ACK to be sent.
 * @param[in]    aIeLength          The length of IE data portion of the ACK to be sent.
 * @param[out]   aAckFrame          A pointer to the ack frame to be generated.
 *
 * @retval  OT_ERROR_NONE           Successfully generated Enh Ack in @p aAckFrame.
 * @retval  OT_ERROR_PARSE          @p aFrame has incorrect format.
 *
 */
otError otMacFrameGenerateEnhAck(const otRadioFrame *aFrame,
                                 bool                aIsFramePending,
                                 const uint8_t *     aIeData,
                                 uint8_t             aIeLength,
                                 otRadioFrame *      aAckFrame);

/**
 * Set CSL IE content into the frame.
 *
 * @param[inout]    aFrame         A pointer to the frame to be modified.
 * @param[in]       aCslPeriod     CSL Period in CSL IE.
 * @param[in]       aCslPhase      CSL Phase in CSL IE.
 *
 */
void otMacFrameSetCslIe(otRadioFrame *aFrame, uint16_t aCslPeriod, uint16_t aCslPhase);

/**
 * Tell if the security of @p aFrame is enabled.
 *
 * @param[in]   aFrame          A pointer to the frame.
 *
 * @retval  true    The frame has security enabled.
 * @retval  false   The frame does not have security enabled.
 *
 */
bool otMacFrameIsSecurityEnabled(otRadioFrame *aFrame);

/**
 * Tell if the key ID mode of @p aFrame is 1.
 *
 * @param[in]   aFrame          A pointer to the frame.
 *
 * @retval  true    The frame key ID mode is 1.
 * @retval  false   The frame security is not enabled or key ID mode is not 1.
 *
 */
bool otMacFrameIsKeyIdMode1(otRadioFrame *aFrame);

/**
 * Get the key ID of @p aFrame.
 *
 * @param[in]   aFrame          A pointer to the frame.
 *
 * @returns The key ID of the frame with key ID mode 1. Returns 0 if failed.
 *
 */
uint8_t otMacFrameGetKeyId(otRadioFrame *aFrame);

/**
 * Set key ID to @p aFrame with key ID mode 1.
 *
 * @param[inout]    aFrame     A pointer to the frame to be modified.
 * @param[in]       aKeyId     Key ID to be set to the frame.
 *
 */
void otMacFrameSetKeyId(otRadioFrame *aFrame, uint8_t aKeyId);

/**
 * Get the frame counter of @p aFrame.
 *
 * @param[in]   aFrame          A pointer to the frame.
 *
 * @returns The frame counter of the frame. Returns UINT32_MAX if failed.
 *
 */
uint32_t otMacFrameGetFrameCounter(otRadioFrame *aFrame);

/**
 * Set frame counter to @p aFrame.
 *
 * @param[inout]    aFrame         A pointer to the frame to be modified.
 * @param[in]       aFrameCounter  Frame counter to be set to the frame.
 *
 */
void otMacFrameSetFrameCounter(otRadioFrame *aFrame, uint32_t aFrameCounter);

/**
 * Write CSL IE to a buffer (without setting IE value).
 *
 * @param[out]  aDest    A pointer to the output buffer.
 *
 * @returns  The total count of bytes (total length of CSL IE) written to the buffer.
 *
 */
uint8_t otMacFrameGenerateCslIeTemplate(uint8_t *aDest);

/**
 * Write Enh-ACK Probing IE (Vendor IE with THREAD OUI) to a buffer.
 *
 * @p aIeData could be `NULL`. If @p aIeData is `NULL`, this method generates the IE with the data unset. This allows
 * users to generate the pattern first and update value later. (For example, using `otMacFrameSetEnhAckProbingIe`)
 *
 * @param[out]  aDest          A pointer to the output buffer.
 * @param[in]   aIeData        A pointer to the Link Metrics data.
 * @param[in]   aIeDataLength  The length of Link Metrics data value. Should be `1` or `2`. (Per spec 4.11.3.4.4.6)
 *
 * @returns  The total count of bytes (total length of the Vendor IE) written to the buffer.
 *
 */
uint8_t otMacFrameGenerateEnhAckProbingIe(uint8_t *aDest, const uint8_t *aIeData, uint8_t aIeDataLength);

/**
 * Sets the data value of Enh-ACK Probing IE (Vendor IE with THREAD OUI) in a frame.
 *
 * If no Enh-ACK Probing IE is found in @p aFrame, nothing would be done.
 *
 * @param[in]  aFrame    The target frame that contains the IE. MUST NOT be `NULL`.
 * @param[in]  aData     A pointer to the data value. MUST NOT be `NULL`.
 * @param[in]  aDataLen  The length of @p aData.
 *
 */
void otMacFrameSetEnhAckProbingIe(otRadioFrame *aFrame, const uint8_t *aData, uint8_t aDataLen);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OPENTHREAD_UTILS_MAC_FRAME_H
