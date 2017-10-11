#!/bin/sh

cd ~/
if [ -d githubsync ] || [ -f githubsync ]; then
    rm -rf githubsync
fi
mkdir githubsync

cd ~/githubsync
git clone git@code.aliyun.com:keepwalking.zeng/aos.git
git clone git@github.com:alibaba/AliOS-Things.git AliOS
aosdir=~/githubsync/aos
githubdir=~/githubsync/AliOS

cd ${githubdir}
git reset --hard 31659df7f8ef0df547a817a218512a820121453e
cd ~/githubsync

rm -rf ${githubdir}/*
rm -rf ${github}/.gitignore
rm -rf ${github}/.vscode
cp -rf ${aosdir}/* ${githubdir}/
cp -rf ${aosdir}/.gitignore ${githubdir}/.gitignore
cp -rf ${aosdir}/.vscode ${githubdir}/.vscode

#remove files from github dir
rm -rf ${githubdir}/board/armhflinux
rm -rf ${githubdir}/board/mk108
rm -rf ${githubdir}/build/astyle
rm -rf ${githubdir}/build/astyle.sh
rm -rf ${githubdir}/build/copyright.py
rm -rf ${githubdir}/build/copyright
rm -rf ${githubdir}/build/doxygen2md.py
rm -rf ${githubdir}/build/github_sync.sh
rm -rf ${githubdir}/build/MD.templet
rm -rf ${githubdir}/build/OpenOCD
rm -rf ${githubdir}/build/compiler/arm-none-eabi*
rm -rf ${githubdir}/platform/mcu/linux/csp/wifi/radiotap
rm -rf ${githubdir}/script
rm -rf ${githubdir}/platform/mcu/linux/csp/wifi
rm -rf ${githubdir}/platform/arch/linux/swap.*
rm -rf ${githubdir}/kernel/rhino/test/perf/realtimelib.c
rm -rf ${githubdir}/example/mqttest
rm -rf ${githubdir}/example/tls
rm -rf ${githubdir}/example/yts
rm -rf ${githubdir}/bootloader
rm -rf ${githubdir}/kernel/rhino/test
rm -rf ${githubdir}/test
cd ${aosdir}/board/linuxhost/
git checkout origin/githubsync -- k_config.h
cp -f k_config.h ${githubdir}/board/linuxhost/
git reset && git checkout k_config.h
cd ${aosdir}/platform/mcu/linux/
git checkout origin/githubsync -- linux.mk
cp -rf linux.mk ${githubdir}/platform/mcu/linux/
git reset && git checkout linux.mk
cd ${aosdir}/build/
git checkout origin/githubsync -- autobuild.sh
cp -rf autobuild.sh ${githubdir}/build/
git reset && git checkout autobuild.sh

#tools folder
rm -rf ${githubdir}/tools/*
cp -rf ${aosdir}/tools/Doxyfile ${githubdir}/tools/
cp -rf ${aosdir}/tools/doxygen.sh ${githubdir}/tools/
cp -rf ${aosdir}/tools/cli ${githubdir}/tools/

#ywss folder
rm -rf ${githubdir}/framework/ywss/*
mkdir ${githubdir}/framework/ywss/lib/
mkdir ${githubdir}/framework/ywss/lib/linuxhost
mkdir ${githubdir}/framework/ywss/lib/mk3060
cp -f ${aosdir}/framework/ywss/awss.h ${githubdir}/framework/ywss/
cp -f ${aosdir}/framework/ywss/enrollee.h ${githubdir}/framework/ywss/
cd ${aosdir}/framework/ywss/
git checkout origin/githubsync -- ywss.mk
cp -rf ywss.mk ${githubdir}/framework/ywss/
git reset && git checkout ywss.mk

#mesh folder
rm -rf ${githubdir}/kernel/protocols/mesh/*
mkdir ${githubdir}/kernel/protocols/mesh/lib
mkdir ${githubdir}/kernel/protocols/mesh/lib/linuxhost
mkdir ${githubdir}/kernel/protocols/mesh/lib/mk3060
mkdir ${githubdir}/kernel/protocols/mesh/include
cp -f ${aosdir}/kernel/protocols/mesh/include/umesh_80211.h ${githubdir}/kernel/protocols/mesh/include/
cp -f ${aosdir}/kernel/protocols/mesh/include/umesh.h ${githubdir}/kernel/protocols/mesh/include/
cp -f ${aosdir}/kernel/protocols/mesh/include/umesh_hal.h ${githubdir}/kernel/protocols/mesh/include/
cp -f ${aosdir}/kernel/protocols/mesh/include/umesh_types.h ${githubdir}/kernel/protocols/mesh/include/
cd ${aosdir}/kernel/protocols/mesh
git checkout origin/githubsync -- mesh.mk
cp -rf mesh.mk ${githubdir}/kernel/protocols/mesh/
git reset && git checkout mesh.mk

#beken folder
rm -rf ${githubdir}/platform/mcu/beken/*
cd ${aosdir}/platform/mcu/beken/
mkdir ${githubdir}/platform/mcu/beken/include
mkdir ${githubdir}/platform/mcu/beken/include/lwip-2.0.2
mkdir ${githubdir}/platform/mcu/beken/include/app
mkdir ${githubdir}/platform/mcu/beken/include/func
mkdir ${githubdir}/platform/mcu/beken/include/os
mkdir ${githubdir}/platform/mcu/beken/include/driver
mkdir ${githubdir}/platform/mcu/beken/include/ip
cp -rf beken7231/beken378/func/mxchip/lwip-2.0.2/port ${githubdir}/platform/mcu/beken/include/lwip-2.0.2/
cp -rf beken7231/beken378/common ${githubdir}/platform/mcu/beken/include/
cp -rf beken7231/beken378/app/config ${githubdir}/platform/mcu/beken/include/app/
cp -rf beken7231/beken378/func/include ${githubdir}/platform/mcu/beken/include/func/
cp -rf beken7231/beken378/os/include ${githubdir}/platform/mcu/beken/include/os/
cp -rf beken7231/beken378/driver/include ${githubdir}/platform/mcu/beken/include/driver/
cp -rf beken7231/beken378/driver/common ${githubdir}/platform/mcu/beken/include/driver/
cp -rf beken7231/beken378/ip/common ${githubdir}/platform/mcu/beken/include/ip/
cp -rf beken7231/beken378/driver/entry/*.h ${githubdir}/platform/mcu/beken/include/
cp -rf beken7231/beken378/build ${githubdir}/platform/mcu/beken/linkinfo
find ${githubdir}/platform/mcu/beken/ -type f -name '*.c' -exec rm {} +
cp -rf aos ${githubdir}/platform/mcu/beken/
cp -rf hal ${githubdir}/platform/mcu/beken/
cp -rf encrypt_linux ${githubdir}/platform/mcu/beken/
cp -rf encrypt_osx ${githubdir}/platform/mcu/beken/
cp -rf encrypt_win.exe ${githubdir}/platform/mcu/beken/
cp -rf gen_crc_bin.mk ${githubdir}/platform/mcu/beken/
cd ${aosdir}/platform/mcu/beken/
git checkout origin/githubsync -- beken.mk
cp -rf beken.mk ${githubdir}/platform/mcu/beken/
git reset && git checkout beken.mk

cd ${aosdir}
aos make meshapp@linuxhost > /dev/null
if [ $? -ne 0 ]; then
    echo "error: build meshapp@linuxhost failed"
    exit 1
fi
cd ${aosdir}/out/meshapp@linuxhost/libraries/
cp mesh.a libmesh.a
strip --strip-debug libmesh.a
mv libmesh.a ${githubdir}/kernel/protocols/mesh/lib/linuxhost/libmesh.a

cd ${aosdir}
aos make alinkapp@linuxhost > /dev/null
if [ $? -ne 0 ]; then
    echo "error: build alinkapp@linuxhost failed"
    exit 1
fi
cd ${aosdir}/out/alinkapp@linuxhost/libraries/
cp ywss.a libywss.a
strip --strip-debug libywss.a
cp libywss.a ${githubdir}/framework/ywss/lib/linuxhost/libywss.a

cd ${aosdir}
aos make alinkapp@mk3060 > /dev/null
if [ $? -ne 0 ]; then
    echo "error: build alinkapp@mk3060 failed"
    exit 1
fi
cd ${aosdir}/out/alinkapp@mk3060/libraries/
cp mesh.a libmesh.a
arm-none-eabi-strip --strip-debug libmesh.a
mv libmesh.a ${githubdir}/kernel/protocols/mesh/lib/mk3060/libmesh.a
cp ywss.a libywss.a
arm-none-eabi-strip --strip-debug libywss.a
cp libywss.a ${githubdir}/framework/ywss/lib/mk3060/libywss.a
echo "create libbeken.a" > packscript
echo "addlib beken.a" >> packscript
echo "addlib entry.a" >> packscript
echo "addlib hal_init.a" >> packscript
echo "addlib ${aosdir}/platform/mcu/beken/librwnx/librwnx.a" >> packscript
echo "save" >> packscript
echo "end" >> packscript
arm-none-eabi-ar -M < packscript
arm-none-eabi-strip --strip-debug libbeken.a
mv libbeken.a ${githubdir}/platform/mcu/beken/
cd ${githubdir}/platform/mcu/beken/
arm-none-eabi-ar dv libbeken.a aos_main.o
arm-none-eabi-ar dv libbeken.a soc_impl.o
arm-none-eabi-ar dv libbeken.a trace_impl.o
arm-none-eabi-ar dv libbeken.a mesh_wifi_hal.o

cd ${githubdir}
git add -A
datetime=`date +%F@%H:%M`
git commit -m "code synchronization at ${datetime}" > /dev/null
git push -f origin master
