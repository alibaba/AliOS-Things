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
import sys
from typing import Union, Any


class Bytes(bytearray):
    """Bytes represents a byte array which is able to handle strings of flexible formats"""

    def __init__(self, s: Union[str, bytearray, 'Bytes', Any]):
        if isinstance(s, str):
            try:
                s = Bytes._parse_compact(s)
            except ValueError:
                try:
                    s = Bytes._parse_octets(s)
                except ValueError:
                    s = Bytes._parse_hextets(s)

        super().__init__(s)

    def __hash__(self):
        return hash(bytes(self))

    def __repr__(self):
        return '%s(%r)' % (self.__class__.__name__, self.format_compact())

    def format_compact(self) -> str:
        """
        Converts the Bytes to a compact string (without ":").
        """
        return ''.join('%02x' % b for b in self)

    def format_octets(self) -> str:
        """
        Converts the Bytes to a string of octets separated by ":".
        """
        return ':'.join('%02x' % b for b in self)

    def format_hextets(self) -> str:
        """
        Converts the Bytes to a string of hextets separated by ":"
        """
        assert len(self) % 2 == 0, self.format_octets()
        return ':'.join('%04x' % (self[i] * 256 + self[i + 1]) for i in range(0, len(self), 2))

    __str__ = format_octets

    @staticmethod
    def _parse_compact(s: str) -> bytearray:
        try:
            assert len(s) % 2 == 0
            return bytearray(int(s[i:i + 2], 16) for i in range(0, len(s), 2))
        except Exception:
            raise ValueError(s)

    @staticmethod
    def _parse_octets(s: str) -> bytearray:
        try:
            assert len(s) % 3 == 2 or not s
            if not s:
                return bytearray(b"")

            return bytearray(int(x, 16) for x in s.split(':'))
        except Exception:
            raise ValueError(s)

    @staticmethod
    def _parse_hextets(s) -> bytearray:
        try:
            assert len(s) % 5 == 4 or not s
            if not s:
                return bytearray(b"")

            return bytearray(int(x[i:i + 2], 16) for x in s.split(':') for i in (0, 2))
        except Exception:
            raise ValueError(s)

    def __getitem__(self, item) -> Union['Bytes', int]:
        """
        Get self[item].

        :param item: index or slice to retrieve
        :return: the byte value at specified index or sub `Bytes` if item is slice
        """
        x = super().__getitem__(item)
        if isinstance(x, bytearray):
            return Bytes(x)
        else:
            return x

    def __eq__(self, other: Union[str, 'Bytes']):
        """
        Check if bytes is equal to other.
        """
        if other is None:
            return False
        elif not isinstance(other, Bytes):
            other = self.__class__(other)

        eq = super().__eq__(other)
        print("[%r %s %r]" % (self, "==" if eq else "!=", other), file=sys.stderr)
        return eq


if __name__ == '__main__':
    # some simple tests
    x = Bytes(b"\x01\x02\x03\x04")
    assert eval(repr(x)) == x, repr(x)  # representation of Bytes should be able to be evaluated back
    assert x == str(x), (x, str(x))

    assert x.format_compact() == "01020304", x.format_compact()
    assert x.format_octets() == "01:02:03:04", x.format_octets()
    assert x.format_hextets() == "0102:0304", x.format_hextets()

    assert Bytes._parse_compact("") == Bytes(b"")
    assert Bytes._parse_compact('01020304') == x

    assert Bytes._parse_octets("") == Bytes(b"")
    assert Bytes._parse_octets('01:02:03:04') == x

    assert Bytes._parse_hextets("") == Bytes(b"")
    assert Bytes._parse_hextets('0102:0304') == x

    assert isinstance(x[:2], Bytes)
    assert isinstance(x[-2:], Bytes)
    assert x[:2] == Bytes(b'\x01\x02')
    assert x[-2:] == Bytes(b'\x03\x04')

    # should also parse string formats
    assert Bytes("01020304") == Bytes(b"\x01\x02\x03\x04")
    assert Bytes("01:02:03:04") == Bytes(b"\x01\x02\x03\x04")
    assert Bytes("0102:0304") == Bytes(b"\x01\x02\x03\x04")
