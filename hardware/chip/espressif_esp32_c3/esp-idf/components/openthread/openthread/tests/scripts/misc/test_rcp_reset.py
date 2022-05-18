#!/usr/bin/env python3
#
#  Copyright (c) 2021, The OpenThread Authors.
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
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS'
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
#

import argparse

from spinel.const import SPINEL
from spinel.codec import WpanApi
from spinel.stream import StreamOpen


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('rcp', type=str, help='path to rcp executable')

    args = parser.parse_args()

    stream = StreamOpen('p', f'{args.rcp} 1', False, 115200, False)
    wpan_api = WpanApi(stream, 1)

    assert wpan_api.prop_set_value(SPINEL.PROP_PHY_ENABLED, 1) == 1

    ext_addr = b'\x00\x11\x22\x33\x44\x55\x66\x77'

    assert wpan_api.prop_set_value(SPINEL.PROP_PHY_CHAN, 17) == 17
    assert wpan_api.prop_set_value(SPINEL.PROP_MAC_15_4_SADDR, 0x1234, 'H') == 0x1234
    assert wpan_api.prop_set_value(SPINEL.PROP_MAC_15_4_LADDR, ext_addr, '8s') == ext_addr
    assert wpan_api.prop_set_value(SPINEL.PROP_MAC_SRC_MATCH_ENABLED, 1) == 1

    while wpan_api.prop_insert_value(SPINEL.PROP_MAC_SRC_MATCH_EXTENDED_ADDRESSES, ext_addr, '8s') == ext_addr:
        ext_addr = (int.from_bytes(ext_addr, 'little') + 1).to_bytes(8, 'little')

    short_addr = 0x2345
    while wpan_api.prop_insert_value(SPINEL.PROP_MAC_SRC_MATCH_SHORT_ADDRESSES, short_addr, 'H') == short_addr:
        short_addr += 1

    assert wpan_api.prop_set_value(SPINEL.PROP_MAC_FILTER_MODE, SPINEL.MAC_FILTER_MODE_PROMISCUOUS) in {
        SPINEL.MAC_FILTER_MODE_PROMISCUOUS,
        SPINEL.MAC_FILTER_MODE_MONITOR,
    }
    assert wpan_api.prop_set_value(SPINEL.PROP_MAC_RAW_STREAM_ENABLED, 1) == 1

    assert wpan_api.prop_set_value(SPINEL.PROP_MAC_SCAN_MASK, 11) == [11]
    assert wpan_api.prop_set_value(SPINEL.PROP_MAC_SCAN_STATE, SPINEL.SCAN_STATE_ENERGY) == SPINEL.SCAN_STATE_ENERGY

    wpan_api.cmd_send(SPINEL.CMD_RESET)

    assert wpan_api.prop_get_value(SPINEL.PROP_PHY_ENABLED) == 0
    assert wpan_api.prop_set_value(SPINEL.PROP_PHY_ENABLED, 1) == 1

    assert wpan_api.prop_get_value(SPINEL.PROP_PHY_CHAN) == 11
    assert wpan_api.prop_get_value(SPINEL.PROP_MAC_15_4_SADDR) == 0xfffe
    assert wpan_api.prop_get_value(SPINEL.PROP_MAC_15_4_LADDR) == b'\x00' * 8

    assert wpan_api.prop_get_value(SPINEL.PROP_MAC_SRC_MATCH_ENABLED) == 0
    assert wpan_api.prop_set_value(SPINEL.PROP_MAC_SRC_MATCH_ENABLED, 1) == 1
    assert wpan_api.prop_insert_value(SPINEL.PROP_MAC_SRC_MATCH_EXTENDED_ADDRESSES, ext_addr, '8s') == ext_addr
    assert wpan_api.prop_insert_value(SPINEL.PROP_MAC_SRC_MATCH_SHORT_ADDRESSES, short_addr, 'H') == short_addr

    assert wpan_api.prop_get_value(SPINEL.PROP_MAC_FILTER_MODE) == SPINEL.MAC_FILTER_MDOE_NORMAL
    assert wpan_api.prop_get_value(SPINEL.PROP_MAC_RAW_STREAM_ENABLED) == 0
    assert wpan_api.prop_get_value(SPINEL.PROP_MAC_SCAN_STATE) == SPINEL.SCAN_STATE_IDLE


if __name__ == '__main__':
    main()
