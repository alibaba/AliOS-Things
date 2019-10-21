#!/usr/bin/env python

# Alios Things core_dump parser. Does some helpful:
# ---- stack backtrace analysis
# ---- ARM/Xtensa fault regs analysis
# ---- memory malloc fault analysis

#Notes:
# ---- support ARM Cortex-M and Xtensa

# Copyright (C) 2015-2019 Alibaba Group Holding Limited


import os
import sys
import subprocess
import argparse
import re
import logging
import operator

#--------------------------------------------------------------
# Global defines
#--------------------------------------------------------------
_TOOLCHIAN_PREFIX_        = "arm-none-eabi-"
_TOOLCHIAN_GCC_           = "arm-none-eabi-gcc"
_TOOLCHAIN_NM_            = "arm-none-eabi-nm"
_TOOLCHAIN_NM_OPT_        = "-nlCS"
_TOOLCHAIN_SIZE_          = "arm-none-eabi-size"
_TOOLCHAIN_SIZE_OPT_      = "-Axt"
_TOOLCHAIN_OBJDUMP_       = "arm-none-eabi-objdump"
_TOOLCHAIN_OBJDUMP_OPT_   = "-D"
_TOOLCHAIN_ADDR2LINE_     = "arm-none-eabi-addr2line"
_TOOLCHAIN_ADDR2LINE_OPT_ = "-pfiCe"
_TOOLCHAIN_READELF_       = "arm-none-eabi-readelf"
_TOOLCHAIN_READELF_OPT_   = "-h"

MATCH_ADDR = re.compile(r'0x[0-9a-f]{1,8}', re.IGNORECASE)


_START_MAGIC_EXCEPTION_   = "!!! Exception  !!!"
_START_MAGIC_FATAL_ERROR_ = "!!! Fatal Error !!!"
_END_MAGIC_EXCEPTION_     = "!! dump end   !!!"
_START_MAGIC_MM_FAULT_    = "all memory blocks"
_END_MAGIC_MM_FAULT_      = "all free memory blocks"

_MALLOC_CALLER_MAGIC_     = "fefefefe"

_ARCH_TYPE_ARM_         = "ARM"
_ARCH_TYPE_XTENSA_      = "Xtensa"

g_arch = [_ARCH_TYPE_ARM_, _ARCH_TYPE_XTENSA_]

g_arch_toolchain = {
    'arm'    : 'arm-none-eabi-gcc',
    'xtensa' : ['xtensa-lx106-elf-gcc', 'xtensa-esp32-elf-gcc']
}

#--------------------------------------------------------------
# Environment and dependency check
#--------------------------------------------------------------

def get_arch_from_elf(elf_file):
    if not elf_file:
        return ""

    arch_info = subprocess.check_output(
        [_TOOLCHAIN_READELF_, _TOOLCHAIN_READELF_OPT_, elf_file])

    for line in arch_info.splitlines():
        if 'Machine' in line:
            temp = line.split()
            for arch in g_arch:
                if arch in temp:
                    print "arch : " + arch
                    return arch
    return ""

#--------------------------------------------------------------
# class Core_Dump
#--------------------------------------------------------------

