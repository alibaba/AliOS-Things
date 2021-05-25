# Date    : 2019/03/08
# Author  : Snow Yang
# Mail    : yangsw@mxchip.com

import re
import os
import sys
from struct import pack

image_file = sys.argv[1]
section_start_name = sys.argv[2]
symbol_list_file = sys.argv[3]

size = os.path.getsize(image_file)
addr = int('0x'+re.search(r"\n(\w+) (\w+) "+section_start_name, open(symbol_list_file).read()).group(1), 16)
header = b'81958711' + pack('<L', size) + pack('<L', addr) + b'\xFF'*16

open(image_file.replace('.bin', '_prepend.bin'), 'wb').write(header + open(image_file, 'rb').read())
