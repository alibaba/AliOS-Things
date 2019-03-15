## Contents

```sh
csky # configuration files for platform/mcu csky
```

## Overview

### Usage 

1. Download compiler:cskt_abiv2_elf-tool
* [CSKY ABIV ELF](https://pan.baidu.com/s/1nvKdhED passwd:62q6)
   $ tar -zxf csky-abiv2-elf-tools-x86_64-minilibc-20160704.tar.gz -C build/compiler/
2. Compiling csky cb2201 project
   $ aos make helloworld@cb2201
3. initialize gdbinit
   $ cp platform/mcu/csky/config/ch2201_gdbinit .gdbinit
4. download bin file to cb2201 board by cds tool
5. run
   $ csky-abiv2-elf-gdb out/helloworld@cb2201/binary/helloworld@cb2201.elf

## Resources

## Reference

