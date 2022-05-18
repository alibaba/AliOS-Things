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
import math
import random
import struct
import unittest

import common
import config
import network_data


def convert_route_to_bytearray(route):
    return struct.pack(">HB", route.border_router_16, ((route.prf & 0x03) << 6))


def convert_routes_to_bytearray(routes):
    data = bytearray()
    for route in routes:
        data += convert_route_to_bytearray(route)

    return data


def convert_has_route_to_bytearray(has_route):
    return convert_routes_to_bytearray(has_route.routes)


def convert_border_router_to_bytearray(border_router):
    data = struct.pack(
        ">HBB",
        border_router.border_router_16,
        (border_router.o & 0x01) | ((border_router.r & 0x01) << 1) | ((border_router.c & 0x01) << 2) |
        ((border_router.d & 0x01) << 3) | ((border_router.s & 0x01) << 4) | ((border_router.p & 0x01) << 5) |
        ((border_router.prf & 0x03) << 6),
        ((border_router.n & 0x01) << 7),
    )

    return data


def convert_lowpan_id_to_bytearray(lowpan_id):
    return bytearray([lowpan_id.cid | (lowpan_id.c << 4), lowpan_id.context_length])


def convert_prefix_sub_tlvs_to_bytearray(sub_tlvs):
    data = bytearray()

    for sub_tlv in sub_tlvs:
        if isinstance(sub_tlv, network_data.HasRoute):
            value = convert_has_route_to_bytearray(sub_tlv)
            _type = sub_tlv.stable | ((0 & 0x7F) << 1)

        elif isinstance(sub_tlv, network_data.BorderRouter):
            value = convert_border_router_to_bytearray(sub_tlv)
            _type = sub_tlv.stable | ((2 & 0x7F) << 1)

        elif isinstance(sub_tlv, network_data.LowpanId):
            value = convert_lowpan_id_to_bytearray(sub_tlv)
            _type = sub_tlv.stable | ((3 & 0x7F) << 1)

        else:
            raise ValueError

        data += bytearray([_type, len(value)]) + value

    return data


def convert_server_to_bytearray(server):
    return struct.pack(">H", server.server_16) + server.server_data


def convert_service_sub_tlvs_to_bytearray(sub_tlvs):
    data = bytearray()

    for sub_tlv in sub_tlvs:
        if isinstance(sub_tlv, network_data.Server):
            value = convert_server_to_bytearray(sub_tlv)
            _type = sub_tlv.stable | ((6 & 0x7F) << 1)

        else:
            raise ValueError

        data += bytearray([_type, len(value)]) + value

    return data


def convert_service_to_bytearray(service):
    return (struct.pack(
        ">BLB",
        ((service.t & 0x01) << 7) | ((service.id) & 0x0F),
        service.enterprise_number,
        service.service_data_length,
    ) + service.service_data + convert_service_sub_tlvs_to_bytearray(service.sub_tlvs))


def any_border_router_16():
    return random.getrandbits(16)


def any_prf():
    return random.getrandbits(2)


def any_route():
    return network_data.Route(any_border_router_16(), any_prf())


def any_routes(count=None):
    if count is None:
        count = random.randint(0, 16)

    return [any_route() for _ in range(6)]


def any_has_route():
    return network_data.HasRoute(any_routes(), any_stable())


def any_domain_id():
    return random.getrandbits(8)


def any_prefix_length():
    return random.randint(1, 16)


def any_prefix(prefix_length=None):
    if prefix_length is None:
        prefix_length = any_prefix_length()

    return bytearray([random.getrandbits(8) for _ in range(int(math.ceil(prefix_length / 8)))])


def any_p():
    return random.getrandbits(1)


def any_s():
    return random.getrandbits(1)


def any_d():
    return random.getrandbits(1)


def any_c():
    return random.getrandbits(1)


def any_r():
    return random.getrandbits(1)


def any_o():
    return random.getrandbits(1)


def any_n():
    return random.getrandbits(1)


