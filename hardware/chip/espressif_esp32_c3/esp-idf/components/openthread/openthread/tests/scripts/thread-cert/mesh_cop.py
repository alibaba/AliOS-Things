#!/usr/bin/env python3
#
#  Copyright (c) 2019, The OpenThread Authors.
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

from binascii import hexlify
from enum import IntEnum
import io
import logging
import struct

from network_data import SubTlvsFactory
from tlvs_parsing import UnknownTlvFactory
import common


class TlvType(IntEnum):
    CHANNEL = 0
    PAN_ID = 1
    EXTENDED_PANID = 2
    NETWORK_NAME = 3
    PSKC = 4
    NETWORK_KEY = 5
    NETWORK_KEY_SEQUENCE_COUNTER = 6
    NETWORK_MESH_LOCAL_PREFIX = 7
    STEERING_DATA = 8
    BORDER_AGENT_LOCATOR = 9
    COMMISSIONER_ID = 10
    COMMISSIONER_SESSION_ID = 11
    SECURITY_POLICY = 12
    GET = 13
    ACTIVE_TIMESTAMP = 14
    COMMISSIONER_UDP_PORT = 15
    STATE = 16
    JOINER_DTLS_ENCAPSULATION = 17
    JOINER_UDP_PORT = 18
    JOINER_IID = 19
    JOINER_ROUTER_LOCATOR = 20
    JOINER_ROUTER_KEK = 21
    PROVISIONING_URL = 32
    VENDOR_NAME = 33
    VENDOR_MODEL = 34
    VENDOR_SW_VERSION = 35
    VENDOR_DATA = 36
    VENDOR_STACK_VERSION = 37
    UDP_ENCAPSULATION = 48
    IPV6_ADDRESS = 49
    PENDING_TIMESTAMP = 51
    DELAY_TIMER = 52
    CHANNEL_MASK = 53
    COUNT = 54
    PERIOD = 55
    SCAN_DURATION = 56
    ENERGY_LIST = 57
    CSL_SYNCHRONIZED_TIMEOUT = 85
    CSL_CLOCK_ACCURACY = 86
    DISCOVERY_REQUEST = 128
    DISCOVERY_RESPONSE = 129


class MeshCopState(IntEnum):
    ACCEPT = 0x1
    REJECT = 0xff


class MeshCopMessageType(IntEnum):
    JOIN_FIN_REQ = (1,)
    JOIN_FIN_RSP = (2,)
    JOIN_ENT_NTF = (3,)
    JOIN_ENT_RSP = 4


def create_mesh_cop_message_type_set():
    return [
        MeshCopMessageType.JOIN_FIN_REQ,
        MeshCopMessageType.JOIN_FIN_RSP,
        MeshCopMessageType.JOIN_ENT_NTF,
        MeshCopMessageType.JOIN_ENT_RSP,
    ]


# Channel TLV (0)
class Channel(object):

    def __init__(self, channel_page, channel):
        self._channel_page = channel_page
        self._channel = channel

    @property
    def channel_page(self):
        return self._channel_page

    @property
    def channel(self):
        return self._channel

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return (self._channel_page == other._channel_page and self._channel == other.__channel)

    def __repr__(self):
        return 'Channel(channel_page={},channel={})'.format(self._channel_page, self._channel)

    def to_hex(self):
        return struct.pack('>BBBH', TlvType.CHANNEL, 3, self.channel_page, self.channel)


class ChannelFactory(object):

    def parse(self, data, message_info):
        data_tp = struct.unpack('>BH', data.read(3))
        channel_page = data_tp[0]
        channel = data_tp[1]
        return Channel(channel_page, channel)


# PanId TLV (1)
class Panid(object):
    # TODO: Not implemented yet
    pass


class PanidFactory(object):
    # TODO: Not implemented yet

    def parse(self, data, message_info):
        raise NotImplementedError("TODO: Not implemented yet")


