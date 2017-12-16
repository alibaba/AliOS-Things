#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import sys,os,re
import time,logging

rootdirs = ['./board',         \
            './bootloader',    \
            './device',       \
            './example',       \
            './framework',     \
            './include',       \
            './kernel',        \
            './platform',      \
            './security',      \
            './test',          \
            './tools',         \
            './utility']

filterType = ['gif','png','bmp','jpg','jpeg','rar','zip',
            'ico','apk','ipa','doc','docx','xls','jar',
            'xlsx','ppt','pptx','pdf','gz','pyc','class']

filterOutType = ['h','c','cpp','s','S','ld','i']

num = 0
syscall_num = 0
symbol_list = []

# DEBUG < INFO < WARNING < ERROR < CRITICAL
logging.basicConfig(level=logging.WARNING)


def search_symbols(path=None, cont=None):
    if not path or not cont:
        print('path or searchString is empty')
        return
    _loopFolder(path, cont)

    return

def _loopFolder(path, cont):
    arr = path.split('/')
    if not arr[-1].startswith('.'):            #Do not check hidden folders
        if os.path.isdir(path):
            folderList = os.listdir(path)
            logging.debug(folderList)
            for x in folderList:
                _loopFolder(path + "/" + x, cont)
        elif os.path.isfile(path):
            _verifyContent(path, cont)

    return

def _verifyContent(path, cont):
    if path.split('.')[-1].lower() in filterType:
        return
    global num
    global symbol_list
    try:
        fh = open(path, 'r+')
        fhContent = fh.read()
        fh.close()
        symbols = re.findall(cont, fhContent, re.M | re.S)
        if symbols:
                logging.debug(symbols)
	        symbol_list.extend(symbols)
	        num += 1
                logging.debug("%s" % (path))
    except:
        print "File '" + path + "'can't be read"

    return

def _disableSyscall(sd_path):
    if os.path.exists(sd_path):
        fsn = open(sd_path, "r+")              # read from syscall_num
        sysdata = fsn.readlines()
        fsn.seek(0)
        for line in sysdata:
            u = line[-(len(line) - line.find((" "))):-1]
            logging.debug(u)
            line = r"%s %s" %(0, u.strip()) + "\n"
            fsn.write(line)
        fsn.close()

    return

def _writeSyscallData(sd_path):
    if os.path.exists(sd_path):
        fsd = open(sd_path, "r+")              # read from syscall_data
    else:
        fsd = open(sd_path, "w+")              # read from syscall_data
    sysdata = fsd.readlines()
    sysdata_num = len(sysdata)
    global symbol_list
    find = 0
    for symbol in symbol_list:                          # write to syscall_data
        for line in sysdata:
            if(re.findall(r"\d+\s\d+\s" + symbol[1] + r"\s\".*?\"\s\".*?\"\n", line, re.M | re.S)):
                serial_num = line.strip().split(" ", line.strip().count(" "))[1]
                newline = r"%s %s %s %s %s" %(1, serial_num, symbol[1], "\"" + symbol[0] + "\"", "\"" + symbol[2] + "\"") + "\n"
                find = 1
                logging.debug(newline)
                sysdata[sysdata.index(line)] = newline
                break
        if find == 0:
            line = r"%s %s %s %s %s" %(1, sysdata_num, symbol[1], "\"" + symbol[0] + "\"", "\"" + symbol[2] + "\"") + "\n"
            sysdata.append(line)
            sysdata_num += 1
        find = 0

    fsd.truncate(0)
    fsd.seek(0, 0)
    fsd.writelines(sysdata)
    fsd.flush()
    fsd.close()

    return

