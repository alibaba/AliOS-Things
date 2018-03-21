#!/bin/sh

PWD=`pwd`

YOC_PATH=$1
TARGETS_ROOT_PATH=$2
OBJCOPY=$3
MK_GENERATED_PATH=$4/generated

ZIP=

IS_MINGW=`which ls | grep exe`

if [ -z "$IS_MINGW" ];then
    ZIP=zip
else
    ZIP=$YOC_PATH/tools/build/hex/zip.exe
fi

#boot to hex
if [ -f "$MK_GENERATED_PATH/imgs/boot" ]; then
    ADDR_BEG=`cat $TARGETS_ROOT_PATH/include/partitions.h | grep  "#define PART_ADDR_BOOTLOADER " | grep -o -E "0x[[:xdigit:]]{8}"`
    $OBJCOPY -I binary -O ihex $MK_GENERATED_PATH/imgs/boot $MK_GENERATED_PATH/hexs/boot.hex --change-address=$ADDR_BEG
fi

#tee to hex
if [ -f "$MK_GENERATED_PATH/imgs/tee" ]; then
    ADDR_BEG=`cat $TARGETS_ROOT_PATH/include/partitions.h | grep  "PART_ADDR_TEE" | grep -o -E "0x[[:xdigit:]]{8}"`
    $OBJCOPY -I binary -O ihex $MK_GENERATED_PATH/imgs/tee $MK_GENERATED_PATH/hexs/tee.hex --change-address=$ADDR_BEG
fi

#mtb to hex
if [ -f "$MK_GENERATED_PATH/imgs/mtb" ]; then
    ADDR_BEG=`cat $TARGETS_ROOT_PATH/include/partitions.h | grep  "PART_ADDR_IMAGES_MTB" | grep -o -E "0x[[:xdigit:]]{8}"`
    $OBJCOPY -I binary -O ihex $MK_GENERATED_PATH/imgs/mtb $MK_GENERATED_PATH/hexs/mtb.hex --change-address=$ADDR_BEG
fi

#prim to hex
if [ -f "$MK_GENERATED_PATH/imgs/prim" ]; then
    ADDR_BEG=`cat $TARGETS_ROOT_PATH/include/partitions.h | grep  "PART_ADDR_REE" | grep -o -E "0x[[:xdigit:]]{8}"`
    $OBJCOPY -I binary -O ihex $MK_GENERATED_PATH/imgs/prim $MK_GENERATED_PATH/hexs/prim.hex --change-address=$ADDR_BEG
fi

#create imgs.zip
mkdir $MK_GENERATED_PATH/image
cp $MK_GENERATED_PATH/imgs/boot $MK_GENERATED_PATH/image
cp $MK_GENERATED_PATH/imgs/tee $MK_GENERATED_PATH/image
cp $MK_GENERATED_PATH/imgs/prim $MK_GENERATED_PATH/image
cd $MK_GENERATED_PATH
$ZIP -p -r imgs.zip image
cd -
rm -rf $MK_GENERATED_PATH/image
