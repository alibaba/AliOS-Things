# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


from __future__ import print_function

from aostools import *


class Search(Command):
    common = True
    helpSummary = "Search for components whose name or summary contains <query>"
    helpUsage = """
%prog [option] <query>
"""
    helpDescription = """
Search for components whose name or summary contains <query>
"""

    def _Options(self, p):
        p.add_option('-u', '--update',
                     dest='update', action='store_true',
                     help='update component from git or OCC')

    def Execute(self, opt, args):
        if len(args) != 1:
            self.Usage()
        kword = args[0]
        if is_contain_chinese(kword):
            put_string("Not support search chinese string!")
            exit(-1)

        yoc = YoC()
        if opt.update or yoc.occ_components == None:
            yoc.update(True)

        for component in yoc.occ_components:
            keys = [kword, kword.lower(), kword.upper()]
            search_text = kword.lower()
            if component.name.lower().find(search_text) >= 0:
                component.show(key=keys)
            else:
                component.load_package()
                if component.description.lower().find(search_text) >= 0:
                    component.show(key=keys)
        return 0
