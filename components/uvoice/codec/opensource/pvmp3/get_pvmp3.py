#!/usr/bin/env python

import os
import sys
import shutil

download_fail_times = 0
pvmp3_path = sys.path[0]

download_path = os.path.join(pvmp3_path, "source")
final_src_path = os.path.join(pvmp3_path, "src")

pvmp3_source = ['pvmp3_normalize.c',
                'pvmp3_alias_reduction.c',
                'pvmp3_crc.c',
                'pvmp3_decode_header.c',
                'pvmp3_decode_huff_cw.c',
                'pvmp3_getbits.c',
                'pvmp3_dequantize_sample.c',
                'pvmp3_framedecoder.c',
                'pvmp3_get_main_data_size.c',
                'pvmp3_get_side_info.c',
                'pvmp3_get_scale_factors.c',
                'pvmp3_mpeg2_get_scale_data.c',
                'pvmp3_mpeg2_get_scale_factors.c',
                'pvmp3_mpeg2_stereo_proc.c',
                'pvmp3_huffman_decoding.c',
                'pvmp3_huffman_parsing.c',
                'pvmp3_tables.c',
                'pvmp3_imdct_synth.c',
                'pvmp3_mdct_6.c',
                'pvmp3_dct_6.c',
                'pvmp3_poly_phase_synthesis.c',
                'pvmp3_equalizer.c',
                'pvmp3_seek_synch.c',
                'pvmp3_stereo_proc.c',
                'pvmp3_reorder.c',
                'pvmp3_polyphase_filter_window.c',
                'pvmp3_mdct_18.c',
                'pvmp3_dct_9.c',
                'pvmp3_dct_16.c']

print('download pvmp3 source file')
if not os.path.exists(final_src_path):
    while True:
        if not os.path.exists(download_path):
            #download_cmd = "git clone https://github.com/aosp-mirror/platform_external_opencore.git -b android-2.2.3_r2.1 " + str(download_path)
            download_cmd = "git clone https://gitee.com/mirrors_aosp-mirror/platform_external_opencore.git -b android-2.2.3_r2.1 \"" + str(download_path) + "\""
            print(download_cmd)
            os.system(download_cmd)
            if os.path.exists(download_path):
                print("Download pvmp3 source success!\n")
                break
            else:
                download_fail_times = download_fail_times + 1

            if download_fail_times >= 3:
                print("Download pvmp3 fail!\n")
                break
        break

    src = os.path.join(pvmp3_path, 'source/codecs_v2/audio/mp3/dec/src')
    dst = os.path.join(pvmp3_path, "src")
    print(src)
    print(dst)
    shutil.copytree(src, dst)

    os.remove(os.path.join(pvmp3_path, 'src/pvmp3_decoder.cpp'))
    
    src = os.path.join(pvmp3_path, 'source/codecs_v2/audio/mp3/dec/include')
    dst = os.path.join(pvmp3_path, 'include')
    #shutil.rmtree(dst)
    print(src)
    print(dst)
    shutil.copytree(src, dst)

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


