#!/usr/bin/env python
#
# Copyright (c) 2016, The OpenThread Authors.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the
#    names of its contributors may be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

import logging
import re
import socket
import threading
import time

import serial

from . import settings

__all__ = ['OpenThreadController']
logger = logging.getLogger(__name__)

linesepx = re.compile(r'\r\n|\n')


class OpenThreadController(threading.Thread):
    """This is an simple wrapper to communicate with openthread"""

    _lock = threading.Lock()
    viewing = False

    def __init__(self, port, log=False):
        """Initialize the controller

        Args:
            port (str): serial port's path or name(windows)
        """
        super(OpenThreadController, self).__init__()
        self.port = port
        self.handle = None
        self.lines = []
        self._log = log
        self._is_net = False
        self._init()

    def _init(self):
        self._connect()
        if not self._log:
            return

        self.start()

    def __del__(self):
        self.close()

    def close(self):
        if self.is_alive():
            self.viewing = False
            self.join()

        self._close()

    def __enter__(self):
        return self

    def __exit__(self, type, value, traceback):
        self.close()

    def _close(self):
        if self.handle:
            self.handle.close()
            self.handle = None

    def _connect(self):
        logger.debug('My port is %s', self.port)
        if self.port.startswith('NET'):
            portnum = settings.SER2NET_PORTBASE + int(self.port.split('NET')[1])
            logger.debug('My port num is %d', portnum)
            address = (settings.SER2NET_HOSTNAME, portnum)
            self.handle = socket.create_connection(address)
            self.handle.setblocking(0)
            self._is_net = True
        elif ':' in self.port:
            host, port = self.port.split(':')
            self.handle = socket.create_connection((host, port))
            self.handle.setblocking(0)
            self._is_net = True
        else:
            self.handle = serial.Serial(self.port, 115200, timeout=0, xonxoff=True)
            self._is_net = False

    def _read(self, size=512):
        if self._is_net:
            return self.handle.recv(size)
        else:
            return self.handle.read(size)

    def _write(self, data):
        if self._is_net:
            self.handle.sendall(data)
        else:
            self.handle.write(data)

    def _expect(self, expected, times=50):
        """Find the `expected` line within `times` trials.

        Args:
            expected    str: the expected string
            times       int: number of trials
        """
        logger.debug('[%s] Expecting [%s]', self.port, expected)
        retry_times = 10
        while times:
            if not retry_times:
                break

            line = self._readline()

            if line == expected:
                return

            if not line:
                retry_times -= 1
                time.sleep(0.1)

            times -= 1

        raise Exception('failed to find expected string[%s]' % expected)

    def _readline(self):
        """Read exactly one line from the device, nonblocking.

        Returns:
            None on no data
        """
        if len(self.lines) > 1:
            return self.lines.pop(0)

        tail = ''
        if len(self.lines):
            tail = self.lines.pop()

        try:
            tail += self._read()
        except socket.error:
            logging.exception('No new data')
            time.sleep(0.1)

        self.lines += linesepx.split(tail)
        if len(self.lines) > 1:
            return self.lines.pop(0)

    def _sendline(self, line):
        """Send exactly one line to the device

        Args:
            line str: data send to device
        """
        self.lines = []
        try:
            self._read()
        except socket.error:
            logging.debug('Nothing cleared')

        logger.debug('sending [%s]', line)
        self._write(line + '\r\n')

        # wait for write to complete
        time.sleep(0.5)

    def _req(self, req):
        """Send command and wait for response.

        The command will be repeated 3 times at most in case data loss of serial port.

        Args:
            req (str): Command to send, please do not include new line in the end.

        Returns:
            [str]: The output lines
        """
        logger.debug('DUT> %s', req)
        self._log and self.pause()
        times = 3
        res = None

        while times:
            times = times - 1
            try:
                self._sendline(req)
                self._expect(req)

                line = None
                res = []

                while True:
                    line = self._readline()
                    logger.debug('Got line %s', line)

                    if line == 'Done':
                        break

                    if line:
                        res.append(line)
                break

            except BaseException:
                logger.exception('Failed to send command')
                self.close()
                self._init()

        self._log and self.resume()
        return res

    def run(self):
        """Threading callback"""

        self.viewing = True
        while self.viewing and self._lock.acquire():
            try:
                line = self._readline()
            except BaseException:
                pass
            else:
                logger.info(line)
            self._lock.release()
            time.sleep(0)

    def is_started(self):
        """check if openthread is started

        Returns:
            bool: started or not
        """
        state = self._req('state')[0]
        return state != 'disabled'

    def start(self):
        """Start openthread
        """
        self._req('ifconfig up')
        self._req('thread start')

    def stop(self):
        """Stop openthread
        """
        self._req('thread stop')
        self._req('ifconfig down')

    def reset(self):
        """Reset openthread device, not equivalent to stop and start
        """
        logger.debug('DUT> reset')
        self._log and self.pause()
        self._sendline('reset')
        self._read()
        self._log and self.resume()

    def resume(self):
        """Start dumping logs"""
        self._lock.release()

    def pause(self):
        """Start dumping logs"""
        self._lock.acquire()

    @property
    def networkname(self):
        """str: Thread network name."""
        return self._req('networkname')[0]

    @networkname.setter
    def networkname(self, value):
        self._req('networkname %s' % value)

    @property
    def mode(self):
        """str: Thread mode."""
        return self._req('mode')[0]

    @mode.setter
    def mode(self, value):
        self._req('mode %s' % value)

    @property
    def mac(self):
        """str: MAC address of the device"""
        return self._req('extaddr')[0]

    @property
    def addrs(self):
        """[str]: IP addresses of the devices"""
        return self._req('ipaddr')

    @property
    def short_addr(self):
        """str: Short address"""
        return self._req('rloc16')[0]

    @property
    def channel(self):
        """int: Channel number of openthread"""
        return int(self._req('channel')[0])

    @channel.setter
    def channel(self, value):
        self._req('channel %d' % value)

    @property
    def panid(self):
        """str: Thread panid"""
        return self._req('panid')[0]

    @panid.setter
    def panid(self, value):
        self._req('panid %s' % value)

    @property
    def extpanid(self):
        """str: Thread extpanid"""
        return self._req('extpanid')[0]

    @extpanid.setter
    def extpanid(self, value):
        self._req('extpanid %s' % value)

    @property
    def child_timeout(self):
        """str: Thread child timeout in seconds"""
        return self._req('childtimeout')[0]

    @child_timeout.setter
    def child_timeout(self, value):
        self._req('childtimeout %d' % value)

    @property
    def version(self):
        """str: Open thread version"""
        return self._req('version')[0]

    def add_prefix(self, prefix, flags, prf):
        """Add network prefix.

        Args:
            prefix (str): network prefix.
            flags (str): network prefix flags, please refer thread documentation for details
            prf (str): network prf, please refer thread documentation for details
        """
        self._req('prefix add %s %s %s' % (prefix, flags, prf))
        time.sleep(1)
        self._req('netdata register')

    def remove_prefix(self, prefix):
        """Remove network prefix.
        """
        self._req('prefix remove %s' % prefix)
        time.sleep(1)
        self._req('netdata register')

    def enable_denylist(self):
        """Enable denylist feature"""
        self._req('denylist enable')

    def add_denylist(self, mac):
        """Add a mac address to denylist"""
        self._req('denylist add %s' % mac)
