# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


from __future__ import print_function
import os
import sys
import time
import hashlib
import zipfile
import codecs
import json
import re
import platform

try:
    from urlparse import urlparse
    import urllib
    import httplib as http
    urlretrieve = urllib.urlretrieve
    import urllib2 as urllib
except:
    from urllib.parse import urlparse
    import urllib.request
    import http.client as http
    urlretrieve = urllib.request.urlretrieve


try:
    import yaml
except:
    put_string("\n\nNot found pyyaml, please install: \nsudo pip install pyyaml")
    sys.exit(-1)

def is_contain_chinese(check_str):
    if sys.version_info.major == 2:
        if type(check_str) == str:
            check_str = check_str.decode('utf-8')
    zhPattern = re.compile(u'[\u4e00-\u9fa5]+')
    match = zhPattern.search(check_str)
    if match:
        return True

def is_leagal_name(name):
    pattern = re.compile(u'^[_a-zA-Z]\w*$')
    match = pattern.search(name)
    if match:
        return True

def string_len(text):
    L = 0
    R = ''
    if sys.version_info.major == 2:
        if type(text) == str:
            text = text.decode('utf8')
    for c in text:
        if ord(c) >= 0x4E00 and ord(c) <= 0x9FA5:
            L += 2
        else:
            L += 1
    return L


def put_string(*args, **keys):
    for a in args:
        if sys.version_info.major == 2:
            if type(a) == unicode:
                a = a.encode('utf8')
        if 'key' in keys:
            key = keys['key']
            color_print(a, key=key, end=' ', ignorecase=True)
        else:
            print(a, end=' ')
    print()


def color_print(text, key=[], end='\n', ignorecase=False):
    idx = {}
    itext = text
    if ignorecase:
        itext = text.lower()
    for k in key:
        index = 0
        while True:
            s = itext.find(k, index)
            if s >= 0:
                e = s + len(k)
                need_del = []
                for a, b in idx.items():
                    if max(s, a) <= min(e, b):
                        s = min(s, a)
                        e = max(e, b)
                        need_del.append(a)
                for v in need_del:
                    del idx[v]
                idx[s] = e
                index = e
            else:
                break
    s = 0
    for v in list(sorted(idx)):
        e = v
        print(text[s: e], end='')
        print('\033[1;31m' + text[v: idx[v]] + '\033[0m', end='')
        s = idx[v]
    print(text[s: len(text)], end=end)


def yaml_load(filename):
    try:
        with codecs.open(filename, 'r', encoding='UTF-8') as fh:
            text = fh.read()
            return yaml.safe_load(text)
    except Exception as e:
        put_string("(%s) in file:%s" % (str(e), filename))
        exit(-1)


def home_path(path=''):
    return os.path.join(os.path.expanduser('~'), path)


def http2git(url):
    conn = urlparse(url)
    url = 'git@' + conn.netloc + ':' + conn.path[1:]
    return url


def MD5(str):
    hl = hashlib.md5()
    hl.update(str.encode(encoding='utf-8'))
    return hl.hexdigest()


def http_request(method, url, data=None, headers={}):
    conn = urlparse(url)

    if conn.scheme == "https":
        connection = http.HTTPSConnection(conn.netloc)
    else:
        connection = http.HTTPConnection(conn.netloc)
    # connection.debuglevel = 1

    connection.request(method=method, url=conn.path + '?' +
                       conn.query, body=data, headers=headers)
    response = connection.getresponse()
    return response.status, response.read(), response.msg



def http_get(url, path):
    conn = urlparse(url)

    if conn.scheme == "https":
        connection = http.HTTPSConnection(conn.netloc)
    else:
        connection = http.HTTPConnection(conn.netloc)

    connection.request('GET', conn.path)
    response = connection.getresponse()

    filename = os.path.join(path, os.path.basename(conn.path))

    try:
        with codecs.open(filename, 'wb', encoding='UTF-8') as f:
            f.write(response.read())
    except:
        pass

    return filename


