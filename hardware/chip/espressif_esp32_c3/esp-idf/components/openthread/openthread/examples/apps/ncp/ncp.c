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

#include "openthread-core-config.h"

#include <openthread/ncp.h>

#include "common/code_utils.hpp"

#include "ncp/ncp_config.h"

#if !OPENTHREAD_CONFIG_NCP_SPI_ENABLE
#include "utils/uart.h"

void otPlatUartReceived(const uint8_t *aBuf, uint16_t aBufLength)
{
    otNcpHdlcReceive(aBuf, aBufLength);
}

void otPlatUartSendDone(void)
{
    otNcpHdlcSendDone();
}
#endif

#if !OPENTHREAD_ENABLE_NCP_VENDOR_HOOK
#if !OPENTHREAD_CONFIG_NCP_SPI_ENABLE
static int NcpSend(const uint8_t *aBuf, uint16_t aBufLength)
{
    IgnoreError(otPlatUartSend(aBuf, aBufLength));
    return aBufLength;
}
#endif

void otAppNcpInit(otInstance *aInstance)
{
#if OPENTHREAD_CONFIG_NCP_SPI_ENABLE
    otNcpSpiInit(aInstance);
#else
    IgnoreError(otPlatUartEnable());

    otNcpHdlcInit(aInstance, NcpSend);
#endif
}
#endif // !OPENTHREAD_ENABLE_NCP_VENDOR_HOOK
