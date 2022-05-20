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
import network_layer


def any_eid():
    return bytearray([random.getrandbits(8) for _ in range(16)])


def any_mac_extended_address():
    return bytearray([random.getrandbits(8) for _ in range(8)])


def any_rloc16():
    return random.getrandbits(16)


def any_ml_eid():
    return bytearray([random.getrandbits(8) for _ in range(8)])


def any_status():
    return random.getrandbits(1)


def any_seconds():
    return random.getrandbits(32)


def any_id_sequence():
    return random.getrandbits(8)


def any_router_id_mask():
    return random.getrandbits(64)


def any_options(count=None):
    count = count if count is not None else random.randint(0, 255)
    return [random.getrandbits(8) for _ in range(count)]


def any_tlv_data(length=None):
    _type = random.getrandbits(8)
    length = length if length is not None else random.getrandbits(8)
    value = bytearray([random.getrandbits(8) for _ in range(length)])

    return bytearray([_type, length]) + value


def any_tlvs_data(count=None):
    count = count if count is not None else random.randint(0, 16)
    data = bytearray()

    for _ in range(count):
        data += any_tlv_data(random.randint(1, 15))

    return data


class TestTargetEid(unittest.TestCase):

    def test_should_return_eid_value_when_eid_property_is_called(self):
        # GIVEN
        eid = any_eid()

        target_eid = network_layer.TargetEid(eid)

        # WHEN
        actual_eid = target_eid.eid

        # THEN
        self.assertEqual(eid, actual_eid)

    def test_should_return_True_when_try_to_equal_two_the_same_type_objects_with_the_same_values(self):
        # GIVEN
        eid = any_eid()

        target_eid = network_layer.TargetEid(eid)

        # THEN
        self.assertEqual(target_eid, network_layer.TargetEid(eid))


class TestTargetEidFactory(unittest.TestCase):

    def test_should_create_TargetEid_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        eid = any_eid()

        factory = network_layer.TargetEidFactory()

        # WHEN
        target_eid = factory.parse(io.BytesIO(eid), common.MessageInfo())

        # THEN
        self.assertTrue(isinstance(target_eid, network_layer.TargetEid))
        self.assertEqual(eid, target_eid.eid)


class TestMacExtendedAddress(unittest.TestCase):

    def test_should_return_mac_address_value_when_mac_address_property_is_called(self):
        # GIVEN
        mac_address = any_mac_extended_address()

        mac_extended_address = network_layer.MacExtendedAddress(mac_address)

        # WHEN
        actual_mac_address = mac_extended_address.mac_address

        # THEN
        self.assertEqual(mac_address, actual_mac_address)

    def test_should_return_True_when_try_to_equal_two_the_same_type_objects_with_the_same_values(self):
        # GIVEN
        mac_address = any_mac_extended_address()

        mac_extended_address = network_layer.MacExtendedAddress(mac_address)

        # THEN
        self.assertEqual(mac_extended_address, network_layer.MacExtendedAddress(mac_address))


class TestMacExtendedAddressFactory(unittest.TestCase):

    def test_should_create_MacExtendedAddress_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        mac_address = any_mac_extended_address()

        factory = network_layer.MacExtendedAddressFactory()

        # WHEN
        mac_extended_address = factory.parse(io.BytesIO(mac_address), common.MessageInfo())

        # THEN
        self.assertTrue(isinstance(mac_extended_address, network_layer.MacExtendedAddress))
        self.assertEqual(mac_address, mac_extended_address.mac_address)


class TestRloc16(unittest.TestCase):

    def test_should_return_rloc16_value_when_rloc16_property_is_called(self):
        # GIVEN
        rloc16 = any_rloc16()

        rloc16_obj = network_layer.Rloc16(rloc16)

        # WHEN
        actual_rloc16 = rloc16_obj.rloc16

        # THEN
        self.assertEqual(rloc16, actual_rloc16)

    def test_should_return_True_when_try_to_equal_two_the_same_type_objects_with_the_same_values(self):
        # GIVEN
        rloc16 = any_rloc16()

        rloc16_obj = network_layer.Rloc16(rloc16)

        # THEN
        self.assertEqual(rloc16_obj, network_layer.Rloc16(rloc16))


