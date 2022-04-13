# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


from __future__ import print_function

import os
from aostools import *


class Generate(Command):
    common = True
    helpSummary = "Generate manifest.yaml templet file"
    helpUsage = """
%prog
"""
    helpDescription = """
Generate manifest.yaml templet file
"""
    def _Options(self, p):
        p.add_option('-a', '--all',
                     dest='gen_all', action='store_true',
                     help='generate all components manifest.yaml')
        p.add_option('-s', '--solution',
                     dest='gen_solution', action='store_true',
                     help='generate current solution manifest.yaml')
        p.add_option('-k', '--sdk',
                     dest='gen_sdk', action='store', type='string',
                     help='generate sdk components manifest.yaml')
    def Execute(self, opt, args):
        yoc = YoC()
        mani = Manifest()
        if opt.gen_all:
            mani.gen_templet(yoc.components)
        elif opt.gen_solution:
            solution = yoc.getSolution()
            if solution == None:
                put_string("The current directory is not a solution!")
                return 1
            mani.gen_templet(solution.components)
        elif opt.gen_sdk:
            print(opt.gen_sdk)
            components = ComponentGroup()
            sdk_cmpt = yoc.components.get(opt.gen_sdk)
            if sdk_cmpt:
                sdk_cmpt.load_package()
                for name in sdk_cmpt.depends:
                    if type(name) == dict:
                        name = list(name.keys())[0]
                    cmpt = yoc.components.get(name)
                    if cmpt:
                        components.add(cmpt)
                mani.gen_templet(components)
            else:
                put_string("The sdk component %s is not exist." % opt.gen_sdk)
                return 1
        return 0
