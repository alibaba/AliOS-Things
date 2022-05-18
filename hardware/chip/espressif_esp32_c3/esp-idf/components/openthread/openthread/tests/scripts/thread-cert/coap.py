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

import collections
import io
import struct

from binascii import hexlify
from enum import IntEnum


class CoapMessageType(IntEnum):
    CON = 0  # Confirmable
    NON = 1  # Non-confirmable
    ACK = 2  # Acknowledgement
    RST = 3  # Reset


class CoapOptionsTypes(IntEnum):
    IF_MATCH = 1
    URI_HOST = 3
    ETAG = 4
    IF_NOT_MATCH = 5
    URI_PORT = 7
    LOCATION_PATH = 8
    URI_PATH = 11
    CONTENT_FORMAT = 12
    MAX_AGE = 14
    URI_QUERY = 15
    ACCEPT = 17
    LOCATION_QUERY = 20
    PROXY_URI = 35
    PROXY_SCHEME = 39
    SIZE1 = 60


class CoapOptionHeader(object):
    """ Class representing CoAP optiona header. """

    def __init__(self, delta, length):
        self._delta = delta
        self._length = length

    @property
    def delta(self):
        return self._delta

    @property
    def length(self):
        return self._length

    @property
    def is_payload_marker(self):
        return self.delta == 0xF and self.length == 0xF

    @classmethod
    def _read_extended_value(cls, data, value):
        if value == 13:
            return ord(data.read(1)) + 13
        elif value == 14:
            data.read(1)
            return ord(data.read(1)) + 269
        else:
            return value

    @classmethod
    def from_bytes(cls, data):
        initial_byte = ord(data.read(1))

        delta = (initial_byte >> 4) & 0xF
        length = initial_byte & 0xF

        delta = cls._read_extended_value(data, delta)
        length = cls._read_extended_value(data, length)

        return cls(delta, length)


class CoapOption(object):
    """ Class representing CoAP option. """

    def __init__(self, _type, value):
        self._type = _type
        self._value = value

    @property
    def type(self):
        return self._type

    @property
    def value(self):
        return self._value

    def __repr__(self):
        return "CoapOption(type={}, value={})".format(self.type, hexlify(self.value))


class CoapOptionsFactory(object):
    """ Factory that produces CoAP options. """

    def parse(self, data, message_info):
        options = []

        _type = 0
        while data.tell() < len(data.getvalue()):
            option_header = CoapOptionHeader.from_bytes(data)
            if option_header.is_payload_marker:
                break

            _type += option_header.delta
            value = data.read(option_header.length)

            option = CoapOption(_type, value)
            options.append(option)

        return options


class CoapCode(object):
    """ Class representing CoAP code. """

    def __init__(self, code):
        self._code = code

    @property
    def code(self):
        return self._code

    @property
    def _class(self):
        return (self.code >> 5) & 0x7

    @property
    def detail(self):
        return self.code & 0x1F

    @classmethod
    def from_class_and_detail(cls, _class, detail):
        return cls(((_class & 0x7) << 5) | (detail & 0x1F))

    @classmethod
    def from_dotted(cls, dotted_str):
        _class, detail = dotted_str.split(".")
        return cls.from_class_and_detail(int(_class), int(detail))

    def is_equal_dotted(self, dotted_code):
        other = self.from_dotted(dotted_code)
        return self.code == other.code

    @property
    def dotted(self):
        return ".".join(["{:01d}".format(self._class), "{:02d}".format(self.detail)])

    def __eq__(self, other):
        if isinstance(other, int):
            return self.code == other

        elif isinstance(other, str):
            return self.is_equal_dotted(other)

        elif isinstance(other, self.__class__):
            return self.code == other.code

        else:
            raise TypeError("Could not compare {} and {}".format(type(self), type(other)))

    def __repr__(self):
        return self.dotted


class CoapMessage(object):
    """ Class representing CoAP message. """

    def __init__(
        self,
        version,
        _type,
        code,
        message_id,
        token,
        options,
        payload,
        uri_path=None,
    ):
        self._version = version
        self._type = _type
        self._code = code
        self._message_id = message_id
        self._token = token
        self._options = options
        self._payload = payload
        self._uri_path = uri_path

    @property
    def version(self):
        return self._version

    @property
    def type(self):
        return self._type

    @property
    def code(self):
        return self._code

    @property
    def message_id(self):
        return self._message_id

    @property
    def token(self):
        return self._token

    @property
    def tkl(self):
        return len(self._token)

    @property
    def options(self):
        return self._options

    @property
    def payload(self):
        return self._payload

    @property
    def uri_path(self):
        return self._uri_path

    def __repr__(self):
        options_str = ", ".join([repr(opt) for opt in self.options])
        return ("CoapMessage(version={}, type={}, code={}, message_id={}, token={}, options=[{}], payload={},",
                "uri-path='{}')").format(
                    self.version,
                    CoapMessageType.name[self.type],
                    self.code,
                    self.message_id,
                    hexlify(self.token),
                    options_str,
                    self.payload,
                    self.uri_path,
                )