# ExtendedPanid TLV (2)
class ExtendedPanid(object):

    def __init__(self, extended_panid):
        self._extended_panid = extended_panid

    @property
    def extended_panid(self):
        return self._extended_panid

    def __eq__(self, other):
        return (isinstance(self, type(other)) and self.extended_panid == other.extended_panid)

    def __repr__(self):
        return "ExtendedPanid(extended_panid={})".format(self.extended_panid)


class ExtendedPanidFactory(object):

    def parse(self, data, message_info):
        extended_panid = struct.unpack(">Q", data.read(8))[0]
        return ExtendedPanid(extended_panid)


# NetworkName TLV (3)
class NetworkName(object):

    def __init__(self, network_name):
        self._network_name = network_name

    @property
    def network_name(self):
        return self._network_name

    def __eq__(self, other):
        return (isinstance(self, type(other)) and self.network_name == other.network_name)

    def __repr__(self):
        return "NetworkName(network_name={})".format(self.network_name)


class NetworkNameFactory(object):

    def parse(self, data, message_info):
        len = message_info.length
        network_name = struct.unpack("{}s".format(10), data.read(len))[0]
        return NetworkName(network_name)


# PSKc TLV (4)
class PSKc(object):
    # TODO: Not implemented yet
    pass


class PSKcFactory(object):
    # TODO: Not implemented yet

    def parse(self, data, message_info):
        raise NotImplementedError("TODO: Not implemented yet")


# NetworkKey TLV (5)
class NetworkKey(object):
    # TODO: Not implemented yet
    pass


class NetworkKeyFactory(object):
    # TODO: Not implemented yet

    def parse(self, data, message_info):
        raise NotImplementedError("TODO: Not implemented yet")


# NetworkKeySequenceCounter TLV (6)
class NetworkKeySequenceCounter(object):
    # TODO: Not implemented yet
    pass


class NetworkKeySequenceCounterFactory(object):
    # TODO: Not implemented yet

    def parse(self, data, message_info):
        raise NotImplementedError("TODO: Not implemented yet")


# NetworkMeshLocalPrefix TLV (7)
class NetworkMeshLocalPrefix(object):
    # TODO: Not implemented yet
    pass


class NetworkMeshLocalPrefixFactory(object):
    # TODO: Not implemented yet

    def parse(self, data, message_info):
        raise NotImplementedError("TODO: Not implemented yet")


# Steering Data TLV (8)
class SteeringData(object):

    def __init__(self, bloom_filter):
        self._bloom_filter = bloom_filter

    @property
    def bloom_filter(self):
        return self._bloom_filter

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return self._bloom_filter == other._bloom_filter

    def __repr__(self):
        return "SteeringData(bloom_filter={})".format(hexlify(self._bloom_filter))

    def to_hex(self):
        bloom_filter_len = len(self.bloom_filter)
        return (struct.pack('>BB', TlvType.STEERING_DATA, bloom_filter_len) + self.bloom_filter)


class SteeringDataFactory:

    def parse(self, data, message_info):
        bloom_filter = data.read(message_info.length)
        return SteeringData(bloom_filter)


# Border Agent Locator TLV (9)
class BorderAgentLocator(object):

    def __init__(self, address):
        self._border_agent_locator = address

    @property
    def border_agent_locator(self):
        return self._border_agent_locator

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return self._border_agent_locator == other._border_agent_locator

    def __repr__(self):
        return "BorderAgentLocator(rloc16={})".format(hex(self._border_agent_locator))

    def to_hex(self):
        return struct.pack('>BBH', TlvType.BORDER_AGENT_LOCATOR, 2, self.border_agent_locator)


class BorderAgentLocatorFactory:

    def parse(self, data, message_info):
        border_agent_locator = struct.unpack(">H", data.read(2))[0]
        return BorderAgentLocator(border_agent_locator)


