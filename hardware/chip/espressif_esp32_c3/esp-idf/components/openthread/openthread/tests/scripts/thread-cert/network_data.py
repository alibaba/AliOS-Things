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
import struct

from binascii import hexlify
from enum import IntEnum
from tlvs_parsing import SubTlvsFactory

import common


class TlvType(IntEnum):
    HAS_ROUTE = 0
    PREFIX = 1
    BORDER_ROUTER = 2
    LOWPAN_ID = 3
    COMMISSIONING = 4
    SERVICE = 5
    SERVER = 6


class NetworkData(object):

    def __init__(self, stable):
        self._stable = stable

    @property
    def stable(self):
        return self._stable


class NetworkDataSubTlvsFactory(SubTlvsFactory):

    def parse(self, data, message_info):
        sub_tlvs = []

        while data.tell() < len(data.getvalue()):
            data_byte = ord(data.read(1))

            stable = data_byte & 0x01
            _type = (data_byte >> 1) & 0x7F

            length = ord(data.read(1))
            value = data.read(length)

            factory = self._get_factory(_type)

            message_info.stable = stable
            tlv = factory.parse(io.BytesIO(value), message_info)

            sub_tlvs.append(tlv)

        return sub_tlvs


class Route(object):

    def __init__(self, border_router_16, prf):
        self._border_router_16 = border_router_16
        self._prf = prf

    @property
    def border_router_16(self):
        return self._border_router_16

    @property
    def prf(self):
        return self._prf

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return (self.border_router_16 == other.border_router_16 and self.prf == other.prf)

    def __repr__(self):
        return "Route(border_router_16={}, prf={})".format(self.border_router_16, self.prf)


class RouteFactory(object):

    def parse(self, data, message_info):
        border_router_16 = struct.unpack(">H", data.read(2))[0]

        data_byte = ord(data.read(1))
        prf = (data_byte >> 6) & 0x03

        return Route(border_router_16, prf)


class RoutesFactory(object):

    def __init__(self, route_factory):
        self._route_factory = route_factory

    def parse(self, data, message_info):
        routes = []

        while data.tell() < len(data.getvalue()):
            route = self._route_factory.parse(data, message_info)

            routes.append(route)

        return routes


class HasRoute(NetworkData):

    def __init__(self, routes, stable):
        super(HasRoute, self).__init__(stable)
        self._routes = routes

    @property
    def routes(self):
        return self._routes

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return self.routes == other.routes

    def __repr__(self):
        routes_str = ", ".join(["{}".format(route) for route in self.routes])
        return "HasRoute(stable={}, routes=[{}])".format(self.stable, routes_str)


class HasRouteFactory(object):

    def __init__(self, routes_factory):
        self._routes_factory = routes_factory

    def parse(self, data, message_info):
        routes = self._routes_factory.parse(data, message_info)

        return HasRoute(routes, message_info.stable)


class Prefix(NetworkData):

    def __init__(self, domain_id, prefix_length, prefix, sub_tlvs, stable):
        super(Prefix, self).__init__(stable)
        self._domain_id = domain_id
        self._prefix_length = prefix_length
        self._prefix = prefix
        self._sub_tlvs = sub_tlvs

    @property
    def domain_id(self):
        return self._domain_id

    @property
    def prefix_length(self):
        return self._prefix_length

    @property
    def prefix(self):
        return self._prefix

    @property
    def sub_tlvs(self):
        return self._sub_tlvs

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return (self.domain_id == other.domain_id and self.prefix_length == other.prefix_length and
                self.prefix == other.prefix and self.sub_tlvs == other.sub_tlvs)

    def __repr__(self):
        sub_tlvs_str = ", ".join(["{}".format(tlv) for tlv in self.sub_tlvs])
        return "Prefix(stable={}, domain_id={}, prefix_length={}, prefix={}, sub_tlvs=[{}])".format(
            self.stable,
            self.domain_id,
            self.prefix_length,
            hexlify(self.prefix),
            sub_tlvs_str,
        )


class PrefixSubTlvsFactory(NetworkDataSubTlvsFactory):

    def __init__(self, sub_tlvs_factories):
        super(PrefixSubTlvsFactory, self).__init__(sub_tlvs_factories)


