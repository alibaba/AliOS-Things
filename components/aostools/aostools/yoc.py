# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


from __future__ import print_function
import os
import shutil
import pickle
import json
import codecs

from .tools import *
from .component import *
from .manifest import *
from .occ import *
from .log import logger
from .solution import *
from .repo import *


class Configure:
    def __init__(self):
        self.lastUpdateTime = 0
        self.gitlab_token = ''
        self.github_token = ''
        self.gitee_token = ''
        self.group = 'yocop'
        self.username = ''
        self.password = ''
        self.occ_host = 'occ.t-head.cn'
        self.repo = 'https://gitee.com/yocop/manifest.git'
        self.branch = 'master'       # manifest.git的branch
        self.init = False
        self.need_auto_init = False

        self.yoc_path = os.getcwd()
        host_os = get_host_os()
        if host_os == 'Win32':
            sys_root = re.compile(r'^[A-Za-z]{1}:\\$')
            if not self.yoc_path:
                self.yoc_path = 'C:\\'
        else:
            sys_root = re.compile('^/$')
            if not self.yoc_path:
                self.yoc_path = '/'
        tmp_path = self.yoc_path
        while not sys_root.match(tmp_path):
            f = os.path.join(tmp_path, '.aos')
            if os.path.isfile(f):
                self.yoc_path = tmp_path
                conf = yaml_load(f)
                if conf:
                    self.init = True
                    for k, v in conf.items():
                        if v:
                            self.__dict__[k] = v
                break
            tmp_path = os.path.dirname(tmp_path)
        # if can not find .aos file, find kernel/rhino/package.yaml (for user manually download source code)
        if not self.init:
            tmp_path = self.yoc_path
            while not sys_root.match(tmp_path):
                f = os.path.join(tmp_path, 'kernel', 'rhino', 'package.yaml')
                if os.path.isfile(f):
                    self.yoc_path = tmp_path
                    self.need_auto_init = True
                    break
                tmp_path = os.path.dirname(tmp_path)

        if self.repo:
            if self.repo.endswith('.git'):
                if self.repo.startswith('http'):    # like 'https://gitee.com/yocop/manifest.git'
                    self.group = self.repo.split('/')[-2]
                elif self.repo.startswith('git@'):  # like 'git@gitlab.alibaba-inc.com:yocopen/manifest.git'
                    self.group = self.repo.split(':')[-1].split('/')[-2]
                else:
                    put_string('The repo is wrong!')
            else:
                if self.repo.startswith('http'):    # like 'https://gitee.com/yocop'
                    self.group = self.repo.split('/')[-1]
                elif self.repo.startswith('git@'):  # like 'git@gitlab.alibaba-inc.com:yocopen'
                    self.group = self.repo.split(':')[-1]
                elif self.repo.startswith('ssh'):   # like 'ssh://<username>@yuncode.alibaba-inc.com:12345/aos/repo'
                    self.group = self.repo.split(':')[-1].split('/')[1]
                    self.username = self.repo.split('@')[0].split('/')[-1]
                else:
                    put_string('The repo is wrong!')
    def load(self, yoc_file):
        conf = yaml_load(yoc_file)
        if conf:
            for k, v in conf.items():
                self.__dict__[k] = v

    def save(self, yoc_file=None):
        if not yoc_file:
            yoc_file = os.path.join(self.yoc_path, '.aos')
        with codecs.open(yoc_file, 'w', 'UTF-8') as f:
            for k, v in self.__dict__.items():
                if k not in ['yoc_path', 'init', 'group', 'yoc_version'] and v:
                    f.write("{}: {}\n".format(k, v))
        self.init = True

    def search_pacakge_yaml(self, subpath=[], sub_prefix=[]):

        def traversalDir_FirstDir(path):
            list = []
            if (os.path.exists(path)):
                files = os.listdir(path)
                for file in files:
                    m = os.path.join(path,file)
                    if (os.path.isdir(m)):
                        h = os.path.split(m)
                        list.append(h[1])
                return list

        paths = []
        if subpath or sub_prefix:
            if subpath:
                for sub in subpath:
                    p = os.path.join(self.yoc_path, sub)
                    if os.path.exists(p):
                        paths.append(p)
            if sub_prefix:
                first_dir = traversalDir_FirstDir(self.yoc_path)
                for d in first_dir:
                    for sub in sub_prefix:
                        if d.startswith(sub):
                            p = os.path.join(self.yoc_path, d)
                            paths.append(p)
        else:
            paths.append(self.yoc_path)

        package_list = []

        while paths:
            path = paths[0]
            filename = os.path.join(path, 'package.yaml')
            if os.path.isfile(filename):
                package_list.append(filename)
            else:
                files = os.listdir(path)
                for file in files:
                    p = os.path.join(path, file)
                    if os.path.isdir(p):
                        paths.append(p)
            del paths[0]
        return package_list


