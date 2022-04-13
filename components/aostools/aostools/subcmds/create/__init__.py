# -*- coding:utf-8 -*-
#
# Copyright (C) 2008 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
from __future__ import print_function
import os
from aostools import *

sub_commands = {}

my_dir = os.path.dirname(__file__)

sub_dir = os.path.basename(my_dir)

for py in os.listdir(my_dir):
    if py == '__init__.py':
        continue

    if py.endswith('.py'):
        name = py[:-3]

        clsn = name.capitalize()
        if name != sub_dir:
            clsn = (sub_dir + name).capitalize()

        while clsn.find('_') > 0:
            h = clsn.index('_')
            clsn = clsn[0:h] + clsn[h + 1:].capitalize()

        mod = __import__(__name__,
                         globals(),
                         locals(),
                         ['%s' % name])
        mod = getattr(mod, name)
        try:
            cmd = getattr(mod, clsn)()
        except AttributeError:
            raise SyntaxError('%s/%s does not define class %s' % (
                __name__, py, clsn))

        name = name.replace('_', '-')
        if name != sub_dir:
            name = sub_dir + " " + name
        cmd.NAME = name
        sub_commands[name] = cmd
