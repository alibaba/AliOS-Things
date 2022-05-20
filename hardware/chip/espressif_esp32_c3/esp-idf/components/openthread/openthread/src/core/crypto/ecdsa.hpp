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
 *   This file includes definitions for performing ECDSA signing.
 */

#ifndef ECDSA_HPP_
#define ECDSA_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_ECDSA_ENABLE

#include <stdint.h>
#include <stdlib.h>

#include "common/error.hpp"
#include "crypto/sha256.hpp"

namespace ot {
namespace Crypto {
namespace Ecdsa {

/**
 * @addtogroup core-security
 *
 * @{
 *
 */

/**
 * This class implements ECDSA key-generation, signing, and verification for NIST P-256 curve using SHA-256 hash.
 *
 * NIST P-256 curve is also known as 256-bit Random ECP Group (RFC 5114 - 2.6), or secp256r1 (RFC 4492 - Appendix A).
 *
 */
class P256
{
public:
    static constexpr uint16_t kFieldBitLength = 256; ///< Prime field bit length used by the P-256 curve.

    /**
     * Max bytes in binary representation of an MPI (multi-precision int).
     *
     */
    static constexpr uint8_t kMpiSize = kFieldBitLength / 8;

    class PublicKey;
    class KeyPair;

    /**
     * This class represents an ECDSA signature.
     *
     * The signature is encoded as the concatenated binary representation of two MPIs `r` and `s` which are calculated
     * during signing (RFC 6605 - section 4).
     *
     */
    OT_TOOL_PACKED_BEGIN
    class Signature
    {
        friend class KeyPair;
        friend class PublicKey;

    public:
        static constexpr uint8_t kSize = 2 * kMpiSize; ///< Signature size in bytes (two times the curve MPI size).

        /**
         * This method returns the signature as a byte array.
         *
         * @returns A pointer to the byte array containing the signature.
         *
         */
        const uint8_t *GetBytes(void) const { return mShared.mKey; }

    private:
        OT_TOOL_PACKED_BEGIN
        struct Mpis
        {
            uint8_t mR[kMpiSize];
            uint8_t mS[kMpiSize];
        } OT_TOOL_PACKED_END;

        union OT_TOOL_PACKED_FIELD
        {
            Mpis    mMpis;
            uint8_t mKey[kSize];
        } mShared;
    } OT_TOOL_PACKED_END;

    /**
     * This class represents a key pair (public and private keys).
     *
     * The key pair is stored using Distinguished Encoding Rules (DER) format (per RFC 5915).
     *
     */
    class KeyPair
    {
    public:
        /**
         * Max buffer size (in bytes) for representing the key-pair in DER format.
         *
         */
        static constexpr uint8_t kMaxDerSize = 125;

        /**
         * This constructor initializes a `KeyPair` as empty (no key).
         *
         */
        KeyPair(void)
            : mDerLength(0)
        {
        }

        /**
         * This method generates and populates the `KeyPair` with a new public/private keys.
         *
         * @retval kErrorNone         A new key pair was generated successfully.
         * @retval kErrorNoBufs       Failed to allocate buffer for key generation.
         * @retval kErrorNotCapable   Feature not supported.
         * @retval kErrorFailed       Failed to generate key.
         *
         */
        Error Generate(void);

        /**
         * This method gets the associated public key from the `KeyPair`.
         *
         * @param[out] aPublicKey     A reference to a `PublicKey` to output the value.
         *
         * @retval kErrorNone      Public key was retrieved successfully, and @p aPublicKey is updated.
         * @retval kErrorParse     The key-pair DER format could not be parsed (invalid format).
         *
         */
        Error GetPublicKey(PublicKey &aPublicKey) const;

        /**
         * This method gets the pointer to start of the buffer containing the key-pair info in DER format.
         *
         * The length (number of bytes) of DER format is given by `GetDerLength()`.
         *
         * @returns The pointer to the start of buffer containing the key-pair in DER format.
         *
         */
        const uint8_t *GetDerBytes(void) const { return mDerBytes; }

        /**
         * This method gets the length of the byte sequence representation of the key-pair in DER format.
         *
         * @returns The length of byte sequence representation of the key-pair in DER format.
         *
         */
        uint8_t GetDerLength(void) const { return mDerLength; }

