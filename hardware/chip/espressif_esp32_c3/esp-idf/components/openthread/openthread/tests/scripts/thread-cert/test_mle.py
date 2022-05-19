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
import random
import struct
import unittest

import common
import config
import mle
import network_data


def any_address():
    return random.getrandbits(16)


def any_receiver():
    return random.getrandbits(1)


def any_secure():
    return random.getrandbits(1)


def any_device_type():
    return random.getrandbits(1)


def any_network_data():
    return random.getrandbits(1)


mode_map = {
    0x00: {
        "receiver": 0,
        "secure": 0,
        "device_type": 0,
        "network_data": 0
    },
    0x08: {
        "receiver": 1,
        "secure": 0,
        "device_type": 0,
        "network_data": 0
    },
    0x04: {
        "receiver": 0,
        "secure": 1,
        "device_type": 0,
        "network_data": 0
    },
    0x0C: {
        "receiver": 1,
        "secure": 1,
        "device_type": 0,
        "network_data": 0
    },
    0x02: {
        "receiver": 0,
        "secure": 0,
        "device_type": 1,
        "network_data": 0
    },
    0x0A: {
        "receiver": 1,
        "secure": 0,
        "device_type": 1,
        "network_data": 0
    },
    0x06: {
        "receiver": 0,
        "secure": 1,
        "device_type": 1,
        "network_data": 0
    },
    0x0E: {
        "receiver": 1,
        "secure": 1,
        "device_type": 1,
        "network_data": 0
    },
    0x01: {
        "receiver": 0,
        "secure": 0,
        "device_type": 0,
        "network_data": 1
    },
    0x09: {
        "receiver": 1,
        "secure": 0,
        "device_type": 0,
        "network_data": 1
    },
    0x05: {
        "receiver": 0,
        "secure": 1,
        "device_type": 0,
        "network_data": 1
    },
    0x0D: {
        "receiver": 1,
        "secure": 1,
        "device_type": 0,
        "network_data": 1
    },
    0x03: {
        "receiver": 0,
        "secure": 0,
        "device_type": 1,
        "network_data": 1
    },
    0x0B: {
        "receiver": 1,
        "secure": 0,
        "device_type": 1,
        "network_data": 1
    },
    0x07: {
        "receiver": 0,
        "secure": 1,
        "device_type": 1,
        "network_data": 1
    },
    0x0F: {
        "receiver": 1,
        "secure": 1,
        "device_type": 1,
        "network_data": 1
    }
}


def any_mode():
    return random.getrandbits(4)


def any_timeout():
    return random.getrandbits(32)


def any_challenge():
    length = random.randint(4, 8)
    return bytearray(random.getrandbits(8) for _ in range(length))


def any_response():
    length = random.randint(4, 8)
    return bytearray(random.getrandbits(8) for _ in range(length))


def any_link_layer_frame_counter():
    return random.getrandbits(32)


def any_mle_frame_counter():
    return random.getrandbits(32)


def any_output():
    return random.getrandbits(2)


def any_input():
    return random.getrandbits(2)


def any_route():
    return random.getrandbits(4)


def any_router_id_mask():
    return random.getrandbits(64)


def any_link_quality_and_route_data(length=None):
    length = length if length is not None else random.randint(0, 63)
    return [random.getrandbits(8) for _ in range(length)]


def any_partition_id():
    return random.getrandbits(32)


def any_weighting():
    return random.getrandbits(8)


def any_data_version():
    return random.getrandbits(8)


def any_stable_data_version():
    return random.getrandbits(8)


def any_leader_router_id():
    return random.getrandbits(8)


scan_mask_map = {
    0x00: {
        "router": 0,
        "end_device": 0
    },
    0x40: {
        "router": 0,
        "end_device": 1
    },
    0x80: {
        "router": 1,
        "end_device": 0
    },
    0xC0: {
        "router": 1,
        "end_device": 1
    },
}


def any_scan_mask_router():
    return random.getrandbits(1)


def any_scan_mask_end_device():
    return random.getrandbits(1)


def any_scan_mask():
    return (random.getrandbits(2) << 6)


def any_link_margin():
    return random.getrandbits(8)


def any_status():
    return random.getrandbits(8)


def any_version():
    return random.getrandbits(16)


def any_channel_page():
    return random.getrandbits(8)


def any_channel():
    return random.getrandbits(16)


def any_pan_id():
    return random.getrandbits(16)


def any_timestamp_seconds():
    return random.getrandbits(48)


def any_timestamp_ticks():
    return random.getrandbits(15)


def any_u():
    return random.getrandbits(1)


def any_pp():
    return (random.getrandbits(2) << 6)


def any_link_quality_3():
    return random.getrandbits(8)


def any_link_quality_2():
    return random.getrandbits(8)


def any_link_quality_1():
    return random.getrandbits(8)


def any_leader_cost():
    return random.getrandbits(8)


def any_id_sequence():
    return random.getrandbits(8)


def any_active_routers():
    return random.getrandbits(8)


def any_sed_buffer_size():
    return random.getrandbits(16)


def any_sed_datagram_count():
    return random.getrandbits(8)


def any_tlvs(length=None):
    if length is None:
        length = random.randint(0, 16)

    return [random.getrandbits(8) for _ in range(length)]


def any_cid():
    return random.getrandbits(4)


def any_iid():
    return bytearray([random.getrandbits(8) for _ in range(8)])


def any_ipv6_address():
    return bytearray([random.getrandbits(8) for _ in range(16)])


def any_addresses():
    addresses = [mle.AddressCompressed(any_cid(), any_iid()), mle.AddressFull(any_ipv6_address())]

    return addresses


def any_key_id_mode():
    return random.getrandbits(2)


def any_security_level():
    return random.getrandbits(3)


def any_frame_counter():
    return random.getrandbits(32)


def any_key_id(key_id_mode):
    if key_id_mode == 0:
        length = 0
    elif key_id_mode == 1:
        length = 1
    elif key_id_mode == 2:
        length = 5
    elif key_id_mode == 3:
        length = 9

    return bytearray([random.getrandbits(8) for _ in range(length)])


def any_eui64():
    return bytearray([random.getrandbits(8) for _ in range(8)])


class TestSourceAddress(unittest.TestCase):

    def test_should_return_address_value_when_address_property_is_called(self):
        # GIVEN
        address = any_address()

        source_address = mle.SourceAddress(address)

        # WHEN
        actual_address = source_address.address

        # THEN
        self.assertEqual(address, actual_address)


class TestSourceAddressFactory(unittest.TestCase):

    def test_should_create_SourceAddress_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        address = any_address()

        factory = mle.SourceAddressFactory()

        data = struct.pack(">H", address)

        # WHEN
        actual_source_address = factory.parse(io.BytesIO(data), dict())

        # THEN
        self.assertTrue(isinstance(actual_source_address, mle.SourceAddress))
        self.assertEqual(address, actual_source_address.address)


