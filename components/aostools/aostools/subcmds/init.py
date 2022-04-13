# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


from __future__ import print_function

import os
from aostools import *

class Init(Command):
    common = True
    helpSummary = "Initialize aos workspace in the current directory"
    helpUsage = """
%prog [repo] [option] <branch>
"""
    helpDescription = """
Initialize aos workspace in the current directory.
"""
    def _Options(self, p):
        p.add_option('-b', '--branch',
                     dest='branch', action='store', type='string',
                     help='the manifest repo branch')

    def Execute(self, opt, args):
        conf = Configure()
        if conf.init:
            put_string("Workspace is initialized already.")
            put_string("The workspace is `%s`" % conf.yoc_path)
            return 1
        if os.path.isfile("package.yaml"):
            put_string("Please intialize it in the workspace root directory.")
            return 1
        if os.path.isdir(os.path.join(conf.yoc_path, '.aos')):
            put_string("It is an invalid directory `%s`." % conf.yoc_path)
            return 1
        conf.gitee_token = "32e702f14950e51e3a230e7ca8a60f10"
        conf.occ_host = "occ.t-head.cn"
        conf.repo = "git@gitlab.alibaba-inc.com:alios_iot/manifest.git"
        conf.branch = "master"
        conf.need_auto_init = False
        # try:
        #     urlretrieve('https://yoctools.oss-cn-beijing.aliyuncs.com/yoc_new', '.aos')
        # except:
        #     pass
        if len(args) > 0:
            # conf.load(os.path.join(conf.yoc_path, '.aos'))
            repourl = args[0]
            repobr = 'master'
            if opt.branch:
                repobr = opt.branch
            conf.branch = repobr
            conf.repo = repourl
            conf.save()
        if not os.path.isfile('.aos'):
            conf.save()
        return 0
