# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


from __future__ import print_function

from aostools import *
from aostools.tools import get_elf_bin_file
import os
import codecs
import shutil
from serial.tools import miniterm

class Monitor(Command):
    common = True
    helpSummary = "Open a simple serial monitor."
    helpUsage = """
%prog [PORT] [BAUDRATE]
"""
    helpDescription = """
Open a simple serial monitor.
"""
    # def _Options(self, p):
    #     p.add_option('-b', '--board',
    #                  dest='board_name', action='store', type='str', default=None,
    #                  help='get burn tools from selected board')

    def Execute(self, opt, args):
        yoc = YoC()
        change_dir_to_solution(yoc.conf.yoc_path)
        config = self.get_config()
        needsave = False
        if len(args) == 2:
            config['serialport'] = args[0]
            config['baudrate'] = args[1]
            needsave = True
        if (not config['serialport']) or (not config['baudrate']):
            config["serialport"] = miniterm.ask_for_port()
            if not config['serialport']:
                print("no specified serial port")
                return 1
            if sys.version_info[0] < 3:
                config['baudrate'] = raw_input('--- Enter serial baudrate: ')
            else:
                config['baudrate'] = input('--- Enter serial baudrate: ')
            if not config['baudrate']:
                print("no specified baudrate")
                return 1
            needsave = True

        ret = os.system("pyserial-miniterm %s %s" % (config["serialport"], config["baudrate"])) >> 8
        if needsave and ret == 0:
            self.save_config(config)
        return ret

    def read_json(self, json_file):
        data = {}
        if os.path.isfile(json_file):
            with codecs.open(json_file, 'r', 'UTF-8') as f:
                data = json.load(f)
                if not data:
                    data = {}
        return data

    def write_json(self, json_file, data):
        with codecs.open(json_file, 'w', 'UTF-8') as f:
            f.write(json.dumps(data, indent=4, separators=(',', ': ')))

    def get_config(self):
        """ get configuration from .config_monitor file """ 
        config = {}
        config_file = os.path.join(os.getcwd(), '.config_monitor')
        if os.path.isfile(config_file):
            config = self.read_json(config_file)

        if 'serialport' not in config:
            config['serialport'] = ""
        if 'baudrate' not in config:
            config['baudrate'] = ""

        return config

    def save_config(self, config):
        """ save configuration to .config_monitor file""" 
        if config:
            config_file = os.path.join(os.getcwd(), '.config_monitor')
            self.write_json(config_file, config)

