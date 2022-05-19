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
 *   This file defines the radio interface for OpenThread.
 *
 */

#ifndef OPENTHREAD_PLATFORM_RADIO_H_
#define OPENTHREAD_PLATFORM_RADIO_H_

#include <stdint.h>

#include <openthread/error.h>
#include <openthread/instance.h>
#include <openthread/platform/crypto.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup plat-radio
 *
 * @brief
 *   This module includes the platform abstraction for radio communication.
 *
 * @{
 *
 */

/**
 * @defgroup radio-types Types
 *
 * @brief
 *   This module includes the platform abstraction for a radio frame.
 *
 * @{
 *
 */

enum
{
    OT_RADIO_FRAME_MAX_SIZE    = 127,    ///< aMaxPHYPacketSize (IEEE 802.15.4-2006)
    OT_RADIO_FRAME_MIN_SIZE    = 3,      ///< Minimal size of frame FCS + CONTROL
    OT_RADIO_SYMBOLS_PER_OCTET = 2,      ///< 2.4 GHz IEEE 802.15.4-2006
    OT_RADIO_BIT_RATE          = 250000, ///< 2.4 GHz IEEE 802.15.4 (bits per second)
    OT_RADIO_BITS_PER_OCTET    = 8,      ///< Number of bits per octet

    OT_RADIO_SYMBOL_TIME   = ((OT_RADIO_BITS_PER_OCTET / OT_RADIO_SYMBOLS_PER_OCTET) * 1000000) / OT_RADIO_BIT_RATE,
    OT_RADIO_LQI_NONE      = 0,   ///< LQI measurement not supported
    OT_RADIO_RSSI_INVALID  = 127, ///< Invalid or unknown RSSI value
    OT_RADIO_POWER_INVALID = 127, ///< Invalid or unknown power value
};

/**
 * This enumeration defines the channel page.
 *
 */
enum
{
    OT_RADIO_CHANNEL_PAGE_0      = 0,                               ///< 2.4 GHz IEEE 802.15.4-2006
    OT_RADIO_CHANNEL_PAGE_0_MASK = (1U << OT_RADIO_CHANNEL_PAGE_0), ///< 2.4 GHz IEEE 802.15.4-2006
    OT_RADIO_CHANNEL_PAGE_2      = 2,                               ///< 915 MHz IEEE 802.15.4-2006
    OT_RADIO_CHANNEL_PAGE_2_MASK = (1U << OT_RADIO_CHANNEL_PAGE_2), ///< 915 MHz IEEE 802.15.4-2006
};

/**
 * This enumeration defines the frequency band channel range.
 *
 */
enum
{
    OT_RADIO_915MHZ_OQPSK_CHANNEL_MIN  = 1,                                           ///< 915 MHz IEEE 802.15.4-2006
    OT_RADIO_915MHZ_OQPSK_CHANNEL_MAX  = 10,                                          ///< 915 MHz IEEE 802.15.4-2006
    OT_RADIO_915MHZ_OQPSK_CHANNEL_MASK = 0x3ff << OT_RADIO_915MHZ_OQPSK_CHANNEL_MIN,  ///< 915 MHz IEEE 802.15.4-2006
    OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MIN  = 11,                                          ///< 2.4 GHz IEEE 802.15.4-2006
    OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MAX  = 26,                                          ///< 2.4 GHz IEEE 802.15.4-2006
    OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MASK = 0xffff << OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MIN, ///< 2.4 GHz IEEE 802.15.4-2006
};

/**
 * This type represents radio capabilities.
 *
 * The value is a bit-field indicating the capabilities supported by the radio. See `OT_RADIO_CAPS_*` definitions.
 *
 */
typedef uint8_t otRadioCaps;

/**
 * This enumeration defines constants that are used to indicate different radio capabilities. See `otRadioCaps`.
 *
 */
enum
{
    OT_RADIO_CAPS_NONE             = 0,      ///< Radio supports no capability.
    OT_RADIO_CAPS_ACK_TIMEOUT      = 1 << 0, ///< Radio supports AckTime event.
    OT_RADIO_CAPS_ENERGY_SCAN      = 1 << 1, ///< Radio supports Energy Scans.
    OT_RADIO_CAPS_TRANSMIT_RETRIES = 1 << 2, ///< Radio supports tx retry logic with collision avoidance (CSMA).
    OT_RADIO_CAPS_CSMA_BACKOFF     = 1 << 3, ///< Radio supports CSMA backoff for frame transmission (but no retry).
    OT_RADIO_CAPS_SLEEP_TO_TX      = 1 << 4, ///< Radio supports direct transition from sleep to TX with CSMA.
    OT_RADIO_CAPS_TRANSMIT_SEC     = 1 << 5, ///< Radio supports tx security.
    OT_RADIO_CAPS_TRANSMIT_TIMING  = 1 << 6, ///< Radio supports tx at specific time.
    OT_RADIO_CAPS_RECEIVE_TIMING   = 1 << 7, ///< Radio supports rx at specific time.
};

#define OT_PANID_BROADCAST 0xffff ///< IEEE 802.15.4 Broadcast PAN ID

/**
 * This type represents the IEEE 802.15.4 PAN ID.
 *
 */
typedef uint16_t otPanId;

/**
 * This type represents the IEEE 802.15.4 Short Address.
 *
 */
typedef uint16_t otShortAddress;

#define OT_EXT_ADDRESS_SIZE 8 ///< Size of an IEEE 802.15.4 Extended Address (bytes)

/**
 * This enumeration defines constants about size of header IE in ACK.
 *
 */
enum
{
    OT_IE_HEADER_SIZE               = 2,  ///< Size of IE header in bytes.
    OT_CSL_IE_SIZE                  = 4,  ///< Size of CSL IE content in bytes.
    OT_ACK_IE_MAX_SIZE              = 16, ///< Max length for header IE in ACK.
    OT_ENH_PROBING_IE_DATA_MAX_SIZE = 2,  ///< Max length of Link Metrics data in Vendor-Specific IE.
};

#define CSL_IE_HEADER_BYTES_LO 0x04 ///< Fixed CSL IE header first byte
#define CSL_IE_HEADER_BYTES_HI 0x0d ///< Fixed CSL IE header second byte

/**
 * @struct otExtAddress
 *
 * This structure represents the IEEE 802.15.4 Extended Address.
 *
 */
OT_TOOL_PACKED_BEGIN
struct otExtAddress
{
    uint8_t m8[OT_EXT_ADDRESS_SIZE]; ///< IEEE 802.15.4 Extended Address bytes
} OT_TOOL_PACKED_END;

/**
 * This structure represents the IEEE 802.15.4 Extended Address.
 *
 */
typedef struct otExtAddress otExtAddress;

#define OT_MAC_KEY_SIZE 16 ///< Size of the MAC Key in bytes.

/**
 * @struct otMacKey
 *
 * This structure represents a MAC Key.
 *
 */
OT_TOOL_PACKED_BEGIN
struct otMacKey
{
    uint8_t m8[OT_MAC_KEY_SIZE]; ///< MAC Key bytes.
} OT_TOOL_PACKED_END;

/**
 * This structure represents a MAC Key.
 *
 */
typedef struct otMacKey otMacKey;

/**
 * This type represents a MAC Key Ref used by PSA.
 *
 */
typedef otCryptoKeyRef otMacKeyRef;

/**
 * @struct otMacKeyMaterial
 *
 * This structure represents a MAC Key.
 *
 */
typedef struct otMacKeyMaterial
{
    union
    {
        otMacKeyRef mKeyRef; ///< Reference to the key stored.
        otMacKey    mKey;    ///< Key stored as literal.
    } mKeyMaterial;
} otMacKeyMaterial;

/**
 * This enumeration defines constants about key types.
 *
 */
typedef enum
{
    OT_KEY_TYPE_LITERAL_KEY = 0, ///< Use Literal Keys.
    OT_KEY_TYPE_KEY_REF     = 1, ///< Use Reference to Key.
} otRadioKeyType;

/**
 * This structure represents the IEEE 802.15.4 Header IE (Information Element) related information of a radio frame.
 */
typedef struct otRadioIeInfo
{
    int64_t mNetworkTimeOffset; ///< The time offset to the Thread network time.
    uint8_t mTimeIeOffset;      ///< The Time IE offset from the start of PSDU.
    uint8_t mTimeSyncSeq;       ///< The Time sync sequence.
} otRadioIeInfo;

/**
 * This structure represents an IEEE 802.15.4 radio frame.
 */
typedef struct otRadioFrame
{
    uint8_t *mPsdu; ///< The PSDU.

    uint16_t mLength;  ///< Length of the PSDU.
    uint8_t  mChannel; ///< Channel used to transmit/receive the frame.

    uint8_t mRadioType; ///< Radio link type - should be ignored by radio driver.

    /**
     * The union of transmit and receive information for a radio frame.
     */
    union
    {
        /**
         * Structure representing radio frame transmit information.
         */
        struct
        {
            const otMacKeyMaterial *mAesKey;  ///< The key material used for AES-CCM frame security.
            otRadioIeInfo *         mIeInfo;  ///< The pointer to the Header IE(s) related information.
            uint32_t                mTxDelay; ///< The delay time for this transmission (based on `mTxDelayBaseTime`).
            uint32_t                mTxDelayBaseTime; ///< The base time for the transmission delay.
            uint8_t mMaxCsmaBackoffs; ///< Maximum number of backoffs attempts before declaring CCA failure.
            uint8_t mMaxFrameRetries; ///< Maximum number of retries allowed after a transmission failure.

            /**
             * Indicates whether frame counter and CSL IEs are properly updated in the header.
             *
             * If the platform layer does not provide `OT_RADIO_CAPS_TRANSMIT_SEC` capability, it can ignore this flag.
             *
             * If the platform provides `OT_RADIO_CAPS_TRANSMIT_SEC` capability, then platform is expected to handle tx
             * security processing and assignment of frame counter. In this case the following behavior is expected:
             *
             * When `mIsHeaderUpdated` is set, it indicates that OpenThread core has already set the frame counter and
             * CSL IEs (if security is enabled) in the prepared frame. The counter is ensured to match the counter value
             * from the previous attempts of the same frame. The platform should not assign or change the frame counter
             * (but may still need to perform security processing depending on `mIsSecurityProcessed` flag).
             *
             * If `mIsHeaderUpdated` is not set, then the frame counter and key CSL IE not set in the frame by
             * OpenThread core and it is the responsibility of the radio platform to assign them. The platform
             * must update the frame header (assign counter and CSL IE values) before sending the frame over the air,
             * however if the the transmission gets aborted and the frame is never sent over the air (e.g., channel
             * access error) the platform may choose to not update the header. If the platform updates the header,
             * it must also set this flag before passing the frame back from the `otPlatRadioTxDone()` callback.
             *
             */
            bool mIsHeaderUpdated : 1;
            bool mIsARetx : 1;             ///< Indicates whether the frame is a retransmission or not.
            bool mCsmaCaEnabled : 1;       ///< Set to true to enable CSMA-CA for this packet, false otherwise.
            bool mCslPresent : 1;          ///< Set to true if CSL header IE is present.
            bool mIsSecurityProcessed : 1; ///< True if SubMac should skip the AES processing of this frame.
        } mTxInfo;

        /**
         * Structure representing radio frame receive information.
         */
        struct
        {
            /**
             * The timestamp when the frame was received in microseconds.
             *
             * The value SHALL be the time when the SFD was received when TIME_SYNC or CSL is enabled.
             * Otherwise, the time when the MAC frame was fully received is also acceptable.
             *
             */
            uint64_t mTimestamp;

            uint32_t mAckFrameCounter; ///< ACK security frame counter (applicable when `mAckedWithSecEnhAck` is set).
            uint8_t  mAckKeyId;        ///< ACK security key index (applicable when `mAckedWithSecEnhAck` is set).
            int8_t   mRssi;            ///< Received signal strength indicator in dBm for received frames.
            uint8_t  mLqi;             ///< Link Quality Indicator for received frames.

            // Flags
            bool mAckedWithFramePending : 1; ///< This indicates if this frame was acknowledged with frame pending set.
            bool mAckedWithSecEnhAck : 1; ///< This indicates if this frame was acknowledged with secured enhance ACK.
        } mRxInfo;
    } mInfo;
} otRadioFrame;

/**
 * This structure represents the state of a radio.
 * Initially, a radio is in the Disabled state.
 */
typedef enum otRadioState
{
    OT_RADIO_STATE_DISABLED = 0,
    OT_RADIO_STATE_SLEEP    = 1,
    OT_RADIO_STATE_RECEIVE  = 2,
    OT_RADIO_STATE_TRANSMIT = 3,
    OT_RADIO_STATE_INVALID  = 255,
} otRadioState;

/**
 * The following are valid radio state transitions:
 *
 *                                    (Radio ON)
 *  +----------+  Enable()  +-------+  Receive() +---------+   Transmit()  +----------+
 *  |          |----------->|       |----------->|         |-------------->|          |
 *  | Disabled |            | Sleep |            | Receive |               | Transmit |
 *  |          |<-----------|       |<-----------|         |<--------------|          |
 *  +----------+  Disable() +-------+   Sleep()  +---------+   Receive()   +----------+
 *                                    (Radio OFF)                 or
 *                                                        signal TransmitDone
 *
 * During the IEEE 802.15.4 data request command the transition Sleep->Receive->Transmit
 * can be shortened to direct transition from Sleep to Transmit if the platform supports
 * the OT_RADIO_CAPS_SLEEP_TO_TX capability.
 */

/**
 * This structure represents radio coexistence metrics.
 */
typedef struct otRadioCoexMetrics
{
    uint32_t mNumGrantGlitch;          ///< Number of grant glitches.
    uint32_t mNumTxRequest;            ///< Number of tx requests.
    uint32_t mNumTxGrantImmediate;     ///< Number of tx requests while grant was active.
    uint32_t mNumTxGrantWait;          ///< Number of tx requests while grant was inactive.
    uint32_t mNumTxGrantWaitActivated; ///< Number of tx requests while grant was inactive that were ultimately granted.
    uint32_t mNumTxGrantWaitTimeout;   ///< Number of tx requests while grant was inactive that timed out.
    uint32_t mNumTxGrantDeactivatedDuringRequest; ///< Number of tx that were in progress when grant was deactivated.
    uint32_t mNumTxDelayedGrant;                  ///< Number of tx requests that were not granted within 50us.
    uint32_t mAvgTxRequestToGrantTime;            ///< Average time in usec from tx request to grant.
    uint32_t mNumRxRequest;                       ///< Number of rx requests.
    uint32_t mNumRxGrantImmediate;                ///< Number of rx requests while grant was active.
    uint32_t mNumRxGrantWait;                     ///< Number of rx requests while grant was inactive.
    uint32_t mNumRxGrantWaitActivated; ///< Number of rx requests while grant was inactive that were ultimately granted.
    uint32_t mNumRxGrantWaitTimeout;   ///< Number of rx requests while grant was inactive that timed out.
    uint32_t mNumRxGrantDeactivatedDuringRequest; ///< Number of rx that were in progress when grant was deactivated.
    uint32_t mNumRxDelayedGrant;                  ///< Number of rx requests that were not granted within 50us.
    uint32_t mAvgRxRequestToGrantTime;            ///< Average time in usec from rx request to grant.
    uint32_t mNumRxGrantNone;                     ///< Number of rx requests that completed without receiving grant.
    bool     mStopped;                            ///< Stats collection stopped due to saturation.
} otRadioCoexMetrics;

/**
 * This structure represents what metrics are specified to query.
 *
 */
typedef struct otLinkMetrics
{
    bool mPduCount : 1;   ///< Pdu count.
    bool mLqi : 1;        ///< Link Quality Indicator.
    bool mLinkMargin : 1; ///< Link Margin.
    bool mRssi : 1;       ///< Received Signal Strength Indicator.
    bool mReserved : 1;   ///< Reserved, this is for reference device.
} otLinkMetrics;

/**
 * @}
 *
 */

/**
 * @defgroup radio-config Configuration
 *
 * @brief
 *   This module includes the platform abstraction for radio configuration.
 *
 * @{
 *
 */

/**
 * Get the radio capabilities.
 *
 * @param[in] aInstance  The OpenThread instance structure.
 *
 * @returns The radio capability bit vector (see `OT_RADIO_CAP_*` definitions).
 *
 */
otRadioCaps otPlatRadioGetCaps(otInstance *aInstance);

/**
 * Get the radio version string.
 *
 * This is an optional radio driver platform function. If not provided by platform radio driver, OpenThread uses
 * the OpenThread version instead (@sa otGetVersionString()).
 *
 * @param[in]  aInstance   The OpenThread instance structure.
 *
 * @returns A pointer to the OpenThread radio version.
 *
 */
const char *otPlatRadioGetVersionString(otInstance *aInstance);

/**
 * Get the radio receive sensitivity value.
 *
 * @param[in] aInstance  The OpenThread instance structure.
 *
 * @returns The radio receive sensitivity value in dBm.
 *
 */
int8_t otPlatRadioGetReceiveSensitivity(otInstance *aInstance);

/**
 * Get the factory-assigned IEEE EUI-64 for this interface.
 *
 * @param[in]  aInstance   The OpenThread instance structure.
 * @param[out] aIeeeEui64  A pointer to the factory-assigned IEEE EUI-64.
 *
 */
void otPlatRadioGetIeeeEui64(otInstance *aInstance, uint8_t *aIeeeEui64);

/**
 * Set the PAN ID for address filtering.
 *
 * @param[in] aInstance  The OpenThread instance structure.
 * @param[in] aPanId     The IEEE 802.15.4 PAN ID.
 *
 */
void otPlatRadioSetPanId(otInstance *aInstance, otPanId aPanId);

/**
 * Set the Extended Address for address filtering.
 *
 * @param[in] aInstance    The OpenThread instance structure.
 * @param[in] aExtAddress  A pointer to the IEEE 802.15.4 Extended Address stored in little-endian byte order.
 *
 *
 */
void otPlatRadioSetExtendedAddress(otInstance *aInstance, const otExtAddress *aExtAddress);

/**
 * Set the Short Address for address filtering.
 *
 * @param[in] aInstance      The OpenThread instance structure.
 * @param[in] aShortAddress  The IEEE 802.15.4 Short Address.
 *
 */
void otPlatRadioSetShortAddress(otInstance *aInstance, otShortAddress aShortAddress);

/**
 * Get the radio's transmit power in dBm.
 *
 * @note The transmit power returned will be no larger than the power specified in the max power table for
 * the current channel.
 *
 * @param[in] aInstance  The OpenThread instance structure.
 * @param[out] aPower    The transmit power in dBm.
 *
 * @retval OT_ERROR_NONE             Successfully retrieved the transmit power.
 * @retval OT_ERROR_INVALID_ARGS     @p aPower was NULL.
 * @retval OT_ERROR_NOT_IMPLEMENTED  Transmit power configuration via dBm is not implemented.
 *
 */
otError otPlatRadioGetTransmitPower(otInstance *aInstance, int8_t *aPower);

/**
 * Set the radio's transmit power in dBm.
 *
 * @note The real transmit power will be no larger than the power specified in the max power table for
 * the current channel.
 *
 * @param[in] aInstance  The OpenThread instance structure.
 * @param[in] aPower     The transmit power in dBm.
 *
 * @retval OT_ERROR_NONE             Successfully set the transmit power.
 * @retval OT_ERROR_NOT_IMPLEMENTED  Transmit power configuration via dBm is not implemented.
 *
 */
otError otPlatRadioSetTransmitPower(otInstance *aInstance, int8_t aPower);

/**
 * Get the radio's CCA ED threshold in dBm measured at antenna connector per IEEE 802.15.4 - 2015 section 10.1.4.
 *
 * @param[in] aInstance    The OpenThread instance structure.
 * @param[out] aThreshold  The CCA ED threshold in dBm.
 *
 * @retval OT_ERROR_NONE             Successfully retrieved the CCA ED threshold.
 * @retval OT_ERROR_INVALID_ARGS     @p aThreshold was NULL.
 * @retval OT_ERROR_NOT_IMPLEMENTED  CCA ED threshold configuration via dBm is not implemented.
 *
 */
otError otPlatRadioGetCcaEnergyDetectThreshold(otInstance *aInstance, int8_t *aThreshold);

/**
 * Set the radio's CCA ED threshold in dBm measured at antenna connector per IEEE 802.15.4 - 2015 section 10.1.4.
 *
 * @param[in] aInstance   The OpenThread instance structure.
 * @param[in] aThreshold  The CCA ED threshold in dBm.
 *
 * @retval OT_ERROR_NONE             Successfully set the transmit power.
 * @retval OT_ERROR_INVALID_ARGS     Given threshold is out of range.
 * @retval OT_ERROR_NOT_IMPLEMENTED  CCA ED threshold configuration via dBm is not implemented.
 *
 */
otError otPlatRadioSetCcaEnergyDetectThreshold(otInstance *aInstance, int8_t aThreshold);

/**
 * Get the external FEM's Rx LNA gain in dBm.
 *
 * @param[in]  aInstance  The OpenThread instance structure.
 * @param[out] aGain     The external FEM's Rx LNA gain in dBm.
 *
 * @retval OT_ERROR_NONE             Successfully retrieved the external FEM's LNA gain.
 * @retval OT_ERROR_INVALID_ARGS     @p aGain was NULL.
 * @retval OT_ERROR_NOT_IMPLEMENTED  External FEM's LNA setting is not implemented.
 *
 */
otError otPlatRadioGetFemLnaGain(otInstance *aInstance, int8_t *aGain);

/**
 * Set the external FEM's Rx LNA gain in dBm.
 *
 * @param[in] aInstance  The OpenThread instance structure.
 * @param[in] aGain      The external FEM's Rx LNA gain in dBm.
 *
 * @retval OT_ERROR_NONE             Successfully set the external FEM's LNA gain.
 * @retval OT_ERROR_NOT_IMPLEMENTED  External FEM's LNA gain setting is not implemented.
 *
 */
otError otPlatRadioSetFemLnaGain(otInstance *aInstance, int8_t aGain);

/**
 * Get the status of promiscuous mode.
 *
 * @param[in] aInstance  The OpenThread instance structure.
 *
 * @retval TRUE   Promiscuous mode is enabled.
 * @retval FALSE  Promiscuous mode is disabled.
 *
 */
bool otPlatRadioGetPromiscuous(otInstance *aInstance);

/**
 * Enable or disable promiscuous mode.
 *
 * @param[in]  aInstance The OpenThread instance structure.
 * @param[in]  aEnable   TRUE to enable or FALSE to disable promiscuous mode.
 *
 */
void otPlatRadioSetPromiscuous(otInstance *aInstance, bool aEnable);

/**
 * Update MAC keys and key index
 *
 * This function is used when radio provides OT_RADIO_CAPS_TRANSMIT_SEC capability.
 *
 * @param[in]   aInstance    A pointer to an OpenThread instance.
 * @param[in]   aKeyIdMode   The key ID mode.
 * @param[in]   aKeyId       Current MAC key index.
 * @param[in]   aPrevKey     A pointer to the previous MAC key.
 * @param[in]   aCurrKey     A pointer to the current MAC key.
 * @param[in]   aNextKey     A pointer to the next MAC key.
 * @param[in]   aKeyType     Key Type used.
 *
 */
void otPlatRadioSetMacKey(otInstance *            aInstance,
                          uint8_t                 aKeyIdMode,
                          uint8_t                 aKeyId,
                          const otMacKeyMaterial *aPrevKey,
                          const otMacKeyMaterial *aCurrKey,
                          const otMacKeyMaterial *aNextKey,
                          otRadioKeyType          aKeyType);

/**
 * This method sets the current MAC frame counter value.
 *
 * This function is used when radio provides `OT_RADIO_CAPS_TRANSMIT_SEC` capability.
 *
 * @param[in]   aInstance         A pointer to an OpenThread instance.
 * @param[in]   aMacFrameCounter  The MAC frame counter value.
 *
 */
void otPlatRadioSetMacFrameCounter(otInstance *aInstance, uint32_t aMacFrameCounter);

/**
 * Get the current estimated time (64bits width) of the radio chip.
 *
 * @param[in]   aInstance    A pointer to an OpenThread instance.
 *
 * @returns The current time in microseconds. UINT64_MAX when platform does not support or radio time is not ready.
 *
 */
uint64_t otPlatRadioGetNow(otInstance *aInstance);

/**
 * Get the bus speed in bits/second between the host and the radio chip.
 *
 * @param[in]   aInstance    A pointer to an OpenThread instance.
 *
 * @returns The bus speed in bits/second between the host and the radio chip.
 *          Return 0 when the MAC and above layer and Radio layer resides on the same chip.
 *
 */
uint32_t otPlatRadioGetBusSpeed(otInstance *aInstance);

/**
 * @}
 *
 */

/**
 * @defgroup radio-operation Operation
 *
 * @brief
 *   This module includes the platform abstraction for radio operations.
 *
 * @{
 *
 */

/**
 * Get current state of the radio.
 *
 * This function is not required by OpenThread. It may be used for debugging and/or application-specific purposes.
 *
 * @note This function may be not implemented. It does not affect OpenThread.
 *
 * @param[in] aInstance  The OpenThread instance structure.
 *
 * @return  Current state of the radio.
 *
 */
otRadioState otPlatRadioGetState(otInstance *aInstance);

/**
 * Enable the radio.
 *
 * @param[in] aInstance  The OpenThread instance structure.
 *
 * @retval OT_ERROR_NONE     Successfully enabled.
 * @retval OT_ERROR_FAILED   The radio could not be enabled.
 *
 */
otError otPlatRadioEnable(otInstance *aInstance);

/**
 * Disable the radio.
 *
 * @param[in] aInstance  The OpenThread instance structure.
 *
 * @retval OT_ERROR_NONE            Successfully transitioned to Disabled.
 * @retval OT_ERROR_INVALID_STATE   The radio was not in sleep state.
 *
 */
otError otPlatRadioDisable(otInstance *aInstance);

/**
 * Check whether radio is enabled or not.
 *
 * @param[in] aInstance  The OpenThread instance structure.
 *
 * @returns TRUE if the radio is enabled, FALSE otherwise.
 *
 */
bool otPlatRadioIsEnabled(otInstance *aInstance);

/**
 * Transition the radio from Receive to Sleep (turn off the radio).
 *
 * @param[in] aInstance  The OpenThread instance structure.
 *
 * @retval OT_ERROR_NONE          Successfully transitioned to Sleep.
 * @retval OT_ERROR_BUSY          The radio was transmitting.
 * @retval OT_ERROR_INVALID_STATE The radio was disabled.
 *
 */
otError otPlatRadioSleep(otInstance *aInstance);

/**
 * Transition the radio from Sleep to Receive (turn on the radio).
 *
 * @param[in]  aInstance  The OpenThread instance structure.
 * @param[in]  aChannel   The channel to use for receiving.
 *
 * @retval OT_ERROR_NONE          Successfully transitioned to Receive.
 * @retval OT_ERROR_INVALID_STATE The radio was disabled or transmitting.
 *
 */
otError otPlatRadioReceive(otInstance *aInstance, uint8_t aChannel);

/**
 * Schedule a radio reception window at a specific time and duration.
 *
 * @param[in]  aChannel   The radio channel on which to receive.
 * @param[in]  aStart     The receive window start time, in microseconds.
 * @param[in]  aDuration  The receive window duration, in microseconds
 *
 * @retval OT_ERROR_NONE    Successfully scheduled receive window.
 * @retval OT_ERROR_FAILED  The receive window could not be scheduled.
 */
otError otPlatRadioReceiveAt(otInstance *aInstance, uint8_t aChannel, uint32_t aStart, uint32_t aDuration);

/**
 * The radio driver calls this method to notify OpenThread of a received frame.
 *
 * @param[in]  aInstance The OpenThread instance structure.
 * @param[in]  aFrame    A pointer to the received frame or NULL if the receive operation failed.
 * @param[in]  aError    OT_ERROR_NONE when successfully received a frame,
 *                       OT_ERROR_ABORT when reception was aborted and a frame was not received,
 *                       OT_ERROR_NO_BUFS when a frame could not be received due to lack of rx buffer space.
 *
 */
extern void otPlatRadioReceiveDone(otInstance *aInstance, otRadioFrame *aFrame, otError aError);

/**
 * The radio driver calls this method to notify OpenThread diagnostics module of a received frame.
 *
 * This function is used when diagnostics is enabled.
 *
 * @param[in]  aInstance The OpenThread instance structure.
 * @param[in]  aFrame    A pointer to the received frame or NULL if the receive operation failed.
 * @param[in]  aError    OT_ERROR_NONE when successfully received a frame,
 *                       OT_ERROR_ABORT when reception was aborted and a frame was not received,
 *                       OT_ERROR_NO_BUFS when a frame could not be received due to lack of rx buffer space.
 *
 */
extern void otPlatDiagRadioReceiveDone(otInstance *aInstance, otRadioFrame *aFrame, otError aError);

/**
 * Get the radio transmit frame buffer.
 *
 * OpenThread forms the IEEE 802.15.4 frame in this buffer then calls `otPlatRadioTransmit()` to request transmission.
 *
 * @param[in] aInstance  The OpenThread instance structure.
 *
 * @returns A pointer to the transmit frame buffer.
 *
 */
otRadioFrame *otPlatRadioGetTransmitBuffer(otInstance *aInstance);

/**
 * Begin the transmit sequence on the radio.
 *
 * The caller must form the IEEE 802.15.4 frame in the buffer provided by `otPlatRadioGetTransmitBuffer()` before
 * requesting transmission.  The channel and transmit power are also included in the otRadioFrame structure.
 *
 * The transmit sequence consists of:
 * 1. Transitioning the radio to Transmit from one of the following states:
 *    - Receive if RX is on when the device is idle or OT_RADIO_CAPS_SLEEP_TO_TX is not supported
 *    - Sleep if RX is off when the device is idle and OT_RADIO_CAPS_SLEEP_TO_TX is supported.
 * 2. Transmits the psdu on the given channel and at the given transmit power.
 *
 * @param[in] aInstance  The OpenThread instance structure.
 * @param[in] aFrame     A pointer to the frame to be transmitted.
 *
 * @retval OT_ERROR_NONE          Successfully transitioned to Transmit.
 * @retval OT_ERROR_INVALID_STATE The radio was not in the Receive state.
 *
 */
otError otPlatRadioTransmit(otInstance *aInstance, otRadioFrame *aFrame);

/**
 * The radio driver calls this method to notify OpenThread that the transmission has started.
 *
 * @note  This function should be called by the same thread that executes all of the other OpenThread code. It should
 *        not be called by ISR or any other task.
 *
 * @param[in]  aInstance  A pointer to the OpenThread instance structure.
 * @param[in]  aFrame     A pointer to the frame that is being transmitted.
 *
 */
extern void otPlatRadioTxStarted(otInstance *aInstance, otRadioFrame *aFrame);

/**
 * The radio driver calls this function to notify OpenThread that the transmit operation has completed,
 * providing both the transmitted frame and, if applicable, the received ack frame.
 *
 * When radio provides `OT_RADIO_CAPS_TRANSMIT_SEC` capability, radio platform layer updates @p aFrame
 * with the security frame counter and key index values maintained by the radio.
 *
 * @param[in]  aInstance  The OpenThread instance structure.
 * @param[in]  aFrame     A pointer to the frame that was transmitted.
 * @param[in]  aAckFrame  A pointer to the ACK frame, NULL if no ACK was received.
 * @param[in]  aError     OT_ERROR_NONE when the frame was transmitted,
 *                        OT_ERROR_NO_ACK when the frame was transmitted but no ACK was received,
 *                        OT_ERROR_CHANNEL_ACCESS_FAILURE tx could not take place due to activity on the channel,
 *                        OT_ERROR_ABORT when transmission was aborted for other reasons.
 *
 */
extern void otPlatRadioTxDone(otInstance *aInstance, otRadioFrame *aFrame, otRadioFrame *aAckFrame, otError aError);

/**
 * The radio driver calls this method to notify OpenThread diagnostics module that the transmission has completed.
 *
 * This function is used when diagnostics is enabled.
 *
 * @param[in]  aInstance      The OpenThread instance structure.
 * @param[in]  aFrame         A pointer to the frame that was transmitted.
 * @param[in]  aError         OT_ERROR_NONE when the frame was transmitted,
 *                            OT_ERROR_CHANNEL_ACCESS_FAILURE tx could not take place due to activity on the channel,
 *                            OT_ERROR_ABORT when transmission was aborted for other reasons.
 *
 */
extern void otPlatDiagRadioTransmitDone(otInstance *aInstance, otRadioFrame *aFrame, otError aError);

/**
 * Get the most recent RSSI measurement.
 *
 * @param[in] aInstance  The OpenThread instance structure.
 *
 * @returns The RSSI in dBm when it is valid.  127 when RSSI is invalid.
 *
 */
int8_t otPlatRadioGetRssi(otInstance *aInstance);

/**
 * Begin the energy scan sequence on the radio.
 *
 * This function is used when radio provides OT_RADIO_CAPS_ENERGY_SCAN capability.
 *
 * @param[in] aInstance      The OpenThread instance structure.
 * @param[in] aScanChannel   The channel to perform the energy scan on.
 * @param[in] aScanDuration  The duration, in milliseconds, for the channel to be scanned.
 *
 * @retval OT_ERROR_NONE             Successfully started scanning the channel.
 * @retval OT_ERROR_NOT_IMPLEMENTED  The radio doesn't support energy scanning.
 *
 */
otError otPlatRadioEnergyScan(otInstance *aInstance, uint8_t aScanChannel, uint16_t aScanDuration);

/**
 * The radio driver calls this method to notify OpenThread that the energy scan is complete.
 *
 * This function is used when radio provides OT_RADIO_CAPS_ENERGY_SCAN capability.
 *
 * @param[in]  aInstance           The OpenThread instance structure.
 * @param[in]  aEnergyScanMaxRssi  The maximum RSSI encountered on the scanned channel.
 *
 */
extern void otPlatRadioEnergyScanDone(otInstance *aInstance, int8_t aEnergyScanMaxRssi);

/**
 * Enable/Disable source address match feature.
 *
 * The source address match feature controls how the radio layer decides the "frame pending" bit for acks sent in
 * response to data request commands from children.
 *
 * If disabled, the radio layer must set the "frame pending" on all acks to data request commands.
 *
 * If enabled, the radio layer uses the source address match table to determine whether to set or clear the "frame
 * pending" bit in an ack to a data request command.
 *
 * The source address match table provides the list of children for which there is a pending frame. Either a short
 * address or an extended/long address can be added to the source address match table.
 *
 * @param[in]  aInstance   The OpenThread instance structure.
 * @param[in]  aEnable     Enable/disable source address match feature.
 *
 */
void otPlatRadioEnableSrcMatch(otInstance *aInstance, bool aEnable);

/**
 * Add a short address to the source address match table.
 *
 * @param[in]  aInstance      The OpenThread instance structure.
 * @param[in]  aShortAddress  The short address to be added.
 *
 * @retval OT_ERROR_NONE      Successfully added short address to the source match table.
 * @retval OT_ERROR_NO_BUFS   No available entry in the source match table.
 *
 */
otError otPlatRadioAddSrcMatchShortEntry(otInstance *aInstance, otShortAddress aShortAddress);

/**
 * Add an extended address to the source address match table.
 *
 * @param[in]  aInstance    The OpenThread instance structure.
 * @param[in]  aExtAddress  The extended address to be added stored in little-endian byte order.
 *
 * @retval OT_ERROR_NONE      Successfully added extended address to the source match table.
 * @retval OT_ERROR_NO_BUFS   No available entry in the source match table.
 *
 */
otError otPlatRadioAddSrcMatchExtEntry(otInstance *aInstance, const otExtAddress *aExtAddress);

/**
 * Remove a short address from the source address match table.
 *
 * @param[in]  aInstance      The OpenThread instance structure.
 * @param[in]  aShortAddress  The short address to be removed.
 *
 * @retval OT_ERROR_NONE        Successfully removed short address from the source match table.
 * @retval OT_ERROR_NO_ADDRESS  The short address is not in source address match table.
 *
 */
otError otPlatRadioClearSrcMatchShortEntry(otInstance *aInstance, otShortAddress aShortAddress);

/**
 * Remove an extended address from the source address match table.
 *
 * @param[in]  aInstance    The OpenThread instance structure.
 * @param[in]  aExtAddress  The extended address to be removed stored in little-endian byte order.
 *
 * @retval OT_ERROR_NONE        Successfully removed the extended address from the source match table.
 * @retval OT_ERROR_NO_ADDRESS  The extended address is not in source address match table.
 *
 */
otError otPlatRadioClearSrcMatchExtEntry(otInstance *aInstance, const otExtAddress *aExtAddress);

/**
 * Clear all short addresses from the source address match table.
 *
 * @param[in]  aInstance   The OpenThread instance structure.
 *
 */
void otPlatRadioClearSrcMatchShortEntries(otInstance *aInstance);

/**
 * Clear all the extended/long addresses from source address match table.
 *
 * @param[in]  aInstance   The OpenThread instance structure.
 *
 */
void otPlatRadioClearSrcMatchExtEntries(otInstance *aInstance);

/**
 * Get the radio supported channel mask that the device is allowed to be on.
 *
 * @param[in]  aInstance   The OpenThread instance structure.
 *
 * @returns The radio supported channel mask.
 *
 */
uint32_t otPlatRadioGetSupportedChannelMask(otInstance *aInstance);

/**
 * Get the radio preferred channel mask that the device prefers to form on.
 *
 * @param[in]  aInstance   The OpenThread instance structure.
 *
 * @returns The radio preferred channel mask.
 *
 */
uint32_t otPlatRadioGetPreferredChannelMask(otInstance *aInstance);

/**
 * Enable the radio coex.
 *
 * This function is used when feature OPENTHREAD_CONFIG_PLATFORM_RADIO_COEX_ENABLE is enabled.
 *
 * @param[in] aInstance  The OpenThread instance structure.
 * @param[in] aEnabled   TRUE to enable the radio coex, FALSE otherwise.
 *
 * @retval OT_ERROR_NONE     Successfully enabled.
 * @retval OT_ERROR_FAILED   The radio coex could not be enabled.
 *
 */
otError otPlatRadioSetCoexEnabled(otInstance *aInstance, bool aEnabled);

/**
 * Check whether radio coex is enabled or not.
 *
 * This function is used when feature OPENTHREAD_CONFIG_PLATFORM_RADIO_COEX_ENABLE is enabled.
 *
 * @param[in] aInstance  The OpenThread instance structure.
 *
 * @returns TRUE if the radio coex is enabled, FALSE otherwise.
 *
 */
bool otPlatRadioIsCoexEnabled(otInstance *aInstance);

/**
 * Get the radio coexistence metrics.
 *
 * This function is used when feature OPENTHREAD_CONFIG_PLATFORM_RADIO_COEX_ENABLE is enabled.
 *
 * @param[in]  aInstance     The OpenThread instance structure.
 * @param[out] aCoexMetrics  A pointer to the coexistence metrics structure.
 *
 * @retval OT_ERROR_NONE          Successfully retrieved the coex metrics.
 * @retval OT_ERROR_INVALID_ARGS  @p aCoexMetrics was NULL.
 */
otError otPlatRadioGetCoexMetrics(otInstance *aInstance, otRadioCoexMetrics *aCoexMetrics);

/**
 * Enable or disable CSL receiver.
 *
 * @param[in]  aInstance     The OpenThread instance structure.
 * @param[in]  aCslPeriod    CSL period, 0 for disabling CSL.
 * @param[in]  aShortAddr    The short source address of CSL receiver's peer.
 * @param[in]  aExtAddr      The extended source address of CSL receiver's peer.
 *
 * @note Platforms should use CSL peer addresses to include CSL IE when generating enhanced acks.
 *
 * @retval  kErrorNotImplemented Radio driver doesn't support CSL.
 * @retval  kErrorFailed         Other platform specific errors.
 * @retval  kErrorNone           Successfully enabled or disabled CSL.
 *
 */
otError otPlatRadioEnableCsl(otInstance *        aInstance,
                             uint32_t            aCslPeriod,
                             otShortAddress      aShortAddr,
                             const otExtAddress *aExtAddr);

/**
 * Update CSL sample time in radio driver.
 *
 * Sample time is stored in radio driver as a copy to calculate phase when sending ACK with CSL IE.
 *
 * @param[in]  aInstance         The OpenThread instance structure.
 * @param[in]  aCslSampleTime    The latest sample time.
 *
 */
void otPlatRadioUpdateCslSampleTime(otInstance *aInstance, uint32_t aCslSampleTime);

/**
 * Get the current accuracy, in units of ± ppm, of the clock used for scheduling CSL operations.
 *
 * @note Platforms may optimize this value based on operational conditions (i.e.: temperature).
 *
 * @param[in]   aInstance    A pointer to an OpenThread instance.
 *
 * @returns The current CSL rx/tx scheduling drift, in units of ± ppm.
 *
 */
uint8_t otPlatRadioGetCslAccuracy(otInstance *aInstance);

/**
 * Get the current uncertainty, in units of 10 us, of the clock used for scheduling CSL operations.
 *
 * @param[in]   aInstance    A pointer to an OpenThread instance.
 *
 * @returns The current CSL Clock Uncertainty in units of 10 us.
 *
 */
uint8_t otPlatRadioGetCslClockUncertainty(otInstance *aInstance);

/**
 * Set the max transmit power for a specific channel.
 *
 * @param[in]  aInstance    The OpenThread instance structure.
 * @param[in]  aChannel     The radio channel.
 * @param[in]  aMaxPower    The max power in dBm, passing OT_RADIO_RSSI_INVALID will disable this channel.
 *
 * @retval  OT_ERROR_NOT_IMPLEMENTED  The feature is not implemented
 * @retval  OT_ERROR_INVALID_ARGS     The specified channel is not valid.
 * @retval  OT_ERROR_FAILED           Other platform specific errors.
 * @retval  OT_ERROR_NONE             Successfully set max transmit power.
 *
 */
otError otPlatRadioSetChannelMaxTransmitPower(otInstance *aInstance, uint8_t aChannel, int8_t aMaxPower);

/**
 * Set the region code.
 *
 * The radio region format is the 2-bytes ascii representation of the
 * ISO 3166 alpha-2 code.
 *
 * @param[in]  aInstance    The OpenThread instance structure.
 * @param[in]  aRegionCode  The radio region.
 *
 * @retval  OT_ERROR_FAILED           Other platform specific errors.
 * @retval  OT_ERROR_NONE             Successfully set region code.
 *
 */
otError otPlatRadioSetRegion(otInstance *aInstance, uint16_t aRegionCode);

/**
 * Get the region code.
 *
 * The radio region format is the 2-bytes ascii representation of the
 * ISO 3166 alpha-2 code.

 * @param[in]  aInstance    The OpenThread instance structure.
 * @param[out] aRegionCode  The radio region.
 *
 * @retval  OT_ERROR_INVALID_ARGS     @p aRegionCode is nullptr.
 * @retval  OT_ERROR_FAILED           Other platform specific errors.
 * @retval  OT_ERROR_NONE             Successfully got region code.
 *
 */
otError otPlatRadioGetRegion(otInstance *aInstance, uint16_t *aRegionCode);

/**
 * Enable/disable or update Enhanced-ACK Based Probing in radio for a specific Initiator.
 *
 * After Enhanced-ACK Based Probing is configured by a specific Probing Initiator, the Enhanced-ACK sent to that
 * node should include Vendor-Specific IE containing Link Metrics data. This method informs the radio to start/stop to
 * collect Link Metrics data and include Vendor-Specific IE that containing the data in Enhanced-ACK sent to that
 * Probing Initiator.
 *
 * @param[in]  aInstance     The OpenThread instance structure.
 * @param[in]  aLinkMetrics  This parameter specifies what metrics to query. Per spec 4.11.3.4.4.6, at most 2 metrics
 *                           can be specified. The probing would be disabled if @p `aLinkMetrics` is bitwise 0.
 * @param[in]  aShortAddr    The short address of the Probing Initiator.
 * @param[in]  aExtAddr      The extended source address of the Probing Initiator. @p aExtAddr MUST NOT be `NULL`.
 *
 * @retval  OT_ERROR_NONE            Successfully configured the Enhanced-ACK Based Probing.
 * @retval  OT_ERROR_INVALID_ARGS    @p aExtAddress is `NULL`.
 * @retval  OT_ERROR_NOT_FOUND       The Initiator indicated by @p aShortAddress is not found when trying to clear.
 * @retval  OT_ERROR_NO_BUFS         No more Initiator can be supported.
 *
 */
otError otPlatRadioConfigureEnhAckProbing(otInstance *        aInstance,
                                          otLinkMetrics       aLinkMetrics,
                                          otShortAddress      aShortAddress,
                                          const otExtAddress *aExtAddress);

/**
 * @}
 *
 */

/**
 * @}
 *
 */

#ifdef __cplusplus
} // end of extern "C"
#endif

#endif // OPENTHREAD_PLATFORM_RADIO_H_
