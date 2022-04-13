# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


from __future__ import print_function

import threadpool, re
from aostools import *


class Install(Command):
    common = True
    helpSummary = "Install component into project environment"
    helpUsage = """
%prog [option] [<component>...]
"""
    helpDescription = """
Install component into project environment
"""

    def _Options(self, p):
        self.jobs = 1
        p.add_option('-j', '--jobs',
                     dest='jobs', action='store', type='int',
                     help="projects to fetch simultaneously (default %d)" % self.jobs)
        p.add_option('-f', '--force',
                     dest='force', action='store_true',
                     help='install component force if exist already')
        p.add_option('-b', '--branch',
                     dest='branch', action='store', type='string',
                     help='the branch for component to download')
        p.add_option('-L', '--local',
                     dest='local', action='store_true',
                     help='install component from local zip file')

    def Execute(self, opt, args):
        if opt.local:
            return self.local_comp(opt, args)
        else:
            return self.remote_comp(opt, args)

    def local_comp(self, opt, args):
        yoc = YoC()
        if len(args) <= 0:
            put_string("No component file provided!")
            return 1
        
        pattern = re.compile(r'(.*)-(.*)\.zip')
        for zipfile in args:
            if not os.path.exists(zipfile):
                put_string("component file \"%s\" is not exist!" % zipfile)
                continue
            match = pattern.match(os.path.basename(zipfile))
            if match:
                name = match.group(1)
                version = match.group(2)
                component = yoc.components.get(name)
                if component:
                    if opt.force:
                        if unzip_path(yoc.yoc_path, zipfile):
                            put_string("install component \"%s\" successful" % name)
                    else:
                        put_string("Component `%s` have installed already! Please add -f option to install force!" % name)
                else:
                    if unzip_path(yoc.yoc_path, zipfile):
                        put_string("install component \"%s\" successful" % name)
            else:
                put_string("invalid component zip file \"%s\", it should be <name>-<version>.zip" % zipfile)
        return 0


    def remote_comp(self, opt, args):
        if opt.jobs:
            jobs = opt.jobs
        else:
            jobs = 12
        put_string("Start to install components...")
        yoc = YoC()
        components = ComponentGroup()
        if len(args) > 0:
            for name in args:
                update = False
                if name == args[0]:
                    update = True
                cmpt = yoc.check_cmpt_download(name, update=True, force=opt.force)
                if cmpt:
                    components.add(cmpt)
        else:
            yoc.update()
            components = yoc.occ_components
        
        if len(components) > 0:
            dled_cmpts =[]
            while len(components) > 0:
                cmpts = components
                self.download(jobs, cmpts, opt.branch)
                for c in cmpts:
                    if c.name not in dled_cmpts:
                        dled_cmpts.append(c.name)
                components = ComponentGroup()
                for c in cmpts:
                    ret = c.load_package()
                    if ret:
                        yoc.update_version(c.depends)
                        cmpt_list = self.get_need_download_cmpts(args, dled_cmpts, c.depends)
                        validComponentList = []
                        for component in yoc.occ_components:
                            if component.name in cmpt_list:
                                components.add(component)
                                validComponentList.append(component.name)
                        invalidComponentList = set(cmpt_list).difference(set(validComponentList))
                        if invalidComponentList:
                            put_string("warning: These components [%s] are not existed!" % (", ").join(invalidComponentList))

            put_string('Download components finish.')
            return 0
        else:
            put_string("No component need to install.")
            return 1

    def get_need_download_cmpts(self, origin_list, downloaded_list, depends):
        cmpt_list = []
        for name in depends:
            if type(name) == dict:
                name = list(name.keys())[0]
                if (name not in origin_list) and (name not in downloaded_list):
                    cmpt_list.append(name)
        return cmpt_list

    def download(self, jobs, components, branch):
        task_pool = threadpool.ThreadPool(jobs)
        tasks = []
        for component in components:
            tasks.append(component)

        def thread_execture(component):
            component.download(branch)

        requests = threadpool.makeRequests(thread_execture, tasks)
        for req in requests:
            task_pool.putRequest(req)
        task_pool.wait()
        task_pool.dismissWorkers(jobs, do_join=True)
