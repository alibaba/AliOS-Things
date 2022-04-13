# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


from __future__ import print_function

from aostools import *
import codecs

class Make(Command):
    common = True
    helpSummary = "Make aos program"
    helpUsage = """
%prog [option] [<subcommand> ...]
"""
    helpDescription = """
Make aos program.\n"
"\nPlease run 'aos make --help' to explore more ...
"""

    def _Options(self, p):
        p.add_option('-b', '--board',
                     dest='board', action='store',
                     help='select board to build')
        p.add_option('-c', '--config',
                     dest='config', action='store',
                     help='select solution and board')


    def Execute(self, opt, args):
        result = 0
        cmds = []
        params = []
        for arg in args:
            if "=" in arg:
                params.append(arg)
            else:
                cmds.append(arg)
        
        params_str = " ".join(params)
        # get workspace root directory
        conf = Configure()
        change_dir_to_solution(conf.yoc_path)

        count = len(cmds)
        if count == 0:
            if opt.board:
                result = os.system("make BOARD=%s %s" % (opt.board, params_str)) >> 8
            else:
                result = os.system("make %s" % params_str) >> 8

        for cmd in cmds:
            if cmd == "clean":
                result = os.system("make clean") >> 8
                break
            elif cmd == "distclean":
                self.clearSolutionAndBoard(conf.yoc_path)
                result = os.system("make clean") >> 8
                break
            elif "@" in cmd:
                self.saveSolutionAndBoard(conf.yoc_path, cmd)
            else:
                result = os.system("make %s %s" % (cmd, params_str)) >> 8
        return result

    def saveSolutionAndBoard(self, yoc_path, app_board):
        config_file = os.path.join(yoc_path, ".config")
        if not os.path.isfile(os.path.join(yoc_path, ".aos")):
            put_string("Please excute \"aos init\" in the workspace root directory first.")
            exit(-1)
        else:
            v = app_board.split("@")
            if len(v) != 2:
                put_string("Invalid format. Please config it like aos make solution@board -c config")
                exit(-1)
            # update package.yaml of solution: set the specified board as default one
            filename = os.path.join(yoc_path, "solutions", v[0], 'package.yaml')
            if os.path.isfile(filename):
                pack = Package(filename)
                if pack.hw_info.board_name != v[1]:
                    for d in pack.depends:
                        if v[1] in d:
                            pack.depends.remove(d)
                    pack.depends.append({v[1]: "master"})
                    pack.hw_info.board_name = v[1]
                    pack.save(filename)
                # save solution and board in .config file
                with codecs.open(config_file, 'w', 'UTF-8') as f:
                    f.write("{}: {}\n".format("solution", v[0]))
                    f.write("{}: {}\n".format("board", v[1]))
                    put_string("configuration %s written to %s." % (app_board, config_file))
            else:
                put_string("solution %s is not existed." % v[0])

    def clearSolutionAndBoard(self, yoc_path):
        config_file = os.path.join(yoc_path, ".config")
        if os.path.isfile(config_file):
            os.remove(config_file)
        if os.path.isfile('.config_burn'):
            os.remove('.config_burn')
        if os.path.isfile('.config_monitor'):
            os.remove('.config_monitor')
