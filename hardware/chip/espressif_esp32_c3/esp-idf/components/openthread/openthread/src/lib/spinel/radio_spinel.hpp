/*
 *  Copyright (c) 2018, The OpenThread Authors.
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
 *   This file includes definitions for the spinel based radio transceiver.
 */

#ifndef RADIO_SPINEL_HPP_
#define RADIO_SPINEL_HPP_

#include <openthread/platform/radio.h>

#include "openthread-spinel-config.h"
#include "spinel.h"
#include "spinel_interface.hpp"
#include "core/radio/max_power_table.hpp"
#include "ncp/ncp_config.h"

namespace ot {
namespace Spinel {

/**
 * The class for providing a OpenThread radio interface by talking with a radio-only
 * co-processor(RCP). The InterfaceType template parameter should provide the following
 * methods:
 *
 * class InterfaceType {
 *
 *    // This constructor initializes the object.
 *
 *    // @param[in] aCallback         Callback on frame received
 *    // @param[in] aCallbackContext  Callback context
 *    // @param[in] aFrameBuffer      A reference to a `RxFrameBuffer` object.
 *
 *    InterfaceType(Spinel::SpinelInterface::ReceiveFrameCallback aCallback,
 *                  void *                                        aCallbackContext,
 *                  Spinel::SpinelInterface::RxFrameBuffer &      aFrameBuffer);
 *
 *
 *    // This method encodes and sends a spinel frame to Radio Co-processor (RCP) over the socket.
 *
 *    // This is blocking call, i.e., if the socket is not writable, this method waits for it to become writable for
 *    // up to `kMaxWaitTime` interval.
 *
 *    // @param[in] aFrame     A pointer to buffer containing the spinel frame to send.
 *    // @param[in] aLength    The length (number of bytes) in the frame.
 *
 *    // @retval OT_ERROR_NONE     Successfully encoded and sent the spinel frame.
 *    // @retval OT_ERROR_NO_BUFS  Insufficient buffer space available to encode the frame.
 *    // @retval OT_ERROR_FAILED   Failed to send due to socket not becoming writable within `kMaxWaitTime`.
 *
 *    otError SendFrame(const uint8_t *aFrame, uint16_t aLength);
 *
 *
 *    // This method waits for receiving part or all of spinel frame within specified interval.
 *
 *    // @param[in]  aTimeout  The timeout value in microseconds.
 *
 *    // @retval OT_ERROR_NONE             Part or all of spinel frame is received.
 *    // @retval OT_ERROR_RESPONSE_TIMEOUT No spinel frame is received within @p aTimeout.
 *
 *    otError WaitForFrame(uint64_t& aTimeoutUs);
 *
 *
 *    // This method performs radio driver processing.
 *
 *    // @param[in]   aContext        The context containing fd_sets.
 *    //                              The type is specified by the user in template parameters.
 *
 *    void Process(const ProcessContextType &aContext);
 *
 *
 *    // This method deinitializes the interface to the RCP.
 *
 *    void Deinit(void);
 * };
 */
template <typename InterfaceType, typename ProcessContextType> class RadioSpinel
{
public:
    /**
     * This constructor initializes the spinel based OpenThread transceiver.
     *
     */
    RadioSpinel(void);

    /**
     * Initialize this radio transceiver.
     *
     * @param[in]  aResetRadio                 TRUE to reset on init, FALSE to not reset on init.
     * @param[in]  aRestoreDatasetFromNcp      TRUE to restore dataset to host from non-volatile memory
     *                                         (only used when attempts to upgrade from NCP to RCP mode),
     *                                         FALSE otherwise.
     * @param[in]  aSkipRcpCompatibilityCheck  TRUE to skip RCP compatibility check, FALSE to perform the check.
     *
     */
    void Init(bool aResetRadio, bool aRestoreDataSetFromNcp, bool aSkipRcpCompatibilityCheck);

    /**
     * Deinitialize this radio transceiver.
     *
     */
    void Deinit(void);

    /**
     * This method gets the status of promiscuous mode.
     *
     * @retval true   Promiscuous mode is enabled.
     * @retval false  Promiscuous mode is disabled.
     *
     */
    bool IsPromiscuous(void) const { return mIsPromiscuous; }

    /**
     * This method sets the status of promiscuous mode.
     *
     * @param[in]   aEnable     Whether to enable or disable promiscuous mode.
     *
     * @retval  OT_ERROR_NONE               Succeeded.
     * @retval  OT_ERROR_BUSY               Failed due to another operation is on going.
     * @retval  OT_ERROR_RESPONSE_TIMEOUT   Failed due to no response received from the transceiver.
     *
     */
    otError SetPromiscuous(bool aEnable);

    /**
     * This method sets the Short Address for address filtering.
     *
     * @param[in] aShortAddress  The IEEE 802.15.4 Short Address.
     *
     * @retval  OT_ERROR_NONE               Succeeded.
     * @retval  OT_ERROR_BUSY               Failed due to another operation is on going.
     * @retval  OT_ERROR_RESPONSE_TIMEOUT   Failed due to no response received from the transceiver.
     *
     */
    otError SetShortAddress(uint16_t aAddress);

    /**
     * This method gets the factory-assigned IEEE EUI-64 for this transceiver.
     *
     * @param[in]  aInstance   The OpenThread instance structure.
     * @param[out] aIeeeEui64  A pointer to the factory-assigned IEEE EUI-64.
     *
     * @retval  OT_ERROR_NONE               Succeeded.
     * @retval  OT_ERROR_BUSY               Failed due to another operation is on going.
     * @retval  OT_ERROR_RESPONSE_TIMEOUT   Failed due to no response received from the transceiver.
     *
     */
    otError GetIeeeEui64(uint8_t *aIeeeEui64);

    /**
     * This method sets the Extended Address for address filtering.
     *
     * @param[in] aExtAddress  A pointer to the IEEE 802.15.4 Extended Address stored in little-endian byte order.
     *
     * @retval  OT_ERROR_NONE               Succeeded.
     * @retval  OT_ERROR_BUSY               Failed due to another operation is on going.
     * @retval  OT_ERROR_RESPONSE_TIMEOUT   Failed due to no response received from the transceiver.
     *
     */
    otError SetExtendedAddress(const otExtAddress &aAddress);

    /**
     * This method sets the PAN ID for address filtering.
     *
     * @param[in]   aPanId  The IEEE 802.15.4 PAN ID.
     *
     * @retval  OT_ERROR_NONE               Succeeded.
     * @retval  OT_ERROR_BUSY               Failed due to another operation is on going.
     * @retval  OT_ERROR_RESPONSE_TIMEOUT   Failed due to no response received from the transceiver.
     *
     */
    otError SetPanId(uint16_t aPanId);

    /**
     * This method gets the radio's transmit power in dBm.
     *
     * @param[out]  aPower    The transmit power in dBm.
     *
     * @retval  OT_ERROR_NONE               Succeeded.
     * @retval  OT_ERROR_BUSY               Failed due to another operation is on going.
     * @retval  OT_ERROR_RESPONSE_TIMEOUT   Failed due to no response received from the transceiver.
     *
     */
    otError GetTransmitPower(int8_t &aPower);

    /**
     * This method sets the radio's transmit power in dBm.
     *
     * @param[in]   aPower     The transmit power in dBm.
     *
     * @retval  OT_ERROR_NONE               Succeeded.
     * @retval  OT_ERROR_BUSY               Failed due to another operation is on going.
     * @retval  OT_ERROR_RESPONSE_TIMEOUT   Failed due to no response received from the transceiver.
     *
     */
    otError SetTransmitPower(int8_t aPower);

    /**
     * This method gets the radio's CCA ED threshold in dBm.
     *
     * @param[out]  aThreshold    The CCA ED threshold in dBm.
     *
     * @retval  OT_ERROR_NONE               Succeeded.
     * @retval  OT_ERROR_BUSY               Failed due to another operation is on going.
     * @retval  OT_ERROR_RESPONSE_TIMEOUT   Failed due to no response received from the transceiver.
     *
     */
    otError GetCcaEnergyDetectThreshold(int8_t &aThreshold);

    /**
     * This method sets the radio's CCA ED threshold in dBm.
     *
     * @param[in]   aThreshold     The CCA ED threshold in dBm.
     *
     * @retval  OT_ERROR_NONE               Succeeded.
     * @retval  OT_ERROR_BUSY               Failed due to another operation is on going.
     * @retval  OT_ERROR_RESPONSE_TIMEOUT   Failed due to no response received from the transceiver.
     *
     */
    otError SetCcaEnergyDetectThreshold(int8_t aThreshold);

    /**
     * This method gets the FEM's Rx LNA gain in dBm.
     *
     * @param[out]  aGain    The FEM's Rx LNA gain in dBm.
     *
     * @retval  OT_ERROR_NONE               Succeeded.
     * @retval  OT_ERROR_BUSY               Failed due to another operation is on going.
     * @retval  OT_ERROR_RESPONSE_TIMEOUT   Failed due to no response received from the transceiver.
     *
     */
    otError GetFemLnaGain(int8_t &aGain);

    /**
     * This method sets the FEM's Rx LNA gain in dBm.
     *
     * @param[in]   aGain     The FEM's Rx LNA gain in dBm.
     *
     * @retval  OT_ERROR_NONE               Succeeded.
     * @retval  OT_ERROR_BUSY               Failed due to another operation is on going.
     * @retval  OT_ERROR_RESPONSE_TIMEOUT   Failed due to no response received from the transceiver.
     *
     */
    otError SetFemLnaGain(int8_t aGain);

    /**
     * This method returns the radio sw version string.
     *
     * @returns A pointer to the radio version string.
     *
     */
    const char *GetVersion(void) const { return mVersion; }

    /**
     * This method returns the radio capabilities.
     *
     * @returns The radio capability bit vector.
     *
     */
    otRadioCaps GetRadioCaps(void) const { return mRadioCaps; }

    /**
     * This method gets the most recent RSSI measurement.
     *
     * @returns The RSSI in dBm when it is valid.  127 when RSSI is invalid.
     */
    int8_t GetRssi(void);

    /**
     * This method returns the radio receive sensitivity value.
     *
     * @returns The radio receive sensitivity value in dBm.
     *
     * @retval  OT_ERROR_NONE               Succeeded.
     * @retval  OT_ERROR_BUSY               Failed due to another operation is on going.
     * @retval  OT_ERROR_RESPONSE_TIMEOUT   Failed due to no response received from the transceiver.
     *
     */
    int8_t GetReceiveSensitivity(void) const { return mRxSensitivity; }

    /**
     * This method gets current state of the radio.
     *
     * @return  Current state of the radio.
     *
     */
    otRadioState GetState(void) const;

    /**
     * This method gets the current receiving channel.
     *
     * @returns Current receiving channel.
     *
     */
    uint8_t GetChannel(void) const { return mChannel; }

#if OPENTHREAD_CONFIG_PLATFORM_RADIO_COEX_ENABLE
    /**
     * Enable the radio coex.
     *
     * @param[in] aInstance  The OpenThread instance structure.
     * @param[in] aEnabled   TRUE to enable the radio coex, FALSE otherwise.
     *
     * @retval OT_ERROR_NONE     Successfully enabled.
     * @retval OT_ERROR_FAILED   The radio coex could not be enabled.
     *
     */
    otError SetCoexEnabled(bool aEnabled);

    /**
     * Check whether radio coex is enabled or not.
     *
     * @param[in] aInstance  The OpenThread instance structure.
     *
     * @returns TRUE if the radio coex is enabled, FALSE otherwise.
     *
     */
    bool IsCoexEnabled(void);

    /**
     * This method retrieves the radio coexistence metrics.
     *
     * @param[out] aCoexMetrics  A reference to the coexistence metrics structure.
     *
     * @retval OT_ERROR_NONE          Successfully retrieved the coex metrics.
     * @retval OT_ERROR_INVALID_ARGS  @p aCoexMetrics was nullptr.
     *
     */
    otError GetCoexMetrics(otRadioCoexMetrics &aCoexMetrics);
#endif // OPENTHREAD_CONFIG_PLATFORM_RADIO_COEX_ENABLE

    /**
     * This method returns a reference to the transmit buffer.
     *
     * The caller forms the IEEE 802.15.4 frame in this buffer then calls otPlatRadioTransmit() to request transmission.
     *
     * @returns A reference to the transmit buffer.
     *
     */
    otRadioFrame &GetTransmitFrame(void) { return mTxRadioFrame; }

    /**
     * This method enables or disables source address match feature.
     *
     * @param[in]  aEnable     Enable/disable source address match feature.
     *
     * @retval  OT_ERROR_NONE               Succeeded.
     * @retval  OT_ERROR_BUSY               Failed due to another operation is on going.
     * @retval  OT_ERROR_RESPONSE_TIMEOUT   Failed due to no response received from the transceiver.
     *
     */
    otError EnableSrcMatch(bool aEnable);

    /**
     * This method adds a short address to the source address match table.
     *
     * @param[in]  aInstance      The OpenThread instance structure.
     * @param[in]  aShortAddress  The short address to be added.
     *
     * @retval  OT_ERROR_NONE               Successfully added short address to the source match table.
     * @retval  OT_ERROR_BUSY               Failed due to another operation is on going.
     * @retval  OT_ERROR_RESPONSE_TIMEOUT   Failed due to no response received from the transceiver.
     * @retval  OT_ERROR_NO_BUFS            No available entry in the source match table.
     */
    otError AddSrcMatchShortEntry(uint16_t aShortAddress);

    /**
     * This method removes a short address from the source address match table.
     *
     * @param[in]  aInstance      The OpenThread instance structure.
     * @param[in]  aShortAddress  The short address to be removed.
     *
     * @retval  OT_ERROR_NONE               Successfully removed short address from the source match table.
     * @retval  OT_ERROR_BUSY               Failed due to another operation is on going.
     * @retval  OT_ERROR_RESPONSE_TIMEOUT   Failed due to no response received from the transceiver.
     * @retval  OT_ERROR_NO_ADDRESS         The short address is not in source address match table.
     */
    otError ClearSrcMatchShortEntry(uint16_t aShortAddress);

    /**
     * Clear all short addresses from the source address match table.
     *
     * @param[in]  aInstance   The OpenThread instance structure.
     *
     * @retval  OT_ERROR_NONE               Succeeded.
     * @retval  OT_ERROR_BUSY               Failed due to another operation is on going.
     * @retval  OT_ERROR_RESPONSE_TIMEOUT   Failed due to no response received from the transceiver.
     *
     */
    otError ClearSrcMatchShortEntries(void);

    /**
     * Add an extended address to the source address match table.
     *
     * @param[in]  aInstance    The OpenThread instance structure.
     * @param[in]  aExtAddress  The extended address to be added stored in little-endian byte order.
     *
     * @retval  OT_ERROR_NONE               Successfully added extended address to the source match table.
     * @retval  OT_ERROR_BUSY               Failed due to another operation is on going.
     * @retval  OT_ERROR_RESPONSE_TIMEOUT   Failed due to no response received from the transceiver.
     * @retval  OT_ERROR_NO_BUFS            No available entry in the source match table.
     */
    otError AddSrcMatchExtEntry(const otExtAddress &aExtAddress);

    /**
     * Remove an extended address from the source address match table.
     *
     * @param[in]  aInstance    The OpenThread instance structure.
     * @param[in]  aExtAddress  The extended address to be removed stored in little-endian byte order.
     *
     * @retval  OT_ERROR_NONE               Successfully removed the extended address from the source match table.
     * @retval  OT_ERROR_BUSY               Failed due to another operation is on going.
     * @retval  OT_ERROR_RESPONSE_TIMEOUT   Failed due to no response received from the transceiver.
     * @retval  OT_ERROR_NO_ADDRESS         The extended address is not in source address match table.
     */
    otError ClearSrcMatchExtEntry(const otExtAddress &aExtAddress);

    /**
     * Clear all the extended/long addresses from source address match table.
     *
     * @param[in]  aInstance   The OpenThread instance structure.
     *
     * @retval  OT_ERROR_NONE               Succeeded.
     * @retval  OT_ERROR_BUSY               Failed due to another operation is on going.
     * @retval  OT_ERROR_RESPONSE_TIMEOUT   Failed due to no response received from the transceiver.
     *
     */
    otError ClearSrcMatchExtEntries(void);

    /**
     * This method begins the energy scan sequence on the radio.
     *
     * @param[in]  aScanChannel     The channel to perform the energy scan on.
     * @param[in]  aScanDuration    The duration, in milliseconds, for the channel to be scanned.
     *
     * @retval  OT_ERROR_NONE               Succeeded.
     * @retval  OT_ERROR_BUSY               Failed due to another operation is on going.
     * @retval  OT_ERROR_RESPONSE_TIMEOUT   Failed due to no response received from the transceiver.
     *
     */
    otError EnergyScan(uint8_t aScanChannel, uint16_t aScanDuration);

    /**
     * This method switches the radio state from Receive to Transmit.
     *
     * @param[in] aFrame     A reference to the transmitted frame.
     *
     * @retval  OT_ERROR_NONE               Successfully transitioned to Transmit.
     * @retval  OT_ERROR_BUSY               Failed due to another transmission is on going.
     * @retval  OT_ERROR_RESPONSE_TIMEOUT   Failed due to no response received from the transceiver.
     * @retval  OT_ERROR_INVALID_STATE      The radio was not in the Receive state.
     */
    otError Transmit(otRadioFrame &aFrame);

    /**
     * This method switches the radio state from Sleep to Receive.
     *
     * @param[in]  aChannel   The channel to use for receiving.
     *
     * @retval OT_ERROR_NONE          Successfully transitioned to Receive.
     * @retval OT_ERROR_INVALID_STATE The radio was disabled or transmitting.
     *
     */
    otError Receive(uint8_t aChannel);

    /**
     * This method switches the radio state from Receive to Sleep.
     *
     * @retval OT_ERROR_NONE          Successfully transitioned to Sleep.
     * @retval OT_ERROR_BUSY          The radio was transmitting
     * @retval OT_ERROR_INVALID_STATE The radio was disabled
     *
     */
    otError Sleep(void);

    /**
     * Enable the radio.
     *
     * @param[in]   aInstance   A pointer to the OpenThread instance.
     *
     * @retval OT_ERROR_NONE     Successfully enabled.
     * @retval OT_ERROR_FAILED   The radio could not be enabled.
     *
     */
    otError Enable(otInstance *aInstance);

    /**
     * Disable the radio.
     *
     * @retval  OT_ERROR_NONE               Successfully transitioned to Disabled.
     * @retval  OT_ERROR_BUSY               Failed due to another operation is on going.
     * @retval  OT_ERROR_RESPONSE_TIMEOUT   Failed due to no response received from the transceiver.
     *
     */
    otError Disable(void);

    /**
     * This method checks whether radio is enabled or not.
     *
     * @returns TRUE if the radio is enabled, FALSE otherwise.
     *
     */
    bool IsEnabled(void) const { return mState != kStateDisabled; }

    /**
     * This method indicates whether there is a pending transmission.
     *
     * @retval TRUE  There is a pending transmission.
     * @retval FALSE There is no pending transmission.
     *
     */
    bool IsTransmitting(void) const { return mState == kStateTransmitting; }

    /**
     * This method indicates whether a transmit has just finished.
     *
     * @retval TRUE  The transmission is done.
     * @retval FALSE The transmission is not done.
     *
     */
    bool IsTransmitDone(void) const { return mState == kStateTransmitDone; }

    /**
     * This method returns the timeout timepoint for the pending transmission.
     *
     * @returns The timeout timepoint for the pending transmission.
     *
     */
    uint64_t GetTxRadioEndUs(void) const { return mTxRadioEndUs; }

    /**
     * This method processes any pending the I/O data.
     *
     * @param[in]  aContext   The process context.
     *
     */
    void Process(const ProcessContextType &aContext);

    /**
     * This method returns the underlying spinel interface.
     *
     * @returns The underlying spinel interface.
     *
     */
    InterfaceType &GetSpinelInterface(void) { return mSpinelInterface; }

#if OPENTHREAD_CONFIG_DIAG_ENABLE
    /**
     * This method enables/disables the factory diagnostics mode.
     *
     * @param[in]  aMode  TRUE to enable diagnostics mode, FALSE otherwise.
     *
     */
    void SetDiagEnabled(bool aMode) { mDiagMode = aMode; }

    /**
     * This method indicates whether or not factory diagnostics mode is enabled.
     *
     * @returns TRUE if factory diagnostics mode is enabled, FALSE otherwise.
     *
     */
    bool IsDiagEnabled(void) const { return mDiagMode; }

    /**
     * This method processes platform diagnostics commands.
     *
     * @param[in]   aString         A null-terminated input string.
     * @param[out]  aOutput         The diagnostics execution result.
     * @param[in]   aOutputMaxLen   The output buffer size.
     *
     * @retval  OT_ERROR_NONE               Succeeded.
     * @retval  OT_ERROR_BUSY               Failed due to another operation is on going.
     * @retval  OT_ERROR_RESPONSE_TIMEOUT   Failed due to no response received from the transceiver.
     *
     */
    otError PlatDiagProcess(const char *aString, char *aOutput, size_t aOutputMaxLen);
#endif

    /**
     * This method returns the radio channel mask.
     *
     * @param[in]   aPreferred  TRUE to get preferred channel mask, FALSE to get supported channel mask.
     *
     * @returns The radio channel mask according to @aPreferred:
     *   The radio supported channel mask that the device is allowed to be on.
     *   The radio preferred channel mask that the device prefers to form on.
     *
     */
    uint32_t GetRadioChannelMask(bool aPreferred);

    /**
     * This method processes a received Spinel frame.
     *
     * The newly received frame is available in `RxFrameBuffer` from `SpinelInterface::GetRxFrameBuffer()`.
     *
     */
    void HandleReceivedFrame(void);

    /**
     * This method sets MAC key and key index to RCP.
     *
     * @param[in] aKeyIdMode  The key ID mode.
     * @param[in] aKeyId      The key index.
     * @param[in] aPrevKey    Pointer to previous MAC key.
     * @param[in] aCurrKey    Pointer to current MAC key.
     * @param[in] aNextKey    Pointer to next MAC key.
     *
     * @retval  OT_ERROR_NONE               Succeeded.
     * @retval  OT_ERROR_INVALID_ARGS       One of the keys passed is invalid..
     * @retval  OT_ERROR_BUSY               Failed due to another operation is on going.
     * @retval  OT_ERROR_RESPONSE_TIMEOUT   Failed due to no response received from the transceiver.
     *
     */
    otError SetMacKey(uint8_t                 aKeyIdMode,
                      uint8_t                 aKeyId,
                      const otMacKeyMaterial *aPrevKey,
                      const otMacKeyMaterial *aCurrKey,
                      const otMacKeyMaterial *aNextKey);

    /**
     * This method sets the current MAC Frame Counter value.
     *
     * @param[in]   aMacFrameCounter  The MAC Frame Counter value.
     *
     */
    otError SetMacFrameCounter(uint32_t aMacFrameCounter);

    /**
     * This method sets the radio region code.
     *
     * @param[in]   aRegionCode  The radio region code.
     *
     * @retval  OT_ERROR_NONE             Successfully set region code.
     * @retval  OT_ERROR_FAILED           Other platform specific errors.
     *
     */
    otError SetRadioRegion(uint16_t aRegionCode);

    /**
     * This method gets the radio region code.
     *
     * @param[out]   aRegionCode  The radio region code.
     *
     * @retval  OT_ERROR_INVALID_ARGS     @p aRegionCode is nullptr.
     * @retval  OT_ERROR_NONE             Successfully got region code.
     * @retval  OT_ERROR_FAILED           Other platform specific errors.
     *
     */
    otError GetRadioRegion(uint16_t *aRegionCode);

    /**
     * This method checks whether the spinel interface is radio-only.
     *
     * @param[out] aSupportsRcpApiVersion   A reference to a boolean variable to update whether the list of spinel
     *                                      capabilities include `SPINEL_CAP_RCP_API_VERSION`.
     *
     * @retval  TRUE    The radio chip is in radio-only mode.
     * @retval  FALSE   Otherwise.
     *
     */
    bool IsRcp(bool &aSupportsRcpApiVersion);

    /**
     * This method checks whether there is pending frame in the buffer.
     *
     * @returns Whether there is pending frame in the buffer.
     *
     */
    bool HasPendingFrame(void) const { return mRxFrameBuffer.HasSavedFrame(); }

    /**
     * This method gets dataset from NCP radio and saves it.
     *
     * @retval  OT_ERROR_NONE               Successfully restore dataset.
     * @retval  OT_ERROR_BUSY               Failed due to another operation is on going.
     * @retval  OT_ERROR_RESPONSE_TIMEOUT   Failed due to no response received from the radio.
     * @retval  OT_ERROR_NOT_FOUND          Failed due to spinel property not supported in radio.
     * @retval  OT_ERROR_FAILED             Failed due to other reasons.
     */
    otError RestoreDatasetFromNcp(void);

    /**
     * This method returns the next timepoint to recalculate RCP time offset.
     *
     * @returns The timepoint to start the recalculation of RCP time offset.
     *
     */
    uint64_t GetNextRadioTimeRecalcStart(void) const { return mRadioTimeRecalcStart; }

    /**
     * This method gets the current estimated time on RCP.
     *
     * @returns The current estimated RCP time in microseconds.
     *
     */
    uint64_t GetNow(void);

    /**
     * This method returns the bus speed between the host and the radio.
     *
     * @returns   bus speed in bits/second.
     *
     */
    uint32_t GetBusSpeed(void) const;

    /**
     * This method sets the max transmit power.
     *
     * @param[in] aChannel    The radio channel.
     * @param[in] aPower      The max transmit power in dBm.
     *
     * @retval  OT_ERROR_NONE           Successfully set the max transmit power.
     * @retval  OT_ERROR_INVALID_ARGS   Channel is not in valid range.
     *
     */
    otError SetChannelMaxTransmitPower(uint8_t aChannel, int8_t aPower);

private:
    enum
    {
        kMaxSpinelFrame        = SpinelInterface::kMaxFrameSize,
        kMaxWaitTime           = 2000, ///< Max time to wait for response in milliseconds.
        kVersionStringSize     = 128,  ///< Max size of version string.
        kCapsBufferSize        = 100,  ///< Max buffer size used to store `SPINEL_PROP_CAPS` value.
        kChannelMaskBufferSize = 32,   ///< Max buffer size used to store `SPINEL_PROP_PHY_CHAN_SUPPORTED` value.
    };

    enum State
    {
        kStateDisabled,     ///< Radio is disabled.
        kStateSleep,        ///< Radio is sleep.
        kStateReceive,      ///< Radio is in receive mode.
        kStateTransmitting, ///< Frame passed to radio for transmission, waiting for done event from radio.
        kStateTransmitDone, ///< Radio indicated frame transmission is done.
    };

    typedef otError (RadioSpinel::*ResponseHandler)(const uint8_t *aBuffer, uint16_t aLength);

    static void HandleReceivedFrame(void *aContext);

    otError CheckSpinelVersion(void);
    otError CheckRadioCapabilities(void);
    otError CheckRcpApiVersion(bool aSupportsRcpApiVersion);

    /**
     * This method triggers a state transfer of the state machine.
     *
     */
    void ProcessRadioStateMachine(void);

    /**
     * This method processes the frame queue.
     *
     */
    void ProcessFrameQueue(void);

    /**
     * This method tries to retrieve a spinel property from OpenThread transceiver.
     *
     * @param[in]   aKey        Spinel property key.
     * @param[in]   aFormat     Spinel formatter to unpack property value.
     * @param[out]  ...         Variable arguments list.
     *
     * @retval  OT_ERROR_NONE               Successfully got the property.
     * @retval  OT_ERROR_BUSY               Failed due to another operation is on going.
     * @retval  OT_ERROR_RESPONSE_TIMEOUT   Failed due to no response received from the transceiver.
     *
     */
    otError Get(spinel_prop_key_t aKey, const char *aFormat, ...);

    /**
     * This method tries to retrieve a spinel property from OpenThread transceiver with parameter appended.
     *
     * @param[in]   aKey        Spinel property key.
     * @param[in]   aParam      Parameter appended to spinel command.
     * @param[in]   aParamSize  Size of parameter appended to spinel command
     * @param[in]   aFormat     Spinel formatter to unpack property value.
     * @param[out]  ...         Variable arguments list.
     *
     * @retval  OT_ERROR_NONE               Successfully got the property.
     * @retval  OT_ERROR_BUSY               Failed due to another operation is on going.
     * @retval  OT_ERROR_RESPONSE_TIMEOUT   Failed due to no response received from the transceiver.
     *
     */
    otError GetWithParam(spinel_prop_key_t aKey,
                         const uint8_t *   aParam,
                         spinel_size_t     aParamSize,
                         const char *      aFormat,
                         ...);

    /**
     * This method tries to update a spinel property of OpenThread transceiver.
     *
     * @param[in]   aKey        Spinel property key.
     * @param[in]   aFormat     Spinel formatter to pack property value.
     * @param[in]   ...         Variable arguments list.
     *
     * @retval  OT_ERROR_NONE               Successfully set the property.
     * @retval  OT_ERROR_BUSY               Failed due to another operation is on going.
     * @retval  OT_ERROR_RESPONSE_TIMEOUT   Failed due to no response received from the transceiver.
     *
     */
    otError Set(spinel_prop_key_t aKey, const char *aFormat, ...);

    /**
     * This method tries to insert a item into a spinel list property of OpenThread transceiver.
     *
     * @param[in]   aKey        Spinel property key.
     * @param[in]   aFormat     Spinel formatter to pack the item.
     * @param[in]   ...         Variable arguments list.
     *
     * @retval  OT_ERROR_NONE               Successfully insert item into the property.
     * @retval  OT_ERROR_BUSY               Failed due to another operation is on going.
     * @retval  OT_ERROR_RESPONSE_TIMEOUT   Failed due to no response received from the transceiver.
     *
     */
    otError Insert(spinel_prop_key_t aKey, const char *aFormat, ...);

    /**
     * This method tries to remove a item from a spinel list property of OpenThread transceiver.
     *
     * @param[in]   aKey        Spinel property key.
     * @param[in]   aFormat     Spinel formatter to pack the item.
     * @param[in]   ...         Variable arguments list.
     *
     * @retval  OT_ERROR_NONE               Successfully removed item from the property.
     * @retval  OT_ERROR_BUSY               Failed due to another operation is on going.
     * @retval  OT_ERROR_RESPONSE_TIMEOUT   Failed due to no response received from the transceiver.
     *
     */
    otError Remove(spinel_prop_key_t aKey, const char *aFormat, ...);

    spinel_tid_t GetNextTid(void);
    void         FreeTid(spinel_tid_t tid) { mCmdTidsInUse &= ~(1 << tid); }

    otError RequestV(uint32_t aCommand, spinel_prop_key_t aKey, const char *aFormat, va_list aArgs);
    otError Request(uint32_t aCommand, spinel_prop_key_t aKey, const char *aFormat, ...);
    otError RequestWithPropertyFormat(const char *      aPropertyFormat,
                                      uint32_t          aCommand,
                                      spinel_prop_key_t aKey,
                                      const char *      aFormat,
                                      ...);
    otError RequestWithPropertyFormatV(const char *      aPropertyFormat,
                                       uint32_t          aCommand,
                                       spinel_prop_key_t aKey,
                                       const char *      aFormat,
                                       va_list           aArgs);
    otError RequestWithExpectedCommandV(uint32_t          aExpectedCommand,
                                        uint32_t          aCommand,
                                        spinel_prop_key_t aKey,
                                        const char *      aFormat,
                                        va_list           aArgs);
    otError WaitResponse(void);
    otError SendReset(void);
    otError SendCommand(uint32_t          command,
                        spinel_prop_key_t key,
                        spinel_tid_t      tid,
                        const char *      pack_format,
                        va_list           args);
    otError ParseRadioFrame(otRadioFrame &aFrame, const uint8_t *aBuffer, uint16_t aLength, spinel_ssize_t &aUnpacked);
    otError ThreadDatasetHandler(const uint8_t *aBuffer, uint16_t aLength);

    /**
     * This method returns if the property changed event is safe to be handled now.
     *
     * If a property handler will go up to core stack, it may cause reentrant issue of `Hdlc::Decode()` and
     * `WaitResponse()`.
     *
     * @param[in] aKey The identifier of the property.
     *
     * @returns Whether this property is safe to be handled now.
     *
     */
    bool IsSafeToHandleNow(spinel_prop_key_t aKey) const
    {
        return !(aKey == SPINEL_PROP_STREAM_RAW || aKey == SPINEL_PROP_MAC_ENERGY_SCAN_RESULT);
    }

    void HandleNotification(SpinelInterface::RxFrameBuffer &aFrameBuffer);
    void HandleNotification(const uint8_t *aBuffer, uint16_t aLength);
    void HandleValueIs(spinel_prop_key_t aKey, const uint8_t *aBuffer, uint16_t aLength);

    void HandleResponse(const uint8_t *aBuffer, uint16_t aLength);
    void HandleTransmitDone(uint32_t aCommand, spinel_prop_key_t aKey, const uint8_t *aBuffer, uint16_t aLength);
    void HandleWaitingResponse(uint32_t aCommand, spinel_prop_key_t aKey, const uint8_t *aBuffer, uint16_t aLength);

    void RadioReceive(void);

    void TransmitDone(otRadioFrame *aFrame, otRadioFrame *aAckFrame, otError aError);

    void CalcRcpTimeOffset(void);

    void HandleRcpUnexpectedReset(spinel_status_t aStatus);
    void HandleRcpTimeout(void);
    void RecoverFromRcpFailure(void);

#if OPENTHREAD_SPINEL_CONFIG_RCP_RESTORATION_MAX_COUNT > 0
    void RestoreProperties(void);
#endif

    otInstance *mInstance;

    SpinelInterface::RxFrameBuffer mRxFrameBuffer;

    InterfaceType mSpinelInterface;

    uint16_t          mCmdTidsInUse;    ///< Used transaction ids.
    spinel_tid_t      mCmdNextTid;      ///< Next available transaction id.
    spinel_tid_t      mTxRadioTid;      ///< The transaction id used to send a radio frame.
    spinel_tid_t      mWaitingTid;      ///< The transaction id of current transaction.
    spinel_prop_key_t mWaitingKey;      ///< The property key of current transaction.
    const char *      mPropertyFormat;  ///< The spinel property format of current transaction.
    va_list           mPropertyArgs;    ///< The arguments pack or unpack spinel property of current transaction.
    uint32_t          mExpectedCommand; ///< Expected response command of current transaction.
    otError           mError;           ///< The result of current transaction.

    uint8_t       mRxPsdu[OT_RADIO_FRAME_MAX_SIZE];
    uint8_t       mTxPsdu[OT_RADIO_FRAME_MAX_SIZE];
    uint8_t       mAckPsdu[OT_RADIO_FRAME_MAX_SIZE];
    otRadioFrame  mRxRadioFrame;
    otRadioFrame  mTxRadioFrame;
    otRadioFrame  mAckRadioFrame;
    otRadioFrame *mTransmitFrame; ///< Points to the frame to send

    otExtAddress mExtendedAddress;
    uint16_t     mShortAddress;
    uint16_t     mPanId;
    otRadioCaps  mRadioCaps;
    uint8_t      mChannel;
    int8_t       mRxSensitivity;
    otError      mTxError;
    char         mVersion[kVersionStringSize];
    otExtAddress mIeeeEui64;

    State mState;
    bool  mIsPromiscuous : 1;     ///< Promiscuous mode.
    bool  mIsReady : 1;           ///< NCP ready.
    bool  mSupportsLogStream : 1; ///< RCP supports `LOG_STREAM` property with OpenThread log meta-data format.
    bool  mIsTimeSynced : 1;      ///< Host has calculated the time difference between host and RCP.

#if OPENTHREAD_SPINEL_CONFIG_RCP_RESTORATION_MAX_COUNT > 0

    bool    mResetRadioOnStartup : 1; ///< Whether should send reset command when init.
    int16_t mRcpFailureCount;         ///< Count of consecutive RCP failures.

    // Properties set by core.
    uint8_t      mKeyIdMode;
    uint8_t      mKeyId;
    otMacKey     mPrevKey;
    otMacKey     mCurrKey;
    otMacKey     mNextKey;
    uint16_t     mSrcMatchShortEntries[OPENTHREAD_CONFIG_MLE_MAX_CHILDREN];
    int16_t      mSrcMatchShortEntryCount;
    otExtAddress mSrcMatchExtEntries[OPENTHREAD_CONFIG_MLE_MAX_CHILDREN];
    int16_t      mSrcMatchExtEntryCount;
    uint8_t      mScanChannel;
    uint16_t     mScanDuration;
    int8_t       mCcaEnergyDetectThreshold;
    int8_t       mTransmitPower;
    int8_t       mFemLnaGain;
    bool         mCoexEnabled : 1;

    bool mMacKeySet : 1;                   ///< Whether MAC key has been set.
    bool mCcaEnergyDetectThresholdSet : 1; ///< Whether CCA energy detect threshold has been set.
    bool mTransmitPowerSet : 1;            ///< Whether transmit power has been set.
    bool mCoexEnabledSet : 1;              ///< Whether coex enabled has been set.
    bool mFemLnaGainSet : 1;               ///< Whether FEM LNA gain has been set.
    bool mRcpFailed : 1;                   ///< RCP failure happened, should recover and retry operation.
    bool mEnergyScanning : 1;              ///< If fails while scanning, restarts scanning.

#endif // OPENTHREAD_SPINEL_CONFIG_RCP_RESTORATION_MAX_COUNT > 0

#if OPENTHREAD_CONFIG_DIAG_ENABLE
    bool   mDiagMode;
    char * mDiagOutput;
    size_t mDiagOutputMaxLen;
#endif

    uint64_t mTxRadioEndUs;
    uint64_t mRadioTimeRecalcStart; ///< When to recalculate RCP time offset.
    int64_t  mRadioTimeOffset;      ///< Time difference with estimated RCP time minus host time.

    MaxPowerTable mMaxPowerTable;
};

} // namespace Spinel
} // namespace ot

#include "radio_spinel_impl.hpp"

#endif // RADIO_SPINEL_HPP_
