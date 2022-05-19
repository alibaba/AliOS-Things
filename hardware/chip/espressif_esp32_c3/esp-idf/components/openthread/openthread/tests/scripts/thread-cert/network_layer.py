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

import ipaddress
import struct
from binascii import hexlify
from enum import IntEnum

import common


class TlvType(IntEnum):
    TARGET_EID = 0
    MAC_EXTENDED_ADDRESS = 1
    RLOC16 = 2
    ML_EID = 3
    STATUS = 4
    TIME_SINCE_LAST_TRANSACTION = 6
    ROUTER_MASK = 7
    ND_OPTION = 8
    ND_DATA = 9
    THREAD_NETWORK_DATA = 10
    MLE_ROUTING = 11
    IPv6_ADDRESSES = 14
    XTAL_ACCURACY = 254


class StatusValues(IntEnum):
    SUCCESS = 0
    NO_ADDRESS_AVAILABLE = 1
    TOO_FEW_ROUTERS = 2
    HAVE_CHILD_ID_REQUEST = 3
    PARENT_PARTITION_CHANGE = 4


class TargetEid(object):

    def __init__(self, eid):
        self._eid = eid

    @property
    def eid(self):
        return self._eid

    def __eq__(self, other):
        common.expect_the_same_class(self, other)
        return self.eid == other.eid

    def __repr__(self):
        return "TargetEid(eid={})".format(hexlify(self.eid))


class TargetEidFactory(object):

    def parse(self, data, message_info):
        eid = bytearray(data.read(16))

        return TargetEid(eid)


class MacExtendedAddress(object):

    def __init__(self, mac_address):
        self._mac_address = mac_address

    @property
    def mac_address(self):
        return self._mac_address

    def __eq__(self, other):
        common.expect_the_same_class(self, other)
        return self.mac_address == other.mac_address

    def __repr__(self):
        return "MacExtendedAddress(mac_address={})".format(hexlify(self.mac_address))


class MacExtendedAddressFactory(object):

    def parse(self, data, message_info):
        mac_address = bytearray(data.read(8))

        return MacExtendedAddress(mac_address)


class Rloc16(object):

    def __init__(self, rloc16):
        self._rloc16 = rloc16

    @property
    def rloc16(self):
        return self._rloc16

    def __eq__(self, other):
        common.expect_the_same_class(self, other)
        return self.rloc16 == other.rloc16

    def __repr__(self):
        return "Rloc16(rloc16={})".format(hex(self.rloc16))


class Rloc16Factory(object):

    def parse(self, data, message_info):
        rloc16 = struct.unpack(">H", data.read(2))[0]

        return Rloc16(rloc16)


class MlEid(object):

    def __init__(self, ml_eid):
        self._ml_eid = ml_eid

    @property
    def ml_eid(self):
        return self._ml_eid

    def __eq__(self, other):
        common.expect_the_same_class(self, other)
        return self.ml_eid == other.ml_eid

    def __repr__(self):
        return "MlEid(ml_eid={})".format(hexlify(self.ml_eid))


class MlEidFactory(object):

    def parse(self, data, message_info):
        ml_eid = bytearray(data.read(8))

        return MlEid(ml_eid)


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


class StatusFactory(object):

    def parse(self, data, message_info):
        status = StatusValues(ord(data.read(1)))

        return Status(status)


class TimeSinceLastTransaction(object):

    def __init__(self, seconds):
        self._seconds = seconds

    @property
    def seconds(self):
        return self._seconds

    def __eq__(self, other):
        common.expect_the_same_class(self, other)
        return self.seconds == other.seconds

    def __repr__(self):
        return "TimeSinceLastTransaction(seconds={})".format(self.seconds)


class TimeSinceLastTransactionFactory(object):

    def parse(self, data, message_info):
        seconds = struct.unpack(">L", data.read(4))[0]

        return TimeSinceLastTransaction(seconds)


class RouterMask(object):

    def __init__(self, id_sequence, router_id_mask):
        self._id_sequence = id_sequence
        self._router_id_mask = router_id_mask

    @property
    def id_sequence(self):
        return self._id_sequence

    @property
    def router_id_mask(self):
        return self._router_id_mask

    def __eq__(self, other):
        common.expect_the_same_class(self, other)
        return (self.id_sequence == other.id_sequence and self.router_id_mask == other.router_id_mask)

    def __repr__(self):
        return "RouterMask(id_sequence={}, router_id_mask={})".format(self.id_sequence, hex(self.router_id_mask))


class RouterMaskFactory(object):

    def parse(self, data, message_info):
        id_sequence = ord(data.read(1))
        router_id_mask = struct.unpack(">Q", data.read(8))[0]

        return RouterMask(id_sequence, router_id_mask)


class NdOption(object):

    def __init__(self, options):
        self._options = options

    @property
    def options(self):
        return self._options

    def __eq__(self, other):
        common.expect_the_same_class(self, other)
        return self.options == other.options

    def __repr__(self):
        return "NdOption(options=[{}])".format(", ".join([str(opt) for opt in self.options]))


class NdOptionFactory(object):

    def parse(self, data, message_info):
        options = [opt for opt in bytearray(data.read())]
        return NdOption(options)


class NdData(object):
    # TODO: Not implemented yet
    pass


class NdDataFactory(object):
    # TODO: Not implemented yet

    def parse(self, data, message_info):
        raise NotImplementedError("TODO: Not implemented yet")


class XtalAccuracy:
    # TODO: Not implemented yet

    def __init__(self):
        print("XtalAccuracy is not implemented yet.")


class XtalAccuracyFactory:

    def parse(self, data, message_info):
        return XtalAccuracy()


class ThreadNetworkData(object):

    def __init__(self, tlvs):
        self._tlvs = tlvs

    @property
    def tlvs(self):
        return self._tlvs

    def __eq__(self, other):
        common.expect_the_same_class(self, other)
        return self.tlvs == other.tlvs

    def __repr__(self):
        return "ThreadNetworkData(tlvs=[{}])".format(", ".join([str(tlv) for tlv in self.tlvs]))


class ThreadNetworkDataFactory(object):

    def __init__(self, network_data_tlvs_factory):
        self._network_data_tlvs_factory = network_data_tlvs_factory

    def parse(self, data, message_info):
        tlvs = self._network_data_tlvs_factory.parse(data, message_info)
        return ThreadNetworkData(tlvs)


class IPv6Addresses(list):
    pass


class IPv6AddressesFactory(object):

    def parse(self, data, message_info):
        data = bytes(data.read())
        assert len(data) % 16 == 0, data
        addrs = IPv6Addresses()
        for i in range(0, len(data), 16):
            addr = ipaddress.IPv6Address(data[i:i + 16])
            addrs.append(addr)

        return addrs
