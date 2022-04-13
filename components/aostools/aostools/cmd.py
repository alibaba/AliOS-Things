# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


from __future__ import print_function
from .yoc import *
from .subcmds import all_commands
import optparse
import sys
import os

import warnings
warnings.filterwarnings("ignore", category=SyntaxWarning)

__version__ = "1.0.17"
__auto_init__ = 12345

global_options = optparse.OptionParser(
    usage="aos COMMAND [ARGS]"
)


class YocCommand:
    def __init__(self):
        self.conf = Configure()
        self.commands = {}
        if self.conf.init:
            self.commands = all_commands
        else:
            self.commands['init'] = all_commands['init']
            self.commands['help'] = all_commands['help']
            self.commands['toolchain'] = all_commands['toolchain']
            self.commands['create project'] = all_commands['create project']
            all_commands['help'].commands = self.commands

    def _ParseArgs(self, argv):
        """Parse the main `aos` command line options."""
        name = None
        glob = []

        for i in range(len(argv)):
            if not argv[i].startswith('-'):
                name = argv[i]
                if i > 0:
                    glob = argv[:i]
                
                if len(argv) > i+1:
                    got_flag = None
                    grp_name = name + ' ' + argv[i+1]
                    for k,_ in self.commands.items():
                        if grp_name == k:
                            argv = argv[(i + 1 + 1):]
                            name = grp_name
                            got_flag = True
                            break
                    if got_flag:
                        break
                argv = argv[i + 1:]
                break
        if not name:
            glob = argv
            name = 'help'
            argv = []
        gopts, _gargs = global_options.parse_args(glob)
        return (name, gopts, argv)

    def _Run(self, name, gopts, argv):
        result = 0
        try:
            cmd = self.commands[name]

        except KeyError:
            if not self.conf.init:
                put_string("aos: the workspace is not initialized.", file=sys.stderr)
                if self.conf.need_auto_init:
                    put_string("note: auto initialize workspace in %s.\n" % self.conf.yoc_path)
                    cur_dir = os.getcwd()
                    os.chdir(self.conf.yoc_path)
                    init_cmd = all_commands['init']
                    init_argv = []
                    copts, cargs = init_cmd.OptionParser.parse_args(init_argv)
                    result = init_cmd.Execute(copts, cargs)
                    os.chdir(cur_dir)
                    self.conf.need_auto_init = False
                    return __auto_init__
                else:
                    put_string("\nnote: please enter into the workspace directory.")
                    put_string("      and run 'aos init' in the workspace root directory.")
                    return 1
            else:
                put_string("aos: '%s' is not a aos command.  See 'aos help'." % name, file=sys.stderr)
                return 1

        try:
            copts, cargs = cmd.OptionParser.parse_args(argv)
            copts = cmd.ReadEnvironmentOptions(copts)
        except Exception as e:
            put_string('error: in `%s`: %s' % (' '.join([name] + argv), str(e)),
                       file=sys.stderr)
            put_string('error: manifest missing or unreadable -- please run init',
                       file=sys.stderr)
            return 1
        try:
            cmd.ValidateOptions(copts, cargs)
            result = cmd.Execute(copts, cargs)
        except Exception as e:
            put_string("AosCommand error:", e)
            result = 1
            pass
        return result

    def Execute(self, argv):
        name, gopts, argv = self._ParseArgs(argv)

        return self._Run(name, gopts, argv)


def main():
    if len(sys.argv) == 2:
        if sys.argv[1] == '-V' or sys.argv[1] == '--version':
            put_string("aos-tools " + __version__ + "\n")
            path = which("aos-tools")
            put_string("script path is: " + path)
            put_string("pip package path is: " + os.path.dirname(os.path.realpath(__file__)))
            exit(0)
    cmd = YocCommand()
    result = cmd.Execute(sys.argv[1:])
    # rerun it after auto initialze the workspace
    if result == __auto_init__:
        cmd = YocCommand()
        result = cmd.Execute(sys.argv[1:])
    exit(result)

def cct_main():
    try:
        cmd = all_commands['cct']
        parser = cmd.OptionParser
        parser.set_usage(cmd.helpUsage.strip().replace('%prog', cmd.NAME))
        copts, cargs = parser.parse_args(sys.argv[1:])
        copts = cmd.ReadEnvironmentOptions(copts)
    except Exception as e:
        put_string('error: manifest missing or unreadable -- please run init',
                    file=sys.stderr)
        return 1
    try:
        cmd.ValidateOptions(copts, cargs)
        cmd.Execute(copts, cargs)
    except Exception as e:
        put_string("AosCommand error:", e)
        pass
