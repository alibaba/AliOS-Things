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
import ipaddress

import common
import net_crypto

network_key = bytearray(
    [0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff])


def convert_aux_sec_hdr_to_bytearray(aux_sec_hdr):
    data = bytearray([aux_sec_hdr.security_level | ((aux_sec_hdr.key_id_mode & 0x03) << 3)])
    data += struct.pack("<L", aux_sec_hdr.frame_counter)
    data += aux_sec_hdr.key_id
    return data


def any_eui64():
    return bytearray([random.getrandbits(8) for _ in range(8)])


def any_security_level():
    return random.getrandbits(3)


def any_key_id_mode():
    """
    Only key id mode 2.
    """
    return 2


def any_key_id(key_id_mode):
    if key_id_mode == 2:
        length = 5

    return bytearray([random.getrandbits(8) for _ in range(length)])


def any_auxiliary_security_header():
    key_id_mode = any_key_id_mode()
    key_id = any_key_id(key_id_mode)

    return net_crypto.AuxiliarySecurityHeader(key_id_mode, any_security_level(), any_frame_counter(), key_id)


def any_frame_counter():
    return random.getrandbits(32)


def any_ip_address():
    ip_address_bytes = bytearray([random.getrandbits(8) for _ in range(16)])
    return ipaddress.ip_address(bytes(ip_address_bytes))


def any_data(length=None):
    length = length if length is not None else random.randint(0, 128)
    return bytearray([random.getrandbits(8) for _ in range(length)])


def any_network_key():
    return bytearray([random.getrandbits(8) for _ in range(16)])


