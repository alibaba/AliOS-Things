#!/usr/bin/env bash
if [ $# -lt 2 ] ; then
    echo "USAGE: $0 appname boardname [option1=xxx]"
    echo " e.g.: $0 uapp1 vexpressa9-mk"
    exit 1;
fi
app=$1
board=$2
user_args=$3
shift 2
params=$@
echo app is $app, board is $board, and params is $params

SCRIPT_DIR=`dirname $0`
SRC_ROOT_DIR=$(cd $(dirname $0)/../; pwd )
REBUILD_LIBC_SO=$(echo $params | grep -P 'REBUILD_LIBC_SO\s*=\s*\d+' -o | grep -P '\d+' -o)

# step 1: kernel build
aos make distclean
aos make alios_kernel@$board -c config && aos make MBINS=kernel $params $user_args
[ $? == 0 ] || { echo "kernel build failed!"; exit 1; }
if [ ! -f ${SRC_ROOT_DIR}/out/alios_kernel@${board}kernel/binary/alios_kernel@$board.kernel.bin ]; then
    echo "kernel build failed"
    exit 1
fi

# step 2.0: rebuild libc.so, it should be removed in release phase
if [ "$REBUILD_LIBC_SO" = "1" ]; then
    ${SRC_ROOT_DIR}/components/musllibc/call_musllibc.sh $board ${SRC_ROOT_DIR}/toolchain/gcc-arm-none-eabi/Linux64/bin/
fi

# step 2.1: dependent apps build together
${SRC_ROOT_DIR}/build/make_deps.sh $app $board $params

# step 2.2: primary app build
aos make $app@$board -c config && aos make MBINS=app $params $user_args
[ $? == 0 ] || { echo "app build failed!"; exit 1; }
if [ ! -f ${SRC_ROOT_DIR}/out/$app@${board}app/binary/$app@$board.app.stripped.elf ] ; then
    echo "app build failed."
    exit 1
fi

# step 3: create file system
${SRC_ROOT_DIR}/build/make_filesystem.sh $app $board $SRC_ROOT_DIR 0

# step 4: make package
${SRC_ROOT_DIR}/vendor/vexpress/script/make_package.sh $app $board $SRC_ROOT_DIR
