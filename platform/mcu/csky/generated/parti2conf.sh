#!/bin/sh

echo '#****************************#'

if [ "$2" = "v2" ];then
echo "CONFIG_PARTION_NUM=6"
else
echo "CONFIG_PARTION_NUM=4"
fi

echo
echo 'CONFIG_PARTION_NAME0=boot'
ADDR_BEG=`cat $1/include/partitions.h | grep  "#define PART_ADDR_BOOTLOADER " | grep -o -E "0x[[:xdigit:]]{8}"`
ADDR_END=`cat $1/include/partitions.h | grep  "#define PART_ADDR_END_BOOTLOADER " | grep -o -E "0x[[:xdigit:]]{8}"`
echo CONFIG_PARTION_ADDR0=$ADDR_BEG
echo CONFIG_PARTION_END0=$(awk 'BEGIN{printf("0x%X", '$ADDR_END' + 1)}')
if [ "$2" = "v2" ];then
echo CONFIG_STATIC_ADDR0=$ADDR_BEG
fi
echo CONFIG_IMG_LOAD_ADDR0=$ADDR_BEG

echo
echo 'CONFIG_PARTION_NAME1=tee'
ADDR_BEG=`cat $1/include/partitions.h | grep  "PART_ADDR_TEE" | grep -o -E "0x[[:xdigit:]]{8}"`
ADDR_END=`cat $1/include/partitions.h | grep  "PART_ADDR_END_TEE" | grep -o -E "0x[[:xdigit:]]{8}"`
echo CONFIG_PARTION_ADDR1=$ADDR_BEG
echo CONFIG_PARTION_END1=$(awk 'BEGIN{printf("0x%X", '$ADDR_END' + 1)}')
if [ "$2" = "v2" ];then
echo CONFIG_STATIC_ADDR1=$ADDR_BEG
fi
echo CONFIG_IMG_LOAD_ADDR1=$ADDR_BEG

echo
echo 'CONFIG_PARTION_NAME2=prim'
ADDR_BEG=`cat $1/include/partitions.h | grep  "PART_ADDR_REE" | grep -o -E "0x[[:xdigit:]]{8}"`
ADDR_END=`cat $1/include/partitions.h | grep  "PART_ADDR_END_REE" | grep -o -E "0x[[:xdigit:]]{8}"`
echo CONFIG_PARTION_ADDR2=$ADDR_BEG
echo CONFIG_PARTION_END2=$(awk 'BEGIN{printf("0x%X", '$ADDR_END' + 1)}')
if [ "$2" = "v2" ];then
echo CONFIG_STATIC_ADDR2=$ADDR_BEG
fi
echo CONFIG_IMG_LOAD_ADDR2=$ADDR_BEG

echo
echo 'CONFIG_PARTION_NAME3=misc'
ADDR_BEG=`cat $1/include/partitions.h | grep  "PART_ADDR_FOTA" | grep -o -E "0x[[:xdigit:]]{8}"`
ADDR_END=`cat $1/include/partitions.h | grep  "PART_ADDR_END_FOTA" | grep -o -E "0x[[:xdigit:]]{8}"`
echo CONFIG_PARTION_ADDR3=$ADDR_BEG
echo CONFIG_PARTION_END3=$(awk 'BEGIN{printf("0x%X", '$ADDR_END' + 1)}')
if [ "$2" = "v2" ];then
echo CONFIG_STATIC_ADDR3=$ADDR_BEG
fi
echo CONFIG_IMG_LOAD_ADDR3=$ADDR_BEG

if [ "$2" = "v2" ];then
echo
echo 'CONFIG_PARTION_NAME4=boot_mtb'
ADDR_BEG=`cat $1/include/partitions.h | grep  "PART_ADDR_BOOTLOADER_MTB" | grep -o -E "0x[[:xdigit:]]{8}"`
ADDR_END=`cat $1/include/partitions.h | grep  "PART_ADDR_END_BOOTLOADER_MTB" | grep -o -E "0x[[:xdigit:]]{8}"`
echo CONFIG_PARTION_ADDR4=$ADDR_BEG
echo CONFIG_PARTION_END4=$(awk 'BEGIN{printf("0x%X", '$ADDR_END' + 1)}')
echo CONFIG_STATIC_ADDR4=$ADDR_BEG
echo CONFIG_IMG_LOAD_ADDR4=$ADDR_BEG
fi

if [ "$2" = "v2" ];then
echo
echo 'CONFIG_PARTION_NAME5=image_mtb'
ADDR_BEG=`cat $1/include/partitions.h | grep  "PART_ADDR_IMAGES_MTB" | grep -o -E "0x[[:xdigit:]]{8}"`
ADDR_END=`cat $1/include/partitions.h | grep  "PART_ADDR_END_IMAGES_MTB" | grep -o -E "0x[[:xdigit:]]{8}"`
echo CONFIG_PARTION_ADDR5=$ADDR_BEG
echo CONFIG_PARTION_END5=$(awk 'BEGIN{printf("0x%X", '$ADDR_END' + 1)}')
echo CONFIG_STATIC_ADDR5=$ADDR_BEG
echo CONFIG_IMG_LOAD_ADDR5=$ADDR_BEG
echo
fi
echo '#****************************#'
