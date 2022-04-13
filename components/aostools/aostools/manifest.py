# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


from __future__ import print_function
import json
import shutil
import glob
import codecs
import re

from .tools import *
from .log import logger


class MRemotes:
    def __init__(self, dic):
        self.name = ''
        self.remote = ''

        for k, v in dic.items():
            if v:
                self.name = k
                self.remote = v

class MComponent:
    def __init__(self, dic):
        self.name = ''
        self.type = ''
        self.remote = ''
        self.desc = ''

        for k, v in dic.items():
            if v:
                self.__dict__[k] = v

class Manifest(object):
    def __init__(self, filename=None):
        self.filename = filename
        self.remotes = []
        self.cmpt_list = []
        self.includes = []

        if filename:
            self.load(filename)

    def load(self, filename):
        m = yaml_load(filename)
        if not m:
            return
        for k, v in m.items():
            if v:
                if k == 'remotes':
                    if type(v) == list:
                        for item in v:
                            self.remotes.append(MRemotes(item))
                if k == 'components':
                    if type(v) == list:
                        for item in v:
                            self.cmpt_list.append(MComponent(item))
                if k == 'includes':
                    if type(v) == list:
                        for item in v:
                            self.includes.append(item)
    
    def save(self, filename, cmpt_list):
        text = '''{remotes}{components}{includes}'''
        text = text.format(
            remotes=self.remotes_out(),
            components=self.components_out(cmpt_list),
            includes=self.includes_out()
        )

        try:
            with codecs.open(filename, 'w', 'UTF-8') as f:
                f.write(text)
            return True
        except:
            put_string("Generate %s failed." % filename)

    def remotes_out(self):
        text = ''
        if len(self.remotes) > 0:
            text = 'remotes:\n'
            for d in self.remotes:
                for k, v in d.items():
                    text += '  - {}: {}\n'.format(k, v)
        return text
    
    def components_out(self, components):
        text = ''
        if len(components) > 0:
            text = 'components:\n'
            for component in components:
                component.load_package()
                desc = component.description
                if sys.version_info.major == 2:
                    if type(desc) == unicode:
                        desc = desc.encode('utf8')
                if type(desc) != str:
                    desc = str(desc)
                text += '  - name: "%s"\n' % component.name
                text += '    remote: "%s"\n' % 'aone'
                text += '    type: "%s"\n' % component.type
                text += '    desc: "%s"\n' % desc
        return text

    def includes_out(self):
        text = ""
        if len(self.includes) > 0:
            text = 'includes:\n'
            for include in self.includes:
                text += '  - "%s"\n' % include
        return text

    def gen_templet(self, components):
        self.remotes = [{'aone': 'git@gitlab.alibaba-inc.com:yocopen', 'gitee': 'https://gitee.com/yocop'}]
        self.includes = ['https://xx.xx.xx.xx/1.yaml', 'https://yy.yy.yy.yy/2.yaml']
        self.save('default.yaml', components)

