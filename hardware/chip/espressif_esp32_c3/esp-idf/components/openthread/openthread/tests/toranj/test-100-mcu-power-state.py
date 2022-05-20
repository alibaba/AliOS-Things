#!/usr/bin/env python3
#
#  Copyright (c) 2018, The OpenThread Authors.
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

import time
import wpan
from wpan import verify

# -----------------------------------------------------------------------------------------------------------------------
# Test description: Testing controlling of NCP's MCU power state

test_name = __file__[:-3] if __file__.endswith('.py') else __file__
print('-' * 120)
print('Starting \'{}\''.format(test_name))

# -----------------------------------------------------------------------------------------------------------------------
# Creating `wpan.Nodes` instances

node = wpan.Node()

# -----------------------------------------------------------------------------------------------------------------------
# Init all nodes

wpan.Node.init_all_nodes()

# -----------------------------------------------------------------------------------------------------------------------
# Test implementation

# Verify that state is ON after a reset
verify(node.get(wpan.WPAN_NCP_MCU_POWER_STATE) == wpan.MCU_POWER_STATE_ON)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Check power state wpantund property get and set

WAIT_TIME = 5


def check_wpan_is_in_offline_state():
    verify(node.get(wpan.WPAN_STATE) == wpan.STATE_OFFLINE)


def check_wpan_is_in_deep_sleep_state():
    verify(node.get(wpan.WPAN_STATE) == wpan.STATE_DEEP_SLEEP)


def check_wpan_is_in_commissioned_state():
    verify(node.get(wpan.WPAN_STATE) == wpan.STATE_COMMISSIONED)


def check_wpan_is_in_associated_state():
    verify(node.get(wpan.WPAN_STATE) == wpan.STATE_ASSOCIATED)


def check_wpan_is_in_associating_state():
    verify(node.get(wpan.WPAN_STATE) == wpan.STATE_ASSOCIATING)


node.form("mcu-power-state")
verify(node.is_associated())

node.set(wpan.WPAN_NCP_MCU_POWER_STATE, 'low-power')
verify(node.get(wpan.WPAN_NCP_MCU_POWER_STATE) == wpan.MCU_POWER_STATE_LOW_POWER)
verify(node.get(wpan.WPAN_STATE) == wpan.STATE_ASSOCIATED)

node.set(wpan.WPAN_NCP_MCU_POWER_STATE, 'on')
verify(node.get(wpan.WPAN_NCP_MCU_POWER_STATE) == wpan.MCU_POWER_STATE_ON)

node.set(wpan.WPAN_NCP_MCU_POWER_STATE, 'lp')  # special short-form string for low-power
verify(node.get(wpan.WPAN_NCP_MCU_POWER_STATE) == wpan.MCU_POWER_STATE_LOW_POWER)

node.set(wpan.WPAN_NCP_MCU_POWER_STATE, wpan.MCU_POWER_STATE_ON)
verify(node.get(wpan.WPAN_NCP_MCU_POWER_STATE) == wpan.MCU_POWER_STATE_ON)

node.set(wpan.WPAN_NCP_MCU_POWER_STATE, wpan.MCU_POWER_STATE_LOW_POWER)
verify(node.get(wpan.WPAN_NCP_MCU_POWER_STATE) == wpan.MCU_POWER_STATE_LOW_POWER)
verify(node.get(wpan.WPAN_STATE) == wpan.STATE_ASSOCIATED)

# Verify that `wpantund` will restore the user-set value after NCP reset

node.reset()
time.sleep(1)
verify(node.get(wpan.WPAN_NCP_MCU_POWER_STATE) == wpan.MCU_POWER_STATE_LOW_POWER)
node.set(wpan.WPAN_NCP_MCU_POWER_STATE, wpan.MCU_POWER_STATE_ON)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Check the `wpantund` state changes between "deep-sleep" and "offline"

node.leave()
verify(not node.is_associated())

verify(node.get(wpan.WPAN_NCP_MCU_POWER_STATE) == wpan.MCU_POWER_STATE_ON)
verify(node.get(wpan.WPAN_STATE) == wpan.STATE_OFFLINE)

# Setting the power state to `low-power` should change wpantund state to
# `DEEP_SLEEP`

node.set(wpan.WPAN_NCP_MCU_POWER_STATE, wpan.MCU_POWER_STATE_LOW_POWER)
wpan.verify_within(check_wpan_is_in_deep_sleep_state, WAIT_TIME)

# Verify that reading/getting a property does not impact the wpantund state.

node.get(wpan.WPAN_THREAD_RLOC16)
verify(node.get(wpan.WPAN_NCP_MCU_POWER_STATE) == wpan.MCU_POWER_STATE_LOW_POWER)
verify(node.get(wpan.WPAN_STATE) == wpan.STATE_DEEP_SLEEP)

# Setting the power state to `on` should change wpantund state to `OFFLINE`

node.set(wpan.WPAN_NCP_MCU_POWER_STATE, wpan.MCU_POWER_STATE_ON)
wpan.verify_within(check_wpan_is_in_offline_state, WAIT_TIME)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Verify the behavior of `begin-low-power` wpanctl command

