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

import hmac
import hashlib
import struct

from binascii import hexlify

from Crypto.Cipher import AES


class CryptoEngine:
    """ Class responsible for encryption and decryption of data. """

    def __init__(self, crypto_material_creator):
        """
        Args:
            network_key (bytearray)

        """
        self._crypto_material_creator = crypto_material_creator

    @property
    def mic_length(self):
        return self._crypto_material_creator.mic_length

    def encrypt(self, data, message_info):
        """ Encrypt message.

        Args:
            data (bytearray)
            message_info (MessageInfo)

        Returns:
            tuple: Encrypted message (bytearray), MIC (bytearray)

        """
        key, nonce, auth_data = self._crypto_material_creator.create_key_and_nonce_and_authenticated_data(message_info)

        cipher = AES.new(key, AES.MODE_CCM, nonce, mac_len=self.mic_length)
        cipher.update(auth_data)

        return cipher.encrypt_and_digest(bytes(data))

    def decrypt(self, enc_data, mic, message_info):
        """ Decrypt MLE message.

        Args:
            enc_data (bytearray)
            mic (bytearray)
            message_info (MessageInfo)

        Returns:
            bytearray: Decrypted message.

        """
        key, nonce, auth_data = self._crypto_material_creator.create_key_and_nonce_and_authenticated_data(message_info)

        cipher = AES.new(key, AES.MODE_CCM, nonce, mac_len=self.mic_length)
        cipher.update(auth_data)

        dec_data = cipher.decrypt_and_verify(bytes(enc_data), bytes(mic))
        return bytearray(dec_data)


class CryptoMaterialCreator(object):

    _salt = b'Thread'

    def __init__(self, network_key):
        """
        Args:
            network_key (bytearray)

        """
        self.network_key = network_key

    def _generate_keys(self, sequence_counter):
        """ Generate MLE and MAC keys.

        Read more: 7.1.4. Key Generation - Thread v1.1 Specification Final

        Args:
            sequence_counter (int)

        Returns:
            tuple: MLE and MAC as bytes

        """
        k = self.network_key
        s = struct.pack(">L", sequence_counter) + self._salt
        d = hmac.new(k, s, digestmod=hashlib.sha256).digest()

        mle = d[:16]
        mac = d[16:]
        return mle, mac

    def create_key_and_nonce_and_authenticated_data(self, message_info):
        raise NotImplementedError

    @property
    def mic_length(self):
        raise NotImplementedError


class MacCryptoMaterialCreator(CryptoMaterialCreator):

    def __init__(self, network_key):
        """
        Args:
            network_key (bytearray)

        """
        super(MacCryptoMaterialCreator, self).__init__(network_key)

    def _create_nonce(self, eui64, frame_counter, security_level):
        """ Create CCM Nonce required by AES-128 CCM for encryption and decryption.

        Read more: 7.6.3.2 CCM Nonce - Std 802.15.4-2006

        Args:
            eui64 (bytes)
            frame_counter (int)
            security_level (int)

        Returns:
            bytes: created Nonce

        """
        return bytes(eui64 + struct.pack(">LB", frame_counter, security_level))

    def _create_authenticated_data(self, mhr, auxiliary_security_header, extra_open_fields):
        """ Create Authenticated Data

        Read more: 7.6.3.3 CCM prerequisites - Std 802.15.4-2006

        Args:
            mhr (bytes)
            auxiliary_security_header (bytes)
            extra_open_fields (bytes)

        Returns:
            bytes: Authenticated Data

        """
        return bytes(mhr + auxiliary_security_header + extra_open_fields)

    def create_key_and_nonce_and_authenticated_data(self, message_info):
        _, mac_key = self._generate_keys(message_info.aux_sec_hdr.sequence_counter)

        nonce = self._create_nonce(
            message_info.source_mac_address,
            message_info.aux_sec_hdr.frame_counter,
            message_info.aux_sec_hdr.security_level,
        )

        auth_data = self._create_authenticated_data(
            message_info.mhr_bytes,
            message_info.aux_sec_hdr_bytes,
            message_info.extra_open_fields,
        )

        return mac_key, nonce, auth_data

    @property
    def mic_length(self):
        return 4