def any_cid():
    return random.getrandbits(4)


def any_context_length():
    return random.getrandbits(8)


def any_border_router():
    return network_data.BorderRouter(
        any_border_router_16(),
        any_prf(),
        any_p(),
        any_s(),
        any_d(),
        any_c(),
        any_r(),
        any_o(),
        any_n(),
        any_stable(),
    )


def any_lowpan_id():
    return network_data.LowpanId(any_c(), any_cid(), any_context_length(), any_stable())


def any_prefix_sub_tlvs():
    creator = [any_has_route, any_border_router, any_lowpan_id]

    sub_tlvs = []

    for _id in range(random.randint(1, 1)):
        c = random.choice(creator)
        sub_tlvs.append(c())

    return sub_tlvs


def any_t():
    return random.getrandbits(1)


def any_id():
    return random.getrandbits(4)


def any_enterprise_number():
    return random.getrandbits(32)


def any_service_data_length():
    return random.getrandbits(8)


def any_service_data(data_length=None):
    if data_length is None:
        data_length = random.randint(0, 16)

    return bytearray([random.getrandbits(8) for _ in range(data_length)])


def any_server_16():
    return random.getrandbits(16)


def any_server_data(data_length=None):
    if data_length is None:
        data_length = random.randint(0, 32)

    return bytearray([random.getrandbits(8) for _ in range(data_length)])


def any_server():
    return network_data.Server(any_server_16(), any_server_data(), any_stable())


def any_service_sub_tlvs():
    creator = [any_server]

    sub_tlvs = []

    for _id in range(random.randint(0, 16)):
        c = random.choice(creator)
        sub_tlvs.append(c())

    return sub_tlvs


def any_stable():
    return random.getrandbits(1)


class TestRoute(unittest.TestCase):

    def test_should_return_border_router_16_value_when_border_router_16_property_is_called(self):
        # GIVEN
        border_router_16 = any_border_router_16()

        route = network_data.Route(border_router_16, any_prf())

        # WHEN
        actual_border_router_16 = route.border_router_16

        # THEN
        self.assertEqual(border_router_16, actual_border_router_16)

    def test_should_return_prf_value_when_prf_property_is_called(self):
        # GIVEN
        prf = any_prf()

        route = network_data.Route(any_border_router_16(), prf)

        # WHEN
        actual_prf = route.prf

        # THEN
        self.assertEqual(prf, actual_prf)


class TestRouteFactory(unittest.TestCase):

    def test_should_create_Route_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        border_router_16 = any_border_router_16()
        prf = any_prf()

        factory = network_data.RouteFactory()

        data = convert_route_to_bytearray(network_data.Route(border_router_16, prf))

        # WHEN
        actual_route = factory.parse(io.BytesIO(data), None)

        # THEN
        self.assertTrue(isinstance(actual_route, network_data.Route))
        self.assertEqual(border_router_16, actual_route.border_router_16)
        self.assertEqual(prf, actual_route.prf)


class TestRoutesFactory(unittest.TestCase):

    def test_should_create_Route_list_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        routes = any_routes()

        factory = network_data.RoutesFactory(network_data.RouteFactory())

        data = convert_routes_to_bytearray(routes)
        # WHEN
        actual_routes = factory.parse(io.BytesIO(data), None)

        # THEN
        self.assertTrue(isinstance(actual_routes, list))
        self.assertEqual(routes, actual_routes)


class TestHasRoute(unittest.TestCase):

    def test_should_return_routes_value_when_routes_property_is_called(self):
        # GIVEN
        routes = any_routes()

        has_route = network_data.HasRoute(routes, any_stable())

        # WHEN
        actual_routes = has_route.routes

        # THEN
        self.assertEqual(routes, actual_routes)

    def test_should_return_stable_value_when_stable_property_is_called(self):
        # GIVEN
        stable = any_stable()

        has_route = network_data.HasRoute(any_routes(), stable)

        # WHEN
        actual_stable = has_route.stable

        # THEN
        self.assertEqual(stable, actual_stable)