class TestCryptoEngine(unittest.TestCase):

    def test_should_decrypt_bytearray_to_mle_message_when_decrypt_method_is_called(self):
        # GIVEN
        message_info = common.MessageInfo()
        message_info.source_mac_address = common.MacAddress.from_eui64(
            bytearray([0x00, 0x35, 0xcc, 0x94, 0xd7, 0x7a, 0x07, 0xe8]))

        message_info.source_ipv6 = "fe80::235:cc94:d77a:07e8"
        message_info.destination_ipv6 = "ff02::2"

        message_info.aux_sec_hdr = net_crypto.AuxiliarySecurityHeader(key_id_mode=2,
                                                                      security_level=5,
                                                                      frame_counter=262165,
                                                                      key_id=bytearray([0x00, 0x00, 0x00, 0x00, 0x01]))
        message_info.aux_sec_hdr_bytes = convert_aux_sec_hdr_to_bytearray(message_info.aux_sec_hdr)

        data = bytearray([
            0x9a, 0x5a, 0x9a, 0x5b, 0xba, 0x25, 0x9c, 0x5e, 0x58, 0xa2, 0x7e, 0x75, 0x74, 0xef, 0x79, 0xbc, 0x4f, 0xa3,
            0xf9, 0xae, 0xa8, 0x34, 0xf6, 0xf2, 0x37, 0x21, 0x93, 0x60
        ])

        mic = bytearray([0xe1, 0xb5, 0xa2, 0x53])

        net_crypto_engine = net_crypto.CryptoEngine(net_crypto.MleCryptoMaterialCreator(network_key))

        # WHEN
        mle_msg = net_crypto_engine.decrypt(data, mic, message_info)

        # THEN
        expected_mle_msg = bytearray([
            0x04, 0x00, 0x02, 0x00, 0x00, 0x09, 0x0b, 0x8f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x01, 0xf1,
            0x0b, 0x08, 0x65, 0x5e, 0x0f, 0x83, 0x40, 0xc7, 0x83, 0x31
        ])
        self.assertEqual(expected_mle_msg, mle_msg)

    def test_should_encrypt_mle_message_to_bytearray_when_encrypt_method_is_called(self):
        # GIVEN
        message_info = common.MessageInfo()
        message_info.source_mac_address = common.MacAddress.from_eui64(
            bytearray([0x00, 0x35, 0xcc, 0x94, 0xd7, 0x7a, 0x07, 0xe8]))

        message_info.source_ipv6 = "fe80::235:cc94:d77a:07e8"
        message_info.destination_ipv6 = "ff02::2"

        message_info.aux_sec_hdr = net_crypto.AuxiliarySecurityHeader(key_id_mode=2,
                                                                      security_level=5,
                                                                      frame_counter=262165,
                                                                      key_id=bytearray([0x00, 0x00, 0x00, 0x00, 0x01]))
        message_info.aux_sec_hdr_bytes = convert_aux_sec_hdr_to_bytearray(message_info.aux_sec_hdr)

        mle_msg = bytearray([
            0x04, 0x00, 0x02, 0x00, 0x00, 0x09, 0x0b, 0x8f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x01, 0xf1,
            0x0b, 0x08, 0x65, 0x5e, 0x0f, 0x83, 0x40, 0xc7, 0x83, 0x31
        ])

        net_crypto_engine = net_crypto.CryptoEngine(net_crypto.MleCryptoMaterialCreator(network_key))

        # WHEN
        encrypted_data, mic = net_crypto_engine.encrypt(mle_msg, message_info)

        # THEN
        expected_encrypted_data = bytearray([
            0x9a, 0x5a, 0x9a, 0x5b, 0xba, 0x25, 0x9c, 0x5e, 0x58, 0xa2, 0x7e, 0x75, 0x74, 0xef, 0x79, 0xbc, 0x4f, 0xa3,
            0xf9, 0xae, 0xa8, 0x34, 0xf6, 0xf2, 0x37, 0x21, 0x93, 0x60, 0xe1, 0xb5, 0xa2, 0x53
        ])

        self.assertEqual(expected_encrypted_data, encrypted_data + mic)

    def test_should_encrypt_and_decrypt_random_data_content_when_proper_methods_are_called(self):
        # GIVEN
        data = any_data()

        network_key = any_network_key()

        key_id_mode = 2
        security_level = 5

        message_info = common.MessageInfo()
        message_info.source_mac_address = common.MacAddress.from_eui64(any_eui64())

        message_info.source_ipv6 = any_ip_address()
        message_info.destination_ipv6 = any_ip_address()

        message_info.aux_sec_hdr = net_crypto.AuxiliarySecurityHeader(key_id_mode=key_id_mode,
                                                                      security_level=security_level,
                                                                      frame_counter=any_frame_counter(),
                                                                      key_id=any_key_id(key_id_mode))
        message_info.aux_sec_hdr_bytes = convert_aux_sec_hdr_to_bytearray(message_info.aux_sec_hdr)

        net_crypto_engine = net_crypto.CryptoEngine(net_crypto.MleCryptoMaterialCreator(network_key))

        # WHEN
        enc_data, mic = net_crypto_engine.encrypt(data, message_info)
        dec_data = net_crypto_engine.decrypt(enc_data, mic, message_info)

        # THEN
        self.assertEqual(data, dec_data)


