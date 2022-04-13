# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


from __future__ import print_function

import threadpool
import time

from aostools import *


class Sync(Command):
    common = True
    helpSummary = "Update working tree to the latest revision"
    helpUsage = """
%prog [<component>...]
"""
    helpDescription = """
The '%prog' command synchronizes local component directories
with the remote repositories specified in the manifest.  If a local
component does not yet exist, it will clone a new local directory from
the remote repository and set up tracking branches as specified in
the manifest.  If the local component already exists, '%prog'
will update the remote branches and rebase any new local changes
on top of the new remote changes.

The --force-sync option can be used to overwrite existing git
directories if they have previously been linked to a different
object direcotry. WARNING: This may cause data to be lost since
refs may be removed when overwriting.

The --force-remove-dirty option can be used to remove previously used
projects with uncommitted changes. WARNING: This may cause data to be
lost since uncommitted changes may be removed with projects that no longer
exist in the manifest.
"""

    def _Options(self, p):
        self.jobs = 1
        p.add_option('--force-sync',
                     dest='force_sync', action='store_true',
                     help="overwrite an existing git directory if it needs to "
                     "point to a different object directory. WARNING: this "
                     "may cause loss of data")
        p.add_option('--force-remove-dirty',
                     dest='force_remove_dirty', action='store_true',
                     help="force remove projects with uncommitted modifications if "
                     "projects no longer exist in the manifest. "
                     "WARNING: this may cause loss of data")
        p.add_option('-j', '--jobs',
                     dest='jobs', action='store', type='int',
                     help="projects to fetch simultaneously (default %d)" % self.jobs)

    def Execute(self, opt, args):
        yoc = YoC()

        if opt.jobs:
            jobs = opt.jobs
        else:
            jobs = 4

        task_pool = threadpool.ThreadPool(jobs)
        if len(args) > 0:
            np = Progress('Fetching projects', len(args))
        else:
            np = Progress('Fetching projects', len(yoc.components))
        tasks = []
        validComponentList = []
        for component in yoc.components:
            if len(args) > 0:
                if component.name not in args:
                    continue
            component.np = np
            component.load_package()
            tasks.append(component)
            validComponentList.append(component.name)
        # specify component name by user
        if len(args) > 0:
            invalidComponentList = set(args).difference(set(validComponentList))
            if invalidComponentList:
                put_string("These components [%s] are not existed!" % (", ").join(invalidComponentList))

        def thread_execture(component):
            put_string("Syncing %s" % component.name)
            component.np.update()
            git = GitRepo(component.path, component.repo_url)
            if component.version:
                branch = component.version
            else:
                branch = 'master'
            git.pull(branch)

        requests = threadpool.makeRequests(thread_execture, tasks)
        for req in requests:
            task_pool.putRequest(req)
        task_pool.wait()
        task_pool.dismissWorkers(jobs, do_join=True)

        np.end()
        return 0
