# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


from __future__ import print_function

from aostools import *
import os
import shutil

class Pack(Command):
    common = True
    helpSummary = "Compress component to dest directory"
    helpUsage = """
%prog <component> [<component> <component> ...] <dest path>
"""
    helpDescription = """
Compress component to dest directory
"""

    def Execute(self, opt, args):
        if len(args) < 2:
            self.Usage()

        yoc = YoC()
        componentList = args[:-1]
        validComponentList = []
        destPath = args[-1]
        destPath = os.path.abspath(destPath)

        for component in yoc.components:
            if component.name in componentList:
                component.load_package()
                self.component_export(component, destPath)
                validComponentList.append(component.name)
        invalidComponentList = set(componentList).difference(set(validComponentList))
        if invalidComponentList:
            put_string("These components [%s] are not existed!" % (", ").join(invalidComponentList))
        return 0


    def component_export(self, component, path):
        p = get_cmpt_path_by_type(component.type)
        if p:
            groupdir = os.path.join(path, get_cmpt_top_path_by_type(component.type))
            compdir = os.path.join(path, p, component.name)
            gitdir = os.path.join(compdir, '.git')
            try:
                # copy component files
                if os.path.exists(groupdir):
                    shutil.rmtree(groupdir)
                shutil.copytree(component.path, compdir)
                # remove .git
                if os.path.exists(gitdir):
                    shutil.rmtree(gitdir)
                # compress
                zipName = os.path.join(path, "%s-%s.zip" % (component.name, component.version))
                put_string("Pack `%s` to %s." % (component.name, zipName))
                if os.path.exists(zipName):
                    os.remove(zipName)
                zip_path(groupdir, zipName)
                shutil.rmtree(groupdir)
            except Exception as ex:
                put_string(str(ex))
                pass
