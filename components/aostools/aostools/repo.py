#!/usr/bin/env python
# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


from __future__ import print_function

# import github
# import gitlab
import json
import os

from .tools import *


class RepoGitee:
    def __init__(self, token=None, group='yocop'):
        self.tokenAccess = token
        self.group = group

    def projects(self, count=10000000):
        page = 1
        total_page = 10000000
        all_json = []
        while page <= total_page and len(all_json) < count:
            per_page = min(count, 100)
            url = 'https://gitee.com/api/v5/orgs/%s/repos?access_token=%s&type=all&page=%d&per_page=%d' % (
                self.group, self.tokenAccess, page, per_page)
            headers = {'Content-Type': 'application/json;charset=UTF-8'}

            code, data, headers = http_request('GET', url, headers=headers)
            if code == 200:
                total_page = int(headers['total_page'])
                for v in json.loads(data):
                    all_json.append(v['description'])
            page += 1

        return all_json

    def create_project(self, name, description=''):
        url = 'https://gitee.com/api/v5/orgs/%s/repos' % self.group
        headers = {'Content-Type': 'application/json;charset=UTF-8'}

        data = {
            "access_token": self.tokenAccess,
            "name": name, "has_issues": "true", "has_wiki": "true", "private": "false",
            "description": description,
            # "license_template":"Apache-2.0",
        }
        code, data, _ = http_request('POST', url, json.dumps(data), headers)
        if code == 201:
            v = json.loads(data)
            return v['ssh_url']
        else:
            return self.update_project(name, description)

    def get_project(self, name):
        url = 'https://gitee.com/api/v5/repos/%s/%s?access_token=%s' % (
            self.group, name, self.tokenAccess)
        headers = {'Content-Type': 'application/json;charset=UTF-8'}

        data = {
            "access_token": self.tokenAccess,
            "name": name, "has_issues": "true", "has_wiki": "true", "private": "true"
        }
        code, data, _ = http_request('GET', url, headers=headers)
        if code == 200:
            v = json.loads(data)
            return v['ssh_url']

    def remove_project(self, name):
        url = 'https://gitee.com/api/v5/repos/%s/%s?access_token=%s' % (
            self.group, name, self.tokenAccess)

        headers = {'Content-Type': 'application/json;charset=UTF-8'}

        code, data, _ = http_request('DELETE', url, headers=headers)
        if code != 204:
            v = json.loads(data)
            return False, v['message']

        return True, ''

    def branch_to_tag(self, name, branch, tag_name):
        # curl -X POST --header 'Content-Type: application/json;charset=UTF-8'
        # 'https://gitee.com/api/v5/repos/yocopen/newlib/releases'
        # -d '{"access_token":"c4d2f6d45952367bxxxxxx",
        # "tag_name":"v7.2.1","name":"v7.2.1","body":"v7.2.1","target_commitish":"v7.2-dev"}'
        url = 'https://gitee.com/api/v5/repos/%s/%s/releases' % (self.group, name)

        headers = {'Content-Type': 'application/json;charset=UTF-8'}

        data = {
            "access_token": self.tokenAccess,
            "tag_name": tag_name,
            "name": tag_name, "body": tag_name,
            "target_commitish": branch
        }
        code, data, _ = http_request('POST', url, json.dumps(data), headers)

        if code == 201:
            v = json.loads(data)
        else:
            v = json.loads(data)
            put_string(v)

    def update_project(self, name, description=''):
        # curl -X PATCH --header 'Content-Type: application/json;charset=UTF-8'
        # 'https://gitee.com/api/v5/repos/yocopen/yunvoice' -d '{"access_token":"c4d2f6d45952367bxxxxxx","name":"yunvoice",
        # "description":"{\"name\": \"yunvoice\", \"description\": \"yunvoice\", \"versions\": \"v7.2-dev\", \"license\": \"\", \"type\": \"common\", \"depends\": []}",
        # "has_issues":"true","has_wiki":"true"}
        # '

        url = 'https://gitee.com/api/v5/repos/%s/%s' % (self.group, name)
        headers = {'Content-Type': 'application/json;charset=UTF-8'}

        data = {
            "access_token": self.tokenAccess,
            "name": name, "has_issues": "true", "has_wiki": "true", "private": "false",
            "description": description,
            # "license_template":"Apache-2.0",
        }
        code, data, _ = http_request('PATCH', url, json.dumps(data), headers)
        if code == 200:
            v = json.loads(data)
            return v['ssh_url']
        else:
            v = json.loads(data)
            put_string(v)

    def set_branch_protection(self, name, branch):
        # curl -X PUT --header 'Content-Type: application/json;charset=UTF-8' 
        # 'https://gitee.com/api/v5/repos/yocop/test_cmp/branches/test2/protection' 
        # -d '{"access_token":"c4d2f6d45952367bxxxxxx"}'
        url = 'https://gitee.com/api/v5/repos/%s/%s/branches/%s/protection' % (self.group, name, branch)

        headers = {'Content-Type': 'application/json;charset=UTF-8'}

        data = {
            "access_token": self.tokenAccess,
        }
        code, data, _ = http_request('PUT', url, json.dumps(data), headers)

        if code == 200:
            v = json.loads(data)
            put_string("保护分支设置成功.")
        else:
            v = json.loads(data)
            put_string(v['message'])

    def add_user_to_repo(self, repo, username):
        # curl -X PUT --header 'Content-Type: application/json;charset=UTF-8' 
        # 'https://gitee.com/api/v5/repos/yocop/test_cmp/collaborators/aaa' 
        # -d '{"access_token":"c4d2f6d45952367bxxxxxx","permission":"push"}'
        url = 'https://gitee.com/api/v5/repos/%s/%s/collaborators/%s' % (self.group, repo, username)

        headers = {'Content-Type': 'application/json;charset=UTF-8'}

        data = {
            "access_token": self.tokenAccess,
            "permission":"push",
        }
        code, data, _ = http_request('PUT', url, json.dumps(data), headers)

        if code == 200:
            v = json.loads(data)
            put_string("添加用户到仓库成功.")
        else:
            v = json.loads(data)
            put_string(v['message'])

    def delete_user_to_repo(self, repo, username):
        # curl -X DELETE --header 'Content-Type: application/json;charset=UTF-8' 
        # 'https://gitee.com/api/v5/repos/yocop/test_cmp/collaborators/aaa?access_token=c4d2f6d45952367bxxxxxx'
        url = 'https://gitee.com/api/v5/repos/%s/%s/collaborators/%s' % (self.group, repo, username)

        headers = {'Content-Type': 'application/json;charset=UTF-8'}

        data = {
            "access_token": self.tokenAccess,
        }
        code, data, _ = http_request('DELETE', url, json.dumps(data), headers)

        if code == 204:
            put_string("移除用户成功.")
        else:
            v = json.loads(data)
            put_string(v['message'])

    def add_public_key(self):
        id_ras_pub = home_path('.ssh/id_rsa.pub')
        if not os.path.isfile(id_ras_pub):
            cmd = "ssh-keygen -f ~/.ssh/id_rsa -N ' ' -t rsa"
            os.system(cmd)

        try:
            with codecs.open(id_ras_pub, 'r') as f:
                pub_key = f.read()
                k = pub_key.split(' ')
                if len(k) == 3:
                    title = k[2]

                url = 'https://gitee.com/api/v5/user/keys'
                headers = {'Content-Type': 'application/json;charset=UTF-8'}

                data = {
                    "access_token": self.tokenAccess,
                    "key": pub_key,
                    "title": title
                }

                code, data, _ = http_request(
                    'POST', url, json.dumps(data), headers)
                v = json.loads(data)
                put_string(v['message'])
                if code == 200:
                    return v['id']
        except Exception as ex:
            put_string(str(ex))
            pass


