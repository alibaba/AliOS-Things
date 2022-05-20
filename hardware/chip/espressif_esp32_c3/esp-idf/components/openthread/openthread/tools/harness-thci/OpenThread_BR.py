#!/usr/bin/env python
#
# Copyright (c) 2020, The OpenThread Authors.
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
"""
>> Thread Host Controller Interface
>> Device : OpenThread_BR THCI
>> Class : OpenThread_BR
"""
import logging
import re
import sys
import time

import serial
from GRLLibs.UtilityModules.ModuleHelper import ModuleHelper
from IThci import IThci
from THCI.OpenThread import OpenThreadTHCI, watched, API

RPI_FULL_PROMPT = 'pi@raspberrypi:~$ '
RPI_USERNAME_PROMPT = 'raspberrypi login: '
RPI_PASSWORD_PROMPT = 'Password: '
"""regex: used to split lines"""
LINESEPX = re.compile(r'\r\n|\n')

LOGX = re.compile(r'.*Under-voltage detected!')
"""regex: used to filter logging"""

assert LOGX.match('[57522.618196] Under-voltage detected! (0x00050005)')

OTBR_AGENT_SYSLOG_PATTERN = re.compile(r'raspberrypi otbr-agent\[\d+\]: (.*)')
assert OTBR_AGENT_SYSLOG_PATTERN.search(
    'Jun 23 05:21:22 raspberrypi otbr-agent[323]: =========[[THCI] direction=send | type=JOIN_FIN.req | len=039]==========]'
).group(1) == '=========[[THCI] direction=send | type=JOIN_FIN.req | len=039]==========]'


class SSHHandle(object):

    def __init__(self, ip, port, username, password):
        self.ip = ip
        self.port = int(port)
        self.username = username
        self.password = password
        self.__handle = None

        self.__connect()

    def __connect(self):
        import paramiko

        self.close()

        self.__handle = paramiko.SSHClient()
        self.__handle.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        self.__handle.connect(self.ip, port=self.port, username=self.username, password=self.password)

    def close(self):
        if self.__handle is not None:
            self.__handle.close()
            self.__handle = None

    def bash(self, cmd, timeout):
        from paramiko import SSHException
        retry = 3
        for i in range(retry):
            try:
                stdin, stdout, stderr = self.__handle.exec_command(cmd, timeout=timeout)

                sys.stderr.write(stderr.read())
                output = [r.encode('utf8').rstrip('\r\n') for r in stdout.readlines()]
                return output

            except Exception:
                if i < retry - 1:
                    print('SSH connection is lost, try reconnect after 1 second.')
                    time.sleep(1)
                    self.__connect()
                else:
                    raise

    def log(self, fmt, *args):
        try:
            msg = fmt % args
            print('%s - %s - %s' % (self.port, time.strftime('%b %d %H:%M:%S'), msg))
        except Exception:
            pass


