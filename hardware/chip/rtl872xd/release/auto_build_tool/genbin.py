#!/usr/bin/python
# -*- coding: utf-8 -*-
#本文件位于\AliOS-Things\platform\mcu\haas1000\release\auto_build_tool文件夹下
#本文件用于适配python3.x版本，在3.7.3版本下测试通过。其他版本未测试。
#修改本文件的目的是解决3.x版本下，使用aos make命令编译固件时发生的两个错误
#第一个错误是“missing parentheses in call to 'print'”
#第二个错误是“module ‘string’ has no attribute 'find'"

#与阿里云提供的haashaas1000_genbin.py原版相比，本文件一共修改了4处，详见注释。
#花生，2020年10月6日

import os
import sys
#import struct
import time
#import yaml
import string
import subprocess
#from time import strftime,gmtime
import os,os.path
import zipfile
import platform
import shutil
import glob

osstr = platform.system()

def pack_dir(zip_dir,zip_file):
    filelist = []
    if os.path.isfile(zip_dir):
        filelist.append(zip_dir)
    else :
        filelist = os.listdir(zip_dir)
    zf = zipfile.ZipFile(zip_file, "w", zipfile.zlib.DEFLATED)
    print('zip file')
    for file in filelist:
        #print arcname
        print(zip_dir + '/' + file)
        zf.write(file)
    zf.close()

flag = sys.argv[1]
f_cfg = open('haas_genbin_config.yaml')

output_dir = sys.argv[2]
cfg_dict = {}
print("%s" % output_dir)
dict1_name_list = []
dict2_name_list = []

num_of_first_items = -1
num_of_second_items = -1
is_list = False
file_num = -1
sign_file_tmp = []

while True:
    line = f_cfg.readline()
    if not line:
        break

    print("%s" %(line))

    arry_parm = line.split('-', 1 )

    if len(arry_parm) == 2:
        is_list = True
    else :
        arry_parm = line.split(':', 1 )
        is_list = False

    para_key = ''
    para_val = ''
    if len(arry_parm) == 2:
        para_key = arry_parm[0]
        para_val = arry_parm[1]

    print("para_key %s"  % para_key)
    print("para_val %s"  % para_val)

    if para_key[0:1].isspace() == False:
        num_of_first_items = num_of_first_items + 1
        if para_val != '\r\n' and para_val != '\n':
            #花生，2020年10月6日17:32:39，下面的语句，原版本是print "format wrong"，要想在3.x版本支持，需要加括号。
            #否则会提示"missing parentheses in call to print"错误。下面若干个print也是相同处理方式
            print ("format wrong")
            break

        dict1_name_list.append(para_key);

        cfg_dict[dict1_name_list[num_of_first_items]] = {}

        if dict1_name_list[num_of_first_items] == 'SIGN_FILE_LIST':
            cfg_dict[dict1_name_list[num_of_first_items]] = []

    else:

        cfg_tmp = cfg_dict[dict1_name_list[num_of_first_items]]

        if dict1_name_list[num_of_first_items] == 'SIGN_FILE_LIST':
            if is_list == True and para_val.strip()[-1:] == ':':
                file_num = file_num + 1
                cfg_tmp.append({})
                continue

            if para_key.strip() == 'SWITCH':
                cfg_tmp[file_num][para_key.strip()] = para_val.strip()
                continue

            # print para_key.strip()
            # print para_val.strip()[1:-1]
            cfg_tmp[file_num][para_key.strip()] = para_val.strip()[1:-1]
            continue

        if para_val == '\r\n' or para_val == '\n':
            is_list = True
            num_of_second_items = num_of_second_items + 1
            dict2_name_list.append(para_key[2:])
            cfg_tmp[dict2_name_list[num_of_second_items]] = []
            #print cfg_dict
            continue

        if is_list == True & para_key[:3].isspace() == True :
            if para_val[0].isspace() == True:

                cfg_tmp[dict2_name_list[num_of_second_items]].append(para_val.strip()[1:-1])
                #print cfg_dict
                continue
           #花生，2020年10月6日17:30:53，str在原版本是string，在2.x版本中是可以的，但是3.x版本中，string没有find属性
        if str.find(para_key,'SWITCH') != -1:
            cfg_tmp[para_key[2:]] = para_val.strip()
            continue

        cfg_tmp[para_key[2:]] = para_val.strip()[1:-1]
        #print cfg_tmp
        #print cfg_dict