class TestHasRouteFactory(unittest.TestCase):

    def test_should_create_HasRoute_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        routes = any_routes()
        stable = any_stable()

        factory = network_data.HasRouteFactory(network_data.RoutesFactory(network_data.RouteFactory()))

        data = convert_routes_to_bytearray(routes)

        message_info = common.MessageInfo()
        message_info.stable = stable

        # WHEN
        actual_has_route = factory.parse(io.BytesIO(data), message_info)

        # THEN
        self.assertTrue(isinstance(actual_has_route, network_data.HasRoute))
        self.assertEqual(routes, actual_has_route.routes)
        self.assertEqual(stable, actual_has_route.stable)


class TestPrefix(unittest.TestCase):

    def test_should_return_domain_id_value_when_domain_id_property_is_called(self):
        # GIVEN
        domain_id = any_domain_id()

        prefix = network_data.Prefix(
            domain_id,
            any_prefix_length(),
            any_prefix(),
            any_prefix_sub_tlvs(),
            any_stable(),
        )

        # WHEN
        actual_domain_id = prefix.domain_id

        # THEN
        self.assertEqual(domain_id, actual_domain_id)

    def test_should_return_prefix_length_value_when_prefix_length_property_is_called(self):
        # GIVEN
        prefix_length = any_prefix_length()

        prefix = network_data.Prefix(
            any_domain_id(),
            prefix_length,
            any_prefix(),
            any_prefix_sub_tlvs(),
            any_stable(),
        )

        # WHEN
        actual_prefix_length = prefix.prefix_length

        # THEN
        self.assertEqual(prefix_length, actual_prefix_length)

    def test_should_return_prefix_value_when_prefix_property_is_called(self):
        # GIVEN
        prefix = any_prefix()

        prefix_obj = network_data.Prefix(
            any_domain_id(),
            any_prefix_length(),
            prefix,
            any_prefix_sub_tlvs(),
            any_stable(),
        )

        # WHEN
        actual_prefix = prefix_obj.prefix

        # THEN
        self.assertEqual(prefix, actual_prefix)

    def test_should_return_sub_tlvs_value_when_sub_tlvs_property_is_called(self):
        # GIVEN
        sub_tlvs = any_prefix_sub_tlvs()

        prefix_obj = network_data.Prefix(
            any_domain_id(),
            any_prefix_length(),
            any_prefix(),
            sub_tlvs,
            any_stable(),
        )

        # WHEN
        actual_sub_tlvs = prefix_obj.sub_tlvs

        # THEN
        self.assertEqual(sub_tlvs, actual_sub_tlvs)

    def test_should_return_stable_value_when_stable_property_is_called(self):
        # GIVEN
        stable = any_stable()

        prefix_obj = network_data.Prefix(
            any_domain_id(),
            any_prefix_length(),
            any_prefix(),
            any_prefix_sub_tlvs(),
            stable,
        )

        # WHEN
        actual_stable = prefix_obj.stable

        # THEN
        self.assertEqual(stable, actual_stable)


class TestPrefixSubTlvsFactory(unittest.TestCase):

    def test_should_create_SubTlvs_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        sub_tlvs = any_prefix_sub_tlvs()

        factory = network_data.PrefixSubTlvsFactory(config.create_default_network_data_prefix_sub_tlvs_factories())

        data = convert_prefix_sub_tlvs_to_bytearray(sub_tlvs)

        # WHEN
        actual_sub_tlvs = factory.parse(io.BytesIO(data), common.MessageInfo())

        # THEN
        self.assertTrue(isinstance(actual_sub_tlvs, list))
        self.assertEqual(sub_tlvs, actual_sub_tlvs)


