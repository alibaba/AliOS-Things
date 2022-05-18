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

import socket
import struct
import subprocess
import sys
import time

from ipaddress import ip_address


def get_maddrs(if_name):
    lines = subprocess.run(['ot-ctl', '-I', if_name, 'ipmaddr'], stdout=subprocess.PIPE).stdout.decode().split()
    return [ip_address(l) for l in lines if l.startswith('ff')]


def main():
    group = 'ff02::158'
    if_index = int(sys.argv[1])
    if_name = sys.argv[2]

    with socket.socket(socket.AF_INET6, socket.SOCK_DGRAM) as s:
        s.setsockopt(socket.IPPROTO_IPV6, socket.IPV6_MULTICAST_IF, if_index)
        s.setsockopt(socket.IPPROTO_IPV6, socket.IPV6_JOIN_GROUP,
                     struct.pack('16si', socket.inet_pton(socket.AF_INET6, group), if_index))
        time.sleep(2)
        maddrs = get_maddrs(if_name)
        print(maddrs)
        if not any(addr == ip_address(group) for addr in maddrs):
            return -1

        s.setsockopt(socket.IPPROTO_IPV6, socket.IPV6_LEAVE_GROUP,
                     struct.pack('16si', socket.inet_pton(socket.AF_INET6, group), if_index))

        time.sleep(2)
        maddrs = get_maddrs(if_name)
        print(maddrs)
        if any(addr == ip_address(group) for addr in maddrs):
            return -1
    return 0


if __name__ == '__main__':
    exit(main())