class TestMode(unittest.TestCase):

    def test_should_return_receiver_value_when_receiver_property_is_called(self):
        # GIVEN
        receiver = any_receiver()

        mode = mle.Mode(receiver, any_secure(), any_device_type(), any_network_data())

        # WHEN
        actual_receiver = mode.receiver

        # THEN
        self.assertEqual(receiver, actual_receiver)

    def test_should_return_secure_value_when_secure_property_is_called(self):
        # GIVEN
        secure = any_secure()

        mode = mle.Mode(any_receiver(), secure, any_device_type(), any_network_data())

        # WHEN
        actual_secure = mode.secure

        # THEN
        self.assertEqual(secure, actual_secure)

    def test_should_return_device_type_value_when_device_type_property_is_called(self):
        # GIVEN
        device_type = any_device_type()

        mode = mle.Mode(any_receiver(), any_secure(), device_type, any_network_data())

        # WHEN
        actual_device_type = mode.device_type

        # THEN
        self.assertEqual(device_type, actual_device_type)

    def test_should_return_network_data_value_when_network_data_property_is_called(self):
        # GIVEN
        network_data = any_network_data()

        mode = mle.Mode(any_receiver(), any_secure(), any_device_type(), network_data)

        # WHEN
        actual_network_data = mode.network_data

        # THEN
        self.assertEqual(network_data, actual_network_data)


class TestModeFactory(unittest.TestCase):

    def test_should_create_Mode_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        mode = any_mode()

        factory = mle.ModeFactory()

        data = bytearray([mode])

        # WHEN
        actual_mode = factory.parse(io.BytesIO(data), dict())

        # THEN
        self.assertTrue(isinstance(actual_mode, mle.Mode))
        self.assertEqual(mode_map[mode]["receiver"], actual_mode.receiver)
        self.assertEqual(mode_map[mode]["secure"], actual_mode.secure)
        self.assertEqual(mode_map[mode]["device_type"], actual_mode.device_type)
        self.assertEqual(mode_map[mode]["network_data"], actual_mode.network_data)


class TestTimeout(unittest.TestCase):

    def test_should_return_timeout_value_when_timeout_property_is_called(self):
        # GIVEN
        timeout = any_timeout()

        timeout_obj = mle.Timeout(timeout)

        # WHEN
        actual_timeout = timeout_obj.timeout

        # THEN
        self.assertEqual(timeout, actual_timeout)


class TestTimeoutFactory(unittest.TestCase):

    def test_should_create_Timeout_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        timeout = any_timeout()

        factory = mle.TimeoutFactory()

        data = struct.pack(">I", timeout)

        # WHEN
        actual_timeout = factory.parse(io.BytesIO(data), dict())

        # THEN
        self.assertTrue(isinstance(actual_timeout, mle.Timeout))
        self.assertEqual(timeout, actual_timeout.timeout)


class TestChallenge(unittest.TestCase):

    def test_should_return_challenge_value_when_challenge_property_is_called(self):
        # GIVEN
        challenge = any_challenge()

        challenge_obj = mle.Challenge(challenge)

        # WHEN
        actual_challenge = challenge_obj.challenge

        # THEN
        self.assertEqual(challenge, actual_challenge)


class TestChallengeFactory(unittest.TestCase):

    def test_should_create_Challenge_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        challenge = any_challenge()

        factory = mle.ChallengeFactory()

        data = challenge

        # WHEN
        actual_challenge = factory.parse(io.BytesIO(data), dict())

        # THEN
        self.assertTrue(isinstance(actual_challenge, mle.Challenge))
        self.assertEqual(challenge, actual_challenge.challenge)


class TestResponse(unittest.TestCase):

    def test_should_return_response_value_when_response_property_is_called(self):
        # GIVEN
        response = any_response()

        response_obj = mle.Response(response)

        # WHEN
        actual_response = response_obj.response

        # THEN
        self.assertEqual(response, actual_response)


class TestResponseFactory(unittest.TestCase):

    def test_should_create_Challenge_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        response = any_response()

        factory = mle.ResponseFactory()

        data = response

        # WHEN
        actual_response = factory.parse(io.BytesIO(data), dict())

        # THEN
        self.assertTrue(isinstance(actual_response, mle.Response))
        self.assertEqual(response, actual_response.response)


class TestLinkLayerFrameCounter(unittest.TestCase):

    def test_should_return_frame_counter_value_when_frame_counter_property_is_called(self):
        # GIVEN
        link_layer_frame_counter = any_link_layer_frame_counter()

        link_layer_frame_counter_obj = mle.LinkLayerFrameCounter(link_layer_frame_counter)

        # WHEN
        actual_link_layer_frame_counter = link_layer_frame_counter_obj.frame_counter

        # THEN
        self.assertEqual(link_layer_frame_counter, actual_link_layer_frame_counter)


class TestLinkLayerFrameCounterFactory(unittest.TestCase):

    def test_should_create_LinkLayerFrameCounter_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        link_layer_frame_counter = any_link_layer_frame_counter()

        factory = mle.LinkLayerFrameCounterFactory()

        data = struct.pack(">I", link_layer_frame_counter)

        # WHEN
        actual_link_layer_frame_counter = factory.parse(io.BytesIO(data), dict())

        # THEN
        self.assertTrue(isinstance(actual_link_layer_frame_counter, mle.LinkLayerFrameCounter))
        self.assertEqual(link_layer_frame_counter, actual_link_layer_frame_counter.frame_counter)


class TestMleFrameCounter(unittest.TestCase):

    def test_should_return_frame_counter_value_when_frame_counter_property_is_called(self):
        # GIVEN
        mle_frame_counter = any_mle_frame_counter()

        mle_frame_counter_obj = mle.MleFrameCounter(mle_frame_counter)

        # WHEN
        actual_mle_frame_counter = mle_frame_counter_obj.frame_counter

        # THEN
        self.assertEqual(mle_frame_counter, actual_mle_frame_counter)


class TestMleFrameCounterFactory(unittest.TestCase):

    def test_should_create_MleFrameCounter_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        mle_frame_counter = any_mle_frame_counter()

        factory = mle.MleFrameCounterFactory()

        data = struct.pack(">I", mle_frame_counter)

        # WHEN
        actual_mle_frame_counter = factory.parse(io.BytesIO(data), dict())

        # THEN
        self.assertTrue(isinstance(actual_mle_frame_counter, mle.MleFrameCounter))
        self.assertEqual(mle_frame_counter, actual_mle_frame_counter.frame_counter)


