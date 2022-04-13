# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


from __future__ import print_function
from aostools import *

class Create(Command):
    common = True
    helpSummary = "Create project or component from templet"
    helpUsage = """
%prog [project|comp]
"""
    helpDescription = """
Create project or component from templet
"""


    def Execute(self, opt, args):
        self.Usage()