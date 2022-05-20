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
import logging
import struct

from binascii import hexlify

import common

from enum import IntEnum
from tlvs_parsing import UnknownTlvFactory


class CommandType(IntEnum):
    LINK_REQUEST = 0
    LINK_ACCEPT = 1
    LINK_ACCEPT_AND_REQUEST = 2
    LINK_REJECT = 3
    ADVERTISEMENT = 4
    UPDATE = 5
    UPDATE_REQUEST = 6
    DATA_REQUEST = 7
    DATA_RESPONSE = 8
    PARENT_REQUEST = 9
    PARENT_RESPONSE = 10
    CHILD_ID_REQUEST = 11
    CHILD_ID_RESPONSE = 12
    CHILD_UPDATE_REQUEST = 13
    CHILD_UPDATE_RESPONSE = 14
    ANNOUNCE = 15
    DISCOVERY_REQUEST = 16
    DISCOVERY_RESPONSE = 17
    LINK_METRICS_MANAGEMENT_REQUEST = 18
    LINK_METRICS_MANAGEMENT_RESPONSE = 19
    LINK_PROBE = 20
    TIME_SYNC = 99


class TlvType(IntEnum):
    SOURCE_ADDRESS = 0
    MODE = 1
    TIMEOUT = 2
    CHALLENGE = 3
    RESPONSE = 4
    LINK_LAYER_FRAME_COUNTER = 5
    MLE_FRAME_COUNTER = 8
    ROUTE64 = 9
    ADDRESS16 = 10
    LEADER_DATA = 11
    NETWORK_DATA = 12
    TLV_REQUEST = 13
    SCAN_MASK = 14
    CONNECTIVITY = 15
    LINK_MARGIN = 16
    STATUS = 17
    VERSION = 18
    ADDRESS_REGISTRATION = 19
    CHANNEL = 20
    PANID = 21
    ACTIVE_TIMESTAMP = 22
    PENDING_TIMESTAMP = 23
    ACTIVE_OPERATIONAL_DATASET = 24
    PENDING_OPERATIONAL_DATASET = 25
    THREAD_DISCOVERY = 26
    CSL_CHANNEL = 80
    CSL_SYNCHRONIZED_TIMEOUT = 85
    CSL_CLOCK_ACCURACY = 86
    LINK_METRICS_QUERY = 87
    LINK_METRICS_MANAGEMENT = 88
    LINK_METRICS_REPORT = 89
    LINK_PROBE = 90
    TIME_REQUEST = 252
    TIME_PARAMETER = 253


class LinkMetricsSubTlvType(IntEnum):
    LINK_METRICS_REPORT = 0
    LINK_METRICS_QUERY_ID = 1
    LINK_METRICS_QUERY_OPTIONS = 2
    FORWARD_PROBING_REGISTRATION = 3
    LINK_METRICS_STATUS = 5
    ENHANCED_ACK_LINK_METRICS_CONFIGURATION = 7


class SourceAddress(object):

    def __init__(self, address):
        self._address = address

    @property
    def address(self):
        return self._address

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return self.address == other.address

    def __repr__(self):
        return "SourceAddress(address={})".format(hex(self._address))


class SourceAddressFactory:

    def parse(self, data, message_info):
        address = struct.unpack(">H", data.read(2))[0]
        return SourceAddress(address)


class Mode(object):

    def __init__(self, receiver, secure, device_type, network_data):
        self._receiver = receiver
        self._secure = secure
        self._device_type = device_type
        self._network_data = network_data

    @property
    def receiver(self):
        return self._receiver

    @property
    def secure(self):
        return self._secure

    @property
    def device_type(self):
        return self._device_type

    @property
    def network_data(self):
        return self._network_data

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return (self.receiver == other.receiver and self.secure == other.secure and
                self.device_type == other.device_type and self.network_data == other.network_data)

    def __repr__(self):
        return "Mode(receiver={}, secure={}, device_type={}, network_data={})".format(
            self.receiver, self.secure, self.device_type, self.network_data)