class TestLinkQualityAndRouteData(unittest.TestCase):

    def test_should_return_output_value_when_output_property_is_called(self):
        # GIVEN
        output = any_output()

        lqrd = mle.LinkQualityAndRouteData(output, any_input(), any_route())

        # WHEN
        actual_output = lqrd.output

        # THEN
        self.assertEqual(output, actual_output)

    def test_should_return_input_value_when_input_property_is_called(self):
        # GIVEN
        _input = any_input()

        lqrd = mle.LinkQualityAndRouteData(any_output(), _input, any_route())

        # WHEN
        actual_input = lqrd.input

        # THEN
        self.assertEqual(_input, actual_input)

    def test_should_return_route_value_when_route_property_is_called(self):
        # GIVEN
        route = any_route()

        lqrd = mle.LinkQualityAndRouteData(any_output(), any_input(), route)

        # WHEN
        actual_route = lqrd.route

        # THEN
        self.assertEqual(route, actual_route)


class TestLinkQualityAndRouteDataFactory(unittest.TestCase):

    def test_should_create_LinkQualityAndRouteData_from_well_known_byte_when_parse_method_is_called(self):
        # GIVEN
        factory = mle.LinkQualityAndRouteDataFactory()

        data = bytearray([0x66])

        # WHEN
        actual_lqrd = factory.parse(io.BytesIO(data), dict())

        # THEN
        self.assertEqual(1, actual_lqrd.output)
        self.assertEqual(2, actual_lqrd.input)
        self.assertEqual(6, actual_lqrd.route)

    def test_should_create_LinkQualityAndRouteData_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        output = any_output()
        _input = any_input()
        route = any_route()

        lqrd = (output << 6) | (_input << 4) | route

        factory = mle.LinkQualityAndRouteDataFactory()

        data = bytearray([lqrd])

        # WHEN
        actual_lqrd = factory.parse(io.BytesIO(data), dict())

        # THEN
        self.assertTrue(isinstance(actual_lqrd, mle.LinkQualityAndRouteData))
        self.assertEqual(output, actual_lqrd.output)
        self.assertEqual(_input, actual_lqrd.input)
        self.assertEqual(route, actual_lqrd.route)


class TestRoute64(unittest.TestCase):

    def test_should_return_id_sequence_value_when_id_sequence_property_is_called(self):
        # GIVEN
        id_sequence = any_id_sequence()

        route64_obj = mle.Route64(id_sequence, any_router_id_mask(), any_link_quality_and_route_data())

        # WHEN
        actual_id_sequence = route64_obj.id_sequence

        # THEN
        self.assertEqual(id_sequence, actual_id_sequence)

    def test_should_return_router_id_mask_value_when_router_id_mask_property_is_called(self):
        # GIVEN
        router_id_mask = any_router_id_mask()

        route64_obj = mle.Route64(any_id_sequence(), router_id_mask, any_link_quality_and_route_data())

        # WHEN
        actual_router_id_mask = route64_obj.router_id_mask

        # THEN
        self.assertEqual(router_id_mask, actual_router_id_mask)

    def test_should_return_link_quality_and_route_data_value_when_link_quality_and_route_data_property_is_called(self):
        # GIVEN
        link_quality_and_route_data = any_link_quality_and_route_data()

        route64_obj = mle.Route64(any_id_sequence(), any_router_id_mask(), link_quality_and_route_data)

        # WHEN
        actual_link_quality_and_route_data = route64_obj.link_quality_and_route_data

        # THEN
        self.assertEqual(link_quality_and_route_data, actual_link_quality_and_route_data)


class TestRoute64Factory(unittest.TestCase):

    def test_should_create_Route64_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        class DummyLQRDFactory:

            def parse(self, data, context):
                return ord(data.read(1))

        id_sequence = any_id_sequence()
        router_id_mask = any_router_id_mask()

        router_count = 0
        for i in range(64):
            router_count += (router_id_mask >> i) & 0x01

        link_quality_and_route_data = any_link_quality_and_route_data(router_count)

        factory = mle.Route64Factory(DummyLQRDFactory())

        data = bytearray([id_sequence]) + struct.pack(">Q", router_id_mask) + bytearray(link_quality_and_route_data)

        # WHEN
        actual_route64 = factory.parse(io.BytesIO(data), dict())

        # THEN
        self.assertTrue(isinstance(actual_route64, mle.Route64))
        self.assertEqual(id_sequence, actual_route64.id_sequence)
        self.assertEqual(router_id_mask, actual_route64.router_id_mask)
        self.assertEqual([b for b in link_quality_and_route_data], actual_route64.link_quality_and_route_data)


class TestAddress16(unittest.TestCase):

    def test_should_return_address_value_when_address_property_is_called(self):
        # GIVEN
        address = any_address()

        address16 = mle.Address16(address)

        # WHEN
        actual_address = address16.address

        # THEN
        self.assertEqual(address, actual_address)


class TestAddress16Factory(unittest.TestCase):

    def test_should_create_Address16_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        address = any_address()

        factory = mle.Address16Factory()

        data = struct.pack(">H", address)

        # WHEN
        actual_address16 = factory.parse(io.BytesIO(data), dict())

        # THEN
        self.assertTrue(isinstance(actual_address16, mle.Address16))
        self.assertEqual(address, actual_address16.address)


class TestLeaderData(unittest.TestCase):

    def test_should_return_partition_id_value_when_partition_id_property_is_called(self):
        # GIVEN
        partition_id = any_partition_id()

        leader_data = mle.LeaderData(partition_id, any_weighting(), any_data_version(), any_stable_data_version(),
                                     any_leader_router_id())

        # WHEN
        actual_partition_id = leader_data.partition_id

        # THEN
        self.assertEqual(partition_id, actual_partition_id)

    def test_should_return_weighting_value_when_weighting_property_is_called(self):
        # GIVEN
        weighting = any_weighting()

        leader_data = mle.LeaderData(any_partition_id(), weighting, any_data_version(), any_stable_data_version(),
                                     any_leader_router_id())

        # WHEN
        actual_weighting = leader_data.weighting

        # THEN
        self.assertEqual(weighting, actual_weighting)

    def test_should_return_data_version_value_when_data_version_property_is_called(self):
        # GIVEN
        data_version = any_data_version()

        leader_data = mle.LeaderData(any_partition_id(), any_weighting(), data_version, any_stable_data_version(),
                                     any_leader_router_id())

        # WHEN
        actual_data_version = leader_data.data_version

        # THEN
        self.assertEqual(data_version, actual_data_version)

    def test_should_return_stable_data_version_value_when_stable_data_version_property_is_called(self):
        # GIVEN
        stable_data_version = any_stable_data_version()

        leader_data = mle.LeaderData(any_partition_id(), any_weighting(), any_data_version(), stable_data_version,
                                     any_leader_router_id())

        # WHEN
        actual_stable_data_version = leader_data.stable_data_version

        # THEN
        self.assertEqual(stable_data_version, actual_stable_data_version)

    def test_should_return_leader_router_id_value_when_leader_router_id_property_is_called(self):
        # GIVEN
        leader_router_id = any_leader_router_id()

        leader_data = mle.LeaderData(any_partition_id(), any_weighting(), any_data_version(),
                                     any_stable_data_version(), leader_router_id)

        # WHEN
        actual_leader_router_id = leader_data.leader_router_id

        # THEN
        self.assertEqual(leader_router_id, actual_leader_router_id)


