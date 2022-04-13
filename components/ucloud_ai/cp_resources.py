#!/usr/bin/env python3

import os
import sys
import getpass
import shutil

# original folder
comp_path="../../../../components/ucloud_ai/src/example/image"
data_path="../../../../hardware/chip/haas1000/prebuild/data"

# new folder
image_path=data_path+"/ucloud_ai_image"

# delete prebuild/data resources
if os.path.exists(image_path):
    print ('Delete /data/ucloud_ai_image firstly')
    shutil.rmtree(image_path)

# copy resources
shutil.copytree(comp_path, image_path)

# result
print("run external script success")