node.wpanctl('begin-low-power')
wpan.verify_within(check_wpan_is_in_deep_sleep_state, WAIT_TIME)
verify(node.get(wpan.WPAN_NCP_MCU_POWER_STATE) == wpan.MCU_POWER_STATE_LOW_POWER)

node.set(wpan.WPAN_NCP_MCU_POWER_STATE, wpan.MCU_POWER_STATE_ON)
wpan.verify_within(check_wpan_is_in_offline_state, WAIT_TIME)

# Check the `wpantund` state changes between "offline:commissioned" and
# "deep-sleep"

node.form("test-network")
node.set('Daemon:AutoAssociateAfterReset', '0')

# Verify that issuing a `begin-low-power` when in "associated" state
# does not change the state.
node.wpanctl('begin-low-power')
verify(node.get(wpan.WPAN_NCP_MCU_POWER_STATE) == wpan.MCU_POWER_STATE_LOW_POWER)
verify(node.get(wpan.WPAN_STATE) == wpan.STATE_ASSOCIATED)

# After reset, power state should remain `LOW_POWER` (wpantund would restore the value
# on NCP) and since "AutoAssociateAfterReset" is disabled, wpantund state should
# be `DEEP_SLEEP`.

node.reset()
wpan.verify_within(check_wpan_is_in_deep_sleep_state, WAIT_TIME)

node.set(wpan.WPAN_NCP_MCU_POWER_STATE, wpan.MCU_POWER_STATE_ON)
wpan.verify_within(check_wpan_is_in_commissioned_state, WAIT_TIME)

node.set(wpan.WPAN_NCP_MCU_POWER_STATE, wpan.MCU_POWER_STATE_LOW_POWER)
wpan.verify_within(check_wpan_is_in_deep_sleep_state, WAIT_TIME)

node.set(wpan.WPAN_NCP_MCU_POWER_STATE, wpan.MCU_POWER_STATE_ON)
node.leave()

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Verify sleep behavior after disabling `wpantund` ("Daemon:Enabled"
# property) when state is "offline"

verify(node.get(wpan.WPAN_NCP_MCU_POWER_STATE) == wpan.MCU_POWER_STATE_ON)
verify(node.get(wpan.WPAN_STATE) == wpan.STATE_OFFLINE)
verify(node.get('Daemon:Enabled') == 'true')

# Disabling `wpantund` should put the NCP to deep sleep
node.set('Daemon:Enabled', 'false')
verify(node.get('Daemon:Enabled') == 'false')
wpan.verify_within(check_wpan_is_in_deep_sleep_state, WAIT_TIME)
verify(node.get(wpan.WPAN_NCP_MCU_POWER_STATE) == wpan.MCU_POWER_STATE_LOW_POWER)

# Enabling `wpantund` should update the `MCU_POWER_STATE` back to `ON`.
node.set('Daemon:Enabled', 'true')
wpan.verify_within(check_wpan_is_in_offline_state, WAIT_TIME)
verify(node.get(wpan.WPAN_NCP_MCU_POWER_STATE) == wpan.MCU_POWER_STATE_ON)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Verify sleep behavior after disabling `wpantund` ("Daemon:Enabled"
# property) when state is "associated"

node.form("disable-test")
verify(node.is_associated())
verify(node.get(wpan.WPAN_NCP_MCU_POWER_STATE) == wpan.MCU_POWER_STATE_ON)

node.set('Daemon:Enabled', 'false')
verify(node.get('Daemon:Enabled') == 'false')
wpan.verify_within(check_wpan_is_in_deep_sleep_state, WAIT_TIME)
verify(node.get(wpan.WPAN_NCP_MCU_POWER_STATE) == wpan.MCU_POWER_STATE_LOW_POWER)

node.set('Daemon:Enabled', 'true')
wpan.verify_within(check_wpan_is_in_commissioned_state, WAIT_TIME)
verify(node.get(wpan.WPAN_NCP_MCU_POWER_STATE) == wpan.MCU_POWER_STATE_ON)

node.leave()

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Verify `AutoAssociateAfterReset` behavior after reset from "deep-sleep"
# (but commissioned).

node.set('Daemon:AutoAssociateAfterReset', '1')

node.set(wpan.WPAN_NCP_MCU_POWER_STATE, wpan.MCU_POWER_STATE_LOW_POWER)
verify(node.get(wpan.WPAN_NCP_MCU_POWER_STATE) == wpan.MCU_POWER_STATE_LOW_POWER)

node.form("resume-test")
verify(node.is_associated())
verify(node.get(wpan.WPAN_NCP_MCU_POWER_STATE) == wpan.MCU_POWER_STATE_LOW_POWER)

node.reset()

# After reset, power state should remain `LOW_POWER` (wpantund would restore the value
# on NCP) and  wpantund state should start as "deep-sleep" but since AutoAssociateAfterReset
# is enabled, network should be recovered.

wpan.verify_within(check_wpan_is_in_associating_state, WAIT_TIME)
verify(node.get(wpan.WPAN_NCP_MCU_POWER_STATE) == wpan.MCU_POWER_STATE_LOW_POWER)

# -----------------------------------------------------------------------------------------------------------------------
# Test finished

wpan.Node.finalize_all_nodes()

print('\'{}\' passed.'.format(test_name))
