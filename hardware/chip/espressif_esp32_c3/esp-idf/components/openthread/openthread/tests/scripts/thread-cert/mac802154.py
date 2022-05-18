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
"""
    This module provides simple 802.15.4 MAC parser.
"""

import io
import struct

import config
from common import MacAddress, MacAddressType, MessageInfo
from net_crypto import (
    AuxiliarySecurityHeader,
    CryptoEngine,
    MacCryptoMaterialCreator,
)


class KeyIdMode0Exception(Exception):
    """
    Raised when key id mode of packet is 0.
    Such packet wouldn't be handled in test scripts,
    but it's not abnormal behavior.
    """
    pass


class DeviceDescriptors:
    """Class representing 802.15.4 Device Descriptors."""

    device_descriptors = {}

    @classmethod
    def add(cls, short_address, extended_address):
        short_address = cls._get_short_address_value(short_address)
        cls.device_descriptors[short_address] = extended_address

    @classmethod
    def get_extended(cls, short_address):
        short_address = cls._get_short_address_value(short_address)
        return cls.device_descriptors[short_address]

    @staticmethod
    def _get_short_address_value(short_address):
        if isinstance(short_address, MacAddress):
            short_address = short_address.rloc
        return short_address


class InformationElement:
    """Class representing 802.15.4 MAC Information Element."""

    def __init__(self, id, length, content):
        self._id = id
        self._length = length
        self._content = content

    @property
    def id(self):
        return self._id

    @property
    def length(self):
        return self._length

    @property
    def content(self):
        return self._content


class MacHeader:
    """Class representing 802.15.4 MAC header."""

    class FrameType:
        BEACON = 0
        DATA = 1
        ACK = 2
        COMMAND = 3

    class AddressMode:
        NOT_PRESENT = 0
        SHORT = 2
        EXTENDED = 3

    class CommandIdentifier:
        DATA_REQUEST = 4

    def __init__(
        self,
        frame_type,
        frame_pending,
        ack_request,
        frame_version,
        seq,
        dest_pan_id=None,
        dest_address=None,
        src_pan_id=None,
        src_address=None,
        command_type=None,
        aux_sec_header=None,
        mic=None,
        fcs=None,
    ):

        self.frame_type = frame_type
        self.frame_pending = frame_pending
        self.ack_request = ack_request
        self.frame_version = frame_version
        self.seq = seq

        self.dest_pan_id = dest_pan_id
        self.dest_address = dest_address
        self.src_pan_id = src_pan_id
        self.src_address = src_address
        self.command_type = command_type

        self.aux_sec_header = aux_sec_header
        self.mic = mic

        self.fcs = fcs


class MacPayload:
    """Class representing 802.15.4 MAC payload."""

    def __init__(self, data):
        self.data = bytearray(data)


