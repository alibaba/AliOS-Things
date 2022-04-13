# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


from __future__ import print_function

from aostools import *


class Show(Command):
    common = True
    helpSummary = "Display the detailed compilation information of the current solution"
    helpUsage = """
%prog [option]
"""
    helpDescription = """
Display the detailed compilation information of the current solution.
"""

    def _Options(self, p):
        p.add_option('-a', '--all',
                     dest='show_all', action='store_true',
                     help='show the complete list of commands')
        p.add_option('-c', '--show-component',
                     dest='show_component', action='store_true',
                     help='show all component')
        p.add_option('-i', '--show-include',
                     dest='show_include', action='store_true',
                     help='show all include path')
        p.add_option('-l', '--show-libs',
                     dest='show_libs', action='store_true',
                     help='show all libs')
        p.add_option('-f', '--show-flags',
                     dest='show_flags', action='store_true',
                     help='show all FLAGS')
        p.add_option('-v', '--show-variable',
                     dest='show_variables', action='store_true',
                     help='show all variables')
        p.add_option('-d', '--show-define',
                     dest='show_define', action='store_true',
                     help='show all macro define')
        p.add_option('-s', '--show-source',
                     dest='show_source', action='store_true',
                     help='show all source code file')
        p.add_option('-S', '--show-source-path',
                     dest='show_source_path', action='store_true',
                     help='show all source code file with absolute path')
        p.add_option('-b', '--board',
                     dest='board_name', action='store', type='str', default=None,
                     help='show configuration of selected board')


    def Execute(self, opt, args):
        yoc = YoC()
        solution = yoc.getSolution(opt.board_name)
        if solution == None:
            put_string("The current directory is not a solution!")
            exit(-1)
        if opt.show_all:
            opt.show_component = True
            opt.show_libs = True
            opt.show_include = True
            opt.show_variables = True
            opt.show_define = True
            opt.show_flags = True
            opt.show_source_path = True
        elif not (opt.show_component or opt.show_libs or opt.show_include or opt.show_variables or opt.show_define or opt.show_flags or opt.show_source):
            opt.show_component = True
            opt.show_flags = True

        solution.show(show_component=opt.show_component, show_libs=opt.show_libs,
                      show_include=opt.show_include, show_veriable=opt.show_variables,
                      show_define=opt.show_define, show_flags=opt.show_flags)
        if opt.show_source or opt.show_source_path:
            show_path = opt.show_source_path
            for c in solution.components:
                c.show()
                c.info(4, show_path)
        return 0
