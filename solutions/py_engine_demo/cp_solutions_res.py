# coding=utf-8
import shutil
import os
import sys
import stat
import datetime
import subprocess
from datetime import datetime

def copy_resources_objs(script_in, build_params_in):

    build_params = open(build_params_in)
    data = build_params.read()

    data_prebuild_dir=''
    if 'ALI_AOS_HAAS_EDU_K1' in data:  # haasedu
        print('HaaS EDU platform')
        data_prebuild_dir = 'hardware/chip/haas1000/prebuild/data/'

    elif 'ALI_AOS_HAAS200' in data:     # haas200
        print('HaaS 200 platform')
        data_prebuild_dir = 'hardware/chip/rtl872xd/prebuild/data/'
    else:                               # haas100
        print('HaaS100 chip')
        data_prebuild_dir='hardware/chip/haas1000/prebuild/data/'

    print('data_prebuild_dir = %s' % data_prebuild_dir)


    script_dir = os.path.dirname(script_in)
    project_dir = script_dir + '/../../'
    font_dir_src = os.path.join(project_dir, 'components', 'py_engine', 'external', 'bitmap_fonts')
    font_dir_dist = os.path.join(project_dir, data_prebuild_dir, 'font')

    print('project_dir = %s' % project_dir)
    print('font_dir_src = %s' % font_dir_src)
    print('font_dir_dist = %s' % font_dir_dist)

    if os.path.exists(font_dir_dist):
        shutil.rmtree(font_dir_dist)

    shutil.copytree(font_dir_src, font_dir_dist)


if __name__ == '__main__':

    copy_resources_objs(sys.argv[0], sys.argv[1])

    print('run cp_solutions_res script success')
