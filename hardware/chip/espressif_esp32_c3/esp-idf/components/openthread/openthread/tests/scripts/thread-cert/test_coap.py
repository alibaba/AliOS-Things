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
import string
import unittest

import coap


def any_delta():
    return random.getrandbits(4)


def any_coap_option_type():
    return random.getrandbits(4)


def any_value():
    return random.getrandbits(8)


def any_4bits_value_different_than_13_and_14():
    value = None
    while value is None:
        value = random.getrandbits(4)
        if value == 13 or value == 14:
            value = None

    return value


def any_4bits_value_lower_or_equal_than_12():
    value = None
    while value is None:
        value = random.getrandbits(4)
        if value > 12:
            value = None

    return value


def any_bytearray(length):
    return bytearray([random.getrandbits(8) for _ in range(length)])


def any_version():
    return random.getrandbits(2)


def any_type():
    return random.getrandbits(2)


def any_code():
    return random.getrandbits(8)


def any_message_id():
    return random.getrandbits(16)


def any_token():
    length = random.randint(0, 8)
    return bytearray([random.getrandbits(8) for _ in range(length)])


def any_options():
    return []


def any_payload(length=None):
    length = length if length is not None else random.randint(0, 64)
    return bytearray([random.getrandbits(8) for _ in range(length)])


def any_uri_path():
    return "/" + random.choice(string.ascii_lowercase)


class TestCoapMessageOptionHeader(unittest.TestCase):

    def test_should_return_passed_on_value_when_read_extended_value_is_called_with_value_different_than_13_and_14(
            self):
        # GIVEN
        value = any_4bits_value_different_than_13_and_14()

        # WHEN
        actual_value = coap.CoapOptionHeader._read_extended_value(None, value)

        # THEN
        self.assertEqual(value, actual_value)

    def test_should_return_value_stored_in_first_byte_plus_13_when_read_extended_value_is_called_with_value_equal_13(
            self):
        # GIVEN
        value = 13
        extended_value = any_value()

        data = io.BytesIO(bytearray([extended_value]))

        # WHEN
        actual_value = coap.CoapOptionHeader._read_extended_value(data, value)

        # THEN
        self.assertEqual(extended_value + 13, actual_value)

    def test_should_return_value_stored_in_first_byte_plus_269_when_read_extended_value_is_called_with_value_equal_14(
            self):
        # GIVEN
        value = 14
        extended_value = any_value()

        data = io.BytesIO(bytearray([any_value(), extended_value]))

        # WHEN
        actual_value = coap.CoapOptionHeader._read_extended_value(data, value)

        # THEN
        self.assertEqual(extended_value + 269, actual_value)

    def test_should_create_CoapOptionHeader_when_from_bytes_classmethod_is_called(self):
        # GIVEN
        delta = any_4bits_value_different_than_13_and_14()
        length = any_4bits_value_different_than_13_and_14()

        data = bytearray([delta << 4 | length])

        # WHEN
        option_header = coap.CoapOptionHeader.from_bytes(io.BytesIO(data))

        # THEN
        self.assertEqual(delta, option_header.delta)
        self.assertEqual(length, option_header.length)

    def test_should_return_True_when_is_payload_marker_property_called_with_delta_and_length_equal_15(self):
        # GIVEN
        delta = 15
        length = 15

        data = bytearray([delta << 4 | length])

        # WHEN
        option_header = coap.CoapOptionHeader.from_bytes(io.BytesIO(data))

        # THEN
        self.assertTrue(option_header.is_payload_marker)


class TestCoapOption(unittest.TestCase):

    def test_should_return_type_value_when_type_property_is_called(self):
        # GIVEN
        _type = any_coap_option_type()

        coap_opt = coap.CoapOption(_type, any_value())

        # WHEN
        actual_type = coap_opt.type

        # THEN
        self.assertEqual(_type, actual_type)

    def test_should_return_value_value_when_value_property_is_called(self):
        # GIVEN
        value = any_value()

        coap_opt = coap.CoapOption(any_coap_option_type(), value)

        # WHEN
        actual_value = coap_opt.value

        # THEN
        self.assertEqual(value, actual_value)


class TestCoapOptionsFactory(unittest.TestCase):

    def test_should_create_list_of_CoapOption_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        delta = any_4bits_value_lower_or_equal_than_12()
        length = any_4bits_value_lower_or_equal_than_12()
        value = any_bytearray(length)

        data = bytearray([delta << 4 | length]) + value

        factory = coap.CoapOptionsFactory()

        # WHEN
        coap_options = factory.parse(io.BytesIO(data), None)

        # THEN
        self.assertEqual(1, len(coap_options))
        self.assertEqual(delta, coap_options[0].type)
        self.assertEqual(value, coap_options[0].value)