class TestLeaderDataFactory(unittest.TestCase):

    def test_should_create_Address16_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        partition_id = any_partition_id()
        weighting = any_weighting()
        data_version = any_data_version()
        stable_data_version = any_stable_data_version()
        leader_router_id = any_leader_router_id()

        factory = mle.LeaderDataFactory()

        data = bytearray(struct.pack(">I", partition_id)) + \
            bytearray([weighting, data_version, stable_data_version, leader_router_id])

        # WHEN
        actual_leader_data = factory.parse(io.BytesIO(data), dict())

        # THEN
        self.assertTrue(isinstance(actual_leader_data, mle.LeaderData))
        self.assertEqual(partition_id, actual_leader_data.partition_id)
        self.assertEqual(weighting, actual_leader_data.weighting)
        self.assertEqual(data_version, actual_leader_data.data_version)
        self.assertEqual(stable_data_version, actual_leader_data.stable_data_version)
        self.assertEqual(leader_router_id, actual_leader_data.leader_router_id)


class TestNetworkData(unittest.TestCase):

    def test_should_return_tlvs_value_when_tlvs_property_is_called(self):
        # GIVEN
        tlvs = any_tlvs()

        network_data = mle.NetworkData(tlvs)

        # WHEN
        actual_tlvs = network_data.tlvs

        # THEN
        self.assertEqual(tlvs, actual_tlvs)


class TestNetworkDataFactory(unittest.TestCase):

    def test_should_create_TlvRequest_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        class DummyNetworkTlvsFactory:

            def parse(self, data, context):
                return [b for b in bytearray(data.read())]

        tlvs = any_tlvs()

        factory = mle.NetworkDataFactory(DummyNetworkTlvsFactory())

        data = bytearray(tlvs)

        # WHEN
        actual_network_data = factory.parse(io.BytesIO(data), dict())

        # THEN
        self.assertTrue(isinstance(actual_network_data, mle.NetworkData))
        self.assertEqual(tlvs, actual_network_data.tlvs)


class TestTlvRequest(unittest.TestCase):

    def test_should_return_tlvs_value_when_tlvs_property_is_called(self):
        # GIVEN
        tlvs = any_tlvs()

        tlv_request = mle.TlvRequest(tlvs)

        # WHEN
        actual_tlvs = tlv_request.tlvs

        # THEN
        self.assertEqual(tlvs, actual_tlvs)


class TestTlvRequestFactory(unittest.TestCase):

    def test_should_create_TlvRequest_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        tlvs = any_tlvs()

        factory = mle.TlvRequestFactory()

        data = bytearray(tlvs)

        # WHEN
        actual_tlv_request = factory.parse(io.BytesIO(data), dict())

        # THEN
        self.assertTrue(isinstance(actual_tlv_request, mle.TlvRequest))
        self.assertEqual(tlvs, actual_tlv_request.tlvs)


class TestScanMask(unittest.TestCase):

    def test_should_return_router_value_when_router_property_is_called(self):
        # GIVEN
        router = any_scan_mask_router()

        scan_mask = mle.ScanMask(router, any_scan_mask_end_device())

        # WHEN
        actual_router = scan_mask.router

        # THEN
        self.assertEqual(router, actual_router)

    def test_should_return_end_device_value_when_end_device_property_is_called(self):
        # GIVEN
        end_device = any_scan_mask_end_device()

        scan_mask = mle.ScanMask(any_scan_mask_router(), end_device)

        # WHEN
        actual_end_device = scan_mask.end_device

        # THEN
        self.assertEqual(end_device, actual_end_device)


class TestScanMaskFactory(unittest.TestCase):

    def test_should_create_ScanMask_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        scan_mask = any_scan_mask()

        factory = mle.ScanMaskFactory()

        data = bytearray([scan_mask])

        # WHEN
        actual_scan_mask = factory.parse(io.BytesIO(data), dict())

        # THEN
        self.assertTrue(isinstance(actual_scan_mask, mle.ScanMask))
        self.assertEqual(scan_mask_map[scan_mask]["router"], actual_scan_mask.router)
        self.assertEqual(scan_mask_map[scan_mask]["end_device"], actual_scan_mask.end_device)


