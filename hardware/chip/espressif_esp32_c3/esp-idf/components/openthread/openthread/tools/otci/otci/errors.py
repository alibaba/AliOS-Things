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
from typing import List


class OTCIError(Exception):
    """Base class for OTCI Errors."""
    pass


class ExpectLineTimeoutError(OTCIError):
    """OTCI failed to find an expected line before timeout."""

    def __init__(self, line):
        super(ExpectLineTimeoutError, self).__init__("Expected line %r, but timed out" % line)


class CommandError(OTCIError):
    """OTCI failed to execute a command."""

    def __init__(self, cmd: str, output: List[str]):
        self.__output = output
        super(CommandError, self).__init__("Command error while executing %r:\n%s\n" % (cmd, '\n'.join(output)))

    def error(self) -> str:
        return self.__output[-1]


class UnexpectedCommandOutput(OTCIError):
    """OTCI got unexpected command output."""

    def __init__(self, output: List[str]):
        super(UnexpectedCommandOutput, self).__init__("Unexpected command output:\n%s\n" % '\n'.join(output))


class InvalidArgumentsError(OTCIError):
    """Invalid arguments."""
    pass