class TestPrefixFactory(unittest.TestCase):

    def test_should_create_Prefix_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        domain_id = any_domain_id()
        prefix_length = any_prefix_length()
        prefix = any_prefix(prefix_length)
        sub_tlvs = any_prefix_sub_tlvs()

        factory = network_data.PrefixFactory(config.create_default_network_data_prefix_sub_tlvs_factory())

        data = (bytearray([domain_id, prefix_length]) + prefix + convert_prefix_sub_tlvs_to_bytearray(sub_tlvs))

        message_info = common.MessageInfo()

        # WHEN
        actual_prefix = factory.parse(io.BytesIO(data), message_info)

        # THEN
        self.assertTrue(isinstance(actual_prefix, network_data.Prefix))
        self.assertEqual(domain_id, actual_prefix.domain_id)
        self.assertEqual(prefix_length, actual_prefix.prefix_length)
        self.assertEqual(prefix, actual_prefix.prefix)
        self.assertEqual(sub_tlvs, actual_prefix.sub_tlvs)


class TestBorderRouter(unittest.TestCase):

    def test_should_return_border_router_16_value_when_border_router_16_property_is_called(self):
        # GIVEN
        border_router_16 = any_border_router_16()

        border_router = network_data.BorderRouter(
            border_router_16,
            any_prf(),
            any_p(),
            any_s(),
            any_d(),
            any_c(),
            any_r(),
            any_o(),
            any_n(),
            any_stable(),
        )

        # WHEN
        actual_border_router_16 = border_router.border_router_16

        # THEN
        self.assertEqual(border_router_16, actual_border_router_16)

    def test_should_return_prf_value_when_prf_property_is_called(self):
        # GIVEN
        prf = any_prf()

        border_router = network_data.BorderRouter(
            any_border_router_16(),
            prf,
            any_p(),
            any_s(),
            any_d(),
            any_c(),
            any_r(),
            any_o(),
            any_n(),
            any_stable(),
        )

        # WHEN
        actual_prf = border_router.prf

        # THEN
        self.assertEqual(prf, actual_prf)

    def test_should_return_p_value_when_p_property_is_called(self):
        # GIVEN
        p = any_p()

        border_router = network_data.BorderRouter(
            any_border_router_16(),
            any_prf(),
            p,
            any_s(),
            any_d(),
            any_c(),
            any_r(),
            any_o(),
            any_n(),
            any_stable(),
        )

        # WHEN
        actual_p = border_router.p

        # THEN
        self.assertEqual(p, actual_p)

    def test_should_return_s_value_when_s_property_is_called(self):
        # GIVEN
        s = any_s()

        border_router = network_data.BorderRouter(
            any_border_router_16(),
            any_prf(),
            any_p(),
            s,
            any_d(),
            any_c(),
            any_r(),
            any_o(),
            any_n(),
            any_stable(),
        )

        # WHEN
        actual_s = border_router.s

        # THEN
        self.assertEqual(s, actual_s)

    def test_should_return_d_value_when_d_property_is_called(self):
        # GIVEN
        d = any_d()

        border_router = network_data.BorderRouter(
            any_border_router_16(),
            any_prf(),
            any_p(),
            any_s(),
            d,
            any_c(),
            any_r(),
            any_o(),
            any_n(),
            any_stable(),
        )

        # WHEN
        actual_d = border_router.d

        # THEN
        self.assertEqual(d, actual_d)

    def test_should_return_c_value_when_c_property_is_called(self):
        # GIVEN
        c = any_c()

        border_router = network_data.BorderRouter(
            any_border_router_16(),
            any_prf(),
            any_p(),
            any_s(),
            any_d(),
            c,
            any_r(),
            any_o(),
            any_n(),
            any_stable(),
        )

        # WHEN
        actual_c = border_router.c

        # THEN
        self.assertEqual(c, actual_c)

    def test_should_return_r_value_when_r_property_is_called(self):
        # GIVEN
        r = any_r()

        border_router = network_data.BorderRouter(
            any_border_router_16(),
            any_prf(),
            any_p(),
            any_s(),
            any_d(),
            any_c(),
            r,
            any_o(),
            any_n(),
            any_stable(),
        )

        # WHEN
        actual_r = border_router.r

        # THEN
        self.assertEqual(r, actual_r)

    def test_should_return_o_value_when_o_property_is_called(self):
        # GIVEN
        o = any_o()

        border_router = network_data.BorderRouter(
            any_border_router_16(),
            any_prf(),
            any_p(),
            any_s(),
            any_d(),
            any_c(),
            any_r(),
            o,
            any_n(),
            any_stable(),
        )

        # WHEN
        actual_o = border_router.o

        # THEN
        self.assertEqual(o, actual_o)

    def test_should_return_n_value_when_n_property_is_called(self):
        # GIVEN
        n = any_n()

        border_router = network_data.BorderRouter(
            any_border_router_16(),
            any_prf(),
            any_p(),
            any_s(),
            any_d(),
            any_c(),
            any_r(),
            any_o(),
            n,
            any_stable(),
        )

        # WHEN
        actual_n = border_router.n

        # THEN
        self.assertEqual(n, actual_n)

    def test_should_return_stable_value_when_stable_property_is_called(self):
        # GIVEN
        stable = any_stable()

        border_router = network_data.BorderRouter(
            any_border_router_16(),
            any_prf(),
            any_p(),
            any_s(),
            any_d(),
            any_c(),
            any_r(),
            any_o(),
            any_n(),
            stable,
        )

        # WHEN
        actual_stable = border_router.stable

        # THEN
        self.assertEqual(stable, actual_stable)