def _writeSyscallHeader(cr_path, sh_path, sd_path, sn_path, stype):
    fcr = open(cr_path, 'r')               # read copyright
    copyright = fcr.read()
    fcr.close()

    fsh = open(sh_path, "w+")              # creat syscall_tbl.h
    fsh.seek(0, 0)
    fsh.write(copyright)
    fsh.write("#define SYSCALL_MAX 0" + "\n")
    fsh.write("#define SYSCALL(nr, func) [nr] = func," + "\n\n")
    if stype == "kernel":
        fsh.write("const void *syscall_ktbl[] __attribute__ ((section(\".syscall_ktbl\"))) = {" + "\n")
    elif stype == "framework":
        fsh.write("const void *syscall_ftbl[] __attribute__ ((section(\".syscall_ftbl\"))) = {" + "\n")

    fsh.write("[0 ... SYSCALL_MAX - 1] = (void *)NULL," + "\n\n")

    fsn = open(sn_path, "w+")              # creat syscall_num.h
    fsn.seek(0, 0)
    fsn.write(copyright)

    if os.path.exists(sd_path):
        fsd = open(sd_path, "r+")              # read from syscall_data
    else:
        fsd = open(sd_path, "w+")              # read from syscall_data
    sysdata = fsd.readlines()
    global syscall_num
    syscall_num = len(sysdata)
    global symbol_list
    find = 0
    for symbol in symbol_list:                          # write to syscall_data
        for line in sysdata:
            if(re.findall(r"\d+\s\d+\s" + symbol[1] + r"\s\".*?\"\s\".*?\"\n", line, re.M | re.S)):
                serial_num = line.strip().split(" ", line.strip().count(" "))[1]
                # create syscall_xtbl.h
                strdef = "#define SYS_" + symbol[1].upper() + " " + serial_num + "\n"
                strsysc = "SYSCALL(SYS_" + symbol[1].upper() + ", " + symbol[1] + ")"
                fsh.write(strdef + strsysc + "\n")
                fsh.write("\n")

                # create syscall_xnum.h
                fsn.write(strdef)
                fsn.write("\n")
                find = 1
        if find == 0:
            logging.error(symbol[1] + " not find of h file!")
        find = 0

    fsh.write("};" + "\n")

    fsn.close()
    fsd.close()
    fsh.close()

    return