class TestRloc16Factory(unittest.TestCase):

    def test_should_create_Rloc16_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        rloc16 = any_rloc16()

        factory = network_layer.Rloc16Factory()

        data = bytearray(struct.pack(">H", rloc16))

        # WHEN
        rloc16_obj = factory.parse(io.BytesIO(data), common.MessageInfo())

        # THEN
        self.assertTrue(isinstance(rloc16_obj, network_layer.Rloc16))
        self.assertEqual(rloc16, rloc16_obj.rloc16)


class TestMlEid(unittest.TestCase):

    def test_should_return_ml_eid_value_when_ml_eid_property_is_called(self):
        # GIVEN
        ml_eid = any_ml_eid()

        ml_eid_obj = network_layer.MlEid(ml_eid)

        # WHEN
        actual_ml_eid = ml_eid_obj.ml_eid

        # THEN
        self.assertEqual(ml_eid, actual_ml_eid)

    def test_should_return_True_when_try_to_equal_two_the_same_type_objects_with_the_same_values(self):
        # GIVEN
        ml_eid = any_ml_eid()

        ml_eid_obj = network_layer.MlEid(ml_eid)

        # THEN
        self.assertEqual(ml_eid_obj, network_layer.MlEid(ml_eid))


class TestMlEidFactory(unittest.TestCase):

    def test_should_create_MlEid_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        ml_eid = any_ml_eid()

        factory = network_layer.MlEidFactory()

        # WHEN
        ml_eid_obj = factory.parse(io.BytesIO(ml_eid), common.MessageInfo())

        # THEN
        self.assertTrue(isinstance(ml_eid_obj, network_layer.MlEid))
        self.assertEqual(ml_eid, ml_eid_obj.ml_eid)


class TestStatus(unittest.TestCase):

    def test_should_return_status_value_when_status_property_is_called(self):
        # GIVEN
        status = any_status()

        status_obj = network_layer.Status(status)

        # WHEN
        actual_status = status_obj.status

        # THEN
        self.assertEqual(status, actual_status)

    def test_should_return_True_when_try_to_equal_two_the_same_type_objects_with_the_same_values(self):
        # GIVEN
        status = any_status()

        status_obj = network_layer.Status(status)

        # THEN
        self.assertEqual(status_obj, network_layer.Status(status))


class TestStatusFactory(unittest.TestCase):

    def test_should_create_Status_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        status = any_status()

        factory = network_layer.StatusFactory()

        data = bytearray([status])

        # WHEN
        status_obj = factory.parse(io.BytesIO(data), common.MessageInfo())

        # THEN
        self.assertTrue(isinstance(status_obj, network_layer.Status))
        self.assertEqual(status, status_obj.status)


class TestTimeSinceLastTransaction(unittest.TestCase):

    def test_should_return_seconds_value_when_seconds_property_is_called(self):
        # GIVEN
        seconds = any_seconds()

        time_since_last_transaction = network_layer.TimeSinceLastTransaction(seconds)

        # WHEN
        actual_seconds = time_since_last_transaction.seconds

        # THEN
        self.assertEqual(seconds, actual_seconds)

    def test_should_return_True_when_try_to_equal_two_the_same_type_objects_with_the_same_values(self):
        # GIVEN
        seconds = any_seconds()

        time_since_last_transaction = network_layer.TimeSinceLastTransaction(seconds)

        # THEN
        self.assertEqual(
            time_since_last_transaction,
            network_layer.TimeSinceLastTransaction(seconds),
        )


class TestTimeSinceLastTransactionFactory(unittest.TestCase):

    def test_should_create_TimeSinceLastTransaction_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        seconds = any_seconds()

        factory = network_layer.TimeSinceLastTransactionFactory()

        data = bytearray(struct.pack(">L", seconds))

        # WHEN
        time_since_last_transaction = factory.parse(io.BytesIO(data), common.MessageInfo())

        # THEN
        self.assertTrue(isinstance(
            time_since_last_transaction,
            network_layer.TimeSinceLastTransaction,
        ))
        self.assertEqual(seconds, time_since_last_transaction.seconds)


