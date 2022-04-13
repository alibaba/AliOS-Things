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

qemu_csky_32 = ""
qemu_csky_64 = "http://yoctools.oss-cn-beijing.aliyuncs.com/csky-qemu-x86_64-Ubuntu-16.04-20200901-1526.tar.gz"

qemu_rv_32 = ""
qemu_rv_64 = "http://yoctools.oss-cn-beijing.aliyuncs.com/csky-qemu-x86_64-Ubuntu-16.04-20200901-1526.tar.gz"


all_toolchain_url = {
    'csky-qemu': [qemu_csky_32, qemu_csky_64],
    'riscv-qemu': [qemu_rv_32, qemu_rv_64]
}


class QemuYoC:
    def __init__(self):
        if os.getuid() != 0:
            self.basepath = home_path('.thead')
        else:
            self.basepath = '/usr/local/thead/'

    def download(self):
        qemu_path = os.path.join(self.basepath, self.qemu_arch)

        if os.path.exists(qemu_path):
            self.update_env()
            return

        architecture = platform.architecture()
        if architecture[0] == '64bit':
            qemu_url = all_toolchain_url[self.qemu_arch][1]
        else:
            qemu_url = all_toolchain_url[self.qemu_arch][0]

        tar_path = '/tmp/' + os.path.basename(qemu_url)
        if not qemu_url:
            put_string("Url is empty!")
            return
        put_string("Start to download qemu: %s" % self.qemu_arch)
        wget(qemu_url, tar_path)
        put_string("")
        put_string("Start install, wait half a minute please.")
        if tar_path.endswith('.bz2'):
            with tarfile.open(tar_path, 'r:bz2') as tar:
                tar.extractall(qemu_path)
        elif tar_path.endswith('.gz'):
            with tarfile.open(tar_path, 'r:gz') as tar:
                tar.extractall(qemu_path)
        else:
            put_string("%s extra not support." % tar_path)
            return

        os.remove(tar_path)
        if os.getuid() == 0:
            self.link_bin(qemu_path)
        else:
            self.update_env()
        put_string("Congratulations!")

    def link_bin(self, qemu_path):
        qemu_bin = os.path.join(qemu_path, 'bin')
        files = os.listdir(qemu_bin)

        for fil in files:
            p = os.path.join(qemu_bin, fil)
            if os.path.isfile(p):
                if os.stat(p).st_mode & (stat.S_IXUSR | stat.S_IXGRP | stat.S_IXOTH) != 0:
                    try:
                        os.symlink(p, os.path.join(
                            '/usr/bin', os.path.basename(p)))
                    except FileExistsError:
                        pass
                    except PermissionError:
                        put_string("Please use: sudo", ' '.join(sys.argv))
                        exit(-1)
                    except Exception as e:
                        pass

    def update_env(self):
        qemu_path = '$HOME/.thead/%s/bin' % self.qemu_arch
        shell = os.getenv('SHELL')
        if not shell:
            shell = 'bash'
        else:
            shell = os.path.basename(shell)

        if shell == 'zsh':
            rc = home_path('.zshrc')

        elif shell == 'bash':
            rc = home_path('.bashrc')

        with codecs.open(rc, 'r', 'UTF-8') as f:
            contents = f.readlines()

        export_path = ''
        for i in range(len(contents)):
            c = contents[i]
            idx = c.find(' PATH')
            if idx > 0:
                idx = c.find('=')
                if idx >= 0:
                    export_path = c[idx + 1:]

                    if export_path.find(qemu_path) < 0:
                        export_path = 'export PATH=' + qemu_path + ':' + export_path
                        contents[i] = export_path

        if not export_path:
            contents.insert(0, 'export PATH=' + qemu_path + ':$PATH\n\n')

        with codecs.open(rc, 'w', 'UTF-8') as f:
            contents = f.writelines(contents)

    def check_qemu(self, arch, verbose=0):
        if arch == 'csky':
            self.qemu_arch = 'csky-qemu'
        elif arch == 'riscv':
            self.qemu_arch = 'riscv-qemu'
        elif arch == 'arm':
            self.qemu_arch = 'arm-qemu'
            exe_file = 'qemu-system-arm'
        else:
            put_string("Not support %s qemu yet." % arch)
            exit(1)
        bin_file = self.check_program()

        if bin_file == '':
            if arch == 'arm':
                put_string('Please use shell cmd to install qemu as follow:\nsudo apt install qemu-system-arm')
                exit(1)            
            self.download()
            bin_file = self.check_program()
        else:
            if verbose == 1:
                put_string('warn: the toolchains was installed already, path = %s.' % bin_file)
        return bin_file

    def which(self, cmd):
        gcc = subprocess.Popen('which ' + cmd, shell=True,
                               stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        lines = gcc.stdout.readlines()
        for text in lines:
            text = text.decode().strip()
            info = 'which: no ' + os.path.basename(cmd) + ' in'
            if not text.find(info) >= 0:
                return text
        return ''

    def check_program(self):
        if self.qemu_arch == 'csky-qemu':
            exe_file = 'qemu-system-cskyv2'
        elif self.qemu_arch == 'riscv-qemu':
            exe_file = 'qemu-system-riscv32'
        elif self.qemu_arch == 'arm-qemu':
            exe_file = 'qemu-system-arm'

        path = self.which(exe_file)
        if not path:
            # put_string("Can't find %s" % exe_file)
            path = home_path('.thead/' + self.qemu_arch + '/bin/' + exe_file)
            path = self.which(path)
        return path
