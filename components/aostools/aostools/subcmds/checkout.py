# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


from __future__ import print_function

from aostools import *


class Checkout(Command):
    common = True
    helpSummary = "Checkout a branch for development"
    helpUsage = """
%prog <branchname> [<project>...]
"""
    helpDescription = """
Initialize yoc workspace in the current directory.
"""

    def ValidateOptions(self, opt, args):
        if not args:
            self.Usage()

    # def _Options(self, p):
    #     p.add_option('-a', '--all',
    #                  dest='show_all', action='store_true',
    #                  help='show the complete list of commands')

    def Execute(self, opt, args):
        nb = args[0]

        yoc = YoC()
        pm = Progress('Checkout %s' % nb, len(args[1:]))
        for arg in args[1:]:
            pm.update()
            component = yoc.components.get(arg)
            if component:
                git = GitRepo(component.path, component.repo_url)
                git.CheckoutBranch(nb)
        pm.end()