class ModeFactory:

    def parse(self, data, message_info):
        mode = ord(data.read(1))
        receiver = (mode >> 3) & 0x01
        secure = (mode >> 2) & 0x01
        device_type = (mode >> 1) & 0x01
        network_data = (mode >> 0) & 0x01
        return Mode(receiver, secure, device_type, network_data)


class Timeout(object):

    def __init__(self, timeout):
        self._timeout = timeout

    @property
    def timeout(self):
        return self._timeout

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return self.timeout == other.timeout

    def __repr__(self):
        return "Timeout(timeout={})".format(self.timeout)


class TimeoutFactory:

    def parse(self, data, message_info):
        timeout = struct.unpack(">I", data.read(4))[0]
        return Timeout(timeout)


class Challenge(object):

    def __init__(self, challenge):
        self._challenge = challenge

    @property
    def challenge(self):
        return self._challenge

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return self.challenge == other.challenge

    def __repr__(self):
        return "Challenge(challenge={})".format(hexlify(self.challenge))


class ChallengeFactory:

    def parse(self, data, message_info):
        challenge = data.read()
        return Challenge(challenge)


class Response(object):

    def __init__(self, response):
        self._response = response

    @property
    def response(self):
        return self._response

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return self.response == other.response

    def __repr__(self):
        return "Response(response={})".format(hexlify(self.response))


class ResponseFactory:

    def parse(self, data, message_info):
        response = data.read()
        return Response(response)


class LinkLayerFrameCounter(object):

    def __init__(self, frame_counter):
        self._frame_counter = frame_counter

    @property
    def frame_counter(self):
        return self._frame_counter

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return self.frame_counter == other.frame_counter

    def __repr__(self):
        return "LinkLayerFrameCounter(frame_counter={})".format(self.frame_counter)


class LinkLayerFrameCounterFactory:

    def parse(self, data, message_info):
        frame_counter = struct.unpack(">I", data.read(4))[0]
        return LinkLayerFrameCounter(frame_counter)


class MleFrameCounter(object):

    def __init__(self, frame_counter):
        self._frame_counter = frame_counter

    @property
    def frame_counter(self):
        return self._frame_counter

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return self.frame_counter == other.frame_counter

    def __repr__(self):
        return "MleFrameCounter(frame_counter={})".format(self.frame_counter)


class MleFrameCounterFactory:

    def parse(self, data, message_info):
        frame_counter = struct.unpack(">I", data.read(4))[0]
        return MleFrameCounter(frame_counter)


class LinkQualityAndRouteData(object):

    def __init__(self, output, _input, route):
        self._output = output
        self._input = _input
        self._route = route

    @property
    def output(self):
        return self._output

    @property
    def input(self):
        return self._input

    @property
    def route(self):
        return self._route

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return (self.output == other.output and self.input == other.input and self.route == other.route)

    def __repr__(self):
        return "LinkQualityAndRouteData(ouput={}, input={}, route={})".format(self.output, self.input, self.route)


class LinkQualityAndRouteDataFactory:

    def parse(self, data, message_info):
        lqrd = ord(data.read(1))
        output = (lqrd >> 6) & 0x3
        _input = (lqrd >> 4) & 0x3
        route = lqrd & 0x0F
        return LinkQualityAndRouteData(output, _input, route)


class Route64(object):

    def __init__(self, id_sequence, router_id_mask, link_quality_and_route_data):
        self._id_sequence = id_sequence
        self._router_id_mask = router_id_mask
        self._link_quality_and_route_data = link_quality_and_route_data

    @property
    def id_sequence(self):
        return self._id_sequence

    @property
    def router_id_mask(self):
        return self._router_id_mask

    @property
    def link_quality_and_route_data(self):
        return self._link_quality_and_route_data

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return (self.id_sequence == other.id_sequence and self.router_id_mask == other.router_id_mask and
                self.link_quality_and_route_data == other.link_quality_and_route_data)

    def __repr__(self):
        lqrd_str = ", ".join(["{}".format(lqrd) for lqrd in self.link_quality_and_route_data])
        return "Route64(id_sequence={}, router_id_mask={}, link_quality_and_route_data=[{}])".format(
            self.id_sequence, hex(self.router_id_mask), lqrd_str)