class TestConnectivity(unittest.TestCase):

    def test_should_return_pp_value_when_pp_property_is_called(self):
        # GIVEN
        pp_byte = any_pp()

        connectivity_obj = mle.Connectivity(pp_byte, any_link_quality_3(), any_link_quality_2(), any_link_quality_1(),
                                            any_leader_cost(), any_id_sequence(), any_active_routers(),
                                            any_sed_buffer_size(), any_sed_datagram_count())

        # WHEN
        actual_pp = connectivity_obj.pp

        # THEN
        self.assertEqual(common.map_pp(pp_byte), actual_pp)

    def test_should_return_link_quality_3_value_when_link_quality_3_property_is_called(self):
        # GIVEN
        link_quality_3 = any_link_quality_3()

        connectivity_obj = mle.Connectivity(any_pp(), link_quality_3, any_link_quality_2(), any_link_quality_1(),
                                            any_leader_cost(), any_id_sequence(), any_active_routers(),
                                            any_sed_buffer_size(), any_sed_datagram_count())

        # WHEN
        actual_link_quality_3 = connectivity_obj.link_quality_3

        # THEN
        self.assertEqual(link_quality_3, actual_link_quality_3)

    def test_should_return_link_quality_2_value_when_link_quality_2_property_is_called(self):
        # GIVEN
        link_quality_2 = any_link_quality_2()

        connectivity_obj = mle.Connectivity(any_pp(), any_link_quality_3(), link_quality_2, any_link_quality_1(),
                                            any_leader_cost(), any_id_sequence(), any_active_routers(),
                                            any_sed_buffer_size(), any_sed_datagram_count())

        # WHEN
        actual_link_quality_2 = connectivity_obj.link_quality_2

        # THEN
        self.assertEqual(link_quality_2, actual_link_quality_2)

    def test_should_return_link_quality_1_value_when_link_quality_1_property_is_called(self):
        # GIVEN
        link_quality_1 = any_link_quality_1()

        connectivity_obj = mle.Connectivity(any_pp(), any_link_quality_3(), any_link_quality_2(), link_quality_1,
                                            any_leader_cost(), any_id_sequence(), any_active_routers(),
                                            any_sed_buffer_size(), any_sed_datagram_count())

        # WHEN
        actual_link_quality_1 = connectivity_obj.link_quality_1

        # THEN
        self.assertEqual(link_quality_1, actual_link_quality_1)

    def test_should_return_leader_cost_value_when_leader_cost_property_is_called(self):
        # GIVEN
        leader_cost = any_leader_cost()

        connectivity_obj = mle.Connectivity(any_pp(), any_link_quality_3(), any_link_quality_2(),
                                            any_link_quality_1(), leader_cost, any_id_sequence(), any_active_routers(),
                                            any_sed_buffer_size(), any_sed_datagram_count())

        # WHEN
        actual_leader_cost = connectivity_obj.leader_cost

        # THEN
        self.assertEqual(leader_cost, actual_leader_cost)

    def test_should_return_id_sequence_value_when_id_sequence_property_is_called(self):
        # GIVEN
        id_sequence = any_id_sequence()

        connectivity_obj = mle.Connectivity(any_pp(), any_link_quality_3(), any_link_quality_2(), any_link_quality_1(),
                                            any_leader_cost(), id_sequence, any_active_routers(),
                                            any_sed_buffer_size(), any_sed_datagram_count())

        # WHEN
        actual_id_sequence = connectivity_obj.id_sequence

        # THEN
        self.assertEqual(id_sequence, actual_id_sequence)

    def test_should_return_active_routers_value_when_active_routers_property_is_called(self):
        # GIVEN
        active_routers = any_active_routers()

        connectivity_obj = mle.Connectivity(any_pp(), any_link_quality_3(), any_link_quality_2(), any_link_quality_1(),
                                            any_leader_cost(), any_id_sequence(), active_routers,
                                            any_sed_buffer_size(), any_sed_datagram_count())

        # WHEN
        actual_active_routers = connectivity_obj.active_routers

        # THEN
        self.assertEqual(active_routers, actual_active_routers)

    def test_should_return_sed_buffer_size_value_when_sed_buffer_size_property_is_called(self):
        # GIVEN
        sed_buffer_size = any_sed_buffer_size()

        connectivity_obj = mle.Connectivity(any_pp(), any_link_quality_3(), any_link_quality_2(), any_link_quality_1(),
                                            any_leader_cost(), any_id_sequence(), any_active_routers(),
                                            sed_buffer_size, any_sed_datagram_count())

        # WHEN
        actual_sed_buffer_size = connectivity_obj.sed_buffer_size

        # THEN
        self.assertEqual(sed_buffer_size, actual_sed_buffer_size)

    def test_should_return_sed_datagram_count_value_when_sed_datagram_count_property_is_called(self):
        # GIVEN
        sed_datagram_count = any_sed_datagram_count()

        connectivity_obj = mle.Connectivity(any_pp(), any_link_quality_3(), any_link_quality_2(), any_link_quality_1(),
                                            any_leader_cost(), any_id_sequence(), any_active_routers(),
                                            any_sed_buffer_size(), sed_datagram_count)

        # WHEN
        actual_sed_datagram_count = connectivity_obj.sed_datagram_count

        # THEN
        self.assertEqual(sed_datagram_count, actual_sed_datagram_count)


class TestConnectivityFactory(unittest.TestCase):

    def test_should_create_Connectivity_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        pp_byte = any_pp()
        link_quality_3 = any_link_quality_3()
        link_quality_2 = any_link_quality_2()
        link_quality_1 = any_link_quality_1()
        leader_cost = any_leader_cost()
        id_sequence = any_id_sequence()
        active_routers = any_active_routers()
        sed_buffer_size = any_sed_buffer_size()
        sed_datagram_count = any_sed_datagram_count()

        factory = mle.ConnectivityFactory()

        data = bytearray([
            pp_byte, link_quality_3, link_quality_2, link_quality_1, leader_cost, id_sequence, active_routers
        ]) + struct.pack(">H", sed_buffer_size) + bytearray([sed_datagram_count])

        # WHEN
        actual_connectivity = factory.parse(io.BytesIO(data), dict())

        # THEN
        self.assertTrue(isinstance(actual_connectivity, mle.Connectivity))
        self.assertEqual(common.map_pp(pp_byte), actual_connectivity.pp)
        self.assertEqual(link_quality_3, actual_connectivity.link_quality_3)
        self.assertEqual(link_quality_2, actual_connectivity.link_quality_2)
        self.assertEqual(link_quality_1, actual_connectivity.link_quality_1)
        self.assertEqual(leader_cost, actual_connectivity.leader_cost)
        self.assertEqual(id_sequence, actual_connectivity.id_sequence)
        self.assertEqual(active_routers, actual_connectivity.active_routers)
        self.assertEqual(sed_buffer_size, actual_connectivity.sed_buffer_size)
        self.assertEqual(sed_datagram_count, actual_connectivity.sed_datagram_count)

    def test_should_create_Connectivity_without_sed_data_when_parse_method_is_called(self):
        # GIVEN
        pp_byte = any_pp()
        link_quality_3 = any_link_quality_3()
        link_quality_2 = any_link_quality_2()
        link_quality_1 = any_link_quality_1()
        leader_cost = any_leader_cost()
        id_sequence = any_id_sequence()
        active_routers = any_active_routers()
        any_sed_buffer_size()
        any_sed_datagram_count()

        factory = mle.ConnectivityFactory()

        data = bytearray(
            [pp_byte, link_quality_3, link_quality_2, link_quality_1, leader_cost, id_sequence, active_routers])

        # WHEN
        actual_connectivity = factory.parse(io.BytesIO(data), dict())

        # THEN
        self.assertTrue(isinstance(actual_connectivity, mle.Connectivity))
        self.assertEqual(common.map_pp(pp_byte), actual_connectivity.pp)
        self.assertEqual(link_quality_3, actual_connectivity.link_quality_3)
        self.assertEqual(link_quality_2, actual_connectivity.link_quality_2)
        self.assertEqual(link_quality_1, actual_connectivity.link_quality_1)
        self.assertEqual(leader_cost, actual_connectivity.leader_cost)
        self.assertEqual(id_sequence, actual_connectivity.id_sequence)
        self.assertEqual(active_routers, actual_connectivity.active_routers)
        self.assertEqual(None, actual_connectivity.sed_buffer_size)
        self.assertEqual(None, actual_connectivity.sed_datagram_count)


class TestLinkMargin(unittest.TestCase):

    def test_should_return_link_margin_value_when_link_margin_property_is_called(self):
        # GIVEN
        link_margin = any_link_margin()

        link_margin_obj = mle.LinkMargin(link_margin)

        # WHEN
        actual_link_margin = link_margin_obj.link_margin

        # THEN
        self.assertEqual(link_margin, actual_link_margin)


