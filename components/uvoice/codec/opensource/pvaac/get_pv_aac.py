#!/usr/bin/env python

import os
import sys
import shutil

download_fail_times = 0
pvmp3_path = sys.path[0]
download_path = os.path.join(pvmp3_path, "platform_external_opencore")
final_src_path = os.path.join(pvmp3_path, "src")

print('download pvmp3 source file')
if not os.path.exists(final_src_path):
    while True:
        if not os.path.exists(download_path):
            #os.system("git clone https://github.com/aosp-mirror/platform_external_opencore.git -b android-2.2.3_r2.1 " + str(download_path))
            os.system("git clone https://gitee.com/mirrors_aosp-mirror/platform_external_opencore.git -b android-2.2.3_r2.1 \"" + str(download_path) + "\"")
            if os.path.exists(download_path):
                print("Download pvaac source success!\n")
                break
            else:
                download_fail_times = download_fail_times + 1

            if download_fail_times >= 3:
                print("Download pvaac fail!\n")
                break
        break

    shutil.copytree('platform_external_opencore/codecs_v2/audio/aac/dec/src', 'src')
    shutil.copytree('platform_external_opencore/codecs_v2/audio/aac/dec/include', 'include')
    shutil.copyfile('platform_external_opencore/oscl/oscl/osclerror/src/oscl_error_codes.h', "include/oscl_error_codes.h")
    shutil.copyfile('platform_external_opencore/codecs_v2/audio/aac/patent_disclaimer.txt', 'patent_disclaimer.txt')
    os.rename('src/fft_rx4_tables_fxp.cpp', 'src/fft_rx4_tables_fxp.c')

print('Download opencore aac source file success!')