f_cfg.close()


# BES_KEY
dict_bes_key = cfg_dict['BES_KEY']
in_key_dir = dict_bes_key['IN_KEY_DIR']
out_key_dir = dict_bes_key['OUT_KEY_DIR']
pub_key_name = dict_bes_key['PUB_KEY_NAME']
pri_key_name = dict_bes_key['PRI_KEY_NAME']
out_key_name = dict_bes_key['OUT_KEY_NAME']
extract_dir = dict_bes_key['EXTRACT_DIR']
extract_cmd = dict_bes_key['EXTRACT_CMD']
key_dir = dict_bes_key['KEY_DIR']
sign_cmd = dict_bes_key['SIGN_CMD']
sign_dir = dict_bes_key['SIGN_DIR']
sign_file_dir = dict_bes_key['SIGN_FILE_DIR']
binary_dir = dict_bes_key['BINARY_DIR']
rtos_res_file_name = dict_bes_key['RTOS_RES_FILE_NAME']
rtos_file_name = dict_bes_key['RTOS_FILE_NAME']
release_file_dir = dict_bes_key['RELEASE_BIN_DIR']
pack_file_name = dict_bes_key['PACK_FILE_NAME']
ota_bin_dir = dict_bes_key['OTA_BIN_DIR']
gui_bin_dir = dict_bes_key['GUI_BIN_DIR']
download_tools_dir = dict_bes_key['DLOWNLOAD_TOOLS_DIR']
gui_tools_dir = dict_bes_key['GUI_TOOLS_DIR']
log_swich = dict_bes_key['LOG_SWITCH']
chmod_swich = dict_bes_key['CHMOD_SWITCH']
extract_switch = dict_bes_key['EXTRACT_SWITCH']
pub_otp_switch = dict_bes_key['PUB_OTP_SWITCH']
sign_file_switch = dict_bes_key['SIGN_FILE_SWITCH']
pack_switch = dict_bes_key['PACK_SWITCH']
dld_cfg_dir = dict_bes_key['GUI_CFG_YAML_DIR']
dld_cfg_file = dict_bes_key['GUI_CFG_YAML_FILE']
pro_dld_cfg_file = dict_bes_key['PRO_CFG_YAML_FILE']

# PUB_OTP
dict_pub_otp = cfg_dict['PUB_OTP']
outkey_dir = dict_pub_otp['OUTKEY_DIR']
outkey_file_name = dict_pub_otp['OUTKEY_FILE_NAME']
pub_otp_name = dict_pub_otp['PUB_OTP_FILE_NAME']


#print cfg_dict

# chmod sign_dir
if chmod_swich == 'ON' and (osstr =="Linux"):
    cmd_list = []
    cmd_list.append('chmod')
    cmd_list.append('777')
    cur_dir = os.getcwd()
    #花生，2020年10月6日17:32:39，下面的语句，原版本是print cur_dir，要想在3.x版本支持，需要加括号。
    #否则会提示"missing parentheses in call to print"错误。下面若干个print也是相同处理方式
    print (cur_dir)
    cmd_list.append(cur_dir + '/' + sign_dir)
    if log_swich == "ON":
        print(cmd_list)
    child = subprocess.Popen(cmd_list)
    is_fst = True
    while True:
        status = child.poll()
        if status == 0:
            break
        if status == 1:
            break
        if str(status) == 'None' and is_fst:
            print('Start chmod sign dir...')
            is_fst = False
    print("chmod sign dir done.\n")
