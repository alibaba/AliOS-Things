#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import sys, os
import shutil

from copy_map import copy_map

SDKNAME = "linkkit-sdk-c"


def main(argv):
    source_dir = os.path.abspath(sys.argv[1])
    build_dir = os.path.abspath(sys.argv[2])

    sdk_dir = os.path.join(build_dir, SDKNAME)
    if not os.path.isdir(sdk_dir):
        print("[ERROR]:dir {} is not exist".format(sdk_dir))
        return 1
        
    # Copy components to linkkit sdk dir
    for src, dest in reversed(copy_map):
        tmp_src = os.path.join(source_dir, src)
        if dest:
            tmp_dest = os.path.join(build_dir, SDKNAME, dest)
        else:
            tmp_dest = os.path.join(build_dir, SDKNAME)

        print( "delete {}".format(tmp_src))
        if os.path.exists(tmp_src):
            if os.path.isdir(tmp_src):
                shutil.rmtree(tmp_src)
            else:
                os.remove(tmp_src)
                   
        print( "move {} to {}".format(tmp_dest, tmp_src))
        if os.path.isdir(tmp_dest):
            shutil.move(tmp_dest, tmp_src)
        else:
            shutil.move(tmp_dest, tmp_src)
        
    

if __name__ == "__main__":
    main(sys.argv)
    
    
