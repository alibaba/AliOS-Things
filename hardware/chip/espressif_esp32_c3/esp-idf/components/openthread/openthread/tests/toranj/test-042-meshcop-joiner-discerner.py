#!/usr/bin/env python3
#
#  Copyright (c) 2020, The OpenThread Authors.
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#  3. Neither the name of the copyright holder nor the
#     names of its contributors may be used to endorse or promote products
#     derived from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
#  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.

import wpan
from wpan import verify

# -----------------------------------------------------------------------------------------------------------------------
# Test description: Test use of Joiner Discerner for commissioning
#

test_name = __file__[:-3] if __file__.endswith('.py') else __file__
print('-' * 120)
print('Starting \'{}\''.format(test_name))

# -----------------------------------------------------------------------------------------------------------------------
# Creating `wpan.Nodes` instances

speedup = 4
wpan.Node.set_time_speedup_factor(speedup)

commr = wpan.Node()
joiner1 = wpan.Node()
joiner2 = wpan.Node()
joiner3 = wpan.Node()

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Build network topology

commr.form('discerner')

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation

WAIT_TIME = 2  # seconds

PSK1 = 'UNCHARTEDTHEL0STLEGACY'
PSK2 = 'UNCHARTED4ATH1EFSEND'
PSK3 = 'THELAST0FUS'

DISCERNER1 = '0x777'
D_LEN1 = 12

DISCERNER2 = '0x7777777'
D_LEN2 = 32

EUI64_3 = joiner3.get(wpan.WPAN_HW_ADDRESS)[1:-1]  # Remove the "[]"

JOINER_TIMOUT = '500'  # in seconds

# Verify Discerner value after device reset on Joiner

verify(int(joiner1.get(wpan.WPAN_THREAD_JOINER_DISCERNER_BIT_LENGTH), 0) == 0)
verify(int(joiner1.get(wpan.WPAN_THREAD_JOINER_DISCERNER_VALUE), 0) == 0)

# Set Joiner Discerner on `joiner1` and `joiner2`

joiner1.set(wpan.WPAN_THREAD_JOINER_DISCERNER_BIT_LENGTH, str(D_LEN1))
joiner1.set(wpan.WPAN_THREAD_JOINER_DISCERNER_VALUE, DISCERNER1)
verify(int(joiner1.get(wpan.WPAN_THREAD_JOINER_DISCERNER_BIT_LENGTH), 0) == D_LEN1)
verify(int(joiner1.get(wpan.WPAN_THREAD_JOINER_DISCERNER_VALUE), 0) == int(DISCERNER1, 0))

joiner2.set(wpan.WPAN_THREAD_JOINER_DISCERNER_BIT_LENGTH, str(D_LEN2))
joiner2.set(wpan.WPAN_THREAD_JOINER_DISCERNER_VALUE, DISCERNER2)
verify(int(joiner2.get(wpan.WPAN_THREAD_JOINER_DISCERNER_BIT_LENGTH), 0) == D_LEN2)
verify(int(joiner2.get(wpan.WPAN_THREAD_JOINER_DISCERNER_VALUE), 0) == int(DISCERNER2, 0))

# Check clearing of a previously set Joiner Discerner

joiner3.set(wpan.WPAN_THREAD_JOINER_DISCERNER_BIT_LENGTH, str(D_LEN2))
joiner3.set(wpan.WPAN_THREAD_JOINER_DISCERNER_VALUE, DISCERNER2)
verify(int(joiner3.get(wpan.WPAN_THREAD_JOINER_DISCERNER_BIT_LENGTH), 0) == D_LEN2)
verify(int(joiner3.get(wpan.WPAN_THREAD_JOINER_DISCERNER_VALUE), 0) == int(DISCERNER2, 0))

joiner3.set(wpan.WPAN_THREAD_JOINER_DISCERNER_BIT_LENGTH, '0')
verify(int(joiner3.get(wpan.WPAN_THREAD_JOINER_DISCERNER_BIT_LENGTH), 0) == 0)
verify(int(joiner3.get(wpan.WPAN_THREAD_JOINER_DISCERNER_VALUE), 0) == 0)

# Adding Joiners on Commissioner

commr.commissioner_start()

commr.commissioner_add_joiner_with_discerner(DISCERNER1, D_LEN1, PSK1, JOINER_TIMOUT)
commr.commissioner_add_joiner_with_discerner(DISCERNER2, D_LEN2, PSK2, JOINER_TIMOUT)
commr.commissioner_add_joiner(EUI64_3, PSK3, JOINER_TIMOUT)

verify(len(wpan.parse_list(commr.get(wpan.WPAN_THREAD_COMMISSIONER_JOINERS))) == 3)

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Start `joiner2` first

# Starting with `joiner2` verifies the behavior of Commissioner to
# prefer the Joiner entry with the longest matching discriminator. Note
# that `joiner2` uses a longer discriminator compared to `joiner1` with
# similar value.

joiner2.joiner_join(PSK2)
verify(joiner2.get(wpan.WPAN_STATE) == wpan.STATE_COMMISSIONED)
joiner2.joiner_attach()


def joiner2_is_associated():
    verify(joiner2.is_associated())


wpan.verify_within(joiner2_is_associated, WAIT_TIME)

# Start `joiner1`

joiner1.joiner_join(PSK1)
verify(joiner1.get(wpan.WPAN_STATE) == wpan.STATE_COMMISSIONED)
joiner1.joiner_attach()


def joiner1_is_associated():
    verify(joiner1.is_associated())


wpan.verify_within(joiner1_is_associated, WAIT_TIME)

# Start `joiner3`

joiner3.joiner_join(PSK3)
verify(joiner3.get(wpan.WPAN_STATE) == wpan.STATE_COMMISSIONED)
joiner3.joiner_attach()


def joiner3_is_associated():
    verify(joiner3.is_associated())


wpan.verify_within(joiner3_is_associated, WAIT_TIME)

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
