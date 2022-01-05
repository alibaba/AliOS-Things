#!/usr/bin/env python
#
# Copyright 2020 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from __future__ import division
from __future__ import print_function
from __future__ import unicode_literals
import re
from tiny_test_fw import Utility
import ttfw_idf
import os
import six
import socket
import hashlib
import base64
import struct


OPCODE_TEXT = 0x1
OPCODE_BIN = 0x2
OPCODE_PING = 0x9
OPCODE_PONG = 0xa


class WsClient:
    def __init__(self, ip, port):
        self.port = port
        self.ip = ip
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.client_key = "abcdefghjk"
        self.socket.settimeout(10.0)

    def __enter__(self):
        self.socket.connect((self.ip, self.port))
        self._handshake()
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.socket.close()

    def _handshake(self):
        MAGIC_STRING = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
        client_key = self.client_key + MAGIC_STRING
        expected_accept = base64.standard_b64encode(hashlib.sha1(client_key.encode()).digest())
        request = ('GET /ws HTTP/1.1\r\nHost: localhost\r\nUpgrade: websocket\r\nConnection: '
                   'Upgrade\r\nSec-WebSocket-Key: {}\r\n'
                   'Sec-WebSocket-Version: 13\r\n\r\n'.format(self.client_key))
        self.socket.send(request.encode('utf-8'))
        response = self.socket.recv(1024)
        ws_accept = re.search(b'Sec-WebSocket-Accept: (.*)\r\n', response, re.IGNORECASE)
        if ws_accept and ws_accept.group(1) is not None and ws_accept.group(1) == expected_accept:
            pass
        else:
            raise("Unexpected Sec-WebSocket-Accept, handshake response: {}".format(response))

    def _masked(self, data):
        mask = struct.unpack('B' * 4, os.urandom(4))
        out = list(mask)
        for i, d in enumerate(struct.unpack('B' * len(data), data)):
            out.append(d ^ mask[i % 4])
        return struct.pack('B' * len(out), *out)

    def _ws_encode(self, data="", opcode=OPCODE_TEXT, mask=1):
        data = data.encode('utf-8')
        length = len(data)
        if length >= 126:
            raise("Packet length of {} not supported!".format(length))
        frame_header = chr(1 << 7 | opcode)
        frame_header += chr(mask << 7 | length)
        frame_header = six.b(frame_header)
        if not mask:
            return frame_header + data
        return frame_header + self._masked(data)

    def read(self):
        header = self.socket.recv(2)
        if not six.PY3:
            header = [ord(character) for character in header]
        opcode = header[0] & 15
        length = header[1] & 127
        payload = self.socket.recv(length)
        return opcode, payload.decode('utf-8')

    def write(self, data="", opcode=OPCODE_TEXT, mask=1):
        return self.socket.sendall(self._ws_encode(data=data, opcode=opcode, mask=mask))


@ttfw_idf.idf_example_test(env_tag="Example_WIFI")
def test_examples_protocol_http_ws_echo_server(env, extra_data):
    # Acquire DUT
    dut1 = env.get_dut("http_server", "examples/protocols/http_server/ws_echo_server", dut_class=ttfw_idf.ESP32DUT)

    # Get binary file
    binary_file = os.path.join(dut1.app.binary_path, "ws_echo_server.bin")
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance("http_ws_server_bin_size", "{}KB".format(bin_size // 1024))
    ttfw_idf.check_performance("http_ws_server_bin_size", bin_size // 1024, dut1.TARGET)

    # Upload binary and start testing
    Utility.console_log("Starting ws-echo-server test app based on http_server")
    dut1.start_app()

    # Parse IP address of STA
    Utility.console_log("Waiting to connect with AP")
    got_ip = dut1.expect(re.compile(r"IPv4 address: (\d+.\d+.\d+.\d+)"), timeout=60)[0]
    got_port = dut1.expect(re.compile(r"Starting server on port: '(\d+)'"), timeout=60)[0]

    Utility.console_log("Got IP   : " + got_ip)
    Utility.console_log("Got Port : " + got_port)

    # Start ws server test
    with WsClient(got_ip, int(got_port)) as ws:
        DATA = 'Espressif'
        for expected_opcode in [OPCODE_TEXT, OPCODE_BIN, OPCODE_PING]:
            ws.write(data=DATA, opcode=expected_opcode)
            opcode, data = ws.read()
            Utility.console_log("Testing opcode {}: Received opcode:{}, data:{}".format(expected_opcode, opcode, data))
            if expected_opcode == OPCODE_PING:
                dut1.expect("Got a WS PING frame, Replying PONG")
                if opcode != OPCODE_PONG or data != DATA:
                    raise RuntimeError("Failed to receive correct opcode:{} or data:{}".format(opcode, data))
                continue
            dut_data = dut1.expect(re.compile(r"Got packet with message: ([A-Za-z0-9_]*)"))[0]
            dut_opcode = int(dut1.expect(re.compile(r"Packet type: ([0-9]*)"))[0])
            if opcode != expected_opcode or data != DATA or opcode != dut_opcode or data != dut_data:
                raise RuntimeError("Failed to receive correct opcode:{} or data:{}".format(opcode, data))
        ws.write(data="Trigger async", opcode=OPCODE_TEXT)
        opcode, data = ws.read()
        Utility.console_log("Testing async send: Received opcode:{}, data:{}".format(opcode, data))
        if opcode != OPCODE_TEXT or data != "Async data":
            raise RuntimeError("Failed to receive correct opcode:{} or data:{}".format(opcode, data))


if __name__ == '__main__':
    test_examples_protocol_http_ws_echo_server()
