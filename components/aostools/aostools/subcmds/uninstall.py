# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


from __future__ import print_function

from aostools import *


class Uninstall(Command):
    common = True
    helpSummary = "Uninstall component"
    helpUsage = """
%prog [<component>...]
"""
    helpDescription = """
Uninstall component.
"""
    # def _Options(self, p):
    #     p.add_option('-a', '--all',
    #                  dest='show_all', action='store_true',
    #                  help='show the complete list of commands')

    def Execute(self, opt, args):
        if len(args) > 0:
            yoc = YoC()
            for name in args:
                if yoc.remove_component(name):
                    yoc.update()
                    put_string("%s uninstall Success!" % name)
        return 0
