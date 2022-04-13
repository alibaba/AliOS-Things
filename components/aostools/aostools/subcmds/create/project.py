# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


from __future__ import print_function
import os
import shutil
from aostools import *
from aostools.subcmds import all_commands

class Createproject(Command):
    common = True
    helpSummary = "Create project from templet"
    helpUsage = """
%prog [-b board] [-t templet] [-d destdir] [-r repo] [-B branch] [solution_name]
"""
    helpDescription = """
Create a new workspace and intialize it, 
Create project from templet
"""

    def _Options(self, p):
        p.add_option('-b', '--board',
                     dest='board', action='store', type='str', default=None,
                     help='select board templet')
        p.add_option('-t', '--templet',
                     dest='templet', action='store', type='str', default="helloworld_demo",
                     help='select project templet')
        p.add_option('-d', '--destdir',
                     dest='destdir', action='store', type='str', default=None,
                     help='workspace destination directory')
        p.add_option('-r', '--repo',
                     dest='repo', action='store', type='str', default=None,
                     help='the manifest repo address')
        p.add_option('-B', '--branch',
                     dest='branch', action='store', type='str', default=None,
                     help='the manifest repo branch')


    def Execute(self, opt, args):
        invalid_opt = False
        if not opt.board:
            put_string("Select board with option -b or --board.")
            invalid_opt = True
        if not opt.destdir:
            put_string("Specify workspace destination directory with option -d or --destdir.")
            invalid_opt = True
        if not args:
            put_string("Specify the solution name.")
            invalid_opt = True
        if invalid_opt:
            put_string("")
            self.Usage()
            exit(-1)
        opt.destdir = os.path.abspath(opt.destdir)
        solution_name = args[0]

        # mkdir destdir
        created_newdir = ""
        inited_workspace = False
        if os.path.exists(opt.destdir):
            if os.path.isdir(opt.destdir):
                pass
            else:
                put_string("\"%s\" is not directory." % opt.destdir)
                exit(-1)
        else:
            put_string("create directory: %s" % opt.destdir)
            os.makedirs(opt.destdir)
            created_newdir = opt.destdir

        # enter into destdir
        put_string("enter into directory: %s" % opt.destdir)
        os.chdir(opt.destdir)

        # check destdir is under an existed workspace
        conf = Configure()
        if conf.init:
            opt.destdir = conf.yoc_path
            os.chdir(opt.destdir)
            if created_newdir:
                shutil.rmtree(created_newdir)
                created_newdir = ""
            put_string("Workspace is already existed at %s." % opt.destdir)
        cur_dir = os.getcwd()

        # check solution is already existed
        solution_path = os.path.join(opt.destdir, "solutions", solution_name)
        if os.path.exists(solution_path):
            put_string("Directory \"%s\" is already existed." % solution_path)
            if created_newdir:
                # if cur_dir == created_newdir:
                os.chdir(os.path.dirname(cur_dir))
                shutil.rmtree(created_newdir)
            exit(-1)

        # aos init
        if not os.path.exists(".aos"):
            try:
                cmd = all_commands['init']
                argv = []
                if opt.repo:
                    argv.append(opt.repo)
                if opt.branch:
                    argv.append('-b')
                    argv.append(opt.branch)
                put_string("aos init %s" % ' '.join(argv))
                copts, cargs = cmd.OptionParser.parse_args(argv)
                result = cmd.Execute(copts, cargs)
                inited_workspace = True
            except Exception as e:
                put_string("AosCommand error:", e)
                if created_newdir:
                    # if cur_dir == created_newdir:
                    os.chdir(os.path.dirname(cur_dir))
                    shutil.rmtree(created_newdir)
                exit(-1)

        def remove_project_files(cur_dir, created_newdir, inited_workspace):
            if created_newdir:
                # if cur_dir == created_newdir:
                os.chdir(os.path.dirname(cur_dir))
                shutil.rmtree(created_newdir)
            elif inited_workspace:
                os.remove(".aos")
                shutil.rmtree("components")
                shutil.rmtree("kernel")
                shutil.rmtree("hardware")
                shutil.rmtree("solutions")
                shutil.rmtree("documentation")

        # aos install [templet] and [board]
        try:
            cmd = all_commands['install']
            argv = ['aos_sdk', opt.templet, opt.board]
            if opt.branch:
                argv.append('-b')
                argv.append(opt.branch)
            put_string("aos install %s" % ' '.join(argv))
            copts, cargs = cmd.OptionParser.parse_args(argv)
            result = cmd.Execute(copts, cargs)
        except Exception as e:
            put_string("AosCommand error:", e)
            remove_project_files(cur_dir, created_newdir, inited_workspace)
            exit(-1)
        
        # aos rename [templet] [solution_name]
        if opt.templet != solution_name:
            try:
                cmd = all_commands['rename']
                argv = [opt.templet, solution_name]
                put_string("aos rename %s" % ' '.join(argv))
                copts, cargs = cmd.OptionParser.parse_args(argv)
                result = cmd.Execute(copts, cargs)
                if result != 0:
                    remove_project_files(cur_dir, created_newdir, inited_workspace)
                    exit(-1)
            except Exception as e:
                put_string("AosCommand error:", e)
                remove_project_files(cur_dir, created_newdir, inited_workspace)
                exit(-1)

        # update package.yaml of solution: set the specified board as default one
        filename = os.path.join(cur_dir, "solutions", solution_name, 'package.yaml')
        if os.path.isfile(filename):
            pack = Package(filename)
            if pack.hw_info.board_name != opt.board:
                for d in pack.depends:
                    if opt.board in d:
                        pack.depends.remove(d)
                pack.depends.append({opt.board: "master"})
                pack.hw_info.board_name = opt.board
                pack.save(filename)
        else:
            put_string("File \"%s\" is not existed" % filename)
            remove_project_files(cur_dir, created_newdir, inited_workspace)
            exit(-1)

        # success
        put_string("Create solution \"%s\" successfully." % solution_name)
        return 0