class TestRouterMask(unittest.TestCase):

    def test_should_return_id_sequence_value_when_id_sequence_property_is_called(self):
        # GIVEN
        id_sequence = any_id_sequence()

        router_mask = network_layer.RouterMask(id_sequence, any_router_id_mask())

        # WHEN
        actual_id_sequence = router_mask.id_sequence

        # THEN
        self.assertEqual(id_sequence, actual_id_sequence)

    def test_should_return_router_id_mask_value_when_router_id_mask_property_is_called(self):
        # GIVEN
        router_id_mask = any_router_id_mask()

        router_mask = network_layer.RouterMask(any_id_sequence(), router_id_mask)

        # WHEN
        actual_router_id_mask = router_mask.router_id_mask

        # THEN
        self.assertEqual(router_id_mask, actual_router_id_mask)

    def test_should_return_True_when_try_to_equal_two_the_same_type_objects_with_the_same_values(self):
        # GIVEN
        id_sequence = any_id_sequence()
        router_id_mask = any_router_id_mask()

        router_mask = network_layer.RouterMask(id_sequence, router_id_mask)

        # THEN
        self.assertEqual(router_mask, network_layer.RouterMask(id_sequence, router_id_mask))


class TestRouterMaskFactory(unittest.TestCase):

    def test_should_create_RouterMask_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        id_sequence = any_id_sequence()
        router_id_mask = any_router_id_mask()

        factory = network_layer.RouterMaskFactory()

        data = bytearray([id_sequence]) + struct.pack(">Q", router_id_mask)

        # WHEN
        router_mask = factory.parse(io.BytesIO(data), common.MessageInfo())

        # THEN
        self.assertTrue(isinstance(router_mask, network_layer.RouterMask))
        self.assertEqual(id_sequence, router_mask.id_sequence)
        self.assertEqual(router_id_mask, router_mask.router_id_mask)


class TestNdOption(unittest.TestCase):

    def test_should_return_options_value_when_options_property_is_called(self):
        # GIVEN
        options = any_options()

        nd_option = network_layer.NdOption(options)

        # WHEN
        actual_options = nd_option.options

        # THEN
        self.assertEqual(options, actual_options)

    def test_should_return_True_when_try_to_equal_two_the_same_type_objects_with_the_same_values(self):
        # GIVEN
        options = any_options()

        nd_option = network_layer.NdOption(options)

        # THEN
        self.assertEqual(nd_option, network_layer.NdOption(options))


class TestNdOptionFactory(unittest.TestCase):

    def test_should_create_NdOption_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        options = any_options()

        factory = network_layer.NdOptionFactory()

        data = bytearray(options)

        # WHEN
        nd_option = factory.parse(io.BytesIO(data), common.MessageInfo())

        # THEN
        self.assertTrue(isinstance(nd_option, network_layer.NdOption))
        self.assertEqual(options, nd_option.options)


class TestThreadNetworkData(unittest.TestCase):

    def test_should_return_options_value_when_options_property_is_called(self):
        # GIVEN
        tlvs = any_tlvs_data()

        thread_network_data = network_layer.ThreadNetworkData(tlvs)

        # WHEN
        actual_tlvs = thread_network_data.tlvs

        # THEN
        self.assertEqual(tlvs, actual_tlvs)

    def test_should_return_True_when_try_to_equal_two_the_same_type_objects_with_the_same_values(self):
        # GIVEN
        tlvs = any_tlvs_data()

        thread_network_data = network_layer.ThreadNetworkData(tlvs)

        # THEN
        self.assertEqual(thread_network_data, network_layer.ThreadNetworkData(tlvs))


class TestThreadNetworkDataFactory(unittest.TestCase):

    def test_should_create_ThreadNetworkData_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        tlvs = any_tlvs_data()

        class DummyNetworkDataTlvsFactory:

            def parse(self, data, message_info):
                return bytearray(data.read())

        factory = network_layer.ThreadNetworkDataFactory(DummyNetworkDataTlvsFactory())

        # WHEN
        thread_network_data = factory.parse(io.BytesIO(tlvs), common.MessageInfo())

        # THEN
        self.assertTrue(isinstance(thread_network_data, network_layer.ThreadNetworkData))
        self.assertEqual(tlvs, thread_network_data.tlvs)


if __name__ == "__main__":
    unittest.main()