# CommissionerId TLV (10)
class CommissionerId(object):

    def __init__(self, commissioner_id):
        self._commissioner_id = commissioner_id

    @property
    def commissioner_id(self):
        return self._commissioner_id

    def __eq__(self, other):
        return self.commissioner_id == other.commissioner_id

    def __repr__(self):
        return "CommissionerId(commissioner_id={})".format(self.commissioner_id)


class CommissionerIdFactory(object):

    def parse(self, data, message_info):
        commissioner_id = data.getvalue().decode('utf-8')
        return CommissionerId(commissioner_id)


# Commissioner Session ID TLV (11)
class CommissionerSessionId(object):

    def __init__(self, commissioner_session_id):
        self._commissioner_session_id = commissioner_session_id

    @property
    def commissioner_session_id(self):
        return self._commissioner_session_id

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return self._commissioner_session_id == other._commissioner_session_id

    def __repr__(self):
        return "CommissionerSessionId(commissioner_session_id={})".format(self._commissioner_session_id)

    def to_hex(self):
        return struct.pack(
            '>BBH',
            TlvType.COMMISSIONER_SESSION_ID,
            2,
            self.commissioner_session_id,
        )


class CommissionerSessionIdFactory:

    def parse(self, data, message_info):
        session_id = struct.unpack(">H", data.read(2))[0]
        return CommissionerSessionId(session_id)


# SecurityPolicy TLV (12)
class SecurityPolicy(object):
    # TODO: Not implemented yet
    pass


class SecurityPolicyFactory(object):
    # TODO: Not implemented yet

    def parse(self, data, message_info):
        raise NotImplementedError("TODO: Not implemented yet")


# Get TLV (13)
class Get(object):
    # TODO: Not implemented yet
    pass


class GetFactory(object):
    # TODO: Not implemented yet

    def parse(self, data, message_info):
        raise NotImplementedError("TODO: Not implemented yet")


# ActiveTimestamp TLV (14)
class ActiveTimestamp(object):
    # TODO: Not implemented yet
    pass


class ActiveTimestampFactory(object):
    # TODO: Not implemented yet

    def parse(self, data, message_info):
        raise NotImplementedError("TODO: Not implemented yet")


# Commissioner UDP Port TLV (15)
class CommissionerUdpPort(object):

    def __init__(self, udp_port):
        self._udp_port = udp_port

    @property
    def udp_port(self):
        return self._udp_port

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return self._udp_port == other._udp_port

    def __repr__(self):
        return "CommissionerUdpPort(udp_port={})".format(self._udp_port)


class CommissionerUdpPortFactory:

    def parse(self, data, message_info):
        udp_port = struct.unpack(">H", data.read(2))[0]
        return CommissionerUdpPort(udp_port)


# State TLV (16)
class State(object):

    def __init__(self, state):
        self._state = state

    @property
    def state(self):
        return self._state

    def __eq__(self, other):
        return self.state == other.state

    def __repr__(self):
        return "State(state={})".format(self.state)


class StateFactory:

    def parse(self, data, message_info):
        state = ord(data.read(1))
        return State(state)


# JoinerDtlsEncapsulation TLV (17)
class JoinerDtlsEncapsulation(object):
    # TODO: Not implemented yet
    pass


class JoinerDtlsEncapsulationFactory(object):
    # TODO: Not implemented yet

    def parse(self, data, message_info):
        raise NotImplementedError("TODO: Not implemented yet")


# JoinerUdpPort TLV (18)
class JoinerUdpPort(object):

    def __init__(self, udp_port):
        self._udp_port = udp_port

    @property
    def udp_port(self):
        return self._udp_port

    def __eq__(self, other):
        return (isinstance(self, type(other)) and self.udp_port == other.udp_port)

    def __repr__(self):
        return "JoinerUdpPort(udp_port={})".format(self.udp_port)


class JoinerUdpPortFactory(object):

    def parse(self, data, message_info):
        udp_port = struct.unpack(">H", data.read(2))[0]
        return JoinerUdpPort(udp_port)


