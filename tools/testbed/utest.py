#!/usr/bin/env python

import sys, os

DEFAULT_SERVER_PORT = 34567

def print_usage():
    print "Usage: {0} mode [-s xxx.xxx.xx.xx] [-p xxxxx]\n".format(sys.argv[0])
    print "  example 1: server run at 192.168.1.10@default_port"
    print "    server: {0} server".format(sys.argv[0])
    print "    client: {0} client -s 192.168.1.10".format(sys.argv[0])
    print "    terminal: {0} terminal -s 192.168.1.10\n".format(sys.argv[0])
    print "  example 2: server run at 192.168.1.18@12345"
    print "    server: {0} server -p 12345".format(sys.argv[0])
    print "    terminal: {0} client -s 192.168.1.18 -p 12345".format(sys.argv[0])
    print "    terminal: {0} terminal -s 192.168.1.18 -p 12345\n".format(sys.argv[0])

if len(sys.argv) < 2:
    print_usage()
    sys.exit(1)

server_port = DEFAULT_SERVER_PORT
if sys.argv[1] == "client":
    from client import Client
    if len(sys.argv) < 4 or sys.argv[2] != "-s":
        print_usage()
        sys.exit(1)

    if os.path.exists('client') == False:
        os.mkdir('client')
    tmpfile = '.testbed_client'
    if os.name == 'posix':
        tmpfile = '/tmp/' + tmpfile
    elif os.name == 'nt':
        tmpfile = os.path.expanduser('~') + '\\' + tmpfile
    if os.path.exists(tmpfile):
        print "An urader testbed client is already running"
        sys.exit(0)

    server_ip = sys.argv[3]
    if len(sys.argv) >= 6 and sys.argv[4] == '-p':
        try:
            server_port = int(sys.argv[5])
            if server_port<0 or server_port > 65534:
                raise BaseException
        except:
            print "Usage error: invalid server port"
            print_usage()
            sys.exit(1)
    open(tmpfile, 'a').close()
    client = Client()
    client.client_func(server_ip, server_port)
    os.remove(tmpfile)
elif sys.argv[1] == "server":
    from server import Server
    if len(sys.argv) >= 4 and sys.argv[2] == '-p':
        try:
            server_port = int(sys.argv[3])
            if server_port<0 or server_port > 65534:
                raise BaseException
        except:
            print "Usage error: invalid server port"
            print_usage()
            sys.exit(1)

    if os.path.exists('server') == False:
        os.mkdir('server')
    tmpfile = ".testbed_server_{0}".format(server_port)
    if os.name == 'posix':
        tmpfile = '/tmp/' + tmpfile
    elif os.name == 'nt':
        tmpfile = os.path.expanduser('~') + '\\' + tmpfile
    if os.path.exists(tmpfile):
        print "An urader testbed server is already running at port", server_port
        sys.exit(0)
    open(tmpfile, 'a').close()
    server = Server()
    server.server_func(server_port)
    os.remove(tmpfile)
elif sys.argv[1] == "terminal":
    from terminal import Terminal
    if len(sys.argv) < 4 or sys.argv[2] != "-s":
        print_usage()
        sys.exit(1)
    server_ip = sys.argv[3]
    if len(sys.argv) >= 6 and sys.argv[4] == '-p':
        try:
            server_port = int(sys.argv[5])
            if server_port<0 or server_port > 65534:
                raise BaseException
        except:
            print "Usage error: invalid server port"
            print_usage()
            sys.exit(1)
    if os.path.exists('terminal') == False:
        os.mkdir('terminal')
    terminal = Terminal()
    terminal.terminal_func(server_ip, server_port)
else:
    print_usage()
    sys.exit(0)
