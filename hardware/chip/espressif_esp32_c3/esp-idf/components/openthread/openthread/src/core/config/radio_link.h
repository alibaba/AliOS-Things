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
 *   This file includes compile-time configurations for the radio links.
 *
 */

#ifndef CONFIG_RADIO_LINK_H_
#define CONFIG_RADIO_LINK_H_

/**
 * @def OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
 *
 * Set to 1 to enable support for IEEE802.15.4 radio link.
 *
 */
#ifndef OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
#define OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
 *
 * Set to 1 to enable support for Thread Radio Encapsulation Link (TREL).
 *
 */
#ifndef OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
#define OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE 0
#endif

//--------------------------------------------------------------

#if !OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE && !OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
#error "No radio link type is enabled - at least one radio link type should be enabled"
#endif

#ifdef OPENTHREAD_CONFIG_MULTI_RADIO
#error "OPENTHREAD_CONFIG_MULTI_RADIO should not be defined directly." \
       "It is derived from CONFIG_RADIO_LINK_<TYPE>_ENABLE options."
#endif

#if (OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE && OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE)
#define OPENTHREAD_CONFIG_MULTI_RADIO 1
#else
#define OPENTHREAD_CONFIG_MULTI_RADIO 0
#endif

#endif // CONFIG_RADIO_LINK_H_
