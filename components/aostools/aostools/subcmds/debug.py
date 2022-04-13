# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


from __future__ import print_function

from aostools import *
from aostools.tools import get_elf_bin_file
import os
import shutil

class Debug(Command):
    common = True
    helpSummary = "Start gdb server"
    helpUsage = """
%prog 
"""
    helpDescription = """
Start gdb server
"""
    def _Options(self, p):
        p.add_option('-b', '--board',
                     dest='board_name', action='store', type='str', default=None,
                     help='get gdb server tools from selected board')

    def Execute(self, opt, args):
        yoc = YoC()
        solution = yoc.getSolution(opt.board_name)
        if solution == None:
            put_string("The current directory is not a solution!")
            return 1
        if not solution.debug_server or not os.path.isfile(solution.debug_server):
            put_string("Can not find gdb server tool for this solution(or board or chip)!")
            return 1
        return self.debug_script(solution.debug_server)
        
    def debug_script(self, script_file):
        cmdstr = script_file
        elf_file, bin_file = get_elf_bin_file('SConstruct')
        if elf_file:
            cmdstr += " --elf=%s" % os.path.abspath(elf_file)
        if bin_file:
            cmdstr += " --bin=%s" % os.path.abspath(bin_file)
        return os.system(cmdstr) >> 8

