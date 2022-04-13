# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


from __future__ import print_function

from aostools import *


class Branch(Command):
    common = True
    helpSummary = "View current component branches"
    helpUsage = """
%prog [<compnent> ...]
"""
    helpDescription = """
View current component branches
"""
    # def _Options(self, p):
    #     p.add_option('-a', '--all',
    #                  dest='show_all', action='store_true',
    #                  help='show the complete list of commands')

    def Execute(self, opt, args):
        yoc = YoC()
        if len(args) == 0:
            for component in yoc.components:
                git = GitRepo(component.path, component.repo_url)
                branches = git.GetRemoteBranches()
                put_string(component.name + ':')
                for b in branches:
                    put_string('  ', b.name)
        else:
            for arg in args:
                component = yoc.components.get(arg)
                if component:
                    git = GitRepo(component.path, component.repo_url)
                    branches = git.GetRemoteBranches()
                    put_string(component.name + ':')
                    for b in branches:
                        put_string('  ', b.name)
