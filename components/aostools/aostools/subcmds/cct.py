# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


from __future__ import print_function

from aostools import *
import os
import sys
import struct
import time
import zlib

try:
    import serial
except ImportError:
    print("Pyserial is not installed for %s. Check the README for installation instructions." % (sys.executable))
    raise

try:
    import serial.tools.list_ports as list_ports
except ImportError:
    print("The installed version (%s) of pyserial appears to be too old for esptool.py (Python interpreter %s). "
          "Check the README for installation instructions." % (sys.VERSION, sys.executable))
    raise
except Exception:
    if sys.platform == "darwin":
        list_ports = None
    else:
        raise

def print_overwrite(message, last_line=False):
    if sys.stdout.isatty():
        print("\r%s" % message, end='\n' if last_line else '')
        sys.stdout.flush()
    else:
        print(message)

class UnsupportedCommandError(RuntimeError):
    """
    Wrapper class for when ROM loader returns an invalid command response.
    Usually this indicates the loader is running in Secure Download Mode.
    """
    def __init__(self, cct, op):
        msg = "Invalid (unsupported) command 0x%x" % op
        RuntimeError.__init__(self, msg)

def to_size(size):
    if size < 1024:
        return '{}'.format(size)
    elif size < 1024 * 1024:
        return '%6.1fKB' % (size / 1024)
    elif size < 1024 * 1024 * 1024:
        return '%6.1fMB'% (size / 1024 / 1024)
    elif size < 1024 * 1024 * 1024 * 1024:
        return '%6.1fGB' % (size / 1024 / 1024 / 1024.0)
    return '{}'.format(size)

DEFAULT_TIMEOUT = 3      # timeout for most flash operations
MAX_TIMEOUT = 300        # longest any command can run

