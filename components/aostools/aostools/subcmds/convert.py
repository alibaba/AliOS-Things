# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


from __future__ import print_function

from aostools import *
import os
import re
import shutil
import codecs

class Convert(Command):
    common = True
    helpSummary = "Directory to component"
    helpUsage = """
%prog <directory> [...]
"""
    helpDescription = """
Directory to component
"""

    def Execute(self, opt, args):
        if len(args) == 0:
            self.Usage()
        for dir_name in args:
            self.convert(dir_name)

    def convert(self, directory):
        if not os.path.exists(directory):
            print("Directory `%s` is not exists" % directory)
            return
        if os.path.isfile(os.path.join(directory, 'package.yaml')):
            print("Directory `%s` is already a component" % directory)
            return
        pack = Package()
        pack.name = os.path.basename(directory)
        pack.version = 'master'
        pack.description = ' '
        pack.type = get_type_by_cmpt_path(directory)
        pack.tag = ' '
        pack.license = 'Apache license v2.0'

        incs = []
        code_incs = {}
        global_incs = {}
        lib_path = {}

        list_dirs = os.walk(directory)
        for root, _, files in list_dirs:
            for f in files:
                f = os.path.join(root, f)
                relpath = os.path.relpath(f, directory)
                filename = os.path.basename(f)

                ext = os.path.splitext(filename)
                if ext[1] in ['.c', '.cxx', '.cpp']:
                    pack.source_file.append(relpath)
                    for fs in find_inc(f):
                        code_incs[fs] = True
                elif ext[1] in ['.h', '.hpp', '.hxx']:
                    incs.append(relpath)
                    for fs in find_inc(f):
                        code_incs[fs] = True
                elif ext[1] in ['.a'] and ext[0][:3] == 'lib':
                    pack.build_config.libs.append(ext[0][3:])
                    lib_path[os.path.dirname(relpath)] = True
                    pack.install.append({
                        'dest' : 'aos_sdk/lib',
                        'source' : [relpath]
                    })
        for k, _ in code_incs.items():
            for inc in incs:
                idx = inc.find(k)
                if idx > 0:
                    f = inc[:idx]
                    if os.path.isdir(os.path.join(directory, f)):
                        global_incs[f] = True

        for k, _ in global_incs.items():
            pack.build_config.include.append(k)
            pack.install.append({
                'dest' : k,
                'source' : [k + '*']
            })

        for p, _ in lib_path.items():
            pack.build_config.libpath.append(p)
        pack.save(os.path.join(directory, 'package.yaml'))


def find_inc(filename):
    with codecs.open(filename, 'r', 'UTF-8') as f:
        contents = f.read()
        pattern = re.findall("^[ \t]*#[ \t]*include[ \t]*[<\"][ \t]*([a-zA-Z0-9_./]+)[ \t]*[>\"]", contents, re.MULTILINE)
        return pattern