# JoinerIID TLV (19)
class JoinerIID(object):
    # TODO: Not implemented yet
    pass


class JoinerIIDFactory(object):
    # TODO: Not implemented yet

    def parse(self, data, message_info):
        raise NotImplementedError("TODO: Not implemented yet")


# JoinerRouterLocator TLV (20)
class JoinerRouterLocator(object):
    # TODO: Not implemented yet
    pass


class JoinerRouterLocatorFactory(object):
    # TODO: Not implemented yet

    def parse(self, data, message_info):
        raise NotImplementedError("TODO: Not implemented yet")


# JoinerRouterKEK TLV (21)
class JoinerRouterKEK(object):
    # TODO: Not implemented yet
    pass


class JoinerRouterKEKFactory(object):
    # TODO: Not implemented yet

    def parse(self, data, message_info):
        raise NotImplementedError("TODO: Not implemented yet")


# ProvisioningURL TLV (32)
class ProvisioningUrl(object):

    def __init__(self, url):
        self._url = url

    @property
    def url(self):
        return self._url

    def __repr__(self):
        return "ProvisioningUrl(url={})".format(self.url)


class ProvisioningUrlFactory:

    def parse(self, data, message_info):
        url = data.getvalue().decode('utf-8')
        return ProvisioningUrl(url)


# VendorName TLV (33)
class VendorName(object):

    def __init__(self, vendor_name):
        self._vendor_name = vendor_name

    @property
    def vendor_name(self):
        return self._vendor_name

    def __eq__(self, other):
        return self.vendor_name == other.vendor_name

    def __repr__(self):
        return "VendorName(vendor_name={})".format(self.vendor_name)


class VendorNameFactory:

    def parse(self, data, message_info):
        vendor_name = data.getvalue().decode('utf-8')
        return VendorName(vendor_name)


# VendorModel TLV (34)
class VendorModel(object):

    def __init__(self, vendor_model):
        self._vendor_model = vendor_model

    @property
    def vendor_model(self):
        return self._vendor_model

    def __eq__(self, other):
        return self.vendor_model == other.vendor_model

    def __repr__(self):
        return "VendorModel(vendor_model={})".format(self.vendor_model)


class VendorModelFactory:

    def parse(self, data, message_info):
        vendor_model = data.getvalue().decode('utf-8')
        return VendorModel(vendor_model)


# VendorSWVersion TLV (35)
class VendorSWVersion(object):

    def __init__(self, vendor_sw_version):
        self._vendor_sw_version = vendor_sw_version

    @property
    def vendor_sw_version(self):
        return self._vendor_sw_version

    def __eq__(self, other):
        return self.vendor_sw_version == other.vendor_sw_version

    def __repr__(self):
        return "VendorName(vendor_sw_version={})".format(self.vendor_sw_version)


class VendorSWVersionFactory:

    def parse(self, data, message_info):
        vendor_sw_version = data.getvalue()
        return VendorSWVersion(vendor_sw_version)


# VendorData TLV (36)
class VendorData(object):

    def __init__(self, data):
        self._vendor_data = data

    @property
    def vendor_data(self):
        return self._vendor_data

    def __repr__(self):
        return "Vendor(url={})".format(self.vendor_data)


class VendorDataFactory(object):

    def parse(self, data, message_info):
        return VendorData(data)


# VendorStackVersion TLV (37)
class VendorStackVersion(object):

    def __init__(self, stack_vendor_oui, build, rev, minor, major):
        self._stack_vendor_oui = stack_vendor_oui
        self._build = build
        self._rev = rev
        self._minor = minor
        self._major = major
        return

    @property
    def stack_vendor_oui(self):
        return self._stack_vendor_oui

    @property
    def build(self):
        return self._build

    @property
    def rev(self):
        return self._rev

    @property
    def minor(self):
        return self._minor

    @property
    def major(self):
        return self._major

    def __repr__(self):
        return "VendorStackVersion(vendor_stack_version={}, build={}, rev={}, minor={}, major={})".format(
            self.stack_vendor_oui, self.build, self.rev, self.minor, self.major)


