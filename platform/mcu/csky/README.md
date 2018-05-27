/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

## Usage 
1. Download compiler:cskt_abiv2_elf-tool
* [CSKY ABIV ELF](https://pan.baidu.com/s/1nvKdhED passwd:62q6)
   $ tar -zxf csky-abiv2-elf-tools-x86_64-minilibc-20160704.tar.gz -C build/compiler/
2. Compiling csky hobbit project
   $ aos make helloworld@hobbit1_evb
3. initialize gdbinit
   $ cp platform/mcu/csky/config/hobbit_gdbinit .gdbinit
4. download bin file to hobbit1_2 board by cds tool
5. run
   $ csky-abiv2-elf-gdb out/helloworld@hobbit1_2/binary/helloworld@hobbit1_evb.elf 
