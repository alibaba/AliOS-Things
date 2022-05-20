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
import ctypes
import ctypes.util
import socket
import struct
import sys
import time

MYPORT = 8123
MYTTL = 1  # Increase to reach other networks

libc = ctypes.CDLL(ctypes.util.find_library('c'))


def if_nametoindex(name):
    if not isinstance(name, str):
        raise TypeError('name must be a string.')
    ret = libc.if_nametoindex(name.encode('ascii'))
    if not ret:
        raise RuntimeError("Invalid Name")
    return ret


def if_indextoname(index):
    if not isinstance(index, int):
        raise TypeError('index must be an int.')
    libc.if_indextoname.argtypes = [ctypes.c_uint32, ctypes.c_char_p]
    libc.if_indextoname.restype = ctypes.c_char_p

    ifname = ctypes.create_string_buffer(32)
    ifname = libc.if_indextoname(index, ifname)
    if not ifname:
        raise RuntimeError("Inavlid Index")
    return ifname


def main():
    args = sys.argv[1:]
    is_sender = False

    if args[0] == '-s':
        is_sender = True
        args.pop(0)

    ifname, group = args

    if is_sender:
        sender(ifname, group)
    else:
        receiver(ifname, group)


def sender(ifname, group):
    addrinfo = socket.getaddrinfo(group, None)[0]

    s = socket.socket(addrinfo[0], socket.SOCK_DGRAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_BINDTODEVICE, (ifname + '\0').encode('ascii'))

    # Set Time-to-live (optional)
    ttl_bin = struct.pack('@i', MYTTL)
    assert addrinfo[0] == socket.AF_INET6
    s.setsockopt(socket.IPPROTO_IPV6, socket.IPV6_MULTICAST_HOPS, ttl_bin)

    while True:
        data = repr(time.time())
        s.sendto((data + '\0').encode('ascii'), (addrinfo[4][0], MYPORT))
        time.sleep(1)


def receiver(ifname, group):
    # Look up multicast group address in name server and find out IP version
    addrinfo = socket.getaddrinfo(group, None)[0]
    assert addrinfo[0] == socket.AF_INET6

    # Create a socket
    s = socket.socket(addrinfo[0], socket.SOCK_DGRAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_BINDTODEVICE, (ifname + '\0').encode('ascii'))

    # Allow multiple copies of this program on one machine
    # (not strictly needed)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    # Bind it to the port
    s.bind(('', MYPORT))

    group_bin = socket.inet_pton(addrinfo[0], addrinfo[4][0])
    # Join group
    interface_index = if_nametoindex(ifname)
    mreq = group_bin + struct.pack('@I', interface_index)
    s.setsockopt(socket.IPPROTO_IPV6, socket.IPV6_JOIN_GROUP, mreq)

    # Loop, printing any data we receive
    while True:
        data, sender = s.recvfrom(1500)
        while data[-1:] == '\0':
            data = data[:-1]  # Strip trailing \0's
        print(str(sender) + '  ' + repr(data))


if __name__ == '__main__':
    main()