def _writeSyscallUapi(sc_path, sd_path, ui_path, stype):
    fui = open(ui_path, 'r')               # read xsyscall_include
    usys_incl = fui.read()
    fui.close()

    fsc = open(sc_path, "w+")              # creat xsyscall_tbl.c
    fsc.seek(0, 0)
    fsc.write(usys_incl)
    fsc.write("\n")

    if os.path.exists(sd_path):
        fsd = open(sd_path, "r+")              # read from syscall_data
    else:
        fsd = open(sd_path, "w+")              # read from syscall_data

    fsdContent = fsd.read()
    newsymbols = re.findall(r"(\d+)\s(\d+)\s(.*?)\s\"(.*?)\"\s\"(.*?)\"\n", fsdContent, re.M | re.S)
    find = 0
    global symbol_list
    for ele in symbol_list:                          # write to syscall_data
        for symbol in newsymbols:
            if ele[1] == symbol[2]:
                fsc.write(symbol[3].strip() + " " + symbol[2].strip() + "(")
                snum = 0
                seri = 0
                conti = 0
                variable_arg = 0
                args_num = symbol[4].strip().count(",")
                args = symbol[4].strip().split(",")
                if r"(*)" in symbol[4].strip():                          ######## fun((void (*)()))
                    for arg in args:
                        if r"(*)" in arg:
                            substr = arg.strip().split("(*")
                            fsc.write(substr[0] + "(*a" + str(snum) + substr[1])
                            snum += 1
                            if arg.count("(") != arg.count(")"):
                                conti = 1
                        else:
                            if conti == 1:
                                if r")" in arg:
                                    fsc.write(arg.strip())
                                    conti = 0
                                else:
                                    fsc.write(arg.strip())
                            else:
                                if arg.strip() == r"...":
                                    fsc.write(arg.strip())
                                    snum += 1
                                    variable_arg = 1
                                else:
                                    fsc.write(arg.strip() + " a" + str(snum))
                                    snum += 1
                        if seri != args_num:
                                fsc.write(", ")
                        seri += 1
                elif r"..." in symbol[4].strip():                        ######## fun(fmt, ...)
                    for arg in args:
                        if arg.strip() == r"...":
                            fsc.write(arg.strip())
                            snum += 1
                            variable_arg = 1
                        else:
                            fsc.write(arg.strip() + " a" + str(snum))
                            snum += 1
                        if seri != args_num:
                                fsc.write(", ")
                        seri += 1
                elif r"void" == symbol[4].strip():                       ######## fun(void)
                    fsc.write(symbol[4].strip())
                else:                                                    ######## normal
                    for arg in args:
                        fsc.write(arg.strip() + " a" + str(snum))
                        snum += 1
                        if seri != args_num:
                            fsc.write(", ")
                        seri += 1

                fsc.write(")" + "\n" + "{\n")
                if variable_arg == 1:
                    if snum < 2:
                        snum = 2
                    fsc.write("    va_list args;\n    char msg[128];\n    int ret;\n\n    memset(msg, 0, 128);\n\n")
                    fsc.write("    va_start(args, a" +  str(snum - 2) + ");\n    ret = vsnprintf(msg, 128, a" + str(snum - 2) + ", args);\n    va_end(args);\n\n")
                fsc.write(r"    if (SYSCALL_TBL[" + "SYS_" + symbol[2].upper() + "] != NULL) {\n" + "        ")
                needreturn = 0
                if symbol[3].strip() != r"void":
                    fsc.write("return ")
                    needreturn = 1
                fsc.write("SYS_CALL" + str(snum) + "(SYS_" + symbol[2].upper() + ", " + symbol[3].strip())

                snum = 0
                conti = 0
                if r"(*)" in symbol[4].strip():                          ######## fun((void (*)()))
                    for arg in args:
                        if r"(*)" in arg:
                            fsc.write(", " + arg.strip())
                            if arg.count("(") == arg.count(")"):
                                fsc.write(", " + "a" + str(snum))
                                snum += 1
                            else:
                                conti = 1
                        else:
                            if conti == 1:
                                if r")" in arg:
                                    fsc.write(", " + arg.strip() + ", " + "a" + str(snum))
                                    snum += 1
                                    conti = 0
                                else:
                                    fsc.write(", " + arg.strip())
                            else:
                                if arg.strip() == r"...":
                                    fsc.write(", " + "..." + "," + "msg")
                                else:
                                    fsc.write(", " + arg.strip() + ", " + "a" + str(snum))
                                    snum += 1
                elif r"..." in symbol[4].strip():                        ######## fun(fmt, ...)
                    for arg in args:
                        if arg.strip() == r"...":
                            fsc.write(", " + "..." + "," +  "msg")
                        else:
                            fsc.write(", " + arg.strip() + ", " + "a" + str(snum))
                            snum += 1
                else:                                                    ######## normal
                    if r"void" != symbol[4].strip():
                        for arg in args:
                            fsc.write(", " + arg.strip() + ", " + "a" + str(snum))
                            snum += 1

                fsc.write(r");")

                fsc.write("\n    } else {\n" + "        ")
                fsc.write("LOGE(\"BINS\", \"%s is NULL in SYSCALL_TBL\", __func__);\n")
                if needreturn == 1:
                    fsc.write("        return;\n")
                fsc.write("    }\n}\n\n")
                find = 1
        if find == 0:
            logging.error(ele[1] + " not find of c file!")
        find = 0

    fsc.close()

    return

def _writeKsyscallMk(sm_path):
    fsh = open(sm_path, "w+")              # creat ksyscall.mk
    fsh.seek(0, 0)
    fsh.write(r"NAME := syscall_kapi" + "\n\n")
    fsh.write(r"$(NAME)_TYPE := app&framework" + "\n\n")
    fsh.write(r"$(NAME)_INCLUDES := ./ ../../../framework/fsyscall/syscall_kapi" + "\n\n")
    fsh.write(r"$(NAME)_CFLAGS += -Wall -Werror" + "\n\n")
    fsh.write(r"$(NAME)_SOURCES := syscall_kapi.c" + "\n\n")
    fsh.write(r"GLOBAL_DEFINES += AOS_BINS" + "\n")

    fsh.close()

    return

