#!/usr/bin/env python3

import os
import sys
import getpass
import shutil

# original folder
org_image_path="../../../../components/ugraphics/src/example/image"
org_font_path="../../../../components/ugraphics/src/resources/font"
data_path="../../../../hardware/chip/haas1000/prebuild/data"

# new folder
image_path=data_path+"/ugraphics_image"
font_path=data_path+"/font"

# delete prebuild/data resources
if os.path.exists(image_path):
    print ('Delete /data/ugraphics_image firstly')
    shutil.rmtree(image_path)

if os.path.exists(font_path):
    print ('Delete /data/font firstly')
    shutil.rmtree(font_path)

# copy resources
shutil.copytree(org_image_path, image_path)
shutil.copytree(org_font_path, font_path)

# result
print("run external script success")
