/*
 *  Copyright (c) 2020, The OpenThread Authors.
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

#include "test_platform.h"
#include "test_util.hpp"

#include "common/debug.hpp"
#include "common/random.hpp"
#include "crypto/ecdsa.hpp"

#include <mbedtls/ctr_drbg.h>
#include <mbedtls/ecdsa.h>
#include <mbedtls/pk.h>

#if OPENTHREAD_CONFIG_ECDSA_ENABLE

namespace ot {
namespace Crypto {

void TestEcdsaVector(void)
{
    // This case is derived from the test vector from RFC 6979 - section A.2.5 (for NIST P-256 and SHA-256 hash).

    const uint8_t kKeyPairInfo[] = {
        0x30, 0x78, 0x02, 0x01, 0x01, 0x04, 0x21, 0x00, 0xC9, 0xAF, 0xA9, 0xD8, 0x45, 0xBA, 0x75, 0x16, 0x6B, 0x5C,
        0x21, 0x57, 0x67, 0xB1, 0xD6, 0x93, 0x4E, 0x50, 0xC3, 0xDB, 0x36, 0xE8, 0x9B, 0x12, 0x7B, 0x8A, 0x62, 0x2B,
        0x12, 0x0F, 0x67, 0x21, 0xA0, 0x0A, 0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x07, 0xA1, 0x44,
        0x03, 0x42, 0x00, 0x04, 0x60, 0xFE, 0xD4, 0xBA, 0x25, 0x5A, 0x9D, 0x31, 0xC9, 0x61, 0xEB, 0x74, 0xC6, 0x35,
        0x6D, 0x68, 0xC0, 0x49, 0xB8, 0x92, 0x3B, 0x61, 0xFA, 0x6C, 0xE6, 0x69, 0x62, 0x2E, 0x60, 0xF2, 0x9F, 0xB6,
        0x79, 0x03, 0xFE, 0x10, 0x08, 0xB8, 0xBC, 0x99, 0xA4, 0x1A, 0xE9, 0xE9, 0x56, 0x28, 0xBC, 0x64, 0xF2, 0xF1,
        0xB2, 0x0C, 0x2D, 0x7E, 0x9F, 0x51, 0x77, 0xA3, 0xC2, 0x94, 0xD4, 0x46, 0x22, 0x99};

    const uint8_t kPublicKey[] = {
        0x60, 0xFE, 0xD4, 0xBA, 0x25, 0x5A, 0x9D, 0x31, 0xC9, 0x61, 0xEB, 0x74, 0xC6, 0x35, 0x6D, 0x68,
        0xC0, 0x49, 0xB8, 0x92, 0x3B, 0x61, 0xFA, 0x6C, 0xE6, 0x69, 0x62, 0x2E, 0x60, 0xF2, 0x9F, 0xB6,
        0x79, 0x03, 0xFE, 0x10, 0x08, 0xB8, 0xBC, 0x99, 0xA4, 0x1A, 0xE9, 0xE9, 0x56, 0x28, 0xBC, 0x64,
        0xF2, 0xF1, 0xB2, 0x0C, 0x2D, 0x7E, 0x9F, 0x51, 0x77, 0xA3, 0xC2, 0x94, 0xD4, 0x46, 0x22, 0x99,
    };

    const uint8_t kMessage[] = {'s', 'a', 'm', 'p', 'l', 'e'};

    const uint8_t kExpectedSignature[] = {
        0xEF, 0xD4, 0x8B, 0x2A, 0xAC, 0xB6, 0xA8, 0xFD, 0x11, 0x40, 0xDD, 0x9C, 0xD4, 0x5E, 0x81, 0xD6,
        0x9D, 0x2C, 0x87, 0x7B, 0x56, 0xAA, 0xF9, 0x91, 0xC3, 0x4D, 0x0E, 0xA8, 0x4E, 0xAF, 0x37, 0x16,
        0xF7, 0xCB, 0x1C, 0x94, 0x2D, 0x65, 0x7C, 0x41, 0xD4, 0x36, 0xC7, 0xA1, 0xB6, 0xE2, 0x9F, 0x65,
        0xF3, 0xE9, 0x00, 0xDB, 0xB9, 0xAF, 0xF4, 0x06, 0x4D, 0xC4, 0xAB, 0x2F, 0x84, 0x3A, 0xCD, 0xA8,
    };

    Instance *instance = testInitInstance();

    Ecdsa::P256::KeyPair   keyPair;
    Ecdsa::P256::PublicKey publicKey;
    Ecdsa::P256::Signature signature;
    Sha256                 sha256;
    Sha256::Hash           hash;

    VerifyOrQuit(instance != nullptr, "Null OpenThread instance");

    printf("\n===========================================================================\n");
    printf("Test ECDA with test vector from RFC 6979 (A.2.5)\n");

    printf("\nLoading key-pair ----------------------------------------------------------\n");
    memcpy(keyPair.GetDerBytes(), kKeyPairInfo, sizeof(kKeyPairInfo));
    keyPair.SetDerLength(sizeof(kKeyPairInfo));

    DumpBuffer("KeyPair", keyPair.GetDerBytes(), keyPair.GetDerLength());

    SuccessOrQuit(keyPair.GetPublicKey(publicKey));
    DumpBuffer("PublicKey", publicKey.GetBytes(), Ecdsa::P256::PublicKey::kSize);

    VerifyOrQuit(sizeof(kPublicKey) == Ecdsa::P256::PublicKey::kSize, "Example public key is invalid");
    VerifyOrQuit(memcmp(publicKey.GetBytes(), kPublicKey, sizeof(kPublicKey)) == 0,
                 "KeyPair::GetPublicKey() did not return the expected key");

    printf("\nHash the message ----------------------------------------------------------\n");
    sha256.Start();
    sha256.Update(kMessage);
    sha256.Finish(hash);

    DumpBuffer("Hash", hash.GetBytes(), sizeof(hash));

    printf("\nSign the message ----------------------------------------------------------\n");
    SuccessOrQuit(keyPair.Sign(hash, signature));
    DumpBuffer("Signature", signature.GetBytes(), sizeof(signature));

    printf("\nCheck signature against expected sequence----------------------------------\n");
    DumpBuffer("Expected signature", kExpectedSignature, sizeof(kExpectedSignature));

    VerifyOrQuit(sizeof(kExpectedSignature) == sizeof(signature));
    VerifyOrQuit(memcmp(signature.GetBytes(), kExpectedSignature, sizeof(kExpectedSignature)) == 0);

    printf("Signature matches expected sequence.\n");

    printf("\nVerify the signature ------------------------------------------------------\n");
    SuccessOrQuit(publicKey.Verify(hash, signature));
    printf("\nSignature was verified successfully.\n\n");

    testFreeInstance(instance);
}

void TestEdsaKeyGenerationSignAndVerify(void)
{
    Instance *instance = testInitInstance();

    const char *kMessage = "You are not a drop in the ocean. You are the entire ocean in a drop.";

    Ecdsa::P256::KeyPair   keyPair;
    Ecdsa::P256::PublicKey publicKey;
    Ecdsa::P256::Signature signature;
    Sha256                 sha256;
    Sha256::Hash           hash;

    VerifyOrQuit(instance != nullptr, "Null OpenThread instance");

    printf("\n===========================================================================\n");
    printf("Test ECDA key generation, signing and verification\n");

    printf("\nGenerating key-pair -------------------------------------------------------\n");
    SuccessOrQuit(keyPair.Generate());

    DumpBuffer("KeyPair", keyPair.GetDerBytes(), keyPair.GetDerLength());

    SuccessOrQuit(keyPair.GetPublicKey(publicKey));
    DumpBuffer("PublicKey", publicKey.GetBytes(), Ecdsa::P256::PublicKey::kSize);

    printf("\nHash the message ----------------------------------------------------------\n");
    sha256.Start();
    sha256.Update(kMessage, sizeof(kMessage) - 1);
    sha256.Finish(hash);

    DumpBuffer("Hash", hash.GetBytes(), sizeof(hash));

    printf("\nSign the message ----------------------------------------------------------\n");
    SuccessOrQuit(keyPair.Sign(hash, signature));
    DumpBuffer("Signature", signature.GetBytes(), sizeof(signature));

    printf("\nVerify the signature ------------------------------------------------------\n");
    SuccessOrQuit(publicKey.Verify(hash, signature));
    printf("\nSignature was verified successfully.");

    sha256.Start();
    sha256.Update(kMessage, sizeof(kMessage)); // include null char
    sha256.Finish(hash);
    VerifyOrQuit(publicKey.Verify(hash, signature) != kErrorNone, "PublicKey::Verify() passed for invalid signature");
    printf("\nSignature verification correctly failed with incorrect hash/signature.\n\n");

    testFreeInstance(instance);
}

} // namespace Crypto
} // namespace ot

#endif // OPENTHREAD_CONFIG_ECDSA_ENABLE

int main(void)
{
#if OPENTHREAD_CONFIG_ECDSA_ENABLE
    ot::Crypto::TestEcdsaVector();
    ot::Crypto::TestEdsaKeyGenerationSignAndVerify();
    printf("All tests passed\n");
#else
    printf("ECDSA feature is not enabled\n");
#endif

    return 0;
}