class VendorStackVersionFactory:

    def parse(self, data, message_info):
        stack_vendor_oui = struct.unpack(">H", data.read(2))[0]
        rest = struct.unpack(">BBBB", data.read(4))
        build = rest[1] << 4 | (0xf0 & rest[2])
        rev = 0xF & rest[2]
        minor = rest[3] & 0xf0
        major = rest[3] & 0xF
        return VendorStackVersion(stack_vendor_oui, build, rev, minor, major)


# UdpEncapsulation TLV (48)
class UdpEncapsulation(object):
    # TODO: Not implemented yet
    pass


class UdpEncapsulationFactory(object):
    # TODO: Not implemented yet

    def parse(self, data, message_info):
        raise NotImplementedError("TODO: Not implemented yet")


# Ipv6Address TLV (49)
class Ipv6Address(object):
    # TODO: Not implemented yet
    pass


class Ipv6AddressFactory(object):
    # TODO: Not implemented yet

    def parse(self, data, message_info):
        raise NotImplementedError("TODO: Not implemented yet")


# PendingTimestamp TLV (51)
class PendingTimestamp(object):
    # TODO: Not implemented yet
    pass


class PendingTimestampFactory(object):
    # TODO: Not implemented yet

    def parse(self, data, message_info):
        raise NotImplementedError("TODO: Not implemented yet")


# DelayTimer TLV (52)
class DelayTimer(object):
    # TODO: Not implemented yet
    pass


class DelayTimerFactory(object):
    # TODO: Not implemented yet

    def parse(self, data, message_info):
        raise NotImplementedError("TODO: Not implemented yet")


# ChannelMask TLV (53)
class ChannelMask(object):
    # TODO: Not implemented yet
    pass


class ChannelMaskFactory(object):
    # TODO: Not implemented yet

    def parse(self, data, message_info):
        raise NotImplementedError("TODO: Not implemented yet")


# Count TLV (54)
class Count(object):
    # TODO: Not implemented yet
    pass


class CountFactory(object):
    # TODO: Not implemented yet

    def parse(self, data, message_info):
        raise NotImplementedError("TODO: Not implemented yet")


# Period TLV (55)
class Period(object):
    # TODO: Not implemented yet
    pass


class PeriodFactory(object):
    # TODO: Not implemented yet

    def parse(self, data, message_info):
        raise NotImplementedError("TODO: Not implemented yet")


# ScanDuration TLV (56)
class ScanDuration(object):
    # TODO: Not implemented yet
    pass


class ScanDurationFactory(object):
    # TODO: Not implemented yet

    def parse(self, data, message_info):
        raise NotImplementedError("TODO: Not implemented yet")


# EnergyList TLV (57)
class EnergyList(object):
    # TODO: Not implemented yet
    pass


class EnergyListFactory(object):
    # TODO: Not implemented yet

    def parse(self, data, message_info):
        raise NotImplementedError("TODO: Not implemented yet")


# Discovery Request TLV (128)
class DiscoveryRequest(object):

    def __init__(self, version, joiner_flag):
        self._version = version
        self._joiner_flag = joiner_flag

    @property
    def version(self):
        return self._version

    @property
    def joiner_flag(self):
        return self._joiner_flag

    def __eq__(self, other):
        return (isinstance(self, type(other)) and self.version == other.version and
                self.joiner_flag == other.joiner_flag)

    def __repr__(self):
        return "DiscoveryRequest(version={}, joiner_flag={})".format(self.version, self.joiner_flag)


class DiscoveryRequestFactory(object):

    def parse(self, data, message_info):
        data_byte = struct.unpack(">B", data.read(1))[0]
        version = (data_byte & 0xf0) >> 4
        joiner_flag = (data_byte & 0x08) >> 3

        return DiscoveryRequest(version, joiner_flag)


