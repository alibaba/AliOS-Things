# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


from __future__ import print_function

from aostools import *
import os
import shutil

class Export(Command):
    common = True
    helpSummary = "Export component to dest directory"
    helpUsage = """
%prog <component> [<component> <component> ...] <dest path>
"""
    helpDescription = """
Export component to dest directory
"""

    def Execute(self, opt, args):
        if len(args) < 2:
            self.Usage()

        yoc = YoC()
        componentList = args[:-1]
        validComponentList = []
        destPath = args[-1]

        for component in yoc.components:
            if component.name in componentList:
                component.load_package()
                depend = yoc.check_depend(component)
                for d in depend:
                    self.component_export(d, destPath)
                self.component_export(component, destPath)
                validComponentList.append(component.name)
        if validComponentList:
            yoc.conf.save(os.path.join(destPath, '.aos'))
        invalidComponentList = set(componentList).difference(set(validComponentList))
        if invalidComponentList:
            put_string("These components [%s] are not existed!" % (", ").join(invalidComponentList))
        return 0


    def component_export(self, component, path):
        p = get_cmpt_path_by_type(component.type)
        if p:
            dest = os.path.join(path, p, component.name)
            put_string("Export `%s` to %s." % (component.name, dest))
            try:
                if os.path.exists(dest):
                    shutil.rmtree(dest)
                shutil.copytree(component.path, dest)
            except Exception as ex:
                put_string(str(ex))
                pass
