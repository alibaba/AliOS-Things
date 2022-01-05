#!/usr/bin/env python
# Copyright 2020 Espressif Systems (Shanghai) Co., Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
import os
from sys import exit
import time
import subprocess
import collections
import sys
try:
    import esptool
except ImportError:  # cheat and use IDF's copy of esptool if available
    idf_path = os.getenv("IDF_PATH")
    if not idf_path or not os.path.exists(idf_path):
        raise
    sys.path.insert(0, os.path.join(idf_path, "components", "esptool_py", "esptool"))
    import esptool


class cmd_interpreter:
    """
    This class is for is the command line interaction with the secure_cert_mfg firmware for manufacturing.
    It executes the specified commands and returns its result.
    It is a stateless, thus does not maintain the current state of the firmware.
    """

    def wait_for_init(self, port):
        print("Wait for init")
        port.timeout = 1.5
        port.baudrate = 115200
        start_time = time.time()
        p_timeout = 20
        while True:
            line = port.readline()
            if b'Initialising Command line: >>' in line:
                print("- CLI Initialised")
                return True
            elif (time.time() - start_time) > p_timeout:
                print("connection timed out")
                return False

    def exec_cmd(self, port, command, args=None):
        ret = ""
        status = None
        port.timeout = 3
        port.baudrate = 115200
        port.write(command.encode() + b'\r')
        if args:
            time.sleep(0.1)
            if type(args) is str:
                args = args.encode()
            port.write(args)
            port.write(b'\0')

        while True:
            port.timeout = 1.5
            line = (port.readline()).decode()
            if 'Status: Success' in line:
                status = True
            elif 'Status: Failure' in line:
                status = False
            if status is True or status is False:
                while True:
                    line = (port.readline()).decode()
                    if ">>" in line:
                        if status is True:
                            print(line)
                        break
                    else:
                        ret += line
                return [{"Status": status}, {"Return": ret}]


def _exec_shell_cmd(self, command):
    result = subprocess.Popen((command).split(), stdout=subprocess.PIPE)
    out, err = result.communicate()
    return out


def load_app_stub(bin_path, esp):
    esp.connect()
    abs_bin_path = os.path.dirname(os.path.abspath(__file__)) + '/../' + bin_path
    if (os.path.exists(abs_bin_path) is False):
        print("Stub not found")
        exit(0)
    arg_tuple = collections.namedtuple('ram_image', ['filename'])
    args = arg_tuple(abs_bin_path)
    esp.change_baud(baud=921600)
    esptool.load_ram(esp, args)


def esp_cmd_check_ok(retval, cmd_str):
    if retval[0]['Status'] is not True:
        print((cmd_str + "failed to execute"))
        print((retval[1]['Return']))
        exit(0)
