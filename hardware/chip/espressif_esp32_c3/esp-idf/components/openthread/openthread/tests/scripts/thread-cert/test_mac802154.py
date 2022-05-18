#!/usr/bin/env python3
#
#  Copyright (c) 2016, The OpenThread Authors.
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

import io
import unittest

from common import MacAddress, MacAddressType
import mac802154

longaddrs = bytearray([0x61, 0xcc, 0x00, 0xce, 0xfa])
shortaddrs = bytearray([0x61, 0x88, 0x00, 0xce, 0xfa])
longshortaddrs = bytearray([0x61, 0xc8, 0x00, 0xce, 0xfa])
shortlongaddrs = bytearray([0x61, 0x8c, 0x00, 0xce, 0xfa])


class TestMacParser(unittest.TestCase):

    def test_should_parse_ack_frame(self):
        frame = mac802154.MacFrame()
        frame.parse(io.BytesIO(bytearray([0x12, 0x00, 0x12, 0x34, 0x56])))

        self.assertEqual(mac802154.MacHeader.FrameType.ACK, frame.header.frame_type)
        self.assertEqual(True, frame.header.frame_pending)
        self.assertEqual(False, frame.header.ack_request)
        self.assertEqual(0, frame.header.frame_version)
        self.assertEqual(0x12, frame.header.seq)
        self.assertEqual(bytearray([0x34, 0x56]), frame.header.fcs)
        self.assertEqual(None, frame.payload)

    def test_should_parse_data_frame_with_short_addresses(self):
        frame = mac802154.MacFrame()
        frame.parse(
            io.BytesIO(bytearray([0x61, 0x88, 0x34, 0xce, 0xfa, 0xad, 0xde, 0xef, 0xbe, 0x12, 0x34, 0xfe, 0xdc])))

        self.assertEqual(mac802154.MacHeader.FrameType.DATA, frame.header.frame_type)
        self.assertEqual(False, frame.header.frame_pending)
        self.assertEqual(True, frame.header.ack_request)
        self.assertEqual(0, frame.header.frame_version)
        self.assertEqual(0x34, frame.header.seq)
        self.assertEqual(bytearray([0xfe, 0xdc]), frame.header.fcs)
        self.assertEqual(0xface, frame.header.dest_pan_id)
        self.assertEqual(0xdead, frame.header.dest_address.rloc)
        self.assertEqual(0xface, frame.header.src_pan_id)
        self.assertEqual(0xbeef, frame.header.src_address.rloc)

        self.assertEqual(bytearray([0x12, 0x34]), frame.payload.data)

    def test_should_parse_data_frame_with_extended_addresses(self):
        frame = mac802154.MacFrame()
        frame.parse(
            io.BytesIO(
                bytearray([
                    0x61, 0xcc, 0x56, 0xce, 0xfa, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0x0b, 0xad, 0xf0,
                    0x0d, 0xba, 0xd0, 0xd0, 0x0d, 0x12, 0x34, 0xfe, 0xdc
                ])))

        self.assertEqual(mac802154.MacHeader.FrameType.DATA, frame.header.frame_type)
        self.assertEqual(False, frame.header.frame_pending)
        self.assertEqual(True, frame.header.ack_request)
        self.assertEqual(0, frame.header.frame_version)
        self.assertEqual(0x56, frame.header.seq)
        self.assertEqual(bytearray([0xfe, 0xdc]), frame.header.fcs)
        self.assertEqual(0xface, frame.header.dest_pan_id)
        self.assertEqual(bytearray(reversed([0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef])),
                         frame.header.dest_address.mac_address)
        self.assertEqual(0xface, frame.header.src_pan_id)
        self.assertEqual(bytearray(reversed([0x0b, 0xad, 0xf0, 0x0d, 0xba, 0xd0, 0xd0, 0x0d])),
                         frame.header.src_address.mac_address)

        self.assertEqual(bytearray([0x12, 0x34]), frame.payload.data)

    def test_should_parse_data_frame_with_short_and_extended_addresses(self):
        frame = mac802154.MacFrame()
        frame.parse(
            io.BytesIO(
                bytearray([
                    0x61, 0xc8, 0x56, 0xce, 0xfa, 0xad, 0xde, 0x0b, 0xad, 0xf0, 0x0d, 0xba, 0xd0, 0xd0, 0x0d, 0x12,
                    0x34, 0xfe, 0xdc
                ])))

        self.assertEqual(mac802154.MacHeader.FrameType.DATA, frame.header.frame_type)
        self.assertEqual(False, frame.header.frame_pending)
        self.assertEqual(True, frame.header.ack_request)
        self.assertEqual(0, frame.header.frame_version)
        self.assertEqual(0x56, frame.header.seq)
        self.assertEqual(bytearray([0xfe, 0xdc]), frame.header.fcs)
        self.assertEqual(0xface, frame.header.dest_pan_id)
        self.assertEqual(0xdead, frame.header.dest_address.rloc)
        self.assertEqual(0xface, frame.header.src_pan_id)
        self.assertEqual(bytearray(reversed([0x0b, 0xad, 0xf0, 0x0d, 0xba, 0xd0, 0xd0, 0x0d])),
                         frame.header.src_address.mac_address)

        self.assertEqual(bytearray([0x12, 0x34]), frame.payload.data)

    def test_should_parse_data_frame_with_extended_and_short_addresses(self):
        frame = mac802154.MacFrame()
        frame.parse(
            io.BytesIO(
                bytearray([
                    0x61, 0x8c, 0x56, 0xce, 0xfa, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0x0d, 0xf0, 0x12,
                    0x34, 0xfe, 0xdc
                ])))

        self.assertEqual(mac802154.MacHeader.FrameType.DATA, frame.header.frame_type)
        self.assertEqual(False, frame.header.frame_pending)
        self.assertEqual(True, frame.header.ack_request)
        self.assertEqual(0, frame.header.frame_version)
        self.assertEqual(0x56, frame.header.seq)
        self.assertEqual(bytearray([0xfe, 0xdc]), frame.header.fcs)
        self.assertEqual(0xface, frame.header.dest_pan_id)
        self.assertEqual(bytearray(reversed([0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef])),
                         frame.header.dest_address.mac_address)
        self.assertEqual(0xface, frame.header.src_pan_id)
        self.assertEqual(0xf00d, frame.header.src_address.rloc)

        self.assertEqual(bytearray([0x12, 0x34]), frame.payload.data)

    def test_should_parse_data_request_command(self):
        frame = mac802154.MacFrame()
        frame.parse(io.BytesIO(bytearray([0x63, 0x88, 0x78, 0xce, 0xfa, 0xad, 0xde, 0x0d, 0xf0, 0x04, 0xfe, 0xdc])))

        self.assertEqual(mac802154.MacHeader.FrameType.COMMAND, frame.header.frame_type)
        self.assertEqual(False, frame.header.frame_pending)
        self.assertEqual(True, frame.header.ack_request)
        self.assertEqual(0, frame.header.frame_version)
        self.assertEqual(0x78, frame.header.seq)
        self.assertEqual(bytearray([0xfe, 0xdc]), frame.header.fcs)
        self.assertEqual(0xface, frame.header.dest_pan_id)
        self.assertEqual(0xdead, frame.header.dest_address.rloc)
        self.assertEqual(0xface, frame.header.src_pan_id)
        self.assertEqual(0xf00d, frame.header.src_address.rloc)

        self.assertEqual(bytearray([0x4]), frame.payload.data)

    def test_should_decrypt_data_frame(self):

        mac802154.DeviceDescriptors.add(
            0x2001, MacAddress(bytearray([0x16, 0x6e, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x07]), MacAddressType.LONG))

        frame = mac802154.MacFrame()
        frame.parse(
            io.BytesIO(
                bytearray([
                    0x69,
                    0x98,
                    0x68,  # FC, seq
                    0xce,
                    0xfa,  # Pan Id
                    0x00,
                    0x20,  # Dst addr
                    0x01,
                    0x20,  # Src addr
                    0x0d,
                    0x00,
                    0x00,
                    0x00,
                    0x00,
                    0x01,  # Aux Security Header
                    0xb5,
                    0x5a,
                    0x0d,
                    0x8e,
                    0x18,
                    0x5c,
                    0xb1,
                    0x06,  # Payload
                    0xc4,
                    0x6f,
                    0x7d,
                    0x6b,
                    0xb5,
                    0x4a,
                    0x87,
                    0x14,
                    0xae,
                    0xdd,
                    0x8e,
                    0xb7,
                    0x37,
                    0x62,
                    0x27,
                    0x48,
                    0xc9,
                    0x53,
                    0x0c,
                    0x44,
                    0x31,
                    0x59,
                    0x8b,
                    0xa2,
                    0x83,
                    0x59,
                    0xa1,
                    0x43,  # MIC (valid)
                    0x74,
                    0xe0,
                    0x2a,
                    0xf6,
                    0x99,
                    0xfc
                ])))  # FCS (valid)

        self.assertEqual(mac802154.MacHeader.FrameType.DATA, frame.header.frame_type)
        self.assertEqual(False, frame.header.frame_pending)
        self.assertEqual(True, frame.header.ack_request)
        self.assertEqual(1, frame.header.frame_version)
        self.assertEqual(0x68, frame.header.seq)
        self.assertEqual(bytearray([0x99, 0xfc]), frame.header.fcs)
        self.assertEqual(0xface, frame.header.dest_pan_id)
        self.assertEqual(0x2000, frame.header.dest_address.rloc)
        self.assertEqual(0xface, frame.header.src_pan_id)
        self.assertEqual(0x2001, frame.header.src_address.rloc)

        self.assertEqual(0, frame.header.aux_sec_header.frame_counter)
        self.assertEqual(5, frame.header.aux_sec_header.security_level)

        self.assertEqual(
            bytes(
                bytearray([
                    0x7c, 0x77, 0x80, 0xf0, 0x4d, 0x4d, 0x4d, 0x4d, 0xe0, 0x04, 0x44, 0x02, 0x44, 0x66, 0x13, 0x5f,
                    0x22, 0x80, 0xb1, 0x61, 0x02, 0x61, 0x73, 0x11, 0x2a, 0xff, 0x01, 0x08, 0x16, 0x6e, 0x0a, 0x00,
                    0x00, 0x00, 0x00, 0x07
                ])), frame.payload.data)

    def test_should_decrypt_command_frame(self):
        frame = mac802154.MacFrame()
        frame.parse(
            io.BytesIO(
                bytearray([
                    0x6b, 0xdc, 0xce, 0xce, 0xfa, 0x02, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x6e, 0x16, 0x03, 0x00, 0x00,
                    0x00, 0x00, 0x0a, 0x6e, 0x16, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x01, 0x04, 0x2d, 0xbc, 0x12, 0xbe,
                    0x0a, 0x4f
                ])))

        self.assertEqual(mac802154.MacHeader.FrameType.COMMAND, frame.header.frame_type)
        self.assertEqual(False, frame.header.frame_pending)
        self.assertEqual(True, frame.header.ack_request)
        self.assertEqual(1, frame.header.frame_version)
        self.assertEqual(206, frame.header.seq)
        self.assertEqual(bytearray([0x0a, 0x4f]), frame.header.fcs)
        self.assertEqual(0xface, frame.header.dest_pan_id)
        self.assertEqual(bytearray([0x16, 0x6e, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x02]),
                         frame.header.dest_address.mac_address)
        self.assertEqual(0xface, frame.header.src_pan_id)
        self.assertEqual(bytearray([0x16, 0x6e, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x03]),
                         frame.header.src_address.mac_address)

        self.assertEqual(0, frame.header.aux_sec_header.frame_counter)
        self.assertEqual(5, frame.header.aux_sec_header.security_level)


if __name__ == "__main__":
    unittest.main()
