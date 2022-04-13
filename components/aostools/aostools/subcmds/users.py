# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


from __future__ import print_function
from aostools import *


class Users(Command):
    common = True
    helpSummary = "add or delete user to repo"
    helpUsage = """
%prog [option] <username> [-r <reponame>]
"""
    helpDescription = """
add or delete user to repo
"""

    def _Options(self, p):
        p.add_option('-a', '--add',
                     dest='add', action='store_true',
                     help='add a developer user to repo')
        p.add_option('-d', '--delete',
                     dest='delete', action='store_true',
                     help='delete a developer user to repo')
        p.add_option('-r', '--repo',
                     dest='reponame', action='store', type='string',
                     help='the repo name')

    def Execute(self, opt, args):
        if not (opt.add or opt.delete):
            self.Usage()
            return 1
        username = args[0]
        reponame = None
        yoc = YoC()
        if opt.reponame:
            reponame = opt.reponame
        else:
            yoc.update(True)

        repo = RepoGitee(yoc.conf.gitee_token, yoc.conf.group)
        if opt.add:
            if reponame:
                put_string("Add user:%s to repo:%s." % (username, reponame))
                repo.add_user_to_repo(reponame, username)
            else:
                for component in yoc.occ_components:
                    put_string("Add user:%s to repo:%s." % (username, component.name))
                    repo.add_user_to_repo(component.name, username)
        elif opt.delete:
            if reponame:
                put_string("Delete user:%s to repo:%s." % (username, reponame))
                repo.delete_user_to_repo(reponame, username)
            else:
                for component in yoc.occ_components:
                    put_string("Delete user:%s to repo:%s." % (username, component.name))
                    repo.delete_user_to_repo(component.name, username)
        return 0
