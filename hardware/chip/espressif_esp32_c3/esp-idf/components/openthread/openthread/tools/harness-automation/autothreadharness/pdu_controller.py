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
import os
import re
import telnetlib
import time

try:
    # python 2
    from urllib2 import HTTPPasswordMgrWithDefaultRealm, HTTPBasicAuthHandler, build_opener
except ImportError:
    # python 3
    from urllib.request import HTTPPasswordMgrWithDefaultRealm, HTTPBasicAuthHandler, build_opener

logger = logging.getLogger(__name__)

try:
    from pysnmp.hlapi import SnmpEngine, CommunityData, UdpTransportTarget, ContextData, getCmd, setCmd, ObjectType, ObjectIdentity, Integer32
except ImportError:
    logger.warning('PySNMP module is not installed. Install if EATON_PDU_CONTROLLER is used')


class PduController(object):

    def open(self, **params):
        """Open PDU controller connection"""
        raise NotImplementedError

    def reboot(self, **params):
        """Reboot an outlet or a board passed as params"""
        raise NotImplementedError

    def close(self):
        """Close PDU controller connection"""
        raise NotImplementedError


class DummyPduController(PduController):
    """Dummy implementation which only says that PDU controller is not connected"""

    def open(self, **params):
        pass

    def reboot(self, **params):
        logger.info('No PDU controller connected.')

    def close(self):
        pass


class ApcPduController(PduController):

    def __init__(self):
        self.tn = None

    def __del__(self):
        self.close()

    def _init(self):
        """Initialize the telnet connection
        """
        self.tn = telnetlib.Telnet(self.ip, self.port)
        self.tn.read_until('User Name')
        self.tn.write('apc\r\n')
        self.tn.read_until('Password')
        self.tn.write('apc\r\n')
        self.until_done()

    def open(self, **params):
        """Open telnet connection
        
        Args:
            params (dict), must contain two parameters "ip" - ip address or hostname and "port" - port number
            
        Example:
            params = {'port': 23, 'ip': 'localhost'}
        """
        logger.info('opening telnet')
        self.port = params['port']
        self.ip = params['ip']
        self.tn = None
        self._init()

    def close(self):
        """Close telnet connection"""
        logger.info('closing telnet')
        if self.tn:
            self.tn.close()

    def until_done(self):
        """Wait until the prompt encountered
        """
        self.until(r'^>')

    def until(self, regex):
        """Wait until the regex encountered
        """
        logger.debug('waiting for %s', regex)
        r = re.compile(regex, re.M)
        self.tn.expect([r])

    def reboot(self, **params):
        """Reboot outlet
        
        Args:
            params (dict), must contain parameter "outlet" - outlet number
            
        Example:
            params = {'outlet': 1}
        """
        outlet = params['outlet']

        # main menu
        self.tn.write('\x1b\r\n')
        self.until_done()
        # Device Manager
        self.tn.write('1\r\n')
        self.until_done()
        # Outlet Management
        self.tn.write('2\r\n')
        self.until_done()
        # Outlet Control
        self.tn.write('1\r\n')
        self.until_done()
        # Select outlet
        self.tn.write('%d\r\n' % outlet)
        self.until_done()
        # Control
        self.tn.write('1\r\n')
        self.until_done()
        # off
        self.tn.write('2\r\n')
        self.until('to cancel')
        self.tn.write('YES\r\n')
        self.until('to continue')
        self.tn.write('\r\n')
        self.until_done()

        time.sleep(5)
        # on
        self.tn.write('1\r\n')
        self.until('to cancel')
        self.tn.write('YES\r\n')
        self.until('to continue')
        self.tn.write('\r\n')
        self.until_done()


class NordicBoardPduController(PduController):

    def open(self, **params):
        pass

    def _pin_reset(self, serial_number):
        os.system('nrfjprog -f NRF52 --snr {} -p'.format(serial_number))

    def reboot(self, **params):
        boards_serial_numbers = params['boards_serial_numbers']

        for serial_number in boards_serial_numbers:
            logger.info('Resetting board with the serial number: %s', serial_number)
            self._pin_reset(serial_number)

    def close(self):
        pass