class TestCryptoMaterialCreator(unittest.TestCase):
    """ Key generaion was described in Thread specification.

    Read more: Thread 1.1.0 Specification Candidate Final - 7.1.4 Key Generation

    Test vectors was taken from thread specification.
    """

    def test_should_generate_mle_and_mac_key_when_generate_keys_method_is_called_with_sequence_counter_equal_0(self):
        """
        7.1.4.1 Test Vector 1
        """

        # GIVEN
        sequence_counter = 0

        creator = net_crypto.CryptoMaterialCreator(network_key)

        # WHEN
        mle_key, mac_key = creator._generate_keys(sequence_counter)

        # THEN
        self.assertEqual(
            mle_key,
            bytearray([0x54, 0x45, 0xf4, 0x15, 0x8f, 0xd7, 0x59, 0x12, 0x17, 0x58, 0x09, 0xf8, 0xb5, 0x7a, 0x66,
                       0xa4]))
        self.assertEqual(
            mac_key,
            bytearray([0xde, 0x89, 0xc5, 0x3a, 0xf3, 0x82, 0xb4, 0x21, 0xe0, 0xfd, 0xe5, 0xa9, 0xba, 0xe3, 0xbe,
                       0xf0]))

    def test_should_generate_mle_and_mac_key_when_generate_keys_method_is_called_with_sequence_counter_equal_1(self):
        """
        7.1.4.2 Test Vector 2
        """

        # GIVEN
        sequence_counter = 1

        creator = net_crypto.CryptoMaterialCreator(network_key)

        # WHEN
        mle_key, mac_key = creator._generate_keys(sequence_counter)

        # THEN
        self.assertEqual(
            mle_key,
            bytearray([0x8f, 0x4c, 0xd1, 0xa2, 0x7d, 0x95, 0xc0, 0x7d, 0x12, 0xdb, 0x89, 0x74, 0xbd, 0x61, 0x5c,
                       0x13]))
        self.assertEqual(
            mac_key,
            bytearray([0x9b, 0xe0, 0xd1, 0xaf, 0x7b, 0xd8, 0x73, 0x50, 0xde, 0xab, 0xcd, 0xd0, 0x7f, 0xeb, 0xb9,
                       0xd5]))

    def test_should_generate_mle_and_mac_key_when_generate_keys_method_is_called_with_sequence_counter_equal_2(self):
        """
        7.1.4.3 Test Vector 3
        """

        # GIVEN
        sequence_counter = 2

        creator = net_crypto.CryptoMaterialCreator(network_key)

        # WHEN
        mle_key, mac_key = creator._generate_keys(sequence_counter)

        # THEN
        self.assertEqual(
            mle_key,
            bytearray([0x01, 0x6e, 0x2a, 0xb8, 0xec, 0x88, 0x87, 0x96, 0x87, 0xa7, 0x2e, 0x0a, 0x35, 0x7e, 0xcf,
                       0x2a]))
        self.assertEqual(
            mac_key,
            bytearray([0x56, 0x41, 0x09, 0xe9, 0xd2, 0xaa, 0xd7, 0xf7, 0x23, 0xec, 0x3b, 0x96, 0x11, 0x0e, 0xef,
                       0xa3]))


class TestMleCryptoMaterialCreator(unittest.TestCase):

    def test_should_create_nonce_when_create_nonce_method_is_called(self):
        # GIVEN
        source_eui64 = any_eui64()
        frame_counter = any_frame_counter()
        security_level = any_security_level()

        creator = net_crypto.MleCryptoMaterialCreator(network_key)

        # WHEN
        nonce = creator._create_nonce(source_eui64, frame_counter, security_level)

        # THEN
        nonce_bytes = io.BytesIO(nonce)

        self.assertEqual(source_eui64, nonce_bytes.read(8))
        self.assertEqual(struct.pack(">L", frame_counter), nonce_bytes.read(4))
        self.assertEqual(security_level, ord(nonce_bytes.read(1)))

    def test_should_create_authenticated_data_when_create_authenticated_data_method_is_called(self):
        """
        Only Key id mode 2.
        Length of the Auxiliary Security Header is constantly equal 10.
        """

        # GIVEN
        source_address = any_ip_address()
        destination_address = any_ip_address()
        auxiliary_security_header_bytes = convert_aux_sec_hdr_to_bytearray(any_auxiliary_security_header())

        creator = net_crypto.MleCryptoMaterialCreator(network_key)

        # WHEN
        authenticated_data = creator._create_authenticated_data(source_address, destination_address,
                                                                auxiliary_security_header_bytes)

        # THEN
        authenticated_data_bytes = io.BytesIO(authenticated_data)

        self.assertEqual(source_address.packed, authenticated_data_bytes.read(16))
        self.assertEqual(destination_address.packed, authenticated_data_bytes.read(16))
        self.assertEqual(auxiliary_security_header_bytes, authenticated_data_bytes.read(10))

    def test_should_create_key_and_nonce_and_auth_data_when_create_key_and_nonce_and_auth_data_is_called(self):
        # GIVEN
        message_info = common.MessageInfo()
        message_info.source_mac_address = common.MacAddress.from_eui64(any_eui64())

        message_info.source_ipv6 = any_ip_address()
        message_info.destination_ipv6 = any_ip_address()

        message_info.aux_sec_hdr = any_auxiliary_security_header()
        message_info.aux_sec_hdr_bytes = convert_aux_sec_hdr_to_bytearray(message_info.aux_sec_hdr)

        creator = net_crypto.MleCryptoMaterialCreator(network_key)

        # WHEN
        key, nonce, auth_data = creator.create_key_and_nonce_and_authenticated_data(message_info)

        # THEN
        self.assertEqual(
            message_info.source_mac_address.mac_address +
            struct.pack(">LB", message_info.aux_sec_hdr.frame_counter, message_info.aux_sec_hdr.security_level), nonce)

        self.assertEqual(
            message_info.source_ipv6.packed + message_info.destination_ipv6.packed + message_info.aux_sec_hdr_bytes,
            auth_data)


