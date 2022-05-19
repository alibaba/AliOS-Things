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
"""
>> Thread Host Controller Interface
>> Device : OpenThread THCI
>> Class : OpenThread
"""

import functools
import ipaddress
import logging
import random
import re
import socket
import time
from abc import abstractmethod

import serial
from Queue import Queue

TESTHARNESS_1_1 = '1.1'
TESTHARNESS_1_2 = '1.2'

if 'Thread1.2' in __file__:
    TESTHARNESS_VERSION = TESTHARNESS_1_2
else:
    TESTHARNESS_VERSION = TESTHARNESS_1_1

from GRLLibs.ThreadPacket.PlatformPackets import (
    PlatformDiagnosticPacket,
    PlatformPackets,
)
from GRLLibs.UtilityModules.ModuleHelper import ModuleHelper, ThreadRunner
from GRLLibs.UtilityModules.Plugins.AES_CMAC import Thread_PBKDF2
from GRLLibs.UtilityModules.Test import (
    Thread_Device_Role,
    Device_Data_Requirement,
    MacType,
)
from GRLLibs.UtilityModules.enums import (
    PlatformDiagnosticPacket_Direction,
    PlatformDiagnosticPacket_Type,
)

if TESTHARNESS_VERSION == TESTHARNESS_1_2:
    from GRLLibs.UtilityModules.enums import (
        DevCapb,)

    import commissioner
    from commissioner_impl import OTCommissioner

from IThci import IThci

LINESEPX = re.compile(r'\r\n|\n')
"""regex: used to split lines"""

LOGX = re.compile(r'((\[(NONE|CRIT|WARN|NOTE|INFO|DEBG)\])'
                  r'|(-(CLI|MLR|API|MLE|BBR|DUA|ARP|N-DATA|ICMP|IP6|MAC|MEM|NCP|MESH-CP|DIAG|PLAT|COAP|CORE|UTIL)-+: )'
                  r'|(-+$)'  # e.x. ------------------------------------------------------------------------
                  r'|(=+\[.*\]=+$)'  # e.x. ==============================[TX len=108]===============================
                  r'|(\|.+\|.+\|.+)'  # e.x. | 61 DC D2 CE FA 04 00 00 | 00 00 0A 6E 16 01 00 00 | aRNz......n....
                  r')')
"""regex used to filter logs"""

assert LOGX.match('[NONE]')
assert LOGX.match('[CRIT]')
assert LOGX.match('[WARN]')
assert LOGX.match('[NOTE]')
assert LOGX.match('[INFO]')
assert LOGX.match('[DEBG]')
assert LOGX.match('-CLI-----: ')
assert LOGX.match('-N-DATA--: ')
assert LOGX.match('-MESH-CP-: ')
assert LOGX.match('------------------------------------------------------------------------')
assert LOGX.match('==============================[TX len=108]===============================')
assert LOGX.match('| 61 DC D2 CE FA 04 00 00 | 00 00 0A 6E 16 01 00 00 | aRNz......n....')

# OT Errors
OT_ERROR_ALREADY = 24

logging.basicConfig(level=logging.INFO, format="%(asctime)s - %(levelname)s - %(message)s")

_callStackDepth = 0


def watched(func):
    func_name = func.func_name

    @functools.wraps(func)
    def wrapped_api_func(self, *args, **kwargs):
        global _callStackDepth
        callstr = '====' * _callStackDepth + "> %s%s%s" % (func_name, str(args) if args else "",
                                                           str(kwargs) if kwargs else "")

        _callStackDepth += 1
        try:
            ret = func(self, *args, **kwargs)
            self.log("%s returns %r", callstr, ret)
            return ret
        except Exception as ex:
            self.log("FUNC %s failed: %s", func_name, str(ex))
            raise
        finally:
            _callStackDepth -= 1

    return wrapped_api_func


def retry(n, interval=0):
    assert n >= 0, n

    def deco(func):

        @functools.wraps(func)
        def retried_func(*args, **kwargs):
            for i in range(n + 1):
                try:
                    return func(*args, **kwargs)
                except Exception:
                    if i == n:
                        raise

                    time.sleep(interval)

        return retried_func

    return deco


def API(api_func):
    return watched(api_func)


def commissioning(func):

    @functools.wraps(func)
    def comm_func(self, *args, **kwargs):
        self._onCommissionStart()
        try:
            return func(self, *args, **kwargs)
        finally:
            self._onCommissionStop()

    return comm_func


class CommandError(Exception):

    def __init__(self, code, msg):
        assert isinstance(code, int), code
        self.code = code
        self.msg = msg

        super(CommandError, self).__init__("Error %d: %s" % (code, msg))