class TestBorderRouterFactory(unittest.TestCase):

    def test_should_create_BorderRouter_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        border_router_16 = any_border_router_16()
        prf = any_prf()
        p = any_p()
        s = any_s()
        d = any_d()
        c = any_c()
        r = any_r()
        o = any_o()
        n = any_n()
        stable = any_stable()

        factory = network_data.BorderRouterFactory()

        data = convert_border_router_to_bytearray(
            network_data.BorderRouter(border_router_16, prf, p, s, d, c, r, o, n, stable))

        message_info = common.MessageInfo()
        message_info.stable = stable

        # WHEN
        actual_border_router = factory.parse(io.BytesIO(data), message_info)

        # THEN
        self.assertTrue(isinstance(actual_border_router, network_data.BorderRouter))
        self.assertEqual(border_router_16, actual_border_router.border_router_16)
        self.assertEqual(prf, actual_border_router.prf)
        self.assertEqual(p, actual_border_router.p)
        self.assertEqual(s, actual_border_router.s)
        self.assertEqual(d, actual_border_router.d)
        self.assertEqual(c, actual_border_router.c)
        self.assertEqual(r, actual_border_router.r)
        self.assertEqual(o, actual_border_router.o)
        self.assertEqual(n, actual_border_router.n)
        self.assertEqual(stable, actual_border_router.stable)


class TestLowpanId(unittest.TestCase):

    def test_should_return_c_value_when_c_property_is_called(self):
        # GIVEN
        c = any_c()

        lowpan_id = network_data.LowpanId(c, any_cid(), any_context_length(), any_stable())

        # WHEN
        actual_c = lowpan_id.c

        # THEN
        self.assertEqual(c, actual_c)

    def test_should_return_cid_value_when_cid_property_is_called(self):
        # GIVEN
        cid = any_cid()

        lowpan_id = network_data.LowpanId(any_c(), cid, any_context_length(), any_stable())

        # WHEN
        actual_cid = lowpan_id.cid

        # THEN
        self.assertEqual(cid, actual_cid)

    def test_should_return_context_length_value_when_context_length_property_is_called(self):
        # GIVEN
        context_length = any_context_length()

        lowpan_id = network_data.LowpanId(any_c(), any_cid(), context_length, any_stable())

        # WHEN
        actual_context_length = lowpan_id.context_length

        # THEN
        self.assertEqual(context_length, actual_context_length)

    def test_should_return_stable_value_when_stable_property_is_called(self):
        # GIVEN
        stable = any_stable()

        lowpan_id = network_data.LowpanId(any_c(), any_cid(), any_context_length(), stable)

        # WHEN
        actual_stable = lowpan_id.stable

        # THEN
        self.assertEqual(stable, actual_stable)