class TestLinkMarginFactory(unittest.TestCase):

    def test_should_create_LinkMargin_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        link_margin = any_link_margin()

        factory = mle.LinkMarginFactory()

        data = bytearray([link_margin])

        # WHEN
        actual_link_margin = factory.parse(io.BytesIO(data), dict())

        # THEN
        self.assertTrue(isinstance(actual_link_margin, mle.LinkMargin))
        self.assertEqual(link_margin, actual_link_margin.link_margin)


class TestStatus(unittest.TestCase):

    def test_should_return_status_value_when_status_property_is_called(self):
        # GIVEN
        status = any_status()

        status_obj = mle.Status(status)

        # WHEN
        actual_status = status_obj.status

        # THEN
        self.assertEqual(status, actual_status)


class TestStatusFactory(unittest.TestCase):

    def test_should_create_Status_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        status = any_status()

        factory = mle.StatusFactory()

        data = bytearray([status])

        # WHEN
        actual_status = factory.parse(io.BytesIO(data), dict())

        # THEN
        self.assertTrue(isinstance(actual_status, mle.Status))
        self.assertEqual(status, actual_status.status)


class TestVersion(unittest.TestCase):

    def test_should_return_version_value_when_version_property_is_called(self):
        # GIVEN
        version = any_version()

        version_obj = mle.Version(version)

        # WHEN
        actual_version = version_obj.version

        # THEN
        self.assertEqual(version, actual_version)


class TestVersionFactory(unittest.TestCase):

    def test_should_create_Version_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        version = any_version()

        factory = mle.VersionFactory()

        data = struct.pack(">H", version)

        # WHEN
        actual_version = factory.parse(io.BytesIO(data), dict())

        # THEN
        self.assertTrue(isinstance(actual_version, mle.Version))
        self.assertEqual(version, actual_version.version)


class TestAddressRegistrationFull(unittest.TestCase):

    def test_should_return_ipv6_address_value_when_ipv6_address_property_is_called(self):
        # GIVEN
        ipv6_address = any_ipv6_address()

        addr_reg_full_obj = mle.AddressFull(ipv6_address)

        # WHEN
        actual_ipv6_address = addr_reg_full_obj.ipv6_address

        # THEN
        self.assertEqual(ipv6_address, actual_ipv6_address)


class TestAddressRegistrationFullFactory(unittest.TestCase):

    def test_should_create_AddressFull_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        ipv6_address = any_ipv6_address()

        factory = mle.AddressFullFactory()

        data = bytearray([0x00]) + bytearray(ipv6_address)

        # WHEN
        actual_addr_reg_full = factory.parse(io.BytesIO(data), dict())

        # THEN
        self.assertTrue(isinstance(actual_addr_reg_full, mle.AddressFull))
        self.assertEqual(ipv6_address, actual_addr_reg_full.ipv6_address)


class TestAddressRegistrationCompressed(unittest.TestCase):

    def test_should_return_cid_value_when_cid_property_is_called(self):
        # GIVEN
        cid = any_cid()

        addr_reg_compressed_obj = mle.AddressCompressed(cid, any_iid())

        # WHEN
        actual_cid = addr_reg_compressed_obj.cid

        # THEN
        self.assertEqual(cid, actual_cid)

    def test_should_return_cid_value_when_iid_property_is_called(self):
        # GIVEN
        iid = any_iid()

        addr_reg_compressed_obj = mle.AddressCompressed(any_cid(), iid)

        # WHEN
        actual_iid = addr_reg_compressed_obj.iid

        # THEN
        self.assertEqual(iid, actual_iid)


class TestAddressRegistrationCompressedFactory(unittest.TestCase):

    def test_should_create_AddressRegistrationCompressed_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        cid = any_cid()
        iid = any_iid()

        factory = mle.AddressCompressedFactory()

        data = bytearray([(1 << 7) | cid]) + iid

        # WHEN
        actual_addr_reg_compressed = factory.parse(io.BytesIO(data), dict())

        # THEN
        self.assertTrue(isinstance(actual_addr_reg_compressed, mle.AddressCompressed))
        self.assertEqual(cid, actual_addr_reg_compressed.cid)
        self.assertEqual(iid, actual_addr_reg_compressed.iid)


class TestAddressRegistration(unittest.TestCase):

    def test_should_return_addresses_value_when_addresses_property_is_called(self):
        # GIVEN
        addresses = any_addresses()

        addr_reg_obj = mle.AddressRegistration(addresses)

        # WHEN
        actual_addresses = addr_reg_obj.addresses

        # THEN
        self.assertEqual(addresses, actual_addresses)


class TestAddressRegistrationFactory(unittest.TestCase):

    def test_should_create_AddressRegistration_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        cid = any_cid()
        iid = any_iid()
        ipv6_address = any_ipv6_address()

        addresses = [mle.AddressCompressed(cid, iid), mle.AddressFull(ipv6_address)]

        factory = mle.AddressRegistrationFactory(mle.AddressCompressedFactory(), mle.AddressFullFactory())

        data = bytearray([(1 << 7) | cid]) + iid + bytearray([0]) + ipv6_address

        # WHEN
        actual_addr_reg = factory.parse(io.BytesIO(data), dict())

        # THEN
        self.assertTrue(isinstance(actual_addr_reg, mle.AddressRegistration))
        self.assertEqual(addresses[0].cid, actual_addr_reg.addresses[0].cid)
        self.assertEqual(addresses[0].iid, actual_addr_reg.addresses[0].iid)
        self.assertEqual(addresses[1].ipv6_address, actual_addr_reg.addresses[1].ipv6_address)


class TestChannel(unittest.TestCase):

    def test_should_return_channel_page_value_when_channel_page_property_is_called(self):
        # GIVEN
        channel_page = any_channel_page()

        channel_obj = mle.Channel(channel_page, any_channel())

        # WHEN
        actual_channel_page = channel_obj.channel_page

        # THEN
        self.assertEqual(channel_page, actual_channel_page)

    def test_should_return_channel_value_when_channel_property_is_called(self):
        # GIVEN
        channel = any_channel()

        channel_obj = mle.Channel(any_channel_page(), channel)

        # WHEN
        actual_channel = channel_obj.channel

        # THEN
        self.assertEqual(channel, actual_channel)


class TestChannelFactory(unittest.TestCase):

    def test_should_create_Channel_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        channel_page = any_channel_page()
        channel = any_channel()

        factory = mle.ChannelFactory()

        data = bytearray([channel_page]) + struct.pack(">H", channel)

        # WHEN
        actual_channel = factory.parse(io.BytesIO(data), dict())

        # THEN
        self.assertTrue(isinstance(actual_channel, mle.Channel))
        self.assertEqual(channel_page, actual_channel.channel_page)
        self.assertEqual(channel, actual_channel.channel)


