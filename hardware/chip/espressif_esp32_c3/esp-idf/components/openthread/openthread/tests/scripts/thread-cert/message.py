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
import ipaddress
import struct

import coap
import common
import dtls
import ipv6
import mac802154
import mle

from enum import IntEnum


class DropPacketException(Exception):
    pass


class MessageType(IntEnum):
    MLE = 0
    COAP = 1
    ICMP = 2
    ACK = 3
    BEACON = 4
    DATA = 5
    COMMAND = 6
    DTLS = 7


class Message(object):

    def __init__(self):
        self._type = None
        self._channel = None
        self._mac_header = None
        self._ipv6_packet = None
        self._coap = None
        self._mle = None
        self._icmp = None
        self._dtls = None

    def _extract_udp_datagram(self, udp_datagram):
        if isinstance(udp_datagram.payload, mle.MleMessage):
            self._type = MessageType.MLE
            self._mle = udp_datagram.payload

        elif isinstance(udp_datagram.payload, (coap.CoapMessage, coap.CoapMessageProxy)):
            self._type = MessageType.COAP
            self._coap = udp_datagram.payload

        # DTLS message factory returns a list of messages
        elif isinstance(udp_datagram.payload, list):
            self._type = MessageType.DTLS
            self._dtls = udp_datagram.payload

    def _extract_upper_layer_protocol(self, upper_layer_protocol):
        if isinstance(upper_layer_protocol, ipv6.ICMPv6):
            self._type = MessageType.ICMP
            self._icmp = upper_layer_protocol

        elif isinstance(upper_layer_protocol, ipv6.UDPDatagram):
            self._extract_udp_datagram(upper_layer_protocol)

    def try_extract_dtls_messages(self):
        """Extract multiple dtls messages that are sent in a single UDP datagram
        """
        if self.type != MessageType.DTLS:
            return [self.clone()]

        assert isinstance(self.dtls, list)
        ret = []
        for dtls in self.dtls:
            msg = self.clone()
            msg._dtls = dtls
            ret.append(msg)
        return ret

    def clone(self):
        msg = Message()
        msg._type = self.type
        msg._channel = self.channel
        msg._mac_header = self.mac_header
        msg._ipv6_packet = self.ipv6_packet
        msg._coap = self.coap
        msg._mle = self.mle
        msg._icmp = self.icmp
        msg._dtls = self.dtls
        return msg

    @property
    def type(self):
        return self._type

    @type.setter
    def type(self, value):
        self._type = value

    @property
    def channel(self):
        return self._channel

    @channel.setter
    def channel(self, value):
        self._channel = value

    @property
    def mac_header(self):
        return self._mac_header

    @mac_header.setter
    def mac_header(self, value):
        self._mac_header = value

        if self._mac_header.frame_type == mac802154.MacHeader.FrameType.BEACON:
            self._type = MessageType.BEACON

        elif self._mac_header.frame_type == mac802154.MacHeader.FrameType.ACK:
            self._type = MessageType.ACK

        elif self._mac_header.frame_type == mac802154.MacHeader.FrameType.DATA:
            self._type = MessageType.DATA
        elif (self._mac_header.frame_type == mac802154.MacHeader.FrameType.COMMAND):
            self._type = MessageType.COMMAND
        else:
            raise ValueError('Invalid mac frame type %d' % self._mac_header.frame_type)

    @property
    def ipv6_packet(self):
        return self._ipv6_packet

    @ipv6_packet.setter
    def ipv6_packet(self, value):
        self._ipv6_packet = value
        self._extract_upper_layer_protocol(value.upper_layer_protocol)

    @property
    def coap(self):
        return self._coap

    @property
    def mle(self):
        return self._mle

    @property
    def icmp(self):
        return self._icmp

    @icmp.setter
    def icmp(self, value):
        self._icmp = value

    @property
    def dtls(self):
        return self._dtls

    def get_mle_message_tlv(self, tlv_class_type):
        if self.type != MessageType.MLE:
            raise ValueError("Invalid message type. Expected MLE message.")

        for tlv in self.mle.command.tlvs:
            if isinstance(tlv, tlv_class_type):
                return tlv

    def assertMleMessageIsType(self, command_type):
        if self.type != MessageType.MLE:
            raise ValueError("Invalid message type. Expected MLE message.")

        assert self.mle.command.type == command_type

    def assertMleMessageContainsTlv(self, tlv_class_type):
        """To confirm if Mle message contains the TLV type.

        Args:
            tlv_class_type: tlv's type.

        Returns:
            mle.Route64: If contains the TLV, return it.
        """
        if self.type != MessageType.MLE:
            raise ValueError("Invalid message type. Expected MLE message.")

        contains_tlv = False
        for tlv in self.mle.command.tlvs:
            if isinstance(tlv, tlv_class_type):
                contains_tlv = True
                break

        assert contains_tlv
        return tlv

    def assertAssignedRouterQuantity(self, router_quantity):
        """Confirm if Leader contains the Route64 TLV with router_quantity assigned Router IDs.

        Args:
            router_quantity: the quantity of router.
        """
        tlv = self.assertMleMessageContainsTlv(mle.Route64)
        router_id_mask = tlv.router_id_mask

        count = 0
        for i in range(1, 65):
            count += router_id_mask & 1
            router_id_mask = router_id_mask >> 1
        assert count == router_quantity

    def assertMleMessageDoesNotContainTlv(self, tlv_class_type):
        if self.type != MessageType.MLE:
            raise ValueError("Invalid message type. Expected MLE message.")

        contains_tlv = False
        for tlv in self.mle.command.tlvs:
            if isinstance(tlv, tlv_class_type):
                contains_tlv = True
                break

        assert contains_tlv is False

    def assertMleMessageContainsOptionalTlv(self, tlv_class_type):
        if self.type != MessageType.MLE:
            raise ValueError("Invalid message type. Expected MLE message.")

        contains_tlv = False
        for tlv in self.mle.command.tlvs:
            if isinstance(tlv, tlv_class_type):
                contains_tlv = True
                break

        if contains_tlv:
            print("MleMessage contains optional TLV: {}".format(tlv_class_type))
        else:
            print("MleMessage doesn't contain optional TLV: {}".format(tlv_class_type))

    def get_coap_message_tlv(self, tlv_class_type):
        if self.type != MessageType.COAP:
            raise ValueError("Invalid message type. Expected CoAP message.")

        for tlv in self.coap.payload:
            if isinstance(tlv, tlv_class_type):
                return tlv

    def assertCoapMessageContainsTlv(self, tlv_class_type):
        if self.type != MessageType.COAP:
            raise ValueError("Invalid message type. Expected CoAP message.")

        contains_tlv = False
        for tlv in self.coap.payload:
            if isinstance(tlv, tlv_class_type):
                contains_tlv = True
                break

        assert contains_tlv

    def assertCoapMessageDoesNotContainTlv(self, tlv_class_type):
        if self.type != MessageType.COAP:
            raise ValueError("Invalid message type. Expected COAP message.")

        contains_tlv = False
        for tlv in self.coap.payload:
            if isinstance(tlv, tlv_class_type):
                contains_tlv = True
                break

        assert contains_tlv is False

    def assertCoapMessageContainsOptionalTlv(self, tlv_class_type):
        if self.type != MessageType.COAP:
            raise ValueError("Invalid message type. Expected CoAP message.")

        for tlv in self.coap.payload:
            if isinstance(tlv, tlv_class_type):
                break

        print("CoapMessage doesn't contain optional TLV: {}".format(tlv_class_type))

    def assertCoapMessageRequestUriPath(self, uri_path):
        if self.type != MessageType.COAP:
            raise ValueError("Invalid message type. Expected CoAP message.")

        assert uri_path == self.coap.uri_path

    def assertCoapMessageCode(self, code):
        if self.type != MessageType.COAP:
            raise ValueError("Invalid message type. Expected CoAP message.")

        assert code == self.coap.code

    def assertSentToNode(self, node):
        sent_to_node = False
        dst_addr = self.ipv6_packet.ipv6_header.destination_address

        for addr in node.get_addrs():
            if dst_addr == ipaddress.ip_address(addr):
                sent_to_node = True

        if self.mac_header.dest_address.type == common.MacAddressType.SHORT:
            mac_address = common.MacAddress.from_rloc16(node.get_addr16())
            if self.mac_header.dest_address == mac_address:
                sent_to_node = True

        elif self.mac_header.dest_address.type == common.MacAddressType.LONG:
            mac_address = common.MacAddress.from_eui64(bytearray(node.get_addr64(), encoding="utf-8"))
            if self.mac_header.dest_address == mac_address:
                sent_to_node = True

        assert sent_to_node

    def assertSentToDestinationAddress(self, ipv6_address):
        assert (self.ipv6_packet.ipv6_header.destination_address == ipaddress.ip_address(ipv6_address))

    def assertSentFromSourceAddress(self, ipv6_address):
        assert (self.ipv6_packet.ipv6_header.source_address == ipaddress.ip_address(ipv6_address))

    def assertSentWithHopLimit(self, hop_limit):
        assert self.ipv6_packet.ipv6_header.hop_limit == hop_limit

    def isMacAddressTypeLong(self):
        return self.mac_header.dest_address.type == common.MacAddressType.LONG

    def get_dst_udp_port(self):
        assert isinstance(self.ipv6_packet.upper_layer_protocol, ipv6.UDPDatagram)
        return self.ipv6_packet.upper_layer_protocol.header.dst_port

    def is_data_poll(self):
        return self._type == MessageType.COMMAND and \
            self._mac_header.command_type == mac802154.MacHeader.CommandIdentifier.DATA_REQUEST

    def __repr__(self):
        if (self.type == MessageType.DTLS and self.dtls.content_type == dtls.ContentType.HANDSHAKE):
            return "Message(type={})".format(str(self.dtls.handshake_type))
        return "Message(type={})".format(MessageType(self.type).name)


