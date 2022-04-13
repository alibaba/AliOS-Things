# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


from __future__ import print_function
from aostools import *


class Protect(Command):
    common = True
    helpSummary = "set branch protection"
    helpUsage = """
%prog
"""
    helpDescription = """
set branch protection
"""

    def _Options(self, p):
        p.add_option('-b', '--branch',
                     dest='branch', action='store_true',
                     help='set branch protection')


    def Execute(self, opt, args):
        if not (opt.branch):
            self.Usage()
            return 1
        br = args[0]
        yoc = YoC()
        repo = RepoGitee(yoc.conf.gitee_token, yoc.conf.group)
        for component in yoc.components:
            put_string("Start to set branch: %s protection for component:%s ." % (br, component.name))
            repo.set_branch_protection(component.name, br)
        return 0
