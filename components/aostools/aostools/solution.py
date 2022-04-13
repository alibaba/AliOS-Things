# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


from __future__ import print_function
import os
import re

from .log import logger
from .tools import *
from .package import *
from .component import *


class Variables:
    def __init__(self):
        self.vars = {}
        pass

    def set(self, k, v, lower=False):
        self.vars[k] = v
        if lower:
            self.vars[k.lower()] = v.lower()

    def get(self, var):
        if var in self.vars:
            return self.vars[var]

    def convert(self, var):
        if var == None or len(var) == 0:
            return None

        v = var.split('?')
        if len(v) > 1:
            x = re.search('<(.+?)>', v[1], re.M | re.I)
            if x:
                x = x.group(1).strip()
                if(x.startswith('!')):
                    x = self.get(x[1:])
                    if str(x) not in ['n', 'no', 'f', 'false', '0', False, 0, 'None']:
                        return None
                    var = v[0].strip()
                else:
                    x = self.get(x)
                    if str(x) not in ['y', 'yes', 't', 'true', '1', True, 1]:
                        return None
                    var = v[0].strip()

        x = re.findall('<(.+?)>', var)
        for key in x:
            value = self.get(key)
            if value != None:
                var = var.replace('<'+key+'>', self.vars[key])
            else:
                put_string('Not found variable:', var)
        return var

    def items(self):
        return self.vars.items()


