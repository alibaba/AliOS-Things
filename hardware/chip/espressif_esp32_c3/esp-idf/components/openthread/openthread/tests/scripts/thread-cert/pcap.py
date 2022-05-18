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
#
""" Module to provide codec utilities for .pcap formatters. """

import os
import struct
import time

# https://www.tcpdump.org/linktypes.html
DLT_IEEE802_15_4_WITHFCS = 195
DLT_IEEE802_15_4_TAP = 283
PCAP_MAGIC_NUMBER = 0xA1B2C3D4
PCAP_VERSION_MAJOR = 2
PCAP_VERSION_MINOR = 4

PACKET_VERIFICATION = int(os.getenv('PACKET_VERIFICATION', 0))


class PcapCodec(object):
    """ Utility class for .pcap formatters. """

    def __init__(self, filename):
        self._dlt = DLT_IEEE802_15_4_WITHFCS if PACKET_VERIFICATION else DLT_IEEE802_15_4_TAP
        self._pcap_file = open('%s.pcap' % filename, 'wb')
        self._pcap_file.write(self.encode_header())

    def encode_header(self):
        """ Returns a pcap file header. """
        return struct.pack(
            "<LHHLLLL",
            PCAP_MAGIC_NUMBER,
            PCAP_VERSION_MAJOR,
            PCAP_VERSION_MINOR,
            0,
            0,
            256,
            self._dlt,
        )

    def encode_frame(self, frame, sec, usec):
        """ Returns a pcap encapsulation of the given frame. """
        length = 0

        if self._dlt == DLT_IEEE802_15_4_TAP:
            # Append TLVs according to 802.15.4 TAP specification:
            # https://github.com/jkcko/ieee802.15.4-tap
            pcap_tap_fcs_tlv = struct.pack("<HHL", 0, 1, 1)
            pcap_tap_channel_tlv = struct.pack("<HHHH", 3, 3, frame[0], 0)
            length = 4 + len(pcap_tap_fcs_tlv) + len(pcap_tap_channel_tlv)
            pcap_tap_header = struct.pack("<HH", 0, length)

        frame = frame[1:]
        length += len(frame)
        pcap_frame = struct.pack("<LLLL", sec, usec, length, length)

        if self._dlt == DLT_IEEE802_15_4_TAP:
            pcap_frame += pcap_tap_header + pcap_tap_fcs_tlv + pcap_tap_channel_tlv

        pcap_frame += frame
        return pcap_frame

    def _get_timestamp(self):
        """ Returns the internal timestamp. """
        timestamp = time.time()
        timestamp_sec = int(timestamp)
        timestamp_usec = int((timestamp - timestamp_sec) * 1000000)
        return timestamp_sec, timestamp_usec

    def append(self, frame, timestamp=None):
        """ Appends a frame. """
        if timestamp is None:
            timestamp = self._get_timestamp()
        pkt = self.encode_frame(frame, *timestamp)
        self._pcap_file.write(pkt)
        self._pcap_file.flush()
        return pkt

    def __del__(self):
        self._pcap_file.close()