class CCT:
    DEFAULT_PORT = "/dev/ttyUSB0"
    CHIP_ROM_BAUD = 115200

    def __init__(self, port=DEFAULT_PORT, baud=CHIP_ROM_BAUD, trace_enabled=True):
        self._port = None
        self._trace_enabled = trace_enabled
        self._last_trace = time.time()
        if port == None and list_ports:
            port_list = list(list_ports.comports())
            for i in port_list:
                if i.hwid != 'n/a':
                    port = i.device
                    break
        if not port:
            print("Not found serial port.")
            exit()
        try:
            self._port = serial.Serial(port, baud)
        except:
            raise Exception("could not open serial port {}".format(port))

    def read(self, count):
        return self._port.read(count)

    def write(self, data):
        self._port.write(data)

    def command(self, cmd, param, data=b'', timeout=DEFAULT_TIMEOUT):
        saved_timeout = self._port.timeout
        new_timeout = min(timeout, MAX_TIMEOUT)
        if new_timeout != saved_timeout:
            self._port.timeout = new_timeout

        try:
            c = struct.pack('!BBH', cmd, param, len(data)) + data
            self.write(c)
            v = self.read(4)
            if len(v) == 4:
                data = struct.unpack('!BBH', v)
                payload = self.read(data[2])

                if data[0] == cmd and data[1] >= 0:
                    return True, data, payload

                if data[1] == 0xFD or data[1] == 0xFF:
                    raise UnsupportedCommandError(self, cmd)

            if len(v) >= 4 and data[0] != cmd:
                raise UnsupportedCommandError(self, cmd)
        finally:
            if new_timeout != saved_timeout:
                self._port.timeout = saved_timeout

        return False, [], None


    def connect(self, timeout=30):
        start_ticks = time.time()
        print_count = 0
        while True:
            now_tick = time.time()
            if now_tick - start_ticks > timeout:
                break

            ret, v, _ = self.command(0x10, 0xEF, struct.pack('!BBBB', 0x43, 0x53, 0x4B, 0x59), timeout=0.1)
            if ret and v[1] == 1:
                if print_count != 0:
                    print()
                self.trace('connect succuss!')

                return True
            else:
                if print_count == 0:
                    print("Wait ", end='')
                else:
                    print(".", end='')
                print_count += 1
                sys.stdout.flush()
        return False


    def get_version(self):
        ret, v, _ = self.command(0x01, 0x00)
        if ret:
            print('CCT Version: %d' % v[1])


    def get_timeout(self):
        ret, v, _ = self.command(0x0A, 0x00)
        if ret:
            print(v)


    def get_device_id(self, param):
        ret, v, _ = self.command(0x0B, param)
        if ret:
            if v[1] == 0xE0:
                self.trace('device id not exist.')

    def get_memory_info(self):
        mem = {}
        ret, v, data = self.command(0x11, 0x00)
        if ret and v[1] == 1:
            length = v[2]

            idx = 0
            for _ in range(0, length // 6):
                vv = struct.unpack('!BBI', data[idx : idx + 6])
                TY = vv[0]
                if TY == 0x00:
                    mem['recv'] = {'type': vv[0], 'id': vv[1], 'size': vv[2], 'desc': '协议接收缓冲区'}
                elif TY == 0x01:
                    mem['send'] = {'type': vv[0], 'id': vv[1], 'size': vv[2], 'desc': '协议发送缓冲区'}
                elif TY == 0x02:
                    mem['ram{}'.format(vv[1])] = {'type': vv[0], 'id': vv[1], 'size': vv[2], 'desc': 'SRAM'}
                elif TY == 0x03:
                    mem['eflash{}'.format(vv[1])] = {'type': vv[0], 'id': vv[1], 'size': vv[2], 'desc': 'EFlash'}
                elif TY == 0x10:
                    mem['ddr{}'.format(vv[1])] = {'type': vv[0], 'id': vv[1], 'size': vv[2], 'desc': 'SDRAM/DDR'}
                elif TY == 0x11:
                    mem['nflash{}'.format(vv[1])] = {'type': vv[0], 'id': vv[1], 'size': vv[2], 'desc': 'NOR Flash'}
                elif TY == 0x12:
                    mem['sflash{}'.format(vv[1])] = {'type': vv[0], 'id': vv[1], 'size': vv[2], 'desc': 'SPI Flash'}
                elif TY == 0x13:
                    mem['nand{}'.format(vv[1])] = {'type': vv[0], 'id': vv[1], 'size': vv[2], 'desc': 'Nand Flash'}
                elif TY == 0x14:
                    mem['sd{}'.format(vv[1])] = {'type': vv[0], 'id': vv[1], 'size' : vv[2] * 512, 'desc': 'SD Flash'}
                elif TY == 0x15:
                    mem['emmc{}'.format(vv[1])] = {'type': vv[0], 'id': vv[1], 'size': vv[2] * 512, 'desc': 'eMMC Flash'}
                idx += 6

        return mem


    def download_init(self, Type, ID, offset, length):
        # Cmd	Param	Len16_H	Len16_L	    Data, Len=(Len16_H<<8 & Len16_L)
        # 0x12	0x00	0x00	1+1+4+4+N	1:Type 1: ID, 4: Offset, 4: Size, N=Data Name
        # 0x12	0x01	0x00	4+4+N	     4: Address, 4: Size, N=Image Name
        c = struct.pack('!BBII', Type, ID, offset, length)
        ret, v, _ = self.command(0x12, 0x00, c)

        if ret and v[1] == 1 and v[2] == 0:
            self.trace('download_init type=%d, id=%d, offset=%d, lenght=%d', Type, ID, offset, length)
        return ret

    def download_data(self, flags, data):
        # Cmd	Param	Len16_H	Len16_L	Data, Len=(Len16_H<<8 & Len16_L)
        # 0x13	0x00~03	N	            N: (1~SoC Buffer Size)
        ret, v, _ = self.command(0x13, flags, data)
        if ret:
            if v[1] == flags:
                pass
            else:
                if v[1] == 0xE0:
                    self.trace("写入错误")
                elif v[1] == 0xE1:
                    self.trace("单数据包长度过大")
                elif v[1] == 0xE1:
                    self.trace("超过请求下载数据长度")
                return False

        return ret

    def download_file(self, offset, filename, Type=2, ID=0, block_size=512, compress=False):
        ret = False
        with open(filename, "rb") as F:
            file_size = 0
            if compress:
                while True:
                    data = F.read(block_size)
                    if not data:
                        break
                    data = zlib.compress(data, 9)
                    file_size += len(data)
                    full_size = F.tell()
                print("%d => %d %3.2f%%" % (full_size, file_size, file_size * 100.0 / full_size))

            else:
                F.seek(0, 2)
                file_size = F.tell()

            self.download_init(Type, ID, offset, file_size)

            offset = 0
            F.seek(0, 0)
            while True:
                data = F.read(block_size)
                data_len = len(data)
                if data_len > 0:
                    if compress:
                        uncdata = data
                        data = zlib.compress(uncdata, 9)
                        data_len = len(data)
                    if offset == 0:
                        ret = self.download_data(0x03, data)
                    elif offset + data_len < file_size:
                        ret = self.download_data(0x02, data)
                    else:
                        ret = self.download_data(0x00, data)
                    offset += data_len
                else:
                    break
                print_overwrite('Writing at 0x%08x... (%d%%)' % (offset, 100 * (offset / file_size)))
            print_overwrite("File {} download success.".format(filename), last_line=True)

        return ret

    def verify_data(self, verify_code):
        pass
        # c = struct.pack('!BBII', 2, 0, 0x00000)
        # ret, v, _ = self.command(0x18, 0x00, c)

    def read_memory(self, address, size=512):
        # 0x26	0x00	0x00	1+1+4+2	1:Type 1: ID, 4: Offset, 2: Size
        # 0x26	0x01	0x00	4+2	4: Address, 2: Size
        c = struct.pack('!II', address, size)
        ret, v, _ = self.command(0x12, 0x01, c)
        if ret:
            print(v)


    def run_image(self, offset):
        # Cmd	Param	Len16_H	Len16_L	   Data, Len=(Len16_H<<8 & Len16_L)
        # 0x18	0x00	0x00	1+1+4+N	   1:Type 1: ID, 4: Offset
        #                                  N: Run parameters, SoC will load to (P0-P3)
        c = struct.pack('!BBI', 2, 0, offset)
        ret, v, _ = self.command(0x18, 0x00, c)
        if ret:
            if v[1] == 0x01:
                self.trace('已准备好，立即运行!')
            elif v[1] == 0xE1:
                self.trace("设备不存在或地址不合法")
            elif v[1] == 0xE2:
                self.trace("源不可XIP执行")
            elif v[1] == 0xE3:
                self.trace("源读取失败")

        return ret and v[1] == 1


    def trace(self, message, *format_args):
        if self._trace_enabled:
            now = time.time()
            try:

                delta = now - self._last_trace
            except AttributeError:
                delta = 0.0
            self._last_trace = now
            prefix = "TRACE +%.3f " % delta
            print(prefix + (message % format_args))

class Cct(Command):
    common = True
    helpSummary = "Bootrom CCT tools"
    helpUsage = """
%prog <list|download> [param]
"""
    helpDescription = """
Bootrom CCT tools
cct read <address> <size>
cct write <type> <id> <offset> <filename>
"""

    def Execute(self, opt, args):
        if len(args) < 1:
            self.Usage()

        cct = CCT(opt.uart)
        cct._trace_enabled = opt.debug
        if not cct.connect():
            return
        cct.get_version()
        mem = cct.get_memory_info()

        if args[0] == 'list':
            print("memory device list:")
            for k, v in mem.items():
                if v['type'] > 1:
                    # print("  dev = %-5s, size = %s, type = %2d, id = %d, desc = %s" % (k, to_size(v['size']), v['type'], v['id'], v['desc']))
                    print("  dev = %-5s, size = %s" % (k, to_size(v['size'])))
        elif args[0] == 'download':
            if os.path.exists(opt.file):
                if opt.device in mem:
                    opt_type = mem[opt.device]['type']
                    opt_id = mem[opt.device]['id']
                    print("Send file '%s' to %d:%d ..." % (opt.file, opt_type, opt_id))
                    cct.download_file(opt.offset, opt.file, opt_type, opt_id, opt.block, opt.compress)
                    cct.run_image(opt.offset)
                else:
                    print('Device `%s` not exists, please select' % opt.device)
            else:
                print("file %s not exists" % opt.file)


    def _Options(self, p):
        p.add_option('-u',
                     dest='uart', action='store', type='str', default=None,
                     help='CCT serial port device')
        p.add_option('-f',
                     dest='file', action='store', type='str', default='',
                     help='')
        p.add_option('-o',
                     dest='offset', action='store', type='int', default=0,
                     help='Device start address')
        p.add_option('-b', '--block',
                     dest='block', action='store', type='int', default=512,
                     help='')
        p.add_option('-d', '--device',
                     dest='device', action='store', type='str', default=None,
                     help='Device name')
        p.add_option('-c', '--compress',
                     dest='compress', action='store_true', default=False,
                     help='')
        p.add_option('-D', '--debug',
                     dest='debug', action='store_true', default=False,
                     help='Enable debbug trace info')
