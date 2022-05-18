#!/usr/bin/env python
#
# Copyright (c) 2017, The OpenThread Authors.
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

import sys
import abc
import logging
import serial
import time

ABC = abc.ABC if sys.version_info >= (3, 4) else abc.ABCMeta('ABC', (), {})
logger = logging.getLogger(__name__)


class RfShieldController(ABC):

    @abc.abstractmethod
    def shield(self):
        pass

    @abc.abstractmethod
    def unshield(self):
        pass

    @abc.abstractmethod
    def __enter__(self):
        pass

    @abc.abstractmethod
    def __exit__(self, *args, **kwargs):
        pass


class RfSwitchController(RfShieldController):

    def __init__(self, channel, port):
        self._channel = channel
        self._port = port
        self._conn = None

    def shield(self):
        self._display_string('CLOSE {}'.format(self._channel))
        self._write('CLOSE (@{})'.format(self._channel))

    def unshield(self):
        self._display_string('OPEN {}'.format(self._channel))
        self._write('OPEN (@{})'.format(self._channel))

    def _write(self, data):
        return self._conn.write('{}\r\n'.format(data))

    def _display_string(self, string):
        self._write('DIAGNOSTIC:DISPLAY "{}"'.format(string))

    def __enter__(self):
        self._conn = serial.Serial(self._port, 9600)
        if not self._conn.isOpen():
            self._conn.open()
        self._write('')
        time.sleep(1)
        return self

    def __exit__(self, *args, **kwargs):
        if self._conn:
            self._conn.close()
            self._conn = None


CONTROLLERS = {'RF_SWITCH': RfSwitchController}


def get_rf_shield_controller(shield_type, params):
    if shield_type in CONTROLLERS:
        return CONTROLLERS[shield_type](**params)
    logger.exception('Unknown RF shield controller type: {}'.format(shield_type))
