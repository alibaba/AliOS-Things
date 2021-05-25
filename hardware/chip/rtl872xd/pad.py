# Date    : 2019/03/08
# Author  : Snow Yang
# Mail    : yangsw@mxchip.com

import os
import sys

image_file = sys.argv[1]

size = os.path.getsize(image_file)
pad_size = (((size - 1) >> 12) + 1) << 12
open(image_file, 'ab').write(b'\xFF'*(pad_size - size))