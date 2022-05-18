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

from enum import IntEnum
from functools import reduce
import io
import struct

from ipv6 import BuildableFromBytes
from ipv6 import ConvertibleToBytes


class HandshakeType(IntEnum):
    HELLO_REQUEST = 0
    CLIENT_HELLO = 1
    SERVER_HELLO = 2
    HELLO_VERIFY_REQUEST = 3
    CERTIFICATE = 11
    SERVER_KEY_EXCHANGE = 12
    CERTIFICATE_REQUEST = 13
    SERVER_HELLO_DONE = 14
    CERTIFICATE_VERIFY = 15
    CLIENT_KEY_EXCHANGE = 16
    FINISHED = 20


class ContentType(IntEnum):
    CHANGE_CIPHER_SPEC = 20
    ALERT = 21
    HANDSHAKE = 22
    APPLICATION_DATA = 23


class AlertLevel(IntEnum):
    WARNING = 1
    FATAL = 2


class AlertDescription(IntEnum):
    CLOSE_NOTIFY = 0
    UNEXPECTED_MESSAGE = 10
    BAD_RECORD_MAC = 20
    DECRYPTION_FAILED_RESERVED = 21
    RECORD_OVERFLOW = 22
    DECOMPRESSION_FAILURE = 30
    HANDSHAKE_FAILURE = 40
    NO_CERTIFICATE_RESERVED = 41
    BAD_CERTIFICATE = 42
    UNSUPPORTED_CERTIFICATE = 43
    CERTIFICATE_REVOKED = 44
    CERTIFICATE_EXPIRED = 45
    CERTIFICATE_UNKNOWN = 46
    ILLEGAL_PARAMETER = 47
    UNKNOWN_CA = 48
    ACCESS_DENIED = 49
    DECODE_ERROR = 50
    DECRYPT_ERROR = 51
    EXPORT_RESTRICTION_RESERVED = 60
    PROTOCOL_VERSION = 70
    INSUFFICIENT_SECURITY = 71
    INTERNAL_ERROR = 80
    USER_CANCELED = 90
    NO_RENEGOTIATION = 100
    UNSUPPORTED_EXTENSION = 110


class Record(ConvertibleToBytes, BuildableFromBytes):

    def __init__(self, content_type, version, epoch, sequence_number, length, fragment):
        self.content_type = content_type
        self.version = version
        self.epoch = epoch
        self.sequence_number = sequence_number
        self.length = length
        self.fragment = fragment

    def to_bytes(self):
        return (struct.pack(">B", self.content_type) + self.version.to_bytes() + struct.pack(">H", self.epoch) +
                self.sequence_number.to_bytes(6, byteorder='big') + struct.pack(">H", self.length) + self.fragment)

    @classmethod
    def from_bytes(cls, data):
        content_type = ContentType(struct.unpack(">B", data.read(1))[0])
        version = ProtocolVersion.from_bytes(data)
        epoch = struct.unpack(">H", data.read(2))[0]
        sequence_number = struct.unpack(">Q", b'\x00\x00' + data.read(6))[0]
        length = struct.unpack(">H", data.read(2))[0]
        fragment = bytes(data.read(length))
        return cls(content_type, version, epoch, sequence_number, length, fragment)

    def __repr__(self):
        return "Record(content_type={}, version={}, epoch={}, sequence_number={}, length={})".format(
            str(self.content_type),
            self.version,
            self.epoch,
            self.sequence_number,
            self.length,
        )


class Message(ConvertibleToBytes, BuildableFromBytes):

    def __init__(self, content_type):
        self.content_type = content_type

    def to_bytes(self):
        raise NotImplementedError

    @classmethod
    def from_bytes(cls, data):
        raise NotImplementedError


class HandshakeMessage(Message):

    def __init__(
        self,
        handshake_type,
        length,
        message_seq,
        fragment_offset,
        fragment_length,
        body,
    ):
        super(HandshakeMessage, self).__init__(ContentType.HANDSHAKE)
        self.handshake_type = handshake_type
        self.length = length
        self.message_seq = message_seq
        self.fragment_offset = fragment_offset
        self.fragment_length = fragment_length
        self.body = body

    def to_bytes(self):
        return (struct.pack(">B", self.handshake_type) + struct.pack(">I", self.length)[1:] +
                struct.pack(">H", self.message_seq) + struct.pack(">I", self.fragment_offset)[1:] +
                struct.pack(">I", self.fragment_length)[1:] + self.body.to_bytes())

    @classmethod
    def from_bytes(cls, data):
        handshake_type = HandshakeType(struct.unpack(">B", data.read(1))[0])
        length = struct.unpack(">I", b'\x00' + data.read(3))[0]
        message_seq = struct.unpack(">H", data.read(2))[0]
        fragment_offset = struct.unpack(">I", b'\x00' + bytes(data.read(3)))[0]
        fragment_length = struct.unpack(">I", b'\x00' + bytes(data.read(3)))[0]
        end_position = data.tell() + fragment_length
        # TODO(wgtdkp): handle fragmentation

        message_class, body = handshake_map[handshake_type], None
        if message_class:
            body = message_class.from_bytes(data)
        else:
            print("{} messages are not handled".format(str(handshake_type)))
            body = bytes(data.read(fragment_length))
        assert data.tell() == end_position

        return cls(
            handshake_type,
            length,
            message_seq,
            fragment_offset,
            fragment_length,
            body,
        )

    def __repr__(self):
        return "Handshake(type={}, length={})".format(str(self.handshake_type), self.length)


