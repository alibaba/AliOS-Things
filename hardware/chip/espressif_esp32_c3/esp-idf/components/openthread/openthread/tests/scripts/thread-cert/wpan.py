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

import struct

from collections import namedtuple
from enum import Enum

FCF_FRAME_BEACON = 0 << 0
FCF_FRAME_DATA = 1 << 0
FCF_FRAME_ACK = 2 << 0
FCF_FRAME_MAC_CMD = 3 << 0
FCF_FRAME_TYPE_MASK = 7 << 0
FCF_SECURITY_ENABLED = 1 << 3
FCF_FRAME_PENDING = 1 << 4
FCF_ACK_REQUEST = 1 << 5
FCF_PANID_COMPRESSION = 1 << 6
FCF_IE_PRESENT = 1 << 9
FCF_DST_ADDR_NONE = 0 << 10
FCF_DST_ADDR_SHORT = 2 << 10
FCF_DST_ADDR_EXT = 3 << 10
FCF_DST_ADDR_MASK = 3 << 10
FCF_FRAME_VERSION_2006 = 1 << 12
FCF_FRAME_VERSION_2015 = 2 << 12
FCF_FRAME_VERSION_MASK = 3 << 12
FCF_SRC_ADDR_NONE = 0 << 14
FCF_SRC_ADDR_SHORT = 2 << 14
FCF_SRC_ADDR_EXT = 3 << 14
FCD_SRC_ADDR_MASK = 3 << 14


class DstAddrMode(Enum):
    NONE = 0
    RESERVED = 1
    SHORT = 2
    EXTENDED = 3


class FrameType(Enum):
    BEACON = 0
    DATA = 1
    ACK = 2
    COMMAND = 3


class WpanFrameInfo(namedtuple('WpanFrameInfo', ['fcf', 'seq_no', 'dst_extaddr', 'dst_short'])):

    @property
    def frame_type(self) -> FrameType:
        return FrameType(self.fcf & 0x7)

    @property
    def dst_addr_mode(self) -> DstAddrMode:
        return DstAddrMode((self.fcf & 0x0c00) >> 10)

    @property
    def is_broadcast(self) -> bool:
        return self.dst_addr_mode == DstAddrMode.SHORT and self.dst_short == 0xffff


def _is_version_2015(fcf: int) -> bool:
    return (fcf & FCF_FRAME_VERSION_MASK) == FCF_FRAME_VERSION_2015


def _is_dst_addr_present(fcf: int) -> bool:
    dst_addr_mode = DstAddrMode((fcf & 0x0c00) >> 10)
    return dst_addr_mode != DstAddrMode.NONE


_DST_PAN_ID_NOT_PRESENT_SET = {
    FCF_DST_ADDR_NONE | FCF_SRC_ADDR_NONE,
    FCF_DST_ADDR_EXT | FCF_SRC_ADDR_NONE | FCF_PANID_COMPRESSION,
    FCF_DST_ADDR_SHORT | FCF_SRC_ADDR_NONE | FCF_PANID_COMPRESSION,
    FCF_DST_ADDR_NONE | FCF_SRC_ADDR_EXT,
    FCF_DST_ADDR_NONE | FCF_SRC_ADDR_SHORT,
    FCF_DST_ADDR_NONE | FCF_SRC_ADDR_EXT | FCF_PANID_COMPRESSION,
    FCF_DST_ADDR_NONE | FCF_SRC_ADDR_SHORT | FCF_PANID_COMPRESSION,
    FCF_DST_ADDR_EXT | FCF_SRC_ADDR_EXT | FCF_PANID_COMPRESSION,
}


def _is_dst_pan_id_present(fcf: int) -> bool:
    if _is_version_2015(fcf):
        v = fcf & (FCF_DST_ADDR_MASK | FCD_SRC_ADDR_MASK | FCF_PANID_COMPRESSION)
        present = v not in _DST_PAN_ID_NOT_PRESENT_SET
    else:
        present = _is_dst_addr_present(fcf)

    return present


def dissect(frame: bytes) -> WpanFrameInfo:
    fcf = struct.unpack("<H", frame[1:3])[0]

    seq_no = frame[3]

    dst_addr_mode = (fcf & 0x0c00) >> 10

    dst_extaddr, dst_short = None, None

    if dst_addr_mode in [DstAddrMode.SHORT.value, DstAddrMode.EXTENDED.value]:
        offset = 4

        if _is_dst_pan_id_present(fcf):
            offset += 2

        if dst_addr_mode == DstAddrMode.SHORT.value:
            dst_short = struct.unpack('<H', frame[offset:offset + 2])[0]
        else:
            dst_extaddr = '%016x' % struct.unpack('<Q', frame[offset:offset + 8])[0]

    return WpanFrameInfo(fcf=fcf, seq_no=seq_no, dst_extaddr=dst_extaddr, dst_short=dst_short)
