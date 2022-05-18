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
from typing import Optional

from pyshark.packet.fields import LayerField, LayerFieldsContainer
from pyshark.packet.layer import Layer as RawLayer
from pyshark.packet.packet import Packet as RawPacket

from pktverify.layer_fields import get_layer_field, check_layer_field_exists


class Layer(object):
    """
    Represents a layer of a packet.
    """

    def __init__(self, packet: RawPacket, layer_name: str):
        assert isinstance(packet, RawPacket)
        assert isinstance(layer_name, str)
        self._packet = packet
        self._layer_name = layer_name

    @property
    def _layer(self) -> Optional[RawLayer]:
        try:
            return getattr(self._packet, self._layer_name)
        except AttributeError:
            return None

    @property
    def layer_name(self) -> str:
        """
        Returns the layer name.
        """
        return self._layer_name

    def show(self):
        """
        Print the layer information.
        """
        print(self._layer)

    def has(self, name) -> bool:
        """
        Returns if the layer has a given field.

        :param name: The field name.
        """
        path = '%s.%s' % (self.layer_name, name)
        return check_layer_field_exists(self._packet, path)

    def __bool__(self):
        """
        Returns if this layer exists in the packet.
        """
        layer_exists = hasattr(self._packet, self._layer_name)
        return layer_exists

    def __getattr__(self, name):
        """
        Returns the layer field or container of a given field name.

        :param name: The name of layer field or container.
        """
        path = '%s.%s' % (self.layer_name, name)
        v = get_layer_field(self._packet, path)
        assert not isinstance(v, (LayerField, LayerFieldsContainer)), '%s = %s(%r)' % (path, v.__class__.__name__, v)
        setattr(self, name, v)
        return v

    def _add_field(self, key: str, val: str):
        logging.debug("layer %s add field: %s = %s", self.layer_name, key, val)
        field = LayerField(name=key, value=val)
        all_fields = self._layer._all_fields
        if key not in all_fields:
            all_fields[key] = LayerFieldsContainer(main_field=field)
        else:
            all_fields[key].fields.append(field)


class ThreadMeshcopLayer(Layer):
    """
    Represents the Thread MeshCop layer of a packet.
    """

    def __bool__(self):
        raise NotImplementedError("thread_meshcop is not a real layer, please do not check as bool")


class ThreadNetworkDataLayer(Layer):
    """
    Represents the Thread NetworkData layer of a packet.
    """

    def __bool__(self):
        raise NotImplementedError("thread_nwd is not a real layer, please do not check as bool")


class Icmpv6Layer(Layer):
    """
    Represents the ICMPv6 layer of a packet.
    """

    @property
    def is_ping(self) -> bool:
        """
        Returns if the ICMPv6 layer is a Ping Request or Reply.
        """
        return self.type in (128, 129)

    @property
    def is_ping_request(self) -> bool:
        """
        Returns if the ICMPv6 layer is a Ping Request.
        """
        return self.type == 128

    @property
    def is_ping_reply(self) -> bool:
        """
        Returns if the ICMPv6 layer is a Ping Reply.
        """
        return self.type == 129

    @property
    def is_neighbor_advertisement(self) -> bool:
        """
        Returns if the ICMPv6 layer is a Neighbor Advertisement.
        """
        return self.type == 136

    @property
    def is_neighbor_solicitation(self) -> bool:
        """
        Returns if the ICMPv6 layer is a Neighbor Solicitation.
        """
        return self.type == 135

    @property
    def is_router_advertisement(self) -> bool:
        """
        Returns if the ICMPv6 layer is a Router Advertisement.
        """
        return self.type == 134


class WpanLayer(Layer):
    """
    Represents the WPAN layer of a packet.
    """

    @property
    def is_ack(self) -> bool:
        return self.frame_type == 0x2
