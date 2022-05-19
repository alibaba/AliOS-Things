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

#include <ctype.h>

#include "common/code_utils.hpp"
#include "common/instance.hpp"
#include "lib/hdlc/hdlc.hpp"

#include "test_util.h"

namespace ot {
namespace Ncp {

enum
{
    kBufferSize        = 1500,  // Frame buffer size
    kMaxFrameLength    = 500,   // Maximum allowed frame length (used when randomly generating frames)
    kFuzzTestIteration = 50000, // Number of iteration during fuzz test (randomly generating frames)
    kFrameHeaderSize   = 4,     // Frame header size

    kFlagXOn        = 0x11,
    kFlagXOff       = 0x13,
    kFlagSequence   = 0x7e, ///< HDLC Flag value
    kEscapeSequence = 0x7d, ///< HDLC Escape value
    kFlagSpecial    = 0xf8,

};

static const uint8_t sOpenThreadText[] = "OpenThread Rocks";
static const uint8_t sHelloText[]      = "Hello there!";
static const uint8_t sMottoText[]      = "Think good thoughts, say good words, do good deeds!";
static const uint8_t sHexText[]        = "0123456789abcdef";
static const uint8_t sSkipText[]       = "Skip text";
static const uint8_t sHdlcSpecials[]   = {kFlagSequence, kFlagXOn,        kFlagXOff,
                                        kFlagSequence, kEscapeSequence, kFlagSpecial};

otError WriteToBuffer(const uint8_t *aText, Hdlc::FrameWritePointer &aWritePointer)
{
    otError error = OT_ERROR_NONE;

    while (*aText != 0)
    {
        SuccessOrExit(aWritePointer.WriteByte(*aText++));
    }

exit:
    return error;
}

void TestHdlcFrameBuffer(void)
{
    Hdlc::FrameBuffer<kBufferSize> frameBuffer;

    printf("Testing Hdlc::FrameBuffer");

    VerifyOrQuit(frameBuffer.IsEmpty(), "after constructor");
    VerifyOrQuit(frameBuffer.GetLength() == 0, "after constructor");

    SuccessOrQuit(WriteToBuffer(sOpenThreadText, frameBuffer));

    VerifyOrQuit(frameBuffer.GetLength() == sizeof(sOpenThreadText) - 1);
    VerifyOrQuit(memcmp(frameBuffer.GetFrame(), sOpenThreadText, frameBuffer.GetLength()) == 0);

    VerifyOrQuit(frameBuffer.CanWrite(1));
    VerifyOrQuit(!frameBuffer.IsEmpty());

    SuccessOrQuit(WriteToBuffer(sHelloText, frameBuffer));
    VerifyOrQuit(frameBuffer.GetLength() == sizeof(sOpenThreadText) + sizeof(sHelloText) - 2);

    frameBuffer.UndoLastWrites(sizeof(sHelloText) - 1);
    VerifyOrQuit(frameBuffer.GetLength() == sizeof(sOpenThreadText) - 1);
    VerifyOrQuit(memcmp(frameBuffer.GetFrame(), sOpenThreadText, frameBuffer.GetLength()) == 0);

    VerifyOrQuit(!frameBuffer.IsEmpty());
    frameBuffer.Clear();
    VerifyOrQuit(frameBuffer.IsEmpty(), "after Clear()");
    VerifyOrQuit(frameBuffer.GetLength() == 0, "after Clear()");

    SuccessOrQuit(WriteToBuffer(sMottoText, frameBuffer));

    VerifyOrQuit(frameBuffer.GetLength() == sizeof(sMottoText) - 1);
    VerifyOrQuit(memcmp(frameBuffer.GetFrame(), sMottoText, frameBuffer.GetLength()) == 0);

    frameBuffer.Clear();
    VerifyOrQuit(frameBuffer.CanWrite(kBufferSize));
    VerifyOrQuit(frameBuffer.CanWrite(kBufferSize + 1) == false, "CanWrite(kBufferSize + 1) did not fail as expected");

    for (uint16_t i = 0; i < kBufferSize; i++)
    {
        VerifyOrQuit(frameBuffer.CanWrite(1));
        SuccessOrQuit(frameBuffer.WriteByte(i & 0xff));
    }

    VerifyOrQuit(frameBuffer.CanWrite(1) == false, "did not fail with full buffer");
    VerifyOrQuit(frameBuffer.WriteByte(0) == OT_ERROR_NO_BUFS, "did not fail with full buffer");

    printf(" -- PASS\n");
}

void TestHdlcMultiFrameBuffer(void)
{
    Hdlc::MultiFrameBuffer<kBufferSize> frameBuffer;
    uint8_t *                           frame    = nullptr;
    uint8_t *                           newFrame = nullptr;
    uint16_t                            length;
    uint16_t                            newLength;

    printf("Testing Hdlc::MultiFrameBuffer");

    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Check state after constructor

    VerifyOrQuit(!frameBuffer.HasFrame(), "after constructor");
    VerifyOrQuit(!frameBuffer.HasSavedFrame(), "after constructor");
    VerifyOrQuit(frameBuffer.GetLength() == 0, "after constructor");
    VerifyOrQuit(frameBuffer.GetNextSavedFrame(frame, length) == OT_ERROR_NOT_FOUND, "after constructor");

    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Write multiple frames, save them and read later

    SuccessOrQuit(WriteToBuffer(sMottoText, frameBuffer));

    VerifyOrQuit(frameBuffer.GetLength() == sizeof(sMottoText) - 1);
    VerifyOrQuit(memcmp(frameBuffer.GetFrame(), sMottoText, frameBuffer.GetLength()) == 0);

    frameBuffer.SaveFrame();

    VerifyOrQuit(!frameBuffer.HasFrame(), "after SaveFrame()");
    VerifyOrQuit(frameBuffer.HasSavedFrame(), "after SaveFrame()");
    VerifyOrQuit(frameBuffer.GetLength() == 0, "after SaveFrame()");

    SuccessOrQuit(WriteToBuffer(sHelloText, frameBuffer));
    VerifyOrQuit(frameBuffer.GetLength() == sizeof(sHelloText) - 1);
    VerifyOrQuit(memcmp(frameBuffer.GetFrame(), sHelloText, frameBuffer.GetLength()) == 0);

    frameBuffer.SaveFrame();

    VerifyOrQuit(!frameBuffer.HasFrame(), "after SaveFrame()");
    VerifyOrQuit(frameBuffer.HasSavedFrame(), "after SaveFrame()");
    VerifyOrQuit(frameBuffer.GetLength() == 0, "after SaveFrame()");

    SuccessOrQuit(WriteToBuffer(sOpenThreadText, frameBuffer));
    VerifyOrQuit(frameBuffer.GetLength() == sizeof(sOpenThreadText) - 1);
    VerifyOrQuit(memcmp(frameBuffer.GetFrame(), sOpenThreadText, frameBuffer.GetLength()) == 0);

    frameBuffer.DiscardFrame();

    VerifyOrQuit(!frameBuffer.HasFrame(), "after DiscardFrame()");
    VerifyOrQuit(frameBuffer.HasSavedFrame(), "after SaveFrame()");
    VerifyOrQuit(frameBuffer.GetLength() == 0, "after DiscardFrame()");

    SuccessOrQuit(WriteToBuffer(sMottoText, frameBuffer));
    VerifyOrQuit(frameBuffer.GetLength() == sizeof(sMottoText) - 1);
    VerifyOrQuit(memcmp(frameBuffer.GetFrame(), sMottoText, frameBuffer.GetLength()) == 0);

    frameBuffer.DiscardFrame();

    VerifyOrQuit(!frameBuffer.HasFrame(), "after DiscardFrame()");
    VerifyOrQuit(frameBuffer.GetLength() == 0, "after DiscardFrame()");

    SuccessOrQuit(WriteToBuffer(sHexText, frameBuffer));
    VerifyOrQuit(frameBuffer.GetLength() == sizeof(sHexText) - 1);
    VerifyOrQuit(memcmp(frameBuffer.GetFrame(), sHexText, frameBuffer.GetLength()) == 0);

    frameBuffer.SaveFrame();

    VerifyOrQuit(!frameBuffer.HasFrame(), "after SaveFrame()");
    VerifyOrQuit(frameBuffer.HasSavedFrame(), "after SaveFrame()");
    VerifyOrQuit(frameBuffer.GetLength() == 0, "after SaveFrame()");

    SuccessOrQuit(WriteToBuffer(sOpenThreadText, frameBuffer));
    VerifyOrQuit(frameBuffer.GetLength() == sizeof(sOpenThreadText) - 1);
    VerifyOrQuit(memcmp(frameBuffer.GetFrame(), sOpenThreadText, frameBuffer.GetLength()) == 0);

    // Read the first saved frame and check the content
    frame = nullptr;
    SuccessOrQuit(frameBuffer.GetNextSavedFrame(frame, length));
    VerifyOrQuit(length == sizeof(sMottoText) - 1, "GetNextSavedFrame() length is incorrect");
    VerifyOrQuit(memcmp(frame, sMottoText, length) == 0, "GetNextSavedFrame() frame content is incorrect");

    // Read the second saved frame and check the content
    SuccessOrQuit(frameBuffer.GetNextSavedFrame(frame, length));
    VerifyOrQuit(length == sizeof(sHelloText) - 1, "GetNextSavedFrame() length is incorrect");
    VerifyOrQuit(memcmp(frame, sHelloText, length) == 0, "GetNextSavedFrame() frame content is incorrect");

    // Read the third saved frame and check the content
    SuccessOrQuit(frameBuffer.GetNextSavedFrame(frame, length));
    VerifyOrQuit(length == sizeof(sHexText) - 1, "GetNextSavedFrame() length is incorrect");
    VerifyOrQuit(memcmp(frame, sHexText, length) == 0, "GetNextSavedFrame() frame content is incorrect");

    newFrame  = frame;
    newLength = length;
    VerifyOrQuit(frameBuffer.GetNextSavedFrame(newFrame, newLength) == OT_ERROR_NOT_FOUND,
                 "GetNextSavedFrame() incorrect behavior after all frames were read");
    VerifyOrQuit(newFrame == nullptr, "GetNextSavedFrame() incorrect behavior after all frames were read");

    VerifyOrQuit(frameBuffer.GetLength() == sizeof(sOpenThreadText) - 1);
    VerifyOrQuit(memcmp(frameBuffer.GetFrame(), sOpenThreadText, frameBuffer.GetLength()) == 0,
                 "GetFrame() content is incorrect");

    frameBuffer.SaveFrame();

    // Read the fourth saved frame and check the content
    SuccessOrQuit(frameBuffer.GetNextSavedFrame(frame, length));
    VerifyOrQuit(length == sizeof(sOpenThreadText) - 1, "GetNextSavedFrame() length is incorrect");
    VerifyOrQuit(memcmp(frame, sOpenThreadText, length) == 0, "GetNextSavedFrame() frame content is incorrect");

    // Re-read all the saved frames
    frame = nullptr;
    SuccessOrQuit(frameBuffer.GetNextSavedFrame(frame, length));
    VerifyOrQuit(length == sizeof(sMottoText) - 1, "GetNextSavedFrame() length is incorrect");
    VerifyOrQuit(memcmp(frame, sMottoText, length) == 0, "GetNextSavedFrame() frame content is incorrect");

    // Second saved frame
    SuccessOrQuit(frameBuffer.GetNextSavedFrame(frame, length));
    VerifyOrQuit(length == sizeof(sHelloText) - 1, "GetNextSavedFrame() length is incorrect");
    VerifyOrQuit(memcmp(frame, sHelloText, length) == 0, "GetNextSavedFrame() frame content is incorrect");

    // Third saved frame
    SuccessOrQuit(frameBuffer.GetNextSavedFrame(frame, length));
    VerifyOrQuit(length == sizeof(sHexText) - 1, "GetNextSavedFrame() length is incorrect");
    VerifyOrQuit(memcmp(frame, sHexText, length) == 0, "GetNextSavedFrame() frame content is incorrect");

    // Fourth saved frame and check the content
    SuccessOrQuit(frameBuffer.GetNextSavedFrame(frame, length));
    VerifyOrQuit(length == sizeof(sOpenThreadText) - 1, "GetNextSavedFrame() length is incorrect");
    VerifyOrQuit(memcmp(frame, sOpenThreadText, length) == 0, "GetNextSavedFrame() frame content is incorrect");

    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Verify behavior of `Clear()`

    frameBuffer.Clear();

    VerifyOrQuit(!frameBuffer.HasFrame(), "after Clear()");
    VerifyOrQuit(!frameBuffer.HasSavedFrame(), "after Clear()");
    VerifyOrQuit(frameBuffer.GetLength() == 0, "after Clear()");

    SuccessOrQuit(WriteToBuffer(sOpenThreadText, frameBuffer));
    frameBuffer.SaveFrame();

    SuccessOrQuit(WriteToBuffer(sHelloText, frameBuffer));
    frameBuffer.SaveFrame();
    VerifyOrQuit(frameBuffer.HasSavedFrame(), "after SaveFrame()");

    frame = nullptr;
    SuccessOrQuit(frameBuffer.GetNextSavedFrame(frame, length));
    VerifyOrQuit(frameBuffer.HasSavedFrame(), "after GetNextSavedFrame()");

    frameBuffer.Clear();

    frame = nullptr;
    VerifyOrQuit(frameBuffer.GetNextSavedFrame(frame, length) == OT_ERROR_NOT_FOUND, "after Clear()");

    VerifyOrQuit(!frameBuffer.HasFrame(), "after Clear()");
    VerifyOrQuit(!frameBuffer.HasSavedFrame(), "after Clear()");
    VerifyOrQuit(frameBuffer.CanWrite(kBufferSize - (kFrameHeaderSize - 1)) == false, "after Clear()");
    VerifyOrQuit(frameBuffer.CanWrite(kBufferSize - kFrameHeaderSize) == true, "after Clear()");

    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Verify behavior of `ClearSavedFrames()`

    SuccessOrQuit(WriteToBuffer(sHelloText, frameBuffer));
    frameBuffer.SaveFrame();
    SuccessOrQuit(WriteToBuffer(sOpenThreadText, frameBuffer));
    frameBuffer.SaveFrame();
    SuccessOrQuit(WriteToBuffer(sMottoText, frameBuffer));
    frameBuffer.SaveFrame();
    SuccessOrQuit(WriteToBuffer(sHexText, frameBuffer));

    frame = nullptr;
    SuccessOrQuit(frameBuffer.GetNextSavedFrame(frame, length));
    VerifyOrQuit(length == sizeof(sHelloText) - 1, "GetNextSavedFrame() length is incorrect");
    VerifyOrQuit(memcmp(frame, sHelloText, length) == 0, "GetNextSavedFrame() frame content is incorrect");

    frameBuffer.ClearSavedFrames();

    VerifyOrQuit(frameBuffer.HasFrame(), "after ClearSavedFrames()");
    VerifyOrQuit(!frameBuffer.HasSavedFrame(), "after ClearSavedFrames()");

    VerifyOrQuit(frameBuffer.GetLength() == sizeof(sHexText) - 1, "after ClearSavedFrames()");
    VerifyOrQuit(memcmp(frameBuffer.GetFrame(), sHexText, frameBuffer.GetLength()) == 0);

    frameBuffer.SaveFrame();

    SuccessOrQuit(WriteToBuffer(sHelloText, frameBuffer));

    frame = nullptr;
    SuccessOrQuit(frameBuffer.GetNextSavedFrame(frame, length));
    VerifyOrQuit(length == sizeof(sHexText) - 1, "GetNextSavedFrame() length is incorrect");
    VerifyOrQuit(memcmp(frame, sHexText, length) == 0, "GetNextSavedFrame() frame content is incorrect");

    VerifyOrQuit(frameBuffer.GetLength() == sizeof(sHelloText) - 1, "after ClearSavedFrames()");
    VerifyOrQuit(memcmp(frameBuffer.GetFrame(), sHelloText, frameBuffer.GetLength()) == 0, "after ClearSavedFrames()");

    frameBuffer.ClearSavedFrames();
    frameBuffer.DiscardFrame();

    VerifyOrQuit(!frameBuffer.HasFrame(), "after all frames are read and discarded");
    VerifyOrQuit(!frameBuffer.HasSavedFrame(), "after all read or discarded");
    VerifyOrQuit(frameBuffer.CanWrite(kBufferSize - (kFrameHeaderSize - 1)) == false, "after all read or discarded");
    VerifyOrQuit(frameBuffer.CanWrite(kBufferSize - kFrameHeaderSize) == true, "after all read of discarded");

    SuccessOrQuit(WriteToBuffer(sHelloText, frameBuffer));

    frameBuffer.ClearSavedFrames();

    VerifyOrQuit(frameBuffer.GetLength() == sizeof(sHelloText) - 1);
    VerifyOrQuit(memcmp(frameBuffer.GetFrame(), sHelloText, frameBuffer.GetLength()) == 0);

    frameBuffer.SaveFrame();
    frame = nullptr;
    SuccessOrQuit(frameBuffer.GetNextSavedFrame(frame, length));
    VerifyOrQuit(length == sizeof(sHelloText) - 1, "GetNextSavedFrame() length is incorrect");
    VerifyOrQuit(memcmp(frame, sHelloText, length) == 0, "GetNextSavedFrame() frame content is incorrect");

    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Verify behavior of `SetSkipLength()` and `GetSkipLength()`

    frameBuffer.Clear();

    VerifyOrQuit(frameBuffer.GetSkipLength() == 0, "after Clear()");
    VerifyOrQuit(frameBuffer.SetSkipLength(sizeof(sSkipText)) == OT_ERROR_NONE);
    SuccessOrQuit(WriteToBuffer(sMottoText, frameBuffer));
    VerifyOrQuit(memcmp(frameBuffer.GetFrame(), sMottoText, frameBuffer.GetLength()) == 0);
    memcpy(frameBuffer.GetFrame() - sizeof(sSkipText), sSkipText, sizeof(sSkipText));
    VerifyOrQuit(frameBuffer.GetSkipLength() == sizeof(sSkipText));
    VerifyOrQuit(frameBuffer.GetLength() == sizeof(sMottoText) - 1);
    VerifyOrQuit(memcmp(frameBuffer.GetFrame(), sMottoText, frameBuffer.GetLength()) == 0);

    frameBuffer.SaveFrame();
    VerifyOrQuit(!frameBuffer.HasFrame(), "after SaveFrame()");
    VerifyOrQuit(frameBuffer.HasSavedFrame(), "after SaveFrame()");
    VerifyOrQuit(frameBuffer.GetSkipLength() == 0, "after SaveFrame()");

    VerifyOrQuit(frameBuffer.SetSkipLength(sizeof(sSkipText)) == OT_ERROR_NONE);
    SuccessOrQuit(WriteToBuffer(sOpenThreadText, frameBuffer));
    VerifyOrQuit(memcmp(frameBuffer.GetFrame(), sOpenThreadText, frameBuffer.GetLength()) == 0);
    memcpy(frameBuffer.GetFrame() - sizeof(sSkipText), sSkipText, sizeof(sSkipText));
    VerifyOrQuit(frameBuffer.GetSkipLength() == sizeof(sSkipText));
    VerifyOrQuit(frameBuffer.GetLength() == sizeof(sOpenThreadText) - 1);
    VerifyOrQuit(memcmp(frameBuffer.GetFrame(), sOpenThreadText, frameBuffer.GetLength()) == 0);

    frameBuffer.SaveFrame();
    VerifyOrQuit(!frameBuffer.HasFrame(), "after SaveFrame()");
    VerifyOrQuit(frameBuffer.HasSavedFrame(), "after SaveFrame()");
    VerifyOrQuit(frameBuffer.GetSkipLength() == 0, "after SaveFrame()");

    frame = nullptr;
    SuccessOrQuit(frameBuffer.GetNextSavedFrame(frame, length));
    VerifyOrQuit(length == sizeof(sMottoText) - 1, "GetNextSavedFrame() length is incorrect");
    VerifyOrQuit(memcmp(frame, sMottoText, length) == 0, "GetNextSavedFrame() frame content is incorrect");
    VerifyOrQuit(memcmp(frame - sizeof(sSkipText), sSkipText, sizeof(sSkipText)) == 0,
                 "GetNextSavedFrame() reserved frame buffer content is incorrect");

    SuccessOrQuit(frameBuffer.GetNextSavedFrame(frame, length));
    VerifyOrQuit(length == sizeof(sOpenThreadText) - 1, "GetNextSavedFrame() length is incorrect");
    VerifyOrQuit(memcmp(frame, sOpenThreadText, length) == 0, "GetNextSavedFrame() frame content is incorrect");
    VerifyOrQuit(memcmp(frame - sizeof(sSkipText), sSkipText, sizeof(sSkipText)) == 0,
                 "GetNextSavedFrame() reserved frame buffer content is incorrect");

    frameBuffer.Clear();
    VerifyOrQuit(frameBuffer.SetSkipLength(kBufferSize - (kFrameHeaderSize - 1)) == OT_ERROR_NO_BUFS, "after Clear()");
    VerifyOrQuit(frameBuffer.SetSkipLength(kBufferSize - kFrameHeaderSize) == OT_ERROR_NONE, "after Clear()");

    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Verify behavior of `SetLength()` and `GetLength()`

    frameBuffer.Clear();
    VerifyOrQuit((frame = frameBuffer.GetFrame()) != nullptr);
    memcpy(frame, sHelloText, sizeof(sHelloText));
    VerifyOrQuit(frameBuffer.SetLength(sizeof(sHelloText)) == OT_ERROR_NONE);
    VerifyOrQuit(frameBuffer.GetLength() == sizeof(sHelloText));
    VerifyOrQuit(frameBuffer.HasFrame());
    frameBuffer.SaveFrame();

    VerifyOrQuit((frame = frameBuffer.GetFrame()) != nullptr);
    memcpy(frame, sMottoText, sizeof(sMottoText));
    VerifyOrQuit(frameBuffer.SetLength(sizeof(sMottoText)) == OT_ERROR_NONE);
    VerifyOrQuit(frameBuffer.GetLength() == sizeof(sMottoText));
    VerifyOrQuit(frameBuffer.HasFrame());
    frameBuffer.SaveFrame();

    VerifyOrQuit((frame = frameBuffer.GetFrame()) != nullptr);
    memcpy(frame, sHexText, sizeof(sHexText));
    VerifyOrQuit(frameBuffer.SetLength(sizeof(sHexText)) == OT_ERROR_NONE);
    VerifyOrQuit(frameBuffer.GetLength() == sizeof(sHexText));
    frameBuffer.DiscardFrame();
    VerifyOrQuit(!frameBuffer.HasFrame());

    frame = nullptr;
    SuccessOrQuit(frameBuffer.GetNextSavedFrame(frame, length));
    VerifyOrQuit(length == sizeof(sHelloText), "GetNextSavedFrame() length is incorrect");
    VerifyOrQuit(memcmp(frame, sHelloText, length) == 0, "GetNextSavedFrame() frame content is incorrect");

    SuccessOrQuit(frameBuffer.GetNextSavedFrame(frame, length));
    VerifyOrQuit(length == sizeof(sMottoText), "GetNextSavedFrame() length is incorrect");
    VerifyOrQuit(memcmp(frame, sMottoText, length) == 0, "GetNextSavedFrame() frame content is incorrect");

    SuccessOrQuit(!frameBuffer.GetNextSavedFrame(frame, length));

    frameBuffer.Clear();
    VerifyOrQuit(frameBuffer.SetLength(kBufferSize - (kFrameHeaderSize - 1)) == OT_ERROR_NO_BUFS, "after Clear()");
    VerifyOrQuit(frameBuffer.SetLength(kBufferSize - kFrameHeaderSize) == OT_ERROR_NONE, "after Clear()");

    printf(" -- PASS\n");
}

struct DecoderContext
{
    bool    mWasCalled;
    otError mError;
};

void ProcessDecodedFrame(void *aContext, otError aError)
{
    DecoderContext &decoderContext = *static_cast<DecoderContext *>(aContext);

    decoderContext.mError     = aError;
    decoderContext.mWasCalled = true;
}

void TestEncoderDecoder(void)
{
    otError                             error;
    uint8_t                             byte;
    Hdlc::MultiFrameBuffer<kBufferSize> encoderBuffer;
    Hdlc::MultiFrameBuffer<kBufferSize> decoderBuffer;
    DecoderContext                      decoderContext;
    Hdlc::Encoder                       encoder(encoderBuffer);
    Hdlc::Decoder                       decoder(decoderBuffer, ProcessDecodedFrame, &decoderContext);
    uint8_t *                           frame;
    uint16_t                            length;
    uint8_t                             badShortFrame[3] = {kFlagSequence, 0xaa, kFlagSequence};

    printf("Testing Hdlc::Encoder and Hdlc::Decoder");

    SuccessOrQuit(encoder.BeginFrame());
    SuccessOrQuit(encoder.Encode(sOpenThreadText, sizeof(sOpenThreadText) - 1));
    SuccessOrQuit(encoder.EndFrame());
    encoderBuffer.SaveFrame();

    SuccessOrQuit(encoder.BeginFrame());
    SuccessOrQuit(encoder.Encode(sMottoText, sizeof(sMottoText) - 1));
    SuccessOrQuit(encoder.EndFrame());
    encoderBuffer.SaveFrame();

    SuccessOrQuit(encoder.BeginFrame());
    SuccessOrQuit(encoder.Encode(sHdlcSpecials, sizeof(sHdlcSpecials)));
    SuccessOrQuit(encoder.EndFrame());
    encoderBuffer.SaveFrame();

    SuccessOrQuit(encoder.BeginFrame());
    SuccessOrQuit(encoder.Encode(sHelloText, sizeof(sHelloText) - 1));
    SuccessOrQuit(encoder.EndFrame());
    encoderBuffer.SaveFrame();

    SuccessOrQuit(encoder.BeginFrame());
    // Empty frame
    SuccessOrQuit(encoder.EndFrame());
    encoderBuffer.SaveFrame();

    byte = kFlagSequence;
    SuccessOrQuit(encoder.BeginFrame());
    SuccessOrQuit(encoder.Encode(&byte, sizeof(uint8_t)));
    SuccessOrQuit(encoder.EndFrame());
    encoderBuffer.SaveFrame();

    // Feed the encoded frames to decoder and save the content
    for (frame = nullptr; encoderBuffer.GetNextSavedFrame(frame, length) == OT_ERROR_NONE;)
    {
        decoderContext.mWasCalled = false;

        decoder.Decode(frame, length);

        VerifyOrQuit(decoderContext.mWasCalled);
        VerifyOrQuit(decoderContext.mError == OT_ERROR_NONE, "Decoder::Decode() returned incorrect error code");

        decoderBuffer.SaveFrame();
    }

    // Verify the decoded frames match the original frames
    frame = nullptr;
    SuccessOrQuit(decoderBuffer.GetNextSavedFrame(frame, length));
    VerifyOrQuit(length == sizeof(sOpenThreadText) - 1, "Decoded frame length does not match original frame");
    VerifyOrQuit(memcmp(frame, sOpenThreadText, length) == 0, "Decoded frame content does not match original frame");

    SuccessOrQuit(decoderBuffer.GetNextSavedFrame(frame, length));
    VerifyOrQuit(length == sizeof(sMottoText) - 1, "Decoded frame length does not match original frame");
    VerifyOrQuit(memcmp(frame, sMottoText, length) == 0, "Decoded frame content does not match original frame");

    SuccessOrQuit(decoderBuffer.GetNextSavedFrame(frame, length));
    VerifyOrQuit(length == sizeof(sHdlcSpecials), "Decoded frame length does not match original frame");
    VerifyOrQuit(memcmp(frame, sHdlcSpecials, length) == 0, "Decoded frame content does not match original frame");

    SuccessOrQuit(decoderBuffer.GetNextSavedFrame(frame, length));
    VerifyOrQuit(length == sizeof(sHelloText) - 1, "Decoded frame length does not match original frame");
    VerifyOrQuit(memcmp(frame, sHelloText, length) == 0, "Decoded frame content does not match original frame");

    SuccessOrQuit(decoderBuffer.GetNextSavedFrame(frame, length));
    VerifyOrQuit(length == 0, "Decoded frame length does not match original frame");

    SuccessOrQuit(decoderBuffer.GetNextSavedFrame(frame, length));
    VerifyOrQuit(length == sizeof(uint8_t), "Decoded frame length does not match original frame");
    VerifyOrQuit(*frame == kFlagSequence, "Decoded frame content does not match original frame");

    VerifyOrQuit(decoderBuffer.GetNextSavedFrame(frame, length) == OT_ERROR_NOT_FOUND, "Extra decoded frame");

    encoderBuffer.Clear();
    decoderBuffer.Clear();

    // Test `Encoder` behavior when running out of buffer space
    SuccessOrQuit(encoder.BeginFrame());

    error = OT_ERROR_NONE;

    for (uint16_t i = 0; error == OT_ERROR_NONE; i++)
    {
        byte  = i & 0xff;
        error = encoder.Encode(&byte, sizeof(uint8_t));
    }

    VerifyOrQuit(encoder.Encode(&byte, sizeof(uint8_t)) == OT_ERROR_NO_BUFS,
                 "Encoder::Encode() did not fail with a full buffer");
    VerifyOrQuit(encoder.EndFrame(), "Encoder::EndFrame() did not fail with a full buffer");

    encoderBuffer.Clear();

    // Test `Decoder` behavior with incorrect FCS

    SuccessOrQuit(encoder.BeginFrame());
    SuccessOrQuit(encoder.Encode(sMottoText, sizeof(sMottoText) - 1));
    SuccessOrQuit(encoder.EndFrame());

    encoderBuffer.GetFrame()[0] ^= 0x0a; // Change the first byte in the frame to cause FCS failure

    decoderContext.mWasCalled = false;
    decoder.Decode(encoderBuffer.GetFrame(), encoderBuffer.GetLength());
    VerifyOrQuit(decoderContext.mWasCalled);
    VerifyOrQuit(decoderContext.mError == OT_ERROR_PARSE, "Decoder::Decode() did not fail with bad FCS");

    decoderBuffer.Clear();

    // Test `Decoder` behavior with short frame (smaller than FCS)

    decoderContext.mWasCalled = false;
    decoder.Decode(badShortFrame, sizeof(badShortFrame));
    VerifyOrQuit(decoderContext.mWasCalled);
    VerifyOrQuit(decoderContext.mError == OT_ERROR_PARSE, "Decoder::Decode() did not fail for short frame");

    decoderBuffer.Clear();

    // Test `Decoder` with back to back `kFlagSequence` and ensure callback is not invoked.

    byte                      = kFlagSequence;
    decoderContext.mWasCalled = false;
    decoder.Decode(&byte, sizeof(uint8_t));
    VerifyOrQuit(!decoderContext.mWasCalled);
    decoder.Decode(&byte, sizeof(uint8_t));
    VerifyOrQuit(!decoderContext.mWasCalled);
    decoder.Decode(&byte, sizeof(uint8_t));
    VerifyOrQuit(!decoderContext.mWasCalled);
    decoder.Decode(&byte, sizeof(uint8_t));
    VerifyOrQuit(!decoderContext.mWasCalled);

    printf(" -- PASS\n");
}

uint32_t GetRandom(uint32_t max)
{
    return static_cast<uint32_t>(rand()) % max;
}

void TestFuzzEncoderDecoder(void)
{
    uint16_t                       length;
    uint8_t                        frame[kMaxFrameLength];
    Hdlc::FrameBuffer<kBufferSize> encoderBuffer;
    Hdlc::FrameBuffer<kBufferSize> decoderBuffer;
    DecoderContext                 decoderContext;
    Hdlc::Encoder                  encoder(encoderBuffer);
    Hdlc::Decoder                  decoder(decoderBuffer, ProcessDecodedFrame, &decoderContext);

    printf("Testing Hdlc::Encoder and Hdlc::Decoder with randomly generated frames");

    for (uint32_t iter = 0; iter < kFuzzTestIteration; iter++)
    {
        encoderBuffer.Clear();
        decoderBuffer.Clear();

        do
        {
            length = static_cast<uint16_t>(GetRandom(kMaxFrameLength));
        } while (length == 0);

        for (uint16_t i = 0; i < length; i++)
        {
            frame[i] = static_cast<uint8_t>(GetRandom(256));
        }

        SuccessOrQuit(encoder.BeginFrame());
        SuccessOrQuit(encoder.Encode(frame, length));
        SuccessOrQuit(encoder.EndFrame());

        VerifyOrQuit(!encoderBuffer.IsEmpty(), "Encoded frame is empty");
        VerifyOrQuit(encoderBuffer.GetLength() > length, "Encoded frame is too short");

        decoderContext.mWasCalled = false;
        decoder.Decode(encoderBuffer.GetFrame(), encoderBuffer.GetLength());
        VerifyOrQuit(decoderContext.mWasCalled);
        VerifyOrQuit(decoderContext.mError == OT_ERROR_NONE, "Decoder::Decode() returned incorrect error code");

        VerifyOrQuit(!decoderBuffer.IsEmpty(), "Decoded frame is empty");
        VerifyOrQuit(decoderBuffer.GetLength() == length, "Decoded frame length does not match original frame");
        VerifyOrQuit(memcmp(decoderBuffer.GetFrame(), frame, length) == 0,
                     "Decoded frame content does not match original frame");
    }

    printf(" -- PASS\n");
}

} // namespace Ncp
} // namespace ot

int main(void)
{
    ot::Ncp::TestHdlcFrameBuffer();
    ot::Ncp::TestHdlcMultiFrameBuffer();
    ot::Ncp::TestEncoderDecoder();
    ot::Ncp::TestFuzzEncoderDecoder();
    printf("\nAll tests passed.\n");
    return 0;
}
