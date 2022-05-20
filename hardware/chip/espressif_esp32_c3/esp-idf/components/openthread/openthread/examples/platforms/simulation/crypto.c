/*
 *  Copyright (c) 2021, The OpenThread Authors.
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

#include "platform-simulation.h"

#include <stdio.h>
#include <stdlib.h>

#include <openthread/config.h>
#include <openthread/platform/crypto.h>

#if OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE

// crypto key storage stubs

otError otPlatCryptoImportKey(otCryptoKeyRef *     aKeyRef,
                              otCryptoKeyType      aKeyType,
                              otCryptoKeyAlgorithm aKeyAlgorithm,
                              int                  aKeyUsage,
                              otCryptoKeyStorage   aKeyPersistence,
                              const uint8_t *      aKey,
                              size_t               aKeyLen)
{
    OT_UNUSED_VARIABLE(aKeyRef);
    OT_UNUSED_VARIABLE(aKeyType);
    OT_UNUSED_VARIABLE(aKeyAlgorithm);
    OT_UNUSED_VARIABLE(aKeyUsage);
    OT_UNUSED_VARIABLE(aKeyPersistence);
    OT_UNUSED_VARIABLE(aKey);
    OT_UNUSED_VARIABLE(aKeyLen);

    return OT_ERROR_NOT_IMPLEMENTED;
}

otError otPlatCryptoExportKey(otCryptoKeyRef aKeyRef, uint8_t *aBuffer, size_t aBufferLen, size_t *aKeyLen)
{
    OT_UNUSED_VARIABLE(aKeyRef);
    OT_UNUSED_VARIABLE(aBuffer);
    OT_UNUSED_VARIABLE(aBufferLen);
    OT_UNUSED_VARIABLE(aKeyLen);

    return OT_ERROR_NOT_IMPLEMENTED;
}

otError otPlatCryptoDestroyKey(otCryptoKeyRef aKeyRef)
{
    OT_UNUSED_VARIABLE(aKeyRef);

    return OT_ERROR_NOT_IMPLEMENTED;
}

bool otPlatCryptoHasKey(otCryptoKeyRef aKeyRef)
{
    OT_UNUSED_VARIABLE(aKeyRef);

    return false;
}

#endif // OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE
