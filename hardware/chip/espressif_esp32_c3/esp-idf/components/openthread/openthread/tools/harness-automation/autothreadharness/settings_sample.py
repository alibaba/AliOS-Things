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

AUTO_DUT = True
"""bool: Whether use the auto DUT feature of thread harness."""

DUT_DEVICE = ('COM16', 'OpenThread')
"""(str, str): The first element is serial port of the DUT, and the second is
the device type."""

DUT2_DEVICE = ('COM18', 'OpenThread')
"""(str, str): The first element is serial port of the DUT, and the second is
the device type. DUT in RF-box for RF-shield needed cases."""

DUT_VERSION = 'g12345'
"""str: Version of DUT"""

DUT_MANUFACTURER = 'Open Thread'
"""str: Manufacturer of the DUT"""

THREAD_CHANNEL = 18
"""int: Thread channel"""

THREAD_PANID = '0xface'
"""str: Thread PAN ID"""

THREAD_NETWORKNAME = 'GRL'
"""str: Thread network name"""

THREAD_EXTPANID = '000db80000000000'
"""str: Thread extended PAN ID"""

THREAD_CHILD_TIMEOUT = 0
"""int: Child timeout in seconds. Set to 0 to use Harness's default value."""

THREAD_SED_POLLING_INTERVAL = 0
"""int: SED polling interval in seconds. Set to 0 to use Harness's default value."""

HARNESS_HOME = 'C:\\GRL\\Thread1.1'
"""str: Harness installation path, e.g. 'C:\\GRL\\Thread1.1'"""

HARNESS_URL = 'http://127.0.0.1:8000'
"""str: Harness front-end url"""

TESTER_NAME = 'Thread Open'
"""str: Who are you"""

TESTER_REMARKS = 'OpenThread is great'
"""str: Any comments in the final PDF"""

GOLDEN_DEVICES = []
"""[(str, str)]: golden device list.
It is a port and vendor pair list like [('COM1', 'OpenThread'), ('COM2', 'ARM')] for over-the-air golden devices
connected to Windows. For OpenThread golden devices, ser2net is also supported by using IP:PORT as the port
like ('192.168.1.2:5001', 'OpenThread').
"""

SHIELD_GOLDEN_DEVICES = []
"""[(str, str)]: shielded golden device list.
It is a port and vendor pair list like [('COM1', 'OpenThread'), ('COM2', 'ARM')] for shielded golden devices
connected to Windows. For OpenThread golden devices, ser2net is also supported by using IP:PORT as the port
like ('192.168.1.2:5001', 'OpenThread').

For current topology, maximal common Leader and Commissioner devices for case 9.2.9 and 9.2.10 should be put
into the RF-box besides DUT2_DEVICE.

Example for CV testbed, 2 conformance devices should be put into the RF-box and listed here.

Example for IV testbed using TopologyConfig_20180907b.txt, 1 OpenThread, 1 ARM and 1 SiLabs devices should be
put into the RF-box and listed here.

Example for IV testbed using TopologyConfig_20180907cK.txt, 1 OpenThread and 2 ARM devices should be put into
the RF-box and listed here.

"""

MIXED_DEVICE_TYPE = True
r"""bool: Whether golden devices are mixed-type. Must put
        TopologyConfig.txt under
        HARNESS_HOME\Thread_Harness\TestScripts before testing if True
"""

OUTPUT_PATH = '.\\output'
"""str: Path to store results and logs, MUST be writable."""

SHIELD_SIMULATION = False
"""bool: whether to simulate RF shield by changing channel"""

PDU_CONTROLLER_TYPE = None
"""str: Type of connected PDU controller.

Keep this None if no PDU controller available.

Types of supported PDU controllers:
    - None - when no PDU controller connected
    - 'APC_PDU_CONTROLLER' - when APC PDU controller connected
    - 'NORDIC_BOARD_PDU_CONTOLLER' - when Nordic boards PDU controller connected
    - 'IP_POWER_SOCKET_PDU_CONTROLLER' - when IP Power Socket 5G10A connected
    - 'EATON_PDU_CONTROLLER' - when EATON PDU controller connected
"""

PDU_CONTROLLER_OPEN_PARAMS = {'port': 23, 'ip': '127.0.0.1'}
"""dict: Parameters pass to the "open" method of PDU controller.

Example parameters for the 'APC_PDU_CONTROLLER':
    {'port': 23, 'ip': '127.0.0.1'}

Example parameters for the 'NORDIC_BOARD_PDU_CONTOLLER':
    {} - empty dictionary

Example parameters for the 'IP_POWER_SOCKET_PDU_CONTROLLER':
    {'user': 'user', 'pass': 'pass', 'ip': '127.0.0.1'}
"""

PDU_CONTROLLER_REBOOT_PARAMS = {'outlet': 1}
"""dict: Parameters pass to the "reboot" method of PDU controller.

Example parameters for the 'APC_PDU_CONTROLLER':
    {'outlet': 1}

Example parameters for the 'NORDIC_BOARD_PDU_CONTOLLER':
    {'boards_serial_numbers': ('12345123', ...)}

Example parameters for the 'IP_POWER_SOCKET_PDU_CONTROLLER':
    {'sockets': [0, 1]}
Example parameters for the 'EATON_PDU_CONTROLLER':
    {'ip': '127.0.0.1'}
"""

SHIELD_CONTROLLER_TYPE = None
"""str: Type of connected RF Shield controller.

Keep this None if no RF Shield controller available.

Types of supported RF Shield controllers:
    - None - when no RF Shield controller connected
    - 'RF_SWITCH' - when RF Switch connected
"""

SHIELD_CONTROLLER_PARAMS = None
"""dict: Parameters passed to the "__init__" method of RF Shield controller.

Example parameters for the 'RF_SWITCH':
    {'channel': 200, 'port': 'COM50'}
"""