class OpenThreadTHCI(object):
    LOWEST_POSSIBLE_PARTATION_ID = 0x1
    LINK_QUALITY_CHANGE_TIME = 100
    DEFAULT_COMMAND_TIMEOUT = 10
    firmwarePrefix = 'OPENTHREAD/'
    DOMAIN_NAME = 'Thread'
    MLR_TIMEOUT_MIN = 300

    IsBorderRouter = False
    IsBackboneRouter = False
    IsHost = False

    externalCommissioner = None
    _update_router_status = False

    if TESTHARNESS_VERSION == TESTHARNESS_1_2:
        _ROLE_MODE_DICT = {
            Thread_Device_Role.Leader: 'rdn',
            Thread_Device_Role.Router: 'rdn',
            Thread_Device_Role.SED: '-',
            Thread_Device_Role.EndDevice: 'rn',
            Thread_Device_Role.REED: 'rdn',
            Thread_Device_Role.EndDevice_FED: 'rdn',
            Thread_Device_Role.EndDevice_MED: 'rn',
            Thread_Device_Role.SSED: '-',
        }

    def __init__(self, **kwargs):
        """initialize the serial port and default network parameters
        Args:
            **kwargs: Arbitrary keyword arguments
                      Includes 'EUI' and 'SerialPort'
        """
        self.intialize(kwargs)

    @abstractmethod
    def _connect(self):
        """
        Connect to the device.
        """

    @abstractmethod
    def _disconnect(self):
        """
        Disconnect from the device
        """

    @abstractmethod
    def _cliReadLine(self):
        """Read exactly one line from the device

        Returns:
            None if no data
        """

    @abstractmethod
    def _cliWriteLine(self, line):
        """Send exactly one line to the device

        Args:
            line str: data send to device
        """

    @abstractmethod
    def _onCommissionStart(self):
        """Called when commissioning starts."""

    @abstractmethod
    def _onCommissionStop(self):
        """Called when commissioning stops."""

    def __sendCommand(self, cmd, expectEcho=True):
        self.log("command: %s", cmd)
        self._cliWriteLine(cmd)
        if expectEcho:
            self.__expect(cmd, endswith=True)

    _COMMAND_OUTPUT_ERROR_PATTERN = re.compile(r'Error (\d+): (.*)')

    @retry(3)
    @watched
    def __executeCommand(self, cmd, timeout=DEFAULT_COMMAND_TIMEOUT):
        """send specific command to reference unit over serial port

        Args:
            cmd: OpenThread CLI string

        Returns:
            Done: successfully send the command to reference unit and parse it
            Value: successfully retrieve the desired value from reference unit
            Error: some errors occur, indicates by the followed specific error number
        """
        if self.logThreadStatus == self.logStatus['running']:
            self.logThreadStatus = self.logStatus['pauseReq']
            while (self.logThreadStatus != self.logStatus['paused'] and
                   self.logThreadStatus != self.logStatus['stop']):
                pass

        self.__sendCommand(cmd)
        line = None
        response = []

        t_end = time.time() + timeout
        while time.time() < t_end:
            line = self.__readCliLine()
            if line is None:
                time.sleep(0.01)
                continue

            self.log("readline: %s", line)
            response.append(line)

            if line == 'Done':
                break
            else:
                m = OpenThreadTHCI._COMMAND_OUTPUT_ERROR_PATTERN.match(line)
                if m is not None:
                    code, msg = m.groups()
                    raise CommandError(int(code), msg)
        else:
            raise Exception('%s: failed to find end of response: %s' % (self, response))

        return response

    def __expect(self, expected, timeout=5, endswith=False):
        """Find the `expected` line within `times` tries.

        Args:
            expected    str: the expected string
            times       int: number of tries
        """
        print('[%s] Expecting [%s]' % (self, expected))

        deadline = time.time() + timeout
        while True:
            line = self.__readCliLine()
            if line is not None:
                self.log("readline: %s", line)

            if line is None:
                if time.time() >= deadline:
                    break

                self.sleep(0.01)
                continue

            matched = line.endswith(expected) if endswith else line == expected
            if matched:
                print('[%s] Expected [%s]' % (self, expected))
                return

        raise Exception('failed to find expected string[%s]' % expected)

    def __readCliLine(self, ignoreLogs=True):
        """Read the next line from OT CLI.d"""
        line = self._cliReadLine()
        if ignoreLogs:
            while line is not None and LOGX.match(line):
                line = self._cliReadLine()

        return line

    @API
    def getVersionNumber(self):
        """get OpenThread stack firmware version number"""
        return self.__executeCommand('version')[0]

    def log(self, fmt, *args):
        try:
            msg = fmt % args
            print('%s - %s - %s' % (self.port, time.strftime('%b %d %H:%M:%S'), msg))
        except Exception:
            pass

    def sleep(self, duration):
        if duration >= 1:
            self.log("sleeping for %ss ...", duration)
        time.sleep(duration)

    @API
    def intialize(self, params):
        """initialize the serial port with baudrate, timeout parameters"""
        self.port = params.get('SerialPort', '')
        self.log('%s intialize: %r', self.__class__.__name__, params)
        # params example: {'EUI': 1616240311388864514L, 'SerialBaudRate': None, 'TelnetIP': '192.168.8.181', 'SerialPort': None, 'Param7': None, 'Param6': None, 'Param5': 'ip', 'TelnetPort': '22', 'Param9': None, 'Param8': None}

        try:

            ipaddress.ip_address(self.port)
            # handle TestHarness Discovery Protocol
            self.connectType = 'ip'
            self.telnetIp = self.port
            self.telnetPort = 22
            self.telnetUsername = 'pi'
            self.telnetPassword = 'raspberry'
        except ValueError:
            self.connectType = (params.get('Param5') or 'usb').lower()
            self.telnetIp = params.get('TelnetIP')
            self.telnetPort = int(params.get('TelnetPort')) if params.get('TelnetPort') else 22
            # username for SSH
            self.telnetUsername = 'pi' if params.get('Param6') is None else params.get('Param6')
            # password for SSH
            self.telnetPassword = 'raspberry' if params.get('Param7') is None else params.get('Param7')

        self.mac = params.get('EUI')

        self.UIStatusMsg = ''
        self.AutoDUTEnable = False
        self._is_net = False  # whether device is through ser2net
        self.logStatus = {
            'stop': 'stop',
            'running': 'running',
            'pauseReq': 'pauseReq',
            'paused': 'paused',
        }
        self.joinStatus = {
            'notstart': 'notstart',
            'ongoing': 'ongoing',
            'succeed': 'succeed',
            'failed': 'failed',
        }
        self.logThreadStatus = self.logStatus['stop']

        self.deviceConnected = False

        # init serial port
        self._connect()
        if TESTHARNESS_VERSION == TESTHARNESS_1_2:
            self.__discoverDeviceCapability()
        self.UIStatusMsg = self.getVersionNumber()

        if self.firmwarePrefix in self.UIStatusMsg:
            self.deviceConnected = True
        else:
            self.UIStatusMsg = ('Firmware Not Matching Expecting ' + self.firmwarePrefix + ', Now is ' +
                                self.UIStatusMsg)
            ModuleHelper.WriteIntoDebugLogger('Err: OpenThread device Firmware not matching..')

    def __repr__(self):
        if self.connectType == 'ip':
            return '[%s:%d]' % (self.telnetIp, self.telnetPort)
        else:
            return '[%s]' % self.port

    @API
    def closeConnection(self):
        """close current serial port connection"""
        self._disconnect()

    def __disableRouterEligible(self):
        """disable router role
        """
        print('call __disableRouterEligible')
        try:
            cmd = 'routereligible disable'
            self.__executeCommand(cmd)
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('__disableRouterEligible() Error: ' + str(e))

    def __setDeviceMode(self, mode):
        """set thread device mode:

        Args:
           mode: thread device mode
           r: rx-on-when-idle
           s: secure IEEE 802.15.4 data request
           d: full thread device
           n: full network data

        Returns:
           True: successful to set the device mode
           False: fail to set the device mode
        """
        print('call __setDeviceMode')
        print(mode)
        try:
            cmd = 'mode %s' % mode
            return self.__executeCommand(cmd)[-1] == 'Done'
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('setDeviceMode() Error: ' + str(e))

    def __setRouterUpgradeThreshold(self, iThreshold):
        """set router upgrade threshold

        Args:
            iThreshold: the number of active routers on the Thread network
                        partition below which a REED may decide to become a Router.

        Returns:
            True: successful to set the ROUTER_UPGRADE_THRESHOLD
            False: fail to set ROUTER_UPGRADE_THRESHOLD
        """
        print('call __setRouterUpgradeThreshold')
        try:
            cmd = 'routerupgradethreshold %s' % str(iThreshold)
            print(cmd)
            return self.__executeCommand(cmd)[-1] == 'Done'
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('setRouterUpgradeThreshold() Error: ' + str(e))

    def __setRouterDowngradeThreshold(self, iThreshold):
        """set router downgrade threshold

        Args:
            iThreshold: the number of active routers on the Thread network
                        partition above which an active router may decide to
                        become a child.

        Returns:
            True: successful to set the ROUTER_DOWNGRADE_THRESHOLD
            False: fail to set ROUTER_DOWNGRADE_THRESHOLD
        """
        print('call __setRouterDowngradeThreshold')
        try:
            cmd = 'routerdowngradethreshold %s' % str(iThreshold)
            print(cmd)
            return self.__executeCommand(cmd)[-1] == 'Done'
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('setRouterDowngradeThreshold() Error: ' + str(e))

    def __setRouterSelectionJitter(self, iRouterJitter):
        """set ROUTER_SELECTION_JITTER parameter for REED to upgrade to Router

        Args:
            iRouterJitter: a random period prior to request Router ID for REED

        Returns:
            True: successful to set the ROUTER_SELECTION_JITTER
            False: fail to set ROUTER_SELECTION_JITTER
        """
        print('call _setRouterSelectionJitter')
        try:
            cmd = 'routerselectionjitter %s' % str(iRouterJitter)
            print(cmd)
            return self.__executeCommand(cmd)[-1] == 'Done'
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('setRouterSelectionJitter() Error: ' + str(e))

    def __setAddressfilterMode(self, mode):
        """set address filter mode

        Returns:
            True: successful to set address filter mode.
            False: fail to set address filter mode.
        """
        print('call setAddressFilterMode() ' + mode)
        try:
            cmd = 'macfilter addr ' + mode
            return self.__executeCommand(cmd)[-1] == 'Done'
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('__setAddressFilterMode() Error: ' + str(e))

    def __startOpenThread(self):
        """start OpenThread stack

        Returns:
            True: successful to start OpenThread stack and thread interface up
            False: fail to start OpenThread stack
        """
        print('call startOpenThread')
        if self.hasActiveDatasetToCommit:
            if self.__executeCommand('dataset commit active')[0] != 'Done':
                raise Exception('failed to commit active dataset')
            else:
                self.hasActiveDatasetToCommit = False

        # restore allowlist/denylist address filter mode if rejoin after
        # reset
        if self.isPowerDown:
            if self._addressfilterMode == 'allowlist':
                if self.__setAddressfilterMode('allowlist'):
                    for addr in self._addressfilterSet:
                        self.addAllowMAC(addr)
            elif self._addressfilterMode == 'denylist':
                if self.__setAddressfilterMode('denylist'):
                    for addr in self._addressfilterSet:
                        self.addBlockedMAC(addr)

        # Set routerselectionjitter to 1 for certain device roles
        if self.deviceRole in [
                Thread_Device_Role.Leader,
                Thread_Device_Role.Router,
                Thread_Device_Role.REED,
        ]:
            self.__setRouterSelectionJitter(1)
        elif TESTHARNESS_VERSION == TESTHARNESS_1_2 and self.deviceRole in [
                Thread_Device_Role.BR_1, Thread_Device_Role.BR_2
        ]:
            self.IsBackboneRouter = True
            self.__setRouterSelectionJitter(1)

        if self.IsBackboneRouter:
            # Configure default BBR dataset
            self.__configBbrDataset(SeqNum=self.bbrSeqNum,
                                    MlrTimeout=self.bbrMlrTimeout,
                                    ReRegDelay=self.bbrReRegDelay)
            # Add default domain prefix is not configured otherwise
            if self.__useDefaultDomainPrefix:
                self.__addDefaultDomainPrefix()

            self._deviceBeforeThreadStart()

        self.__executeCommand('ifconfig up')
        self.__executeCommand('thread start')
        self.isPowerDown = False
        return True

    def _deviceBeforeThreadStart(self):
        pass

    def __stopOpenThread(self):
        """stop OpenThread stack

        Returns:
            True: successful to stop OpenThread stack and thread interface down
            False: fail to stop OpenThread stack
        """
        self.log('call stopOpenThread')
        try:
            if self.__executeCommand('thread stop')[-1] == 'Done':
                return self.__executeCommand('ifconfig down')[-1] == 'Done'
            else:
                return False
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('stopOpenThread() Error: ' + str(e))

    @watched
    def __isOpenThreadRunning(self):
        """check whether or not OpenThread is running

        Returns:
            True: OpenThread is running
            False: OpenThread is not running
        """
        self.log('call isOpenThreadRunning')
        return self.__executeCommand('state')[0] != 'disabled'

    # rloc16 might be hex string or integer, need to return actual allocated
    # router id
    def __convertRlocToRouterId(self, xRloc16):
        """mapping Rloc16 to router id

        Args:
            xRloc16: hex rloc16 short address

        Returns:
            actual router id allocated by leader
        """
        routerList = []
        routerList = self.__executeCommand('router list')[0].split()
        print(routerList)
        print(xRloc16)

        for index in routerList:
            router = []
            cmd = 'router %s' % index
            router = self.__executeCommand(cmd)

            for line in router:
                if 'Done' in line:
                    break
                elif 'Router ID' in line:
                    routerid = line.split()[2]
                elif 'Rloc' in line:
                    rloc16 = line.split()[1]
                else:
                    pass

            # process input rloc16
            if isinstance(xRloc16, str):
                rloc16 = '0x' + rloc16
                if rloc16 == xRloc16:
                    return routerid
            elif isinstance(xRloc16, int):
                if int(rloc16, 16) == xRloc16:
                    return routerid
            else:
                pass

        return None

    def __convertIp6PrefixStringToIp6Address(self, strIp6Prefix):
        """convert IPv6 prefix string to IPv6 dotted-quad format
           for example:
           2001000000000000 -> 2001:0000:0000:0000::

        Args:
            strIp6Prefix: IPv6 address string

        Returns:
            IPv6 address dotted-quad format
        """
        prefix1 = strIp6Prefix.rstrip('L')
        prefix2 = self.__lstrip0x(prefix1)
        hexPrefix = str(prefix2).ljust(16, '0')
        hexIter = iter(hexPrefix)
        finalMac = ':'.join(a + b + c + d for a, b, c, d in zip(hexIter, hexIter, hexIter, hexIter))
        prefix = str(finalMac)
        strIp6Prefix = prefix[:19]
        return strIp6Prefix + '::'

    # pylint: disable=no-self-use
    def __convertLongToHex(self, iValue, fillZeros=None):
        """convert a long hex integer to string
           remove '0x' and 'L' return string

        Args:
            iValue: long integer in hex format
            fillZeros: pad string with zeros on the left to specified width

        Returns:
            string of this long integer without '0x' and 'L'
        """
        fmt = '%x'
        if fillZeros is not None:
            fmt = '%%0%dx' % fillZeros

        return fmt % iValue

    @commissioning
    def __readCommissioningLogs(self, durationInSeconds):
        """read logs during the commissioning process

        Args:
            durationInSeconds: time duration for reading commissioning logs

        Returns:
            Commissioning logs
        """
        self.logThreadStatus = self.logStatus['running']
        logs = Queue()
        t_end = time.time() + durationInSeconds
        joinSucceed = False

        while time.time() < t_end:

            if self.logThreadStatus == self.logStatus['pauseReq']:
                self.logThreadStatus = self.logStatus['paused']

            if self.logThreadStatus != self.logStatus['running']:
                self.sleep(0.01)
                continue

            try:
                line = self.__readCliLine(ignoreLogs=False)

                if line:
                    self.log("commissioning log: %s", line)
                    logs.put(line)

                    if 'Join success' in line:
                        joinSucceed = True
                        # read commissioning logs for 3 more seconds
                        t_end = time.time() + 3
                    elif 'Join failed' in line:
                        # read commissioning logs for 3 more seconds
                        t_end = time.time() + 3
                elif line is None:
                    self.sleep(0.01)

            except Exception:
                pass

        self.joinCommissionedStatus = self.joinStatus['succeed'] if joinSucceed else self.joinStatus['failed']
        self.logThreadStatus = self.logStatus['stop']
        return logs

    # pylint: disable=no-self-use
    def __convertChannelMask(self, channelsArray):
        """convert channelsArray to bitmask format

        Args:
            channelsArray: channel array (i.e. [21, 22])

        Returns:
            bitmask format corresponding to a given channel array
        """
        maskSet = 0

        for eachChannel in channelsArray:
            mask = 1 << eachChannel
            maskSet = maskSet | mask

        return maskSet

    def __setChannelMask(self, channelMask):
        self.log('call _setChannelMask')
        try:
            cmd = 'dataset channelmask %s' % channelMask
            self.hasActiveDatasetToCommit = True
            return self.__executeCommand(cmd)[-1] == 'Done'
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('setChannelMask() Error: ' + str(e))

    def __setSecurityPolicy(self, securityPolicySecs, securityPolicyFlags):
        print('call _setSecurityPolicy')
        try:
            cmd = 'dataset securitypolicy %s %s' % (
                str(securityPolicySecs),
                securityPolicyFlags,
            )
            self.hasActiveDatasetToCommit = True
            return self.__executeCommand(cmd)[-1] == 'Done'
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('setSecurityPolicy() Error: ' + str(e))

    def __setKeySwitchGuardTime(self, iKeySwitchGuardTime):
        """ set the Key switch guard time

        Args:
            iKeySwitchGuardTime: key switch guard time

        Returns:
            True: successful to set key switch guard time
            False: fail to set key switch guard time
        """
        print('%s call setKeySwitchGuardTime' % self)
        print(iKeySwitchGuardTime)
        try:
            cmd = 'keysequence guardtime %s' % str(iKeySwitchGuardTime)
            if self.__executeCommand(cmd)[-1] == 'Done':
                self.sleep(1)
                return True
            else:
                return False
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('setKeySwitchGuardTime() Error; ' + str(e))

    def __getCommissionerSessionId(self):
        """ get the commissioner session id allocated from Leader """
        print('%s call getCommissionerSessionId' % self)
        return self.__executeCommand('commissioner sessionid')[0]

    # pylint: disable=no-self-use
    def _deviceEscapeEscapable(self, string):
        """Escape CLI escapable characters in the given string.

        Args:
            string (str): UTF-8 input string.

        Returns:
            [str]: The modified string with escaped characters.
        """
        escapable_chars = '\\ \t\r\n'
        for char in escapable_chars:
            string = string.replace(char, '\\%s' % char)
        return string

    @API
    def setNetworkName(self, networkName='GRL'):
        """set Thread Network name

        Args:
            networkName: the networkname string to be set

        Returns:
            True: successful to set the Thread Networkname
            False: fail to set the Thread Networkname
        """
        print('%s call setNetworkName' % self)
        print(networkName)
        networkName = self._deviceEscapeEscapable(networkName)
        try:
            cmd = 'networkname %s' % networkName
            datasetCmd = 'dataset networkname %s' % networkName
            self.hasActiveDatasetToCommit = True
            return self.__executeCommand(cmd)[-1] == 'Done' and self.__executeCommand(datasetCmd)[-1] == 'Done'
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('setNetworkName() Error: ' + str(e))

    @API
    def setChannel(self, channel=11):
        """set channel of Thread device operates on.

        Args:
            channel:
                    (0  - 10: Reserved)
                    (11 - 26: 2.4GHz channels)
                    (27 - 65535: Reserved)

        Returns:
            True: successful to set the channel
            False: fail to set the channel
        """
        print('%s call setChannel' % self)
        print(channel)
        try:
            cmd = 'channel %s' % channel
            datasetCmd = 'dataset channel %s' % channel
            self.hasSetChannel = True
            self.hasActiveDatasetToCommit = True
            return self.__executeCommand(cmd)[-1] == 'Done' and self.__executeCommand(datasetCmd)[-1] == 'Done'
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('setChannel() Error: ' + str(e))

    @API
    def getChannel(self):
        """get current channel"""
        print('%s call getChannel' % self)
        return self.__executeCommand('channel')[0]

    @API
    def setMAC(self, xEUI):
        """set the extended addresss of Thread device

        Args:
            xEUI: extended address in hex format

        Returns:
            True: successful to set the extended address
            False: fail to set the extended address
        """
        print('%s call setMAC' % self)
        print(xEUI)
        address64 = ''
        try:
            if not isinstance(xEUI, str):
                address64 = self.__convertLongToHex(xEUI, 16)
            else:
                address64 = xEUI

            cmd = 'extaddr %s' % address64
            print(cmd)
            if self.__executeCommand(cmd)[-1] == 'Done':
                self.mac = address64
                return True
            else:
                return False
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('setMAC() Error: ' + str(e))

    @API
    def getMAC(self, bType=MacType.RandomMac):
        """get one specific type of MAC address
           currently OpenThread only supports Random MAC address

        Args:
            bType: indicate which kind of MAC address is required

        Returns:
            specific type of MAC address
        """
        print('%s call getMAC' % self)
        print(bType)
        # if power down happens, return extended address assigned previously
        if self.isPowerDown:
            macAddr64 = self.mac
        else:
            if bType == MacType.FactoryMac:
                macAddr64 = self.__executeCommand('eui64')[0]
            elif bType == MacType.HashMac:
                macAddr64 = self.__executeCommand('joiner id')[0]
            elif TESTHARNESS_VERSION == TESTHARNESS_1_2 and bType == MacType.EthMac:
                return self._deviceGetEtherMac()
            else:
                macAddr64 = self.__executeCommand('extaddr')[0]
        print(macAddr64)

        return int(macAddr64, 16)

    @API
    def getLL64(self):
        """get link local unicast IPv6 address"""
        print('%s call getLL64' % self)
        return self.__executeCommand('ipaddr linklocal')[0]

    @API
    def getRloc16(self):
        """get rloc16 short address"""
        print('%s call getRloc16' % self)
        rloc16 = self.__executeCommand('rloc16')[0]
        return int(rloc16, 16)

    @API
    def getRloc(self):
        """get router locator unicast Ipv6 address"""
        print('%s call getRloc' % self)
        return self.__executeCommand('ipaddr rloc')[0]

    def __getGlobal(self):
        """get global unicast IPv6 address set
           if configuring multiple entries
        """
        print('%s call getGlobal' % self)
        globalAddrs = []
        rlocAddr = self.getRloc()

        addrs = self.__executeCommand('ipaddr')

        # take rloc address as a reference for current mesh local prefix,
        # because for some TCs, mesh local prefix may be updated through
        # pending dataset management.
        for ip6Addr in addrs:
            if ip6Addr == 'Done':
                break

            fullIp = ModuleHelper.GetFullIpv6Address(ip6Addr).lower()

            print('address %s' % fullIp)

            if fullIp.startswith('fe80'):
                print('link local')
                continue

            if fullIp.startswith(rlocAddr[0:19]):
                print('mesh local')
                continue

            globalAddrs.append(fullIp)
            print('global')

        return globalAddrs

    @API
    def setNetworkKey(self, key):
        """set Thread network key

        Args:
            key: Thread network key used in secure the MLE/802.15.4 packet

        Returns:
            True: successful to set the Thread network key
            False: fail to set the Thread network key
        """
        networkKey = ''
        print('%s call setNetworkKey' % self)
        print(key)
        try:
            if not isinstance(key, str):
                networkKey = self.__convertLongToHex(key, 32)
                cmd = 'networkkey %s' % networkKey
                datasetCmd = 'dataset networkkey %s' % networkKey
            else:
                networkKey = key
                cmd = 'networkkey %s' % networkKey
                datasetCmd = 'dataset networkkey %s' % networkKey

            self.networkKey = networkKey
            self.hasActiveDatasetToCommit = True
            return self.__executeCommand(cmd)[-1] == 'Done' and self.__executeCommand(datasetCmd)[-1] == 'Done'
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('setNetworkkey() Error: ' + str(e))

    @API
    def addBlockedMAC(self, xEUI):
        """add a given extended address to the denylist entry

        Args:
            xEUI: extended address in hex format

        Returns:
            True: successful to add a given extended address to the denylist entry
            False: fail to add a given extended address to the denylist entry
        """
        print('%s call addBlockedMAC' % self)
        print(xEUI)
        if isinstance(xEUI, str):
            macAddr = xEUI
        else:
            macAddr = self.__convertLongToHex(xEUI)

        try:
            # if blocked device is itself
            if macAddr == self.mac:
                print('block device itself')
                return True

            if self._addressfilterMode != 'denylist':
                if self.__setAddressfilterMode('denylist'):
                    self._addressfilterMode = 'denylist'

            cmd = 'macfilter addr add %s' % macAddr
            print(cmd)
            ret = self.__executeCommand(cmd)[-1] == 'Done'

            self._addressfilterSet.add(macAddr)
            print('current denylist entries:')
            for addr in self._addressfilterSet:
                print(addr)

            return ret
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('addBlockedMAC() Error: ' + str(e))

    @API
    def addAllowMAC(self, xEUI):
        """add a given extended address to the allowlist addressfilter

        Args:
            xEUI: a given extended address in hex format

        Returns:
            True: successful to add a given extended address to the allowlist entry
            False: fail to add a given extended address to the allowlist entry
        """
        print('%s call addAllowMAC' % self)
        print(xEUI)
        if isinstance(xEUI, str):
            macAddr = xEUI
        else:
            macAddr = self.__convertLongToHex(xEUI)

        try:
            if self._addressfilterMode != 'allowlist':
                if self.__setAddressfilterMode('allowlist'):
                    self._addressfilterMode = 'allowlist'

            cmd = 'macfilter addr add %s' % macAddr
            print(cmd)
            ret = self.__executeCommand(cmd)[-1] == 'Done'

            self._addressfilterSet.add(macAddr)
            print('current allowlist entries:')
            for addr in self._addressfilterSet:
                print(addr)
            return ret

        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('addAllowMAC() Error: ' + str(e))

    @API
    def clearBlockList(self):
        """clear all entries in denylist table

        Returns:
            True: successful to clear the denylist
            False: fail to clear the denylist
        """
        print('%s call clearBlockList' % self)

        # remove all entries in denylist
        try:
            print('clearing denylist entries:')
            for addr in self._addressfilterSet:
                print(addr)

            # disable denylist
            if self.__setAddressfilterMode('disable'):
                self._addressfilterMode = 'disable'
                # clear ops
                cmd = 'macfilter addr clear'
                if self.__executeCommand(cmd)[-1] == 'Done':
                    self._addressfilterSet.clear()
                    return True
            return False
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('clearBlockList() Error: ' + str(e))

    @API
    def clearAllowList(self):
        """clear all entries in allowlist table

        Returns:
            True: successful to clear the allowlist
            False: fail to clear the allowlist
        """
        print('%s call clearAllowList' % self)

        # remove all entries in allowlist
        try:
            print('clearing allowlist entries:')
            for addr in self._addressfilterSet:
                print(addr)

            # disable allowlist
            if self.__setAddressfilterMode('disable'):
                self._addressfilterMode = 'disable'
                # clear ops
                cmd = 'macfilter addr clear'
                if self.__executeCommand(cmd)[-1] == 'Done':
                    self._addressfilterSet.clear()
                    return True
            return False
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('clearAllowList() Error: ' + str(e))

    @API
    def getDeviceRole(self):
        """get current device role in Thread Network"""
        print('%s call getDeviceRole' % self)
        return self.__executeCommand('state')[0]

    @API
    def joinNetwork(self, eRoleId):
        """make device ready to join the Thread Network with a given role

        Args:
            eRoleId: a given device role id

        Returns:
            True: ready to set Thread Network parameter for joining desired Network
        """
        print('%s call joinNetwork' % self)
        print(eRoleId)

        self.deviceRole = eRoleId
        mode = '-'
        try:
            if ModuleHelper.LeaderDutChannelFound and not self.hasSetChannel:
                self.channel = ModuleHelper.Default_Channel

            # FIXME: when Harness call setNetworkDataRequirement()?
            # only sleep end device requires stable networkdata now
            if eRoleId == Thread_Device_Role.Leader:
                print('join as leader')
                mode = 'rdn'
                if self.AutoDUTEnable is False:
                    # set ROUTER_DOWNGRADE_THRESHOLD
                    self.__setRouterDowngradeThreshold(33)
            elif eRoleId == Thread_Device_Role.Router:
                print('join as router')
                mode = 'rdn'
                if self.AutoDUTEnable is False:
                    # set ROUTER_DOWNGRADE_THRESHOLD
                    self.__setRouterDowngradeThreshold(33)
            elif eRoleId in (Thread_Device_Role.BR_1, Thread_Device_Role.BR_2):
                print('join as BBR')
                mode = 'rdn'
                if self.AutoDUTEnable is False:
                    # set ROUTER_DOWNGRADE_THRESHOLD
                    self.__setRouterDowngradeThreshold(33)
            elif eRoleId == Thread_Device_Role.SED:
                print('join as sleepy end device')
                mode = '-'
                self.__setPollPeriod(self.__sedPollPeriod)
            elif eRoleId == Thread_Device_Role.SSED:
                print('join as SSED')
                mode = '-'
                self.setCSLperiod(self.cslPeriod)
            elif eRoleId == Thread_Device_Role.EndDevice:
                print('join as end device')
                mode = 'rn'
            elif eRoleId == Thread_Device_Role.REED:
                print('join as REED')
                mode = 'rdn'
                if self.AutoDUTEnable is False:
                    # set ROUTER_UPGRADE_THRESHOLD
                    self.__setRouterUpgradeThreshold(0)
            elif eRoleId == Thread_Device_Role.EndDevice_FED:
                print('join as FED')
                mode = 'rdn'
                # always remain an ED, never request to be a router
                self.__disableRouterEligible()
            elif eRoleId == Thread_Device_Role.EndDevice_MED:
                print('join as MED')
                mode = 'rn'
            else:
                pass

            # set Thread device mode with a given role
            self.__setDeviceMode(mode)

            # start OpenThread
            self.__startOpenThread()
            self.sleep(3)

            if self._update_router_status and eRoleId == Thread_Device_Role.Router:
                self.__updateRouterStatus()

            self.sleep(5)  # increase delay temporally (+5s) to remedy TH's delay updates

            return True
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('joinNetwork() Error: ' + str(e))

    @API
    def getNetworkFragmentID(self):
        """get current partition id of Thread Network Partition from LeaderData

        Returns:
            The Thread network Partition Id
        """
        print('%s call getNetworkFragmentID' % self)
        if not self.__isOpenThreadRunning():
            print('OpenThread is not running')
            return None

        leaderData = []
        leaderData = self.__executeCommand('leaderdata')
        return int(leaderData[0].split()[2], 16)

    @API
    def getParentAddress(self):
        """get Thread device's parent extended address and rloc16 short address

        Returns:
            The extended address of parent in hex format
        """
        print('%s call getParentAddress' % self)
        parentInfo = []
        parentInfo = self.__executeCommand('parent')

        for line in parentInfo:
            if 'Done' in line:
                break
            elif 'Ext Addr' in line:
                eui = line.split()[2]
                print(eui)
            # elif 'Rloc' in line:
            #    rloc16 = line.split()[1]
            #    print(rloc16)
            else:
                pass

        return int(eui, 16)

    @API
    def powerDown(self):
        """power down the Thread device"""
        print('%s call powerDown' % self)
        self.__sendCommand('reset', expectEcho=False)
        self.isPowerDown = True

    @API
    def powerUp(self):
        """power up the Thread device"""
        print('%s call powerUp' % self)
        self.isPowerDown = False

        if not self.__isOpenThreadRunning():
            self.__startOpenThread()

    @API
    def reboot(self):
        """reset and rejoin to Thread Network without any timeout

        Returns:
            True: successful to reset and rejoin the Thread Network
            False: fail to reset and rejoin the Thread Network
        """
        print('%s call reboot' % self)
        try:
            self.__sendCommand('reset', expectEcho=False)
            self.isPowerDown = True
            self.sleep(3)

            self.__startOpenThread()
            self.sleep(3)

            if self.__executeCommand('state')[0] == 'disabled':
                print('[FAIL] reboot')
                return False
            else:
                return True
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('reboot() Error: ' + str(e))

    @API
    def ping(self, strDestination, ilength=0, hop_limit=64, timeout=5):
        """ send ICMPv6 echo request with a given length/hoplimit to a unicast
            destination address

        TODO: add hop_limit support

        Args:
            srcDestination: the unicast destination address of ICMPv6 echo request
            ilength: the size of ICMPv6 echo request payload
            hop_limit: hop limit

        """
        print('%s call ping' % self.port)
        print('destination: %s' % strDestination)
        cmd = 'ping %s %s 1 1 %d %d' % (strDestination, str(ilength), hop_limit, timeout)
        self.__executeCommand(cmd)
        time.sleep(1)

    @API
    def multicast_Ping(self, destination, length=20):
        """send ICMPv6 echo request with a given length to a multicast destination
           address

        Args:
            destination: the multicast destination address of ICMPv6 echo request
            length: the size of ICMPv6 echo request payload
        """
        print('%s call multicast_Ping' % self)
        print('destination: %s' % destination)
        try:
            cmd = 'ping %s %s' % (destination, str(length))
            print(cmd)
            self.__sendCommand(cmd)
            # wait echo reply
            self.sleep(1)
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('multicast_ping() Error: ' + str(e))

    @API
    def setPANID(self, xPAN):
        """set Thread Network PAN ID

        Args:
            xPAN: a given PAN ID in hex format

        Returns:
            True: successful to set the Thread Network PAN ID
            False: fail to set the Thread Network PAN ID
        """
        print('%s call setPANID' % self)
        print(xPAN)
        panid = ''
        try:
            if not isinstance(xPAN, str):
                panid = str(hex(xPAN))
                print(panid)

            cmd = 'panid %s' % panid
            datasetCmd = 'dataset panid %s' % panid
            self.hasActiveDatasetToCommit = True
            return self.__executeCommand(cmd)[-1] == 'Done' and self.__executeCommand(datasetCmd)[-1] == 'Done'
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('setPANID() Error: ' + str(e))

    @API
    def reset(self):
        """factory reset"""
        print('%s call reset' % self)

        self._deviceBeforeReset()

        self.__sendCommand('factoryreset', expectEcho=False)
        start_time = time.time()
        while time.time() < start_time + 10:
            time.sleep(0.3)
            try:
                self.__executeCommand('state', timeout=0.1)
                break
            except Exception:
                continue

        self.log('factoryreset finished in %dms', int(time.time() - start_time))

        self._deviceAfterReset()

    @API
    def removeRouter(self, xRouterId):
        """kickoff router with a given router id from the Thread Network

        Args:
            xRouterId: a given router id in hex format

        Returns:
            True: successful to remove the router from the Thread Network
            False: fail to remove the router from the Thread Network
        """
        print('%s call removeRouter' % self)
        print(xRouterId)
        routerId = ''
        routerId = self.__convertRlocToRouterId(xRouterId)
        print(routerId)

        if routerId is None:
            print('no matched xRouterId')
            return False

        try:
            cmd = 'releaserouterid %s' % routerId
            return self.__executeCommand(cmd)[-1] == 'Done'
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('removeRouter() Error: ' + str(e))

    @API
    def setDefaultValues(self):
        """set default mandatory Thread Network parameter value"""
        print('%s call setDefaultValues' % self)

        # initialize variables
        self.networkName = ModuleHelper.Default_NwkName
        self.networkKey = ModuleHelper.Default_NwkKey
        self.channel = ModuleHelper.Default_Channel
        self.channelMask = '0x7fff800'  # (0xffff << 11)
        self.panId = ModuleHelper.Default_PanId
        self.xpanId = ModuleHelper.Default_XpanId
        self.meshLocalPrefix = ModuleHelper.Default_MLPrefix
        # OT only accept hex format PSKc for now
        self.pskc = '00000000000000000000000000000001'
        self.securityPolicySecs = ModuleHelper.Default_SecurityPolicy
        self.securityPolicyFlags = 'onrcb'
        self.activetimestamp = ModuleHelper.Default_ActiveTimestamp
        # self.sedPollingRate = ModuleHelper.Default_Harness_SED_Polling_Rate
        self.__sedPollPeriod = 3 * 1000  # in milliseconds
        self.cslPeriod = 500  # in milliseconds
        self.deviceRole = None
        self.provisioningUrl = ''
        self.hasActiveDatasetToCommit = False
        self.logThread = Queue()
        self.logThreadStatus = self.logStatus['stop']
        self.joinCommissionedStatus = self.joinStatus['notstart']
        # indicate Thread device requests full or stable network data
        self.networkDataRequirement = ''
        # indicate if Thread device experiences a power down event
        self.isPowerDown = False
        # indicate AddressFilter mode ['disable', 'allowlist', 'denylist']
        self._addressfilterMode = 'disable'
        self._addressfilterSet = set()  # cache filter entries
        # indicate if Thread device is an active commissioner
        self.isActiveCommissioner = False
        # indicate that the channel has been set, in case the channel was set
        # to default when joining network
        self.hasSetChannel = False
        # indicate whether the default domain prefix is used.
        self.__useDefaultDomainPrefix = True
        self.__isUdpOpened = False
        self.IsBackboneRouter = False
        self.IsHost = False

        # BBR dataset
        self.bbrSeqNum = random.randint(0, 254)  # random seqnum except 255, so that BBR-TC-02 never need re-run
        self.bbrMlrTimeout = 3600
        self.bbrReRegDelay = 5

        # initialize device configuration
        try:
            self.setMAC(self.mac)
            self.__setChannelMask(self.channelMask)
            self.__setSecurityPolicy(self.securityPolicySecs, self.securityPolicyFlags)
            self.setChannel(self.channel)
            self.setPANID(self.panId)
            self.setXpanId(self.xpanId)
            self.setNetworkName(self.networkName)
            self.setNetworkKey(self.networkKey)
            self.setMLPrefix(self.meshLocalPrefix)
            self.setPSKc(self.pskc)
            self.setActiveTimestamp(self.activetimestamp)
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('setDefaultValue() Error: ' + str(e))

    @API
    def getDeviceConncetionStatus(self):
        """check if serial port connection is ready or not"""
        print('%s call getDeviceConnectionStatus' % self)
        return self.deviceConnected

    @API
    def setPollingRate(self, iPollingRate):
        """set data polling rate for sleepy end device

        Args:
            iPollingRate: data poll period of sleepy end device (in seconds)

        Returns:
            True: successful to set the data polling rate for sleepy end device
            False: fail to set the data polling rate for sleepy end device
        """
        print('%s call setPollingRate' % self)

        iPollingRate = int(iPollingRate * 1000)
        print(iPollingRate)

        if self.__sedPollPeriod != iPollingRate:
            self.__sedPollPeriod = iPollingRate

            # apply immediately
            if self.__isOpenThreadRunning():
                return self.__setPollPeriod(self.__sedPollPeriod)

        return True

    def __setPollPeriod(self, iPollPeriod):
        """set data poll period for sleepy end device

        Args:
            iPollPeriod: data poll period of sleepy end device (in milliseconds)

        Returns:
            True: successful to set the data poll period for sleepy end device
            False: fail to set the data poll period for sleepy end device
        """
        try:
            cmd = 'pollperiod %d' % iPollPeriod
            print(cmd)
            return self.__executeCommand(cmd)[-1] == 'Done'
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('__setPollPeriod() Error: ' + str(e))

    @API
    def setLinkQuality(self, EUIadr, LinkQuality):
        """set custom LinkQualityIn for all receiving messages from the specified EUIadr

        Args:
            EUIadr: a given extended address
            LinkQuality: a given custom link quality
                         link quality/link margin mapping table
                         3: 21 - 255 (dB)
                         2: 11 - 20 (dB)
                         1: 3 - 9 (dB)
                         0: 0 - 2 (dB)

        Returns:
            True: successful to set the link quality
            False: fail to set the link quality
        """
        print('%s call setLinkQuality' % self)
        print(EUIadr)
        print(LinkQuality)
        try:
            # process EUIadr
            euiHex = hex(EUIadr)
            euiStr = str(euiHex)
            euiStr = euiStr.rstrip('L')
            address64 = ''
            if '0x' in euiStr:
                address64 = self.__lstrip0x(euiStr)
                # prepend 0 at the beginning
                if len(address64) < 16:
                    address64 = address64.zfill(16)
                    print(address64)

            cmd = 'macfilter rss add-lqi %s %s' % (address64, str(LinkQuality))
            print(cmd)
            return self.__executeCommand(cmd)[-1] == 'Done'
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('setLinkQuality() Error: ' + str(e))

    @API
    def setOutBoundLinkQuality(self, LinkQuality):
        """set custom LinkQualityIn for all receiving messages from the any address

        Args:
            LinkQuality: a given custom link quality
                         link quality/link margin mapping table
                         3: 21 - 255 (dB)
                         2: 11 - 20 (dB)
                         1: 3 - 9 (dB)
                         0: 0 - 2 (dB)

        Returns:
            True: successful to set the link quality
            False: fail to set the link quality
        """
        print('%s call setOutBoundLinkQuality' % self)
        print(LinkQuality)
        try:
            cmd = 'macfilter rss add-lqi * %s' % str(LinkQuality)
            print(cmd)
            return self.__executeCommand(cmd)[-1] == 'Done'
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('setOutBoundLinkQuality() Error: ' + str(e))

    @API
    def removeRouterPrefix(self, prefixEntry):
        """remove the configured prefix on a border router

        Args:
            prefixEntry: a on-mesh prefix entry

        Returns:
            True: successful to remove the prefix entry from border router
            False: fail to remove the prefix entry from border router
        """
        print('%s call removeRouterPrefix' % self)
        print(prefixEntry)
        prefix = self.__convertIp6PrefixStringToIp6Address(str(prefixEntry))
        try:
            prefixLen = 64
            cmd = 'prefix remove %s/%d' % (prefix, prefixLen)
            print(cmd)
            if self.__executeCommand(cmd)[-1] == 'Done':
                # send server data ntf to leader
                return self.__executeCommand('netdata register')[-1] == 'Done'
            else:
                return False
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('removeRouterPrefix() Error: ' + str(e))

    @API
    def resetAndRejoin(self, timeout):
        """reset and join back Thread Network with a given timeout delay

        Args:
            timeout: a timeout interval before rejoin Thread Network

        Returns:
            True: successful to reset and rejoin Thread Network
            False: fail to reset and rejoin the Thread Network
        """
        print('%s call resetAndRejoin' % self)
        print(timeout)
        try:
            self.__sendCommand('reset', expectEcho=False)
            self.isPowerDown = True
            self.sleep(timeout)

            if self.deviceRole == Thread_Device_Role.SED:
                self.__setPollPeriod(self.__sedPollPeriod)

            self.__startOpenThread()
            self.sleep(3)

            if self.__executeCommand('state')[0] == 'disabled':
                print('[FAIL] reset and rejoin')
                return False
            return True
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('resetAndRejoin() Error: ' + str(e))

    @API
    def configBorderRouter(
        self,
        P_Prefix=None,
        P_stable=1,
        P_default=1,
        P_slaac_preferred=0,
        P_Dhcp=0,
        P_preference=0,
        P_on_mesh=1,
        P_nd_dns=0,
        P_dp=0,
    ):
        """configure the border router with a given prefix entry parameters

        Args:
            P_Prefix: IPv6 prefix that is available on the Thread Network
            P_stable: true if the default router is expected to be stable network data
            P_default: true if border router offers the default route for P_Prefix
            P_slaac_preferred: true if allowing auto-configure address using P_Prefix
            P_Dhcp: is true if border router is a DHCPv6 Agent
            P_preference: is two-bit signed integer indicating router preference
            P_on_mesh: is true if P_Prefix is considered to be on-mesh
            P_nd_dns: is true if border router is able to supply DNS information obtained via ND

        Returns:
            True: successful to configure the border router with a given prefix entry
            False: fail to configure the border router with a given prefix entry
        """
        print('%s call configBorderRouter' % self)
        assert TESTHARNESS_VERSION == TESTHARNESS_1_2 or P_dp == 0

        # turn off default domain prefix if configBorderRouter is called before joining network
        if TESTHARNESS_VERSION == TESTHARNESS_1_2 and P_dp == 0 and not self.__isOpenThreadRunning():
            self.__useDefaultDomainPrefix = False

        if TESTHARNESS_VERSION == TESTHARNESS_1_2:
            # TestHarness 1.2 converts 0x2001000000000000 to "2001000000000000"
            if P_Prefix is None:
                P_Prefix = 0xfd007d037d037d03

            P_Prefix = '%016x' % P_Prefix
        else:
            # TestHarness 1.1 converts 2001000000000000 to "2001000000000000" (it's wrong, but not fixed yet.)
            P_Prefix = str(P_Prefix)
            int(P_Prefix, 16)

        prefix = self.__convertIp6PrefixStringToIp6Address(P_Prefix)
        print(prefix)
        parameter = ''
        prf = ''

        if P_dp:
            P_slaac_preferred = 1

        if P_slaac_preferred == 1:
            parameter += 'p'
            parameter += 'a'

        if P_stable == 1:
            parameter += 's'

        if P_default == 1:
            parameter += 'r'

        if P_Dhcp == 1:
            parameter += 'd'

        if P_on_mesh == 1:
            parameter += 'o'

        if P_dp == 1:
            assert P_slaac_preferred and P_default and P_on_mesh and P_stable
            parameter += 'D'

        if P_preference == 1:
            prf = 'high'
        elif P_preference == 0:
            prf = 'med'
        elif P_preference == -1:
            prf = 'low'
        else:
            pass

        cmd = 'prefix add %s/64 %s %s' % (prefix, parameter, prf)
        print(cmd)
        if self.__executeCommand(cmd)[-1] == 'Done':
            # if prefix configured before starting OpenThread stack
            # do not send out server data ntf pro-actively
            if not self.__isOpenThreadRunning():
                return True
            else:
                # send server data ntf to leader
                return self.__executeCommand('netdata register')[-1] == 'Done'
        else:
            return False

    @API
    def setNetworkIDTimeout(self, iNwkIDTimeOut):
        """set networkid timeout for Thread device

        Args:
            iNwkIDTimeOut: a given NETWORK_ID_TIMEOUT

        Returns:
            True: successful to set NETWORK_ID_TIMEOUT
            False: fail to set NETWORK_ID_TIMEOUT
        """
        print('%s call setNetworkIDTimeout' % self)
        print(iNwkIDTimeOut)
        iNwkIDTimeOut /= 1000
        try:
            cmd = 'networkidtimeout %s' % str(iNwkIDTimeOut)
            print(cmd)
            return self.__executeCommand(cmd)[-1] == 'Done'
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('setNetworkIDTimeout() Error: ' + str(e))

    @API
    def setKeepAliveTimeOut(self, iTimeOut):
        """set keep alive timeout for device
           has been deprecated and also set SED polling rate

        Args:
            iTimeOut: data poll period for sleepy end device

        Returns:
            True: successful to set the data poll period for SED
            False: fail to set the data poll period for SED
        """
        print('%s call setKeepAliveTimeOut' % self)
        iTimeOut *= 1000
        print(int(iTimeOut))
        try:
            cmd = 'pollperiod %d' % int(iTimeOut)
            print(cmd)
            return self.__executeCommand(cmd)[-1] == 'Done'
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('setKeepAliveTimeOut() Error: ' + str(e))

    @API
    def setKeySequenceCounter(self, iKeySequenceValue):
        """ set the Key sequence counter corresponding to Thread network key

        Args:
            iKeySequenceValue: key sequence value

        Returns:
            True: successful to set the key sequence
            False: fail to set the key sequence
        """
        print('%s call setKeySequenceCounter' % self)
        print(iKeySequenceValue)
        try:
            # avoid key switch guard timer protection for reference device
            self.__setKeySwitchGuardTime(0)

            cmd = 'keysequence counter %s' % str(iKeySequenceValue)
            if self.__executeCommand(cmd)[-1] == 'Done':
                self.sleep(1)
                return True
            else:
                return False
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('setKeySequenceCounter() Error; ' + str(e))

    @API
    def getKeySequenceCounter(self):
        """get current Thread Network key sequence"""
        print('%s call getKeySequenceCounter' % self)
        keySequence = ''
        keySequence = self.__executeCommand('keysequence counter')[0]
        return keySequence

    @API
    def incrementKeySequenceCounter(self, iIncrementValue=1):
        """increment the key sequence with a given value

        Args:
            iIncrementValue: specific increment value to be added

        Returns:
            True: successful to increment the key sequence with a given value
            False: fail to increment the key sequence with a given value
        """
        print('%s call incrementKeySequenceCounter' % self)
        print(iIncrementValue)
        currentKeySeq = ''
        try:
            # avoid key switch guard timer protection for reference device
            self.__setKeySwitchGuardTime(0)
            currentKeySeq = self.getKeySequenceCounter()
            keySequence = int(currentKeySeq, 10) + iIncrementValue
            print(keySequence)
            return self.setKeySequenceCounter(keySequence)
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('incrementKeySequenceCounter() Error: ' + str(e))

    @API
    def setNetworkDataRequirement(self, eDataRequirement):
        """set whether the Thread device requires the full network data
           or only requires the stable network data

        Args:
            eDataRequirement: is true if requiring the full network data

        Returns:
            True: successful to set the network requirement
        """
        print('%s call setNetworkDataRequirement' % self)
        print(eDataRequirement)

        if eDataRequirement == Device_Data_Requirement.ALL_DATA:
            self.networkDataRequirement = 'n'
        return True

    @API
    def configExternalRouter(self, P_Prefix, P_stable, R_Preference=0):
        """configure border router with a given external route prefix entry

        Args:
            P_Prefix: IPv6 prefix for the route
            P_Stable: is true if the external route prefix is stable network data
            R_Preference: a two-bit signed integer indicating Router preference
                          1: high
                          0: medium
                         -1: low

        Returns:
            True: successful to configure the border router with a given external route prefix
            False: fail to configure the border router with a given external route prefix
        """
        print('%s call configExternalRouter' % self)
        print(P_Prefix)
        stable = ''
        prefix = self.__convertIp6PrefixStringToIp6Address(str(P_Prefix))
        try:
            if R_Preference == 1:
                prf = 'high'
            elif R_Preference == 0:
                prf = 'med'
            elif R_Preference == -1:
                prf = 'low'
            else:
                pass

            if P_stable:
                stable += 's'
                cmd = 'route add %s/64 %s %s' % (prefix, stable, prf)
            else:
                cmd = 'route add %s/64 %s' % (prefix, prf)
            print(cmd)

            if self.__executeCommand(cmd)[-1] == 'Done':
                # send server data ntf to leader
                return self.__executeCommand('netdata register')[-1] == 'Done'
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('configExternalRouter() Error: ' + str(e))

    @API
    def getNeighbouringRouters(self):
        """get neighboring routers information

        Returns:
            neighboring routers' extended address
        """
        print('%s call getNeighbouringRouters' % self)
        try:
            routerInfo = []
            routerList = []
            routerList = self.__executeCommand('router list')[0].split()
            print(routerList)

            if 'Done' in routerList:
                print('no neighbouring routers')
                return None

            for index in routerList:
                router = []
                cmd = 'router %s' % index
                router = self.__executeCommand(cmd)

                for line in router:
                    if 'Done' in line:
                        break
                    # elif 'Rloc' in line:
                    #    rloc16 = line.split()[1]
                    elif 'Ext Addr' in line:
                        eui = line.split()[2]
                        routerInfo.append(int(eui, 16))
                    # elif 'LQI In' in line:
                    #    lqi_in = line.split()[1]
                    # elif 'LQI Out' in line:
                    #    lqi_out = line.split()[1]
                    else:
                        pass

            print(routerInfo)
            return routerInfo
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('getNeighbouringDevice() Error: ' + str(e))

    @API
    def getChildrenInfo(self):
        """get all children information

        Returns:
            children's extended address
        """
        print('%s call getChildrenInfo' % self)
        try:
            childrenInfoAll = []
            childrenInfo = {'EUI': 0, 'Rloc16': 0, 'MLEID': ''}
            childrenList = self.__executeCommand('child list')[0].split()
            print(childrenList)

            if 'Done' in childrenList:
                print('no children')
                return None

            for index in childrenList:
                cmd = 'child %s' % index
                child = []
                child = self.__executeCommand(cmd)

                for line in child:
                    if 'Done' in line:
                        break
                    elif 'Rloc' in line:
                        rloc16 = line.split()[1]
                    elif 'Ext Addr' in line:
                        eui = line.split()[2]
                    # elif 'Child ID' in line:
                    #    child_id = line.split()[2]
                    # elif 'Mode' in line:
                    #    mode = line.split()[1]
                    else:
                        pass

                childrenInfo['EUI'] = int(eui, 16)
                childrenInfo['Rloc16'] = int(rloc16, 16)
                # children_info['MLEID'] = self.getMLEID()

                childrenInfoAll.append(childrenInfo['EUI'])
                # childrenInfoAll.append(childrenInfo)

            print(childrenInfoAll)
            return childrenInfoAll
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('getChildrenInfo() Error: ' + str(e))

    @API
    def setXpanId(self, xPanId):
        """set extended PAN ID of Thread Network

        Args:
            xPanId: extended PAN ID in hex format

        Returns:
            True: successful to set the extended PAN ID
            False: fail to set the extended PAN ID
        """
        xpanid = ''
        print('%s call setXpanId' % self)
        print(xPanId)
        try:
            if not isinstance(xPanId, str):
                xpanid = self.__convertLongToHex(xPanId, 16)
                cmd = 'extpanid %s' % xpanid
                datasetCmd = 'dataset extpanid %s' % xpanid
            else:
                xpanid = xPanId
                cmd = 'extpanid %s' % xpanid
                datasetCmd = 'dataset extpanid %s' % xpanid

            self.xpanId = xpanid
            self.hasActiveDatasetToCommit = True
            return self.__executeCommand(cmd)[-1] == 'Done' and self.__executeCommand(datasetCmd)[-1] == 'Done'
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('setXpanId() Error: ' + str(e))

    @API
    def getNeighbouringDevices(self):
        """gets the neighboring devices' extended address to compute the DUT
           extended address automatically

        Returns:
            A list including extended address of neighboring routers, parent
            as well as children
        """
        print('%s call getNeighbouringDevices' % self)
        neighbourList = []

        # get parent info
        parentAddr = self.getParentAddress()
        if parentAddr != 0:
            neighbourList.append(parentAddr)

        # get ED/SED children info
        childNeighbours = self.getChildrenInfo()
        if childNeighbours is not None and len(childNeighbours) > 0:
            for entry in childNeighbours:
                neighbourList.append(entry)

        # get neighboring routers info
        routerNeighbours = self.getNeighbouringRouters()
        if routerNeighbours is not None and len(routerNeighbours) > 0:
            for entry in routerNeighbours:
                neighbourList.append(entry)

        print(neighbourList)
        return neighbourList

    @API
    def setPartationId(self, partationId):
        """set Thread Network Partition ID

        Args:
            partitionId: partition id to be set by leader

        Returns:
            True: successful to set the Partition ID
            False: fail to set the Partition ID
        """
        print('%s call setPartationId' % self)
        print(partationId)

        cmd = 'partitionid preferred %s' % (str(hex(partationId)).rstrip('L'))
        print(cmd)
        return self.__executeCommand(cmd)[-1] == 'Done'

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
        assert not eth
        # get global addrs set if multiple
        globalAddrs = self.__getGlobal()

        if filterByPrefix is None:
            return globalAddrs[0]
        else:
            for fullIp in globalAddrs:
                if fullIp.startswith(filterByPrefix):
                    print('target global %s' % fullIp)
                    return fullIp
            print('no global address matched')
            return str(globalAddrs[0])

    @API
    def getShortAddress(self):
        """get Rloc16 short address of Thread device"""
        print('%s call getShortAddress' % self)
        return self.getRloc16()

    @API
    def getULA64(self):
        """get mesh local EID of Thread device"""
        print('%s call getULA64' % self)
        return self.__executeCommand('ipaddr mleid')[0]

    @API
    def setMLPrefix(self, sMeshLocalPrefix):
        """set mesh local prefix"""
        print('%s call setMLPrefix' % self)
        try:
            cmd = 'dataset meshlocalprefix %s' % sMeshLocalPrefix
            self.hasActiveDatasetToCommit = True
            return self.__executeCommand(cmd)[-1] == 'Done'
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('setMLPrefix() Error: ' + str(e))

    @API
    def getML16(self):
        """get mesh local 16 unicast address (Rloc)"""
        print('%s call getML16' % self)
        return self.getRloc()

    @API
    def downgradeToDevice(self):
        pass

    @API
    def upgradeToRouter(self):
        pass

    @API
    def forceSetSlaac(self, slaacAddress):
        """force to set a slaac IPv6 address to Thread interface

        Args:
            slaacAddress: a slaac IPv6 address to be set

        Returns:
            True: successful to set slaac address to Thread interface
            False: fail to set slaac address to Thread interface
        """
        print('%s call forceSetSlaac' % self)
        print(slaacAddress)
        try:
            cmd = 'ipaddr add %s' % str(slaacAddress)
            print(cmd)
            return self.__executeCommand(cmd)[-1] == 'Done'
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('forceSetSlaac() Error: ' + str(e))

    @API
    def setSleepyNodePollTime(self):
        pass

    @API
    def enableAutoDUTObjectFlag(self):
        """set AutoDUTenable flag"""
        print('%s call enableAutoDUTObjectFlag' % self)
        self.AutoDUTEnable = True

    @API
    def getChildTimeoutValue(self):
        """get child timeout"""
        print('%s call getChildTimeoutValue' % self)
        childTimeout = self.__executeCommand('childtimeout')[0]
        return int(childTimeout)

    @API
    def diagnosticGet(self, strDestinationAddr, listTLV_ids=[]):
        if not listTLV_ids:
            return

        if len(listTLV_ids) == 0:
            return

        cmd = 'networkdiagnostic get %s %s' % (
            strDestinationAddr,
            ' '.join([str(tlv) for tlv in listTLV_ids]),
        )
        print(cmd)

        return self.__sendCommand(cmd, expectEcho=False)

    @API
    def diagnosticReset(self, strDestinationAddr, listTLV_ids=[]):
        if not listTLV_ids:
            return

        if len(listTLV_ids) == 0:
            return

        cmd = 'networkdiagnostic reset %s %s' % (
            strDestinationAddr,
            ' '.join([str(tlv) for tlv in listTLV_ids]),
        )
        print(cmd)

        return self.__executeCommand(cmd)

    @API
    def diagnosticQuery(self, strDestinationAddr, listTLV_ids=[]):
        self.diagnosticGet(strDestinationAddr, listTLV_ids)

    @API
    def startNativeCommissioner(self, strPSKc='GRLPASSPHRASE'):
        # TODO: Support the whole Native Commissioner functionality
        # Currently it only aims to trigger a Discovery Request message to pass
        # Certification test 5.8.4
        print('%s call startNativeCommissioner' % self)
        self.__executeCommand('ifconfig up')
        cmd = 'joiner start %s' % (strPSKc)
        print(cmd)
        return self.__executeCommand(cmd)[-1] == 'Done'

    @API
    def getBorderAgentPort(self):
        return int(self.__executeCommand('ba port')[0])

    @API
    def startExternalCommissioner(self, baAddr, baPort):
        """Start external commissioner
        Args:
            baAddr: A string represents the border agent address.
            baPort: An integer represents the border agent port.
        Returns:
            A boolean indicates whether this function succeed.
        """
        if self.externalCommissioner is None:
            config = commissioner.Configuration()
            config.isCcmMode = False
            config.domainName = OpenThreadTHCI.DOMAIN_NAME
            config.pskc = bytearray.fromhex(self.pskc)

            self.externalCommissioner = OTCommissioner(config, self)

        if not self.externalCommissioner.isActive():
            self.externalCommissioner.start(baAddr, baPort)

        if not self.externalCommissioner.isActive():
            raise commissioner.Error("external commissioner is not active")

        return True

    @API
    def stopExternalCommissioner(self):
        """Stop external commissioner
        Returns:
            A boolean indicates whether this function succeed.
        """

        if self.externalCommissioner is not None:
            self.externalCommissioner.stop()
            return not self.externalCommissioner.isActive()

    @API
    def startCollapsedCommissioner(self, role=Thread_Device_Role.Leader):
        """start Collapsed Commissioner

        Returns:
            True: successful to start Commissioner
            False: fail to start Commissioner
        """
        print('%s call startCollapsedCommissioner' % self)
        if self.__startOpenThread():
            self.sleep(20)
            cmd = 'commissioner start'
            print(cmd)
            if self.__executeCommand(cmd)[-1] == 'Done':
                self.isActiveCommissioner = True
                self.sleep(20)  # time for petition process
                return True
        return False

    @API
    def setJoinKey(self, strPSKc):
        pass

    @API
    def scanJoiner(self, xEUI='*', strPSKd='THREADJPAKETEST'):
        """scan Joiner

        Args:
            xEUI: Joiner's EUI-64
            strPSKd: Joiner's PSKd for commissioning

        Returns:
            True: successful to add Joiner's steering data
            False: fail to add Joiner's steering data
        """
        self.log("scanJoiner on channel %s", self.getChannel())

        # long timeout value to avoid automatic joiner removal (in seconds)
        timeout = 500

        if not isinstance(xEUI, str):
            eui64 = self.__convertLongToHex(xEUI, 16)
        else:
            eui64 = xEUI

        strPSKd = self.__normalizePSKd(strPSKd)

        cmd = 'commissioner joiner add %s %s %s' % (
            self._deviceEscapeEscapable(eui64),
            strPSKd,
            str(timeout),
        )

        print(cmd)
        if self.__executeCommand(cmd)[-1] == 'Done':
            if self.logThreadStatus == self.logStatus['stop']:
                self.logThread = ThreadRunner.run(target=self.__readCommissioningLogs, args=(120,))
            return True
        else:
            return False

    @staticmethod
    def __normalizePSKd(strPSKd):
        return strPSKd.upper().replace('I', '1').replace('O', '0').replace('Q', '0').replace('Z', '2')

    @API
    def setProvisioningUrl(self, strURL='grl.com'):
        """set provisioning Url

        Args:
            strURL: Provisioning Url string

        Returns:
            True: successful to set provisioning Url
            False: fail to set provisioning Url
        """
        print('%s call setProvisioningUrl' % self)
        self.provisioningUrl = strURL
        if self.deviceRole == Thread_Device_Role.Commissioner:
            cmd = 'commissioner provisioningurl %s' % (strURL)
            return self.__executeCommand(cmd)[-1] == 'Done'
        return True

    @API
    def allowCommission(self):
        """start commissioner candidate petition process

        Returns:
            True: successful to start commissioner candidate petition process
            False: fail to start commissioner candidate petition process
        """
        print('%s call allowCommission' % self)
        try:
            cmd = 'commissioner start'
            print(cmd)
            if self.__executeCommand(cmd)[-1] == 'Done':
                self.isActiveCommissioner = True
                # time for petition process and at least one keep alive
                self.sleep(3)
                return True
            else:
                return False
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('allowcommission() error: ' + str(e))

    @API
    def joinCommissioned(self, strPSKd='THREADJPAKETEST', waitTime=20):
        """start joiner

        Args:
            strPSKd: Joiner's PSKd

        Returns:
            True: successful to start joiner
            False: fail to start joiner
        """
        self.log("joinCommissioned on channel %s", self.getChannel())
        self.__executeCommand('ifconfig up')
        strPSKd = self.__normalizePSKd(strPSKd)
        cmd = 'joiner start %s %s' % (strPSKd, self.provisioningUrl)
        print(cmd)
        if self.__executeCommand(cmd)[-1] == 'Done':
            maxDuration = 150  # seconds
            self.joinCommissionedStatus = self.joinStatus['ongoing']

            if self.logThreadStatus == self.logStatus['stop']:
                self.logThread = ThreadRunner.run(target=self.__readCommissioningLogs, args=(maxDuration,))

            t_end = time.time() + maxDuration
            while time.time() < t_end:
                if self.joinCommissionedStatus == self.joinStatus['succeed']:
                    break
                elif self.joinCommissionedStatus == self.joinStatus['failed']:
                    return False

                self.sleep(1)

            self.__executeCommand('thread start')
            self.sleep(30)
            return True
        else:
            return False

    @API
    def getCommissioningLogs(self):
        """get Commissioning logs

        Returns:
           Commissioning logs
        """
        rawLogs = self.logThread.get()
        ProcessedLogs = []
        payload = []
        while not rawLogs.empty():
            rawLogEach = rawLogs.get()
            print(rawLogEach)
            if '[THCI]' not in rawLogEach:
                continue

            EncryptedPacket = PlatformDiagnosticPacket()
            infoList = rawLogEach.split('[THCI]')[1].split(']')[0].split('|')
            for eachInfo in infoList:
                print(eachInfo)
                info = eachInfo.split('=')
                infoType = info[0].strip()
                infoValue = info[1].strip()
                if 'direction' in infoType:
                    EncryptedPacket.Direction = (PlatformDiagnosticPacket_Direction.IN
                                                 if 'recv' in infoValue else PlatformDiagnosticPacket_Direction.OUT if
                                                 'send' in infoValue else PlatformDiagnosticPacket_Direction.UNKNOWN)
                elif 'type' in infoType:
                    EncryptedPacket.Type = (PlatformDiagnosticPacket_Type.JOIN_FIN_req if 'JOIN_FIN.req' in infoValue
                                            else PlatformDiagnosticPacket_Type.JOIN_FIN_rsp if 'JOIN_FIN.rsp'
                                            in infoValue else PlatformDiagnosticPacket_Type.JOIN_ENT_req if
                                            'JOIN_ENT.ntf' in infoValue else PlatformDiagnosticPacket_Type.JOIN_ENT_rsp
                                            if 'JOIN_ENT.rsp' in infoValue else PlatformDiagnosticPacket_Type.UNKNOWN)
                elif 'len' in infoType:
                    bytesInEachLine = 16
                    EncryptedPacket.TLVsLength = int(infoValue)
                    payloadLineCount = (int(infoValue) + bytesInEachLine - 1) / bytesInEachLine
                    while payloadLineCount > 0:
                        payloadLineCount = payloadLineCount - 1
                        payloadLine = rawLogs.get()
                        payloadSplit = payloadLine.split('|')
                        for block in range(1, 3):
                            payloadBlock = payloadSplit[block]
                            payloadValues = payloadBlock.split(' ')
                            for num in range(1, 9):
                                if '..' not in payloadValues[num]:
                                    payload.append(int(payloadValues[num], 16))

                    EncryptedPacket.TLVs = (PlatformPackets.read(EncryptedPacket.Type, payload)
                                            if payload != [] else [])

            ProcessedLogs.append(EncryptedPacket)
        return ProcessedLogs

    @API
    def MGMT_ED_SCAN(
        self,
        sAddr,
        xCommissionerSessionId,
        listChannelMask,
        xCount,
        xPeriod,
        xScanDuration,
    ):
        """send MGMT_ED_SCAN message to a given destinaition.

        Args:
            sAddr: IPv6 destination address for this message
            xCommissionerSessionId: commissioner session id
            listChannelMask: a channel array to indicate which channels to be scaned
            xCount: number of IEEE 802.15.4 ED Scans (milliseconds)
            xPeriod: Period between successive IEEE802.15.4 ED Scans (milliseconds)
            xScanDuration: ScanDuration when performing an IEEE 802.15.4 ED Scan (milliseconds)

        Returns:
            True: successful to send MGMT_ED_SCAN message.
            False: fail to send MGMT_ED_SCAN message
        """
        print('%s call MGMT_ED_SCAN' % self)
        channelMask = ''
        channelMask = '0x' + self.__convertLongToHex(self.__convertChannelMask(listChannelMask))
        try:
            cmd = 'commissioner energy %s %s %s %s %s' % (
                channelMask,
                xCount,
                xPeriod,
                xScanDuration,
                sAddr,
            )
            print(cmd)
            return self.__executeCommand(cmd)[-1] == 'Done'
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('MGMT_ED_SCAN() error: ' + str(e))

    @API
    def MGMT_PANID_QUERY(self, sAddr, xCommissionerSessionId, listChannelMask, xPanId):
        """send MGMT_PANID_QUERY message to a given destination

        Args:
            xPanId: a given PAN ID to check the conflicts

        Returns:
            True: successful to send MGMT_PANID_QUERY message.
            False: fail to send MGMT_PANID_QUERY message.
        """
        print('%s call MGMT_PANID_QUERY' % self)
        panid = ''
        channelMask = ''
        channelMask = '0x' + self.__convertLongToHex(self.__convertChannelMask(listChannelMask))

        if not isinstance(xPanId, str):
            panid = str(hex(xPanId))

        try:
            cmd = 'commissioner panid %s %s %s' % (panid, channelMask, sAddr)
            print(cmd)
            return self.__executeCommand(cmd)[-1] == 'Done'
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('MGMT_PANID_QUERY() error: ' + str(e))

    @API
    def MGMT_ANNOUNCE_BEGIN(self, sAddr, xCommissionerSessionId, listChannelMask, xCount, xPeriod):
        """send MGMT_ANNOUNCE_BEGIN message to a given destination

        Returns:
            True: successful to send MGMT_ANNOUNCE_BEGIN message.
            False: fail to send MGMT_ANNOUNCE_BEGIN message.
        """
        print('%s call MGMT_ANNOUNCE_BEGIN' % self)
        channelMask = ''
        channelMask = '0x' + self.__convertLongToHex(self.__convertChannelMask(listChannelMask))
        try:
            cmd = 'commissioner announce %s %s %s %s' % (
                channelMask,
                xCount,
                xPeriod,
                sAddr,
            )
            print(cmd)
            return self.__executeCommand(cmd)[-1] == 'Done'
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('MGMT_ANNOUNCE_BEGIN() error: ' + str(e))

    @API
    def MGMT_ACTIVE_GET(self, Addr='', TLVs=[]):
        """send MGMT_ACTIVE_GET command

        Returns:
            True: successful to send MGMT_ACTIVE_GET
            False: fail to send MGMT_ACTIVE_GET
        """
        print('%s call MGMT_ACTIVE_GET' % self)
        try:
            cmd = 'dataset mgmtgetcommand active'

            if Addr != '':
                cmd += ' address '
                cmd += Addr

            if len(TLVs) != 0:
                tlvs = ''.join('%02x' % tlv for tlv in TLVs)
                cmd += ' -x '
                cmd += tlvs

            print(cmd)

            return self.__executeCommand(cmd)[-1] == 'Done'

        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('MGMT_ACTIVE_GET() Error: ' + str(e))

    @API
    def MGMT_ACTIVE_SET(
        self,
        sAddr='',
        xCommissioningSessionId=None,
        listActiveTimestamp=None,
        listChannelMask=None,
        xExtendedPanId=None,
        sNetworkName=None,
        sPSKc=None,
        listSecurityPolicy=None,
        xChannel=None,
        sMeshLocalPrefix=None,
        xMasterKey=None,
        xPanId=None,
        xTmfPort=None,
        xSteeringData=None,
        xBorderRouterLocator=None,
        BogusTLV=None,
        xDelayTimer=None,
    ):
        """send MGMT_ACTIVE_SET command

        Returns:
            True: successful to send MGMT_ACTIVE_SET
            False: fail to send MGMT_ACTIVE_SET
        """
        print('%s call MGMT_ACTIVE_SET' % self)
        try:
            cmd = 'dataset mgmtsetcommand active'

            if listActiveTimestamp is not None:
                cmd += ' activetimestamp '
                cmd += str(listActiveTimestamp[0])

            if xExtendedPanId is not None:
                cmd += ' extpanid '
                xpanid = self.__convertLongToHex(xExtendedPanId, 16)

                cmd += xpanid

            if sNetworkName is not None:
                cmd += ' networkname '
                cmd += self._deviceEscapeEscapable(str(sNetworkName))

            if xChannel is not None:
                cmd += ' channel '
                cmd += str(xChannel)

            if sMeshLocalPrefix is not None:
                cmd += ' localprefix '
                cmd += str(sMeshLocalPrefix)

            if xMasterKey is not None:
                cmd += ' networkkey '
                key = self.__convertLongToHex(xMasterKey, 32)

                cmd += key

            if xPanId is not None:
                cmd += ' panid '
                cmd += str(xPanId)

            if listChannelMask is not None:
                cmd += ' channelmask '
                cmd += '0x' + self.__convertLongToHex(self.__convertChannelMask(listChannelMask))

            if (sPSKc is not None or listSecurityPolicy is not None or xCommissioningSessionId is not None or
                    xTmfPort is not None or xSteeringData is not None or xBorderRouterLocator is not None or
                    BogusTLV is not None):
                cmd += ' -x '

            if sPSKc is not None:
                cmd += '0410'
                stretchedPskc = Thread_PBKDF2.get(
                    sPSKc,
                    ModuleHelper.Default_XpanId,
                    ModuleHelper.Default_NwkName,
                )
                pskc = '%x' % stretchedPskc

                if len(pskc) < 32:
                    pskc = pskc.zfill(32)

                cmd += pskc

            if listSecurityPolicy is not None:
                cmd += '0c03'

                rotationTime = 0
                policyBits = 0

                # previous passing way listSecurityPolicy=[True, True, 3600,
                # False, False, True]
                if len(listSecurityPolicy) == 6:
                    rotationTime = listSecurityPolicy[2]

                    # the last three reserved bits must be 1
                    policyBits = 0b00000111

                    if listSecurityPolicy[0]:
                        policyBits = policyBits | 0b10000000
                    if listSecurityPolicy[1]:
                        policyBits = policyBits | 0b01000000
                    if listSecurityPolicy[3]:
                        policyBits = policyBits | 0b00100000
                    if listSecurityPolicy[4]:
                        policyBits = policyBits | 0b00010000
                    if listSecurityPolicy[5]:
                        policyBits = policyBits | 0b00001000
                else:
                    # new passing way listSecurityPolicy=[3600, 0b11001111]
                    rotationTime = listSecurityPolicy[0]
                    policyBits = listSecurityPolicy[1]

                policy = str(hex(rotationTime))[2:]

                if len(policy) < 4:
                    policy = policy.zfill(4)

                cmd += policy

                cmd += str(hex(policyBits))[2:]

            if xCommissioningSessionId is not None:
                cmd += '0b02'
                sessionid = str(hex(xCommissioningSessionId))[2:]

                if len(sessionid) < 4:
                    sessionid = sessionid.zfill(4)

                cmd += sessionid

            if xBorderRouterLocator is not None:
                cmd += '0902'
                locator = str(hex(xBorderRouterLocator))[2:]

                if len(locator) < 4:
                    locator = locator.zfill(4)

                cmd += locator

            if xSteeringData is not None:
                steeringData = self.__convertLongToHex(xSteeringData)
                cmd += '08' + str(len(steeringData) / 2).zfill(2)
                cmd += steeringData

            if BogusTLV is not None:
                cmd += '8202aa55'

            print(cmd)

            return self.__executeCommand(cmd)[-1] == 'Done'

        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('MGMT_ACTIVE_SET() Error: ' + str(e))

    @API
    def MGMT_PENDING_GET(self, Addr='', TLVs=[]):
        """send MGMT_PENDING_GET command

        Returns:
            True: successful to send MGMT_PENDING_GET
            False: fail to send MGMT_PENDING_GET
        """
        print('%s call MGMT_PENDING_GET' % self)
        try:
            cmd = 'dataset mgmtgetcommand pending'

            if Addr != '':
                cmd += ' address '
                cmd += Addr

            if len(TLVs) != 0:
                tlvs = ''.join('%02x' % tlv for tlv in TLVs)
                cmd += ' -x '
                cmd += tlvs

            print(cmd)

            return self.__executeCommand(cmd)[-1] == 'Done'

        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('MGMT_PENDING_GET() Error: ' + str(e))

    @API
    def MGMT_PENDING_SET(
        self,
        sAddr='',
        xCommissionerSessionId=None,
        listPendingTimestamp=None,
        listActiveTimestamp=None,
        xDelayTimer=None,
        xChannel=None,
        xPanId=None,
        xMasterKey=None,
        sMeshLocalPrefix=None,
        sNetworkName=None,
    ):
        """send MGMT_PENDING_SET command

        Returns:
            True: successful to send MGMT_PENDING_SET
            False: fail to send MGMT_PENDING_SET
        """
        print('%s call MGMT_PENDING_SET' % self)
        try:
            cmd = 'dataset mgmtsetcommand pending'

            if listPendingTimestamp is not None:
                cmd += ' pendingtimestamp '
                cmd += str(listPendingTimestamp[0])

            if listActiveTimestamp is not None:
                cmd += ' activetimestamp '
                cmd += str(listActiveTimestamp[0])

            if xDelayTimer is not None:
                cmd += ' delaytimer '
                cmd += str(xDelayTimer)
                # cmd += ' delaytimer 3000000'

            if xChannel is not None:
                cmd += ' channel '
                cmd += str(xChannel)

            if xPanId is not None:
                cmd += ' panid '
                cmd += str(xPanId)

            if xMasterKey is not None:
                cmd += ' networkkey '
                key = self.__convertLongToHex(xMasterKey, 32)

                cmd += key

            if sMeshLocalPrefix is not None:
                cmd += ' localprefix '
                cmd += str(sMeshLocalPrefix)

            if sNetworkName is not None:
                cmd += ' networkname '
                cmd += self._deviceEscapeEscapable(str(sNetworkName))

            if xCommissionerSessionId is not None:
                cmd += ' -x '
                cmd += '0b02'
                sessionid = str(hex(xCommissionerSessionId))[2:]

                if len(sessionid) < 4:
                    sessionid = sessionid.zfill(4)

                cmd += sessionid

            print(cmd)

            return self.__executeCommand(cmd)[-1] == 'Done'

        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('MGMT_PENDING_SET() Error: ' + str(e))

    @API
    def MGMT_COMM_GET(self, Addr='ff02::1', TLVs=[]):
        """send MGMT_COMM_GET command

        Returns:
            True: successful to send MGMT_COMM_GET
            False: fail to send MGMT_COMM_GET
        """
        print('%s call MGMT_COMM_GET' % self)
        try:
            cmd = 'commissioner mgmtget'

            if len(TLVs) != 0:
                tlvs = ''.join('%02x' % tlv for tlv in TLVs)
                cmd += ' -x '
                cmd += tlvs

            print(cmd)

            return self.__executeCommand(cmd)[-1] == 'Done'

        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('MGMT_COMM_GET() Error: ' + str(e))

    @API
    def MGMT_COMM_SET(
        self,
        Addr='ff02::1',
        xCommissionerSessionID=None,
        xSteeringData=None,
        xBorderRouterLocator=None,
        xChannelTlv=None,
        ExceedMaxPayload=False,
    ):
        """send MGMT_COMM_SET command

        Returns:
            True: successful to send MGMT_COMM_SET
            False: fail to send MGMT_COMM_SET
        """
        print('%s call MGMT_COMM_SET' % self)
        try:
            cmd = 'commissioner mgmtset'

            if xCommissionerSessionID is not None:
                # use assigned session id
                cmd += ' sessionid '
                cmd += str(xCommissionerSessionID)
            elif xCommissionerSessionID is None:
                # use original session id
                if self.isActiveCommissioner is True:
                    cmd += ' sessionid '
                    cmd += self.__getCommissionerSessionId()
                else:
                    pass

            if xSteeringData is not None:
                cmd += ' steeringdata '
                cmd += str(hex(xSteeringData)[2:])

            if xBorderRouterLocator is not None:
                cmd += ' locator '
                cmd += str(hex(xBorderRouterLocator))

            if xChannelTlv is not None:
                cmd += ' -x '
                cmd += '000300' + '%04x' % xChannelTlv

            print(cmd)

            return self.__executeCommand(cmd)[-1] == 'Done'

        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('MGMT_COMM_SET() Error: ' + str(e))

    @API
    def setActiveDataset(self, listActiveDataset=[]):
        print('%s call setActiveDataset' % self)

    @API
    def setCommisionerMode(self):
        print('%s call setCommissionerMode' % self)

    @API
    def setPSKc(self, strPSKc):
        print('%s call setPSKc' % self)
        try:
            cmd = 'dataset pskc %s' % strPSKc
            self.hasActiveDatasetToCommit = True
            return self.__executeCommand(cmd)[-1] == 'Done'
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('setPSKc() Error: ' + str(e))

    @API
    def setActiveTimestamp(self, xActiveTimestamp):
        print('%s call setActiveTimestamp' % self)
        try:
            self.activetimestamp = xActiveTimestamp
            cmd = 'dataset activetimestamp %s' % str(xActiveTimestamp)
            self.hasActiveDatasetToCommit = True
            return self.__executeCommand(cmd)[-1] == 'Done'
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('setActiveTimestamp() Error: ' + str(e))

    @API
    def setUdpJoinerPort(self, portNumber):
        """set Joiner UDP Port

        Args:
            portNumber: Joiner UDP Port number

        Returns:
            True: successful to set Joiner UDP Port
            False: fail to set Joiner UDP Port
        """
        print('%s call setUdpJoinerPort' % self)
        cmd = 'joinerport %d' % portNumber
        print(cmd)
        return self.__executeCommand(cmd)[-1] == 'Done'

    @API
    def commissionerUnregister(self):
        """stop commissioner

        Returns:
            True: successful to stop commissioner
            False: fail to stop commissioner
        """
        print('%s call commissionerUnregister' % self)
        cmd = 'commissioner stop'
        print(cmd)
        return self.__executeCommand(cmd)[-1] == 'Done'

    @API
    def sendBeacons(self, sAddr, xCommissionerSessionId, listChannelMask, xPanId):
        print('%s call sendBeacons' % self)
        self.__sendCommand('scan', expectEcho=False)
        return True

    @API
    def updateRouterStatus(self):
        """force update to router as if there is child id request"""
        self._update_router_status = True

    @API
    def __updateRouterStatus(self):
        print('%s call updateRouterStatus' % self)
        cmd = 'state'
        while True:
            state = self.__executeCommand(cmd)[0]
            if state == 'detached':
                continue
            elif state == 'child':
                break
            else:
                return False

        cmd = 'state router'
        return self.__executeCommand(cmd)[-1] == 'Done'

    @API
    def setRouterThresholdValues(self, upgradeThreshold, downgradeThreshold):
        print('%s call setRouterThresholdValues' % self)
        self.__setRouterUpgradeThreshold(upgradeThreshold)
        self.__setRouterDowngradeThreshold(downgradeThreshold)

    @API
    def setMinDelayTimer(self, iSeconds):
        print('%s call setMinDelayTimer' % self)
        cmd = 'delaytimermin %s' % iSeconds
        print(cmd)
        return self.__executeCommand(cmd)[-1] == 'Done'

    @API
    def ValidateDeviceFirmware(self):
        print('%s call ValidateDeviceFirmware' % self)
        return 'OPENTHREAD' in self.UIStatusMsg

    @API
    def setBbrDataset(self, SeqNumInc=False, SeqNum=None, MlrTimeout=None, ReRegDelay=None):
        """ set BBR Dataset

        Args:
            SeqNumInc:  Increase `SeqNum` by 1 if True.
            SeqNum:     Set `SeqNum` to a given value if not None.
            MlrTimeout: Set `MlrTimeout` to a given value.
            ReRegDelay: Set `ReRegDelay` to a given value.

            MUST NOT set SeqNumInc to True and SeqNum to non-None value at the same time.

        Returns:
            True: successful to set BBR Dataset
            False: fail to set BBR Dataset
        """
        assert not (SeqNumInc and SeqNum is not None), "Must not specify both SeqNumInc and SeqNum"
        if SeqNumInc:
            SeqNum = (self.bbrSeqNum + 1) % 256

        return self.__configBbrDataset(SeqNum=SeqNum, MlrTimeout=MlrTimeout, ReRegDelay=ReRegDelay)

    def __configBbrDataset(self, SeqNum=None, MlrTimeout=None, ReRegDelay=None):
        if MlrTimeout is not None and ReRegDelay is None:
            ReRegDelay = self.bbrReRegDelay

        cmd = 'bbr config'
        if SeqNum is not None:
            cmd += ' seqno %d' % SeqNum
        if ReRegDelay is not None:
            cmd += ' delay %d' % ReRegDelay
        if MlrTimeout is not None:
            cmd += ' timeout %d' % MlrTimeout
        ret = self.__executeCommand(cmd)[-1] == 'Done'

        if SeqNum is not None:
            self.bbrSeqNum = SeqNum

        if MlrTimeout is not None:
            self.bbrMlrTimeout = MlrTimeout

        if ReRegDelay is not None:
            self.bbrReRegDelay = ReRegDelay

        self.__executeCommand('netdata register')

        return ret

    # Low power THCI
    @API
    def setCSLtout(self, tout=30):
        self.log('call setCSLtout')
        cmd = 'csl timeout %u' % tout
        print(cmd)
        return self.__executeCommand(cmd)[-1] == 'Done'

    @API
    def setCSLchannel(self, ch=11):
        self.log('call setCSLchannel')
        cmd = 'csl channel %u' % ch
        print(cmd)
        return self.__executeCommand(cmd)[-1] == 'Done'

    @API
    def setCSLperiod(self, period=500):
        """set Csl Period
        Args:
            period: csl period in ms

        note: OT command 'csl period' accepts parameter in unit of 10 symbols,
        period is converted from unit ms to ten symbols (160us per 10 symbols).

        """
        self.log('call setCSLperiod')
        cmd = 'csl period %u' % (period * 6.25)
        print(cmd)
        return self.__executeCommand(cmd)[-1] == 'Done'

    @staticmethod
    def getForwardSeriesFlagsFromHexStr(flags):
        hexFlags = int(flags, 16)
        strFlags = ''
        if hexFlags == 0:
            strFlags = 'X'
        else:
            if hexFlags & 0x1 != 0:
                strFlags += 'l'
            if hexFlags & 0x2 != 0:
                strFlags += 'd'
            if hexFlags & 0x4 != 0:
                strFlags += 'r'
            if hexFlags & 0x8 != 0:
                strFlags += 'a'

        return strFlags

    @staticmethod
    def mapMetricsHexToChar(metrics):
        metricsFlagMap = {
            0x40: 'p',
            0x09: 'q',
            0x0a: 'm',
            0x0b: 'r',
        }
        return metricsFlagMap.get(metrics, '?')

    @staticmethod
    def getMetricsFlagsFromHexStr(metrics):
        if metrics.startswith('0x'):
            metrics = metrics[2:]
        hexMetricsArray = bytearray.fromhex(metrics)

        strMetrics = ''
        for metric in hexMetricsArray:
            strMetrics += OpenThreadTHCI.mapMetricsHexToChar(metric)

        return strMetrics

    @API
    def LinkMetricsSingleReq(self, dst_addr, metrics):
        self.log('call LinkMetricsSingleReq')
        cmd = 'linkmetrics query %s single %s' % (dst_addr, self.getMetricsFlagsFromHexStr(metrics))
        print(cmd)
        return self.__executeCommand(cmd)[-1] == 'Done'

    @API
    def LinkMetricsMgmtReq(self, dst_addr, type_, flags, metrics, series_id):
        self.log('call LinkMetricsMgmtReq')
        cmd = 'linkmetrics mgmt %s ' % dst_addr
        if type_ == 'FWD':
            cmd += 'forward %d %s' % (series_id, self.getForwardSeriesFlagsFromHexStr(flags))
            if flags != 0:
                cmd += ' %s' % (self.getMetricsFlagsFromHexStr(metrics))
        elif type_ == 'ENH':
            cmd += 'enhanced-ack'
            if flags != 0:
                cmd += ' register'
                metricsFlags = self.getMetricsFlagsFromHexStr(metrics)
                if '?' in metricsFlags:
                    cmd += ' %s r' % metricsFlags.replace('?', '')
                else:
                    cmd += ' %s' % metricsFlags
            else:
                cmd += ' clear'
        print(cmd)
        return self.__executeCommand(cmd)[-1] == 'Done'

    @API
    def LinkMetricsGetReport(self, dst_addr, series_id):
        self.log('call LinkMetricsGetReport')
        cmd = 'linkmetrics query %s forward %d' % (dst_addr, series_id)
        print(cmd)
        return self.__executeCommand(cmd)[-1] == 'Done'

    # TODO: Series Id is not in this API.
    @API
    def LinkMetricsSendProbe(self, dst_addr, ack=True, size=0):
        self.log('call LinkMetricsSendProbe')
        cmd = 'linkmetrics probe %s %d' % (dst_addr, size)
        print(cmd)
        return self.__executeCommand(cmd)[-1] == 'Done'

    @API
    def setTxPower(self, level):
        self.log('call setTxPower')
        cmd = 'txpower '
        if level == 'HIGH':
            cmd += '127'
        elif level == 'MEDIUM':
            cmd += '0'
        elif level == 'LOW':
            cmd += '-128'
        else:
            print('wrong Tx Power level')
        print(cmd)
        return self.__executeCommand(cmd)[-1] == 'Done'

    @API
    def sendUdp(self, destination, port, payload='hello'):
        self.log('call sendUdp')
        assert payload is not None, 'payload should not be none'
        cmd = 'udp send %s %d %s' % (destination, port, payload)
        print(cmd)
        return self.__executeCommand(cmd)[-1] == 'Done'

    @API
    def send_udp(self, interface, destination, port, payload='12ABcd'):
        ''' payload hexstring
        '''
        self.log('call send_udp')
        assert payload is not None, 'payload should not be none'
        assert interface == 0, "non-BR must send UDP to Thread interface"
        self.__udpOpen()
        cmd = 'udp send %s %s -x %s' % (destination, port, payload)
        print(cmd)
        return self.__executeCommand(cmd)[-1] == 'Done'

    def __udpOpen(self):
        if not self.__isUdpOpened:
            cmd = 'udp open'
            self.__executeCommand(cmd)
            self.__isUdpOpened = True

    @API
    def sendMACcmd(self, enh=False):
        self.log('call sendMACcmd')
        cmd = 'mac send datarequest'
        print(cmd)
        return self.__executeCommand(cmd)[-1] == 'Done'

    @API
    def sendMACdata(self, enh=False):
        self.log('call sendMACdata')
        cmd = 'mac send emptydata'
        print(cmd)
        return self.__executeCommand(cmd)[-1] == 'Done'

    @API
    def setCSLsuspension(self, suspend):
        self.log('call setCSLsuspension')
        if suspend:
            self.__setPollPeriod(1000000)
        else:
            self.__setPollPeriod(self.__sedPollPeriod)

    @API
    def set_max_addrs_per_child(self, num):
        cmd = 'childipmax %d' % int(num)
        print(cmd)
        self.__executeCommand(cmd)

    @API
    def config_next_dua_status_rsp(self, mliid, status_code):
        if status_code >= 400:
            # map status_code to correct COAP response code
            a, b = divmod(status_code, 100)
            status_code = ((a & 0x7) << 5) + (b & 0x1f)

        cmd = 'bbr mgmt dua %d' % status_code

        if mliid is not None:
            mliid = mliid.replace(':', '')
            cmd += ' %s' % mliid

        self.__executeCommand(cmd)

    @API
    def getDUA(self):
        dua = self.getGUA('fd00:7d03')
        return dua

    def __addDefaultDomainPrefix(self):
        self.configBorderRouter(P_dp=1, P_slaac_preferred=1, P_stable=1, P_on_mesh=1, P_default=1)

    def __setDUA(self, sDua):
        """specify the DUA before Thread Starts."""
        if isinstance(sDua, str):
            sDua = sDua.decode('utf8')
        iid = ipaddress.IPv6Address(sDua).packed[-8:]
        cmd = 'dua iid %s' % ''.join('%02x' % ord(b) for b in iid)
        return self.__executeCommand(cmd)[-1] == 'Done'

    def __getMlIid(self):
        """get the Mesh Local IID."""
        print('%s call __getMlIid' % self.port)
        # getULA64() would return the full string representation
        mleid = ModuleHelper.GetFullIpv6Address(self.getULA64()).lower()
        mliid = mleid[-19:].replace(':', '')
        print('mliid: %s' % mliid)
        return mliid

    def __setMlIid(self, sMlIid):
        """Set the Mesh Local IID before Thread Starts."""
        assert ':' not in sMlIid
        cmd = 'mliid %s' % sMlIid
        self.__executeCommand(cmd)

    @API
    def registerDUA(self, sAddr=''):
        self.__setDUA(sAddr)

    @API
    def config_next_mlr_status_rsp(self, status_code):
        cmd = 'bbr mgmt mlr response %d' % status_code
        return self.__executeCommand(cmd)[-1] == 'Done'

    @API
    def setMLRtimeout(self, iMsecs):
        """Setup BBR MLR Timeout to `iMsecs` seconds."""
        self.__configBbrDataset(MlrTimeout=iMsecs)

    @API
    def stopListeningToAddr(self, sAddr):
        print('%s call stopListeningToAddr' % self.port)

        # convert to list for single element, for possible extension
        # requirements.
        if not isinstance(sAddr, list):
            sAddr = [sAddr]

        for addr in sAddr:
            cmd = 'ipmaddr del ' + addr
            try:
                self.__executeCommand(cmd)
            except CommandError as ex:
                if ex.code == OT_ERROR_ALREADY:
                    pass
                else:
                    raise

        return True

    @API
    def registerMulticast(self, sAddr='ff04::1234:777a:1', timeout=MLR_TIMEOUT_MIN):
        """subscribe to the given ipv6 address (sAddr) in interface and send MLR.req OTA

        Args:
            sAddr   : str : Multicast address to be subscribed and notified OTA.
        """
        # convert to list for single element, for possible extension
        # requirements.
        if not isinstance(sAddr, list):
            sAddr = [sAddr]

        if self.externalCommissioner is not None:
            self.externalCommissioner.MLR(sAddr, timeout)
            return True

        # subscribe address one by one
        for addr in sAddr:
            cmd = 'ipmaddr add ' + str(addr)

            try:
                self.__executeCommand(cmd)
            except CommandError as ex:
                if ex.code == OT_ERROR_ALREADY:
                    pass
                else:
                    raise

    def deregisterMulticast(self, sAddr):
        """
        Unsubscribe to a given IPv6 address.
        Only used by External Commissioner.

        Args:
            sAddr   : str : Multicast address to be unsubscribed.
        """
        if not isinstance(sAddr, list):
            sAddr = [sAddr]
        self.externalCommissioner.MLR(sAddr, 0)
        return True

    @API
    def migrateNetwork(self, channel=None, net_name=None):
        """migrate to another Thread Partition 'net_name' (could be None)
            on specified 'channel'. Make sure same Mesh Local IID and DUA
            after migration for DUA-TC-06/06b (DEV-1923)
        """
        try:
            if channel is None:
                raise Exception('channel None')

            if channel not in range(11, 27):
                raise Exception('channel %d not in [11, 26] Invalid' % channel)

            print('new partition %s on channel %d' % (net_name, channel))

            mliid = self.__getMlIid()
            dua = self.getDUA()
            self.reset()
            deviceRole = self.deviceRole
            self.setDefaultValues()
            self.setChannel(channel)
            if net_name is not None:
                self.setNetworkName(net_name)
            self.__setMlIid(mliid)
            self.__setDUA(dua)
            return self.joinNetwork(deviceRole)

        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('migrateNetwork() Error: ' + str(e))

    @API
    def setParentPrio(self, prio):
        cmd = 'parentpriority %u' % prio
        print(cmd)
        return self.__executeCommand(cmd)[-1] == 'Done'

    @API
    def role_transition(self, role):
        assert TESTHARNESS_VERSION == TESTHARNESS_1_2
        try:
            cmd = 'mode %s' % OpenThreadTHCI._ROLE_MODE_DICT[role]
            return self.__executeCommand(cmd)[-1] == 'Done'
        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('role_transition() Error: ' + str(e))

    @API
    def setLeaderWeight(self, iWeight=72):
        self.__executeCommand('leaderweight %d' % iWeight)

    def __discoverDeviceCapability(self):
        """Discover device capability according to version"""
        self.DeviceCapability = DevCapb.NotSpecified

        if self.IsBorderRouter:
            self.DeviceCapability = DevCapb.C_BBR | DevCapb.C_Host | DevCapb.C_Comm
        else:
            # Get Thread stack version to distinguish device capability.
            thver = self.__executeCommand('thread version')[0]

            if thver in ['1.2', '3']:
                self.DeviceCapability = (DevCapb.C_FFD | DevCapb.C_RFD | DevCapb.L_AIO)
            elif thver in ['1.1', '2']:
                self.DeviceCapability = DevCapb.V1_1
            else:
                assert False, thver

    @staticmethod
    def __lstrip0x(s):
        """strip 0x at the beginning of a hex string if it exists

        Args:
            s: hex string

        Returns:
            hex string with leading 0x stripped
        """
        if s.startswith('0x'):
            s = s[2:]

        return s