class SerialHandle:

    def __init__(self, port, baudrate):
        self.port = port
        self.__handle = serial.Serial(port, baudrate, timeout=0)

        self.__lines = ['']
        assert len(self.__lines) >= 1, self.__lines

        self.log("inputing username ...")
        self.__bashWriteLine('pi')
        deadline = time.time() + 20
        loginOk = False
        while time.time() < deadline:
            time.sleep(1)

            lastLine = None
            while True:
                line = self.__bashReadLine(timeout=1)

                if not line:
                    break

                lastLine = line

            if lastLine == RPI_FULL_PROMPT:
                self.log("prompt found, login success!")
                loginOk = True
                break

            if lastLine == RPI_PASSWORD_PROMPT:
                self.log("inputing password ...")
                self.__bashWriteLine('raspberry')
            elif lastLine == RPI_USERNAME_PROMPT:
                self.log("inputing username ...")
                self.__bashWriteLine('pi')
            elif not lastLine:
                self.log("inputing username ...")
                self.__bashWriteLine('pi')

        if not loginOk:
            raise Exception('login fail')

        self.bash('stty cols 256')

    def close(self):
        self.__handle.close()

    def bash(self, cmd, timeout=10):
        """
        Execute the command in bash.
        """
        self.__bashClearLines()
        self.__bashWriteLine(cmd)
        self.__bashExpect(cmd, timeout=timeout, endswith=True)

        response = []

        deadline = time.time() + timeout
        while time.time() < deadline:
            line = self.__bashReadLine()
            if line is None:
                time.sleep(0.01)
                continue

            if line == RPI_FULL_PROMPT:
                # return response lines without prompt
                return response

            response.append(line)

        self.__bashWrite('\x03')
        raise Exception('%s: failed to find end of response' % self.port)

    def __bashExpect(self, expected, timeout=20, endswith=False):
        print('[%s] Expecting [%r]' % (self.port, expected))

        deadline = time.time() + timeout
        while time.time() < deadline:
            line = self.__bashReadLine()
            if line is None:
                time.sleep(0.01)
                continue

            print('[%s] Got line [%r]' % (self.port, line))

            if endswith:
                matched = line.endswith(expected)
            else:
                matched = line == expected

            if matched:
                print('[%s] Expected [%r]' % (self.port, expected))
                return

        # failed to find the expected string
        # send Ctrl+C to terminal
        self.__bashWrite('\x03')
        raise Exception('failed to find expected string[%s]' % expected)

    def __bashRead(self, timeout=1):
        deadline = time.time() + timeout
        data = ''
        while True:
            piece = self.__handle.read()
            data = data + piece.decode('utf8')
            if piece:
                continue

            if data or time.time() >= deadline:
                break

        if data:
            self.log('>>> %r', data)

        return data

    def __bashReadLine(self, timeout=1):
        line = self.__bashGetNextLine()
        if line is not None:
            return line

        assert len(self.__lines) == 1, self.__lines
        tail = self.__lines.pop()

        try:
            tail += self.__bashRead(timeout=timeout)
            tail = tail.replace(RPI_FULL_PROMPT, RPI_FULL_PROMPT + '\r\n')
            tail = tail.replace(RPI_USERNAME_PROMPT, RPI_USERNAME_PROMPT + '\r\n')
            tail = tail.replace(RPI_PASSWORD_PROMPT, RPI_PASSWORD_PROMPT + '\r\n')
        finally:
            self.__lines += [l.rstrip('\r') for l in LINESEPX.split(tail)]
            assert len(self.__lines) >= 1, self.__lines

        return self.__bashGetNextLine()

    def __bashGetNextLine(self):
        assert len(self.__lines) >= 1, self.__lines
        while len(self.__lines) > 1:
            line = self.__lines.pop(0)
            assert len(self.__lines) >= 1, self.__lines
            if LOGX.match(line):
                logging.info('LOG: %s', line)
                continue
            else:
                return line
        assert len(self.__lines) >= 1, self.__lines
        return None

    def __bashWrite(self, data):
        self.__handle.write(data)
        self.log("<<< %r", data)

    def __bashClearLines(self):
        assert len(self.__lines) >= 1, self.__lines
        while self.__bashReadLine(timeout=0) is not None:
            pass
        assert len(self.__lines) >= 1, self.__lines

    def __bashWriteLine(self, line):
        self.__bashWrite(line + '\n')