class PrefixFactory(object):

    def __init__(self, sub_tlvs_factory):
        self._sub_tlvs_factory = sub_tlvs_factory

    def _bits_to_bytes(self, bits):
        return int(math.ceil(bits / 8))

    def parse(self, data, message_info):
        domain_id = ord(data.read(1))

        prefix_length = ord(data.read(1))

        prefix = bytearray(data.read(self._bits_to_bytes(prefix_length)))

        sub_tlvs = self._sub_tlvs_factory.parse(io.BytesIO(data.read()), message_info)

        return Prefix(domain_id, prefix_length, prefix, sub_tlvs, message_info.stable)


class BorderRouter(NetworkData):

    def __init__(self, border_router_16, prf, p, s, d, c, r, o, n, stable):
        super(BorderRouter, self).__init__(stable)
        self._border_router_16 = border_router_16
        self._prf = prf
        self._p = p
        self._s = s
        self._d = d
        self._c = c
        self._r = r
        self._o = o
        self._n = n

    @property
    def border_router_16(self):
        return self._border_router_16

    @property
    def prf(self):
        return self._prf

    @property
    def p(self):
        return self._p

    @property
    def s(self):
        return self._s

    @property
    def d(self):
        return self._d

    @property
    def c(self):
        return self._c

    @property
    def r(self):
        return self._r

    @property
    def o(self):
        return self._o

    @property
    def n(self):
        return self._n

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return (self.border_router_16 == other.border_router_16 and self.prf == other.prf and self.p == other.p and
                self.s == other.s and self.d == other.d and self.c == other.c and self.r == other.r and
                self.o == other.o and self.n == other.n)

    def __repr__(self):
        return "BorderRouter(stable={}, border_router_16={}, prf={}, p={}, s={}, d={}, c={}, r={}, o={}, n={})".format(
            self.stable,
            self.border_router_16,
            self.prf,
            self.p,
            self.s,
            self.d,
            self.c,
            self.r,
            self.o,
            self.n,
        )


class BorderRouterFactory(object):

    def parse(self, data, message_info):
        border_router_16 = struct.unpack(">H", data.read(2))[0]

        data_byte = ord(data.read(1))
        o = data_byte & 0x01
        r = (data_byte >> 1) & 0x01
        c = (data_byte >> 2) & 0x01
        d = (data_byte >> 3) & 0x01
        s = (data_byte >> 4) & 0x01
        p = (data_byte >> 5) & 0x01
        prf = (data_byte >> 6) & 0x03

        data_byte = ord(data.read(1))
        n = (data_byte >> 7) & 0x01

        return BorderRouter(border_router_16, prf, p, s, d, c, r, o, n, message_info.stable)


class LowpanId(NetworkData):

    def __init__(self, c, cid, context_length, stable):
        super(LowpanId, self).__init__(stable)
        self._c = c
        self._cid = cid
        self._context_length = context_length

    @property
    def c(self):
        return self._c

    @property
    def cid(self):
        return self._cid

    @property
    def context_length(self):
        return self._context_length

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return (self.c == other.c and self.cid == other.cid and self.context_length == other.context_length)

    def __repr__(self):
        return "LowpanId(stable={}, c={}, cid={}, context_length={})".format(self.stable, self.c, self.cid,
                                                                             self.context_length)


class LowpanIdFactory(object):

    def parse(self, data, message_info):
        data_byte = ord(data.read(1))

        cid = data_byte & 0x0F
        c = (data_byte >> 4) & 0x01

        context_length = ord(data.read(1))

        return LowpanId(c, cid, context_length, message_info.stable)


class CommissioningData(NetworkData):

    def __init__(self, sub_tlvs, stable):
        super(CommissioningData, self).__init__(stable)
        self._sub_tlvs = sub_tlvs

    @property
    def sub_tlvs(self):
        return self._sub_tlvs

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return self.sub_tlvs == other.sub_tlvs

    def __repr__(self):
        sub_tlvs_str = ", ".join(["{}".format(tlv) for tlv in self._sub_tlvs])
        return "CommissioningData(stable={}, sub_tlvs=[{}])".format(self._stable, sub_tlvs_str)