class Route64Factory:

    def __init__(self, link_quality_and_route_data_factory):
        self._lqrd_factory = link_quality_and_route_data_factory

    def parse(self, data, message_info):
        id_sequence = ord(data.read(1))
        router_id_mask = struct.unpack(">Q", data.read(8))[0]

        link_quality_and_route_data = []

        while data.tell() < len(data.getvalue()):
            link_quality_and_route_data.append(self._lqrd_factory.parse(data, message_info))

        return Route64(id_sequence, router_id_mask, link_quality_and_route_data)


class Address16(object):

    def __init__(self, address):
        self._address = address

    @property
    def address(self):
        return self._address

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return self.address == other.address

    def __repr__(self):
        return "Address16(address={})".format(hex(self.address))


class Address16Factory:

    def parse(self, data, message_info):
        address = struct.unpack(">H", data.read(2))[0]
        return Address16(address)


class LeaderData(object):

    def __init__(
        self,
        partition_id,
        weighting,
        data_version,
        stable_data_version,
        leader_router_id,
    ):
        self._partition_id = partition_id
        self._weighting = weighting
        self._data_version = data_version
        self._stable_data_version = stable_data_version
        self._leader_router_id = leader_router_id

    @property
    def partition_id(self):
        return self._partition_id

    @property
    def weighting(self):
        return self._weighting

    @property
    def data_version(self):
        return self._data_version

    @property
    def stable_data_version(self):
        return self._stable_data_version

    @property
    def leader_router_id(self):
        return self._leader_router_id

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return (self.partition_id == other.partition_id and self.weighting == other.weighting and
                self.data_version == other.data_version and self.stable_data_version == other.stable_data_version and
                self.leader_router_id == other.leader_router_id)

    def __repr__(self):
        return 'LeaderData(partition_id={}, weighting={}, data_version={}, stable_data_version={},leader_router_id={}'.format(
            self.partition_id,
            self.weighting,
            self.data_version,
            self.stable_data_version,
            self.leader_router_id,
        )


class LeaderDataFactory:

    def parse(self, data, message_info):
        partition_id = struct.unpack(">I", data.read(4))[0]
        weighting = ord(data.read(1))
        data_version = ord(data.read(1))
        stable_data_version = ord(data.read(1))
        leader_router_id = ord(data.read(1))
        return LeaderData(
            partition_id,
            weighting,
            data_version,
            stable_data_version,
            leader_router_id,
        )


class NetworkData(object):

    def __init__(self, tlvs):
        self._tlvs = tlvs

    @property
    def tlvs(self):
        return self._tlvs

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return self.tlvs == other.tlvs

    def __repr__(self):
        tlvs_str = ", ".join(["{}".format(tlv) for tlv in self.tlvs])
        return "NetworkData(tlvs=[{}])".format(tlvs_str)


class NetworkDataFactory:

    def __init__(self, network_data_tlvs_factory):
        self._tlvs_factory = network_data_tlvs_factory

    def parse(self, data, message_info):
        tlvs = self._tlvs_factory.parse(data, message_info)
        return NetworkData(tlvs)


class TlvRequest(object):

    def __init__(self, tlvs):
        self._tlvs = tlvs

    @property
    def tlvs(self):
        return self._tlvs

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return self.tlvs == other.tlvs

    def __repr__(self):
        tlvs_str = ", ".join(["{}".format(tlv) for tlv in self.tlvs])
        return "TlvRequest(tlvs=[{}])".format(tlvs_str)


class TlvRequestFactory:

    def parse(self, data, message_info):
        tlvs = [b for b in bytearray(data.read())]
        return TlvRequest(tlvs)


class ScanMask(object):

    def __init__(self, router, end_device):
        self._router = router
        self._end_device = end_device

    @property
    def router(self):
        return self._router

    @property
    def end_device(self):
        return self._end_device

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return (self.router == other.router and self.end_device == other.end_device)

    def __repr__(self):
        return "ScanMask(router={}, end_device={})".format(self.router, self.end_device)


class ScanMaskFactory:

    def parse(self, data, message_info):
        scan_mask = ord(data.read(1))
        router = (scan_mask >> 7) & 0x01
        end_device = (scan_mask >> 6) & 0x01
        return ScanMask(router, end_device)


