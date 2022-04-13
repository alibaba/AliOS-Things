# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


from __future__ import print_function

from aostools import *


class List(Command):
    common = True
    helpSummary = "List component"
    helpUsage = """
%prog [option] [<component> ...]
"""
    helpDescription = """
List all projects; pass '.' to list the project for the cwd.
"""

    def _Options(self, p):
        p.add_option('-r', '--remote',
                     dest='show_remote', action='store_true',
                     help='show OCC all compoent')

        p.add_option('-s', '--solution',
                     dest='solution', action='store_true',
                     help='list all soution component')

        p.add_option('-b', '--board',
                     dest='board', action='store_true',
                     help='list all board component')

        p.add_option('-c', '--chip',
                     dest='chip', action='store_true',
                     help='list all chip component')

        p.add_option('-a', '--arch',
                     dest='arch', action='store_true',
                     help='list all arch component')

        p.add_option('-o', '--core',
                     dest='core', action='store_true',
                     help='list all drv_core component')

        p.add_option('-p', '--peripheral',
                     dest='peripheral', action='store_true',
                     help='list all drv_peripheral component')

        p.add_option('-e', '--device',
                     dest='device', action='store_true',
                     help='list all drv_external_device component')

        p.add_option('-n', '--kernel',
                     dest='kernel', action='store_true',
                     help='list all kernel component')

        p.add_option('-m', '--common',
                     dest='common', action='store_true',
                     help='list all common component')

        p.add_option('-k', '--sdk',
                     dest='sdk', action='store_true',
                     help='list all sdk component')

        p.add_option('-d', '--depend',
                     dest='depend', action='store_true',
                     help='list component dependencies')

    def Execute(self, opt, args):
        yoc = YoC()

        count = len(args)

        if opt.show_remote:
            yoc.update(True)
            group = yoc.occ_components
        else:
            group = yoc.components
            for c in group:
                if count == 0 or c.name in args:
                    c.load_package()
        
        if len(group) == 0:
            put_string("There is no component found!")
            return 1

        nonexistent_cnt = 0
        name_list = yoc.get_name_list(group)
        for name in args:
            if name not in name_list:
                put_string("Can't found component `%s`!" % name)
                nonexistent_cnt+=1

        show = opt.solution or opt.board or opt.chip or opt.arch or opt.core or opt.peripheral or opt.device or opt.kernel or opt.common or opt.sdk
        show_cnt = 0
        for c in group:
            if count == 0 or c.name in args:
                if show:
                    if (opt.solution and c.type == 'solution') or (opt.board and c.type == 'board') or (opt.chip and c.type == 'chip') or (opt.arch and c.type == 'arch') or (opt.core and c.type == 'drv_core') or (opt.peripheral and c.type == 'drv_peripheral') or (opt.device and c.type == 'drv_external_device') or (opt.kernel and c.type == 'kernel') or (opt.common and c.type == 'common') or (opt.sdk and c.type == 'sdk') :
                        self.show_depend(yoc, c, opt.depend)
                        show_cnt+=1
                else:
                    self.show_depend(yoc, c, opt.depend)
                    show_cnt +=1
        if show_cnt == 0 and nonexistent_cnt == 0:
            put_string('No matched component found!')
            return 1
        return 0

    def show_depend(self, yoc, component, show_depend=False):
        component.load_package()
        component.show()
        if show_depend:
            depend = yoc.check_depend(component)
            if len(depend) > 0:
                put_string("    depends:")
                depend.show(8)
            depend_on = yoc.check_depend_on(component)
            if len(depend_on) > 0:
                put_string("    depends on:")
                depend_on.show(8)
