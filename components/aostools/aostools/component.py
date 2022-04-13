# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


from __future__ import print_function

import os
import sys
import json
import zipfile
import shutil
import threadpool
import codecs
import copy

from .tools import *
from .log import logger
from .package import *
from .gitproject import *

class Component:
    def __init__(self, conf, filename=''):
        self.conf = conf
        self.path = os.path.dirname(filename)
        self.name = os.path.basename(self.path)
        self.tag = ''
        self.license = ''
        self.type = ''
        self.version = ''
        self.depends = [] # [{'aos':'v7.3'},{'kv':'v7.4'}]
        self.description = ''
        self.historyVersion = {}
        self.updated = ''
        self.repo_url = ''
        self.repo = None
        self.source_files = []
        self.source_files_updated = [] # updated source files after conditional depends
        self.defconfig = {}
        self.build_config = None
        self.hw_info = None
        self.installs = []
        self.exports = []

        self.depends_on = []  # 该组件被哪个组件依赖,组件句柄列表
        self.need_build = True
        self.loaded = False

        self.conditions = []



    def check_file_integrity(self):
        readme_file = os.path.join(self.path, 'README.md')
        package_file = os.path.join(self.path, 'package.yaml')
        if os.path.isfile(readme_file) and os.path.isfile(package_file):
            return True
        return False

    def check_git_integrity(self):
        if self.check_file_integrity():
            git_path = os.path.join(self.path, '.git')
            if os.path.exists(git_path):
                return True
        return False

    def json_dumps(self):
        dep_names = []
        for item in self.depends:
            if type(item) == dict:
                dep_names.append(list(item.keys())[0])
            else:
                dep_names.append(item)
        js = {
            'name': self.name,
            'description': self.description,
            'versions': self.version,
            'license': self.license,
            'type': self.type,
            'depends': dep_names,
        }
        return json.dumps(js, ensure_ascii=False)

    # load js from git/occ
    def loader_json(self, js):
        self.js = js
        self.name = js['name']
        self.depends = js['depends']
        self.description = js['description']
        self.version = js['versions'] # FIXME:
        self.type = js['type']
        self.repo_url = js['repo_url']

        if 'license' in js:
            self.license = js['license']
        if 'updated' in js:
            self.updated = js['updated']

        if 'historyVersion' in js:
            for ver in js['historyVersion']:
                self.historyVersion[ver['version']] = ver['url']

        self.path = os.path.join(get_cmpt_path_by_type(self.type), self.name)

    def download(self, branch=''):
        if True:
            if self.version:
                branch = self.version
                v = branch.split('?')
                if len(v) > 1:
                    branch = v[0].strip()
            elif not branch:
                branch = 'master'
            # if not self.conf.repo.endswith('.git'):
            #     repo_url = os.path.join(self.conf.repo, '%s.git' % self.name)
            if not self.repo_url:
                put_string('Component repo url "%s" is not existed.' % self.name)
                return

            git = GitRepo(self.path, self.repo_url)
            pull_str = "clone"
            if self.check_git_integrity():
                active_branch = git.GetActiveBranch()
                if branch == active_branch:
                    put_string('Component %s is installed, skip to install it. Syncing...' % self.name)
                    # case 1: same branch or tag name
                    git.sync()
                    return
                else:
                    pull_str = "pull"
            # 如果找不到对应的branch，则clone master分支
            branches = git.GetRemoteBranches('origin')
            tags = git.GetRemoteTags('origin')
            if len(branches) == 0:
                if len(tags) == 0:
                    put_string('There is no branch/tag found for component:%s in remote.' % self.name)
                    return
                branches = tags
            if branch not in branches and branch not in tags:
                put_string("%-16s(%s->master), %s %s ..." % (self.name, branch, pull_str, self.repo_url))
                branch = 'master'
            else:
                put_string("%-16s(%s), %s %s ..." % (self.name, branch, pull_str, self.repo_url))
            if pull_str == "clone" or branch in tags:
                # case 2: download a new git 
                # case 3: switch to different tag
                git.pull(branch, None)
            else:
                # case 4: switch to different branch
                git.CheckoutBranch(branch)

        elif self.version in self.historyVersion.keys():
            zip_url = self.historyVersion[self.version]
            filename = http_get(zip_url, os.path.join(yoc_path, '.cache'))
            zipf = zipfile.ZipFile(filename)
            if self.path != '.':
                zipf.extractall('components/')
            else:
                zipf.extractall('.')

    def upload(self, repo_base_url=''):
        if not self.check_file_integrity():
            put_string("There is no `README.md` file found in %s component,please add it first." % self.name)
            exit(-1)
        repo_url = os.path.join(self.conf.repo, '%s.git' % self.name)
        if self.conf.repo.endswith('.git'):
            a = self.conf.repo.split('/')[-1]
            b = self.conf.repo[:-(len(a) + 1)]
            repo_url = os.path.join(b, '%s.git' % self.name)
        if repo_base_url:
            repo_url = os.path.join(repo_base_url, '%s.git' % self.name)

        try:
            git = GitRepo('/tmp/' + self.name)
            git.set_remote(repo_url)
            branch = self.version
            git.pull(branch)
            git.import_path(self.path, branch)
        except:
            put_string("Component `%s` upload fail." % self.name)

        try:
            shutil.rmtree('/tmp/' + self.name)
        except Exception as e:
            put_string("%s" % str(e))
            exit(-1)

    def zip(self, path):
        zipName = os.path.join(
            path, '.cache', self.name + '-' + self.version + '.zip')
        if os.path.exists(zipName):
            os.remove(zipName)
        zip_path(self.path, zipName)

        return zipName

    def show(self, indent=0, key=[]):
        if os.path.isdir(self.path):
            status = '*'
        else:
            status = ' '

        # s1 = self.name + ' (' + self.version + ')'
        s1 = self.name
        size = len(s1)

        text1, text2 = string_strip(self.description, 80)
        put_string("%s%s %s %s - %s" %
                   (' '*indent, status, s1, ' ' * (40 - size), text1), key=key)
        while text2:
            text1, text2 = string_strip(text2, 80)
            put_string(' ' * (46 + indent) + text1, key=key)

    def info(self, indent=0, show_path=False):
        for f in self.source_files_updated:
            if show_path:
                put_string('%s%s' % (' '*indent, os.path.join(self.path, f)))
            else:
                put_string('%s%s' % (' '*indent, f))

    def load_package(self):
        # if self.type:
        #     return False

        if self.loaded:
            return True

        # if not self.check_file_integrity():
        #     exit(-1)

        filename = os.path.join(self.path, 'package.yaml')
        if not os.path.isfile(filename):
            return False
        pack = Package(filename)
        self.pack = pack
        if os.path.basename(self.path) != pack.name:
            logger.warning(
                "component `%s`, but the directory is `%s`." % (pack.name, filename))

        self.name = pack.name
        self.version = pack.version
        self.type = pack.type
        self.tag = pack.tag
        self.license = pack.license
        self.description = pack.description
        self.source_files = pack.source_file
        self.build_config = copy.deepcopy(pack.build_config)
        self.defconfig = copy.deepcopy(pack.defconfig)
        self.installs = copy.deepcopy(pack.install)
        self.exports = copy.deepcopy(pack.export)
        self.hw_info = copy.deepcopy(pack.hw_info)
        self.hw_info.update_path(self.path)
        self.need_build = self.type not in ['board', 'chip']

        self.depends = []
        for d in pack.depends:
            if type(d) == dict:
                self.depends.append(d)

        self.loaded = True
        return True

    def variable_convert(self, varList):
        # include
        incs = []
        for inc in self.build_config.include:
            inc = varList.convert(inc)
            if inc != None:
                path = os.path.join(self.path, inc)
                if not (os.path.isdir(path) and os.path.exists(path)):
                    logger.warning('%s is not exists or not directory.' % path)
                if path not in incs:
                    incs.append(path)
        self.build_config.include_updated = incs

        # libpath
        libpaths = []
        for var in self.build_config.libpath:
            var = varList.convert(var)
            if var != None:
                path = os.path.join(self.path, var)
                if not (os.path.isdir(path) and os.path.exists(path)):
                    logger.warning('%s is not exists or not directory.' % path)
                if path not in libpaths:
                    libpaths.append(path)
        self.build_config.libpath_updated = libpaths

        # internal_include
        internal_include = []
        for var in self.build_config.internal_include:
            var = varList.convert(var)
            if var != None:
                path = os.path.join(self.path, var)
                if not (os.path.isdir(path) and os.path.exists(path)):
                    logger.warning('%s is not exists or not directory.' % path)
                if path not in internal_include:
                    internal_include.append(path)
        self.build_config.internal_include_updated = internal_include

        # libs
        libs = []
        for lib in self.build_config.libs:
            lib = varList.convert(lib)
            if lib != None and lib not in libs:
                libs.append(lib)
        self.build_config.libs_updated = libs

        # depend:
        depends = []
        for dep in self.depends:
            d = dep
            if type(dep) == dict:
                for k, v in dep.items():
                    d = "{}: {}".format(k, v)
            s = varList.convert(d)
            if s:
                if type(dep) == dict:
                    depends.append({s.split(':')[0]: list(dep.values())[0]})
                else:
                    depends.append(s)
        self.depends = depends  # depend was updated in check_conditions()

        # sources
        sources = []
        for s in self.source_files:
            fn = varList.convert(s)
            if fn != None:
                filename = os.path.join(self.path, fn)
                if not os.path.isfile(filename):
                    if not ('*' in filename or '?' in filename):
                        logger.error('component `%s`: %s is not exists.' %
                                     (self.name, filename))
                        exit(-1)
                sources.append(fn)
        self.source_files_updated = sources

        # script
        def script_filename_convert(root_path, script_file):
            script_filename = varList.convert(script_file)
            if script_filename != None:
                script_file = os.path.join(root_path, script_filename)
            return script_file
        
        self.build_config.prebuild_script = script_filename_convert(self.path, self.build_config.prebuild_script)
        self.build_config.postbuild_script = script_filename_convert(self.path, self.build_config.postbuild_script)
        self.build_config.postimage_script = script_filename_convert(self.path, self.build_config.postimage_script)

        def export_convert(convert):
            exports = []
            for ins in convert:
                if ('source' not in ins) and ('dest' not in ins):
                    continue
                if not ins['source']:
                    continue
                srcs = []
                for src in ins['source']:
                    src = varList.convert(src)
                    if src:
                        srcs.append(src)
                dest = varList.convert(ins['dest'])
                if dest and src:
                    exports.append({'dest': dest, 'source': srcs})
            return exports

        # install
        self.installs = export_convert(self.installs)

        # export
        self.exports = export_convert(self.exports)

    def install(self, dest):
        for ins in self.installs:
            path = os.path.join(dest, ins['dest'])
            if not os.path.exists(path):
                os.makedirs(path)

            for src in ins['source']:
                src = os.path.join(self.path, src)
                for s in glob.iglob(src):
                    fn = os.path.basename(s)
                    ds = os.path.join(path, fn)
                    shutil.copy2(s, ds)

    def export(self):
        for ins in self.exports:
            path = ins['dest']
            if not os.path.exists(path):
                os.makedirs(path)

            for src in ins['source']:
                src = os.path.join(self.path, src)
                for s in glob.iglob(src):
                    fn = os.path.basename(s)
                    ds = os.path.join(path, fn)
                    shutil.copy2(s, ds)

    def rename(self, new_name):
        if new_name == self.name:
            return
        old_path = self.path
        new_path = os.path.join(os.path.dirname(self.path), new_name)
        try:
            os.rename(old_path, new_path)
            filename = os.path.join(new_path, 'package.yaml')
            with codecs.open(filename, 'r', 'UTF-8') as fh:
                lines = fh.readlines()
                for i in range(len(lines)):
                    text = lines[i]
                    if text[0:5] == 'name:':
                        lines[i] = text.replace(self.name, new_name)
                        break
            with codecs.open(filename, 'w', 'UTF-8') as fh:
                fh.writelines(lines)
            self.path = new_path
            self.name = new_name

            new_repo = os.path.join(self.conf.repo, '%s.git' % new_name)
            if self.conf.repo.endswith('.git'):
                a = self.conf.repo.split('/')[-1]
                b = self.conf.repo[:-(len(a) + 1)]
                new_repo = os.path.join(b, '%s.git' % new_name)
            if os.path.exists(os.path.join(self.path, '.git')):
                GitRepo(new_path, new_repo)

            return True
        except Exception as e:
            put_string(e)
            return False

    def copy(self, new_name):
        if new_name == self.name:
            return
        old_path = self.path
        new_path = os.path.join(os.path.dirname(self.path), new_name)
        try:
            shutil.copytree(old_path, new_path)
            filename = os.path.join(new_path, 'package.yaml')
            with codecs.open(filename, 'r', 'UTF-8') as fh:
                lines = fh.readlines()
                for i in range(len(lines)):
                    text = lines[i]
                    if text[0:5] == 'name:':
                        lines[i] = text.replace(self.name, new_name)
                        break
            with codecs.open(filename, 'w', 'UTF-8') as fh:
                fh.writelines(lines)
            self.path = new_path
            self.name = new_name
            if os.path.exists(os.path.join(self.path, '.git')):
                shutil.rmtree(os.path.join(self.path, '.git')) 

            return True
        except Exception as e:
            put_string(e)
            return False


