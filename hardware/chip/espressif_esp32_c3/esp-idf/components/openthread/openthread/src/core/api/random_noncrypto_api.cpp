/*
 *  Copyright (c) 2019, The OpenThread Authors.
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
 *   This file implements the OpenThread software random number generator API.
 */

#include <openthread/random_noncrypto.h>

#include "common/random.hpp"

using namespace ot;

uint32_t otRandomNonCryptoGetUint32(void)
{
    return Random::NonCrypto::GetUint32();
}

uint8_t otRandomNonCryptoGetUint8(void)
{
    return Random::NonCrypto::GetUint8();
}

uint16_t otRandomNonCryptoGetUint16(void)
{
    return Random::NonCrypto::GetUint16();
}

uint8_t otRandomNonCryptoGetUint8InRange(uint8_t aMin, uint8_t aMax)
{
    return Random::NonCrypto::GetUint8InRange(aMin, aMax);
}

uint16_t otRandomNonCryptoGetUint16InRange(uint16_t aMin, uint16_t aMax)
{
    return Random::NonCrypto::GetUint16InRange(aMin, aMax);
}

uint32_t otRandomNonCryptoGetUint32InRange(uint32_t aMin, uint32_t aMax)
{
    return Random::NonCrypto::GetUint32InRange(aMin, aMax);
}

void otRandomNonCryptoFillBuffer(uint8_t *aBuffer, uint16_t aSize)
{
    Random::NonCrypto::FillBuffer(aBuffer, aSize);
}

uint32_t otRandomNonCryptoAddJitter(uint32_t aValue, uint16_t aJitter)
{
    return Random::NonCrypto::AddJitter(aValue, aJitter);
}
