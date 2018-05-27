#!/bin/sh

TARGETS_ROOT_PATH=$1/generated
OBJCOPY=$2
MK_GENERATED_PATH=.
MTBCREATE=
DD=

IS_MINGW=`which ls | grep exe`

if [ -z "$IS_MINGW" ];then
	chmod 755 $TARGETS_ROOT_PATH/mtbcreate_x64
    MTBCREATE=$TARGETS_ROOT_PATH/mtbcreate_x64
    DD=dd
else
    MTBCREATE=./mtbcreate.exe
    DD=./dd.exe
fi

cd $TARGETS_ROOT_PATH
#Create mtb
$MTBCREATE 1

#mtb mtb bak merge
$DD if=img.manifest bs=2048 conv=sync > $MK_GENERATED_PATH/imgs/mtb 2>/dev/null
$DD if=img.manifest bs=2048 conv=sync >> $MK_GENERATED_PATH/imgs/mtb 2>/dev/null


#boot to hex  $(SOURCE_ROOT)/board/hobbit1_evb/include/
if [ -f "$MK_GENERATED_PATH/imgs/boot" ]; then
    ADDR_BEG=`cat $MK_GENERATED_PATH/partitions.h | grep  "#define PART_ADDR_BOOTLOADER " | grep -o -E "0x[[:xdigit:]]{8}"`
    $OBJCOPY -I binary -O ihex $MK_GENERATED_PATH/imgs/boot $MK_GENERATED_PATH/hexs/boot.hex --change-address=$ADDR_BEG
fi

#tee to hex
if [ -f "$MK_GENERATED_PATH/imgs/tee" ]; then
    ADDR_BEG=`cat $MK_GENERATED_PATH/partitions.h | grep  "PART_ADDR_TEE" | grep -o -E "0x[[:xdigit:]]{8}"`
    $OBJCOPY -I binary -O ihex $MK_GENERATED_PATH/imgs/tee $MK_GENERATED_PATH/hexs/tee.hex --change-address=$ADDR_BEG
fi

#mtb to hex
if [ -f "$MK_GENERATED_PATH/imgs/mtb" ]; then
    ADDR_BEG=`cat $MK_GENERATED_PATH/partitions.h | grep  "PART_ADDR_IMAGES_MTB" | grep -o -E "0x[[:xdigit:]]{8}"`
    $OBJCOPY -I binary -O ihex $MK_GENERATED_PATH/imgs/mtb $MK_GENERATED_PATH/hexs/mtb.hex --change-address=$ADDR_BEG
fi

#prim to hex
if [ -f "$MK_GENERATED_PATH/imgs/prim" ]; then
    ADDR_BEG=`cat $MK_GENERATED_PATH/partitions.h | grep  "PART_ADDR_REE" | grep -o -E "0x[[:xdigit:]]{8}"`
    $OBJCOPY -I binary -O ihex $MK_GENERATED_PATH/imgs/prim $MK_GENERATED_PATH/hexs/prim.hex --change-address=$ADDR_BEG
fi

rm $MK_GENERATED_PATH/imgs/prim
#-------------------
cd - >/dev/null