def wget(url, out_file):
    start_time = time.time()

    def barProcess(blocknum, blocksize, totalsize):
        speed = (blocknum * blocksize) / (time.time() - start_time)
        # speed_str = " Speed: %.2f" % speed
        speed_str = " Speed: %sB/S         " % format_size(speed)
        recv_size = blocknum * blocksize

        # 设置下载进度条
        f = sys.stdout
        percent = float(recv_size) / totalsize
        if percent > 1:
            percent = 1
        percent_str = " %.2f%%" % (percent * 100)
        n = int(percent * 50)
        s = ('#' * n).ljust(50, '-')
        f.write(percent_str.ljust(9, ' ') + '[' + s + ']' + speed_str)
        f.flush()
        f.write('\r')

    def format_size(bytes):
        bytes = float(bytes)
        kb = bytes / 1024
        if kb >= 1024:
            M = kb / 1024
            if M >= 1024:
                G = M / 1024
                return "%.3fG" % (G)
            else:
                return "%.3fM" % (M)
        else:
            return "%.3fK" % (kb)

    return urlretrieve(url, out_file, barProcess)


# make_archive(base_name, format, root_dir=None, base_dir=None, verbose=0,dry_run=0, owner=None, group=None, logger=None)
def dfs_get_zip_file(input_path, result):
    files = os.listdir(input_path)
    for file in files:
        if os.path.isdir(input_path + '/' + file):
            dfs_get_zip_file(input_path + '/' + file, result)
        else:
            result.append(input_path + '/' + file)


def version_inc(v, x):
    l = len(v)
    num_start = -1
    for i in range(l - 1, -1, -1):
        if num_start == -1:
            if v[i:i + 1].isdigit():
                num_start = i + 1
        else:
            if not v[i:i + 1].isdigit():
                s = v[i + 1:num_start]
                v2 = v.replace(s, str(int(s) + x))
                return v2

    return v + '-0'


def zip_path(input_path, zipName):
    if os.path.isdir(input_path):
        base = os.path.dirname(zipName)
        try:
            os.makedirs(base)
        except:
            pass
        predir = input_path.rsplit('/', 1)[0]
        f = zipfile.ZipFile(zipName, 'w', zipfile.ZIP_DEFLATED)
        filelists = []
        dfs_get_zip_file(input_path, filelists)
        for file in filelists:
            suffix = os.path.splitext(file)[-1]
            if suffix != '.d' and suffix != '.o':
                f.write(file, file.split(predir)[1])
        f.close()

def unzip_path(output_path, zipName):
    if os.path.exists(output_path):
        if os.path.isfile(output_path):
            os.remove(output_path)
            os.makedirs(output_path)
    else:
        os.makedirs(output_path)
    
    file = os.path.abspath(zipName)
    if zipfile.is_zipfile(file):
        try:
            with zipfile.ZipFile(file, "r") as myzip:
                badcrc = myzip.testzip()
                if badcrc:
                    put_string("Bad CRC for file %s of the zip archive" % badcrc)
                myzip.extractall(output_path)
                return True
        except Exception as e:
            put_string("Failed to install from zip file, error: %s!" % format(e))
    else:
        put_string("%s is not zip file" % file)
    return False

def write_file(text, filename):
    contents = None

    try:
        with codecs.open(filename, 'r', encoding='UTF-8') as f:
            contents = f.read()
    except:
        pass

    if text == contents:
        return
    try:
        p = os.path.dirname(filename)
        try:
            os.makedirs(p)
        except:
            pass

        with codecs.open(filename, 'w', encoding='UTF-8') as f:
            f.write(text)
    except:
        put_string("Generate %s file failed." % filename)


def genSConcript(path):
    file_name = os.path.join(path, 'SConscript')
    text = '''Import('defconfig')\ndefconfig.library_yaml()\n'''
    write_file(text, file_name)
    return file_name


def genSConstruct(components, path):
    text = """#! /bin/env python

from aostools.make import Make

defconfig = Make()

Export('defconfig')

defconfig.build_components()
defconfig.program()
"""

    comp_list = ''
    for component in components:
        if component != '.':
            comp_list += '    "' + component.name + '",\n'

    text = text % comp_list

    script_file = os.path.join(path, 'SConstruct')
    write_file(text, script_file)


def genMakefile(path):
    text = """CPRE := @
ifeq ($(V),1)
CPRE :=
endif


.PHONY:startup
startup: all

all:
	$(CPRE) scons -j4
	@echo YoC SDK Done


.PHONY:clean
clean:
	$(CPRE) rm -rf aos_sdk
	$(CPRE) scons -c
	$(CPRE) find . -name "*.[od]" -delete

%:
	$(CPRE) scons --component="$@" -j4
"""

    script_file = os.path.join(path, 'Makefile')
    write_file(text, script_file)


