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

import os

all_commands = {}


exclude_cmd = ['branch', 'checkout', 'cdk']

my_dir = os.path.dirname(__file__)
for py in os.listdir(my_dir):
    if py == '__init__.py':
        continue

    if py.endswith('.py'):
        name = py[:-3]

        if name in exclude_cmd:
            continue

        clsn = name.capitalize()
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
        cmd.NAME = name
        all_commands[name] = cmd

    if os.path.isdir(os.path.join(my_dir, py)):
        name = py
        mod = __import__(__name__,
                         globals(),
                         locals(),
                         ['%s' % name])
        try:
            mod = getattr(mod, name)
            cmds = getattr(mod, 'sub_commands')
            for i in cmds:
                all_commands[i] = cmds[i]
        except AttributeError:
            pass


if 'help' in all_commands:
    all_commands['help'].commands = all_commands