class EatonPduController(PduController):

    outlet_oid_cmd_get_state_base = '1.3.6.1.4.1.534.6.6.7.6.6.1.2.0.'
    outlet_oid_cmd_set_on_base = '1.3.6.1.4.1.534.6.6.7.6.6.1.4.0.'
    outlet_oid_cmd_set_off_base = '1.3.6.1.4.1.534.6.6.7.6.6.1.3.0.'
    outlet_oid_cmd_reboot_base = '1.3.6.1.4.1.534.6.6.7.6.6.1.5.0.'
    outlet_oid_cmd_set_reboot_delay_seconds_base = '1.3.6.1.4.1.534.6.6.7.6.6.1.8.0.'

    PDU_COMMAND_TIMEOUT = 5

    def open(self, **params):
        missing_fields = ['ip', 'port']
        missing_fields = [field for field in missing_fields if field not in params.keys()]
        if missing_fields:
            raise KeyError('Missing keys in PDU params: {}'.format(missing_fields))
        self.params = params
        self.type = 'pdu'
        self.ip = self.params['ip']
        self.snmp_agent_port = int(self.params['port'])
        self._community = 'public'
        self._snmp_engine = SnmpEngine()
        self._community_data = CommunityData(self._community, mpModel=0)
        self._udp_transport_target = UdpTransportTarget((self.ip, self.snmp_agent_port))
        self._context = ContextData()

    def _outlet_oid_get(self, param, socket):
        """
        Translates command to the OID number representing a command for the specific power socket.

        Args:
            param (str), command string
            socket (int), socket index

        Return:
            full OID identifying the SNMP object (str)
        """
        parameters = {
            'get_state': self.outlet_oid_cmd_get_state_base,
            'set_on': self.outlet_oid_cmd_set_on_base,
            'set_off': self.outlet_oid_cmd_set_off_base,
            'set_reboot_delay': self.outlet_oid_cmd_set_reboot_delay_seconds_base,
            'reboot': self.outlet_oid_cmd_reboot_base
        }

        return parameters[param.lower()] + str(socket)

    # Performs set command to specific OID with a given value by sending a SNMP Set message.
    def _oid_set(self, oid, value):
        """
        Performs set command to specific OID with a given value by sending a SNMP Set message.

        Args:
            oid (str): Full OID identifying the object to be read.
            value (int): Value to be written to the OID as Integer32.
        """
        errorIndication, errorStatus, errorIndex, varBinds = next(
            setCmd(self._snmp_engine, self._community_data, self._udp_transport_target, self._context,
                   ObjectType(ObjectIdentity(oid), Integer32(value))))

        if errorIndication:
            msg = 'Found PDU errorIndication: {}'.format(errorIndication)
            logger.exception(msg)
            raise RuntimeError(msg)
        elif errorStatus:
            msg = 'Found PDU errorStatus: {}'.format(errorStatus)
            logger.exception(msg)
            raise RuntimeError(msg)

    def _oid_get(self, oid):
        """
        Performs SNMP get command and returns OID value by sending a SNMP Get message.

        Args:
            oid (str): Full OID identifying the object to be read.

        Return:
            OID value (int)
        """
        errorIndication, errorStatus, errorIndex, varBinds = next(
            getCmd(self._snmp_engine, self._community_data, self._udp_transport_target, self._context,
                   ObjectType(ObjectIdentity(oid))))

        if errorIndication:
            msg = 'Found PDU errorIndication: {}'.format(errorIndication)
            logger.exception(msg)
            raise RuntimeError(msg)
        elif errorStatus:
            msg = 'Found PDU errorStatus: {}'.format(errorStatus)
            logger.exception(msg)
            raise RuntimeError(msg)

        return int(str(varBinds[-1]).partition('= ')[-1])

    def _outlet_value_set(self, cmd, socket, value=1):
        """
        Sets outlet parameter value.

        Args:
            cmd (str): OID base
            socket (int): socket index (last OID number)
            value (int): value to be set
        """
        oid = self._outlet_oid_get(cmd, socket)

        # Values other than 1 does not make sense with commands other than "set_reboot_delay".
        if cmd != 'set_reboot_delay':
            value = 1

        self._oid_set(oid, value)

    def _outlet_value_get(self, cmd, socket):
        """
        Read outlet parameter value.

        Args:
            cmd (str): OID base
            socket (int): socket index (last OID number)

        Return:
            parameter value (int)
        """
        oid = self._outlet_oid_get(cmd, socket)

        return self._oid_get(oid)

    def validate_state(self, socket, state):
        return (self._outlet_value_get('get_state', socket) == state)

    def turn_off(self, sockets):
        """
        Turns the specified socket off.

        Args:
            sockets (list(int)): sockets to be turned off
        """
        logger.info('Executing turn OFF for: {}'.format(sockets))

        for socket in sockets:
            self._outlet_value_set('set_off', socket)
            time.sleep(2)

            timeout = time.time() + self.PDU_COMMAND_TIMEOUT
            while ((time.time() < timeout) and not self.validate_state(socket, 0)):
                time.sleep(0.1)

            if self.validate_state(socket, 0):
                logger.debug('Turned OFF socket {} at {}'.format(socket, self.ip))
            else:
                logger.error('Failed to turn OFF socket {} at {}'.format(socket, self.ip))

    def turn_on(self, sockets):
        """
        Turns the specified socket on.

        Args:
            sockets (list(int)): sockets to be turned on
        """

        logger.info('Executing turn ON for: {}'.format(sockets))

        for socket in sockets:
            self._outlet_value_set('set_on', socket)
            time.sleep(2)

            timeout = time.time() + self.PDU_COMMAND_TIMEOUT
            while ((time.time() < timeout) and not self.validate_state(socket, 1)):
                time.sleep(0.1)

            if self.validate_state(socket, 1):
                logger.debug('Turned ON socket {} at {}'.format(socket, self.ip))
            else:
                logger.error('Failed to turn ON socket {} at {}'.format(socket, self.ip))

    def close(self):
        self._community = None
        self._snmp_engine = None
        self._community_data = None
        self._udp_transport_target = None
        self._context = None

    def reboot(self, **params):
        """
        Reboots the sockets specified in the constructor with off and on delays.

        Args:
            sockets (list(int)): sockets to reboot
        """

        logger.info('Executing power cycle for: {}'.format(params['sockets']))
        self.turn_off(params['sockets'])
        time.sleep(10)
        self.turn_on(params['sockets'])
        time.sleep(5)


class IpPowerSocketPduController(PduController):

    def open(self, **params):
        self._base_url = 'http://{}/outs.cgi?out'.format(params['ip'])
        password_manager = HTTPPasswordMgrWithDefaultRealm()
        password_manager.add_password(None, self._base_url, params['user'], params['pass'])
        authentication_handler = HTTPBasicAuthHandler(password_manager)
        self._opener = build_opener(authentication_handler)

    def reboot(self, **params):
        logger.info('Executing power cycle')
        for socket in params['sockets']:
            self._turn_off(socket)
            time.sleep(2)
            self._turn_on(socket)
            time.sleep(2)

    def _change_state(self, socket, state):
        self._opener.open('{}{}={}'.format(self._base_url, socket, state))

    def _turn_off(self, socket):
        self._change_state(socket, 1)

    def _turn_on(self, socket):
        self._change_state(socket, 0)

    def close(self):
        self._base_url = None
        self._opener = None


class ManualPduController(PduController):

    def open(self, **kwargs):
        pass

    def reboot(self, **kwargs):
        input('Reset all devices and press enter to continue..')

    def close(self):
        pass
