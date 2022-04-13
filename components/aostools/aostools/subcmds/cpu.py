# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


from __future__ import print_function
from aostools import *


class Cpu(Command):
    common = True
    helpSummary = "list support cpu models"
    helpUsage = """
%prog [option]
"""
    helpDescription = """
list support cpu models
"""

    def _Options(self, p):
        p.add_option('-c', '--csky',
                     dest='csky', action='store_true',
                     help='list csky cpu models')
        p.add_option('-a', '--arm',
                     dest='arm', action='store_true',
                     help='list arm cpu models')
        p.add_option('-r', '--riscv',
                     dest='riscv', action='store_true',
                     help='list riscv cpu models')

    def Execute(self, opt, args):
        ck_cpu = globals()['CkcoreCPU']
        arm_cpu = globals()['ArmCPU']
        rv_cpu = globals()['RiscvCPU']
        if not (opt.csky or opt.arm or opt.riscv):
            print('### csky cpu:')
            print(ck_cpu)
            print('### riscv cpu:')
            print(rv_cpu)
            print('### arm cpu:')
            print(arm_cpu)
            return
        if opt.csky:
            print('### csky cpu:')
            print(ck_cpu)
        if opt.arm:
            print('### arm cpu:')
            print(arm_cpu)
        if opt.riscv:
            print('### riscv cpu:')
            print(rv_cpu)
