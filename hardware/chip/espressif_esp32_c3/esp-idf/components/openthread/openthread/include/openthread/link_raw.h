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
 * @brief
 *   This file defines the raw OpenThread IEEE 802.15.4 Link Layer API.
 */

#ifndef OPENTHREAD_LINK_RAW_H_
#define OPENTHREAD_LINK_RAW_H_

#include <openthread/platform/radio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup api-link-raw
 *
 * @brief
 *   This module includes functions that control the raw link-layer configuration.
 *
 * @{
 *
 */

/**
 * This function pointer on receipt of a IEEE 802.15.4 frame.
 *
 * @param[in]  aInstance    A pointer to an OpenThread instance.
 * @param[in]  aFrame       A pointer to the received frame or NULL if the receive operation was aborted.
 * @param[in]  aError       OT_ERROR_NONE when successfully received a frame.
 *                          OT_ERROR_ABORT when reception was aborted and a frame was not received.
 *
 */
typedef void (*otLinkRawReceiveDone)(otInstance *aInstance, otRadioFrame *aFrame, otError aError);

/**
 * This function enables/disables the raw link-layer.
 *
 * @param[in] aInstance     A pointer to an OpenThread instance.
 * @param[in] aCallback     A pointer to a function called on receipt of a IEEE 802.15.4 frame. NULL to disable the
 * raw-link layer.
 *
 * @retval OT_ERROR_FAILED          The radio could not be enabled/disabled.
 * @retval OT_ERROR_INVALID_STATE   If the OpenThread IPv6 interface is already enabled.
 * @retval OT_ERROR_NONE            If the enable state was successfully set.
 *
 */
otError otLinkRawSetReceiveDone(otInstance *aInstance, otLinkRawReceiveDone aCallback);

/**
 * This function indicates whether or not the raw link-layer is enabled.
 *
 * @param[in] aInstance     A pointer to an OpenThread instance.
 *
 * @retval true     The raw link-layer is enabled.
 * @retval false    The raw link-layer is disabled.
 *
 */
bool otLinkRawIsEnabled(otInstance *aInstance);

/**
 * This function gets the status of promiscuous mode.
 *
 * @param[in] aInstance  A pointer to an OpenThread instance.
 *
 * @retval true     Promiscuous mode is enabled.
 * @retval false    Promiscuous mode is disabled.
 *
 */
bool otLinkRawGetPromiscuous(otInstance *aInstance);

/**
 * This function enables or disables promiscuous mode.
 *
 * @param[in]  aInstance    A pointer to an OpenThread instance.
 * @param[in]  aEnable      A value to enable or disable promiscuous mode.
 *
 * @retval OT_ERROR_NONE             If successful.
 * @retval OT_ERROR_INVALID_STATE    If the raw link-layer isn't enabled.
 *
 */
otError otLinkRawSetPromiscuous(otInstance *aInstance, bool aEnable);

/**
 * Set the Short Address for address filtering.
 *
 * @param[in] aInstance      A pointer to an OpenThread instance.
 * @param[in] aShortAddress  The IEEE 802.15.4 Short Address.
 *
 * @retval OT_ERROR_NONE             If successful.
 * @retval OT_ERROR_INVALID_STATE    If the raw link-layer isn't enabled.
 *
 */
otError otLinkRawSetShortAddress(otInstance *aInstance, uint16_t aShortAddress);

/**
 * Transition the radio from Receive to Sleep.
 * Turn off the radio.
 *
 * @param[in] aInstance  A pointer to an OpenThread instance.
 *
 * @retval OT_ERROR_NONE             Successfully transitioned to Sleep.
 * @retval OT_ERROR_BUSY             The radio was transmitting
 * @retval OT_ERROR_INVALID_STATE    The radio was disabled
 *
 */
otError otLinkRawSleep(otInstance *aInstance);

/**
 * Transitioning the radio from Sleep to Receive.
 * Turn on the radio.
 *
 * @param[in]  aInstance    A pointer to an OpenThread instance.
 *
 * @retval OT_ERROR_NONE             Successfully transitioned to Receive.
 * @retval OT_ERROR_INVALID_STATE    The radio was disabled or transmitting.
 *
 */
otError otLinkRawReceive(otInstance *aInstance);

/**
 * The radio transitions from Transmit to Receive.
 * This method returns a pointer to the transmit buffer.
 *
 * The caller forms the IEEE 802.15.4 frame in this buffer then calls otLinkRawTransmit()
 * to request transmission.
 *
 * @param[in]  aInstance    A pointer to an OpenThread instance.
 *
 * @returns A pointer to the transmit buffer or NULL if the raw link-layer isn't enabled.
 *
 */
otRadioFrame *otLinkRawGetTransmitBuffer(otInstance *aInstance);

/**
 * This function pointer on receipt of a IEEE 802.15.4 frame.
 *
 * @param[in]  aInstance        A pointer to an OpenThread instance.
 * @param[in]  aFrame           A pointer to the frame that was transmitted.
 * @param[in]  aAckFrame        A pointer to the ACK frame.
 * @param[in]  aError           OT_ERROR_NONE when the frame was transmitted.
 *                              OT_ERROR_NO_ACK when the frame was transmitted but no ACK was received
 *                              OT_ERROR_CHANNEL_ACCESS_FAILURE when the transmission could not take place
                                    due to activity on the channel.
 *                              OT_ERROR_ABORT when transmission was aborted for other reasons.
 *
 */
typedef void (*otLinkRawTransmitDone)(otInstance *  aInstance,
                                      otRadioFrame *aFrame,
                                      otRadioFrame *aAckFrame,
                                      otError       aError);

/**
 * This method begins the transmit sequence on the radio.
 *
 * The caller must form the IEEE 802.15.4 frame in the buffer provided by otLinkRawGetTransmitBuffer() before
 * requesting transmission.  The channel and transmit power are also included in the otRadioFrame structure.
 *
 * The transmit sequence consists of:
 * 1. Transitioning the radio to Transmit from Receive.
 * 2. Transmits the PSDU on the given channel and at the given transmit power.
 *
 * @param[in]  aInstance    A pointer to an OpenThread instance.
 * @param[in]  aCallback    A pointer to a function called on completion of the transmission.
 *
 * @retval OT_ERROR_NONE          Successfully transitioned to Transmit.
 * @retval OT_ERROR_INVALID_STATE The radio was not in the Receive state.
 *
 */
otError otLinkRawTransmit(otInstance *aInstance, otLinkRawTransmitDone aCallback);

/**
 * Get the most recent RSSI measurement.
 *
 * @param[in]  aInstance    A pointer to an OpenThread instance.
 *
 * @returns The RSSI in dBm when it is valid. 127 when RSSI is invalid.
 *
 */
int8_t otLinkRawGetRssi(otInstance *aInstance);

/**
 * Get the radio capabilities.
 *
 * @param[in]  aInstance    A pointer to an OpenThread instance.
 *
 * @returns The radio capability bit vector. The stack enables or disables some functions based on this value.
 *
 */
otRadioCaps otLinkRawGetCaps(otInstance *aInstance);

/**
 * This function pointer on receipt of a IEEE 802.15.4 frame.
 *
 * @param[in]  aInstance            A pointer to an OpenThread instance.
 * @param[in]  aEnergyScanMaxRssi   The maximum RSSI encountered on the scanned channel.
 *
 */
typedef void (*otLinkRawEnergyScanDone)(otInstance *aInstance, int8_t aEnergyScanMaxRssi);

/**
 * This method begins the energy scan sequence on the radio.
 *
 * @param[in]  aInstance        A pointer to an OpenThread instance.
 * @param[in]  aScanChannel     The channel to perform the energy scan on.
 * @param[in]  aScanDuration    The duration, in milliseconds, for the channel to be scanned.
 * @param[in]  aCallback        A pointer to a function called on completion of a scanned channel.
 *
 * @retval OT_ERROR_NONE             Successfully started scanning the channel.
 * @retval OT_ERROR_NOT_IMPLEMENTED  The radio doesn't support energy scanning.
 * @retval OT_ERROR_INVALID_STATE    If the raw link-layer isn't enabled.
 *
 */
otError otLinkRawEnergyScan(otInstance *            aInstance,
                            uint8_t                 aScanChannel,
                            uint16_t                aScanDuration,
                            otLinkRawEnergyScanDone aCallback);

/**
 * Enable/Disable source match for frame pending.
 *
 * @param[in]  aInstance    A pointer to an OpenThread instance.
 * @param[in]  aEnable      Enable/disable source match for frame pending.
 *
 * @retval OT_ERROR_NONE             If successful.
 * @retval OT_ERROR_INVALID_STATE    If the raw link-layer isn't enabled.
 *
 */
otError otLinkRawSrcMatchEnable(otInstance *aInstance, bool aEnable);

/**
 * Adding short address to the source match table.
 *
 * @param[in]  aInstance        A pointer to an OpenThread instance.
 * @param[in]  aShortAddress    The short address to be added.
 *
 * @retval OT_ERROR_NONE             Successfully added short address to the source match table.
 * @retval OT_ERROR_NO_BUFS          No available entry in the source match table.
 * @retval OT_ERROR_INVALID_STATE    If the raw link-layer isn't enabled.
 *
 */
otError otLinkRawSrcMatchAddShortEntry(otInstance *aInstance, uint16_t aShortAddress);

/**
 * Adding extended address to the source match table.
 *
 * @param[in]  aInstance        A pointer to an OpenThread instance.
 * @param[in]  aExtAddress      The extended address to be added.
 *
 * @retval OT_ERROR_NONE             Successfully added extended address to the source match table.
 * @retval OT_ERROR_NO_BUFS          No available entry in the source match table.
 * @retval OT_ERROR_INVALID_STATE    If the raw link-layer isn't enabled.
 *
 */
otError otLinkRawSrcMatchAddExtEntry(otInstance *aInstance, const otExtAddress *aExtAddress);

/**
 * Removing short address to the source match table.
 *
 * @param[in]  aInstance        A pointer to an OpenThread instance.
 * @param[in]  aShortAddress    The short address to be removed.
 *
 * @retval OT_ERROR_NONE             Successfully removed short address from the source match table.
 * @retval OT_ERROR_NO_ADDRESS       The short address is not in source match table.
 * @retval OT_ERROR_INVALID_STATE    If the raw link-layer isn't enabled.
 *
 */
otError otLinkRawSrcMatchClearShortEntry(otInstance *aInstance, uint16_t aShortAddress);

/**
 * Removing extended address to the source match table of the radio.
 *
 * @param[in]  aInstance        A pointer to an OpenThread instance.
 * @param[in]  aExtAddress      The extended address to be removed.
 *
 * @retval OT_ERROR_NONE             Successfully removed the extended address from the source match table.
 * @retval OT_ERROR_NO_ADDRESS       The extended address is not in source match table.
 * @retval OT_ERROR_INVALID_STATE    If the raw link-layer isn't enabled.
 *
 */
otError otLinkRawSrcMatchClearExtEntry(otInstance *aInstance, const otExtAddress *aExtAddress);

/**
 * Removing all the short addresses from the source match table.
 *
 * @param[in]  aInstance    A pointer to an OpenThread instance.
 *
 * @retval OT_ERROR_NONE             If successful.
 * @retval OT_ERROR_INVALID_STATE    If the raw link-layer isn't enabled.
 *
 */
otError otLinkRawSrcMatchClearShortEntries(otInstance *aInstance);

/**
 * Removing all the extended addresses from the source match table.
 *
 * @param[in]  aInstance    A pointer to an OpenThread instance.
 *
 * @retval OT_ERROR_NONE             If successful.
 * @retval OT_ERROR_INVALID_STATE    If the raw link-layer isn't enabled.
 *
 */
otError otLinkRawSrcMatchClearExtEntries(otInstance *aInstance);

/**
 * Update MAC keys and key index.
 *
 * @param[in]   aInstance    A pointer to an OpenThread instance.
 * @param[in]   aKeyIdMode   The key ID mode.
 * @param[in]   aKeyId       The key index.
 * @param[in]   aPrevKey     The previous MAC key.
 * @param[in]   aCurrKey     The current MAC key.
 * @param[in]   aNextKey     The next MAC key.
 *
 * @retval OT_ERROR_NONE             If successful.
 * @retval OT_ERROR_INVALID_STATE    If the raw link-layer isn't enabled.
 *
 */
otError otLinkRawSetMacKey(otInstance *    aInstance,
                           uint8_t         aKeyIdMode,
                           uint8_t         aKeyId,
                           const otMacKey *aPrevKey,
                           const otMacKey *aCurrKey,
                           const otMacKey *aNextKey);

/**
 * Sets the current MAC frame counter value.
 *
 * @param[in]   aInstance         A pointer to an OpenThread instance.
 * @param[in]   aMacFrameCounter  The MAC frame counter value.
 *
 * @retval OT_ERROR_NONE             If successful.
 * @retval OT_ERROR_INVALID_STATE    If the raw link-layer isn't enabled.
 *
 */
otError otLinkRawSetMacFrameCounter(otInstance *aInstance, uint32_t aMacFrameCounter);

/**
 * Get current platform time (64bits width) of the radio chip.
 *
 * @param[in]  aInstance    A pointer to an OpenThread instance.
 *
 * @returns The current radio time in microseconds.
 *
 */
uint64_t otLinkRawGetRadioTime(otInstance *aInstance);

/**
 * @}
 *
 */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OPENTHREAD_LINK_RAW_H_