# Discovery Response TLV (128)
class DiscoveryResponse(object):

    def __init__(self, version, native_flag):
        self._version = version
        self._native_flag = native_flag

    @property
    def version(self):
        return self._version

    @property
    def native_flag(self):
        return self._native_flag

    def __eq__(self, other):
        return (isinstance(self, type(other)) and self.version == other.version and
                self.native_flag == other.native_flag)

    def __repr__(self):
        return "DiscoveryResponse(version={}, native_flag={})".format(self.version, self.native_flag)


class DiscoveryResponseFactory(object):

    def parse(self, data, message_info):
        data_byte = struct.unpack(">B", data.read(1))[0]
        version = (data_byte & 0xf0) >> 4
        native_flag = (data_byte & 0x08) >> 3

        return DiscoveryResponse(version, native_flag)


class MeshCopCommand(object):

    def __init__(self, _type, tlvs):
        self._type = _type
        self._tlvs = tlvs

    @property
    def type(self):
        return self._type

    @property
    def tlvs(self):
        return self._tlvs

    def __repr__(self):
        tlvs_str = ", ".join(["{}".format(tlv) for tlv in self.tlvs])
        return "MeshCopCommand(type={}, tlvs=[{}])".format(self.type, tlvs_str)


def create_deault_mesh_cop_msg_type_map():
    return {
        'JOIN_FIN.req': MeshCopMessageType.JOIN_FIN_REQ,
        'JOIN_FIN.rsp': MeshCopMessageType.JOIN_FIN_RSP,
        'JOIN_ENT.ntf': MeshCopMessageType.JOIN_ENT_NTF,
        'JOIN_ENT.rsp': MeshCopMessageType.JOIN_ENT_RSP,
    }


class MeshCopCommandFactory:

    def __init__(self, tlvs_factories):
        self._tlvs_factories = tlvs_factories
        self._mesh_cop_msg_type_map = create_deault_mesh_cop_msg_type_map()

    def _get_length(self, data):
        return ord(data.read(1))

    def _get_tlv_factory(self, _type):
        try:
            return self._tlvs_factories[_type]
        except KeyError:
            logging.error('Could not find TLV factory. Unsupported TLV type: {}'.format(_type))
            return UnknownTlvFactory(_type)

    def _parse_tlv(self, data):
        _type = TlvType(ord(data.read(1)))
        length = self._get_length(data)
        value = data.read(length)
        factory = self._get_tlv_factory(_type)
        return factory.parse(io.BytesIO(value), None)  # message_info not needed here

    def _get_mesh_cop_msg_type(self, msg_type_str):
        try:
            return self._mesh_cop_msg_type_map[msg_type_str]
        except KeyError:
            raise KeyError('Mesh cop message type not found: {}'.format(msg_type_str))

    def parse(self, cmd_type_str, data):
        cmd_type = self._get_mesh_cop_msg_type(cmd_type_str)

        tlvs = []
        while data.tell() < len(data.getvalue()):
            tlv = self._parse_tlv(data)
            tlvs.append(tlv)

        return MeshCopCommand(cmd_type, tlvs)


def create_default_mesh_cop_tlv_factories():
    return {
        TlvType.STATE: StateFactory(),
        TlvType.PROVISIONING_URL: ProvisioningUrlFactory(),
        TlvType.VENDOR_NAME: VendorNameFactory(),
        TlvType.VENDOR_MODEL: VendorModelFactory(),
        TlvType.VENDOR_SW_VERSION: VendorSWVersionFactory(),
        TlvType.VENDOR_DATA: VendorDataFactory(),
        TlvType.VENDOR_STACK_VERSION: VendorStackVersionFactory(),
    }


class ThreadDiscoveryTlvsFactory(SubTlvsFactory):

    def __init__(self, sub_tlvs_factories):
        super(ThreadDiscoveryTlvsFactory, self).__init__(sub_tlvs_factories)
