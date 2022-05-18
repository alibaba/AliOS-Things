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

/**
 * @file
 *   This file implements the infrastructure interface for posix.
 */

#include "openthread-posix-config.h"

#include <net/if.h>

#include "core/common/non_copyable.hpp"
#include "posix/platform/mainloop.hpp"

#if OPENTHREAD_CONFIG_BORDER_ROUTING_ENABLE

namespace ot {
namespace Posix {

/**
 * This class manages infrastructure network interface.
 *
 */
class InfraNetif : public Mainloop::Source, private NonCopyable
{
public:
    /**
     * This method updates the fd_set and timeout for mainloop.
     *
     * @param[inout]    aContext    A reference to the mainloop context.
     *
     */
    void Update(otSysMainloopContext &aContext) override;

    /**
     * This method performs infrastructure network interface processing.
     *
     * @param[in]   aContext   A reference to the mainloop context.
     *
     */
    void Process(const otSysMainloopContext &aContext) override;

    /**
     * This method initializes the infrastructure network interface.
     *
     * @note This method is called before OpenThread instance is created.
     *
     * @param[in]  aIfName      A pointer to infrastructure network interface name.
     *
     */
    void Init(const char *aIfName);

    /**
     * This method sets up the infrastructure network interface.
     *
     * @note This method is called after OpenThread instance is created.
     *
     */
    void SetUp(void);

    /**
     * This method tears down the infrastructure network interface.
     *
     * @note This method is called before OpenThread instance is destructed.
     *
     */
    void TearDown(void);

    /**
     * This method deinitializes the infrastructure network interface.
     *
     * @note This method is called after OpenThread instance is destructed.
     *
     */
    void Deinit(void);

    /**
     * This method checks whether the infrastructure network interface is running.
     *
     */
    bool IsRunning(void) const;

    /**
     * This method sends an ICMPv6 Neighbor Discovery message on given infrastructure interface.
     *
     * See RFC 4861: https://tools.ietf.org/html/rfc4861.
     *
     * @param[in]  aInfraIfIndex  The index of the infrastructure interface this message is sent to.
     * @param[in]  aDestAddress   The destination address this message is sent to.
     * @param[in]  aBuffer        The ICMPv6 message buffer. The ICMPv6 checksum is left zero and the
     *                            platform should do the checksum calculate.
     * @param[in]  aBufferLength  The length of the message buffer.
     *
     * @note  Per RFC 4861, the implementation should send the message with IPv6 link-local source address
     *        of interface @p aInfraIfIndex and IP Hop Limit 255.
     *
     * @retval OT_ERROR_NONE    Successfully sent the ICMPv6 message.
     * @retval OT_ERROR_FAILED  Failed to send the ICMPv6 message.
     *
     */
    otError SendIcmp6Nd(uint32_t            aInfraIfIndex,
                        const otIp6Address &aDestAddress,
                        const uint8_t *     aBuffer,
                        uint16_t            aBufferLength);
    /**
     * This function gets the infrastructure network interface singleton.
     *
     * @returns The singleton object.
     */
    static InfraNetif &Get(void);

private:
    char     mInfraIfName[IFNAMSIZ];
    uint32_t mInfraIfIndex       = 0;
    int      mInfraIfIcmp6Socket = -1;
    int      mNetLinkSocket      = -1;

    void ReceiveNetLinkMessage(void);
    void ReceiveIcmp6Message(void);
    bool HasLinkLocalAddress(void) const;
};

} // namespace Posix
} // namespace ot
#endif // OPENTHREAD_CONFIG_BORDER_ROUTING_ENABLE
