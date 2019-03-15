
## Contents

## Overview

### Usage 
1. Download compiler:cskt_abiv2_elf-tool
* [CSKY ABIV ELF](https://pan.baidu.com/s/1nvKdhED passwd:62q6)
   $ tar -zxf csky-abiv2-elf-tools-x86_64-minilibc-20160704.tar.gz -C build/compiler/
2. Compiling dahua dh5021a project
   $ aos make helloworld@dh5021a_evb
3. initialize gdbinit
   $ cp board/dh5021a_evb/configs/dh5021a_evb.gdbint .gdbinit
4. download bin file to dh5012a board by dahua tool
5. run
   $ csky-abiv2-elf-gdb out/helloworld@dh5012a_evb/binary/helloworld@dh5021a_evb.elf 

## Resources

## Reference


