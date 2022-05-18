#!/usr/bin/env python3
#
#  Copyright (c) 2020, The OpenThread Authors.
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
from collections import namedtuple


class ChildId(int):
    """Represents a Child ID."""
    pass


class RouterId(int):
    """Represents a Router ID."""
    pass


class Rloc16(int):
    """Represents a RLOC16."""

    def __repr__(self):
        return '0x%04x' % self


class PartitionId(int):
    """Represents a Thread Network Partition ID."""
    pass


class NetifIdentifier(int):
    """Represents a network interface identifier."""
    UNSPECIFIED = 0
    THERAD = 1
    BACKBONE = 2


class DeviceMode(str):
    """Represents a device mode."""

    def __new__(cls, o: str):
        ins = str.__new__(cls, o)

        if ins != '-':
            for c in ins:
                if c not in 'rdn':
                    raise ValueError(o)

        # check for empty mode (SED should use "-")
        if not ins:
            raise ValueError(o)

        # check for duplicate chars
        if len(ins) != len(set(ins)):
            raise ValueError(o)

        return ins


class ThreadState(str):
    """Represents a Thread state."""
    _VALID_VALUES = {'disabled', 'detached', 'child', 'router', 'leader'}

    def __new__(cls, o: str):
        ins = str.__new__(cls, o)

        if ins not in ThreadState._VALID_VALUES:
            raise ValueError(o)

        return ins


class Ip6Addr(ipaddress.IPv6Address):
    """Represents an IPv6 address."""

    def __eq__(self, other):
        if isinstance(other, str):
            other = ipaddress.IPv6Address(other)

        return super().__eq__(other)

    def __repr__(self):
        return self.compressed

    def __hash__(self):
        return super().__hash__()


class Ip6Prefix(ipaddress.IPv6Network):
    """Represents an IPv6 prefix."""

    def __eq__(self, other):
        if isinstance(other, str):
            other = ipaddress.IPv6Network(other)

        return super().__eq__(other)

    def __repr__(self):
        return self.compressed

    def __hash__(self):
        return super().__hash__()


SecurityPolicy = namedtuple('SecurityPolicy', ['rotation_time', 'flags'])
"""Represents a Security Policy configuration."""


class RouterTableEntry(dict):

    @property
    def is_link_established(self):
        return bool(self['link'])


if __name__ == '__main__':
    assert Ip6Addr('2001:0:0:0:0:0:0:1') == '2001::1'
    assert repr(Ip6Addr('2001:0:0:0:0:0:0:1')) == '2001::1'
    assert str(Ip6Addr('2001:0:0:0:0:0:0:1')) == '2001::1'
    assert Ip6Prefix('2001:0:0:0::/64') == '2001::/64'
    assert repr(Ip6Prefix('2001:0:0:0::/64')) == '2001::/64'
    assert str(Ip6Prefix('2001:0:0:0::/64')) == '2001::/64'
