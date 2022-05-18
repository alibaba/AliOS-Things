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

import ConfigParser
import logging
import os
import subprocess
import time

from autothreadharness import settings

logger = logging.getLogger(__name__)

HARNESS_SVN_VERSION_R44 = 1471
"""int: this is the first published release that miniweb was removed from Harness"""


def _try_kill(proc):
    logger.info('Try kill process')
    times = 1

    while proc.poll() is None:
        proc.kill()

        time.sleep(5)

        if proc.poll() is not None:
            logger.info('Process has been killed')
            break

        logger.info('Trial %d failed', times)
        times += 1

        if times > 3:
            raise SystemExit()


class HarnessController(object):
    """Harness service control

    This controls harness service, including the harness back-end and front-end.
    """

    harness = None
    """harness back-end"""

    miniweb = None
    """harness front-end"""

    def __init__(self, result_dir=None):
        self.result_dir = result_dir
        self.harness_file = ''

        harness_info = ConfigParser.ConfigParser()
        harness_info.read('%s\\info.ini' % settings.HARNESS_HOME)
        self.version = harness_info.getint('Thread_Harness_Info', 'SVN')

    def start(self):
        logger.info('Starting harness service')
        if self.harness:
            logger.warning('Harness already started')
        else:
            env = dict(
                os.environ,
                PYTHONPATH='%s\\Thread_Harness;%s\\ThirdParty\\hsdk-python\\src' %
                (settings.HARNESS_HOME, settings.HARNESS_HOME),
            )

            self.harness_file = '%s\\harness-%s.log' % (self.result_dir, time.strftime('%Y%m%d%H%M%S'))
            with open(self.harness_file, 'w') as harness_out:
                self.harness = subprocess.Popen(
                    [
                        settings.HARNESS_HOME + '\\Python27\\python.exe',
                        settings.HARNESS_HOME + '\\Thread_Harness\\Run.py',
                    ],
                    cwd=settings.HARNESS_HOME,
                    stdout=harness_out,
                    stderr=harness_out,
                    env=env,
                )
            time.sleep(2)

        if self.version >= HARNESS_SVN_VERSION_R44:
            return

        if self.miniweb:
            logger.warning('Miniweb already started')
        else:
            with open('%s\\miniweb-%s.log' % (self.result_dir, time.strftime('%Y%m%d%H%M%S')), 'w') as miniweb_out:
                self.miniweb = subprocess.Popen(
                    [settings.HARNESS_HOME + '\\MiniWeb\\miniweb.exe'],
                    stdout=miniweb_out,
                    stderr=miniweb_out,
                    cwd=settings.HARNESS_HOME + '\\MiniWeb',
                )

    def stop(self):
        logger.info('Stopping harness service')

        if self.harness:
            _try_kill(self.harness)
            self.harness = None
        else:
            logger.warning('Harness not started yet')

        if self.version >= HARNESS_SVN_VERSION_R44:
            return

        if self.miniweb:
            _try_kill(self.miniweb)
            self.miniweb = None
        else:
            logger.warning('Miniweb not started yet')

    def tail(self):
        with open(self.harness_file) as harness_out:
            harness_out.seek(-100, 2)
            return ''.join(harness_out.readlines())

    def __del__(self):
        self.stop()
