#!/usr/bin/env python3

import os
import sys
import getpass
import shutil

#!/usr/bin/env python3

import os
import sys
import getpass
import shutil

comp_path = sys.path[0]
print("comp_path:")
print(comp_path)

# original folder
src_mp3_path = comp_path + "/resources/mp3"

# new folder
data_path = comp_path + "/../../../../hardware/chip/haas1000/prebuild/data/data"
mp3_dst_path = data_path + "/mp3"

# delete prebuild/data resources
if os.path.exists(mp3_dst_path):
    print ('Delete /data/mp3 firstly')
    shutil.rmtree(mp3_dst_path)

# copy resources
shutil.copytree(src_mp3_path, mp3_dst_path)

# result
print("run external script success")