class Connectivity(object):

    def __init__(
        self,
        pp_byte,
        link_quality_3,
        link_quality_2,
        link_quality_1,
        leader_cost,
        id_sequence,
        active_routers,
        sed_buffer_size=None,
        sed_datagram_count=None,
    ):
        self._pp_byte = pp_byte
        self._link_quality_3 = link_quality_3
        self._link_quality_2 = link_quality_2
        self._link_quality_1 = link_quality_1
        self._leader_cost = leader_cost
        self._id_sequence = id_sequence
        self._active_routers = active_routers
        self._sed_buffer_size = sed_buffer_size
        self._sed_datagram_count = sed_datagram_count

    @property
    def pp_byte(self):
        return self._pp_byte

    @property
    def pp(self):
        return common.map_pp(self._pp_byte)

    @property
    def link_quality_3(self):
        return self._link_quality_3

    @property
    def link_quality_2(self):
        return self._link_quality_2

    @property
    def link_quality_1(self):
        return self._link_quality_1

    @property
    def leader_cost(self):
        return self._leader_cost

    @property
    def id_sequence(self):
        return self._id_sequence

    @property
    def active_routers(self):
        return self._active_routers

    @property
    def sed_buffer_size(self):
        return self._sed_buffer_size

    @property
    def sed_datagram_count(self):
        return self._sed_datagram_count

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return (self.pp == other.pp and self.link_quality_3 == other.link_quality_3 and
                self.link_quality_2 == other.link_quality_2 and self.link_quality_1 == other.link_quality_1 and
                self.leader_cost == other.leader_cost and self.id_sequence == other.id_sequence and
                self.active_routers == other.active_routers and self.sed_buffer_size == other.sed_buffer_size and
                self.sed_datagram_count == other.sed_datagram_count)

    def __repr__(self):
        return r"Connectivity(pp={}, \
                 link_quality_3={}, \
                 link_quality_2={}, \
                 link_quality_1={}, \
                 leader_cost={}, \
                 id_sequence={}, \
                 active_routers={}, \
                 sed_buffer_size={}, \
                 sed_datagram_count={})".format(
            self.pp,
            self.link_quality_3,
            self.link_quality_2,
            self.link_quality_1,
            self.leader_cost,
            self.id_sequence,
            self.active_routers,
            self.sed_buffer_size,
            self.sed_datagram_count,
        )


class ConnectivityFactory:

    def parse(self, data, message_info):
        pp_byte = ord(data.read(1))
        link_quality_3 = ord(data.read(1))
        link_quality_2 = ord(data.read(1))
        link_quality_1 = ord(data.read(1))
        leader_cost = ord(data.read(1))
        id_sequence = ord(data.read(1))
        active_routers = ord(data.read(1))

        sed_data = io.BytesIO(data.read(3))

        if len(sed_data.getvalue()) > 0:
            sed_buffer_size = struct.unpack(">H", sed_data.read(2))[0]
            sed_datagram_count = ord(sed_data.read(1))
        else:
            sed_buffer_size = None
            sed_datagram_count = None

        return Connectivity(
            pp_byte,
            link_quality_3,
            link_quality_2,
            link_quality_1,
            leader_cost,
            id_sequence,
            active_routers,
            sed_buffer_size,
            sed_datagram_count,
        )


class LinkMargin(object):

    def __init__(self, link_margin):
        self._link_margin = link_margin

    @property
    def link_margin(self):
        return self._link_margin

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return self.link_margin == other.link_margin

    def __repr__(self):
        return "LinkMargin(link_margin={})".format(self.link_margin)


class LinkMarginFactory:

    def parse(self, data, message_info):
        link_margin = ord(data.read(1))
        return LinkMargin(link_margin)


class Status(object):

    def __init__(self, status):
        self._status = status

    @property
    def status(self):
        return self._status

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return self.status == other.status

    def __repr__(self):
        return "Status(status={})".format(self.status)


class StatusFactory:

    def parse(self, data, message_info):
        status = ord(data.read(1))
        return Status(status)


