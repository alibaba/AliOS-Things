/*
 *    Copyright (c) 2016-2017, The OpenThread Authors.
 *    All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *    3. Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 *    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements raw link required Spinel interface to the OpenThread stack.
 */

#include "ncp_base.hpp"

#include <openthread/link.h>
#include <openthread/link_raw.h>
#include <openthread/ncp.h>
#include <openthread/platform/radio.h>
#include <openthread/platform/time.h>

#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/instance.hpp"
#include "mac/mac_frame.hpp"

#if OPENTHREAD_RADIO || OPENTHREAD_CONFIG_LINK_RAW_ENABLE

namespace ot {
namespace Ncp {

#if OPENTHREAD_RADIO
template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_RCP_API_VERSION>(void)
{
    return mEncoder.WriteUintPacked(SPINEL_RCP_API_VERSION);
}
#endif

// ----------------------------------------------------------------------------
// MARK: Raw Link-Layer Datapath Glue
// ----------------------------------------------------------------------------

otError NcpBase::PackRadioFrame(otRadioFrame *aFrame, otError aError)
{
    otError  error = OT_ERROR_FAILED;
    uint16_t flags = 0;

    if (aFrame != nullptr && aError == OT_ERROR_NONE)
    {
        // Append the frame contents
        SuccessOrExit(mEncoder.WriteDataWithLen(aFrame->mPsdu, aFrame->mLength));
    }
    else
    {
        // Append length
        SuccessOrExit(mEncoder.WriteUint16(0));
    }

    // Append metadata (rssi, etc)
    SuccessOrExit(mEncoder.WriteInt8(aFrame ? aFrame->mInfo.mRxInfo.mRssi : 0)); // RSSI
    SuccessOrExit(mEncoder.WriteInt8(-128));                                     // Noise Floor (Currently unused)

    if (aFrame != nullptr)
    {
        if (aFrame->mInfo.mRxInfo.mAckedWithFramePending)
        {
            flags |= SPINEL_MD_FLAG_ACKED_FP;
        }

        if (aFrame->mInfo.mRxInfo.mAckedWithSecEnhAck)
        {
            flags |= SPINEL_MD_FLAG_ACKED_SEC;
        }
    }

    SuccessOrExit(mEncoder.WriteUint16(flags)); // Flags

    SuccessOrExit(mEncoder.OpenStruct());                              // PHY-data
    SuccessOrExit(mEncoder.WriteUint8(aFrame ? aFrame->mChannel : 0)); // 802.15.4 channel (Receive channel)
    SuccessOrExit(mEncoder.WriteUint8(aFrame ? aFrame->mInfo.mRxInfo.mLqi
                                             : static_cast<uint8_t>(OT_RADIO_LQI_NONE))); // 802.15.4 LQI

    SuccessOrExit(mEncoder.WriteUint64(aFrame ? aFrame->mInfo.mRxInfo.mTimestamp : 0)); // The timestamp in microseconds
    SuccessOrExit(mEncoder.CloseStruct());

    SuccessOrExit(mEncoder.OpenStruct());            // Vendor-data
    SuccessOrExit(mEncoder.WriteUintPacked(aError)); // Receive error
    SuccessOrExit(mEncoder.CloseStruct());

    SuccessOrExit(mEncoder.OpenStruct());                                             // MAC-data
    SuccessOrExit(mEncoder.WriteUint8(aFrame ? aFrame->mInfo.mRxInfo.mAckKeyId : 0)); // The ACK auxiliary key ID
    SuccessOrExit(
        mEncoder.WriteUint32(aFrame ? aFrame->mInfo.mRxInfo.mAckFrameCounter : 0)); // The ACK auxiliary frame counter
    SuccessOrExit(mEncoder.CloseStruct());

    error = OT_ERROR_NONE;

exit:
    return error;
}

void NcpBase::LinkRawReceiveDone(otInstance *, otRadioFrame *aFrame, otError aError)
{
    sNcpInstance->LinkRawReceiveDone(aFrame, aError);
}

void NcpBase::LinkRawReceiveDone(otRadioFrame *aFrame, otError aError)
{
    uint8_t header = SPINEL_HEADER_FLAG | SPINEL_HEADER_IID_0;

    // Append frame header
    SuccessOrExit(mEncoder.BeginFrame(header, SPINEL_CMD_PROP_VALUE_IS, SPINEL_PROP_STREAM_RAW));

    SuccessOrExit(PackRadioFrame(aFrame, aError));
    SuccessOrExit(mEncoder.EndFrame());

exit:
    return;
}

void NcpBase::LinkRawTransmitDone(otInstance *, otRadioFrame *aFrame, otRadioFrame *aAckFrame, otError aError)
{
    sNcpInstance->LinkRawTransmitDone(aFrame, aAckFrame, aError);
}

void NcpBase::LinkRawTransmitDone(otRadioFrame *aFrame, otRadioFrame *aAckFrame, otError aError)
{
    OT_UNUSED_VARIABLE(aFrame);

    if (mCurTransmitTID)
    {
        uint8_t header        = SPINEL_HEADER_FLAG | SPINEL_HEADER_IID_0 | mCurTransmitTID;
        bool    framePending  = (aAckFrame != nullptr && static_cast<Mac::RxFrame *>(aAckFrame)->GetFramePending());
        bool    headerUpdated = static_cast<Mac::TxFrame *>(aFrame)->IsHeaderUpdated();

        // Clear cached transmit TID
        mCurTransmitTID = 0;

        SuccessOrExit(mEncoder.BeginFrame(header, SPINEL_CMD_PROP_VALUE_IS, SPINEL_PROP_LAST_STATUS));
        SuccessOrExit(mEncoder.WriteUintPacked(ThreadErrorToSpinelStatus(aError)));
        SuccessOrExit(mEncoder.WriteBool(framePending));
        SuccessOrExit(mEncoder.WriteBool(headerUpdated));

        if (aError == OT_ERROR_NONE)
        {
            SuccessOrExit(PackRadioFrame(aAckFrame, aError));
        }

        if (static_cast<Mac::TxFrame *>(aFrame)->GetSecurityEnabled() && headerUpdated)
        {
            uint8_t  keyId;
            uint32_t frameCounter;

            // Transmit frame auxiliary key index and frame counter
            SuccessOrExit(static_cast<Mac::TxFrame *>(aFrame)->GetKeyId(keyId));
            SuccessOrExit(static_cast<Mac::TxFrame *>(aFrame)->GetFrameCounter(frameCounter));

            SuccessOrExit(mEncoder.WriteUint8(keyId));
            SuccessOrExit(mEncoder.WriteUint32(frameCounter));
        }

        SuccessOrExit(mEncoder.EndFrame());
    }

exit:
    return;
}

void NcpBase::LinkRawEnergyScanDone(otInstance *, int8_t aEnergyScanMaxRssi)
{
    sNcpInstance->LinkRawEnergyScanDone(aEnergyScanMaxRssi);
}

void NcpBase::LinkRawEnergyScanDone(int8_t aEnergyScanMaxRssi)
{
    int8_t scanChannel = mCurScanChannel;

    // Clear current scan channel
    mCurScanChannel = kInvalidScanChannel;

    // Make sure we are back listening on the original receive channel,
    // since the energy scan could have been on a different channel.
    IgnoreError(otLinkRawReceive(mInstance));

    SuccessOrExit(mEncoder.BeginFrame(SPINEL_HEADER_FLAG | SPINEL_HEADER_IID_0, SPINEL_CMD_PROP_VALUE_IS,
                                      SPINEL_PROP_MAC_ENERGY_SCAN_RESULT));

    SuccessOrExit(mEncoder.WriteUint8(static_cast<uint8_t>(scanChannel)));
    SuccessOrExit(mEncoder.WriteInt8(aEnergyScanMaxRssi));
    SuccessOrExit(mEncoder.EndFrame());

    // We are finished with the scan, so send out
    // a property update indicating such.
    SuccessOrExit(mEncoder.BeginFrame(SPINEL_HEADER_FLAG | SPINEL_HEADER_IID_0, SPINEL_CMD_PROP_VALUE_IS,
                                      SPINEL_PROP_MAC_SCAN_STATE));

    SuccessOrExit(mEncoder.WriteUint8(SPINEL_SCAN_STATE_IDLE));
    SuccessOrExit(mEncoder.EndFrame());

exit:
    return;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_RADIO_CAPS>(void)
{
    return mEncoder.WriteUintPacked(otLinkRawGetCaps(mInstance));
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_MAC_SRC_MATCH_ENABLED>(void)
{
    // TODO: Would be good to add an `otLinkRaw` API to give the status of source match.
    return mEncoder.WriteBool(mSrcMatchEnabled);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_RCP_TIMESTAMP>(void)
{
    otError error = OT_ERROR_NONE;

    SuccessOrExit(error = mEncoder.WriteUint64(otLinkRawGetRadioTime(mInstance)));

exit:
    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_MAC_SRC_MATCH_ENABLED>(void)
{
    otError error = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadBool(mSrcMatchEnabled));

    error = otLinkRawSrcMatchEnable(mInstance, mSrcMatchEnabled);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_MAC_SRC_MATCH_SHORT_ADDRESSES>(void)
{
    otError error = OT_ERROR_NONE;

    // Clear the list first
    SuccessOrExit(error = otLinkRawSrcMatchClearShortEntries(mInstance));

    // Loop through the addresses and add them
    while (mDecoder.GetRemainingLengthInStruct() >= sizeof(uint16_t))
    {
        uint16_t shortAddress;

        SuccessOrExit(error = mDecoder.ReadUint16(shortAddress));

        SuccessOrExit(error = otLinkRawSrcMatchAddShortEntry(mInstance, shortAddress));
    }

exit:
    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_MAC_SRC_MATCH_EXTENDED_ADDRESSES>(void)
{
    otError error = OT_ERROR_NONE;

    // Clear the list first
    SuccessOrExit(error = otLinkRawSrcMatchClearExtEntries(mInstance));

    // Loop through the addresses and add them
    while (mDecoder.GetRemainingLengthInStruct() >= sizeof(otExtAddress))
    {
        const otExtAddress *extAddress;

        SuccessOrExit(error = mDecoder.ReadEui64(extAddress));

        SuccessOrExit(error = otLinkRawSrcMatchAddExtEntry(mInstance, extAddress));
    }

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyRemove<SPINEL_PROP_MAC_SRC_MATCH_SHORT_ADDRESSES>(void)
{
    otError  error = OT_ERROR_NONE;
    uint16_t shortAddress;

    SuccessOrExit(error = mDecoder.ReadUint16(shortAddress));

    error = otLinkRawSrcMatchClearShortEntry(mInstance, shortAddress);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyRemove<SPINEL_PROP_MAC_SRC_MATCH_EXTENDED_ADDRESSES>(void)
{
    otError             error = OT_ERROR_NONE;
    const otExtAddress *extAddress;

    SuccessOrExit(error = mDecoder.ReadEui64(extAddress));
    ;

    error = otLinkRawSrcMatchClearExtEntry(mInstance, extAddress);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyInsert<SPINEL_PROP_MAC_SRC_MATCH_SHORT_ADDRESSES>(void)
{
    otError  error = OT_ERROR_NONE;
    uint16_t shortAddress;

    SuccessOrExit(error = mDecoder.ReadUint16(shortAddress));

    error = otLinkRawSrcMatchAddShortEntry(mInstance, shortAddress);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyInsert<SPINEL_PROP_MAC_SRC_MATCH_EXTENDED_ADDRESSES>(void)
{
    otError             error      = OT_ERROR_NONE;
    const otExtAddress *extAddress = nullptr;

    SuccessOrExit(error = mDecoder.ReadEui64(extAddress));

    error = otLinkRawSrcMatchAddExtEntry(mInstance, extAddress);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_PHY_ENABLED>(void)
{
    bool    value = false;
    otError error = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadBool(value));

    if (!value)
    {
        error = otLinkRawSetReceiveDone(mInstance, nullptr);
    }
    else
    {
        error = otLinkRawSetReceiveDone(mInstance, &NcpBase::LinkRawReceiveDone);
    }

exit:
    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_MAC_15_4_SADDR>(void)
{
    uint16_t shortAddress;
    otError  error = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadUint16(shortAddress));

    error = otLinkRawSetShortAddress(mInstance, shortAddress);

exit:
    return error;
}

otError NcpBase::DecodeStreamRawTxRequest(otRadioFrame &aFrame)
{
    otError        error;
    const uint8_t *payloadPtr;
    uint16_t       payloadLen;
    bool           csmaEnable;
    bool           isARetx;
    bool           isHeaderUpdated;
    bool           isSecurityProcessed;

    SuccessOrExit(error = mDecoder.ReadDataWithLen(payloadPtr, payloadLen));
    VerifyOrExit(payloadLen <= OT_RADIO_FRAME_MAX_SIZE, error = OT_ERROR_PARSE);

    aFrame.mLength = static_cast<uint8_t>(payloadLen);
    memcpy(aFrame.mPsdu, payloadPtr, aFrame.mLength);

    // Parse the meta data

    // Channel is a required parameter in meta data.
    SuccessOrExit(error = mDecoder.ReadUint8(aFrame.mChannel));

    // Set the default value for all optional parameters.
    aFrame.mInfo.mTxInfo.mMaxCsmaBackoffs     = OPENTHREAD_CONFIG_MAC_MAX_CSMA_BACKOFFS_DIRECT;
    aFrame.mInfo.mTxInfo.mMaxFrameRetries     = OPENTHREAD_CONFIG_MAC_DEFAULT_MAX_FRAME_RETRIES_DIRECT;
    aFrame.mInfo.mTxInfo.mCsmaCaEnabled       = true;
    aFrame.mInfo.mTxInfo.mIsHeaderUpdated     = false;
    aFrame.mInfo.mTxInfo.mIsARetx             = false;
    aFrame.mInfo.mTxInfo.mIsSecurityProcessed = false;
    aFrame.mInfo.mTxInfo.mTxDelay             = 0;
    aFrame.mInfo.mTxInfo.mTxDelayBaseTime     = 0;

    // All the next parameters are optional. Note that even if the
    // decoder fails to parse any of optional parameters we still want to
    // return `OT_ERROR_NONE` (so `error` is not updated after this
    // point).

    SuccessOrExit(mDecoder.ReadUint8(aFrame.mInfo.mTxInfo.mMaxCsmaBackoffs));
    SuccessOrExit(mDecoder.ReadUint8(aFrame.mInfo.mTxInfo.mMaxFrameRetries));
    SuccessOrExit(mDecoder.ReadBool(csmaEnable));
    SuccessOrExit(mDecoder.ReadBool(isHeaderUpdated));
    SuccessOrExit(mDecoder.ReadBool(isARetx));
    SuccessOrExit(mDecoder.ReadBool(isSecurityProcessed));
    SuccessOrExit(mDecoder.ReadUint32(aFrame.mInfo.mTxInfo.mTxDelay));
    SuccessOrExit(mDecoder.ReadUint32(aFrame.mInfo.mTxInfo.mTxDelayBaseTime));
    aFrame.mInfo.mTxInfo.mCsmaCaEnabled       = csmaEnable;
    aFrame.mInfo.mTxInfo.mIsHeaderUpdated     = isHeaderUpdated;
    aFrame.mInfo.mTxInfo.mIsARetx             = isARetx;
    aFrame.mInfo.mTxInfo.mIsSecurityProcessed = isSecurityProcessed;

exit:
    return error;
}

otError NcpBase::HandlePropertySet_SPINEL_PROP_STREAM_RAW(uint8_t aHeader)
{
    otError       error = OT_ERROR_NONE;
    otRadioFrame *frame;

    VerifyOrExit(otLinkRawIsEnabled(mInstance), error = OT_ERROR_INVALID_STATE);

    frame = otLinkRawGetTransmitBuffer(mInstance);
    VerifyOrExit(frame != nullptr, error = OT_ERROR_NO_BUFS);

    SuccessOrExit(error = DecodeStreamRawTxRequest(*frame));

    // Cache the transaction ID for async response
    mCurTransmitTID = SPINEL_HEADER_GET_TID(aHeader);

    // Pass frame to the radio layer. Note, this fails if we
    // haven't enabled raw stream or are already transmitting.
    error = otLinkRawTransmit(mInstance, &NcpBase::LinkRawTransmitDone);

exit:

    if (error == OT_ERROR_NONE)
    {
        // Don't do anything here yet. We will complete the transaction when we get a transmit done callback
    }
    else
    {
        error = WriteLastStatusFrame(aHeader, ThreadErrorToSpinelStatus(error));
    }

    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_RCP_MAC_KEY>(void)
{
    otError        error = OT_ERROR_NONE;
    uint8_t        keyIdMode;
    uint8_t        keyId;
    uint16_t       keySize;
    const uint8_t *prevKey;
    const uint8_t *currKey;
    const uint8_t *nextKey;

    SuccessOrExit(error = mDecoder.ReadUint8(keyIdMode));
    VerifyOrExit(keyIdMode == Mac::Frame::kKeyIdMode1, error = OT_ERROR_INVALID_ARGS);

    SuccessOrExit(error = mDecoder.ReadUint8(keyId));

    SuccessOrExit(error = mDecoder.ReadDataWithLen(prevKey, keySize));
    VerifyOrExit(keySize == sizeof(otMacKey), error = OT_ERROR_INVALID_ARGS);

    SuccessOrExit(error = mDecoder.ReadDataWithLen(currKey, keySize));
    VerifyOrExit(keySize == sizeof(otMacKey), error = OT_ERROR_INVALID_ARGS);

    SuccessOrExit(error = mDecoder.ReadDataWithLen(nextKey, keySize));
    VerifyOrExit(keySize == sizeof(otMacKey), error = OT_ERROR_INVALID_ARGS);

    error =
        otLinkRawSetMacKey(mInstance, keyIdMode, keyId, reinterpret_cast<const otMacKey *>(prevKey),
                           reinterpret_cast<const otMacKey *>(currKey), reinterpret_cast<const otMacKey *>(nextKey));

exit:
    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_RCP_MAC_FRAME_COUNTER>(void)
{
    otError  error = OT_ERROR_NONE;
    uint32_t frameCounter;

    SuccessOrExit(error = mDecoder.ReadUint32(frameCounter));

    error = otLinkRawSetMacFrameCounter(mInstance, frameCounter);

exit:
    return error;
}

} // namespace Ncp
} // namespace ot

#endif // OPENTHREAD_RADIO || OPENTHREAD_CONFIG_LINK_RAW_ENABLE