class TestPanId(unittest.TestCase):

    def test_should_return_pan_id_value_when_pan_id_property_is_called(self):
        # GIVEN
        pan_id = any_pan_id()

        pan_id_obj = mle.PanId(pan_id)

        # WHEN
        actual_pan_id = pan_id_obj.pan_id

        # THEN
        self.assertEqual(pan_id, actual_pan_id)


class TestPanIdFactory(unittest.TestCase):

    def test_should_create_PanId_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        pan_id = any_pan_id()

        factory = mle.PanIdFactory()

        data = struct.pack(">H", pan_id)

        # WHEN
        actual_pan_id = factory.parse(io.BytesIO(data), dict())

        # THEN
        self.assertTrue(isinstance(actual_pan_id, mle.PanId))
        self.assertEqual(pan_id, actual_pan_id.pan_id)


class TestActiveTimestamp(unittest.TestCase):

    def test_should_return_timestamp_seconds_value_when_timestamp_seconds_property_is_called(self):
        # GIVEN
        timestamp_seconds = any_timestamp_seconds()

        active_timestamp_obj = mle.ActiveTimestamp(timestamp_seconds, any_timestamp_ticks(), any_u())

        # WHEN
        actual_timestamp_seconds = active_timestamp_obj.timestamp_seconds

        # THEN
        self.assertEqual(timestamp_seconds, actual_timestamp_seconds)

    def test_should_return_timestamp_ticks_value_when_timestamp_ticks_property_is_called(self):
        # GIVEN
        timestamp_ticks = any_timestamp_ticks()

        active_timestamp_obj = mle.ActiveTimestamp(any_timestamp_seconds(), timestamp_ticks, any_u())

        # WHEN
        actual_timestamp_ticks = active_timestamp_obj.timestamp_ticks

        # THEN
        self.assertEqual(timestamp_ticks, actual_timestamp_ticks)

    def test_should_return_u_value_when_u_property_is_called(self):
        # GIVEN
        u = any_u()

        active_timestamp_obj = mle.ActiveTimestamp(any_timestamp_seconds(), any_timestamp_ticks(), u)

        # WHEN
        actual_u = active_timestamp_obj.u

        # THEN
        self.assertEqual(u, actual_u)


class TestActiveTimestampFactory(unittest.TestCase):

    def test_should_create_ActiveTimestamp_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        timestamp_seconds = any_timestamp_seconds()
        timestamp_ticks = any_timestamp_ticks()
        u = any_u()

        factory = mle.ActiveTimestampFactory()

        data = struct.pack(">Q", timestamp_seconds)[2:] + struct.pack(">H", (timestamp_ticks << 1) | u)

        # WHEN
        active_timestamp = factory.parse(io.BytesIO(data), dict())

        # THEN
        self.assertTrue(isinstance(active_timestamp, mle.ActiveTimestamp))
        self.assertEqual(timestamp_seconds, active_timestamp.timestamp_seconds)
        self.assertEqual(timestamp_ticks, active_timestamp.timestamp_ticks)
        self.assertEqual(u, active_timestamp.u)


class TestPendingTimestamp(unittest.TestCase):

    def test_should_return_timestamp_seconds_value_when_timestamp_seconds_property_is_called(self):
        # GIVEN
        timestamp_seconds = any_timestamp_seconds()

        pending_timestamp_obj = mle.PendingTimestamp(timestamp_seconds, any_timestamp_ticks(), any_u())

        # WHEN
        actual_timestamp_seconds = pending_timestamp_obj.timestamp_seconds

        # THEN
        self.assertEqual(timestamp_seconds, actual_timestamp_seconds)

    def test_should_return_timestamp_ticks_value_when_timestamp_ticks_property_is_called(self):
        # GIVEN
        timestamp_ticks = any_timestamp_ticks()

        pending_timestamp_obj = mle.PendingTimestamp(any_timestamp_seconds(), timestamp_ticks, any_u())

        # WHEN
        actual_timestamp_ticks = pending_timestamp_obj.timestamp_ticks

        # THEN
        self.assertEqual(timestamp_ticks, actual_timestamp_ticks)

    def test_should_return_u_value_when_u_property_is_called(self):
        # GIVEN
        u = any_u()

        pending_timestamp_obj = mle.PendingTimestamp(any_timestamp_seconds(), any_timestamp_ticks(), u)

        # WHEN
        actual_u = pending_timestamp_obj.u

        # THEN
        self.assertEqual(u, actual_u)


class TestPendingTimestampFactory(unittest.TestCase):

    def test_should_create_PendingTimestamp_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        timestamp_seconds = any_timestamp_seconds()
        timestamp_ticks = any_timestamp_ticks()
        u = any_u()

        factory = mle.PendingTimestampFactory()

        data = struct.pack(">Q", timestamp_seconds)[2:] + struct.pack(">H", (timestamp_ticks << 1) | u)

        # WHEN
        pending_timestamp = factory.parse(io.BytesIO(data), dict())

        # THEN
        self.assertTrue(isinstance(pending_timestamp, mle.PendingTimestamp))
        self.assertEqual(timestamp_seconds, pending_timestamp.timestamp_seconds)
        self.assertEqual(timestamp_ticks, pending_timestamp.timestamp_ticks)
        self.assertEqual(u, pending_timestamp.u)


class TestMleCommandFactory(unittest.TestCase):

    def test_should_create_MleCommand_from_bytearray_when_parse_method_is_called(self):
        data = bytearray([
            0x0b, 0x04, 0x08, 0xa5, 0xf2, 0x9b, 0xde, 0xe3, 0xd8, 0xbe, 0xb9, 0x05, 0x04, 0x00, 0x00, 0x00, 0x00, 0x08,
            0x04, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x0d, 0x02, 0x04, 0x00, 0x00, 0x00, 0xf0, 0x12, 0x02, 0x00, 0x02,
            0x13, 0x09, 0x80, 0x86, 0xa2, 0x1b, 0x81, 0x6d, 0xb8, 0xb5, 0xe8, 0x0d, 0x03, 0x0a, 0x0c, 0x09
        ])

        factory = mle.MleCommandFactory(config.create_default_mle_tlvs_factories())

        # WHEN
        actual_mle_command = factory.parse(io.BytesIO(data), None)

        # THEN
        self.assertTrue(isinstance(actual_mle_command, mle.MleCommand))

        self.assertEqual(11, actual_mle_command.type)

        self.assertEqual(mle.Response(bytearray([0xa5, 0xf2, 0x9b, 0xde, 0xe3, 0xd8, 0xbe, 0xb9])),
                         actual_mle_command.tlvs[0])

        self.assertEqual(mle.LinkLayerFrameCounter(0), actual_mle_command.tlvs[1])

        self.assertEqual(mle.MleFrameCounter(1), actual_mle_command.tlvs[2])

        self.assertEqual(mle.Mode(receiver=1, secure=1, device_type=0, network_data=1), actual_mle_command.tlvs[3])

        self.assertEqual(mle.Timeout(240), actual_mle_command.tlvs[4])

        self.assertEqual(mle.Version(2), actual_mle_command.tlvs[5])

        self.assertEqual(
            mle.AddressRegistration(addresses=[
                mle.AddressCompressed(cid=0, iid=bytearray([0x86, 0xa2, 0x1b, 0x81, 0x6d, 0xb8, 0xb5, 0xe8]))
            ]), actual_mle_command.tlvs[6])

        self.assertEqual(mle.TlvRequest(tlvs=[10, 12, 9]), actual_mle_command.tlvs[7])


