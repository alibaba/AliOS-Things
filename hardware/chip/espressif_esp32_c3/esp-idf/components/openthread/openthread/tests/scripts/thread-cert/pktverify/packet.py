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
import logging
import sys
from typing import Iterable, List, Union, Callable

from pyshark.packet.layer import Layer as RawLayer
from pyshark.packet.packet import Packet as RawPacket

from pktverify import errors
from pktverify.addrs import EthAddr
from pktverify.coap import CoapLayer
from pktverify.consts import VALID_LAYER_NAMES
from pktverify.decorators import cached_property
from pktverify.layers import Layer, ThreadMeshcopLayer, Icmpv6Layer, WpanLayer, ThreadNetworkDataLayer
from pktverify.utils import make_filter_func


class Packet(object):

    def __init__(self, packet: RawPacket):
        self._packet = packet
        self._strip_wpan_eth_wrapper(packet)

    def __str__(self) -> str:
        return str(self._packet)

    def __repr__(self) -> str:
        return repr(self._packet)

    def __dir__(self) -> Iterable[str]:
        return dir(self._packet)

    def _strip_wpan_eth_wrapper(self, packet: RawPacket):
        if not hasattr(packet, 'wpan'):
            return

        for layer in packet.layers:
            if layer.layer_name == 'eth':
                packet.layers.remove(layer)
                eth_src = EthAddr(layer.get_field('eth.src'))
                eth_dst = EthAddr(layer.get_field('eth.dst'))
                logging.debug("stripping eth: src=%s, dst=%s", eth_src, eth_dst)
                channel = eth_src[5]
                self.wpan._add_field('wpan.channel', hex(channel))

        return

    @property
    def layers(self) -> Iterable[RawLayer]:
        for l in self._packet.layers:
            if l.layer_name != 'data':
                yield getattr(self, l.layer_name)

    @property
    def layer_names(self) -> List[str]:
        return [l.layer_name for l in self._packet.layers if l.layer_name != 'data']

    @cached_property
    def wpan(self) -> WpanLayer:
        return WpanLayer(self._packet, 'wpan')

    @cached_property
    def coap(self) -> CoapLayer:
        return CoapLayer(self._packet, 'coap')

    @cached_property
    def icmpv6(self) -> Icmpv6Layer:
        return Icmpv6Layer(self._packet, 'icmpv6')

    @cached_property
    def thread_meshcop(self) -> ThreadMeshcopLayer:
        return ThreadMeshcopLayer(self._packet, 'thread_meshcop')

    @cached_property
    def thread_nwd(self) -> ThreadNetworkDataLayer:
        return ThreadNetworkDataLayer(self._packet, 'thread_nwd')

    def __getattr__(self, layer_name: str) -> Layer:

        real_layer_name = layer_name
        if layer_name == 'lowpan':
            real_layer_name = '6lowpan'

        assert real_layer_name in VALID_LAYER_NAMES, '%s is not a valid layer name' % real_layer_name
        _layer = getattr(self._packet, real_layer_name, None)
        assert _layer is None or isinstance(_layer, RawLayer)

        layer = Layer(self._packet, real_layer_name)
        setattr(self, layer_name, layer)
        if real_layer_name != layer_name:
            setattr(self, real_layer_name, layer)

        return layer

    def verify(self, func: Union[str, Callable], **vars) -> bool:
        print("\n>>> verifying packet:", file=sys.stderr, flush=False)
        func = make_filter_func(func, **vars)
        ok = func(self)
        print("\t=> %s" % ok, file=sys.stderr)
        return ok

    def must_verify(self, func: Union[str, Callable], **vars):
        if not self.verify(func, **vars):
            self.debug_fields()
            raise errors.VerifyFailed(self)

        return self

    def must_not_verify(self, func: Union[str, Callable], **vars):
        if self.verify(func, **vars):
            raise errors.VerifyFailed(self)

        return self

    @property
    def sniff_timestamp(self) -> float:
        return float(self._packet.sniff_timestamp)

    def show(self):
        self._packet.show()

    def debug_fields(self):
        for layer in self._packet.layers:
            print("### Layer %s ###" % layer.layer_name)
            for k, v in layer._all_fields.items():
                print("\t\t%r = %r" % (k, v))