def save_yoc_config(defines, filename):
    contents = ""

    try:
        with codecs.open(filename, 'r', encoding='UTF-8') as f:
            contents = f.read()
    except:
        pass

    text = '''/* don't edit, auto generated by tools/toolchain.py */\n
#ifndef __YOC_CONFIG_H__
#define __YOC_CONFIG_H__
#ifndef CONFIG_CPU\n\n'''
    for k, v in defines.items():
        if v in ['y', 'Y']:
            text += '#define %s 1\n' % k
        elif v in ['n', 'N']:
            text += '// #define %s 1\n' % k
        elif type(v) == int:
            text += '#define %s %d\n' % (k, v)
        else:
            text += '#define %s "%s"\n' % (k, v)

    text += '\n#endif\n#endif\n'

    if text == contents:
        return False

    write_file(text, filename)


def save_csi_config(defines, filename):
    text = '''/* don't edit, auto generated by tools/toolchain.py */

#ifndef __CSI_CONFIG_H__
#define __CSI_CONFIG_H__

#include <yoc_config.h>

#endif

'''

    write_file(text, filename)

def get_cmpt_path_by_type(type):
    if type == 'board':
        path = os.path.join('hardware', 'board')
    elif type == 'chip':
        path = os.path.join('hardware', 'chip')
    elif type == 'arch':
        path = os.path.join('hardware', 'arch')
    elif type == 'drv_core':
        path = os.path.join('components', 'drivers', 'core')
    elif type == 'drv_peripheral':
        path = os.path.join('components', 'drivers', 'peripheral')
    elif type == 'drv_external_device':
        path = os.path.join('components', 'drivers', 'external_device')
    elif type == 'kernel':
        path = 'kernel'
    elif type == 'solution':
        path = 'solutions'
    elif type == 'document':
        path = '.'
    else:
        # common and sdk
        path = 'components'
    return path

def get_cmpt_top_path_by_type(type):
    if type == 'board' or type == 'chip' or type == 'arch':
        path = 'hardware'
    elif type == 'drv_core' or type == 'drv_peripheral' or type == 'drv_external_device':
        path = 'components'
    elif type == 'kernel':
        path = 'kernel'
    elif type == 'solution':
        path = 'solutions'
    elif type == 'document':
        path = '.'
    else:
        # common and sdk
        path = 'components'
    return path

def get_type_by_cmpt_path(path):
    path = os.path.abspath(path)
    dirname = os.path.dirname(path)
    name = os.path.basename(dirname)
    if name in ['board', 'chip', 'arch', 'kernel']:
        comp_type = name
    elif name == 'core':
        comp_type = 'drv_core'
    elif name == 'peripheral':
        comp_type = 'drv_peripheral'
    elif name == 'external_device':
        comp_type = 'drv_external_device'
    elif name == 'solutions':
        comp_type = 'solution'
    else:
        comp_type = 'common'
    return comp_type

def get_host_os():
    host_os = platform.system()
    if host_os == 'Windows':
        host_os = 'Win32'
    elif host_os == 'Linux':
        if platform.machine().endswith('64'):
            bit = '64'
        else:
            bit = '32'
        host_os += bit
    elif host_os == 'Darwin':
        host_os = 'OSX'
    else:
        host_os = None
    return host_os

def get_elf_bin_file(filename):
    elf_file = ''
    bin_file = ''
    if not os.path.isfile(filename):
        return elf_file, bin_file
    try:
        with codecs.open(filename, 'r', encoding='UTF-8') as fh:
            lines = fh.readlines()
            elf_pattern = re.compile('elf\s*=\s*[\'\"](.+?)[\'\"]')
            bin_pattern = re.compile('objcopy\s*=\s*[\'\"](.+?)[\'\"]')
            for line in lines:
                if line.startswith('defconfig'):
                    match = elf_pattern.search(line)
                    if match:
                        elf_file = match.group(1)
                    match = bin_pattern.search(line)
                    if match:
                        bin_file = match.group(1)
                    return elf_file, bin_file
    except Exception as e:
        put_string("(%s) in file:%s" % (str(e), filename))
        exit(-1)

def which(program, extra_path=None):
    if get_host_os() == 'Win32' and program.endswith('.exe') == False:
        program += '.exe'

    def is_exe(fpath):
        return os.path.isfile(fpath) and os.access(fpath, os.X_OK)

    fpath, fname = os.path.split(program)
    if fpath:
        if is_exe(program):
            return program
    else:
        paths = os.environ["PATH"].split(os.pathsep)
        if extra_path:
            paths += extra_path.split(os.pathsep)

        for path in paths:
            exe_file = os.path.join(path, program)
            if is_exe(exe_file):
                return exe_file
    return None

