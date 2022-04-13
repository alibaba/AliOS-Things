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
import sys

from aostools import *


class Help(Command):
    common = False
    helpSummary = "Display detailed help on a command"
    helpUsage = """
%prog [--all|command]
"""
    helpDescription = """
Displays detailed usage information about a command.
"""

    def _Options(self, p):
        p.add_option('-a', '--all',
                     dest='show_all', action='store_true',
                     help='show the complete list of commands')
        p.add_option('--help-all',
                     dest='show_all_help', action='store_true',
                     help='show the --help of all commands')

    def _PrintAllCommands(self):
        put_string('usage: aos COMMAND [ARGS]')
        put_string('The complete list of recognized aos commands are:')
        commandNames = list(sorted(self.commands))

        maxlen = 0
        for name in commandNames:
            maxlen = max(maxlen, len(name))
        fmt = '  %%-%ds  %%s' % maxlen

        for name in commandNames:
            command = self.commands[name]
            try:
                summary = command.helpSummary.strip()
            except AttributeError:
                summary = ''
            put_string(fmt % (name, summary))
        put_string("See 'aos help <command>' for more information on a "
                   'specific command.')

    def _PrintCommandHelp(self, cmd, header_prefix=''):
        cmd.OptionParser.print_help()

    def _PrintAllCommandHelp(self):
        for name in sorted(self.commands):
            cmd = self.commands[name]
            cmd.manifest = self.manifest
            self._PrintCommandHelp(cmd, header_prefix='[%s] ' % (name,))

    def Execute(self, opt, args):
        if len(args) == 0:
            if opt.show_all_help:
                self._PrintAllCommandHelp()
            else:
                self._PrintAllCommands()
        elif len(args) < 3:
            if len(args) == 1:
                name = args[0]
            else:
                name = args[0] + " " + args[1]

            try:
                cmd = self.commands[name]
            except KeyError:
                put_string("aos: '%s' is not a aos command." %
                           name, file=sys.stderr)
                return 1

            cmd.manifest = self.manifest
            self._PrintCommandHelp(cmd)

        else:
            self._PrintCommandHelp(self)

        put_string("\r\nGeneral Options:")
        put_string("-V, --version   Show version and exit.")
        return 0