def _writeFsyscallMk(sm_path):
    fsh = open(sm_path, "w+")              # creat fsyscall.mk
    fsh.seek(0, 0)
    fsh.write(r"NAME := syscall_fapi" + "\n\n")
    fsh.write(r"$(NAME)_TYPE := app" + "\n\n")
    fsh.write(r"$(NAME)_INCLUDES := ./ ../../../app/usyscall/syscall_fapi" + "\n\n")
    fsh.write(r"$(NAME)_CFLAGS += -Wall -Werror" + "\n\n")
    fsh.write(r"$(NAME)_SOURCES := syscall_fapi.c" + "\n\n")
    fsh.write(r"GLOBAL_DEFINES += AOS_BINS" + "\n")

    fsh.close()

    return

def _modifySyscallMax(sc_path, snum):
    fcr = open(sc_path, 'r+')               # read syscall_tbl.c
    tblc = fcr.readlines()
    fcr.seek(0)
    logging.debug(snum)
    for line in tblc:
        if(line.find(r"#define SYSCALL_MAX") == 0):
            line = r"#define SYSCALL_MAX %s" % (snum + 1) + "\n"
        fcr.write(line)
    fcr.close()

    return

def _removeSyscallData(sd_path):
    if os.path.exists(sd_path):
        logging.debug(sd_path)
        os.remove(sd_path)                     # remove syscall_num

    return

def _preCreateSyscall(path):
    fpath = open(path, "w+")
    fpath.close()
    return

