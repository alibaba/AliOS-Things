# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


from __future__ import print_function
from aostools import *


class Upload(Command):
    common = True
    helpSummary = "Upload components to git or OCC"
    helpUsage = """
%prog [option] [<component>...]
"""
    helpDescription = """
Upload components to git or OCC
"""

    def _Options(self, p):
        p.add_option('-g', '--git',
                     dest='upload_gitee', action='store_true',
                     help='upload code to git repo')
        p.add_option('-o', '--occ',
                     dest='upload_occ', action='store_true',
                     help='upload code to OCC')
        p.add_option('-e', '--aone',
                     dest='upload_aone', action='store', type='string',
                     help='upload code to code.aone')
        p.add_option('-p', '--prefix',
                     dest='prefix', action='store', type='string',
                     help="specify the component name's prefix")
        p.add_option('-t', '--type',
                     dest='type', action='store', type='string',
                     help="specify the component type")

    def Execute(self, opt, args):
        if not (opt.upload_gitee or opt.upload_occ or opt.upload_aone):
            self.Usage()
            return 1

        yoc = YoC()
        count = len(args)
        if opt.upload_gitee:
            repo = RepoGitee(yoc.conf.gitee_token, yoc.conf.group)
            if repo:
                if count == 0:
                    put_string("Uploading all components, please wait...")
                flag = False
                for component in yoc.components:
                    if component.name in args or count == 0:
                        flag = True
                        component.load_package()
                        ssh_url = repo.create_project(
                            component.name, component.json_dumps())
                        if ssh_url:
                            put_string("Uploading %s(%s), please wait..." % (component.name, component.version))
                            component.upload()
                            put_string("Upload %s finish." % component.name)
                        else:
                            put_string("Upload %s failed!" % component.name)
                if not flag:
                    put_string("Can't find components:%s!" % str(args))
            else:
                put_string("Connect git repo error!")

        elif opt.upload_occ:
            if count == 0:
                put_string("Uploading all components, please wait...")
                yoc.uploadall()
            else:
                for name in args:
                    put_string("Start to upload component:%s to OCC." % name)
                    yoc.upload(name)
        
        elif opt.upload_aone:
            if count == 0:
                put_string("Uploading all components, please wait...")
            flag = False
            for component in yoc.components:
                if (component.name in args) or count == 0:
                    flag = True
                    component.load_package()
                    if opt.prefix and not component.name.startswith(opt.prefix):
                        continue
                    if opt.type and component.type != opt.type:
                        continue
                    put_string("Uploading %s(%s), please wait..." % (component.name, component.version))
                    component.upload(opt.upload_aone)
                    put_string("Upload %s finish." % component.name)
            if not flag:
                put_string("Can't find components:%s!" % str(args))
        return 0
