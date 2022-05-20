#!/usr/bin/env python
#
# Copyright (c) 2019, The OpenThread Authors.
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
''''OpenThread Sniffer API implementation'''

import os
import subprocess
from GRLLibs.UtilityModules.ModuleHelper import ModuleHelper
from ISniffer import ISniffer


class OT_Sniffer(ISniffer):

    def __init__(self, **kwargs):
        try:
            self.channel = kwargs.get('channel', 11)
            self.port = kwargs.get('addressofDevice')
            self.subprocess = None
            self.is_active = False

        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('OT_Sniffer: [intialize] --> ' + str(e))

    def discoverSniffer(self):
        sniffers = []

        p_discover = subprocess.Popen('extcap_ot.bat --extcap-interfaces', stdout=subprocess.PIPE, shell=True)
        for line in p_discover.stdout.readlines():
            if line.startswith('interface'):
                try:
                    # e.g. interface {value=COM10:460800}{display=OpenThread Sniffer COM10}
                    interface_port = line[line.index('value=') + 6:line.index('}{display')]
                    sniffers.append(OT_Sniffer(addressofDevice=interface_port, channel=ModuleHelper.Default_Channel))
                except Exception as e:
                    ModuleHelper.WriteIntoDebugLogger('OT_Sniffer: [discoverSniffer] --> Error: ' + str(e))

        p_discover.wait()
        return sniffers

    def startSniffer(self, channelToCapture, captureFileLocation):
        """
        Method for starting the sniffer capture on given channel and this should create wireshark 'pcapng' file at the
        given location. Capture should happen in background so that method call will be non-blocking and asynchronous.
        @param channelToCapture : int : channel number to start the capture
        @param captureFileLocation : string : Full path with the filename with extension is passed.
        """
        try:
            # start sniffer
            self.setChannel(channelToCapture)
            p_where = subprocess.Popen(
                'py -3 -c "import sys; print(sys.executable)"',
                stdout=subprocess.PIPE,
                shell=True,
            )
            # python_exe: e.g. C:\Python37\python.exe
            python_exe = p_where.stdout.readline().strip()

            if python_exe.endswith(".exe"):
                # sniffer_py: e.g. C:\Python37\Scripts\sniffer.py
                sniffer_py = str(os.path.dirname(python_exe)) + '\\Scripts\\sniffer.py'

                cmd = [
                    python_exe,
                    sniffer_py,
                    '-c',
                    str(self.channel),
                    '-u',
                    str(self.port.split(':')[0]),
                    '-b',
                    str(self.port.split(':')[1]),
                    '--crc',
                    '-o',
                    captureFileLocation,
                ]
                self.is_active = True
                ModuleHelper.WriteIntoDebugLogger('OT_Sniffer: [cmd] --> %s' % str(cmd))
                self.subprocess = subprocess.Popen(cmd)

        except Exception as e:
            ModuleHelper.WriteIntoDebugLogger('OT_Sniffer: [startSniffer] --> Error: ' + str(e))

    def stopSniffer(self):
        """
        Method for ending the sniffer capture.
        Should stop background capturing, No further file I/O in capture file.
        """
        if self.is_active:
            self.is_active = False
            if self.subprocess:
                self.subprocess.terminate()
                self.subprocess.wait()

    def setChannel(self, channelToCapture):
        """
        Method for changing sniffer capture
        @param channelToCapture : int :
            channel number is passed to change the channel which is set during the constructor call.
        """
        self.channel = channelToCapture

    def getChannel(self):
        """
        Method to query sniffer for the channel it is sniffing on
        @return : int : current capture channel of this sniffer instance.
        """
        return self.channel

    def validateFirmwareVersion(self, addressofDevice):
        """
        Method to query sniffer firmware version details.
        Shall be used while discoverSniffer() to validate the sniffer firmware.
        @param addressofDevice : string :
            serial com port or IP address,shall be None if need to verify own fireware version.
        @return : bool : True if expected firmware is found , False if not
        """
        return True

    def isSnifferCapturing(self):
        """
        method that will return true when sniffer device is capturing
        @return : bool
        """
        return self.is_active

    def getSnifferAddress(self):
        """
        Method to retrun the current sniffer's COM/IP address
        @return : string
        """
        return self.port

    def globalReset(self):
        """Method to reset all the global and class varibaled"""
        pass
