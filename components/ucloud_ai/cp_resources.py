#!/usr/bin/env python3

import os
import sys
import getpass
import shutil

comp_path = sys.path[0]
print("comp_path:")
print(comp_path)

# original folder
org_image_path = comp_path + "/example/image"

# new folder
data_path = comp_path + "/../../hardware/chip/haas1000/prebuild/data"
image_path = data_path + "/ucloud_ai_image"

# delete prebuild/data resources
if os.path.exists(image_path):
    print ('Delete /data/ucloud_ai_image firstly')
    shutil.rmtree(image_path)

# copy resources
shutil.copytree(org_image_path, image_path)

# result
print("run external script success")