class Core_Dump(object):
    """docstring for Core_Dump"""
    def __init__(self, crash_log, elf_file, toolchain_path):
        super(Core_Dump, self).__init__()
        self.crash_log       = crash_log
        self.elf_file        = elf_file.name
        self.toolchain_path  = toolchain_path

        self.parse_addr_list = []

        self.sp         = ""
        self.parse_step = 0
        self.task_info  = []
        self.crash_type = ""
        self.arch       = _ARCH_TYPE_ARM_
        self.exc_num    = 0;

        self.arm_exc_reg    = {}
        self.xtensa_exc_reg = {}

        # mm info parse
        self.caller_list    = []
        self.caller_dictory = []

        # print flag
        self.print_flag = 0

        self.check_env()


    def find_pc_addr(self, pc_addr):
        try:
            pc_trans = subprocess.check_output([_TOOLCHAIN_ADDR2LINE_, _TOOLCHAIN_ADDR2LINE_OPT_, self.elf_file, pc_addr])
        except Exception as err:
            logging.exception(err)
        else:
            if not "?? ??:0" in pc_trans:
                print pc_trans

    def get_pc_addr(self, pc_addr):
        try:
            pc_trans = subprocess.check_output([_TOOLCHAIN_ADDR2LINE_, _TOOLCHAIN_ADDR2LINE_OPT_, self.elf_file, pc_addr])
        except Exception as err:
            logging.exception(err)
        else:
            if not "?? ??:0" in pc_trans:
                return pc_trans
            else:
                return "invalid"


    def get_value_form_line(self, line, index):
        val_list = re.findall(MATCH_ADDR, line)
        if val_list:
            if index > len(val_list):
                return ""
            return val_list[index]
        else:
            return ""

    def prase_addr(self, line, index):
        addr = self.get_value_form_line(line, index)
        if addr:
            self.parse_addr_list.append(addr)
            self.find_pc_addr(addr)

    def parse_malloc_caller(self, line):
        if _MALLOC_CALLER_MAGIC_ in line:
            caller_str = line[line.find(_MALLOC_CALLER_MAGIC_):]
            self.prase_addr(caller_str, 0)


    def show_hfsr_parser(self, hfsr):
        err_title = ""
        err       = ""

        if hfsr:
            err_title = " -- Hard Fault Occured --"
        print "HFSR :  " + hex(hfsr) + err_title

        if hfsr & 0x2:
            err += """
"VECTTBL" bit is set, potential reasons:
1. Indicates a Bus Fault on a vector table read during exception processing
2. Some errors in Vec-table setting
"""
        if hfsr & 0x40000000:
            err += """
"FORCED" bit is set, indicates a forced Hard Fault
Hard Fault handler must read the other fault status registers to find the cause of the fault.
"""
        if hfsr & 0x80000000:
            err += """
"DEBUG event" bit is set, occurred to a Hard Fault.
When writing to the register you must write 0 to this bit,
otherwise behavior is unpredictable
"""
        print err


    def show_bfsr_parser(self, bfsr, bfar):
        err_title = ""
        err       = ""

        if bfsr:
            err_title = " -- Bus Fault Occured --"

        print "BFSR :  " + hex(bfsr) + err_title

        if bfsr & 0x200:
            if bfsr & 0x8000:
                err += """
A precise data access error has occurred. Faulting address: {bfar_val}
""".format(bfar_val = bfar)
            else:
                err += """
A precise data access error has occurred. WARNING: Fault address in BFAR is NOT valid
"""

        if bfsr & 0x100:
            err += """
Bus fault on an instruction prefetch has occurred.
Potential reasons:
1. Branch to invalid memory regions for example caused by incorrect function pointers
2. Invalid return due to corrupted sp or stack content
3. Incorrect entry in the exception vector table
"""

        if bfsr & 0x400:
            err += """
Imprecise data access error has occurred
"""

        if bfsr & 0x800:
            err += """
UNSTKERR: Stack pop (for an exception return) has caused one or more BusFaults.
Potential reasons:
1. SP is corrupted during exception handling
"""

        if bfsr & 0x1000:
            err += """
STKERR: Stack push (for an exception entry) has caused one or more BusFaults.
Potential reasons:
1. SP is corrupted
2. Stack overflow
3. PSP is used without initial
"""

        if bfsr & 0x2000:
            err += """
LSPERR: bus fault occurred during FP lazy state preservation(only when FPU present)
"""
        print err



    def show_mfsr_parser(self, mfsr, mfar):
        err_title = ""
        err       = ""

        if mfsr:
            err_title = " -- MemManage Fault Occured --"

        print "MFSR :  " + hex(mfsr) + err_title

        if mfsr & 0x2:
            if mfsr & 0x80:
                err += """
Data access violation, The processor attempted a load or store at a location that does not permit the operation
Faulting address: {mfar_val}
""".format(mfar_val = mfar)
            else:
                err += """
Data access violation, The processor attempted a load or store at a location that does not permit the operation
Fault address in MMFAR is NOT valid
"""

        if mfsr & 0x1:
            err += """
MPU or Execute Never (XN) default memory map access violation on an instruction fetch has occurred
Potential reasons:
1. Branch to regions that are not defined in the MPU or defined as non-executable.
2. Invalid return(EXC_RETURN) due to corrupted stack content
3. Incorrect entry in the exception vector table
4. During the exception handling, the PC value on the stack was destroyed
"""

        if mfsr & 0x8:
            err += """
UNSTKERR: Stack pop (for an exception return) has caused one or more access violations.
Potential reasons:
1. SP is corrupted during exception handling
2. MPU region for the stack changed during exception handling
"""

        if mfsr & 0x10:
            err += """
STKERR: Stack push (for an exception entry) has caused one or more access violations.
Potential reasons:
1. SP is corrupted or not initialized
2. Stack is reaching a region(overflow) not defined by the MPU as read/write memory
"""

        if mfsr & 0x20:
            err += """
LSPERR: MemManage  occurred during FP lazy state preservation(only Cortex-M4 with FPU)
"""
        print err


    def show_ufsr_parser(self, ufsr):
        err_title = ""
        err       = ""

        if ufsr:
            err_title = " -- UsageFault Fault Occured --"
        print "UFSR :  " + hex(ufsr) + err_title

        if ufsr & 0x1:
            err += """
UNDEFINSTR: Undefined instruction
The processor has attempted to execute an undefined instruction
Potential reasons:
1. Use of instructions not supported in the Cortex-M device
2. Bad or corrupted memory contents(.data segment)
3. ARM target code is loaded when linked,please check Compile&Link setting
4. Instruction alignment problem. For example, when using the GNU toolchain,
   forgetting to use .align after .ascii may result in the next instruction not being aligned
"""

        if ufsr & 0x2:
            err += """
INVSTATE: Instruction executed with invalid EPSR.T or EPSR.IT field
This may be caused by Thumb bit not being set in branching instruction
Potential reasons:
1. Loading a branch target address to PC with LSB=0
2. Vector table contains a vector address with LSB=0
3. Stacked PSR corrupted during exception or interrupt handling,
   causes the kernel to try to enter the ARM state when return
"""

        if ufsr & 0x4:
            err += """
INVPC: Invalid EXC_RETURN value
Processor has attempted to load an illegal EXC_RETURN value to the PC as a result of an invalid context switch
Potential reasons:
1. Invalid EXC_RETURN value when exception return, for example:
       return thread model when EXC_RETURN=0xFFFF_FFF1
       return handler model when EXC_RETURN=0xFFFF_FFF9
2. Invalid return due to corrupted SP/LR, or stack content
3. ICI/IT bit in PSR invalid for an instruction
"""

        if ufsr & 0x8:
            err += """
NOCP: No coprocessor
The processor does not support coprocessor instructions
Potential reasons:
The processor has attempted to access a coprocessor that does not exist,
please checkout PC
"""

        if ufsr & 0x100:
            err += """
UNALIGNED: Unaligned access error has occurred
Enable trapping of unaligned accesses by setting the UNALIGN_TRP bit in the CCR.
Unaligned LDM/STM/LDRD/STRD instructions always fault irrespective of the setting of UNALIGN_TRP bit.
"""

        if ufsr & 0x200:
            err += """
DIVBYZERO: Divide by zero error has occurred
Enable trapping of divide by zero by setting the DIV_0_TRP bit in the CCR
"""
        print err


    def show_arm_exc_regs_parser(self, exc_reg):
        if len(exc_reg) == 0:
            return

        cfsr_val = exc_reg['cfsr']
        hfsr_val = exc_reg['hfsr']
        mfar_val = exc_reg['mfar']
        bfar_val = exc_reg['bfar']
        afsr_val = exc_reg['afsr']

        print "\n========== Show Exc Regs Info  =========="

        self.show_hfsr_parser(int(hfsr_val, 16))

        ufsr_val = (int(cfsr_val, 16) & 0xFFFF0000) >> 16
        bfsr_val =  int(cfsr_val, 16) & 0x0000FF00
        mfsr_val =  int(cfsr_val, 16) & 0x000000FF

        print "CFSR =  (UFSR + BFSR + MFSR)"
        self.show_bfsr_parser(bfsr_val, bfar_val)
        self.show_mfsr_parser(mfsr_val, mfar_val)
        self.show_ufsr_parser(ufsr_val)

        if self.exc_num == 12:
            print "DebugMon Exception!"


    def show_xtensa_exc_regs_parser(self, exc_reg):
        if len(exc_reg) == 0:
            return

        exc_cause = exc_reg['exccause']
        exc_addr  = exc_reg['excaddr']

        print "\n========== Show Exc Regs Info  =========="
        print "EXCCAUSE : " + exc_cause
        print "EXCADDR  : " + exc_addr

        exc_cause_val = int(exc_cause, 16) & 0x3F

        err = ""
        if exc_cause_val == 0x1:
            err += """
Invalid command. Potential reasons:
1. Damaged BIN binaries
2. Wild pointers
"""

        elif exc_cause_val == 0x6:
            err += """
Division by zero
"""

        elif exc_cause_val == 0x9:
            err += """
Unaligned read/write operation addresses
Potential reasons:
1. Unaligned read/write Cache addresses
2. Wild pointers
"""

        elif exc_cause_val == 0x1C or exc_cause_val == 0x1D:
            err += """
A load/store referenced a page mapped with an attribute that does not permit
Potential reasons:
1. Access to Cache after it is turned off
2. Wild pointers
"""
        print err


    def show_segment_size_info(self):
        print "\r\n========== Show Segment Size Info  =========="
        print "arch: " + self.arch
        segment_size_info = subprocess.check_output(
                [_TOOLCHAIN_SIZE_, _TOOLCHAIN_SIZE_OPT_, self.elf_file])
        print segment_size_info


    def show_task_info(self):
        if self.crash_type:
            if self.crash_type == 'task':
                if self.sp:
                    sp_val = int(self.sp, 16)
                    if len(self.task_info):
                        for task in self.task_info:
                            task_stack_addr = int(task['task_stack_addr'], 16)
                            task_stack_size = int(task['task_stack_size'], 16)
                            if sp_val >= task_stack_addr and sp_val <= (task_stack_addr + task_stack_size):
                                crash_task_name = task['task_name']
                                break
                        print "Crash in task : {task_name}\r\n".format(task_name=crash_task_name)
            else:
                print "Crash in intertupt"


    def show_statistic_info(self):
        #self.show_segment_size_info()
        self.show_task_info()


    def parse_task(self, line):
        if "TaskName" in line or "---" in line or "===" in line:
            return

        m = re.search(r'[A-Za-z]', line)
        if m:
            task_str = line[line.find(m.group(0)):]
            task_list = task_str.split()

            if len(task_list) >= 5:
                task_info = {'task_name'         : task_list[0],
                             'task_state'        : task_list[1],
                             'task_prio'         : task_list[2],
                             'task_stack_addr'   : task_list[3],
                             'task_stack_size'   : task_list[4][0:10],
                             'task_stack_minfree': task_list[4][11:21]
                            }
                self.task_info.append(task_info)


    def check_env(self):
        global _TOOLCHIAN_GCC_
        global _TOOLCHAIN_ADDR2LINE_

        if sys.version_info.major > 2:
            error = """
            This parser tools do not support Python Version 3 and above.
            Python {py_version} detected.
            """.format(py_version=sys.version_info)

            print error
            sys.exit(1)

        # addr2line supported by esp32 and esp8266 could not work well, so
        # use "arm-none-eabi-addr2line" instead
        '''
        if "esp32" in self.elf_file or "esp8266" in self.elf_file:
            self.arch = _ARCH_TYPE_XTENSA_

        if self.arch == _ARCH_TYPE_XTENSA_:
            if "esp32" in self.elf_file:
                _TOOLCHIAN_GCC_       = "xtensa-esp32-elf-gcc"
                _TOOLCHAIN_ADDR2LINE_ = "xtensa-esp32-elf-addr2line"
            elif "esp8266" in self.elf_file:
                _TOOLCHIAN_GCC_       = "xtensa-lx106-elf-gcc"
                _TOOLCHAIN_ADDR2LINE_ = "xtensa-lx106-elf-addr2line"
        '''
        cmd = _TOOLCHIAN_GCC_;
        if os.name == 'nt':
            cmd = "where " + cmd
        else:
            cmd = "which " + cmd

        retcode = subprocess.call(cmd, shell=True)
        if retcode:
            if not self.toolchain_path:
                error = """
    Can not find toolchian "{magic}" path
    Please set PATH by:
        export PATH=$PATH: ../build/compiler/../bin/
    or:
    use "-p" point to absolute toolchain path, ex:

        python coredump.py {log} {elf} -p {path}
    """.format(magic=_TOOLCHIAN_GCC_, log=self.crash_log.name, elf=self.elf_file, path="../build/compiler/../bin/")

                print error
                sys.exit(1)
            else:
                if not str(self.toolchain_path).endswith('/'):
                    self.toolchain_path = self.toolchain_path + '/'
                _TOOLCHAIN_ADDR2LINE_ = self.toolchain_path + _TOOLCHAIN_ADDR2LINE_


    def heap_parse(self, mem_info):
        m = re.search(MATCH_ADDR, mem_info)
        if m:
            mm_str = mem_info[mem_info.find(m.group(0)):]

            blk_list_all = re.findall(r'(\w+)\s+(\w+)\s+(\d+)\s+(\w+)\s+(\w+).*', mm_str)
            blk_list_all = map(lambda arg : {'Addr':arg[0], 'State':arg[1], 'Size':int(arg[2], 10), 'Caller':arg[4]}, blk_list_all)
            #print blk_list_all

            one_blk_info = blk_list_all

            if not one_blk_info:
                return
            elif one_blk_info[0]['State'] != 'used' or one_blk_info[0]['Caller'] == '0x0':
                return

            caller_addr = one_blk_info[0]['Caller']
            if caller_addr not in self.caller_list:
                idx = len(self.caller_dictory)
                self.caller_dictory.append({'Addr':caller_addr, 'Total Size':0, 'Blk Cnt':0, 'Func':0, 'File':0, 'Line':0})
                self.caller_list.append(caller_addr)
            else:
                idx = self.caller_list.index(caller_addr)

            self.caller_dictory[idx]['Blk Cnt']    += 1
            self.caller_dictory[idx]['Total Size'] += one_blk_info[0]['Size']


    def get_heap_statistic_info(self):

        #print self.caller_list
        #print self.caller_dictory

        if not self.caller_dictory:
            return

        print "\r\n========== Show MM Statistic Info  =========="

        for caller_info in self.caller_dictory:
            info_str = self.get_pc_addr(caller_info['Addr'])
            # e.g.: mbedtls_calloc at /workspace/aliyun/aos_rda/security/mbedtls/src/mbedtls_alt.c:151\r\n

            if info_str == "invalid":
                continue
            info_get = re.findall(r'([\w\?]+)(?: | at )(\S*):(\d*)', info_str)
            if not info_get:
                caller_info['Func'] = info_str
            else:
                caller_info['Func'] = info_get[0][0]
                caller_info['File'] = info_get[0][1]
                caller_info['Line'] = info_get[0][2]

        self.caller_dictory = sorted(self.caller_dictory, key=operator.itemgetter('Total Size'))

        #print outputs
        print('-'*158)
        title = ['Alloc Addr', 'Func', 'Cnt', 'Total Size', 'Line', 'File']
        str_f = '{:^12} | {:^24} | {:^4} | {:^12} | {:^8} | {:^16}'
        str_f2 = '{Addr:^12} | {Func:^24} | {Bc:^4} | {Tz:^12} | {Line:^8} | {File:^16}'
        print str_f.format(*title)
        print('-'*158)
        for mm_dict in self.caller_dictory:
            print str_f2.format(Addr=mm_dict['Addr'], Func=mm_dict['Func'], Bc=mm_dict['Blk Cnt'], Tz=mm_dict['Total Size'], Line=mm_dict['Line'], File=mm_dict['File'])
        print('-'*158)


    def open_print_line(self):
        self.print_flag = 1

    def close_print_line(self):
        self.print_flag = 0

    def get_print_status(self):
        return self.print_flag

    def show(self):

        log_lines = iter(self.crash_log.read().splitlines())

        for line in log_lines:

            if _START_MAGIC_EXCEPTION_ in line:
                self.open_print_line()
                continue

            if _END_MAGIC_EXCEPTION_ in line:
                self.close_print_line()
                print "\r\n******************** Alios Things Exception Core Dump Result ********************"

                if self.arch == _ARCH_TYPE_ARM_:
                    self.show_arm_exc_regs_parser(self.arm_exc_reg)

                elif self.arch == _ARCH_TYPE_XTENSA_:
                    self.show_xtensa_exc_regs_parser(self.xtensa_exc_reg)

                else:
                    pass

                self.show_statistic_info()

            if self.get_print_status() == 1:
                print line

            #begin to parse one line

            if "backtrace" in line:
                if "interrupt" in line:
                    self.crash_type = "interrupt"
                elif "task" in line:
                    self.crash_type = "task"
                else:
                    self.prase_addr(line, 0)

            elif _START_MAGIC_MM_FAULT_ in line:
                #self.parse_malloc_caller(line)
                self.parse_step = 0xaa
                continue

            elif _END_MAGIC_MM_FAULT_ in line:
                self.parse_step = 0
                self.get_heap_statistic_info()
                continue

            elif self.parse_step == 0xaa:
                self.heap_parse(line)

            elif "Task Info" in line:
                self.parse_step = 1

            elif "Queue Info" in line:
                self.parse_step = 2

            elif self.parse_step == 1:
                self.parse_task(line)

            elif "SP" in line or "A1 " in line:
                self.sp = self.get_value_form_line(line, 0)

            elif self.arch == _ARCH_TYPE_ARM_:
                if "CFSR" in line:
                    cfsr_val = self.get_value_form_line(line, 0)
                    self.arm_exc_reg['cfsr'] = cfsr_val

                elif "HFSR" in line:
                    hfsr_val = self.get_value_form_line(line, 0)
                    self.arm_exc_reg['hfsr'] = hfsr_val

                elif "BFAR" in line:
                    bfar_val = self.get_value_form_line(line, 0)
                    self.arm_exc_reg['bfar'] = bfar_val

                elif "MMFAR" in line:
                    mfar_val = self.get_value_form_line(line, 0)
                    self.arm_exc_reg['mfar'] = mfar_val

                elif "AFSR" in line:
                    afsr_val = self.get_value_form_line(line, 0)
                    self.arm_exc_reg['afsr'] = afsr_val

                elif "EXC_NUM" in line:
                    excnum = self.get_value_form_line(line, 0)
                    self.exc_num = int(excnum, 16)
                else:
                    pass

            elif self.arch == _ARCH_TYPE_XTENSA_:
                if line.startswith("EXCCAUSE"):
                    exc_cause_val = self.get_value_form_line(line, 0)
                    self.xtensa_exc_reg['exccause'] = exc_cause_val

                elif line.startswith("EXCVADDR"):
                    exc_addr_val = self.get_value_form_line(line, 0)
                    self.xtensa_exc_reg['excaddr'] = exc_addr_val
                else:
                    pass

            else:
                pass

#--------------------------------------------------------------
# Main
#--------------------------------------------------------------

def main():

    parser = argparse.ArgumentParser(description='Alios Things crash log core dump')

    # specify arguments
    parser.add_argument(metavar='CRASH LOG', type=argparse.FileType('rb', 0),
                        dest='crash_log', help='path to crash log file')

    parser.add_argument(metavar='ELF FILE', type=argparse.FileType('rb', 0),
                        dest='elf_file', help='elf file of application')

    parser.add_argument('-p','--path', help="absolute path of build/compiler/../bin", default='')

    args = parser.parse_args()

    # parser core_dump
    core_dump_ins = Core_Dump(args.crash_log, args.elf_file, args.path)

    core_dump_ins.show()

    #close all files
    if args.crash_log:
        args.crash_log.close()

    if args.elf_file:
        args.elf_file.close()


if __name__ == "__main__":
    main()
