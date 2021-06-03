#!/usr/bin/env python

import os, sys

print("the script is " + sys.argv[0])
print("current dir is " + os.getcwd())
cur_dir = os.getcwd()
script_dir = sys.path[0]
script1_dir = os.path.join(script_dir, "codec", "opensource", "pvaac")
script2_dir = os.path.join(script_dir, "codec", "opensource", "pvmp3")
os.chdir(script1_dir)
print("Downloading open source of aac...")
ret = os.system("python get_pv_aac.py") >> 8
if ret != 0:
    os.chdir(cur_dir)
    exit(1)
os.chdir(script2_dir)
print("Downloading open source of mp3...")
ret = os.system("python get_pvmp3.py") >> 8
if ret != 0:
    os.chdir(cur_dir)
    exit(1)
# result
print("run external script success")
os.chdir(cur_dir)
exit(0)