class RepoCodeup:
    def __init__(self, token=None):
        pass

    def projects(self):
        pass

    def create_project(self, name, description=''):
        headers = {
            'Ao-User-Locale': 'zh_CN',
            'Authorization': '',
            'X-Operator-ID': '',
            'X-Tenant-Type': 'organization',
        }

        body = {
            # "avatar_url": "string",
            "description": description,
            # "import_account": "string",
            # "import_demo_project": True,
            # "import_token": "string",
            # "import_token_encrypted": "text，rsa",
            # "import_url": "string",
            "name": name,
            "namespace_id": 0,
            "path": "string",
            "readme_type": "EMPTY",
            "visibility_level": "0"
        }

        url = 'https://open.teambition.com/api/code/v3/projects?create_parent_path=create_parent_path&sync=sync'
        http_request('POST', url, json.dumps(body), headers=headers)
        pass

    def update_project(self, name, description=''):
        headers = {
            'Authorization': '',
            'X-Operator-ID': '',
            'X-Tenant-Id': '',
            '-Tenant-Type': 'organization'
        }
        url = 'https://open.teambition.com/api/code/v3/projects/{projectId}/settings'
        pass

    def remove_project(self, name):
        pass

    def branch_to_tag(self, name, branch, tag_name):
        pass


# class RepoGithub:
#     def __init__(self, token=None):
#         self.gl = github.Github(token, timeout=30)
#         self.organization = self.gl.get_organization('yoc-components')

#     def projects(self):
#         for repo in self.organization.get_repos():
#             put_string(repo.name, repo)
#         projects = self.gl.projects.list(
#             owned=True, all=True, namespace_id='6883967')
#         return projects

#     def create_project(self, name):
#         try:
#             project = self.organization.create_repo(name)
#             return project.ssh_url
#         except github.GithubException as e:
#             return 'git@github.com:yoc-components/' + name + '.git'
#             put_string(e)

#     def branch_to_tag(self, name, branch, tag_name):
#         project = self.organization.get_repo(name)
#         put_string(project)

#         for tag in project.get_tags():
#             if tag.name == tag_name:
#                 return

#         for br in project.get_branches():
#             if br.name == branch:
#                 put_string(br)
#                 tag = project.create_git_tag(
#                     tag_name, 'Created from tag %s' % br.name, br.commit.sha, 'commit')
#                 if tag:
#                     project.create_git_ref('refs/tags/%s' % tag_name, tag.sha)
#                 break

# class RepoGitlab:
#     def __init__(self):
#         url = 'https://gitlab.com'
#         token = 'KaPY7CR2Fsu4dm_71Zro'

#         self.gl = gitlab.Gitlab(url, token)

#     def projects(self):
#         projects = self.gl.projects.list(
#             owned=True, all=True, namespace_id='6883967')
#         for p in projects:
#             put_string(p.name, p.ssh_url_to_repo)

#     def create_project(self, name):
#         try:
#             project = self.gl.projects.create(
#                 {'name': name, 'namespace_id': '6883967'})

#             return project.ssh_url_to_repo

#         except gitlab.GitlabCreateError as e:
#             put_string(e.error_message)
#             project = self.gl.projects.get('occ-thead/' + name)
#             return project.ssh_url_to_repo