class ProtocolVersion(ConvertibleToBytes, BuildableFromBytes):

    def __init__(self, major, minor):
        self.major = major
        self.minor = minor

    def __eq__(self, other):
        return (isinstance(self, type(other)) and self.major == other.major and self.minor == other.minor)

    def to_bytes(self):
        return struct.pack(">BB", self.major, self.minor)

    @classmethod
    def from_bytes(cls, data):
        major, minor = struct.unpack(">BB", data.read(2))
        return cls(major, minor)

    def __repr__(self):
        return "ProtocolVersion(major={}, minor={})".format(self.major, self.minor)


class Random(ConvertibleToBytes, BuildableFromBytes):

    random_bytes_length = 28

    def __init__(self, gmt_unix_time, random_bytes):
        self.gmt_unix_time = gmt_unix_time
        self.random_bytes = random_bytes
        assert len(self.random_bytes) == Random.random_bytes_length

    def __eq__(self, other):
        return (isinstance(self, type(other)) and self.gmt_unix_time == other.gmt_unix_time and
                self.random_bytes == other.random_bytes)

    def to_bytes(self):
        return struct.pack(">I", self.gmt_unix_time) + (self.random_bytes)

    @classmethod
    def from_bytes(cls, data):
        gmt_unix_time = struct.unpack(">I", data.read(4))[0]
        random_bytes = bytes(data.read(cls.random_bytes_length))
        return cls(gmt_unix_time, random_bytes)


class VariableVector(ConvertibleToBytes):

    def __init__(self, subrange, ele_cls, elements):
        self.subrange = subrange
        self.ele_cls = ele_cls
        self.elements = elements
        assert self.subrange[0] <= len(self.elements) <= self.subrange[1]

    def length(self):
        return len(self.elements)

    def __eq__(self, other):
        return (isinstance(self, type(other)) and self.subrange == other.subrange and self.ele_cls == other.ele_cls and
                self.elements == other.elements)

    def to_bytes(self):
        data = reduce(lambda ele, acc: acc + ele.to_bytes(), self.elements)
        return VariableVector._encode_length(len(data), self.subrange) + data

    @classmethod
    def from_bytes(cls, ele_cls, subrange, data):
        length = cls._decode_length(subrange, data)
        end_position = data.tell() + length
        elements = []
        while data.tell() < end_position:
            elements.append(ele_cls.from_bytes(data))
        return cls(subrange, ele_cls, elements)

    @classmethod
    def _decode_length(cls, subrange, data):
        length_in_byte = cls._calc_length_in_byte(subrange[1])
        return reduce(
            lambda acc, byte: (acc << 8) | byte,
            bytearray(data.read(length_in_byte)),
            0,
        )

    @classmethod
    def _encode_length(cls, length, subrange):
        length_in_byte = cls._calc_length_in_byte(subrange[1])
        ret = bytearray([])
        while length_in_byte > 0:
            ret += bytes(length_in_byte & 0xff)
            length_in_byte = length_in_byte >> 8
        return ret

    @classmethod
    def _calc_length_in_byte(cls, ceiling):
        return (ceiling.bit_length() + 7) // 8


class Opaque(ConvertibleToBytes, BuildableFromBytes):

    def __init__(self, byte):
        self.byte = byte

    def __eq__(self, other):
        return isinstance(self, type(other)) and self.byte == other.byte

    def to_bytes(self):
        return struct.pack(">B", self.byte)

    @classmethod
    def from_bytes(cls, data):
        return cls(struct.unpack(">B", data.read(1))[0])


class CipherSuite(ConvertibleToBytes, BuildableFromBytes):

    def __init__(self, cipher):
        self.cipher = cipher

    def __eq__(self, other):
        return isinstance(self, type(other)) and self.cipher == other.cipher

    def to_bytes(self):
        return struct.pack(">BB", self.cipher[0], self.cipher[1])

    @classmethod
    def from_bytes(cls, data):
        return cls(struct.unpack(">BB", data.read(2)))

    def __repr__(self):
        return "CipherSuite({}, {})".format(self.cipher[0], self.cipher[1])


