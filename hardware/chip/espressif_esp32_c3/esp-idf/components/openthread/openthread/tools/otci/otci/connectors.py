#!/usr/bin/env python3
#
#  Copyright (c) 2020, The OpenThread Authors.
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#  3. Neither the name of the copyright holder nor the
#     names of its contributors may be used to endorse or promote products
#     derived from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
#  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.
#
import logging
import subprocess
import time
from abc import abstractmethod


class OtCliHandler:
    """This abstract class defines interfaces for a OT CLI Handler."""

    @abstractmethod
    def readline(self) -> str:
        """Method readline should return the next line read from OT CLI."""
        pass

    @abstractmethod
    def writeline(self, s: str) -> None:
        """Method writeline should write a line to the OT CLI.

        It should block until all characters are written to OT CLI.
        """
        pass

    @abstractmethod
    def wait(self, duration: float) -> None:
        """Method wait should wait for a given duration.

        A normal implementation should just call `time.sleep(duration)`. This is intended for proceeding Virtual Time
        Simulation instances.
        """
        pass

    @abstractmethod
    def close(self) -> None:
        """Method close should close the OT CLI Handler."""
        pass


class Simulator:
    """This abstract class defines interfaces for a Virtual Time Simulator."""

    @abstractmethod
    def go(self, duration: float):
        """Proceed the simulator for a given duration (in seconds)."""
        pass


class OtCliPopen(OtCliHandler):
    """Connector for OT CLI process (a Popen instance)."""

    def __init__(self, proc: subprocess.Popen, nodeid: int, simulator: Simulator):
        self.__otcli_proc = proc
        self.__nodeid = nodeid
        self.__simulator = simulator

    def __repr__(self):
        return 'OTCli<%d>' % self.__nodeid

    def readline(self) -> str:
        return self.__otcli_proc.stdout.readline().rstrip('\r\n')

    def writeline(self, s: str):
        self.__otcli_proc.stdin.write(s + '\n')
        self.__otcli_proc.stdin.flush()

    def wait(self, duration: float):
        if self.__simulator is not None:
            # Virtual time simulation
            self.__simulator.go(duration)
        else:
            # Real time simulation
            time.sleep(duration)

    def close(self):
        self.__otcli_proc.stdin.close()
        self.__otcli_proc.stdout.close()
        self.__otcli_proc.wait()


class OtCliSim(OtCliPopen):
    """Connector for OT CLI Simulation instances."""

    def __init__(self, executable: str, nodeid: int, simulator: Simulator):
        logging.info('%s: executable=%s', self.__class__.__name__, executable)

        proc = subprocess.Popen(args=[executable, str(nodeid)],
                                executable=executable,
                                stdin=subprocess.PIPE,
                                stdout=subprocess.PIPE,
                                encoding='utf-8',
                                bufsize=1024)
        super().__init__(proc, nodeid, simulator)


class OtNcpSim(OtCliHandler):
    """Connector for OT NCP Simulation instances."""

    def __init__(self, executable: str, nodeid: int, simulator: Simulator):
        logging.info('%s: executable=%s', self.__class__.__name__, executable)

        proc = subprocess.Popen(args=f'spinel-cli.py -p "{executable}" -n {nodeid} 2>&1',
                                stdin=subprocess.PIPE,
                                stdout=subprocess.PIPE,
                                encoding='utf-8',
                                bufsize=1024,
                                shell=True)
        super().__init__(proc, nodeid, simulator)


class OtCliSerial(OtCliHandler):
    """Connector for OT CLI SOC devices via Serial."""

    def __init__(self, dev: str, baudrate: int):
        self.__dev = dev
        self.__baudrate = baudrate

        import serial
        self.__serial = serial.Serial(self.__dev, self.__baudrate, timeout=None, exclusive=True)

    def __repr__(self):
        return self.__dev

    def readline(self) -> str:
        line = self.__serial.readline().decode('utf-8').rstrip('\r\n')
        return line

    def writeline(self, s: str):
        self.__serial.write((s + '\n').encode('utf-8'))

    def wait(self, duration: float):
        time.sleep(duration)

    def close(self):
        self.__serial.close()