class MessagesSet(object):

    def __init__(self, messages, commissioning_messages=()):
        self._messages = messages
        self._commissioning_messages = commissioning_messages

    @property
    def messages(self):
        return self._messages

    @property
    def commissioning_messages(self):
        return self._commissioning_messages

    def next_data_poll(self):
        while True:
            message = self.next_message_of(MessageType.COMMAND, False)
            if not message:
                break
            elif message.is_data_poll():
                return message

    def next_coap_message(self, code, uri_path=None, assert_enabled=True):
        message = None

        while self.messages:
            m = self.messages.pop(0)

            if m.type != MessageType.COAP:
                continue

            if uri_path is not None and m.coap.uri_path != uri_path:
                continue

            else:
                if not m.coap.code.is_equal_dotted(code):
                    continue

            message = m
            break

        if assert_enabled:
            assert (message is not None), "Could not find CoapMessage with code: {}".format(code)

        return message

    def last_mle_message(self, command_type, assert_enabled=True):
        """Get the last Mle Message with specified type from existing capture.

        Args:
            command_type: the specified mle type.
            assert_enabled: interrupt or not when get the mle.

        Returns:
            message.Message: the last Mle Message with specified type.
        """
        message = None
        size = len(self.messages)

        for i in range(size - 1, -1, -1):
            m = self.messages[i]

            if m.type != MessageType.MLE:
                continue

            # for command_type in command_types:
            if m.mle.command.type == command_type:
                message = m
                break

        if assert_enabled:
            assert (message is not None), "Could not find MleMessage with type: {}".format(command_type)

        return message

    def next_mle_message(self, command_type, assert_enabled=True, sent_to_node=None):
        message = self.next_mle_message_of_one_of_command_types(command_type)

        if assert_enabled:
            assert (message is not None), "Could not find MleMessage of the type: {}".format(command_type)

        if sent_to_node is not None:
            message.assertSentToNode(sent_to_node)

        return message

    def next_mle_message_of_one_of_command_types(self, *command_types):
        message = None

        while self.messages:
            m = self.messages.pop(0)

            if m.type != MessageType.MLE:
                continue

            command_found = False

            for command_type in command_types:
                if m.mle.command.type == command_type:
                    command_found = True
                    break

            if command_found:
                message = m
                break

        return message

    def next_message(self, assert_enabled=True):
        message = self.messages.pop(0)
        if assert_enabled:
            assert message is not None, "Could not find next Message"
        return message

    def next_message_of(self, message_type, assert_enabled=True):
        message = None

        while self.messages:
            m = self.messages.pop(0)
            if m.type != message_type:
                continue

            message = m
            break

        if assert_enabled:
            assert (message is not None), "Could not find Message of the type: {}".format(message_type)

        return message

    def next_data_message(self):
        return self.next_message_of(MessageType.DATA)

    def next_command_message(self):
        return self.next_message_of(MessageType.COMMAND)

    def next_dtls_message(self, content_type, handshake_type=None):
        while self.messages:
            msg = self.messages.pop(0)
            if msg.type != MessageType.DTLS:
                continue
            if msg.dtls.content_type != content_type:
                continue
            if (content_type == dtls.ContentType.HANDSHAKE and msg.dtls.handshake_type != handshake_type):
                continue
            return msg

        t = (handshake_type if content_type == dtls.ContentType.HANDSHAKE else content_type)
        raise ValueError("Could not find DTLS message of type: {}".format(str(t)))

    def contains_icmp_message(self):
        for m in self.messages:
            if m.type == MessageType.ICMP:
                return True

        return False

    def get_icmp_message(self, icmp_type):
        for m in self.messages:
            if m.type != MessageType.ICMP:
                continue

            if m.icmp.header.type == icmp_type:
                return m

        return None

    def contains_mle_message(self, command_type):
        for m in self.messages:
            if m.type != MessageType.MLE:
                continue

            if m.mle.command.type == command_type:
                return True

        return False

    def does_not_contain_coap_message(self):
        for m in self.messages:
            if m.type != MessageType.COAP:
                continue

            return False

        return True

    def clone(self):
        """Make a copy of current MessageSet.
        """
        return MessagesSet(self.messages[:], self.commissioning_messages[:])

    def __repr__(self):
        return str(self.messages)