class Solution:
    def __init__(self, components, compiler='gcc'):
        self.variables = Variables()
        self.solution_component = None       # 当前 solution 组件
        self.board_component = None          # 当前开发板组件
        self.chip_component = None           # 当前芯片组件
        self.components = components         # 使用到的组件
        self.cond_components = []            # 依赖某些条件的组件列表
        self.global_includes = []
        self.libs = []
        self.libpath = []
        self.depend_libs = []
        self.org_defines = {}
        self.defines = {}
        self.ASFLAGS = []
        self.CFLAGS = []
        self.CXXFLAGS = []
        self.LINKFLAGS = []
        self.ICC_ASFLAGS = []
        self.ICC_CFLAGS = []
        self.ICC_CXXFLAGS = []
        self.ICC_LINKFLAGS = []
        self.ARMCC_ASFLAGS = []
        self.ARMCC_CFLAGS = []
        self.ARMCC_CXXFLAGS = []
        self.ARMCC_LINKFLAGS = []
        self.ld_script = ''
        self.ld_script_component = None
        self.cpu_name = ''
        self.toolchain_prefix = ''
        self.flash_program = ''
        self.program_data_files = []
        self.debug_server = ''

        for comp in self.components:
            if len(comp.conditions) > 0:
                self.cond_components.append(comp)
                # print("-------%s,%s" % (comp.name, str(comp.conditions)))
        components = ComponentGroup()
        for a in self.components:
            if a not in self.cond_components:
                components.add(a)
        self.components = components

        set_compiler(compiler)
        self.variables.set('compiler', compiler)
        # find current solution & board component
        for component in self.components:
            if component.path == os.getcwd() and component.type == 'solution':
                self.solution_component = component
                if self.solution_component.hw_info.board_name:
                    self.board_component = self.components.get(
                        self.solution_component.hw_info.board_name)
                    if not self.board_component:
                        put_string("Not found in `%s` components that the current project depends on. " %
                                   self.solution_component.hw_info.board_name)
                        exit(-1)

                    self.chip_component = self.components.get(
                        self.board_component.hw_info.chip_name)
                    if not self.chip_component:
                        put_string("Not found in `%s` components that the current project depends on. " %
                                   self.board_component.hw_info.chip_name)
                        exit(-1)
                elif self.solution_component.hw_info.chip_name:
                    self.chip_component = self.components.get(self.solution_component.hw_info.chip_name)
                    if not self.chip_component:
                        put_string("Not found in `%s` components that the current project depends on. " %
                                   self.board_component.hw_info.chip_name)
                        exit(-1)
                break

        if not self.solution_component:
            put_string('The current directory is not a solution directory.')
            exit(-1)

        if (not self.board_component) and (not self.solution_component.hw_info.cpu_name) and (not self.solution_component.hw_info.ld_script):
            for name in self.solution_component.depends:
                if type(name) == dict:
                    name = list(name.keys())[0]
                component = self.components.get(name)
                if component:
                    if component.type == 'board':
                        self.board_component = component
                        self.solution_component.hw_info.board_name = self.board_component.name
                        self.chip_component = self.components.get(
                            self.board_component.hw_info.chip_name)
                        if self.chip_component:
                            self.solution_component.hw_info.chip_name = self.chip_component.name
                        else:
                            put_string("Not found chip component: " +
                                       self.board_component.hw_info.chip_name)
                            exit(-1)
                        break
                else:
                    put_string("`%s` not found, please install it: aos install %s" %
                               (name, name))
                    exit(-1)
            if not self.board_component:
                logger.warning(
                    'Not found board component in depends')

        if self.board_component:
            self.board_component.need_build = True
        if self.chip_component:
            self.chip_component.need_build = True

        components = ComponentGroup()
        for component in self.components:
            if component.need_build:
                components.add(component)
        self.components = components

        def cpu_set_variable(cpu, arch):
            def gen_cpu_defs(cpu):
                c = cpu.upper()
                c = c.replace('-', '').replace('.', '')
                c = '__CPU_' + c + '__'
                return c
            def gen_arch_defs(arch):
                a = arch.upper()
                a = '__ARCH_' + a + '__'
                return a

            self.cpu_name = cpu
            self.variables.set('CPU', cpu.upper(), True)
            if arch == '' or arch.upper() == 'CSKY':
                self.variables.set('cpu_num', cpu_id(cpu))
                self.variables.set('ARCH', 'CSKY', True)
            elif arch.upper() == 'ARM':
                self.variables.set('cpu_num', arm_cpu_id(cpu))
                self.variables.set('ARCH', arch, True)
            elif arch.upper() == 'RISCV':
                self.variables.set('cpu_num', riscv_cpu_id(cpu))
                self.variables.set('ARCH', arch, True)
            else:
                logger.error("not support arch:%s yet" % arch)
                exit(-1)
            # add cpu and arch defines,like __CPU_COTEXM0__ and __ARCH_ARM__
            self.variables.set(gen_arch_defs(arch), 1)
            self.variables.set(gen_cpu_defs(cpu), 1)

        def add_defines(component):
            for k, v in component.defconfig.items():
                self.variables.set(k, v)
                self.defines[k] = v
                self.org_defines[k] = v

        def update_variables():
            # copy defines
            for component in self.components:
                if component.type == 'common':
                    add_defines(component)
            for component in self.components:
                if component.type == 'kernel':
                    add_defines(component)
            for component in self.components:
                if component.type == 'drv_core' or component.type == 'drv_peripheral' or component.type == 'drv_external_device':
                    add_defines(component)
            for component in self.components:
                if component.type == 'arch':
                    add_defines(component)
            for component in self.components:
                if component.type == 'chip':
                    add_defines(component)
            for component in self.components:
                if component.type == 'board':
                    add_defines(component)
            for component in self.components:
                if component.type == 'solution':
                    add_defines(component)

            solution_hw_info = HardwareInfo({})
            for component in [self.chip_component, self.board_component, self.solution_component]:
                if component:
                    solution_hw_info.merge(component.hw_info)

                    for c in component.build_config.cflag.split():
                        self.CFLAGS.append(c)
                    for c in component.build_config.asmflag.split():
                        self.ASFLAGS.append(c)
                    for c in component.build_config.cxxflag.split():
                        self.CXXFLAGS.append(c)
                    for c in component.build_config.ldflag.split():
                        self.LINKFLAGS.append(c)
                    
                    for c in component.build_config.icc_cflag.split():
                        self.ICC_CFLAGS.append(c)
                    for c in component.build_config.icc_asmflag.split():
                        self.ICC_ASFLAGS.append(c)
                    for c in component.build_config.icc_cxxflag.split():
                        self.ICC_CXXFLAGS.append(c)
                    for c in component.build_config.icc_ldflag.split():
                        self.ICC_LINKFLAGS.append(c)
                    
                    for c in component.build_config.armcc_cflag.split():
                        self.ARMCC_CFLAGS.append(c)
                    for c in component.build_config.armcc_asmflag.split():
                        self.ARMCC_ASFLAGS.append(c)
                    for c in component.build_config.armcc_cxxflag.split():
                        self.ARMCC_CXXFLAGS.append(c)
                    for c in component.build_config.armcc_ldflag.split():
                        self.ARMCC_LINKFLAGS.append(c)

            # 根据 cpu_id 加载配置
            if solution_hw_info.cpu_id in solution_hw_info.cpu_list:
                for k, v in solution_hw_info.cpu_list[solution_hw_info.cpu_id].items():
                    solution_hw_info.__dict__[k] = v

            # board 的hw_info 次优先
            if self.board_component and self.board_component.hw_info:
                solution_hw_info.merge(self.board_component.hw_info, True)
            # soution 的hw_info 优先
            solution_hw_info.merge(self.solution_component.hw_info, True)

            if solution_hw_info.cpu_name == '':
                logger.error("not found cpu_id, please set cpu_id")
                exit(-1)

            if solution_hw_info.cpu_name:
                cpu_set_variable(solution_hw_info.cpu_name,
                                solution_hw_info.arch_name)

            if solution_hw_info.cpu_id:
                self.variables.set(solution_hw_info.cpu_id.upper(), True)

            if solution_hw_info.vendor_name:
                self.variables.set(
                    'VENDOR', solution_hw_info.vendor_name.upper(), True)

            if solution_hw_info.chip_name:
                self.variables.set(
                    'CHIP', solution_hw_info.chip_name.upper(), True)

            if solution_hw_info.board_name:
                self.variables.set(
                    'BOARD', solution_hw_info.board_name.upper(), True)

            if self.chip_component:
                self.variables.set('CHIP_PATH', self.chip_component.path)

            if self.board_component:
                self.variables.set('BOARD_PATH', self.board_component.path)

            self.variables.set('SOLUTION_PATH', self.solution_component.path)
            self.yoc_path = os.path.join(self.solution_component.path, 'aos_sdk')
            self.lib_path = os.path.join(self.yoc_path, 'lib')
            self.libpath.append(self.lib_path)
            self.flash_program = solution_hw_info.flash_program
            self.program_data_files = solution_hw_info.program_data_files
            self.debug_server = solution_hw_info.debug_server

            self.global_includes.append(os.path.join(
                self.solution_component.path, 'include'))

            for component in self.components:
                if len(component.source_files) > 0 and component.name not in self.libs:
                    self.libs.append(component.name)
                    # if component.name is start with lib, scons will not add prefix: lib
                    if component.name.startswith('lib'):
                        self.depend_libs.append(os.path.join(
                            self.lib_path, component.name + '.a'))
                    else:
                        self.depend_libs.append(os.path.join(
                            self.lib_path, 'lib' + component.name + '.a'))

                component.variable_convert(self.variables)

                # include
                for path in component.build_config.include_updated:
                    if path not in self.global_includes:
                        self.global_includes.append(path)

                # libpath
                for path in component.build_config.libpath_updated:
                    if path not in self.libpath:
                        self.libpath.append(path)

                # libs
                for lib in component.build_config.libs_updated:
                    if lib not in self.libs:
                        self.libs.append(lib)

            self.toolchain_prefix = solution_hw_info.toolchain_prefix
            self.ld_script = solution_hw_info.ld_script
            self.ld_script = self.variables.convert(self.ld_script)

            if self.solution_component.hw_info.ld_script:
                self.ld_script_component = self.solution_component
            elif self.board_component and self.board_component.hw_info.ld_script:
                self.ld_script_component = self.board_component
            elif self.chip_component and self.chip_component.hw_info.ld_script:
                self.ld_script_component = self.chip_component
            elif self.chip_component and self.ld_script:
                # multi cpu
                self.ld_script_component = self.chip_component
            else:
                logger.error("not found LD script file, please set ld_script")
                exit(-1)

            # append variables to defines
            for k, v in self.variables.items():
                if k.isupper():
                    if k.startswith('__CPU_') or k.startswith('__ARCH_'):
                        self.defines[k] = v
                    elif k in ['CPU', 'ARCH', 'VENDOR', 'CHIP', 'BOARD', 'CHIP_PATH', 'BOARD_PATH', 'SOLUTION_PATH']:
                        self.defines['__AOS_' + k + '__'] = v

        update_variables()

        def check_variables(values=[]):
            # check and condition: [v1, v2, v3...]
            for v in values:
                if(v.startswith('!')):
                    a = self.variables.get(v[1:])
                    if str(a) not in ['n', 'no', 'f', 'false', '0', False, 0, 'None']:
                        return False
                else:
                    a = self.variables.get(v)
                    if str(a) not in ['y', 'yes', 't', 'true', '1', True, 1]:
                        return False

            return True

        for cc in self.cond_components:
            if check_variables(cc.conditions):
                self.components.add(cc)
                update_variables()

    def install(self):
        for component in self.components:
            component.install(self.yoc_path)

    def show(self, show_component=True, show_libs=True, show_include=True, show_define=True, show_veriable=True, show_flags=True):
        if show_component:
            put_string("[component]")
            for c in self.components:
                c.show(4)

            put_string()
            if self.solution_component:
                put_string("    solution component: " +
                           self.solution_component.name)
            if self.board_component:
                put_string("    board component   : " +
                           self.board_component.name)
            if self.chip_component:
                put_string("    chip component    : " +
                           self.chip_component.name)

        if show_libs:
            put_string("[libs]")
            for v in self.libs:
                put_string('    ' + v)

            put_string("[libpath]")
            for v in self.libpath:
                put_string('    ' + v)

        if show_include:
            put_string("[global_includes]")
            for v in self.global_includes:
                put_string('    ' + v)

        if show_define:
            put_string("[defines]")
            for k, v in self.defines.items():
                put_string('    ', k, '=', v)

        if show_veriable:
            put_string("[variables]")
            for k, v in self.variables.items():
                put_string('    %s = %s' % (k, v))

        if show_flags:
            put_string("\nGCC COMPILER")
            put_string("ASFLAGS   :" + ' '.join(self.ASFLAGS))
            put_string("CFLAGS   :" + ' '.join(self.CFLAGS))
            put_string("CXXFLAGS  :" + ' '.join(self.CXXFLAGS))
            put_string("LINKFLAGS :" + ' '.join(self.LINKFLAGS))
            put_string('ld_script : ' + self.ld_script)
            put_string('toolchain : ' + self.toolchain_prefix)
            # put_string("\nIAR COMPILER")
            # put_string("ASFLAGS   :" + ' '.join(self.ICC_ASFLAGS))
            # put_string("CFLAGS   :" + ' '.join(self.ICC_CFLAGS))
            # put_string("CXXFLAGS  :" + ' '.join(self.ICC_CXXFLAGS))
            # put_string("LINKFLAGS :" + ' '.join(self.ICC_LINKFLAGS))
            # put_string("\nKEIL COMPILER")
            # put_string("ASFLAGS   :" + ' '.join(self.ARMCC_ASFLAGS))
            # put_string("CFLAGS   :" + ' '.join(self.ARMCC_CFLAGS))
            # put_string("CXXFLAGS  :" + ' '.join(self.ARMCC_CXXFLAGS))
            # put_string("LINKFLAGS :" + ' '.join(self.ARMCC_LINKFLAGS))



def cpu_id(s):
    s += 'z'
    ids = ''
    for c in s:
        if ord(c) >= ord('0') and ord(c) <= ord('9'):
            ids += c
        elif ids:
            return ids


def arm_cpu_id(s):
    if s.startswith('arm'):
        return s[3:]
    else:
        arr = s.split('-')
        if len(arr) > 1:
            return arr[1]

def riscv_cpu_id(cpu):
    a = cpu.lower()
    if a == 'rv32emc':
        return 'rv32'
    elif a == 'e902' or a == 'e902m' or a == 'e902t' or a == 'e902mt':
        return 'rv32_16gpr'
    elif a == 'e906':
        return 'rv32_32gpr'
    elif a == 'e906f':
        return 'rv32f_32gpr'
    elif a == 'e906fd':
        return 'rv32fd_32gpr'
    elif a == 'c906':
        return 'rv64_32gpr'
    elif a == 'c906fd' or a == 'c906v' or a == 'c910' or a == 'c910v':
        return 'rv64fd_32gpr'
    else:
        put_string("%s not support yet." % a)
        exit(0)