class YoC:
    def __init__(self):
        self.occ = None
        self.occ_components = None
        self.conf = Configure()
        self.yoc_path = self.conf.yoc_path

        try:
            compenent_db = os.path.join(self.yoc_path, '.components.db')
            with open(compenent_db, "rb") as f:
                self.occ_components = pickle.load(f)
                if len(self.occ_components) == 0:
                    self.occ_components = None
        except:
            self.occ_components = None

        if not self.occ_components:
            self.conf.lastUpdateTime = 0

        # scanning yoc all components
        self.components = ComponentGroup()
        package_yamls = self.conf.search_pacakge_yaml(
            ['hardware', 'kernel', 'components', 'examples', 'test', 'documentation'], ['solutions'])

        # filename = os.path.join(os.getcwd(), 'package.yaml')
        # if os.path.isfile(filename):
        #     package_yamls.append(filename)

        filename = os.path.join(os.getcwd(), 'package.yaml')
        if os.path.isfile(filename):
            pack = Package(filename)
            if pack.type == 'solution':
                if filename not in package_yamls:
                    package_yamls.append(filename)

        for filename in package_yamls:
            pack = Component(self.conf, filename)
            if not self.components.add(pack):
                pre_component = self.components.get(pack.name)
                put_string('Component `%s` is multiple, first defined in :%s, redifned here: %s, please check!)' %
                             (pack.name, pre_component.path, pack.path))
                exit(0)

    def clone_manifest(self, is_all=False):
        manifest_path = os.path.join(self.yoc_path, ".repo")
        if not os.path.exists(manifest_path):
            os.mkdir(manifest_path)
        repo_url = self.conf.repo
        prj = GitRepo(manifest_path, repo_url)
        if is_all:
            # if checkout master first time, it will switch to master automatically
            prj.pull('master', None)
            if self.conf.branch and (self.conf.branch != prj.GetActiveBranch()):
                # switch back to conf.branch and merge from master
                prj.CheckoutBranch(self.conf.branch)
                prj.pull('master', None)
        else:
            prj.pull(self.conf.branch, None)

    def manifest_yaml_parse(self, filename=None):
        if not filename:
            filename = os.path.join(self.yoc_path, ".repo/default.yaml")
        if os.path.isfile(filename):
            mani = Manifest(filename)
            return mani
        else:
            put_string("Can't find %s" % filename)
            put_string("Maybe there is no branch(%s) in %s" % (self.conf.branch, self.conf.repo))

    def get_name_list(self, group):
        name_list = []
        for c in group:
            name_list.append(c.name)
        return name_list

    def check_depend(self, component):
        def _check_depend(component):
            component.load_package()

            for name in component.depends:
                if type(name) == dict:
                    name = list(name.keys())[0]
                c = self.components.get(name)
                if c:
                    if c not in depend_cmpts:
                        depend_cmpts.append(c)
                    if component not in c.depends_on:
                        c.depends_on.append(component)
                    _check_depend(c)

        depend_cmpts = ComponentGroup()

        _check_depend(component)
        return depend_cmpts

    def check_depend_on(self, component):
        depends_on = ComponentGroup()
        for c in self.components:
            c.load_package()
            for d in c.depends:
                if component.name in d:
                    depends_on.append(c)
        return depends_on

    def check_conditions(self, components=[], exclude_names=[]):
        cmpts = ComponentGroup()
        for comp in components:
            for dep_dict in comp.depends:
                name = list(dep_dict.keys())[0]
                cond = list(dep_dict.values())[0]
                v = cond.split('?')
                if len(v) > 1:
                    x = re.search('<(.+?)>', v[1], re.M | re.I)
                    if x:
                        cond = x.group(1)
                        c = self.components.get(name)
                        if c:
                            c.conditions.append(cond)
                            cmpts.add(c)
                            # print("comp %s has depends %s with cond %s" % (comp.name, name, cond))
        # 添加所有依赖于此组件的条件
        for comp in cmpts:
            # print("ck1##%s" % comp.name)
            for c in comp.depends_on:
                if c in cmpts:
                    for cond1 in c.conditions:
                        if cond1 not in comp.conditions:
                            comp.conditions.append(cond1)
                            # print("comp %s inherit cond %s from parent %s" % (comp.name, cond1, c.name))
        # 添加所有依赖的组件的条件
        for comp in cmpts:
            depends, _ = self.components.get_depend(comp, exclude_names)
            for dep in depends:
                # print("ck2##%s" % dep.name)
                for cond1 in comp.conditions:
                    if cond1 not in dep.conditions:
                        dep.conditions.append(cond1)
                        # print("depend %s inherit cond %s from comp %s" % (dep.name, cond1, comp.name))
        # 去除强依赖的组件的条件数组
        for comp in components:
            if comp not in cmpts and (len(comp.conditions) == 0):
                depends, _ = self.components.get_depend(comp, exclude_names, cond_break=True)
                for dep in depends:
                    dep.conditions = []
                    # print("comp %s has depend %s directly, clear %s condition" %(comp.name, dep.name, dep.name))


    def getSolution(self, board_name=None, compiler='gcc', print_depends=False):
        for component in self.components:
            if component.path == os.getcwd():
                component.load_package()
                if board_name:
                    if self.components.get(board_name):
                        component.hw_info.board_name = board_name
                    else:
                        put_string("Board component `%s` not found in current workspace, please install it: aos install %s " %(board_name, board_name))
                        exit(-1)
                # 根据指定的board_name过滤其他的board组件
                exclude_names = []
                for name in component.depends:
                    if type(name) == dict:
                        name = list(name.keys())[0]
                    c = self.components.get(name)
                    if c:
                        c.load_package()
                        if c.type == 'board' and name != component.hw_info.board_name:
                            exclude_names.append(name)

                components, lost = self.components.get_depend(component, exclude_names, print_depends=print_depends)
                if lost:
                    self.auto_install_missing_comp(lost)
                    put_string("Please try it again.")
                    exit(-1)
                components.add(component)
                # depends and condition
                self.check_conditions(components, exclude_names)
                solution = Solution(components, compiler)

                return solution

    def auto_install_missing_comp(self, lost_comps):
        put_string("Try to install missing component...")
        for comp_dict in lost_comps:
            name = list(comp_dict.keys())[0]
            cond = list(comp_dict.values())[0]
            v = cond.split('?')
            if len(v) > 1:
                cond = v[0]
            os.system("aos install %s -b %s" %(name, cond))

    def check_cmpt_download(self, name, update=True, force=False):
        if self.components.get(name) and not force:
            put_string("Component `%s` have installed already! Please add -f option to install force!" % name)
        else:
            if update:
                self.update()
            if self.occ_components:
                component = self.occ_components.get(name)
                if component:
                    return component
                else:
                    put_string("Can't find component %s." % name)
            else:
                put_string("There is no component found from server!")
        return None

    def download_component(self, name, update=True, force=False):
        if self.components.get(name) and not force:
            put_string("Component `%s` have installed already! Please add -f option to install force!" % name)
            return None
        if self.components.get(name) == None or force:
            if update:
                # self.occ_update()
                self.gitee_update()

            component = self.occ_components.get(name)
            if component:
                depends, _ = self.occ_components.get_depend(component)
                depends.add(component)

                return depends
            else:
                put_string("There is no component `%s` found from repo!" % name)

    def remove_component(self, name):
        component = self.components.get(name)
        if component:
            if not component.depends_on:                     # 如果没有组件依赖它
                for n in component.depends:
                    if type(n) == dict:
                        n = list(n.keys())[0]
                    p = self.components.get(n)
                    if p:
                        if name in p.depends_on:
                            del p.depends_on[name]
                        self.remove_component(n)

                shutil.rmtree(component.path)
                self.components.remove(component)
                return True
            else:
                logger.info("remove fail, %s depends on:" % component.name)
                for dep in component.depends_on:
                    logger.info('  ' + dep.name)
        else:
            put_string("The component \"%s\" is not existed!" % name)
        return False

    def occ_login(self):
        if self.occ == None:
            self.occ = OCC(self.conf)
        if not self.occ.login():
            put_string("Login OCC failed. Please check your username and password.")
            return False
        return True

    def upload(self, name):
        component = self.components.get(name)
        if component:
            component.load_package()
            version = component.version
            if version:
                if not os.path.isdir(os.path.join(component.path, '.git')):
                    if self.occ == None:
                        self.occ = OCC(self.conf)
                    self.occ.login()
                    zip_file = component.zip(self.yoc_path)
                    if self.occ.upload(version, component.type, zip_file) == 0:
                        put_string("Component %s(%s) upload success!" % (component.name, version))
                    else:
                        put_string("Component %s(%s) upload failed!" % (component.name, version))
                else:
                    put_string("It is a git repo,abort to upload.")
            else:
                put_string("Component %s version is empty!" % (component.name))

    def uploadall(self):
        if self.occ == None:
            self.occ = OCC(self.conf)
        self.occ.login()
        for component in self.components:
            component.load_package()
            version = component.version
            if version:
                zip_file = component.zip(self.yoc_path)
                if self.occ.upload(version, component.type, zip_file) == 0:
                    put_string("Component %s(%s) upload success!" % (component.name, version))
                else:
                    put_string("Component %s(%s) upload failed!" % (component.name, version))
            else:
                put_string("Component %s version is empty!" % (component.name))

    def update(self, is_all=False):
        def get_repo_url(remotes, cmp_ele):
            if cmp_ele.remote.startswith('https://') \
                or cmp_ele.remote.startswith('http://') \
                or cmp_ele.remote.startswith('ssh://') \
                or cmp_ele.remote.startswith('git@'):
                return cmp_ele.remote.replace("<username>", self.conf.username)
            else:
                for r in remotes:
                    if cmp_ele.remote == r.name:
                        repo_url = '%s/%s.git' % (r.remote, cmp_ele.name)
                        return repo_url.replace("<username>", self.conf.username)
        def get_cmpt_path(name, type):
            return os.path.join(self.conf.yoc_path, get_cmpt_path_by_type(type), name)

        self.clone_manifest(is_all)
        self.default_mani = self.manifest_yaml_parse()
        if self.default_mani:
            self.occ_components = ComponentGroup()
            for p in self.default_mani.cmpt_list:
                cmp = Component(self.conf)
                cmp.name = p.name
                cmp.type = p.type
                cmp.description = p.desc
                cmp.repo_url = get_repo_url(self.default_mani.remotes, p)
                cmp.path = get_cmpt_path(cmp.name, cmp.type)
                # print(cmp.name, cmp.type, cmp.repo_url, cmp.description, cmp.path)
                self.occ_components.add(cmp)
            with open(os.path.join(self.yoc_path, '.components.db'), "wb") as f:
                pickle.dump(self.occ_components, f)
                self.conf.save()

    def update_version(self, depends=[]):
        for component in self.occ_components:
            for d in depends:
                if type(d) == dict:
                    name = list(d.keys())[0]
                    if component.name == name:
                        component.version = list(d.values())[0]
                        # print("update %s version:%s" % (component.name, component.version))
                        break
            
    def gitee_update(self):
        self.occ_components = ComponentGroup()
        if not self.conf.gitee_token:
            put_string("Can't fetch from git repo, please check your `.aos` file.")
            return
        put_string("Updating from git...")
        repo = RepoGitee(self.conf.gitee_token, self.conf.group)
        for p in repo.projects():
            pack = Component(self.conf)
            if p:
                if type(p) == bytes:
                    p = bytes.decode(p)
                pack.loader_json(json.loads(p))
                pack.path = os.path.join(self.conf.yoc_path, pack.path)
                self.occ_components.add(pack)
        with open(os.path.join(self.yoc_path, '.components.db'), "wb") as f:
            pickle.dump(self.occ_components, f)
            self.conf.save()

    def occ_update(self):
        if self.occ == None:
            self.occ = OCC(self.conf)
        put_string("Updating from OCC...Please wait.")
        components, time = self.occ.yocComponentList('614193542956318720', self.conf.lastUpdateTime)
        put_string("Update from OCC over.")
        if components:
            self.occ_components = components
            self.conf.lastUpdateTime = time
            for component in self.occ_components:
                component.path = os.path.join(self.yoc_path, component.path)

            with open(os.path.join(self.yoc_path, '.components.db'), "wb") as f:
                pickle.dump(self.occ_components, f)
                self.conf.save()

    def list(self):
        for component in self.components:
            component.load_package()
            component.show()
