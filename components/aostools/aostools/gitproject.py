# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


import os
import sys
import shutil
import git
import time

from .log import *
from .tools import put_string


class simpleProgressBar(git.RemoteProgress):
    def update(self, op_code, cur_count, max_count=None, message=''):
        # text = "%3d%% (%d/%d)" % (cur_count/(max_count or 100.0), cur_count, max_count)
        sys.stdout.write(self._cur_line)
        sys.stdout.flush()
        if op_code & self.END:
            sys.stdout.write('\n')
        else:
            sys.stdout.write('\r')


class pullProgressBar(git.RemoteProgress):
    def update(self, op_code, cur_count, max_count=None, message=''):
        sys.stdout.write(self._cur_line)
        sys.stdout.flush()
        if op_code & self.END:
            sys.stdout.write('\n')
        else:
            sys.stdout.write('\r')


class GitRepo:
    def __init__(self, path, repo_url=None):
        self.repo_url = repo_url
        self.path = path
        git_path = os.path.join(path, '.git')

        if not os.path.exists(git_path):  # 如果未下载，则 git clone 下来
            self.repo = git.Repo.init(path)
        else:
            try:
                self.repo = git.Repo(path)
            except Exception as ex:
                self.repo = git.Repo.init(path)

        if repo_url:
            self.set_remote(repo_url)

    def set_remote(self, repo_url):
        try:
            origin = self.repo.remote()
            origin.set_url(repo_url)
        except Exception as ex:
            origin = self.repo.create_remote(name='origin', url=repo_url)
            try:
                origin.fetch()
            except Exception as ex:
                info = str(ex)
                if info.find("Please make sure you have the correct access rights") > 0:
                    put_string("%s" % origin.url)
                    put_string("Please make sure you have the correct access rights and the repository exists")

    def pull(self, version='', progress=pullProgressBar()):
        try:
            if not version:
                version = self.repo.active_branch

            origin = self.repo.remote()

            if version not in self.repo.heads:
                if version not in origin.refs:
                    origin.fetch(progress=progress)
                if version in origin.refs:
                    branch = self.repo.create_head(version, origin.refs[version])
                    branch.set_tracking_branch(origin.refs[version])
                    branch.checkout()
                elif version in self.repo.tags:
                    git = self.repo.git
                    git.checkout(version, b=version)         # create a new branch, name same as tag
            else:
                origin.pull(version)
                self.repo.heads[version].set_tracking_branch(origin.refs[version])

        except Exception as ex:
            info = str(ex)
            if info.find('Please make sure you have the correct access rights') > 0:
                put_string("%s" % origin.url)
                put_string("Please make sure you have the correct access rights and the repository exists.")
            # put_string("\nPull %s occur error:(%s)" % (origin.url, str(ex)), level='warning')
            pass

    def fetch(self, remote="origin"):
        try:
            remote = self.repo.remote(remote)
        except ValueError:
            msg = "Remote {remote} does not exist on repo {repo}".format(
                remote=remote,
                repo=self.repo.repo.working_dir
            )
            logger.error(msg)
        try:
            remote.fetch(progress=pullProgressBar())
        except git.GitCommandError as ex:
            info = str(ex)
            if info.find('Please make sure you have the correct access rights') > 0:
                put_string("%s" % remote.url)
                put_string("Please make sure you have the correct access rights and the repository exists.")
                # print("Use: `yoc addkey`")


    def import_path(self, path, version):
        files = os.listdir(self.repo.working_dir)
        for f in files:
            if f != '.git':
                fn = os.path.join(self.repo.working_dir, f)
                if os.path.isdir(fn):
                    shutil.rmtree(fn)
                else:
                    os.remove(fn)

        for dirpath, _, filenames in os.walk(path):
            if dirpath.find(os.path.join(path, '.git')) < 0:
                for f in filenames:
                    p1 = os.path.join(dirpath, f)
                    p2 = os.path.relpath(p1, path)
                    p2 = os.path.join(self.repo.working_dir, p2)
                    try:
                        p = os.path.dirname(p2)
                        os.makedirs(p)
                    except:
                        pass

                    shutil.copy2(p1, p2)

        if self.repo.is_dirty(untracked_files=True):
            self.repo.git.add(self.repo.untracked_files)
            self.repo.git.commit('-m', 'init version', '-a')

            branch = self.repo.create_head(version)
            branch.checkout()

            self.repo.git.push(
                "--set-upstream", self.repo.remotes.origin, self.repo.head.ref)

    def GetRemoteBranches(self, remote='origin'):
        br_arr = []
        try:
            branches = self.repo.remote().refs
            for b in branches:
                if remote and remote in b.name:
                    br_arr.append(''.join(b.name.split('/')[1:]))
                else:
                    br_arr.append(b.name)
        except:
            pass
        return br_arr

    def GetRemoteTags(self, remote=''):
        origin = self.repo.remote()
        origin.fetch("--tags")
        return self.repo.tags

    def CheckoutBranch(self, version):
        git = self.repo.git
        try:
            origin = self.repo.remote()
            if version not in origin.refs:
                git.checkout('HEAD', b=version)
            else:
                git.checkout(version)
        except Exception as ex:
            put_string(str(ex))
            pass
        self.pull(version)

    def GetActiveBranch(self):
        return self.repo.active_branch.name

    def sync(self):
        name = os.path.basename(self.path)
        if self.repo.is_dirty(untracked_files=False):
            put_string("There are modified files in %s, please commit or stash first." % name)
            return
        self.pull()

    def status(self):
        name = os.path.basename(self.path)
        changed = [ item.a_path for item in self.repo.index.diff(None) ]
        untracked_files = self.repo.untracked_files
        if len(changed) > 0 or len(untracked_files) > 0:
            put_string("%s: on branch %s" % (name, self.repo.active_branch))
        for m in changed:
            put_string("        modified:   %s" % m)
        for uf in untracked_files:
            put_string("        %s" % uf)
        # git = self.repo.git
        # statusinfo = git.status()
        # if statusinfo.find("Changes not staged for commit:") > 0 or statusinfo.find("Untracked files:") > 0:
        #     put_string("%s:" % name)
        #     put_string("%s" % statusinfo)

    def gitlog(self):
        name = os.path.basename(self.path)
        put_string("%s:" % name, color='magenta')
        headcommit = self.repo.head.commit
        put_string("    commit %s" % headcommit.hexsha)
        put_string("    Author: %s <%s>" % (headcommit.author.name, headcommit.author.email))
        put_string("    Date:   %s" % time.strftime("%a %b %d %H:%M:%S %Y", time.localtime(headcommit.authored_date)))
        # put_string(headcommit.author_tz_offset)
        # put_string(headcommit.committer.name)
        # put_string(headcommit.committed_date)
        put_string("")
        put_string("        %s" % headcommit.message)

    def commit(self, message=''):
        name = os.path.basename(self.path)
        if self.repo.is_dirty(untracked_files=True):
            try:
                index = self.repo.index
                changed = [ item.a_path for item in self.repo.index.diff(None) ]
                untracked_files = self.repo.untracked_files
                files = []
                for a in changed:
                    files.append(a)
                for a in untracked_files:
                    files.append(a)
                index.add(files)
                index.commit(message)
                remote = self.repo.remote()
                remote.push()
                put_string("Commit [%-24s] ok." % name)
            except Exception as ex:
                put_string("Commit [%-24s] failed(%s)." % (name, str(ex)))

    def delete_branch(self, branch, is_remote=False):
        git = self.repo.git
        try:
            git.branch('-D', branch)
        except Exception as ex:
            # put_string("Delete branch:%s error(%s)." % (branch, str(ex)))
            if not is_remote:
                return str(ex)
        if is_remote:
            try:
                git.push('origin', ':'+branch)
            except Exception as ex:
                return str(ex)
        return None