#make littlefs
if False:
    os.system("echo \"Start make littlefs\"")
    #print val
    flag = sys.argv[1]
    if flag == '1':
        cmd_list = ['./s700_genfs.sh']
        if osstr == "Windows":
            cmd_list = ['.\\s700_genfs.bat']
    else:
        cmd_list = ['./haas1000_genfs.sh']
        if osstr == "Windows":
            cmd_list = ['.\\haas1000_genfs.bat']
    child = subprocess.Popen(cmd_list)
    while True:
        status = child.poll()
        if status != None:
            break;
    print("genfs done. ")
    size = os.path.getsize("../../prebuild/littlefs.bin")
    print("Littlefs code size:%d"%size)
    if size<4882432:
        time.sleep(5)
    os.system("echo \"Make littlefs done\"")
# extract pub_key
if extract_switch == 'ON':
    cmd_list = []
    cmd_list = extract_cmd
    cur_dir = os.getcwd()
    #花生，2020年10月6日17:32:39，下面的语句，原版本是print cur_dir，要想在3.x版本支持，需要加括号。
    #否则会提示"missing parentheses in call to print"错误。下面若干个print也是相同处理方式
    print (cur_dir)
    if osstr == "Windows":
        cmd_list = []
        cmd_list.append(cur_dir + '/' + sign_dir + '/' + 'bes_sign.bat')
    cmd_list.append(cur_dir + '/' + sign_dir + '/' + in_key_dir + '/' + pub_key_name)
    cmd_list.append(cur_dir + '/' + sign_dir + '/' + out_key_dir + '/' + out_key_name)
    print(os.getcwd())
    if log_swich == 'ON':
        print(cmd_list)
    os.chdir(cur_dir + '/' + sign_dir + '/' + extract_dir)
    print(os.getcwd())
    child = subprocess.Popen(cmd_list)
    is_fst = True
    while True:
        status = child.poll()
        if status == 0:
            break
        if status == 1:
            break
        if str(status) == 'None' and is_fst:
            print('Start extract the public key in C code format...')
            is_fst = False
    os.chdir(cur_dir)
    print("Extract the public key done.\n")

# makedir release_bin ota_bin download_tools
if True:
    isExists=os.path.exists(release_file_dir)
    if isExists == False:
        os.makedirs(release_file_dir)
    isExists = os.path.exists(ota_bin_dir)
    if isExists == False:
        os.makedirs(ota_bin_dir)
    isExists = os.path.exists(gui_bin_dir)
    if isExists == False:
        os.makedirs(gui_bin_dir)
    isExists = os.path.exists(download_tools_dir)
    if isExists == False:
        os.makedirs(download_tools_dir)
    isExists = os.path.exists(gui_tools_dir)
    if isExists == False:
        os.makedirs(gui_tools_dir)
# Make pub_otp
if pub_otp_switch == "ON":
    cmd_list = []
    #./best_sign -k key/OutKey.bin key/pub_flash2.bin
    cur_dir = os.getcwd()
    if osstr == "Windows":
        sign_cmd = sign_cmd + '.exe'
    cmd_list.append(sign_dir + '/' + sign_cmd)
    cmd_list.append('-k')
    cmd_list.append(sign_dir + '/' + outkey_dir + '/' + outkey_file_name)
    cmd_list.append(release_file_dir + '/' + pub_otp_name)
    if log_swich == "ON":
        print(cmd_list)

    child = subprocess.Popen(cmd_list)
    is_fst = True
    while True:
        status = child.poll()
        if status == 0:
            break
        if status == 1:
            break
        if str(status) == 'None' and is_fst:
            print('Start make %s...'%(pub_otp_name))
            is_fst = False
    print("Make %s done.\n"%(pub_otp_name))
    cmd_cp_list = []
    cmd_cp_list.append('cp')
    cmd_cp_list.append(release_file_dir + '/' + pub_otp_name)
    cmd_cp_list.append(ota_bin_dir + '/')
    if log_swich == "ON":
        print(cmd_cp_list)
    shutil.copy(release_file_dir + '/' + pub_otp_name, ota_bin_dir + '/')
 #   child = subprocess.Popen(cmd_cp_list)
    cmd_cp_list2 = []
    cmd_cp_list2.append('cp')
    cmd_cp_list2.append(release_file_dir + '/' + pub_otp_name)
    cmd_cp_list2.append(gui_bin_dir + '/')
    if log_swich == "ON":
        print(cmd_cp_list2)
    shutil.copy(release_file_dir + '/' + pub_otp_name, gui_bin_dir + '/')