class Version(object):

    def __init__(self, version):
        self._version = version

    @property
    def version(self):
        return self._version

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return self.version == other.version

    def __repr__(self):
        return "Version(version={})".format(self.version)


class VersionFactory:

    def parse(self, data, message_info):
        version = struct.unpack(">H", data.read(2))[0]
        return Version(version)


class AddressFull(object):

    def __init__(self, ipv6_address):
        self._ipv6_address = ipv6_address

    @property
    def ipv6_address(self):
        return self._ipv6_address

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return self.ipv6_address == other.ipv6_address

    def __repr__(self):
        return "AddressFull(ipv6_address={}')".format(hexlify(self.ipv6_address))


class AddressFullFactory:

    def parse(self, data, message_info):
        data.read(1)  # first byte is ignored
        ipv6_address = data.read(16)
        return AddressFull(ipv6_address)


class AddressCompressed(object):

    def __init__(self, cid, iid):
        self._cid = cid
        self._iid = iid

    @property
    def cid(self):
        return self._cid

    @property
    def iid(self):
        return self._iid

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return self.cid == other.cid and self.iid == other.iid

    def __repr__(self):
        return "AddressCompressed(cid={}, iid={}')".format(self.cid, hexlify(self.iid))


class AddressCompressedFactory:

    def parse(self, data, message_info):
        cid = ord(data.read(1)) & 0x0F
        iid = bytearray(data.read(8))
        return AddressCompressed(cid, iid)


class AddressRegistration(object):

    def __init__(self, addresses):
        self._addresses = addresses

    @property
    def addresses(self):
        return self._addresses

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return self.addresses == other.addresses

    def __repr__(self):
        addresses_str = ", ".join(["{}".format(address) for address in self.addresses])
        return "AddressRegistration(addresses=[{}])".format(addresses_str)


class AddressRegistrationFactory:

    def __init__(self, addr_compressed_factory, addr_full_factory):
        self._addr_compressed_factory = addr_compressed_factory
        self._addr_full_factory = addr_full_factory

    def parse(self, data, message_info):
        addresses = []

        while data.tell() < len(data.getvalue()):
            compressed = (ord(data.read(1)) >> 7) & 0x01
            data.seek(-1, io.SEEK_CUR)

            if compressed:
                addresses.append(self._addr_compressed_factory.parse(data, message_info))
            else:
                addresses.append(self._addr_full_factory.parse(data, message_info))

        return AddressRegistration(addresses)


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

        return (self.channel_page == other.channel_page and self.channel == other.channel)

    def __repr__(self):
        return "Channel(channel_page={}, channel={})".format(self.channel_page, self.channel)


class ChannelFactory:

    def parse(self, data, message_info):
        channel_page = ord(data.read(1))
        channel = struct.unpack(">H", data.read(2))[0]
        return Channel(channel_page, channel)


class PanId:

    def __init__(self, pan_id):
        self._pan_id = pan_id

    @property
    def pan_id(self):
        return self._pan_id

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return self.pan_id == other.pan_id

    def __repr__(self):
        return "PanId(pan_id={})".format(self.pan_id)


class PanIdFactory:

    def parse(self, data, message_info):
        pan_id = struct.unpack(">H", data.read(2))[0]
        return PanId(pan_id)


class ActiveTimestamp(object):

    def __init__(self, timestamp_seconds, timestamp_ticks, u):
        self._timestamp_seconds = timestamp_seconds
        self._timestamp_ticks = timestamp_ticks
        self._u = u

    @property
    def timestamp_seconds(self):
        return self._timestamp_seconds

    @property
    def timestamp_ticks(self):
        return self._timestamp_ticks

    @property
    def u(self):
        return self._u

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return (self.timestamp_seconds == other.timestamp_seconds and self.timestamp_ticks == other.timestamp_ticks and
                self.u == other.u)

    def __repr__(self):
        return "ActiveTimestamp(timestamp_seconds={}, timestamp_ticks={}, u={})".format(
            self.timestamp_seconds, self.timestamp_ticks, self.u)


