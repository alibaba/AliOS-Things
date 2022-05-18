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

import io
import logging


class UnknownTlv(object):

    def __init__(self, type, data):
        self.type = type
        self.data = data

    def __repr__(self):
        return 'UnknownTlv(%d, %s)' % (self.type, self.data)


class UnknownTlvFactory(object):

    def __init__(self, type):
        self._type = type

    def parse(self, data, message_info):
        return UnknownTlv(self._type, data)


class SubTlvsFactory(object):

    def __init__(self, sub_tlvs_factories):
        self._sub_tlvs_factories = sub_tlvs_factories

    def _get_factory(self, _type):
        try:
            return self._sub_tlvs_factories[_type]
        except KeyError:
            logging.error('Could not find TLV factory. Unsupported TLV type: {}'.format(_type))
            return UnknownTlvFactory(_type)

    def parse(self, data, message_info):
        sub_tlvs = []

        while data.tell() < len(data.getvalue()):
            _type = ord(data.read(1))

            length = ord(data.read(1))
            value = data.read(length)

            factory = self._get_factory(_type)

            message_info.length = length
            tlv = factory.parse(io.BytesIO(value), message_info)

            sub_tlvs.append(tlv)

        return sub_tlvs