class CompressionMethod(ConvertibleToBytes, BuildableFromBytes):

    NULL = 0

    def __init__(self):
        pass

    def __eq__(self, other):
        return isinstance(self, type(other))

    def to_bytes(self):
        return struct.pack(">B", CompressionMethod.NULL)

    @classmethod
    def from_bytes(cls, data):
        method = struct.unpack(">B", data.read(1))[0]
        assert method == cls.NULL
        return cls()


class Extension(ConvertibleToBytes, BuildableFromBytes):

    def __init__(self, extension_type, extension_data):
        self.extension_type = extension_type
        self.extension_data = extension_data

    def __eq__(self, other):
        return (isinstance(self, type(other)) and self.extension_type == other.extension_type and
                self.extension_data == other.extension_data)

    def to_bytes(self):
        return (struct.pack(">H", self.extension_type) + self.extension_data.to_bytes())

    @classmethod
    def from_bytes(cls, data):
        extension_type = struct.unpack(">H", data.read(2))[0]
        extension_data = VariableVector.from_bytes(Opaque, (0, 2**16 - 1), data)
        return cls(extension_type, extension_data)


class ClientHello(HandshakeMessage):

    def __init__(
        self,
        client_version,
        random,
        session_id,
        cookie,
        cipher_suites,
        compression_methods,
        extensions,
    ):
        self.client_version = client_version
        self.random = random
        self.session_id = session_id
        self.cookie = cookie
        self.cipher_suites = cipher_suites
        self.compression_methods = compression_methods
        self.extensions = extensions

    def to_bytes(self):
        return (self.client_version.to_bytes() + self.random.to_bytes() + self.session_id.to_bytes() +
                self.cookie.to_bytes() + self.cipher_suites.to_bytes() + self.compression_methods.to_bytes() +
                self.extensions.to_bytes())

    @classmethod
    def from_bytes(cls, data):
        client_version = ProtocolVersion.from_bytes(data)
        random = Random.from_bytes(data)
        session_id = VariableVector.from_bytes(Opaque, (0, 32), data)
        cookie = VariableVector.from_bytes(Opaque, (0, 2**8 - 1), data)
        cipher_suites = VariableVector.from_bytes(CipherSuite, (2, 2**16 - 1), data)
        compression_methods = VariableVector.from_bytes(CompressionMethod, (1, 2**8 - 1), data)
        extensions = None
        if data.tell() < len(data.getvalue()):
            extensions = VariableVector.from_bytes(Extension, (0, 2**16 - 1), data)
        return cls(
            client_version,
            random,
            session_id,
            cookie,
            cipher_suites,
            compression_methods,
            extensions,
        )


class HelloVerifyRequest(HandshakeMessage):

    def __init__(self, server_version, cookie):
        self.server_version = server_version
        self.cookie = cookie

    def to_bytes(self):
        return self.server_version.to_bytes() + self.cookie.to_bytes()

    @classmethod
    def from_bytes(cls, data):
        server_version = ProtocolVersion.from_bytes(data)
        cookie = VariableVector.from_bytes(Opaque, (0, 2**8 - 1), data)
        return cls(server_version, cookie)


class ServerHello(HandshakeMessage):

    def __init__(
        self,
        server_version,
        random,
        session_id,
        cipher_suite,
        compression_method,
        extensions,
    ):
        self.server_version = server_version
        self.random = random
        self.session_id = session_id
        self.cipher_suite = cipher_suite
        self.compression_method = compression_method
        self.extensions = extensions

    def to_bytes(self):
        return (self.server_version.to_bytes() + self.random.to_bytes() + self.session_id.to_bytes() +
                self.cipher_suite.to_bytes() + self.compression_method.to_bytes() + self.extensions.to_bytes())

    @classmethod
    def from_bytes(cls, data):
        server_version = ProtocolVersion.from_bytes(data)
        random = Random.from_bytes(data)
        session_id = VariableVector.from_bytes(Opaque, (0, 32), data)
        cipher_suite = CipherSuite.from_bytes(data)
        compression_method = CompressionMethod.from_bytes(data)
        extensions = None
        if data.tell() < len(data.getvalue()):
            extensions = VariableVector.from_bytes(Extension, (0, 2**16 - 1), data)
        return cls(
            server_version,
            random,
            session_id,
            cipher_suite,
            compression_method,
            extensions,
        )


class ServerHelloDone(HandshakeMessage):

    def __init__(self):
        pass

    def to_bytes(self):
        return bytearray([])

    @classmethod
    def from_bytes(cls, data):
        return cls()


class HelloRequest(HandshakeMessage):

    def __init__(self):
        raise NotImplementedError


