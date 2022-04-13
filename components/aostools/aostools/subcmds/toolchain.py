# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


from __future__ import print_function

import os
from aostools import *


class Toolchain(Command):
    common = True
    helpSummary = "Install toolchain"
    helpUsage = """
%prog [--all] [--arm_none] [--csky] [--riscv]
"""
    helpDescription = """
Install toolchain.
"""

    def _Options(self, p):
        p.add_option('-c', '--csky',
                     dest='install_csky', action='store_true',
                     help=' install(or uninstall if with -d, --delete) csky-elfabiv2-tools')
        p.add_option('-r', '--riscv',
                     dest='install_riscv', action='store_true',
                     help=' install(or uninstall if with -d, --delete) riscv-elf-tools')
        p.add_option('-a', '--arm_none',
                     dest='install_arm_none', action='store_true',
                     help=' install(or uninstall if with -d, --delete) arm-none-eabi')
        p.add_option('-A', '--all',
                     dest='install_all', action='store_true',
                     help=' install(or uninstall if with -d, --delete) csky-elfabiv2-tools, riscv-elf-tools and arm-none-eabi')
        p.add_option('-d', '--delete',
                     dest='uninstall_toolchain', action='store_true',
                     help=' uninstall specific toolchain')

    def Execute(self, opt, args):
        need_usage = True
        tool = ToolchainYoC()
        if opt.install_all:
            if not opt.uninstall_toolchain:
                tool.check_toolchain('csky-abiv2-elf', 1)
                tool.check_toolchain('riscv64-unknown-elf', 1)
                tool.check_toolchain('arm-none-eabi', 1)
            else:
                tool.uninstall_toolchain('csky-abiv2-elf')
                tool.uninstall_toolchain('riscv64-unknown-elf')
                tool.uninstall_toolchain('arm-none-eabi')
            need_usage = False
        else:
            if opt.install_csky:
                if not opt.uninstall_toolchain:
                    tool.check_toolchain('csky-abiv2-elf', 1)
                else:
                    tool.uninstall_toolchain('csky-abiv2-elf')
                need_usage = False
            if opt.install_riscv:
                if not opt.uninstall_toolchain:
                    tool.check_toolchain('riscv64-unknown-elf', 1)
                else:
                    tool.uninstall_toolchain('riscv64-unknown-elf')
                need_usage = False
            if opt.install_arm_none:
                if not opt.uninstall_toolchain:
                    tool.check_toolchain('arm-none-eabi', 1)
                else:
                    tool.uninstall_toolchain('arm-none-eabi')
                need_usage = False
        if need_usage:
            self.Usage()
        return 0