class MleCryptoMaterialCreator(CryptoMaterialCreator):

    def __init__(self, network_key):
        """
        Args:
            network_key (bytearray)

        """
        super(MleCryptoMaterialCreator, self).__init__(network_key)

    def _create_nonce(self, source_eui64, frame_counter, security_level):
        """ Create CCM Nonce required by AES-128 CCM for encryption and decryption.

        Read more: 7.6.3.2 CCM Nonce - Std 802.15.4-2006

        Args:
            eui64 (bytearray)
            frame_counter (int)
            security_level (int)

        Returns:
            bytes: created Nonce

        """
        return bytes(source_eui64[:8] + struct.pack(">LB", frame_counter, security_level))

    def _create_authenticated_data(self, source_address, destination_address, auxiliary_security_header):
        """ Create Authenticated Data

        Read more: 4.8 - Thread v1.0 Specification

        Args:
            source_address (ip_address)
            destination_address (ip_address)
            auxiliary_security_header (bytearray)

        Returns:
            bytes: Authenticated Data

        """
        return bytes(source_address.packed + destination_address.packed + auxiliary_security_header)

    def create_key_and_nonce_and_authenticated_data(self, message_info):
        mle_key, _ = self._generate_keys(message_info.aux_sec_hdr.sequence_counter)

        nonce = self._create_nonce(
            message_info.source_mac_address.mac_address,
            message_info.aux_sec_hdr.frame_counter,
            message_info.aux_sec_hdr.security_level,
        )

        auth_data = self._create_authenticated_data(
            message_info.source_ipv6,
            message_info.destination_ipv6,
            message_info.aux_sec_hdr_bytes,
        )

        return mle_key, nonce, auth_data

    @property
    def mic_length(self):
        return 4


class AuxiliarySecurityHeader:

    def __init__(
        self,
        key_id_mode,
        security_level,
        frame_counter,
        key_id,
        big_endian=True,
    ):
        """
        Args:
            key_id_mode (int)
            security_level (int)
            frame_counter (int)
            key_id (bytearray)
        """
        self._key_id_mode = key_id_mode
        self._security_level = security_level
        self._frame_counter = frame_counter
        self._key_id = key_id
        self._big_endian = big_endian

    @property
    def sequence_counter(self):
        """ Compute or extract sequence counter based on currently set Key Index Mode. """

        if self.key_id_mode == 0:
            key_source = self.key_id[:8]
            format = ">Q" if self._big_endian else "<Q"
        elif self.key_id_mode == 1:
            # Try to guess valid Key Sequence Counter based on Key Index. This
            # one should work for now.
            return self.key_index - 1
        elif self.key_id_mode == 2:
            # In this mode sequence counter is stored on the first four bytes
            # of Key ID.
            key_source = self.key_id[:4]
            format = ">I" if self._big_endian else "<I"
        else:
            raise ValueError("Unsupported Key Index Mode: {}".format(self.key_id_mode))

        return struct.unpack(format, key_source)[0]

    @property
    def key_index(self):
        return struct.unpack(">B", self.key_id[-1:])[0]

    @property
    def key_id_mode(self):
        return self._key_id_mode

    @property
    def security_level(self):
        return self._security_level

    @property
    def frame_counter(self):
        return self._frame_counter

    @property
    def key_id(self):
        return self._key_id

    def __repr__(self):
        return "AuxiliarySecurityHeader(key_id_mode={}, security_level={}, frame_counter={}, key_id={})".format(
            self.key_id_mode,
            self.security_level,
            self.frame_counter,
            hexlify(self.key_id),
        )


class AuxiliarySecurityHeaderFactory:

    _SECURITY_CONTROL_LENGTH = 1
    _FRAME_COUNTER_LENGTH = 4

    _KEY_ID_LENGTH_KEY_ID_0 = 0
    _KEY_ID_LENGTH_KEY_ID_1 = 1
    _KEY_ID_LENGTH_KEY_ID_2 = 5
    _KEY_ID_LENGTH_KEY_ID_3 = 9

    _key_id_lengths = {
        0: _KEY_ID_LENGTH_KEY_ID_0,
        1: _KEY_ID_LENGTH_KEY_ID_1,
        2: _KEY_ID_LENGTH_KEY_ID_2,
        3: _KEY_ID_LENGTH_KEY_ID_3,
    }

    def _parse_security_control(self, security_control_byte):
        security_level = security_control_byte & 0x07
        key_id_mode = (security_control_byte >> 3) & 0x03

        return security_level, key_id_mode

    def _parse_frame_counter(self, frame_counter_bytes):
        return struct.unpack("<I", frame_counter_bytes)[0]

    def _key_id_length(self, key_id_mode):
        return self._key_id_lengths[key_id_mode]

    def parse(self, data, message_info):
        security_control_bytes = bytearray(data.read(self._SECURITY_CONTROL_LENGTH))
        frame_counter_bytes = bytearray(data.read(self._FRAME_COUNTER_LENGTH))

        security_level, key_id_mode = self._parse_security_control(security_control_bytes[0])
        frame_counter = self._parse_frame_counter(frame_counter_bytes)

        key_id_length = self._key_id_length(key_id_mode)
        key_id_bytes = bytearray(data.read(key_id_length))

        aux_sec_hdr = AuxiliarySecurityHeader(key_id_mode, security_level, frame_counter, key_id_bytes)

        message_info.aux_sec_hdr_bytes = (security_control_bytes + frame_counter_bytes + key_id_bytes)
        message_info.aux_sec_hdr = aux_sec_hdr

        return aux_sec_hdr