class TestMleMessageFactory(unittest.TestCase):

    def test_should_create_MleMessageSecured_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        message_info = common.MessageInfo()
        message_info.source_ipv6 = "fe80::10cf:d38b:3b61:5558"
        message_info.destination_ipv6 = "fe80::383e:9eed:7a01:36a5"

        message_info.source_mac_address = common.MacAddress.from_eui64(
            bytearray([0x12, 0xcf, 0xd3, 0x8b, 0x3b, 0x61, 0x55, 0x58]))
        message_info.destination_mac_address = common.MacAddress.from_eui64(
            bytearray([0x3a, 0x3e, 0x9e, 0xed, 0x7a, 0x01, 0x36, 0xa5]))

        data = bytearray([
            0x00, 0x15, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x14, 0x03, 0xe3, 0x72, 0x50, 0x4f, 0x8c,
            0x5c, 0x42, 0x81, 0x68, 0xe2, 0x11, 0xfc, 0xf5, 0x8c, 0x62, 0x8e, 0x83, 0x99, 0xe7, 0x26, 0x86, 0x34, 0x3b,
            0xa7, 0x68, 0xc7, 0x93, 0xfb, 0x72, 0xd9, 0xcc, 0x13, 0x5e, 0x5b, 0x96, 0x0e, 0xf1, 0x80, 0x03, 0x55, 0x4f,
            0x27, 0xc2, 0x96, 0xf4, 0x9c, 0x65, 0x82, 0x97, 0xcf, 0x97, 0x35, 0x89, 0xc2
        ])

        factory = config.create_default_mle_message_factory(network_key=config.DEFAULT_NETWORK_KEY)

        # WHEN
        actual_mle_message = factory.parse(io.BytesIO(data), message_info)

        # THEN
        self.assertTrue(isinstance(actual_mle_message, mle.MleMessageSecured))

        self.assertEqual(11, actual_mle_message.command.type)

        self.assertEqual(mle.Response(bytearray([0xa5, 0xf2, 0x9b, 0xde, 0xe3, 0xd8, 0xbe, 0xb9])),
                         actual_mle_message.command.tlvs[0])

        self.assertEqual(mle.LinkLayerFrameCounter(0), actual_mle_message.command.tlvs[1])

        self.assertEqual(mle.MleFrameCounter(1), actual_mle_message.command.tlvs[2])

        self.assertEqual(mle.Mode(receiver=1, secure=1, device_type=0, network_data=1),
                         actual_mle_message.command.tlvs[3])

        self.assertEqual(mle.Timeout(240), actual_mle_message.command.tlvs[4])

        self.assertEqual(mle.Version(2), actual_mle_message.command.tlvs[5])

        self.assertEqual(
            mle.AddressRegistration(addresses=[
                mle.AddressCompressed(cid=0, iid=bytearray([0x86, 0xa2, 0x1b, 0x81, 0x6d, 0xb8, 0xb5, 0xe8]))
            ]), actual_mle_message.command.tlvs[6])

        self.assertEqual(mle.TlvRequest(tlvs=[10, 12, 9]), actual_mle_message.command.tlvs[7])

        self.assertEqual(bytearray(data[-4:]), actual_mle_message.mic)

    def test_should_create_MleMessageSecured_with_MLE_Data_Response_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        message_info = common.MessageInfo()
        message_info.source_ipv6 = "fe80::241c:b11b:7b62:caf1"
        message_info.destination_ipv6 = "ff02::1"

        message_info.source_mac_address = common.MacAddress.from_eui64(
            bytearray([0x26, 0x1c, 0xb1, 0x1b, 0x7b, 0x62, 0xca, 0xf1]))
        message_info.destination_mac_address = common.MacAddress.from_eui64(
            bytearray([0x3a, 0xba, 0xad, 0xca, 0xfe, 0xde, 0xff, 0xa5]))

        data = bytearray([
            0x00, 0x15, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xca, 0xd3, 0x45, 0xe2, 0x35, 0x1d, 0x00,
            0x2d, 0x72, 0x71, 0xb1, 0x19, 0xaf, 0x8b, 0x05, 0xd9, 0x52, 0x74, 0xce, 0xe6, 0x36, 0x53, 0xeb, 0xc6, 0x25,
            0x94, 0x01, 0x6d, 0x20, 0xdf, 0x30, 0x82, 0xf8, 0xbb, 0x34, 0x47, 0x42, 0x50, 0xe9, 0x41, 0xa7, 0x33, 0xa5
        ])

        factory = config.create_default_mle_message_factory(network_key=config.DEFAULT_NETWORK_KEY)

        # WHEN
        actual_mle_message = factory.parse(io.BytesIO(data), message_info)

        # THEN
        self.assertTrue(isinstance(actual_mle_message, mle.MleMessageSecured))

        self.assertEqual(8, actual_mle_message.command.type)

        self.assertEqual(mle.SourceAddress(address=0x9400), actual_mle_message.command.tlvs[0])

        self.assertEqual(
            mle.LeaderData(partition_id=0x06d014ca,
                           weighting=64,
                           data_version=131,
                           stable_data_version=168,
                           leader_router_id=37), actual_mle_message.command.tlvs[1])

        self.assertEqual(
            mle.NetworkData(tlvs=[
                network_data.Prefix(domain_id=0,
                                    prefix_length=64,
                                    prefix=bytearray([0x12, 0x34, 0x12, 0x34, 0x12, 0x34, 0x12, 0x34]),
                                    sub_tlvs=[
                                        network_data.LowpanId(c=1, cid=1, context_length=64, stable=1),
                                        network_data.BorderRouter(
                                            border_router_16=37888, prf=0, p=1, s=1, d=0, c=0, r=1, o=1, n=0, stable=1)
                                    ],
                                    stable=1)
            ]), actual_mle_message.command.tlvs[2])

        self.assertEqual(bytearray(data[-4:]), actual_mle_message.mic)


if __name__ == "__main__":
    unittest.main()
