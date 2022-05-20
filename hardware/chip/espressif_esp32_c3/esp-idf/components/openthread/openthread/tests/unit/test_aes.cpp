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

#include <openthread/config.h>

#include "common/debug.hpp"
#include "crypto/aes_ccm.hpp"

#include "test_platform.h"
#include "test_util.h"

/**
 * Verifies test vectors from IEEE 802.15.4-2006 Annex C Section C.2.1
 */
void TestMacBeaconFrame(void)
{
    uint8_t key[] = {
        0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
    };

    uint8_t test[] = {0x08, 0xD0, 0x84, 0x21, 0x43, 0x01, 0x00, 0x00, 0x00, 0x00, 0x48, 0xDE,
                      0xAC, 0x02, 0x05, 0x00, 0x00, 0x00, 0x55, 0xCF, 0x00, 0x00, 0x51, 0x52,
                      0x53, 0x54, 0x22, 0x3B, 0xC1, 0xEC, 0x84, 0x1A, 0xB5, 0x53};

    uint8_t encrypted[] = {0x08, 0xD0, 0x84, 0x21, 0x43, 0x01, 0x00, 0x00, 0x00, 0x00, 0x48, 0xDE,
                           0xAC, 0x02, 0x05, 0x00, 0x00, 0x00, 0x55, 0xCF, 0x00, 0x00, 0x51, 0x52,
                           0x53, 0x54, 0x22, 0x3B, 0xC1, 0xEC, 0x84, 0x1A, 0xB5, 0x53};

    uint8_t decrypted[] = {0x08, 0xD0, 0x84, 0x21, 0x43, 0x01, 0x00, 0x00, 0x00, 0x00, 0x48, 0xDE,
                           0xAC, 0x02, 0x05, 0x00, 0x00, 0x00, 0x55, 0xCF, 0x00, 0x00, 0x51, 0x52,
                           0x53, 0x54, 0x22, 0x3B, 0xC1, 0xEC, 0x84, 0x1A, 0xB5, 0x53};

    otInstance *       instance = testInitInstance();
    ot::Crypto::AesCcm aesCcm;
    uint32_t           headerLength  = sizeof(test) - 8;
    uint32_t           payloadLength = 0;
    uint8_t            tagLength     = 8;

    uint8_t nonce[] = {
        0xAC, 0xDE, 0x48, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x05, 0x02,
    };

    VerifyOrQuit(instance != nullptr);

    aesCcm.SetKey(key, sizeof(key));
    aesCcm.Init(headerLength, payloadLength, tagLength, nonce, sizeof(nonce));
    aesCcm.Header(test, headerLength);
    VerifyOrQuit(aesCcm.GetTagLength() == tagLength);
    aesCcm.Finalize(test + headerLength);

    VerifyOrQuit(memcmp(test, encrypted, sizeof(encrypted)) == 0);

    aesCcm.Init(headerLength, payloadLength, tagLength, nonce, sizeof(nonce));
    aesCcm.Header(test, headerLength);
    VerifyOrQuit(aesCcm.GetTagLength() == tagLength);
    aesCcm.Finalize(test + headerLength);

    VerifyOrQuit(memcmp(test, decrypted, sizeof(decrypted)) == 0);

    testFreeInstance(instance);
}

/**
 * Verifies test vectors from IEEE 802.15.4-2006 Annex C Section C.2.3
 */
void TestMacCommandFrame()
{
    uint8_t key[] = {
        0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
    };

    uint8_t test[] = {
        0x2B, 0xDC, 0x84, 0x21, 0x43, 0x02, 0x00, 0x00, 0x00, 0x00, 0x48, 0xDE, 0xAC,
        0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x48, 0xDE, 0xAC, 0x06, 0x05, 0x00,
        0x00, 0x00, 0x01, 0xCE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };

    uint32_t headerLength = 29, payloadLength = 1;
    uint8_t  tagLength = 8;

    uint8_t encrypted[] = {
        0x2B, 0xDC, 0x84, 0x21, 0x43, 0x02, 0x00, 0x00, 0x00, 0x00, 0x48, 0xDE, 0xAC,
        0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x48, 0xDE, 0xAC, 0x06, 0x05, 0x00,
        0x00, 0x00, 0x01, 0xD8, 0x4F, 0xDE, 0x52, 0x90, 0x61, 0xF9, 0xC6, 0xF1,
    };

    uint8_t decrypted[] = {
        0x2B, 0xDC, 0x84, 0x21, 0x43, 0x02, 0x00, 0x00, 0x00, 0x00, 0x48, 0xDE, 0xAC,
        0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x48, 0xDE, 0xAC, 0x06, 0x05, 0x00,
        0x00, 0x00, 0x01, 0xCE, 0x4F, 0xDE, 0x52, 0x90, 0x61, 0xF9, 0xC6, 0xF1,
    };

    uint8_t nonce[] = {
        0xAC, 0xDE, 0x48, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x05, 0x06,
    };

    ot::Crypto::AesCcm aesCcm;
    aesCcm.SetKey(key, sizeof(key));
    aesCcm.Init(headerLength, payloadLength, tagLength, nonce, sizeof(nonce));
    aesCcm.Header(test, headerLength);
    aesCcm.Payload(test + headerLength, test + headerLength, payloadLength, ot::Crypto::AesCcm::kEncrypt);
    VerifyOrQuit(aesCcm.GetTagLength() == tagLength);
    aesCcm.Finalize(test + headerLength + payloadLength);
    VerifyOrQuit(memcmp(test, encrypted, sizeof(encrypted)) == 0);

    aesCcm.Init(headerLength, payloadLength, tagLength, nonce, sizeof(nonce));
    aesCcm.Header(test, headerLength);
    aesCcm.Payload(test + headerLength, test + headerLength, payloadLength, ot::Crypto::AesCcm::kDecrypt);
    VerifyOrQuit(aesCcm.GetTagLength() == tagLength);
    aesCcm.Finalize(test + headerLength + payloadLength);

    VerifyOrQuit(memcmp(test, decrypted, sizeof(decrypted)) == 0);
}

int main(void)
{
    TestMacBeaconFrame();
    TestMacCommandFrame();
    printf("All tests passed\n");
    return 0;
}
