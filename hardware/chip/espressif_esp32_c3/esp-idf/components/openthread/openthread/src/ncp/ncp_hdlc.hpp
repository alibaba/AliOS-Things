/*
 *    Copyright (c) 2016, The OpenThread Authors.
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
 *   This file contains definitions for a HDLC based NCP interface to the OpenThread stack.
 */

#ifndef NCP_HDLC_HPP_
#define NCP_HDLC_HPP_

#include "openthread-core-config.h"

#include "lib/hdlc/hdlc.hpp"
#include "ncp/ncp_base.hpp"

#if OPENTHREAD_ENABLE_NCP_SPINEL_ENCRYPTER
#include "lib/spinel/spinel_encrypter.hpp"
#endif // OPENTHREAD_ENABLE_NCP_SPINEL_ENCRYPTER

namespace ot {
namespace Ncp {

class NcpHdlc : public NcpBase
{
    typedef NcpBase super_t;

public:
    /**
     * Constructor
     *
     * @param[in]  aInstance  The OpenThread instance structure.
     *
     */
    explicit NcpHdlc(Instance *aInstance, otNcpHdlcSendCallback aSendCallback);

    /**
     * This method is called when uart tx is finished. It prepares and sends the next data chunk (if any) to uart.
     *
     */
    void HandleHdlcSendDone(void);

    /**
     * This method is called when uart received a data buffer.
     *
     */
    void HandleHdlcReceiveDone(const uint8_t *aBuf, uint16_t aBufLength);

private:
    enum
    {
        kHdlcTxBufferSize = OPENTHREAD_CONFIG_NCP_HDLC_TX_CHUNK_SIZE,   // HDLC tx buffer size.
        kRxBufferSize     = OPENTHREAD_CONFIG_NCP_HDLC_RX_BUFFER_SIZE + // Rx buffer size (should be large enough to fit
                        OPENTHREAD_CONFIG_NCP_SPINEL_ENCRYPTER_EXTRA_DATA_SIZE, // one whole (decoded) received frame).
    };

    enum HdlcTxState
    {
        kStartingFrame,   // Starting a new frame.
        kEncodingFrame,   // In middle of encoding a frame.
        kFinalizingFrame, // Finalizing a frame.
    };

#if OPENTHREAD_ENABLE_NCP_SPINEL_ENCRYPTER
    /**
     * Wraps Spinel::Buffer allowing to read data through spinel encrypter.
     * Creates additional buffers to allow transforming of the whole spinel frames.
     */
    class BufferEncrypterReader
    {
    public:
        /**
         * C-tor.
         * Takes a reference to Spinel::Buffer in order to read spinel frames.
         */
        explicit BufferEncrypterReader(Spinel::Buffer &aTxFrameBuffer);
        bool    IsEmpty(void) const;
        otError OutFrameBegin(void);
        bool    OutFrameHasEnded(void);
        uint8_t OutFrameReadByte(void);
        otError OutFrameRemove(void);

    private:
        void Reset(void);

        Spinel::Buffer &mTxFrameBuffer;
        uint8_t         mDataBuffer[kRxBufferSize];
        size_t          mDataBufferReadIndex;
        size_t          mOutputDataLength;
    };
#endif // OPENTHREAD_ENABLE_NCP_SPINEL_ENCRYPTER

    void EncodeAndSend(void);
    void HandleFrame(otError aError);
    void HandleError(otError aError, uint8_t *aBuf, uint16_t aBufLength);
    void TxFrameBufferHasData(void);
    void HandleFrameAddedToNcpBuffer(void);

    static void           EncodeAndSend(Tasklet &aTasklet);
    static void           HandleFrame(void *aContext, otError aError);
    static void           HandleFrameAddedToNcpBuffer(void *                   aContext,
                                                      Spinel::Buffer::FrameTag aTag,
                                                      Spinel::Buffer::Priority aPriority,
                                                      Spinel::Buffer *         aBuffer);
    otNcpHdlcSendCallback mSendCallback;

    Hdlc::Encoder                        mFrameEncoder;
    Hdlc::Decoder                        mFrameDecoder;
    Hdlc::FrameBuffer<kHdlcTxBufferSize> mHdlcBuffer;
    HdlcTxState                          mState;
    uint8_t                              mByte;
    Hdlc::FrameBuffer<kRxBufferSize>     mRxBuffer;
    bool                                 mHdlcSendImmediate;
    Tasklet                              mHdlcSendTask;

#if OPENTHREAD_ENABLE_NCP_SPINEL_ENCRYPTER
    BufferEncrypterReader mTxFrameBufferEncrypterReader;
#endif // OPENTHREAD_ENABLE_NCP_SPINEL_ENCRYPTER
};

} // namespace Ncp
} // namespace ot

#endif // NCP_HDLC_HPP_
