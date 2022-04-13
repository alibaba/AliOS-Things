# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


from __future__ import print_function

from aostools import *
from aostools.tools import get_elf_bin_file
import os
import shutil

class Burn(Command):
    common = True
    helpSummary = "Burn elf or binary file into Flash"
    helpUsage = """
%prog 
"""
    helpDescription = """
Burn elf or binary file into Flash
"""
    def _Options(self, p):
        p.add_option('-b', '--board',
                     dest='board_name', action='store', type='str', default=None,
                     help='get burn tools from selected board')
        p.add_option('-f', '--binfile',
                     dest='bin_file', action='store', type='str', default=None,
                     help='burn specified binary file only')

    def Execute(self, opt, args):
        yoc = YoC()
        change_dir_to_solution(yoc.conf.yoc_path)
        solution = yoc.getSolution(opt.board_name)
        if solution == None:
            put_string("The current directory is not a solution!")
            return 1
        if not solution.flash_program or not os.path.isfile(solution.flash_program):
            put_string("Can not find flash program tool for this solution(or board or chip)!")
            return 1
        if not opt.bin_file:
            data_files = []
            for item in solution.program_data_files:
                one_file = item["filename"] + "#" + hex(item["address"])
                data_files.append(one_file)
            return self.burn_script(solution.flash_program, True, data_files)
        else:
            data_files = [opt.bin_file]
            return self.burn_script(solution.flash_program, False, data_files)
        
    def burn_script(self, script_file, auto_append, data_files):
        cmdstr = '"%s"' % script_file
        if script_file.endswith('.py'):
            cmdstr = 'python "%s"' % script_file
        if auto_append:
            elf_file, bin_file = get_elf_bin_file('SConstruct')
            if elf_file:
                cmdstr += " --elf=\"%s\"" % os.path.abspath(elf_file)
            if bin_file:
                cmdstr += " --bin=\"%s\"" % os.path.abspath(bin_file)
        if data_files:
            for one_file in data_files:
                cmdstr += " --bin=\"%s\"" % one_file
        return os.system(cmdstr) >> 8

