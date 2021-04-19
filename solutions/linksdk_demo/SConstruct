#! /bin/env python

from aostools import Make

# default elf is out/<solution>@<board>.elf, and default objcopy is out/<solution>@<board>.bin
# defconfig = Make(elf='out/<solution>@<board>.elf', objcopy='out/<solution>@<board>.bin')
defconfig = Make()

Export('defconfig')

defconfig.build_components()