class CoapMessageProxy(object):
    """ Proxy class of CoAP message.

    The main idea behind this class is to delay parsing payload. Due to architecture of the existing solution
    it is possible to process confirmation message before a request message. In such case it is not possible
    to get URI path to get proper payload parser.
    """

    def __init__(
        self,
        coap_message,
        message_info,
        mid_to_uri_path_binder,
        uri_path_based_payload_factories,
    ):
        self._coap_message = coap_message
        self._message_info = message_info
        self._mid_to_uri_path_binder = mid_to_uri_path_binder
        self._uri_path_based_payload_factories = (uri_path_based_payload_factories)

    @property
    def version(self):
        return self._coap_message.version

    @property
    def type(self):
        return self._coap_message.type

    @property
    def code(self):
        return self._coap_message.code

    @property
    def message_id(self):
        return self._coap_message.message_id

    @property
    def token(self):
        return self._coap_message.token

    @property
    def tkl(self):
        return self._coap_message.tkl

    @property
    def options(self):
        return self._coap_message.options

    @property
    def payload(self):
        try:
            binded_uri_path = self._mid_to_uri_path_binder.get_uri_path_for(self.message_id, self.token)

            factory = self._uri_path_based_payload_factories[binded_uri_path]

            return factory.parse(io.BytesIO(self._coap_message.payload), self._message_info)

        except RuntimeError:
            return self._coap_message.payload

    @property
    def uri_path(self):
        return self._coap_message.uri_path

    def __repr__(self):
        options_str = ", ".join([repr(opt) for opt in self.options])
        return ("CoapMessageProxy(version={}, type={}, code={}, message_id={}, token={}, options=[{}], payload={},",
                "uri-path='{}')").format(
                    self.version,
                    self.type,
                    self.code,
                    self.message_id,
                    hexlify(self.token),
                    options_str,
                    self.payload,
                    self.uri_path,
                )


class CoapMessageIdToUriPathBinder:
    """ Class binds message id and token with URI path. """

    def __init__(self):
        self._uri_path_binds = collections.defaultdict(collections.defaultdict)

    def add_uri_path_for(self, message_id, token, uri_path):
        self._uri_path_binds[message_id][hexlify(token)] = uri_path

    def get_uri_path_for(self, message_id, token):
        try:
            return self._uri_path_binds[message_id][hexlify(token)]
        except KeyError:
            raise RuntimeError("Could not find URI PATH for message_id: {} and token: {}".format(
                message_id, hexlify(token)))


class CoapMessageFactory(object):
    """ Factory that produces CoAP messages. """

    def __init__(
        self,
        options_factory,
        uri_path_based_payload_factories,
        message_id_to_uri_path_binder,
    ):
        self._options_factory = options_factory
        self._uri_path_based_payload_factories = (uri_path_based_payload_factories)
        self._mid_to_uri_path_binder = message_id_to_uri_path_binder

    def _uri_path_from(self, options):
        uri_path_options = []

        for option in options:
            if option.type == CoapOptionsTypes.URI_PATH:
                uri_path_options.append(option.value.decode("utf-8"))

        if not uri_path_options:
            return None

        return "/" + "/".join(uri_path_options)

    def _parse_initial_byte(self, data, message_info):
        initial_byte = ord(data.read(1))

        version = (initial_byte >> 6) & 0x3
        _type = CoapMessageType((initial_byte >> 4) & 0x3)
        token_length = initial_byte & 0xF

        return version, _type, token_length

    def parse(self, data, message_info):
        version, _type, token_length = self._parse_initial_byte(data, message_info)

        code = CoapCode(ord(data.read(1)))
        message_id = struct.unpack(">H", data.read(2))[0]
        token = data.read(token_length)

        options = self._options_factory.parse(data, message_info)

        uri_path = self._uri_path_from(options)
        if uri_path is not None:
            self._mid_to_uri_path_binder.add_uri_path_for(message_id, token, uri_path)

        coap_message = CoapMessage(
            version,
            _type,
            code,
            message_id,
            token,
            options,
            data.read(),
            uri_path,
        )

        return CoapMessageProxy(
            coap_message,
            message_info,
            self._mid_to_uri_path_binder,
            self._uri_path_based_payload_factories,
        )
