# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


from __future__ import print_function

from aostools import *


class Addkey(Command):
    common = True
    helpSummary = "Add your RSA public key into repo"
    helpUsage = """
%prog
"""
    helpDescription = """
Add your RSA public key into repo.
"""

    def Execute(self, opt, args):
        conf = Configure()
        repo = RepoGitee(conf.gitee_token, conf.group)
        repo.add_public_key()

