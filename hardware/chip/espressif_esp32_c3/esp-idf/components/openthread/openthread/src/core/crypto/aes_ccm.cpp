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
 *   This file implements AES-CCM.
 */

#include "aes_ccm.hpp"

#include <limits.h>

#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/encoding.hpp"

namespace ot {
namespace Crypto {

void AesCcm::SetKey(const uint8_t *aKey, uint16_t aKeyLength)
{
    Key cryptoKey;

    cryptoKey.Set(aKey, aKeyLength);
    SetKey(cryptoKey);
}

void AesCcm::SetKey(const Mac::KeyMaterial &aMacKey)
{
    Key cryptoKey;

    aMacKey.ConvertToCryptoKey(cryptoKey);
    SetKey(cryptoKey);
}

void AesCcm::Init(uint32_t    aHeaderLength,
                  uint32_t    aPlainTextLength,
                  uint8_t     aTagLength,
                  const void *aNonce,
                  uint8_t     aNonceLength)
{
    const uint8_t *nonceBytes  = reinterpret_cast<const uint8_t *>(aNonce);
    uint8_t        blockLength = 0;
    uint32_t       len;
    uint8_t        L;
    uint8_t        i;

    // Tag length must be even and within [kMinTagLength, kMaxTagLength]
    OT_ASSERT(((aTagLength & 0x1) == 0) && (kMinTagLength <= aTagLength) && (aTagLength <= kMaxTagLength));

    L = 0;

    for (len = aPlainTextLength; len; len >>= 8)
    {
        L++;
    }

    if (L <= 1)
    {
        L = 2;
    }

    if (aNonceLength > 13)
    {
        aNonceLength = 13;
    }

    // increase L to match nonce len
    if (L < (15 - aNonceLength))
    {
        L = 15 - aNonceLength;
    }

    // decrease nonceLength to match L
    if (aNonceLength > (15 - L))
    {
        aNonceLength = 15 - L;
    }

    // setup initial block

    // write flags
    mBlock[0] = (static_cast<uint8_t>((aHeaderLength != 0) << 6) | static_cast<uint8_t>(((aTagLength - 2) >> 1) << 3) |
                 static_cast<uint8_t>(L - 1));

    // write nonce
    memcpy(&mBlock[1], nonceBytes, aNonceLength);

    // write len
    len = aPlainTextLength;

    for (i = sizeof(mBlock) - 1; i > aNonceLength; i--)
    {
        mBlock[i] = len & 0xff;
        len >>= 8;
    }

    // encrypt initial block
    mEcb.Encrypt(mBlock, mBlock);

    // process header
    if (aHeaderLength > 0)
    {
        // process length
        if (aHeaderLength < (65536U - 256U))
        {
            mBlock[blockLength++] ^= aHeaderLength >> 8;
            mBlock[blockLength++] ^= aHeaderLength >> 0;
        }
        else
        {
            mBlock[blockLength++] ^= 0xff;
            mBlock[blockLength++] ^= 0xfe;
            mBlock[blockLength++] ^= aHeaderLength >> 24;
            mBlock[blockLength++] ^= aHeaderLength >> 16;
            mBlock[blockLength++] ^= aHeaderLength >> 8;
            mBlock[blockLength++] ^= aHeaderLength >> 0;
        }
    }

    // init counter
    mCtr[0] = L - 1;
    memcpy(&mCtr[1], nonceBytes, aNonceLength);
    memset(&mCtr[aNonceLength + 1], 0, sizeof(mCtr) - aNonceLength - 1);

    mNonceLength     = aNonceLength;
    mHeaderLength    = aHeaderLength;
    mHeaderCur       = 0;
    mPlainTextLength = aPlainTextLength;
    mPlainTextCur    = 0;
    mBlockLength     = blockLength;
    mCtrLength       = sizeof(mCtrPad);
    mTagLength       = aTagLength;
}

void AesCcm::Header(const void *aHeader, uint32_t aHeaderLength)
{
    const uint8_t *headerBytes = reinterpret_cast<const uint8_t *>(aHeader);

    OT_ASSERT(mHeaderCur + aHeaderLength <= mHeaderLength);

    // process header
    for (unsigned i = 0; i < aHeaderLength; i++)
    {
        if (mBlockLength == sizeof(mBlock))
        {
            mEcb.Encrypt(mBlock, mBlock);
            mBlockLength = 0;
        }

        mBlock[mBlockLength++] ^= headerBytes[i];
    }

    mHeaderCur += aHeaderLength;

    if (mHeaderCur == mHeaderLength)
    {
        // process remainder
        if (mBlockLength != 0)
        {
            mEcb.Encrypt(mBlock, mBlock);
        }

        mBlockLength = 0;
    }
}

void AesCcm::Payload(void *aPlainText, void *aCipherText, uint32_t aLength, Mode aMode)
{
    uint8_t *plaintextBytes  = reinterpret_cast<uint8_t *>(aPlainText);
    uint8_t *ciphertextBytes = reinterpret_cast<uint8_t *>(aCipherText);
    uint8_t  byte;

    OT_ASSERT(mPlainTextCur + aLength <= mPlainTextLength);

    for (unsigned i = 0; i < aLength; i++)
    {
        if (mCtrLength == 16)
        {
            for (int j = sizeof(mCtr) - 1; j > mNonceLength; j--)
            {
                if (++mCtr[j])
                {
                    break;
                }
            }

            mEcb.Encrypt(mCtr, mCtrPad);
            mCtrLength = 0;
        }

        if (aMode == kEncrypt)
        {
            byte               = plaintextBytes[i];
            ciphertextBytes[i] = byte ^ mCtrPad[mCtrLength++];
        }
        else
        {
            byte              = ciphertextBytes[i] ^ mCtrPad[mCtrLength++];
            plaintextBytes[i] = byte;
        }

        if (mBlockLength == sizeof(mBlock))
        {
            mEcb.Encrypt(mBlock, mBlock);
            mBlockLength = 0;
        }

        mBlock[mBlockLength++] ^= byte;
    }

    mPlainTextCur += aLength;

    if (mPlainTextCur >= mPlainTextLength)
    {
        if (mBlockLength != 0)
        {
            mEcb.Encrypt(mBlock, mBlock);
        }

        // reset counter
        memset(&mCtr[mNonceLength + 1], 0, sizeof(mCtr) - mNonceLength - 1);
    }
}

void AesCcm::Finalize(void *aTag)
{
    uint8_t *tagBytes = reinterpret_cast<uint8_t *>(aTag);

    OT_ASSERT(mPlainTextCur == mPlainTextLength);

    mEcb.Encrypt(mCtr, mCtrPad);

    for (int i = 0; i < mTagLength; i++)
    {
        tagBytes[i] = mBlock[i] ^ mCtrPad[i];
    }
}

void AesCcm::GenerateNonce(const Mac::ExtAddress &aAddress,
                           uint32_t               aFrameCounter,
                           uint8_t                aSecurityLevel,
                           uint8_t *              aNonce)
{
    memcpy(aNonce, aAddress.m8, sizeof(Mac::ExtAddress));
    aNonce += sizeof(Mac::ExtAddress);

    Encoding::BigEndian::WriteUint32(aFrameCounter, aNonce);
    aNonce += sizeof(uint32_t);

    aNonce[0] = aSecurityLevel;
}

} // namespace Crypto
} // namespace ot