        /**
         * This method gets the pointer to start of the key-pair buffer in DER format.
         *
         * This method gives non-const pointer to the buffer and is intended for populating the buffer and setting
         * the key-pair (e.g., reading the key-pair from non-volatile settings). The buffer contains `kMaxDerSize`
         * bytes. After populating the buffer, `SetDerLength()` can be used to set the the number of bytes written.
         *
         * @returns The pointer to the start of key-pair buffer in DER format.
         *
         */
        uint8_t *GetDerBytes(void) { return mDerBytes; }

        /**
         * This method sets the length of the byte sequence representation of the key-pair in DER format.
         *
         * @param[in] aDerLength   The length (number of bytes).
         *
         */
        void SetDerLength(uint8_t aDerLength) { mDerLength = aDerLength; }

        /**
         * This method calculates the ECDSA signature for a hashed message using the private key from `KeyPair`.
         *
         * This method uses the deterministic digital signature generation procedure from RFC 6979.
         *
         * @param[in]  aHash               The SHA-256 hash value of the message to use for signature calculation.
         * @param[out] aSignature          A reference to a `Signature` to output the calculated signature value.
         *
         * @retval kErrorNone           The signature was calculated successfully and @p aSignature was updated.
         * @retval kErrorParse          The key-pair DER format could not be parsed (invalid format).
         * @retval kErrorInvalidArgs    The @p aHash is invalid.
         * @retval kErrorNoBufs         Failed to allocate buffer for signature calculation.
         *
         */
        Error Sign(const Sha256::Hash &aHash, Signature &aSignature) const;

    private:
        Error Parse(void *aContext) const;

        uint8_t mDerBytes[kMaxDerSize];
        uint8_t mDerLength;
    };

    /**
     * This class represents a public key.
     *
     * The public key is stored as a byte sequence representation of an uncompressed curve point (RFC 6605 - sec 4).
     *
     */
    OT_TOOL_PACKED_BEGIN
    class PublicKey
    {
        friend class KeyPair;

    public:
        static constexpr uint8_t kSize = kMpiSize * 2; ///< Size of the public key in bytes.

        /**
         * This method gets the pointer to the buffer containing the public key (as an uncompressed curve point).
         *
         * @return The pointer to the buffer containing the public key (with `kSize` bytes).
         *
         */
        const uint8_t *GetBytes(void) const { return mData; }

        /**
         * This method uses the `PublicKey` to verify the ECDSA signature of a hashed message.
         *
         * @param[in] aHash                The SHA-256 hash value of a message to use for signature verification.
         * @param[in] aSignature           The signature value to verify.
         *
         * @retval kErrorNone          The signature was verified successfully.
         * @retval kErrorSecurity      The signature is invalid.
         * @retval kErrorInvalidArgs   The key or has is invalid.
         * @retval kErrorNoBufs        Failed to allocate buffer for signature verification
         *
         */
        Error Verify(const Sha256::Hash &aHash, const Signature &aSignature) const;

    private:
        uint8_t mData[kSize];
    } OT_TOOL_PACKED_END;
};

/**
 * This function creates an ECDSA signature.
 *
 * @param[out]    aOutput            An output buffer where ECDSA sign should be stored.
 * @param[inout]  aOutputLength      The length of the @p aOutput buffer.
 * @param[in]     aInputHash         An input hash.
 * @param[in]     aInputHashLength   The length of the @p aInputHash buffer.
 * @param[in]     aPrivateKey        A private key in PEM format.
 * @param[in]     aPrivateKeyLength  The length of the @p aPrivateKey buffer.
 *
 * @retval  kErrorNone         ECDSA sign has been created successfully.
 * @retval  kErrorNoBufs       Output buffer is too small.
 * @retval  kErrorInvalidArgs  Private key is not valid EC Private Key.
 * @retval  kErrorFailed       Error during signing.
 *
 */
Error Sign(uint8_t *      aOutput,
           uint16_t &     aOutputLength,
           const uint8_t *aInputHash,
           uint16_t       aInputHashLength,
           const uint8_t *aPrivateKey,
           uint16_t       aPrivateKeyLength);

/**
 * @}
 *
 */

} // namespace Ecdsa
} // namespace Crypto
} // namespace ot

#endif // OPENTHREAD_CONFIG_ECDSA_ENABLE

#endif // ECDSA_HPP_