class TestCoapCode(unittest.TestCase):

    def test_should_return_code_value_when_code_property_is_called(self):
        # GIVEN
        code = any_code()

        code_obj = coap.CoapCode(code)

        # WHEN
        actual_code = code_obj.code

        # THEN
        self.assertEqual(code, actual_code)

    def test_should_return_class_value_when_class_property_is_called(self):
        # GIVEN
        code = any_code()

        code_obj = coap.CoapCode(code)

        # WHEN
        actual_class = code_obj._class

        # THEN
        self.assertEqual((code >> 5) & 0x7, actual_class)

    def test_should_return_detail_value_when_detail_property_is_called(self):
        # GIVEN
        code = any_code()

        code_obj = coap.CoapCode(code)

        # WHEN
        actual_detail = code_obj.detail

        # THEN
        self.assertEqual(code & 0x1F, actual_detail)

    def test_should_return_dotted_value_when_dotted_property_is_called(self):
        # GIVEN
        code = any_code()

        code_obj = coap.CoapCode(code)

        # WHEN
        actual_dotted = code_obj.dotted

        # THEN
        _class, detail = actual_dotted.split(".")
        self.assertEqual(code, (int(_class) << 5) | int(detail))

    def test_should_create_CoapCode_when_from_class_and_detail_classmethod_is_called(self):
        # GIVEN
        code = any_code()

        _class = (code >> 5) & 0x7
        detail = code & 0x1F

        # WHEN
        actual_coap_obj = coap.CoapCode.from_class_and_detail(_class, detail)

        # THEN
        self.assertEqual(code, actual_coap_obj.code)

    def test_should_create_CoapCode_when_from_dotted_string_classmethod_is_called(self):
        # GIVEN
        code = any_code()

        code_obj = coap.CoapCode(code)

        # WHEN
        actual_coap_obj = coap.CoapCode.from_dotted(code_obj.dotted)

        # THEN
        self.assertEqual(code, actual_coap_obj.code)


class TestCoapMessage(unittest.TestCase):

    def test_should_return_version_value_when_version_property_is_called(self):
        # GIVEN
        version = any_version()

        coap_message = coap.CoapMessage(
            version,
            any_type(),
            any_code(),
            any_message_id(),
            any_token(),
            any_options(),
            any_payload(),
        )

        # WHEN
        actual_version = coap_message.version

        # THEN
        self.assertEqual(version, actual_version)

    def test_should_return_type_value_when_type_property_is_called(self):
        # GIVEN
        _type = any_type()

        coap_message = coap.CoapMessage(
            any_version(),
            _type,
            any_code(),
            any_message_id(),
            any_token(),
            any_options(),
            any_payload(),
        )

        # WHEN
        actual_type = coap_message.type

        # THEN
        self.assertEqual(_type, actual_type)

    def test_should_return_code_value_when_code_property_is_called(self):
        # GIVEN
        code = any_code()

        coap_message = coap.CoapMessage(
            any_version(),
            any_type(),
            code,
            any_message_id(),
            any_token(),
            any_options(),
            any_payload(),
        )

        # WHEN
        actual_code = coap_message.code

        # THEN
        self.assertEqual(code, actual_code)

    def test_should_return_message_id_value_when_message_id_property_is_called(self):
        # GIVEN
        message_id = any_message_id()

        coap_message = coap.CoapMessage(
            any_version(),
            any_type(),
            any_code(),
            message_id,
            any_token(),
            any_options(),
            any_payload(),
        )

        # WHEN
        actual_message_id = coap_message.message_id

        # THEN
        self.assertEqual(message_id, actual_message_id)

    def test_should_return_token_value_when_token_property_is_called(self):
        # GIVEN
        token = any_token()

        coap_message = coap.CoapMessage(
            any_version(),
            any_type(),
            any_code(),
            any_message_id(),
            token,
            any_options(),
            any_payload(),
        )

        # WHEN
        actual_token = coap_message.token

        # THEN
        self.assertEqual(token, actual_token)

    def test_should_return_tkl_value_when_tkl_property_is_called(self):
        # GIVEN
        token = any_token()

        coap_message = coap.CoapMessage(
            any_version(),
            any_type(),
            any_code(),
            any_message_id(),
            token,
            any_options(),
            any_payload(),
        )

        # WHEN
        actual_tkl = coap_message.tkl

        # THEN
        self.assertEqual(len(token), actual_tkl)

    def test_should_return_options_value_when_options_property_is_called(self):
        # GIVEN
        options = any_options()

        coap_message = coap.CoapMessage(
            any_version(),
            any_type(),
            any_code(),
            any_message_id(),
            any_token(),
            options,
            any_payload(),
        )

        # WHEN
        actual_options = coap_message.options

        # THEN
        self.assertEqual(options, actual_options)

    def test_should_return_payload_value_when_payload_property_is_called(self):
        # GIVEN
        payload = any_payload()

        coap_message = coap.CoapMessage(
            any_version(),
            any_type(),
            any_code(),
            any_message_id(),
            any_token(),
            any_options(),
            payload,
        )

        # WHEN
        actual_payload = coap_message.payload

        # THEN
        self.assertEqual(payload, actual_payload)

    def test_should_return_uri_path_value_when_uri_path_property_is_called(self):
        # GIVEN
        uri_path = any_uri_path()

        coap_message = coap.CoapMessage(
            any_version(),
            any_type(),
            any_code(),
            any_message_id(),
            any_token(),
            any_options(),
            any_payload(),
            uri_path,
        )

        # WHEN
        actual_uri_path = coap_message.uri_path

        # THEN
        self.assertEqual(uri_path, actual_uri_path)


