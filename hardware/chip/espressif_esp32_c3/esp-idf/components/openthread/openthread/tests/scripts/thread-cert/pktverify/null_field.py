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

nullField = None


class NullField(object):
    """
    Represents a null field that does not exists.
    """

    def __new__(cls, *args, **kwargs):
        global nullField

        if nullField is None:
            nullField = object.__new__(cls, *args, **kwargs)

        return nullField

    def __init__(self):
        assert self is nullField

    def __bool__(self):
        """
        NullField is always treated as False.
        """
        return False

    def __getattr__(self, item):
        """
        Any sub field of the NullField is NullField itself.
        """
        return self

    def __setattr__(self, key, value):
        pass

    def __len__(self) -> 0:
        return 0

    def __eq__(self, other):
        """
        NullField is always not equal to any other value.
        """
        return False

    def __ne__(self, other):
        return True

    def __lt__(self, other):
        """
        Comparing NullField to any other value gets False.
        """
        return False

    def __le__(self, other):
        """
        Comparing NullField to any other value gets False.
        """
        return False

    def __gt__(self, other):
        """
        Comparing NullField to any other value gets False.
        """
        return False

    def __ge__(self, other):
        """
        Comparing NullField to any other value gets False.
        """
        return False

    def __str__(self):
        return "nullField"

    def __repr__(self):
        return 'nullField'


NullField()

if __name__ == '__main__':
    assert nullField is NullField()
    assert not nullField, repr(nullField)
    assert nullField != nullField, repr(nullField)
    assert nullField != 0
    assert not (nullField > 1)
    assert not (nullField < 1)
    assert not (nullField < nullField)
    assert not (nullField > nullField)
    assert bool(nullField) is False
    assert nullField != ""
    assert nullField != None  # noqa
    assert nullField is not None