class ActiveTimestampFactory:

    def parse(self, data, message_info):
        seconds = bytearray([0x00, 0x00]) + bytearray(data.read(6))
        ticks = struct.unpack(">H", data.read(2))[0]

        timestamp_seconds = struct.unpack(">Q", bytes(seconds))[0]
        timestamp_ticks = ticks >> 1
        u = ticks & 0x01
        return ActiveTimestamp(timestamp_seconds, timestamp_ticks, u)


class PendingTimestamp(object):

    def __init__(self, timestamp_seconds, timestamp_ticks, u):
        self._timestamp_seconds = timestamp_seconds
        self._timestamp_ticks = timestamp_ticks
        self._u = u

    @property
    def timestamp_seconds(self):
        return self._timestamp_seconds

    @property
    def timestamp_ticks(self):
        return self._timestamp_ticks

    @property
    def u(self):
        return self._u

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return (self.timestamp_seconds == other.timestamp_seconds and self.timestamp_ticks == other.timestamp_ticks and
                self.u == other.u)

    def __repr__(self):
        return "PendingTimestamp(timestamp_seconds={}, timestamp_ticks={}, u={})".format(
            self.timestamp_seconds, self.timestamp_ticks, self.u)


class PendingTimestampFactory:

    def parse(self, data, message_info):
        seconds = bytearray([0x00, 0x00]) + bytearray(data.read(6))
        ticks = struct.unpack(">H", data.read(2))[0]

        timestamp_seconds = struct.unpack(">Q", bytes(seconds))[0]
        timestamp_ticks = ticks >> 1
        u = ticks & 0x01
        return PendingTimestamp(timestamp_seconds, timestamp_ticks, u)


class ActiveOperationalDataset:
    # TODO: Not implemented yet

    def __init__(self):
        print("ActiveOperationalDataset is not implemented yet.")


class ActiveOperationalDatasetFactory:

    def parse(self, data, message_info):
        return ActiveOperationalDataset()


class PendingOperationalDataset:
    # TODO: Not implemented yet

    def __init__(self):
        print("PendingOperationalDataset is not implemented yet.")


class PendingOperationalDatasetFactory:

    def parse(self, data, message_info):
        return PendingOperationalDataset()


class ThreadDiscovery(object):

    def __init__(self, tlvs):
        self._tlvs = tlvs

    @property
    def tlvs(self):
        return self._tlvs

    def __eq__(self, other):
        return self.tlvs == other.tlvs

    def __repr__(self):
        return "ThreadDiscovery(tlvs={})".format(self.tlvs)


class ThreadDiscoveryFactory:

    def __init__(self, thread_discovery_tlvs_factory):
        self._tlvs_factory = thread_discovery_tlvs_factory

    def parse(self, data, message_info):
        tlvs = self._tlvs_factory.parse(data, message_info)
        return ThreadDiscovery(tlvs)


class CslChannel:
    # TODO: Not implemented yet

    def __init__(self):
        print("CslChannel is not implemented yet.")


class CslChannelFactory:
    # TODO: Not implemented yet

    def parse(self, data, message_info):
        return CslChannel()


class CslSynchronizedTimeout:
    # TODO: Not implemented yet

    def __init__(self):
        print("CslSynchronizedTimeout is not implemented yet.")


class CslSynchronizedTimeoutFactory:

    def parse(self, data, message_info):
        return CslSynchronizedTimeout()


class CslClockAccuracy:
    # TODO: Not implemented yet

    def __init__(self):
        print("CslClockAccuracy is not implemented yet.")


class CslClockAccuracyFactory:

    def parse(self, data, message_info):
        return CslClockAccuracy()


class TimeRequest:
    # TODO: Not implemented yet

    def __init__(self):
        print("TimeRequest is not implemented yet.")


class TimeRequestFactory:

    def parse(self, data, message_info):
        return TimeRequest()


class TimeParameter:
    # TODO: Not implemented yet

    def __init__(self):
        print("TimeParameter is not implemented yet.")


class TimeParameterFactory:

    def parse(self, data, message_info):
        return TimeParameter()


class LinkMetricsQuery:
    # TODO: Not implemented yet

    def __init__(self):
        print("LinkMetricsQuery is not implemented yet.")


class LinkMetricsQueryFactory:

    def parse(self, data, message_info):
        return LinkMetricsQuery()


