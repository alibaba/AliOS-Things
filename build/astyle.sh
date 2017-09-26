#!/bin/bash

## http://astyle.sourceforge.net/astyle.html

DIRS=(devices
      framework
      include
      kernel/hal
      kernel/modules/fs/kv
      kernel/protocols/mesh
      kernel/rhino
      kernel/syscall
      kernel/vcall/aos
      kernel/vfs
      kernel/yloop
      platform/arch/arm
      tools/cli
      utility/base64
      utility/digest_algorithm
      utility/hashtable
      utility/libc
      security/tfs
      )

for DIR in ${DIRS[*]}
do

    find ./$DIR -name "*.[ch]" | xargs ./build/astyle --formatted --style=otbs --min-conditional-indent=0 --indent=spaces=4 --indent-switches --indent-col1-comments --pad-oper --pad-header --indent-col1-comments --max-instatement-indent=120 --max-code-length=120 --break-after-logical --align-pointer=name --add-brackets --convert-tabs --lineend=linux --suffix=none

done