class TestLowpanIdFactory(unittest.TestCase):

    def test_should_create_LowpanId_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        c = any_c()
        cid = any_cid()
        context_length = any_context_length()
        stable = any_stable()

        factory = network_data.LowpanIdFactory()

        data = convert_lowpan_id_to_bytearray(network_data.LowpanId(c, cid, context_length, stable))

        message_info = common.MessageInfo()
        message_info.stable = stable

        # WHEN
        actual_lowpan_id = factory.parse(io.BytesIO(data), message_info)

        # THEN
        self.assertTrue(isinstance(actual_lowpan_id, network_data.LowpanId))
        self.assertEqual(c, actual_lowpan_id.c)
        self.assertEqual(cid, actual_lowpan_id.cid)
        self.assertEqual(context_length, actual_lowpan_id.context_length)


class TestService(unittest.TestCase):

    def test_should_return_t_value_when_t_property_is_called(self):
        # GIVEN
        t = any_t()

        service = network_data.Service(
            t,
            any_id(),
            any_enterprise_number(),
            any_service_data_length(),
            any_service_data(),
            any_service_sub_tlvs(),
            any_stable(),
        )

        # WHEN
        actual_t = service.t

        # THEN
        self.assertEqual(t, actual_t)

    def test_should_return_id_value_when_id_property_is_called(self):
        # GIVEN
        _id = any_id()

        service = network_data.Service(
            any_t(),
            _id,
            any_enterprise_number(),
            any_service_data_length(),
            any_service_data(),
            any_service_sub_tlvs(),
            any_stable(),
        )

        # WHEN
        actual_id = service.id

        # THEN
        self.assertEqual(_id, actual_id)

    def test_should_return_enterprise_number_value_when_enterprise_number_property_is_called(self):
        # GIVEN
        enterprise_number = any_enterprise_number()

        service = network_data.Service(
            any_t(),
            any_id(),
            enterprise_number,
            any_service_data_length(),
            any_service_data(),
            any_service_sub_tlvs(),
            any_stable(),
        )

        # WHEN
        actual_enterprise_number = service.enterprise_number

        # THEN
        self.assertEqual(enterprise_number, actual_enterprise_number)

    def test_should_return_service_data_length_value_when_service_data_length_property_is_called(self):
        # GIVEN
        service_data_length = any_service_data_length()

        service = network_data.Service(
            any_t(),
            any_id(),
            any_enterprise_number(),
            service_data_length,
            any_service_data(),
            any_service_sub_tlvs(),
            any_stable(),
        )

        # WHEN
        actual_service_data_length = service.service_data_length

        # THEN
        self.assertEqual(service_data_length, actual_service_data_length)

    def test_should_return_service_data_value_when_service_data_property_is_called(self):
        # GIVEN
        service_data = any_service_data()

        service = network_data.Service(
            any_t(),
            any_id(),
            any_enterprise_number(),
            any_service_data_length(),
            service_data,
            any_service_sub_tlvs(),
            any_stable(),
        )

        # WHEN
        actual_service_data = service.service_data

        # THEN
        self.assertEqual(service_data, actual_service_data)

    def test_should_return_sub_tlvs_value_when_sub_tlvs_property_is_called(self):
        # GIVEN
        sub_tlvs = any_service_sub_tlvs()

        service = network_data.Service(
            any_t(),
            any_id(),
            any_enterprise_number(),
            any_service_data_length(),
            any_service_data(),
            sub_tlvs,
            any_stable(),
        )

        # WHEN
        actual_sub_tlvs = service.sub_tlvs

        # THEN
        self.assertEqual(sub_tlvs, actual_sub_tlvs)

    def test_should_return_stable_value_when_stable_property_is_called(self):
        # GIVEN
        stable = any_stable()

        service = network_data.Service(
            any_t(),
            any_id(),
            any_enterprise_number(),
            any_service_data_length(),
            any_service_data(),
            any_service_sub_tlvs(),
            stable,
        )

        # WHEN
        actual_stable = service.stable

        # THEN
        self.assertEqual(stable, actual_stable)


