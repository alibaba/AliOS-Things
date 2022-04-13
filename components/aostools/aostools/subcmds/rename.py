# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


from __future__ import print_function

from aostools import *
import os


class Rename(Command):
    common = True
    helpSummary = "Component rename <old_name> to <new_name>"
    helpUsage = """
%prog <old_name> <new_name>
"""
    helpDescription = """
Component rename <old_name> to <new_name>.
"""

    def Execute(self, opt, args):
        yoc = YoC()
        if len(args) == 2:
            old_name = args[0]
            new_name = args[1]
            if is_contain_chinese(new_name) or not is_leagal_name(new_name):
                put_string("New name: %s is not valid!" % new_name)
                exit(-1)
            if len(new_name) > 64:
                put_string("New name: %s is too long, max length is 64." % new_name)
                exit(-1)
            component = yoc.components.get(old_name)
            if component:
                if os.getcwd() == component.path:
                    put_string("Can't rename component in its own directory!")
                    exit(-1)
                dep_on = yoc.check_depend_on(component)
                if component.rename(new_name):
                    for c in dep_on:
                        for v in c.pack.depends:
                            if old_name in v:
                                c.pack.depends.remove(v)
                        c.pack.depends.append({component.name: component.version})
                        c.pack.save(os.path.join(c.path, 'package.yaml'))
                        put_string("Component %s's package.yaml is upgraded." % c.name)
                    put_string("Component `%s` -> `%s` success." % (old_name, new_name))
                    return 0
                else:
                    put_string("Component rename fail!")
            else:
                put_string("Component `%s` not found!" % old_name)
        else:
            self.Usage()
        return 1