class Certificate(HandshakeMessage):

    def __init__(self):
        raise NotImplementedError


class ServerKeyExchange(HandshakeMessage):

    def __init__(self):
        raise NotImplementedError


class CertificateRequest(HandshakeMessage):

    def __init__(self):
        raise NotImplementedError


class CertificateVerify(HandshakeMessage):

    def __init__(self):
        raise NotImplementedError


class ClientKeyExchange(HandshakeMessage):

    def __init__(self):
        raise NotImplementedError


class Finished(HandshakeMessage):

    def __init__(self, verify_data):
        raise NotImplementedError


class AlertMessage(Message):

    def __init__(self, level, description):
        super(AlertMessage, self).__init__(ContentType.ALERT)
        self.level = level
        self.description = description

    def to_bytes(self):
        struct.pack(">BB", self.level, self.description)

    @classmethod
    def from_bytes(cls, data):
        level, description = struct.unpack(">BB", data.read(2))
        try:
            return cls(AlertLevel(level), AlertDescription(description))
        except BaseException:
            data.read()
            # An AlertMessage could be encrypted and we can't parsing it.
            return cls(None, None)

    def __repr__(self):
        return "Alert(level={}, description={})".format(str(self.level), str(self.description))


class ChangeCipherSpecMessage(Message):

    def __init__(self):
        super(ChangeCipherSpecMessage, self).__init__(ContentType.CHANGE_CIPHER_SPEC)

    def to_bytes(self):
        return struct.pack(">B", 1)

    @classmethod
    def from_bytes(cls, data):
        assert struct.unpack(">B", data.read(1))[0] == 1
        return cls()

    def __repr__(self):
        return "ChangeCipherSpec(value=1)"


class ApplicationDataMessage(Message):

    def __init__(self, raw):
        super(ApplicationDataMessage, self).__init__(ContentType.APPLICATION_DATA)
        self.raw = raw
        self.body = None

    def to_bytes(self):
        return self.raw

    @classmethod
    def from_bytes(cls, data):
        # It is safe to read until the end of this byte stream, because
        # there is single application data message in a record.
        length = len(data.getvalue()) - data.tell()
        return cls(bytes(data.read(length)))

    def __repr__(self):
        if self.body:
            return "ApplicationData(body={})".format(self.body)
        else:
            return "ApplicationData(raw_length={})".format(len(self.raw))


handshake_map = {
    HandshakeType.HELLO_REQUEST: None,  # HelloRequest
    HandshakeType.CLIENT_HELLO: ClientHello,
    HandshakeType.SERVER_HELLO: ServerHello,
    HandshakeType.HELLO_VERIFY_REQUEST: HelloVerifyRequest,
    HandshakeType.CERTIFICATE: None,  # Certificate
    HandshakeType.SERVER_KEY_EXCHANGE: None,  # ServerKeyExchange
    HandshakeType.CERTIFICATE_REQUEST: None,  # CertificateRequest
    HandshakeType.SERVER_HELLO_DONE: ServerHelloDone,
    HandshakeType.CERTIFICATE_VERIFY: None,  # CertificateVerify
    HandshakeType.CLIENT_KEY_EXCHANGE: None,  # ClientKeyExchange
    HandshakeType.FINISHED: None,  # Finished
}

content_map = {
    ContentType.CHANGE_CIPHER_SPEC: ChangeCipherSpecMessage,
    ContentType.ALERT: AlertMessage,
    ContentType.HANDSHAKE: HandshakeMessage,
    ContentType.APPLICATION_DATA: ApplicationDataMessage,
}


class MessageFactory(object):

    last_msg_is_change_cipher_spec = False

    def __init__(self):
        pass

    def parse(self, data, message_info):
        messages = []

        # Multiple records could be sent in the same UDP datagram
        while data.tell() < len(data.getvalue()):
            record = Record.from_bytes(data)

            if record.version.major != 0xfe or record.version.minor != 0xFD:
                raise ValueError("DTLS version error, expect DTLSv1.2")

            last_msg_is_change_cipher_spec = type(self).last_msg_is_change_cipher_spec
            type(self).last_msg_is_change_cipher_spec = (record.content_type == ContentType.CHANGE_CIPHER_SPEC)

            # FINISHED message immediately follows CHANGE_CIPHER_SPEC message
            # We skip FINISHED message as it is encrypted
            if last_msg_is_change_cipher_spec:
                continue

            fragment_data = io.BytesIO(record.fragment)

            # Multiple handshake messages could be sent in the same record
            while fragment_data.tell() < len(fragment_data.getvalue()):
                content_class = content_map[record.content_type]
                assert content_class
                messages.append(content_class.from_bytes(fragment_data))

        return messages
