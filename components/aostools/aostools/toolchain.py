# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


from __future__ import print_function

import os
import stat
import tarfile
import subprocess
import platform
import codecs

from .tools import *
from .gitproject import *


all_toolchain_url = {
    'Win32':{
    'csky-abiv2-elf': '',
    'riscv64-unknown-elf': '',
    'arm-none-eabi': 'https://gitee.com/alios-things/gcc-arm-none-eabi-win32.git'
    }, 
    'Linux32':{
    'csky-abiv2-elf': 'http://yoctools.oss-cn-beijing.aliyuncs.com/csky-elfabiv2-tools-i386-minilibc-20200407.tar.gz',
    'riscv64-unknown-elf': 'http://yoctools.oss-cn-beijing.aliyuncs.com/riscv64-elf-i386-20200820.tar.gz',
    'arm-none-eabi': ''
    }, 
    'Linux64':{
    'csky-abiv2-elf': 'http://yoctools.oss-cn-beijing.aliyuncs.com/csky-elfabiv2-tools-x86_64-minilibc-20200407.tar.gz',
    'riscv64-unknown-elf': 'http://yoctools.oss-cn-beijing.aliyuncs.com/riscv64-elf-x86_64-20200820.tar.gz',
    'arm-none-eabi': 'https://gitee.com/alios-things/gcc-arm-none-eabi-linux.git -b aos'
    }, 
    'OSX':{
    'csky-abiv2-elf': '',
    'riscv64-unknown-elf': '',
    'arm-none-eabi': 'https://gitee.com/alios-things/gcc-arm-none-eabi-osx.git -b aos'
    }
}


class ToolchainYoC:
    def __init__(self):
        self.basepath = home_path('.aliot')

    def download(self, arch):
        toolchain_path = os.path.join(self.basepath, arch)

        host_os = get_host_os()
        if arch not in all_toolchain_url[host_os]:
            put_string("Can not find toolchain for %s!" % arch)
            return

        if os.path.exists(toolchain_path):
            shutil.rmtree(toolchain_path)
            put_string("remove corrupt directory %s." % toolchain_path)
        
        toolchain_url = all_toolchain_url[host_os][arch]
        if not toolchain_url:
            put_string("Url is empty!")
            return
        put_string("Start to download toolchain: %s" % arch)
        pattern = re.compile("(.*)\s+\-b\s+(.*)")
        branch = "master"
        match = pattern.match(toolchain_url)
        if match:
            toolchain_url = match.group(1)
            branch = match.group(2)
        if toolchain_url.endswith('.git'):
            prj = GitRepo(toolchain_path, toolchain_url)
            put_string("clone %s -b %s ..." % (toolchain_url, branch))
            prj.pull(branch, None)
        else:
            if host_os == "Win32":
                tar_path = os.path.join(os.environ.get('TEMP'), os.path.basename(toolchain_url))
            else:
                tar_path = os.path.join('/tmp', os.path.basename(toolchain_url))
            wget(toolchain_url, tar_path)
            put_string("")
            put_string("Start install, wait half a minute please.")
            if tar_path.endswith('.bz2'):
                with tarfile.open(tar_path, 'r:bz2') as tar:
                    tar.extractall(toolchain_path)
            elif tar_path.endswith('.gz'):
                with tarfile.open(tar_path, 'r:gz') as tar:
                    tar.extractall(toolchain_path)
            else:
                put_string("%s extra not support." % tar_path)
                os.remove(tar_path)
                return

            os.remove(tar_path)
        put_string("Congratulations!")

    def check_toolchain(self, arch='csky-abiv2-elf', verbose=0):
        bin_file = self.check_program(arch)
        if not bin_file:
            self.download(arch)
            bin_file = self.check_program(arch)
        else:
            if verbose == 1:
                put_string('warn: the toolchains was installed already, path = %s.' % bin_file)
        return bin_file

    def check_program(self, arch='csky-abiv2-elf'):
        """ check PATH, ~/.aliot/<arch>/bin,  ~/.aliot/<arch>/main/bin"""
        path = which(arch + '-gcc')
        if arch.startswith('arm-none-eabi'):
            # arm-none-eabi toolchain updated!
            path = ""

        if not path:
            path = os.path.join(self.basepath, arch, "bin", arch + '-gcc')
            path = which(path)
            if not path:
                path = os.path.join(self.basepath, arch, "main", "bin", arch + '-gcc')
                path = which(path)
        return path

    def uninstall_toolchain(self, arch='csky-abiv2-elf'):
        path = which(arch + '-gcc')
        if arch.startswith('arm-none-eabi'):
            # arm-none-eabi toolchain updated!
            path = ""
        if not path:
            path = os.path.join(self.basepath, arch, "bin", arch + '-gcc')
            path = which(path)
            if not path:
                path = os.path.join(self.basepath, arch, "main", "bin", arch + '-gcc')
                path = which(path)
            if path:
                shutil.rmtree(os.path.join(self.basepath, arch))
                put_string("The toolchain \"%s\" is uninstalled." % path)
            else:
                put_string("The toolchain \"%s\" is not existed." % arch)
        else:
            put_string("The toolchain \"%s\" is in your PATH!\nPlease remove it manually if you really want." % path)
