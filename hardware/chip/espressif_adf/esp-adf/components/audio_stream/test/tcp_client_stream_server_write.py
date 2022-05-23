#!/user/bin/env python

#  ESPRESSIF MIT License
#
#  Copyright (c) 2020 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
#
#  Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
#  it is free of charge, to any person obtaining a copy of this software and associated
#  documentation files (the "Software"), to deal in the Software without restriction, including
#  without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
#  and/or sell copies of the Software, and to permit persons to whom the Software is furnished
#  to do so, subject to the following conditions:
#
#  The above copyright notice and this permission notice shall be included in all copies or
#  substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
#  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
#  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
#  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
#  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

import socket
import sys

SERVER_PORT   = 8000
RECV_BUF_SIZE = 8194
 
def start_tcp_server(ip, port):

    fo = open("esp32.mp3", "w+")
    print ("create a file named esp32.p3")

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_address = (ip, port)

    print("starting listen on ip %s, port %s" % server_address)
    sock.bind(server_address)
 
    sock.settimeout(1000)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, RECV_BUF_SIZE)
 
    try:
        sock.listen(1)
    except socket.error:
        print("fail to listen on port %s" % e)
        sys.exit(1)
    while True:
        print("waiting for client to connect")
        client, addr = sock.accept()
        break

    while True:
        msg = client.recv(8194)
        print('recv data len ', len(msg));
        if len(msg) <= 0:
            break
        fo.write(msg)
 
    fo.close()
    client.close()
    sock.close() 
    print(" close client connect ")
 
if __name__=='__main__':
    start_tcp_server(socket.gethostbyname(socket.getfqdn(socket.gethostname())), SERVER_PORT)