#    child = subprocess.Popen(cmd_cp_list2)
    is_fst = True
    while True:
        status = child.poll()
        if status == 0:
            break
        if status == 1:
            break
        if str(status) == 'None' and is_fst:
            print('Cp %s from dir release_bin to dir ota_bin...' % (pub_otp_name))
            is_fst = False
    print("Cp %s done.\n" % (pub_otp_name))

#  cp prebuild/*.bin  release_bin/
if True:
    if (osstr =="Linux"):
        os.system('chmod 777 ' + release_file_dir + '/')
    path = sign_file_dir
    file_list = os.listdir(path)
    for i in range(len(file_list)):
        res_file_path = sign_file_dir + '/' + file_list[i]
        des_path = release_file_dir + '/'
        cmd_str = 'cp -f ' + res_file_path + ' ' + des_path
        if log_swich == "ON":
            print(cmd_str)
        if os.path.isfile(res_file_path):
            shutil.copy(res_file_path, des_path)
#        os.system(cmd_str)

#  cp out/helloworld_demo@haas1000/binary/helloworld_demo@haas1000.bin release_bin
if True:
    res_file_path = binary_dir + '/' + rtos_res_file_name
    print("yxy %s" % os.getcwd())
    print("yxy %s" % res_file_path)
    if not glob.glob(res_file_path):
        print("yxy %s" % glob.glob(res_file_path))
        res_file_path = os.path.join(output_dir, "binary", rtos_res_file_name)
    res_file_path = sys.argv[2]
    des_path = release_file_dir + '/' + rtos_file_name
    cmd_str = 'cp ' + res_file_path + ' ' + des_path
    if log_swich == "ON":
        print(cmd_str)
        print(glob.glob(res_file_path)[0])
    shutil.copy(glob.glob(res_file_path)[0], des_path)

#  mv release_bin/helloworld_demo@haas1000.bin release_bin/ota_rtos.bin
'''
if True:
    res_file_path = release_file_dir + '/' + rtos_res_file_name
    des_file_path = release_file_dir + '/' + rtos_file_name
    cmd_str = 'mv ' + res_file_path + ' ' + des_file_path
    if log_swich == "ON":
        print(cmd_str)
    os.system(cmd_str)
'''

# cp ../write_flash_gui/dld_cfg to ../write_flash_gui/
if False:
    res_file_path = dld_cfg_dir + '/' + pro_dld_cfg_file
    des_path = gui_tools_dir + '/'
    cmd_str = 'cp -f ' + res_file_path + ' ' + des_path
    if log_swich == "ON":
        print(cmd_str)
    shutil.copy(res_file_path, des_path)
    #os.system(cmd_str)

# mv ../write_flash_gui/dld_cfg/haas1000_dld_cfg_*.yaml to ../write_flash_gui/haas1000_dld_cfg.yaml
if False:
    res_file_path = gui_tools_dir + '/' + pro_dld_cfg_file
    des_file_path = gui_tools_dir + '/' + dld_cfg_file
    cmd_str = 'cp -f ' + res_file_path + ' ' + des_file_path
    if log_swich == "ON":
        print(cmd_str)
    if (not os.path.isfile(des_file_path)) or (pro_dld_cfg_file != dld_cfg_file):
        shutil.copy(res_file_path, des_path)
    #os.system(cmd_str)