def get_mem_info(map_file):
    total_ro = 0
    total_rw = 0
    map_lines = []
    with open(map_file, 'r') as f:
        s = f.read()
        pattern_str = 'Linker script and memory map([\s\S]+?)OUTPUT\(\S*%s\s' % os.path.basename(map_file).replace('.map','.elf')
        # find memory map (without discard and debug sections)
        mem_map_list = re.findall(pattern_str, s)
        mem_map = '' if not mem_map_list else mem_map_list[0]
        if not mem_map:
            print('Can\'t parse memory info, memory info get fail!')
            return

        mem_map = mem_map.replace('\r', '')
        modules = []
        for l in mem_map.split('\n'):
            m = re.search(r'0x\w+\s+0x\w+\s+.+?([^/\\]+\.[ao])\((.+\.o)\)', l)
            if m == None:
                continue
            modules.append(m.groups()[0])
        modules = list(set(modules))

        # find sections address - length in memory map
        # modules = list(set(item[0] for item in re.findall('0x\w+\s+0x\w+\s+.+?([^/\\\]+\.[ao])(\(.+\.o\))?\r?\n', mem_map)))
        modules.sort(key = lambda x : x.upper())

        for module in modules:
            module = module.replace('+', r'\+')

            # get module's sections's address and size
            sec_text    = map(lambda arg : {'address':int(arg[0], 16), 'size':int(arg[1], 16)}, re.findall(r'\.text\.\S+\n?\s+(0x\w+)[ \t]+(0x\w+)[ \t]+.+[/\\]'+module+r'(\(.+\.o\))?\r?\n', mem_map))
            sec_rodata  = map(lambda arg : {'address':int(arg[0], 16), 'size':int(arg[1], 16)}, re.findall(r'\.rodata\.\S+\n?\s+(0x\w+)[ \t]+(0x\w+)[ \t]+.+[/\\]'+module+r'(\(.+\.o\))?\r?\n', mem_map))
            sec_data    = map(lambda arg : {'address':int(arg[0], 16), 'size':int(arg[1], 16)}, re.findall(r'\.data\.\S+\n?\s+(0x\w+)[ \t]+(0x\w+)[ \t]+.+[/\\]'+module+r'(\(.+\.o\))?\r?\n', mem_map))
            sec_bss     = map(lambda arg : {'address':int(arg[0], 16), 'size':int(arg[1], 16)}, re.findall(r'\.bss\.\S+\n?\s+(0x\w+)[ \t]+(0x\w+)[ \t]+.+[/\\]'+module+r'(\(.+\.o\))?\r?\n', mem_map))
            
            sec_ro      = list(sec_text) + list(sec_rodata);
            sec_rw      = list(sec_data) + list(sec_bss);

            ro_size = 0
            rw_size = 0
            for sec in sec_ro:
                ro_size += sec['size']
            for sec in sec_rw:
                rw_size += sec['size']

            total_ro += ro_size
            total_rw += rw_size

            map_lines.append(r'| %-40s | %-8d  | %-8d |'%(re.sub('\.[ao]','',module)[:40],ro_size,rw_size))

    print('\n                        AOS MEMORY MAP                            ')
    print('|=================================================================|')
    print('| %-40s | %-8s  | %-8s |'%('MODULE','RO Size','RW Size'))
    print('|=================================================================|')
    for line in map_lines:
        print(line)
    print('|=================================================================|')
    print('| %-40s | %-8d  | %-8d |'%('TOTAL (bytes)', total_ro, total_rw))
    print('|=================================================================|')

def change_dir_to_solution(yoc_path):
    def getSolutionAndBoard(yoc_path):
        config_file = os.path.join(yoc_path, ".config")
        if os.path.isfile(config_file) and os.path.isfile(os.path.join(yoc_path, ".aos")):
            conf = yaml_load(config_file)
            return conf["solution"], conf["board"]
        return None, None
    cur_dir = os.getcwd()
    # enter into solution directory if in the workspace root directory
    if cur_dir == yoc_path:
        solution, board = getSolutionAndBoard(yoc_path)
        if solution:
            new_dir = os.path.join(cur_dir, "solutions", solution)
            if os.path.isdir(new_dir):
                os.chdir(new_dir)
                print("change dir to %s." % new_dir)
    return cur_dir