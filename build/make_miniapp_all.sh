#!/usr/bin/env bash
if [ $# -lt 2 ] ; then
    echo "USAGE: $0 appname boardname [option1=xxx]"
    echo " e.g.: $0 uapp1 vexpressa9-mk"
    exit 1;
fi
app=$1
board=$2
shift 2
params=$@
echo app is $app, board is $board, and params is $params

SCRIPT_DIR=`dirname $0`
SRC_ROOT_DIR=$(cd $(dirname $0)/../; pwd )
BUILD_MINIAPP=1

#step 1: build miniapp
MINIAPP_SRC=$SRC_ROOT_DIR/miniapp/core
if [ -d $MINIAPP_SRC ]; then
cd $MINIAPP_SRC
./build.sh user qemu_vexpressa9
rm -rf ../lite/include/ ../lite/lib/
cp -rfv out/compile/qemu_vexpressa9/usr/* ../lite/
cd -
fi

# step 1: kernel build
aos make distclean
aos make alios_kernel@$board -c config && aos make MBINS=kernel
[ $? == 0 ] || { echo "kernel build failed!"; exit 1; }
if [ ! -f ${SRC_ROOT_DIR}/out/alios_kernel@${board}kernel/binary/alios_kernel@$board.kernel.bin ]; then
    echo "kernel build failed"
    exit 1
fi

# step 2.1: primary app build
aos make $app@$board -c config && aos make MBINS=app $params QEMU_BOARD=1 BUILD_MINIAPP=1
[ $? == 0 ] || { echo "app build failed!"; exit 1; }
if [ ! -f ${SRC_ROOT_DIR}/out/$app@${board}app/binary/$app@$board.app.stripped.elf ] ; then
    echo "app build failed."
    exit 1
fi

# step 2.2: dependent apps build together
${SRC_ROOT_DIR}/build/make_deps.sh $app $board $params

# step 3: create file system
${SRC_ROOT_DIR}/build/make_filesystem.sh $app $board $SRC_ROOT_DIR $BUILD_MINIAPP

# step 4: make package
${SRC_ROOT_DIR}/vendor/vexpress/script/make_package.sh $app $board $SRC_ROOT_DIR