class CommissioningDataSubTlvsFactory(SubTlvsFactory):

    def __init__(self, sub_tlvs_factories):
        super(CommissioningDataSubTlvsFactory, self).__init__(sub_tlvs_factories)


class CommissioningDataFactory(object):

    def __init__(self, sub_tlvs_factory):
        self._sub_tlvs_factory = sub_tlvs_factory

    def parse(self, data, message_info):
        sub_tlvs = self._sub_tlvs_factory.parse(io.BytesIO(data.read()), message_info)

        return CommissioningData(sub_tlvs, message_info.stable)


class Service(NetworkData):

    def __init__(
        self,
        t,
        _id,
        enterprise_number,
        service_data_length,
        service_data,
        sub_tlvs,
        stable,
    ):
        super(Service, self).__init__(stable)
        self._t = t
        self._id = _id
        self._enterprise_number = enterprise_number
        self._service_data_length = service_data_length
        self._service_data = service_data
        self._sub_tlvs = sub_tlvs

    @property
    def t(self):
        return self._t

    @property
    def id(self):
        return self._id

    @property
    def enterprise_number(self):
        return self._enterprise_number

    @property
    def service_data_length(self):
        return self._service_data_length

    @property
    def service_data(self):
        return self._service_data

    @property
    def sub_tlvs(self):
        return self._sub_tlvs

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return (self.t == other.t and self.id == other.id and self.enterprise_number == other.enterprise_number and
                self.service_data_length == other.service_data_length and self.service_data == other.service_data and
                self.sub_tlvs == other.sub_tlvs)

    def __repr__(self):
        sub_tlvs_str = ", ".join(["{}".format(tlv) for tlv in self.sub_tlvs])
        return (
            "LowpanId(stable={}, t={}, id={}, enterprise_number={}, service_data_length={}, service_data={}, sub_tlvs=[{}])"
        ).format(
            self.stable,
            self.t,
            self.id,
            self.enterprise_number,
            self.service_data_length,
            self.service_data,
            sub_tlvs_str,
        )


class ServiceSubTlvsFactory(NetworkDataSubTlvsFactory):

    def __init__(self, sub_tlvs_factories):
        super(ServiceSubTlvsFactory, self).__init__(sub_tlvs_factories)


class ServiceFactory(object):

    def __init__(self, sub_tlvs_factory):
        self._sub_tlvs_factory = sub_tlvs_factory

    def parse(self, data, message_info):
        data_byte = ord(data.read(1))
        t = (data_byte >> 7) & 0x01
        _id = data_byte & 0x0F

        enterprise_number = struct.unpack(">L", data.read(4))[0]
        service_data_length = ord(data.read(1))
        service_data = data.read(service_data_length)

        sub_tlvs = self._sub_tlvs_factory.parse(io.BytesIO(data.read()), message_info)

        return Service(
            t,
            _id,
            enterprise_number,
            service_data_length,
            service_data,
            sub_tlvs,
            message_info.stable,
        )


class Server(NetworkData):

    def __init__(self, server_16, server_data, stable):
        super(Server, self).__init__(stable)
        self._server_16 = server_16
        self._server_data = server_data

    @property
    def server_16(self):
        return self._server_16

    @property
    def server_data(self):
        return self._server_data

    def __eq__(self, other):
        common.expect_the_same_class(self, other)

        return (self.server_16 == other.server_16 and self.server_data == other.server_data)

    def __repr__(self):
        return "LowpanId(stable={}, server_16={}, server_data=b'{}')".format(self.stable, self.server_16,
                                                                             hexlify(self.server_data))


class ServerFactory(object):

    def parse(self, data, message_info):
        server_16 = struct.unpack(">H", data.read(2))[0]
        server_data = bytearray(data.read())

        return Server(server_16, server_data, message_info.stable)


class NetworkDataTlvsFactory(NetworkDataSubTlvsFactory):

    def __init__(self, sub_tlvs_factories):
        super(NetworkDataTlvsFactory, self).__init__(sub_tlvs_factories)
