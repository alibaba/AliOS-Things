#! /bin/env python

from aostools import Make

# defconfig = Make(elf='yoc.elf', objcopy='generated/data/prim', objdump='yoc.asm')
defconfig = Make(elf='aos.elf', objcopy='aos.bin')

Export('defconfig')

defconfig.build_components()
