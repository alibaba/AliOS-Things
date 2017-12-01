#!/usr/bin/python

import os, sys, time, socket, re, pdb
import subprocess, thread, threading, pickle
from operator import itemgetter
import TBframe
sys.path.append('../')

MAX_MSG_LENTH = 2000
DEBUG = True

class Autotest:
    def __init__(self):
        self.keep_running = True
        self.device_list= {}
        self.service_socket = 0

    def heartbeat_func(self):
        heartbeat_timeout = time.time() + 10
        while self.keep_running:
            time.sleep(0.05)
            if time.time() >= heartbeat_timeout:
                try:
                    self.service_socket.send(TBframe.construct(TBframe.HEARTBEAT, ''))
                    heartbeat_timeout += 10
                except:
                    continue
        print 'heart beat thread exited'

    def server_interaction(self):
        msg = ''
        while self.keep_running:
            try:
                new_msg = self.service_socket.recv(MAX_MSG_LENTH)
                if new_msg == '':
                    print 'connection lost'
                    break

                msg += new_msg
                while msg != '':
                    type, length, value, msg = TBframe.parse(msg)
                    if type == TBframe.TYPE_NONE:
                        break

                    #print type, length
                    if type == TBframe.ALL_DEV:
                        new_list = {}
                        clients = value.split(':')
                        for c in clients:
                            if c == '':
                                continue
                            devs = c.split(',')
                            uuid = devs[0]
                            devs = devs[1:]
                            for d in devs:
                                if d == '':
                                    continue
                                [dev, using] = d.split('|')
                                new_list[uuid + ',' + dev] = using

                        old_list = list(self.device_list)
                        for dev in list(new_list):
                            if dev not in old_list:
                                data = TBframe.construct(TBframe.STATUS_SUB, dev)
                                self.service_socket.send(data)
                            self.device_list[dev] = new_list[dev]

                        for dev in list(self.device_list):
                            if dev in list(new_list):
                                continue
                            self.device_list.pop(dev)
                    if type == TBframe.DEVICE_STATUS:
                        print value
            except:
                if DEBUG:
                    raise
                break
        self.keep_running = False;
        print 'server interaction thread exited'

    def start(self, server_ip, server_port, logname=None):
        #connect to server
        self.service_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            self.service_socket.connect((server_ip, server_port))
        except:
            print "connect to server {0}:{1} failed".format(server_ip, server_port)
            return False

        self.logfile = None
        if logname != None:
            if os.path.exists('testlog') == False:
                subprocess.call(['mkdir','testlog'])

            try:
                self.logfile = open("testlog/"+logname, 'w');
            except:
                print "open logfile {0} failed".format(logfile)
                return False

        thread.start_new_thread(self.server_interaction, ())
        thread.start_new_thread(self.heartbeat_func, ())
        time.sleep(0.5)
        return True

    def stop(self):
        self.keep_running = False
        time.sleep(0.2)
        self.service_socket.close()

    def is_running(self):
        return self.keep_running


if __name__ == '__main__':
    server_ip = '10.125.52.132'
    for arg in sys.argv[1:]:
        if arg.startswith("--sever="):
            args = arg.split('=')
            if len(args) != 2:
                print 'wrong argument {0} input, example: --server=192.168.0.8'.format(arg)
            server_ip = args[1]
    at = Autotest()
    if at.start(server_ip, 34568) == False:
        print 'connect server failed'
        sys.exit(1)
    while at.is_running():
        try:
            time.sleep(1)
        except:
            at.stop()
            break
    sys.exit(0)