class TestServiceSubTlvsFactory(unittest.TestCase):

    def test_should_create_SubTlvs_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        sub_tlvs = any_service_sub_tlvs()

        factory = network_data.ServiceSubTlvsFactory(config.create_default_network_data_service_sub_tlvs_factories())

        data = convert_service_sub_tlvs_to_bytearray(sub_tlvs)

        # WHEN
        actual_sub_tlvs = factory.parse(io.BytesIO(data), common.MessageInfo())

        # THEN
        self.assertTrue(isinstance(actual_sub_tlvs, list))
        self.assertEqual(sub_tlvs, actual_sub_tlvs)


class TestServiceFactory(unittest.TestCase):

    def test_should_create_Service_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        t = any_t()
        _id = any_id()
        enterprise_number = any_enterprise_number()
        service_data_length = any_service_data_length()
        service_data = any_service_data(service_data_length)
        sub_tlvs = any_service_sub_tlvs()
        stable = any_stable()

        factory = network_data.ServiceFactory(config.create_default_network_data_service_sub_tlvs_factory())

        data = convert_service_to_bytearray(
            network_data.Service(
                t,
                _id,
                enterprise_number,
                service_data_length,
                service_data,
                sub_tlvs,
                stable,
            ))

        message_info = common.MessageInfo()
        message_info.stable = stable

        # WHEN
        actual_service = factory.parse(io.BytesIO(data), message_info)

        # THEN
        self.assertTrue(isinstance(actual_service, network_data.Service))
        self.assertEqual(t, actual_service.t)
        self.assertEqual(_id, actual_service.id)
        self.assertEqual(enterprise_number, actual_service.enterprise_number)
        self.assertEqual(service_data_length, actual_service.service_data_length)
        self.assertEqual(service_data, actual_service.service_data)
        self.assertEqual(sub_tlvs, actual_service.sub_tlvs)


class TestServer(unittest.TestCase):

    def test_should_return_server_16_value_when_server_16_property_is_called(self):
        # GIVEN
        server_16 = any_server_16()

        server = network_data.Server(server_16, any_server_data(), any_stable())

        # WHEN
        actual_server_16 = server.server_16

        # THEN
        self.assertEqual(server_16, actual_server_16)

    def test_should_return_server_data_value_when_server_data_property_is_called(self):
        # GIVEN
        server_data = any_server_data()

        server = network_data.Server(any_server_16(), server_data, any_stable())

        # WHEN
        actual_server_data = server.server_data

        # THEN
        self.assertEqual(server_data, actual_server_data)

    def test_should_return_stable_value_when_stable_property_is_called(self):
        # GIVEN
        stable = any_stable()

        server = network_data.Server(any_server_16(), any_server_data(), stable)

        # WHEN
        actual_stable = server.stable

        # THEN
        self.assertEqual(stable, actual_stable)


class TestServerFactory(unittest.TestCase):

    def test_should_create_Server_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        server_16 = any_server_16()
        server_data = any_server_data()
        stable = any_stable()

        factory = network_data.ServerFactory()

        data = convert_server_to_bytearray(network_data.Server(server_16, server_data, stable))

        message_info = common.MessageInfo()
        message_info.stable = stable

        # WHEN
        actual_server = factory.parse(io.BytesIO(data), message_info)

        # THEN
        self.assertTrue(isinstance(actual_server, network_data.Server))
        self.assertEqual(server_16, actual_server.server_16)
        self.assertEqual(server_data, actual_server.server_data)
        self.assertEqual(stable, actual_server.stable)


if __name__ == "__main__":
    unittest.main()
