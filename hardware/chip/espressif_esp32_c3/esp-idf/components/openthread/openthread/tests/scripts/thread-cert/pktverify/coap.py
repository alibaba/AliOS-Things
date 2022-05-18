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
from typing import Tuple, List

from pktverify.consts import COAP_CODE_POST, COAP_CODE_ACK
from pktverify.layers import Layer


class CoapTlvParser(object):

    @staticmethod
    def parse(t, v: bytearray) -> List[Tuple[str, str]]:
        assert isinstance(v, bytearray)
        return []


class CoapLayer(Layer):
    """
    Represents a COAP layer.
    """

    def __init__(self, packet, layer_name):
        super().__init__(packet, layer_name)

    @property
    def is_post(self) -> bool:
        """
        Returns if the COAP layer is using code POST.
        """
        return self.code == COAP_CODE_POST

    @property
    def is_ack(self) -> bool:
        """
        Returns if the COAP layer is using code ACK.
        """
        return self.code == COAP_CODE_ACK

    def __getattr__(self, name):
        super_attr = super().__getattr__(name)
        if name == 'tlv':
            self._parse_coap_payload()

        return super_attr

    def _parse_coap_payload(self):
        payload = self.payload

        r = 0
        while True:
            t, tvs, r = self._parse_next_tlv(payload, r)
            if t is None:
                break

            self._add_field('coap.tlv.type', hex(t))
            for k, v in tvs:
                assert isinstance(k, str), (t, k, v)
                assert isinstance(v, str), (t, k, v)
                self._add_field('coap.tlv.' + k, v)

    @staticmethod
    def _parse_next_tlv(payload, read_pos) -> tuple:
        assert read_pos <= len(payload)
        if read_pos == len(payload):
            return None, None, read_pos

        t = payload[read_pos]
        len_ = payload[read_pos + 1]
        assert (len(payload) - read_pos - 2 >= len_)
        kvs = CoapTlvParser.parse(t, payload[read_pos + 2:read_pos + 2 + len_])
        return t, kvs, read_pos + len_ + 2