# sign release_bin/*.bin --- according to configuration(bes_sign_cfg.yaml)
if sign_file_switch == "ON":
    #./ best_sign  key/pri.pem before_sign/noapp_test.bin
    dict_sign_file = cfg_dict['SIGN_FILE_LIST']
    print (dict_sign_file)
    list_items_file = dict_sign_file
    file_itme_len = len(list_items_file)
    #print "file_itme_len %d " % file_itme_len
    for index in range(file_itme_len):
        sign_file_cmd_str = []
        file_item = {}
        #print "list_items_file[index] %s" %list_items_file[index]
        file_item = list_items_file[index]
        tmp_file_item = file_item;
        if tmp_file_item:
            print(str(file_item))
            sign_file_name = tmp_file_item['FILE_NAME']
            sign_switch = tmp_file_item['SWITCH']
            #print "sign_file_name %s" % sign_file_name
            #print "sign_switch %s" % sign_switch
            if sign_switch and sign_file_name:
                cmd_list = []
                #os.system('chmod 777 ' + release_file_dir + '/' + sign_file_name)
                cmd_list.append(sign_dir + '/' + sign_cmd)
                cmd_list.append(sign_dir + '/' + in_key_dir + '/' + pri_key_name)
                cmd_list.append(release_file_dir + '/' + sign_file_name)
                if log_swich == "ON":
                    print(cmd_list)
                child = subprocess.Popen(cmd_list)
                is_fst = True
                while True:
                    status = child.poll()
                    if status == 0:
                        break
                    if status == 1:
                        break
                    if str(status) == 'None' and is_fst:
                        print('Start sign file...')
                        is_fst = False
                print("Sign file done.\n")

# mv release_bin/programmer2001.bin   write_flash_tool/tools/
# changed by yanxiaoyong.yxy from True to False
if False:
    res_file_path = release_file_dir + '/' + 'programmer2001.bin'
    des_file_path = download_tools_dir + '/'
    cmd_str = 'cp -f ' + res_file_path + '  ' + des_file_path
    if log_swich == "ON":
        print(cmd_str)
    shutil.copy(res_file_path, des_path)
    #os.system(cmd_str)
    res_file_path = download_tools_dir + '/' + 'programmer2001.bin'
    des_file_path = gui_tools_dir + '/'
    cmd_str = 'cp -f ' + res_file_path + '  ' + des_file_path
    if log_swich == "ON":
        print(cmd_str)
    shutil.copy(res_file_path, des_path)
    #os.system(cmd_str)

# cp release_bin/*.bin ota_bin
if True:
    path = release_file_dir
    file_list = os.listdir(path)
    for i in range(len(file_list)):
        res_file_path = release_file_dir + '/' + file_list[i]
        des_path = ota_bin_dir + '/'
        cmd_str = 'cp -f ' + res_file_path + ' ' + des_path
        if log_swich == "ON":
            print(cmd_str)
        shutil.copy(res_file_path, des_path)
        #os.system(cmd_str)
        des_path = gui_bin_dir + '/'
        cmd_str = 'cp -f ' + res_file_path + ' ' + des_path
        if log_swich == "ON":
            print(cmd_str)
        shutil.copy(res_file_path, des_path)
        #os.system(cmd_str)
print('all files done.')

if pack_switch == 'ON':
    print('Pack files start...')
    res_dir = cur_dir + '/' + sign_dir + '/' + sign_file_dir + '/*.bin'
    dst_dir = release_file_dir + '/'
    os.system('cp "' + res_dir + '" "' + dst_dir + '"')
    cur_dir = os.getcwd()
    to_pack_dir = os.getcwd() + '/' + release_file_dir
    os.chdir(to_pack_dir)
    changed_dir = os.getcwd()
    to_pack_file_name = pack_file_name
    pack_dir(changed_dir,to_pack_file_name)
    os.chdir(cur_dir)
    print('Pack files done.')