class ComponentGroup(list):
    def __init__(self):
        list.__init__([])
        self.components = {}

    def add(self, component):
        not_exists = component.name not in self.components
        if not_exists:
            self.append(component)
            self.components[component.name] = component
        return not_exists

    def get(self, name):
        for c in self:
            if c.name == name:
                return c

    def remove(self, name):
        for c in self:
            if c.name == name:
                del c
                break

    def show(self, indent=0):
        for c in self:
            c.show(indent)

    def download_all(self):
        def thread_execture(component):
            component.download()

        components = []
        for component in self.components:
            components.append(component)

        task_pool = threadpool.ThreadPool(5)
        requests = threadpool.makeRequests(thread_execture, components)
        for req in requests:
            task_pool.putRequest(req)
        task_pool.wait()

    def get_depend(self, component, exclude_names=[], cond_break=False, print_depends=False):
        arr_non = []
        dep_non = []
        depends_txt = []
        def _check_depend(component):
            component.load_package()

            for dep_dict in component.depends:
                name = list(dep_dict.keys())[0]
                cond = list(dep_dict.values())[0]
                v = cond.split('?')
                if name not in exclude_names:
                    if not self.get(name):
                        if name not in arr_non:
                            put_string("There is no depend component `%s` found!" % name)
                            arr_non.append(name)
                            dep_non.append(dep_dict)

                    c = self.components.get(name)
                    if c:
                        if cond_break:
                            v = cond.split('?')
                            if len(v) > 1:
                                continue
                        if c not in depends:
                            depends.append(c)
                        # print depend chain
                        if print_depends:
                            if len(v) > 1:
                                depend_str = "  %s->\t%s ? %s" % ('{0: <32}'.format(component.name), name, v[1])
                            else:
                                depend_str = "  %s->\t%s" % ('{0: <32}'.format(component.name), name)
                            if depend_str not in depends_txt:
                                depends_txt.append(depend_str)

                        if component not in c.depends_on:
                            c.depends_on.append(component)
                        _check_depend(c)

        depends = ComponentGroup()

        _check_depend(component)
        if print_depends:
            put_string("depend chain information is as below:")
            depends_txt.sort()
            for item in depends_txt:
                put_string(item)

        return depends, dep_non

def string_strip(text, size):
    L = 0
    R = ''
    i = 0
    if sys.version_info.major == 2:
        if type(text) not in (str, unicode):
            text = str(text)
    else:
        if type(text) != str:
            text = str(text)
    for c in text:
        if c >= '\u4E00' and c <= '\u9FA5':
            # put_string(c)
            L += 2
        else:
            # put_string('  ', c)
            L += 1
        R += c
        i += 1
        if L >= size:
            break
    return R, text[i:]


def version_compr(a, b):
    if b[:2] == '>=':
        return a >= b[2:]
    if b[:1] == '>':
        return a > b[1:]
    return a == b
