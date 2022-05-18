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
import ipaddress
from typing import Union

from pktverify.bytes import Bytes


class EthAddr(Bytes):
    """
    Represents an Ethernet address.
    """

    def __init__(self, addr: Union[str, bytearray, 'Bytes']):
        super().__init__(addr)
        if len(self) not in (6, 8):
            raise ValueError((addr, self))

    def __repr__(self):
        return '%s(%r)' % (self.__class__.__name__, self.format_octets())

    def __str__(self):
        return self.format_octets()


class ExtAddr(Bytes):
    """
    Represents an WPAN Extended address.
    """

    def __init__(self, addr: Union[str, bytearray, 'Bytes']):
        super().__init__(addr)
        if len(self) != 8:
            raise ValueError((addr, self))

    def __repr__(self):
        return '%s(%r)' % (self.__class__.__name__, self.format_octets())

    def __str__(self):
        return self.format_octets()


class Ipv6Addr(Bytes):
    """
    Represents an Ip6 address.
    """

    def __init__(self, addr: Union[str, bytearray, 'Bytes']):
        if isinstance(addr, str):
            # try to parse compacted ipv6 address
            try:
                addr = Ipv6Addr._expand(addr)
            except ipaddress.AddressValueError:
                pass

        super().__init__(addr)
        if len(self) != 16:
            raise ValueError(addr)
        self._addr = ipaddress.IPv6Address(self)

    def __repr__(self):
        return '%s(%r)' % (self.__class__.__name__, self.format_hextets())

    def __str__(self):
        return self.format_hextets()

    @staticmethod
    def _expand(addr) -> str:
        assert isinstance(addr, str)
        a = ipaddress.IPv6Address(addr)
        return a.exploded

    @property
    def is_global(self) -> bool:
        """
        Returns if the Ip6 address is global.
        """
        if self._addr.is_global:
            return True

        if self._addr.is_link_local or self._addr.is_multicast or self._addr.is_loopback:
            return False

        return True

    @property
    def is_dua(self) -> bool:
        """
        Returns if the Ip6 address is Domain Unicast Address.
        """
        from pktverify import consts
        return self.startswith(consts.DOMAIN_PREFIX)

    @property
    def is_backbone_gua(self) -> bool:
        """
        Returns if the Ip6 address is Backbone address.
        """
        from pktverify import consts
        return self.startswith(consts.BACKBONE_IPV6_PREFIX)

    @property
    def is_link_local(self) -> bool:
        """
        Returns if the Ip6 address is link local.
        """
        return self._addr.is_link_local

    @property
    def is_multicast(self) -> bool:
        """
        Returns if the Ip6 address is multicast.
        """
        return self._addr.is_multicast

    @property
    def is_mleid(self) -> bool:
        """
        Returns if the Ip6 address is ML-EID.
        """
        from pktverify.consts import DEFAULT_MESH_LOCAL_PREFIX
        return self.startswith(DEFAULT_MESH_LOCAL_PREFIX)


if __name__ == '__main__':
    a = EthAddr("010203040506")
    assert a == EthAddr("01:02:03:04:05:06")
    assert a == EthAddr("0102:0304:0506")
    assert eval(repr(a)) == a
    assert a == str(a)
    assert str(a) == a

    print(a, repr(a))
    assert isinstance(a[:], Bytes)
    assert a[:3] == "010203"
    assert a[3:] == "040506"

    a = ExtAddr("0102030405060708")
    assert a == ExtAddr("01:02:03:04:05:06:07:08")
    assert a == ExtAddr("0102:0304:0506:0708")
    assert eval(repr(a)) == a
    assert a == str(a)
    assert str(a) == a

    print(a, repr(a))
    assert isinstance(a[:], Bytes)
    assert a[:4] == "01020304"
    assert a[4:] == "05060708"

    a = Ipv6Addr("00112233445566778899aabbccddeeff")
    assert a == Ipv6Addr("00:11:22:33:44:55:66:77:88:99:aa:bb:cc:dd:ee:ff")
    assert a == Ipv6Addr("0011:2233:4455:6677:8899:aabb:ccdd:eeff")
    assert eval(repr(a)) == a
    assert a == str(a)
    assert str(a) == a

    print(a, repr(a))
    assert isinstance(a[:], Bytes)
    assert a[:4] == "00112233"
    assert a[-4:] == "ccddeeff"

    assert Ipv6Addr("fd00:db8::ff:fe00:8001") == Ipv6Addr("fd00:0db8:0000:0000:0000:00ff:fe00:8001")
    print(Ipv6Addr("fdde:ad00:beef:0:9d87:85f0:3358:3fff"))