class LinkMetricsManagement:
    # TODO: Not implemented yet

    def __init__(self):
        print("LinkMetricsManagement is not implemented yet.")


class LinkMetricsManagementFactory:

    def parse(self, data, message_info):
        return LinkMetricsManagement()


class LinkMetricsReport:
    # TODO: Not implemented yet

    def __init__(self):
        print("LinkMetricsReport is not implemented yet.")


class LinkMetricsReportFactory:

    def parse(self, data, message_info):
        return LinkMetricsReport()


class LinkProbe:
    # TODO: Not implemented yet

    def __init__(self):
        print("LinkProbe is not implemented yet.")


class LinkProbeFactory:

    def parse(self, data, message_info):
        return LinkProbe()


class MleCommand(object):

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
        return "MleCommand(type={}, tlvs=[{}])".format(self.type.name, tlvs_str)


class MleCommandFactory:

    _MARKER_EXTENDED_LENGTH = 0xff

    def __init__(self, tlvs_factories):
        self._tlvs_factories = tlvs_factories

    def _get_length(self, data):
        length = ord(data.read(1))

        if length == self._MARKER_EXTENDED_LENGTH:
            length = struct.unpack(">H", data.read(2))[0]

        return length

    def _get_tlv_factory(self, _type):
        try:
            return self._tlvs_factories[_type]
        except KeyError:
            logging.error('Could not find TLV factory. Unsupported TLV type: {}'.format(_type))
            return UnknownTlvFactory(_type)

    def _parse_tlv(self, data, message_info):
        _type = TlvType(ord(data.read(1)))
        length = self._get_length(data)
        value = data.read(length)

        factory = self._get_tlv_factory(_type)

        return factory.parse(io.BytesIO(value), message_info)

    def parse(self, data, message_info):
        cmd_type = CommandType(ord(data.read(1)))
        tlvs = []

        while data.tell() < len(data.getvalue()):
            tlv = self._parse_tlv(data, message_info)
            tlvs.append(tlv)

        return MleCommand(cmd_type, tlvs)


class MleMessage(object):

    def __init__(self, command):
        self._command = command

    @property
    def command(self):
        return self._command

    def __repr__(self):
        return "MleMessage(command={})".format(self.command)


class MleMessageSecured(MleMessage):

    def __init__(self, aux_sec_hdr, command, mic):
        super(MleMessageSecured, self).__init__(command)
        self._aux_sec_hdr = aux_sec_hdr
        self._mic = mic

    @property
    def aux_sec_hdr(self):
        return self._aux_sec_hdr

    @property
    def mic(self):
        return self._mic

    def __repr__(self):
        return "MleMessageSecured(aux_sec_hdr={}, command={}, mic=\"{}\")".format(self.aux_sec_hdr, self.command,
                                                                                  hexlify(self.mic))


class MleMessageFactory:

    def __init__(self, aux_sec_hdr_factory, mle_command_factory, crypto_engine):
        self._aux_sec_hdr_factory = aux_sec_hdr_factory
        self._mle_command_factory = mle_command_factory
        self._crypto_engine = crypto_engine

    def _create_mle_secured_message(self, data, message_info):
        aux_sec_hdr = self._aux_sec_hdr_factory.parse(data, message_info)

        enc_data_length = len(data.getvalue())

        enc_data = bytearray(data.read(enc_data_length - data.tell() - self._crypto_engine.mic_length))
        mic = bytearray(data.read())

        dec_data = self._crypto_engine.decrypt(enc_data, mic, message_info)

        command = self._mle_command_factory.parse(io.BytesIO(dec_data), message_info)

        return MleMessageSecured(aux_sec_hdr, command, mic)

    def _create_mle_message(self, data, message_info):
        command = self._mle_command_factory.parse(data, message_info)

        return MleMessage(command)

    def parse(self, data, message_info):
        security_indicator = ord(data.read(1))

        if security_indicator == 0:
            return self._create_mle_secured_message(data, message_info)

        elif security_indicator == 255:
            return self._create_mle_message(data, message_info)

        else:
            raise RuntimeError(
                "Could not create MLE message. Unknown security indicator value: {}".format(security_indicator))
