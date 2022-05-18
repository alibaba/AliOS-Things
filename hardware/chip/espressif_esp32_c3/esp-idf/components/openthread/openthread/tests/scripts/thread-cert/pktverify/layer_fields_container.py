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
from pyshark.packet.packet import Packet as RawPacket

from pktverify.layer_fields import get_layer_field, check_layer_field_exists


class LayerFieldsContainer(object):
    """
    Represents a layer field container.
    """

    def __init__(self, packet: RawPacket, path: str):
        assert isinstance(packet, RawPacket)
        assert isinstance(path, str)

        self._packet = packet
        self._path = path

    def __getattr__(self, name):
        subpath = self._path + '.' + name
        v = get_layer_field(self._packet, subpath)
        setattr(self, name, v)
        return v

    def has(self, subpath):
        """
        Returns if the layer field container has a sub layer field or container.

        :param subpath: The sub path to the layer field or container.
        """
        subpath = self._path + '.' + subpath
        return check_layer_field_exists(self._packet, subpath) is not None

    def __getitem__(self, item):
        return getattr(self, item)

    @property
    def full_path(self):
        """
        Returns the full path to this layer field container.
        """
        return self._path

    @property
    def field_path(self):
        """
        Returns the field that references to this layer field container.
        """
        secs = self._path.split('.')
        assert len(secs) >= 2
        return '.'.join(secs[1:])

    def __bool__(self):
        """
        Returns if this layer field container exists in the packet.
        """
        return check_layer_field_exists(self._packet, self._path)