class OpenThread(OpenThreadTHCI, IThci):

    # Used for reference firmware version control for Test Harness.
    # This variable will be updated to match the OpenThread reference firmware
    # officially released.

    def _connect(self):
        print('My port is %s' % self)
        self.__lines = []
        if self.port.startswith('COM'):
            self.__handle = serial.Serial(self.port, 115200, timeout=0)
            self.sleep(1)
            self.__handle.write('\r\n')
            self.sleep(0.1)
            self._is_net = False
        elif ':' in self.port:
            host, port = self.port.split(':')
            self.__handle = socket.create_connection((host, port))
            self.__handle.setblocking(0)
            self._is_net = True
        else:
            raise Exception('Unknown port schema')

    def _disconnect(self):
        if self.__handle:
            self.__handle.close()
            self.__handle = None

    def _deviceBeforeReset(self):
        pass

    def _deviceAfterReset(self):
        pass

    def __socRead(self, size=512):
        if self._is_net:
            return self.__handle.recv(size)
        else:
            return self.__handle.read(size)

    def __socWrite(self, data):
        if self._is_net:
            self.__handle.sendall(data)
        else:
            self.__handle.write(data)

    def _cliReadLine(self):
        if len(self.__lines) > 1:
            return self.__lines.pop(0)

        tail = ''
        if len(self.__lines) != 0:
            tail = self.__lines.pop()

        try:
            tail += self.__socRead()
        except socket.error:
            logging.exception('%s: No new data', self)
            self.sleep(0.1)

        self.__lines += LINESEPX.split(tail)
        if len(self.__lines) > 1:
            return self.__lines.pop(0)

    def _cliWriteLine(self, line):
        self.__socWrite(line + '\r\n')

    def _onCommissionStart(self):
        pass

    def _onCommissionStop(self):
        pass
