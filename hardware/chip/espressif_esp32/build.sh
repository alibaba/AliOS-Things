#!/usr/bin/env bash

a=`uname  -a`
b="Darwin"
c="Linux"
dst="$HOME/.espressif/dist/"

# store workdir path
workdir=$PWD
echo "workdir = ${workdir}"

# get shelldir path
shelldir=$(cd $(dirname ${BASH_SOURCE[0]}); pwd )
echo "shelldir = ${shelldir}"

if [[ $a =~ $b ]];then
    echo "OS Type is Mac"

    mkdir -p $dst

    cp -rf ${shelldir}/toolchain/openocd-esp32-macos-0.10.0-esp32-20200709.tar.gz $dst
    cp -rf ${shelldir}/toolchain/openocd-esp32-macos-0.10.0-esp32-20210902.tar.gz $dst
    cp -rf ${shelldir}/toolchain/binutils-esp32ulp-macos-2.28.51-esp-20191205.tar.gz $dst
    cp -rf ${shelldir}/toolchain/binutils-esp32s2ulp-macos-2.28.51-esp-20191205.tar.gz  $dst
    cp -rf ${shelldir}/toolchain/xtensa-esp32-elf-gcc8_4_0-esp-2021r1-macos.tar.gz $dst
    cp -rf ${shelldir}/toolchain/xtensa-esp32-elf-gcc8_4_0-esp-2020r3-macos.tar.gz $dst
    cp -rf ${shelldir}/toolchain/xtensa-esp32s2-elf-gcc8_4_0-esp-2020r3-macos.tar.gz $dst

elif [[ $a =~ $c ]];then
    echo "OS Type is Linux"

    mkdir -p $dst

    cp -rf ${shelldir}/toolchain/openocd-esp32-linux64-0.10.0-esp32-20200709.tar.gz $dst
    cp -rf ${shelldir}/toolchain/openocd-esp32-linux64-0.10.0-esp32-20210902.tar.gz $dst
    cp -rf ${shelldir}/toolchain/binutils-esp32ulp-linux-amd64-2.28.51-esp-20191205.tar.gz $dst
    cp -rf ${shelldir}/toolchain/binutils-esp32s2ulp-linux-amd64-2.28.51-esp-20191205.tar.gz  $dst
    cp -rf ${shelldir}/toolchain/xtensa-esp32-elf-gcc8_4_0-esp-2021r1-linux-amd64.tar.gz $dst
    cp -rf ${shelldir}/toolchain/xtensa-esp32-elf-gcc8_4_0-esp-2020r3-linux-amd64.tar.gz $dst
    cp -rf ${shelldir}/toolchain/xtensa-esp32s2-elf-gcc8_4_0-esp-2020r3-linux-amd64.tar.gz $dst
else
    echo $a
fi

cd ${shelldir}/esp-idf
./install.sh esp32
source export.sh


cd ${workdir}