class TestAuxiliarySecurityHeader(unittest.TestCase):

    def test_should_return_key_id_mode_value_when_key_id_mode_property_is_called(self):
        # GIVEN
        key_id_mode = any_key_id_mode()

        aux_sec_hdr_obj = net_crypto.AuxiliarySecurityHeader(key_id_mode, any_security_level(), any_frame_counter(),
                                                             any_key_id(key_id_mode))

        # WHEN
        actual_key_id_mode = aux_sec_hdr_obj.key_id_mode

        # THEN
        self.assertEqual(key_id_mode, actual_key_id_mode)

    def test_should_return_security_level_value_when_security_level_property_is_called(self):
        # GIVEN
        security_level = any_security_level()
        key_id_mode = any_key_id_mode()

        aux_sec_hdr_obj = net_crypto.AuxiliarySecurityHeader(key_id_mode, security_level, any_frame_counter(),
                                                             any_key_id(key_id_mode))

        # WHEN
        actual_security_level = aux_sec_hdr_obj.security_level

        # THEN
        self.assertEqual(security_level, actual_security_level)

    def test_should_return_frame_counter_value_when_frame_counter_property_is_called(self):
        # GIVEN
        frame_counter = any_frame_counter()
        key_id_mode = any_key_id_mode()

        aux_sec_hdr_obj = net_crypto.AuxiliarySecurityHeader(key_id_mode, any_security_level(), frame_counter,
                                                             any_key_id(key_id_mode))

        # WHEN
        actual_frame_counter = aux_sec_hdr_obj.frame_counter

        # THEN
        self.assertEqual(frame_counter, actual_frame_counter)

    def test_should_return_key_id_value_when_key_id_property_is_called(self):
        # GIVEN
        key_id_mode = any_key_id_mode()
        key_id = any_key_id(key_id_mode)

        aux_sec_hdr_obj = net_crypto.AuxiliarySecurityHeader(key_id_mode, any_security_level(), any_frame_counter(),
                                                             key_id)

        # WHEN
        actual_key_id = aux_sec_hdr_obj.key_id

        # THEN
        self.assertEqual(key_id, actual_key_id)

    def test_should_return_sequence_counter_value_when_sequence_counter_property_is_called(self):
        # GIVEN
        key_id_mode = 2
        key_id = any_key_id(key_id_mode)

        aux_sec_hdr_obj = net_crypto.AuxiliarySecurityHeader(key_id_mode, any_security_level(), any_frame_counter(),
                                                             key_id)

        # WHEN
        actual_sequence_counter = aux_sec_hdr_obj.sequence_counter

        # THEN
        self.assertEqual(struct.unpack(">I", key_id[:4])[0], actual_sequence_counter)


class TestAuxiliarySecurityHeaderFactory(unittest.TestCase):

    def test_should_create_AuxiliarySecurityHeader_from_bytearray_when_parse_method_is_called(self):
        # GIVEN
        key_id_mode = any_key_id_mode()
        sec_lvl = any_security_level()
        frame_counter = any_frame_counter()
        key_id = any_key_id(key_id_mode)

        factory = net_crypto.AuxiliarySecurityHeaderFactory()

        data = bytearray([sec_lvl | key_id_mode << 3]) + struct.pack("<I", frame_counter) + key_id

        # WHEN
        aux_sec_hdr = factory.parse(io.BytesIO(data), common.MessageInfo())

        # THEN
        self.assertTrue(isinstance(aux_sec_hdr, net_crypto.AuxiliarySecurityHeader))
        self.assertEqual(key_id_mode, aux_sec_hdr.key_id_mode)
        self.assertEqual(sec_lvl, aux_sec_hdr.security_level)
        self.assertEqual(frame_counter, aux_sec_hdr.frame_counter)


if __name__ == "__main__":
    unittest.main()