class MessageFactory:

    def __init__(self, lowpan_parser):
        self._lowpan_parser = lowpan_parser

    def _add_device_descriptors(self, message):
        for tlv in message.mle.command.tlvs:

            if isinstance(tlv, mle.SourceAddress):
                mac802154.DeviceDescriptors.add(tlv.address, message.mac_header.src_address)

            if isinstance(tlv, mle.Address16):
                mac802154.DeviceDescriptors.add(tlv.address, message.mac_header.dest_address)

    def _parse_mac_frame(self, data):
        mac_frame = mac802154.MacFrame()
        mac_frame.parse(data)
        return mac_frame

    def set_lowpan_context(self, cid, prefix):
        self._lowpan_parser.set_lowpan_context(cid, prefix)

    def create(self, data):
        try:
            message = Message()
            message.channel = struct.unpack(">B", data.read(1))

            # Parse MAC header
            mac_frame = self._parse_mac_frame(data)
            message.mac_header = mac_frame.header

            if message.mac_header.frame_type != mac802154.MacHeader.FrameType.DATA:
                return [message]

            message_info = common.MessageInfo()
            message_info.source_mac_address = message.mac_header.src_address
            message_info.destination_mac_address = message.mac_header.dest_address

            # Create stream with 6LoWPAN datagram
            lowpan_payload = io.BytesIO(mac_frame.payload.data)

            ipv6_packet = self._lowpan_parser.parse(lowpan_payload, message_info)
            if ipv6_packet is None:
                return [message]

            message.ipv6_packet = ipv6_packet

            if message.type == MessageType.MLE:
                self._add_device_descriptors(message)

            return message.try_extract_dtls_messages()

        except mac802154.KeyIdMode0Exception:
            print('Received packet with key_id_mode = 0, cannot be handled in test scripts')
            raise DropPacketException
