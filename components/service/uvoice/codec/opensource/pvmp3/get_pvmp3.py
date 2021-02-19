#!/usr/bin/env python

import os
import sys
import shutil


download_fail_times = 0
pvmp3_path = sys.path[0]
download_path = os.path.join(pvmp3_path, "source")
final_src_path = os.path.join(pvmp3_path, "src")

print('download pvmp3 source file')
if not os.path.exists(final_src_path):
    while True:
        if not os.path.exists(download_path):
            #os.system("git clone https://github.com/aosp-mirror/platform_external_opencore.git -b android-2.2.3_r2.1 " + str(download_path))
            os.system("git clone https://gitee.com/mirrors_aosp-mirror/platform_external_opencore.git -b android-2.2.3_r2.1 " + str(download_path))
            if os.path.exists(download_path):
                print("Download pvmp3 source success!\n")
                break
            else:
                download_fail_times = download_fail_times + 1

            if download_fail_times >= 3:
                print("Download pvmp3 fail!\n")
                break
        break

    shutil.copytree('source/codecs_v2/audio/mp3/dec/src', 'src')
    shutil.copytree('source/codecs_v2/audio/mp3/dec/include', 'include')

    n = 0
    filelist = os.listdir(final_src_path)
    for i in filelist:
        oldname = os.path.join(final_src_path, filelist[n])

        suffix = oldname.split('.')[-1]
        if suffix == 'h' or suffix == 'cpp':
            code =''
            with open(oldname, 'r') as f:
                code = f.read()
                code = code.replace('double(', '(double)(')
                code = code.replace('int32(', '(int32)(')
                code = code.replace('huffcodetab     ht[HUFF_TBL];', 'struct huffcodetab     ht[HUFF_TBL];')
                code = code.replace('huffcodetab       *pHuff;', 'struct huffcodetab       *pHuff;')
                code = code.replace('__inline', 'static inline')
                code = code.replace('inline int16 saturate16', 'static int16 saturate16')
                code = code.replace('new_slen[4];', 'new_slen[4] = {0,0,0,0};')
            with open(oldname, 'w') as f:
                f.write(code)
        if suffix == 'cpp':
            newname = oldname[:-4] + '.c'
            os.rename(oldname, newname)
            print(oldname,'->', newname)
        n = n + 1
    shutil.copyfile('oscl_base.h', 'include/oscl_base.h')
    shutil.copyfile('oscl_mem.h', 'include/oscl_mem.h')
    shutil.rmtree(download_path)

print('Download pvmp3 source file success!')