class TestCoapMessageIdToUriPathBinder(unittest.TestCase):

    def test_should_add_uri_path_to_binds_when_add_uri_path_for_method_is_called(self):
        # GIVEN
        message_id = any_message_id()
        token = any_token()
        uri_path = any_uri_path()

        binder = coap.CoapMessageIdToUriPathBinder()

        # WHEN
        binder.add_uri_path_for(message_id, token, uri_path)

        # THEN
        self.assertEqual(uri_path, binder.get_uri_path_for(message_id, token))

    def test_should_raise_KeyError_when_get_uri_path_for_is_called_but_it_is_not_present_in_database(self):
        # GIVEN
        message_id = any_message_id()
        token = any_token()
        any_uri_path()

        binder = coap.CoapMessageIdToUriPathBinder()

        # THEN
        self.assertRaises(RuntimeError, binder.get_uri_path_for, message_id, token)


class TestCoapMessageFactory(unittest.TestCase):

    def _create_dummy_payload_factory(self):

        class DummyPayloadFactory:

            def parse(self, data, message_info):
                return data.read()

        return DummyPayloadFactory()

    def _create_coap_message_factory(self):
        return coap.CoapMessageFactory(
            options_factory=coap.CoapOptionsFactory(),
            uri_path_based_payload_factories={"/a/as": self._create_dummy_payload_factory()},
            message_id_to_uri_path_binder=coap.CoapMessageIdToUriPathBinder(),
        )

    def test_should_create_CoapMessage_from_solicit_request_data_when_parse_method_is_called(self):
        # GIVEN
        data = bytearray([
            0x42,
            0x02,
            0x00,
            0xBD,
            0x65,
            0xee,
            0xB1,
            0x61,
            0x02,
            0x61,
            0x73,
            0xff,
            0x01,
            0x08,
            0x16,
            0x6E,
            0x0A,
            0x00,
            0x00,
            0x00,
            0x00,
            0x02,
            0x04,
            0x01,
            0x02,
        ])

        factory = self._create_coap_message_factory()

        # WHEN
        coap_message = factory.parse(io.BytesIO(data), None)

        # THEN
        self.assertEqual(1, coap_message.version)
        self.assertEqual(0, coap_message.type)
        self.assertEqual(2, coap_message.tkl)
        self.assertEqual(2, coap_message.code)
        self.assertEqual(189, coap_message.message_id)
        self.assertEqual(bytearray([0x65, 0xee]), coap_message.token)
        self.assertEqual("a", coap_message.options[0].value.decode("utf-8"))
        self.assertEqual("as", coap_message.options[1].value.decode("utf-8"))
        self.assertEqual("/a/as", coap_message.uri_path)
        self.assertEqual(
            bytearray([
                0x01,
                0x08,
                0x16,
                0x6E,
                0x0A,
                0x00,
                0x00,
                0x00,
                0x00,
                0x02,
                0x04,
                0x01,
                0x02,
            ]),
            coap_message.payload,
        )

    def test_should_create_CoapMessage_from_solicit_response_data_when_parse_method_is_called(self):
        # GIVEN
        data = bytearray([
            0x62,
            0x44,
            0x00,
            0xBD,
            0x65,
            0xee,
            0xff,
            0x04,
            0x01,
            0x00,
            0x02,
            0x02,
            0x00,
            0x00,
            0x07,
            0x09,
            0x76,
            0x80,
            0x00,
            0x01,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
        ])

        mid_binder = coap.CoapMessageIdToUriPathBinder()
        mid_binder.add_uri_path_for(189, bytearray([0x65, 0xee]), "/a/as")

        factory = coap.CoapMessageFactory(
            options_factory=coap.CoapOptionsFactory(),
            uri_path_based_payload_factories={"/a/as": self._create_dummy_payload_factory()},
            message_id_to_uri_path_binder=mid_binder,
        )

        # WHEN
        coap_message = factory.parse(io.BytesIO(data), None)

        # THEN
        self.assertEqual(1, coap_message.version)
        self.assertEqual(2, coap_message.type)
        self.assertEqual(2, coap_message.tkl)
        self.assertEqual("2.04", coap_message.code)
        self.assertEqual(189, coap_message.message_id)
        self.assertEqual(bytearray([0x65, 0xee]), coap_message.token)
        self.assertEqual(None, coap_message.uri_path)
        self.assertEqual(
            bytearray([
                0x04,
                0x01,
                0x00,
                0x02,
                0x02,
                0x00,
                0x00,
                0x07,
                0x09,
                0x76,
                0x80,
                0x00,
                0x01,
                0x00,
                0x00,
                0x00,
                0x00,
                0x00,
            ]),
            coap_message.payload,
        )


if __name__ == "__main__":
    unittest.main()
