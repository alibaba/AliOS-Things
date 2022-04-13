# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


from __future__ import print_function
import os
from aostools import *


class Sdk(Command):
    common = True
    helpSummary = "Generate current solution SDK"
    helpUsage = """
%prog
"""
    helpDescription = """
generate current solution SDK.
"""
    # def _Options(self, p):
    #     p.add_option('-a', '--all',
    #                  dest='show_all', action='store_true',
    #                  help='show the complete list of commands')

    def Execute(self, opt, args):
        yoc = YoC()
        solution = yoc.getSolution()
        if solution:
            solution.install()
            pack = solution.solution_component.pack
            if pack:
                for libname in solution.libs:
                    if libname not in pack.build_config.libs:
                        pack.build_config.libs.append(libname)

                pack.build_config.libpath.append('aos_sdk/lib')
                pack.build_config.include.append('aos_sdk/include')
                pack.depends = {}
                pack.hw_info.reset()
                pack.hw_info.cpu_name = solution.cpu_name
                # pack.hw_info.ld_script = 'aos_sdk/' + solution.ld_script_component.name + '/' + solution.ld_script_component.pack.hw_info.ld_script
                # text = pack.dumps()
                # print(text)
                # copy and set ld file
                ld_script_name = os.path.basename(solution.ld_script)
                pack.hw_info.ld_script = ld_script_name
                if os.path.dirname(solution.ld_script) != solution.solution_component.path:
                    shutil.copy2(solution.ld_script, pack.hw_info.ld_script)
                put_string("Rebuilding package.yaml")
                pack.defconfig.update(solution.org_defines)
                pack.save(os.path.join(solution.solution_component.path, 'package.yaml'))
                # Makefile
                put_string("Rebuilding Makefile")
                self.rebuild_makefile(solution.solution_component.path, 'Makefile')
                # gitignore
                put_string("Rebuilding .gitignore")
                self.rebuild_gitignore(solution.solution_component.path, '.gitignore')
                # .aos
                if not os.path.exists(os.path.join(os.getcwd(), '.aos')):
                    conf = Configure()
                    shutil.copy2(os.path.join(conf.yoc_path, ".aos"), os.getcwd())

                put_string("Generate %s SDK successful!" % solution.solution_component.name)
                return 0
        else:
            put_string("No solution found in current directory!")
        return 1

    def rebuild_makefile(self, path, filename):
        filename2 = filename + '.new'
        filepath = os.path.join(path, filename)
        filepath2 = os.path.join(path, filename2)
        if os.path.isfile(filepath):
            with codecs.open(filepath, 'r', 'UTF-8') as f:
                with codecs.open(filepath2, 'w', 'UTF-8') as fn:
                    for line in f.readlines():
                        if "rm" in line and "aos_sdk" in line:
                            fn.write("\t%s" % '$(CPRE) rm -rf out aos.elf aos_sdk generated/*.hex generated/*.zip\n')
                        else:
                            fn.write(line)
                    shutil.move(filepath2, filepath)

    def rebuild_gitignore(self, path, filename):
        filename2 = filename + '.new'
        filepath = os.path.join(path, filename)
        filepath2 = os.path.join(path, filename2)
        if os.path.isfile(filepath):
            with codecs.open(filepath, 'r', 'UTF-8') as f:
                with codecs.open(filepath2, 'w', 'UTF-8') as fn:
                    for line in f.readlines():
                        if "aos_sdk" not in line and "generated" not in line:
                            fn.write(line)
                    shutil.move(filepath2, filepath)


