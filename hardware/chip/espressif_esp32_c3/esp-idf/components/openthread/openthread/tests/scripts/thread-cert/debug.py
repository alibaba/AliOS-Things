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

from builtins import input


class Inspector:
    """This class provides a way to inspect node status of a test case.

    USAGE:
        `#` mode
            This is selection mode. You may select the node to inspect here.

            `list`      - list available nodes.
            `exit`      - end inspecting, continue running test case.
            <numbers>   - select the node with id <number>. This will result in enter `>  mode.

        `>` mode
            This is node mode. You may run OpenThread CLI here.
            `exit`      - go back to `#` mode.

    EXAMPLE:
        #
        # 1
        > state
        leader
        > exit
        # 2
        > panid
        face
        > exit
        # exit
    """

    def __init__(self, test_case):
        """ Initialize a inspector.

        Args:
            test_case: The test case to inspect
        """
        self.test_case = test_case

    def inspect_node(self, nodeid):
        """ Inspect the node with the given nodeid.

        Args:
            nodeid: key in self.test_case.nodes.
        """
        node = self.test_case.nodes[nodeid]
        while True:
            line = input('> ')
            if not line:
                continue

            if line == 'exit':
                break
            else:
                node.send_command(line)
                node._expect('Done')

    def inspect(self):
        """ Start inspecting.
        """
        while True:
            line = input('# ')
            if not line:
                continue

            if line.isdigit():
                self.inspect_node(int(line))
            elif line == 'list':
                print(self.test_case.nodes.keys())
            elif line == 'exit':
                break