class MacFrame:
    """Class representing 802.15.4 MAC frame."""

    IEEE802154_HEADER_IE_TYPE_MASK = 0x8000
    IEEE802154_HEADER_IE_ID_MASK = 0x7F80
    IEEE802154_HEADER_IE_LENGTH_MASK = 0x007F

    IEEE802154_HEADER_IE_HT1 = 0x7E
    IEEE802154_HEADER_IE_HT2 = 0x7F

    IEEE802154_VERSION_2015 = 0x02

    def parse(self, data):
        """Parse a MAC 802.15.4 frame

        Format of MAC 802.15.4 Frame:
        | Frame Header | ASH | IE | Frame Payload | MIC | FCS |

        Frame Header:  Frame Control | Sequence Number | Addressing Field
        ASH: Auxiliary Security Header (optional)
        IE: Header IE | Payload IE (optional)
        Frame Payload: Data payload (optional)
        MIC: To authenticate the frame (optional)

        In the parsing process, Frame Header is parsed first. Then it can be
        determined if security is enabled for this frame. If it's enabled,
        then ASH should be parsed.
        After that, go directly to the end to parse FCS first instead
        of parsing IE or Frame Payload. This is because sometimes, IE doesn't
        have a termination IE and parsing needs to know the start of MIC to determine
        where's the end of IEs(In such cases there are no payload).
        After parsing FCS, MIC is parsed if security is enabled.
        Then the end of IE(or payload) is known. And at last IEs and payload
        are parsed.
        And after parsing everything, build MacHeader and MacPayload with
        the things parsed.
        """
        mhr_start = data.tell()

        fc, seq = struct.unpack("<HB", data.read(3))

        frame_type = fc & 0x0007
        security_enabled = bool(fc & 0x0008)
        frame_pending = bool(fc & 0x0010)
        ack_request = bool(fc & 0x0020)
        panid_compression = bool(fc & 0x0040)
        dest_addr_mode = (fc & 0x0C00) >> 10
        frame_version = (fc & 0x3000) >> 12
        source_addr_mode = (fc & 0xC000) >> 14
        ie_present = bool(fc & 0x0200)

        if frame_type == MacHeader.FrameType.ACK:
            fcs = self._parse_fcs(data, data.tell())
            self.header = MacHeader(
                frame_type,
                frame_pending,
                ack_request,
                frame_version,
                seq,
                fcs=fcs,
            )
            self.payload = None
            return

        dest_addr_present = dest_addr_mode != MacHeader.AddressMode.NOT_PRESENT
        src_addr_present = source_addr_mode != MacHeader.AddressMode.NOT_PRESENT

        if frame_version < 2:
            dest_pan_present = dest_addr_present
        else:
            dest_pan_present = True
            if not src_addr_present:
                dest_pan_present = src_pan_present != panid_compression
            elif not dest_addr_present:
                dest_pan_present = False
            else:
                if dest_addr_mode == MacHeader.AddressMode.EXTENDED and source_addr_mode == MacHeader.AddressMode.EXTENDED and panid_compression:
                    dest_pan_present = False

        if dest_pan_present:
            dest_pan_id = struct.unpack("<H", data.read(2))[0]
        else:
            dest_pan_id = None

        dest_address = self._parse_address(data, dest_addr_mode)

        src_pan_present = not panid_compression
        while src_pan_present:
            if frame_version < 2:
                break
            if frame_version == 3:
                assert False
            if dest_addr_mode == 0:
                break
            if dest_addr_mode == 2:
                break
            if dest_addr_mode == 3 and source_addr_mode == 2:
                break
            src_pan_present = False

        if src_pan_present:
            src_pan_id = struct.unpack("<H", data.read(2))[0]
        else:
            src_pan_id = dest_pan_id

        src_address = self._parse_address(data, source_addr_mode)

        mhr_end = data.tell()

        if security_enabled:
            aux_sec_header = self._parse_aux_sec_header(data)
            aux_sec_header_end = data.tell()
        else:
            aux_sec_header = None

        # Record the beginning position of header IE.
        # Or the position of payload if there is no header IE.
        cur_pos = data.tell()
        header_ie_start = cur_pos

        data.seek(-2, io.SEEK_END)
        fcs_start = data.tell()
        fcs = self._parse_fcs(data, fcs_start)

        data.seek(fcs_start)
        if aux_sec_header and aux_sec_header.security_level:
            mic, payload_end = self._parse_mic(data, aux_sec_header.security_level)
        else:
            payload_end = fcs_start
            mic = None

        # There may be no termination IE. In such case, there is no payload, get
        # IE until the beginning position of MIC or a termination IE.
        header_ie_list = []
        data.seek(cur_pos)
        while ie_present and cur_pos + 2 < payload_end:
            header_ie = struct.unpack("<H", data.read(2))[0]
            id = (header_ie & MacFrame.IEEE802154_HEADER_IE_ID_MASK) >> 7
            # Currently, payload IE doesn't exist in the code. So only HT2 is required.
            # TODO: support HT1 when there are Payload IEs in our code
            assert id != MacFrame.IEEE802154_HEADER_IE_HT1, \
                'Currently there should be no HT1!'
            header_ie_length = (header_ie & MacFrame.IEEE802154_HEADER_IE_LENGTH_MASK)
            assert cur_pos + 2 + header_ie_length <= payload_end, \
                'Parsing Header IE error, IE id:{} length:{}'.format(id, header_ie_length)
            header_ie_content = data.read(header_ie_length)
            header_ie_list.append(InformationElement(id, header_ie_length, header_ie_content))
            cur_pos += 2 + header_ie_length
            if id == MacFrame.IEEE802154_HEADER_IE_HT2:
                break
        header_ie_end = cur_pos

        payload_pos = cur_pos
        payload_len = payload_end - payload_pos
        data.seek(payload_pos)
        payload = data.read(payload_len)

        if security_enabled:
            mhr_len = mhr_end - mhr_start
            data.seek(mhr_start)
            mhr_bytes = data.read(mhr_len)

            aux_sec_header_len = aux_sec_header_end - mhr_end
            aux_sec_hdr_bytes = data.read(aux_sec_header_len)

            extra_open_fields = bytearray([])

            if ie_present:
                data.seek(header_ie_start)
                extra_open_fields += data.read(header_ie_end - header_ie_start)

            message_info = MessageInfo()
            message_info.aux_sec_hdr = aux_sec_header
            message_info.aux_sec_hdr_bytes = aux_sec_hdr_bytes
            message_info.extra_open_fields = extra_open_fields
            message_info.mhr_bytes = mhr_bytes

            open_payload = []
            private_payload = payload

            # Check end of MAC frame
            if frame_type == MacHeader.FrameType.COMMAND:
                if frame_version < MacFrame.IEEE802154_VERSION_2015:
                    extra_open_fields.append(payload[0])
                    open_payload = payload[0:1]
                    private_payload = payload[1:]
                    message_info.open_payload_length = 1

            if src_address.type == MacAddressType.SHORT:
                message_info.source_mac_address = DeviceDescriptors.get_extended(src_address).mac_address
            else:
                message_info.source_mac_address = src_address.mac_address

            sec_obj = CryptoEngine(MacCryptoMaterialCreator(config.DEFAULT_NETWORK_KEY))
            self.payload = MacPayload(bytearray(open_payload) + sec_obj.decrypt(private_payload, mic, message_info))

        else:
            self.payload = MacPayload(payload)

        if frame_type == MacHeader.FrameType.COMMAND:
            command_type = self.payload.data[0]
        else:
            command_type = None

        # Create Header object
        self.header = MacHeader(
            frame_type,
            frame_pending,
            ack_request,
            frame_version,
            seq,
            dest_pan_id,
            dest_address,
            src_pan_id,
            src_address,
            command_type,
            aux_sec_header,
            mic,
            fcs,
        )

    def _parse_address(self, data, mode):
        if mode == MacHeader.AddressMode.SHORT:
            return MacAddress(data.read(2), MacAddressType.SHORT, big_endian=False)

        if mode == MacHeader.AddressMode.EXTENDED:
            return MacAddress(data.read(8), MacAddressType.LONG, big_endian=False)

        else:
            return None

    def _parse_aux_sec_header(self, data):
        security_control, frame_counter = struct.unpack("<BL", data.read(5))

        security_level = security_control & 0x07
        key_id_mode = (security_control & 0x18) >> 3

        if key_id_mode == 0:
            raise KeyIdMode0Exception
        elif key_id_mode == 1:
            key_id = data.read(1)
        elif key_id_mode == 2:
            key_id = data.read(5)
        elif key_id_mode == 3:
            key_id = data.read(9)
        else:
            pass

        return AuxiliarySecurityHeader(
            key_id_mode,
            security_level,
            frame_counter,
            key_id,
            big_endian=False,
        )

    def _parse_mic(self, data, security_level):
        if security_level in (1, 5):
            data.seek(-4, io.SEEK_CUR)
            payload_end = data.tell()
            mic = data.read(4)
        elif security_level in (2, 6):
            data.seek(-8, io.SEEK_CUR)
            payload_end = data.tell()
            mic = data.read(8)
        elif security_level in (3, 7):
            data.seek(-16, io.SEEK_CUR)
            payload_end = data.tell()
            mic = data.read(16)
        else:
            payload_end = data.tell()

        return mic, payload_end

    def _parse_fcs(self, data, fcs_start):
        data.seek(fcs_start)
        fcs = bytearray(data.read(2))
        return fcs