def main():
    syscall_path = sys.argv[1]
    logging.info(sys.argv[1])

    # stage for create syscall, pre-create/create
    syscall_stage = sys.argv[2]
    logging.info(sys.argv[2])

    search_string = r"AOS_EXPORT\((.*?)\s*?\,\s*?[\\|\s]\s*?(\S*?)\s*?\,\s*?(.*?)\);$"
    syscall_data_path = r"./build/scripts/syscall_data"
    copyright_path = r"./build/copyright"
    ksearch_rootdirs = syscall_path + r"/precompile/kernel"
    ksearch_rootdirs_additional = r"kernel/ksyscall"
    fsearch_rootdirs = syscall_path + r"/precompile/framework"
    fsearch_rootdirs_additional = r"framework/fsyscall"

    ksyscall_kapi_path = syscall_path + r"/syscall/syscall_kapi/syscall_kapi.c"
    ksyscall_tbl_path = syscall_path + r"/syscall/syscall_kapi/syscall_ktbl.h"
    ksyscall_num_path = syscall_path + r"/syscall/syscall_kapi/syscall_knum.h"
    ksyscall_mk_path = syscall_path + r"/syscall/syscall_kapi/syscall_kapi.mk"
    ksyscall_data_path = r"./build/scripts/syscall_kdata"
    ksyscall_incl_path = r"./framework/fsyscall/syscall_kapi/syscall_kapi.h"

    fsyscall_fapi_path = syscall_path + r"/syscall/syscall_fapi/syscall_fapi.c"
    fsyscall_tbl_path = syscall_path + r"/syscall/syscall_fapi/syscall_ftbl.h"
    fsyscall_num_path = syscall_path + r"/syscall/syscall_fapi/syscall_fnum.h"
    fsyscall_mk_path = syscall_path + r"/syscall/syscall_fapi/syscall_fapi.mk"
    fsyscall_data_path = r"./build/scripts/syscall_fdata"
    fsyscall_incl_path = r"./app/usyscall/syscall_fapi.h"

    global symbol_list
    global num
    global syscall_num
    if syscall_stage == r"pre-create":
        starttime = time.time()

        # pre-create ksyscall
        _preCreateSyscall(ksyscall_kapi_path)
        _preCreateSyscall(ksyscall_tbl_path)
        _preCreateSyscall(ksyscall_num_path)
        _writeKsyscallMk(ksyscall_mk_path)
        # pre-create fsyscall
        _preCreateSyscall(fsyscall_fapi_path)
        _preCreateSyscall(fsyscall_tbl_path)
        _preCreateSyscall(fsyscall_num_path)
        _writeFsyscallMk(fsyscall_mk_path)

        # Search for each directory, find the symbol
        for rootdir in rootdirs:
            search_symbols(rootdir, search_string)

        # Remove duplicate element & Element sorting
        symbol_list = sorted(set(symbol_list),key=symbol_list.index)
        symbol_list = sorted(symbol_list, key=lambda fun: fun[1].lower())

        # set syscall serial num to 0
        _disableSyscall(syscall_data_path)

        # Creat and write to syscall_data
        _writeSyscallData(syscall_data_path)

        endtime = time.time()
        logging.info(" pre-create total time: %s s." % (endtime - starttime))

    elif syscall_stage == r"create":
        symbol_list = []
        num = 0

        starttime = time.time()

        ######################## ksyscall ##############################

        # Search for each directory, find the symbol
        search_symbols(ksearch_rootdirs, search_string)
        #search_symbols(ksearch_rootdirs_additional, search_string)

        # Remove duplicate element & Element sorting
        symbol_list = sorted(set(symbol_list),key=symbol_list.index)
        symbol_list = sorted(symbol_list, key=lambda fun: fun[1].lower())

        stype = "kernel"
        logging.info("======================================")
        logging.info(" ksyscall new symbol:")
        # Creat and write to syscall_ktbl.h syscall_knum.h syscall_kdata
        _writeSyscallHeader(copyright_path, ksyscall_tbl_path, syscall_data_path, ksyscall_num_path, stype)
        logging.info("======================================")

        # Creat and write to syscall_kapi.c
        _writeSyscallUapi(ksyscall_kapi_path, syscall_data_path, ksyscall_incl_path, stype)

        # Creat and write to ksyscall.mk
        #_writeKsyscallMk(ksyscall_mk_path)

        #modify SYSCALL_MAX
        _modifySyscallMax(ksyscall_tbl_path, syscall_num)

        print "======================================"
        print (" create ksyscall file:")
        print (" total: %s ksymbol find." % len(symbol_list))
        print (" total: %s file find." % num)
        print "--------------------------------------"

        ######################## fsyscall ##############################

        # reset global variable
        symbol_list = []
        num = 0

        # Search for each directory, find the symbol
        search_symbols(fsearch_rootdirs, search_string)
        #search_symbols(fsearch_rootdirs_additional, search_string)

        # Remove duplicate element & Element sorting
        symbol_list = sorted(set(symbol_list),key=symbol_list.index)
        symbol_list = sorted(symbol_list, key=lambda fun: fun[1].lower())

        stype = "framework"
        logging.info("======================================")
        logging.info(" fsyscall new symbol:")
        # Creat and write to syscall_ftbl.h syscall_fnum.h syscall_fdata
        _writeSyscallHeader(copyright_path, fsyscall_tbl_path, syscall_data_path, fsyscall_num_path, stype)
        logging.info("======================================")

        # Creat and write to syscall_fapi.c
        _writeSyscallUapi(fsyscall_fapi_path, syscall_data_path, fsyscall_incl_path, stype)
    
        # Creat and write to fsyscall.mk
        #_writeFsyscallMk(fsyscall_mk_path)

        #modify SYSCALL_MAX
        _modifySyscallMax(fsyscall_tbl_path, syscall_num)

        endtime = time.time()

        print (" create fsyscall file:")
        print (" total: %s fsymbol find." % len(symbol_list))
        print (" total: %s file find." % num)
        print "--------------------------------------"
        print (" total time: %s s." % (endtime - starttime))
        print "======================================"

if __name__ == "__main__":
    main()