class OpenThread_BR(OpenThreadTHCI, IThci):
    DEFAULT_COMMAND_TIMEOUT = 20

    IsBorderRouter = True

    def _connect(self):
        self.log("logining Raspberry Pi ...")
        self.__cli_output_lines = []
        self.__syslog_skip_lines = None
        self.__syslog_last_read_ts = 0

        if self.connectType == 'ip':
            self.__handle = SSHHandle(self.telnetIp, self.telnetPort, self.telnetUsername, self.telnetPassword)
        else:
            self.__handle = SerialHandle(self.port, 115200)

        self.__afterConnect()

    def _disconnect(self):
        if self.__handle:
            self.__handle.close()
            self.__handle = None

    def _deviceBeforeReset(self):
        if self.IsHost:
            self.__stopRadvdService()
            self.bash('sudo ip -6 addr del 910b::1 dev eth0 || true')
            self.bash('sudo ip -6 addr del fd00:7d03:7d03:7d03::1 dev eth0 || true')

    def _deviceAfterReset(self):
        self.__dumpSyslog()
        self.__truncateSyslog()

    @API
    def setupHost(self, setDua=False):
        self.IsHost = True

        if not setDua:
            cmd = 'sudo ip -6 addr add 910b::1 dev eth0'
        else:
            cmd = 'sudo ip -6 addr add fd00:7d03:7d03:7d03::1 dev eth0'
        self.bash(cmd)

        self.__startRadvdService()

    def _deviceEscapeEscapable(self, string):
        """Escape CLI escapable characters in the given string.

        Args:
            string (str): UTF-8 input string.

        Returns:
            [str]: The modified string with escaped characters.
        """
        return '"' + string + '"'

    @watched
    def bash(self, cmd, timeout=DEFAULT_COMMAND_TIMEOUT):
        return self.__handle.bash(cmd, timeout=timeout)

    def bash_unwatched(self, cmd, timeout=DEFAULT_COMMAND_TIMEOUT):
        return self.__handle.bash(cmd, timeout=timeout)

    # Override send_udp
    @API
    def send_udp(self, interface, dst, port, payload):
        if interface == 0:  # Thread Interface
            super(OpenThread_BR, self).send_udp(interface, dst, port, payload)
            return

        if interface == 1:
            ifname = 'eth0'
        else:
            print('invalid interface')
            return

        cmd = 'sudo /home/pi/ot-br-posix/script/reference-device/send_udp.py %s %s %s %s' % (ifname, dst, port,
                                                                                             payload)
        print(cmd)
        self.bash(cmd)

    @API
    def ip_neighbors_flush(self):
        print('%s call clear_cache' % self.port)
        # clear neigh cache on linux
        cmd1 = 'sudo ip -6 neigh flush nud all nud failed nud noarp dev eth0'
        cmd2 = 'sudo ip -6 neigh list nud all dev eth0 ' \
               '| cut -d " " -f1 ' \
               '| sudo xargs -I{} ip -6 neigh delete {} dev eth0'
        cmd = '%s ; %s' % (cmd1, cmd2)
        self.bash(cmd)

    @API
    def ip_neighbors_add(self, addr, lladdr, nud='noarp'):
        print('%s ip_neighbors_add' % self.port)
        cmd1 = 'sudo ip -6 neigh delete %s dev eth0' % addr
        cmd2 = 'sudo ip -6 neigh add %s dev eth0 lladdr %s nud %s' % (addr, lladdr, nud)
        cmd = '%s ; %s' % (cmd1, cmd2)
        self.bash(cmd)

    @API
    def get_eth_ll(self):
        print('%s get_eth_ll' % self.port)
        cmd = "ip -6 addr list dev eth0 | grep 'inet6 fe80' | awk '{print $2}'"
        ret = self.bash(cmd)[0].split('/')[0]
        return ret

    @API
    def ping(self, strDestination, ilength=0, hop_limit=5, timeout=5):
        """ send ICMPv6 echo request with a given length to a unicast destination
            address

        Args:
            strDestination: the unicast destination address of ICMPv6 echo request
            ilength: the size of ICMPv6 echo request payload
            hop_limit: the hop limit
            timeout: time before ping() stops
        """
        if hop_limit is None:
            hop_limit = 5

        if self.IsHost or self.IsBackboneRouter:
            ifName = 'eth0'
        else:
            ifName = 'wpan0'

        cmd = 'ping -6 -I %s %s -c 1 -s %d -W %d -t %d' % (
            ifName,
            strDestination,
            int(ilength),
            int(timeout),
            int(hop_limit),
        )

        self.bash(cmd)
        time.sleep(1)

    def multicast_Ping(self, destination, length=20):
        """send ICMPv6 echo request with a given length to a multicast destination
           address

        Args:
            destination: the multicast destination address of ICMPv6 echo request
            length: the size of ICMPv6 echo request payload
        """
        print('%s call multicast_Ping' % self.port)
        print('destination: %s' % destination)
        hop_limit = 5

        if self.IsHost or self.IsBackboneRouter:
            ifName = 'eth0'
        else:
            ifName = 'wpan0'

        cmd = 'ping -6 -I %s %s -c 1 -s %d -t %d' % (ifName, destination, str(length), hop_limit)

        self.bash(cmd)

    @API
    def getGUA(self, filterByPrefix=None, eth=False):
        """get expected global unicast IPv6 address of Thread device

        note: existing filterByPrefix are string of in lowercase. e.g.
        '2001' or '2001:0db8:0001:0000".

        Args:
            filterByPrefix: a given expected global IPv6 prefix to be matched

        Returns:
            a global IPv6 address
        """
        # get global addrs set if multiple
        if eth:
            return self.__getEthGUA(filterByPrefix=filterByPrefix)
        else:
            return super(OpenThread_BR, self).getGUA(filterByPrefix=filterByPrefix)

    def __getEthGUA(self, filterByPrefix=None):
        globalAddrs = []

        cmd = 'ip -6 addr list dev eth0 | grep inet6'
        output = self.bash(cmd)
        for line in output:
            # example: inet6 2401:fa00:41:23:274a:1329:3ab9:d953/64 scope global dynamic noprefixroute
            line = line.strip().split()

            if len(line) < 4 or line[2] != 'scope':
                continue

            if line[3] != 'global':
                continue

            addr = line[1].split('/')[0]
            addr = ModuleHelper.GetFullIpv6Address(addr).lower()
            globalAddrs.append(addr)

        if not filterByPrefix:
            return globalAddrs[0]
        else:
            for fullIp in globalAddrs:
                if fullIp.startswith(filterByPrefix):
                    return fullIp

    def _cliReadLine(self):
        # read commissioning log if it's commissioning
        if not self.__cli_output_lines:
            self.__readSyslogToCli()

        if self.__cli_output_lines:
            return self.__cli_output_lines.pop(0)

        return None

    @watched
    def _deviceGetEtherMac(self):
        # Harness wants it in string. Because wireshark filter for eth
        # cannot be applies in hex
        return self.bash('ip addr list dev eth0 | grep ether')[0].strip().split()[1]

    @watched
    def _onCommissionStart(self):
        assert self.__syslog_skip_lines is None
        self.__syslog_skip_lines = int(self.bash('wc -l /var/log/syslog')[0].split()[0])
        self.__syslog_last_read_ts = 0

    @watched
    def _onCommissionStop(self):
        assert self.__syslog_skip_lines is not None
        self.__syslog_skip_lines = None

    def _deviceBeforeThreadStart(self):
        self.bash('sudo sysctl net.ipv6.conf.eth0.accept_ra=2')

    @watched
    def __startRadvdService(self):
        assert self.IsHost, "radvd service runs on Host only"

        self.bash("""sudo sh -c "cat >/etc/radvd.conf <<EOF
interface eth0
{
    AdvSendAdvert on;

    MinRtrAdvInterval 3;
    MaxRtrAdvInterval 30;
    AdvDefaultPreference low;

    prefix 910b::/64
    {
        AdvOnLink on;
        AdvAutonomous on;
        AdvRouterAddr on;
    };
    
    prefix fd00:7d03:7d03:7d03::/64
    {
        AdvOnLink on;
        AdvAutonomous off;
        AdvRouterAddr off;
    };
};
EOF"
""")
        self.bash('sudo service radvd restart')
        self.bash('sudo service radvd status')

    @watched
    def __stopRadvdService(self):
        assert self.IsHost, "radvd service runs on Host only"
        self.bash('sudo service radvd stop')

    def __readSyslogToCli(self):
        if self.__syslog_skip_lines is None:
            return 0

        # read syslog once per second
        if time.time() < self.__syslog_last_read_ts + 1:
            return 0

        self.__syslog_last_read_ts = time.time()

        lines = self.bash_unwatched('tail +%d /var/log/syslog' % self.__syslog_skip_lines)
        for line in lines:
            m = OTBR_AGENT_SYSLOG_PATTERN.search(line)
            if not m:
                continue

            self.__cli_output_lines.append(m.group(1))

        self.__syslog_skip_lines += len(lines)
        return len(lines)

    def _cliWriteLine(self, line):
        cmd = 'sudo ot-ctl -- %s' % line
        output = self.bash(cmd)
        # fake the line echo back
        self.__cli_output_lines.append(line)
        for line in output:
            self.__cli_output_lines.append(line)

    def __afterConnect(self):
        self.__truncateSyslog()
        self.__checkServiceStatus()

    def __checkServiceStatus(self):
        self.bash('sudo service radvd stop')
        self.bash('sudo service otbr-agent restart')

    def __truncateSyslog(self):
        self.bash('sudo truncate -s 0 /var/log/syslog')

    def __dumpSyslog(self):
        output = self.bash_unwatched('sudo grep "otbr-agent" /var/log/syslog')
        for line in output:
            self.log('%s', line)
