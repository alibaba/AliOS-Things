#!/bin/sh


PARA=$1
PARA_STRIP=${PARA#*=}
LINK_OUTPUT_DIR=${PARA_STRIP%.*}
LINK_OUTPUT_FILE=${LINK_OUTPUT_DIR}.elf
#echo ${LINK_OUTPUT_DIR}
#echo ${LINK_OUTPUT_FILE}

OUTPUT_DIR=$PWD
TOP_DIR=${PWD}/../..
AMEBAZ_DIR=${TOP_DIR}/hardware/chip/rtl872xd/
#${${HOST_MCU_FAMILY}_LOCATION}
BIN_DIR=${TOP_DIR}/hardware/chip/rtl872xd/bin/
AMEBAZ_TOOLDIR=${AMEBAZ_DIR}/tools

OTA_BIN_OUTPUT_FILE=${OUTPUT_DIR}/binary/ota.bin
ALL_BIN_OUTPUT_FILE=${OUTPUT_DIR}/binary/all.bin
BIN_OUTPUT_FILE=${OUTPUT_DIR}/binary/burn.bin
ota=0x08006000

AMEBAZ_TOOLDIR=${AMEBAZ_DIR}/tools

PREPENDTOOL=${AMEBAZ_DIR}/prepend_header.sh
GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT=${AMEBAZ_DIR}/gen_common_bin_output_file.py

KM0_BOOT_FILE=${BIN_DIR}/km0_boot_all.bin
KM0_BOOT_OFFSET=0x0

KM4_BOOT_FILE=${BIN_DIR}/km4_boot_all.bin
KM4_BOOT_OFFSET=0x4000

ATE_FILE=${BIN_DIR}/ate.bin
ATE_OFFSET=0x188000

OTA_OFFSET=0x6000

PYTHON=python

TOOLCHAIN_PATH=${HOME}"/.aliot/arm-none-eabi/main/bin/"

OBJCOPY=${TOOLCHAIN_PATH}arm-none-eabi-objcopy
NM=${TOOLCHAIN_PATH}arm-none-eabi-nm
STRIP=${TOOLCHAIN_PATH}arm-none-eabi-strip
CC_SIZE=${TOOLCHAIN_PATH}arm-none-eabi-size
OBJDUMP=${TOOLCHAIN_PATH}arm-none-eabi-objdump

CAT=cat
FROMELF=${TOOLCHAIN_PATH}arm-none-eabi-objcopy

#${OUTPUT_DIR}/binary/

#BIN_OUTPUT_FILE
IMAGE_TARGET_FOLDER=${OUTPUT_DIR}/binary


# EXTRA_POST_BUILD_TARGETS += gen_cm0_cm4_bin

gen_cm0_cm4_bin() {
	echo "========= linker img2_ns start ========="
    mkdir ${IMAGE_TARGET_FOLDER}
	cp ${LINK_OUTPUT_FILE}  ${IMAGE_TARGET_FOLDER}/target_img2.axf
	#${MOVE} target_img2.axf ${IMAGE_TARGET_FOLDER}
	${NM} ${IMAGE_TARGET_FOLDER}/target_img2.axf | sort > ${IMAGE_TARGET_FOLDER}/target_img2.map
	${OBJDUMP} -d ${IMAGE_TARGET_FOLDER}/target_img2.axf > ${IMAGE_TARGET_FOLDER}/target_img2.asm
	#${OBJDUMP} -h -S --disassemble-all ${IMAGE_TARGET_FOLDER}/target_img2.axf > ${IMAGE_TARGET_FOLDER}/target_img2.txt
	cp ${IMAGE_TARGET_FOLDER}/target_img2.axf ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf
	${STRIP} ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf
	${NM} ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf | sort > ${IMAGE_TARGET_FOLDER}/target_pure_img2.map
	# ${FROMELF} -Obinary ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf ${IMAGE_TARGET_FOLDER}/target_img2.bin

	${FROMELF} -j .ram_image2.entry -j .ram_image2.text -j .ram_image2.data -Obinary ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf ${IMAGE_TARGET_FOLDER}/ram_2.bin

	${FROMELF} -j .xip_image2.text -Obinary ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf ${IMAGE_TARGET_FOLDER}/xip_image2.bin

	# for Bluetooth trace
	${FROMELF} -j .bluetooth_trace.text -Obinary ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf ${IMAGE_TARGET_FOLDER}/APP.trace


	echo "========== Image Info HEX =========="
	${CC_SIZE} -A --radix=16 ${IMAGE_TARGET_FOLDER}/target_img2.axf
	${CC_SIZE} -t --radix=16 ${IMAGE_TARGET_FOLDER}/target_img2.axf
	echo "========== Image Info HEX =========="

	echo "========== Image Info DEC =========="
	${CC_SIZE} -A --radix=10 ${IMAGE_TARGET_FOLDER}/target_img2.axf
	${CC_SIZE} -t --radix=10 ${IMAGE_TARGET_FOLDER}/target_img2.axf
	echo "========== Image Info DEC =========="

	echo "========== linker img2_ns end =========="

	echo "========== Image manipulating start =========="
	${PYTHON} ${AMEBAZ_DIR}/prepend_header.py ${IMAGE_TARGET_FOLDER}/ram_2.bin  __ram_image2_text_start__  ${IMAGE_TARGET_FOLDER}/target_img2.map
	${PYTHON} ${AMEBAZ_DIR}/prepend_header.py ${IMAGE_TARGET_FOLDER}/xip_image2.bin  __flash_text_start__  ${IMAGE_TARGET_FOLDER}/target_img2.map
	# ${AMEBAZ_DIR}/prepend_header.sh ${IMAGE_TARGET_FOLDER}/ram_2.bin  __ram_image2_text_start__  ${IMAGE_TARGET_FOLDER}/target_img2.map
	# ${AMEBAZ_DIR}/prepend_header.sh ${IMAGE_TARGET_FOLDER}/xip_image2.bin  __flash_text_start__  ${IMAGE_TARGET_FOLDER}/target_img2.map

	${CAT} ${IMAGE_TARGET_FOLDER}/xip_image2_prepend.bin ${IMAGE_TARGET_FOLDER}/ram_2_prepend.bin > ${IMAGE_TARGET_FOLDER}/km4_image2_all.bin
	${PYTHON} ${AMEBAZ_DIR}/pad.py ${IMAGE_TARGET_FOLDER}/km4_image2_all.bin
	${CAT} ${BIN_DIR}/km0_image2_all.bin  ${IMAGE_TARGET_FOLDER}/km4_image2_all.bin > ${IMAGE_TARGET_FOLDER}/km0_km4_image2.bin

	cp ${IMAGE_TARGET_FOLDER}/km0_km4_image2.bin ${BIN_OUTPUT_FILE}
	echo "========== Image manipulating end =========="

	echo "========== Generate ota.bin =========="
	rm -rf ${OTA_BIN_OUTPUT_FILE}
	${PYTHON} ${AMEBAZ_DIR}/ota.py ${IMAGE_TARGET_FOLDER}/km0_km4_image2.bin 0xFFFFFFFF ${OTA_BIN_OUTPUT_FILE}
	echo "========== Generate ota.bin end =========="

	echo "========== Generate all.bin =========="
	rm -rf ${ALL_BIN_OUTPUT_FILE}
	${PYTHON} ${GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT} -o ${ALL_BIN_OUTPUT_FILE} -f ${KM0_BOOT_OFFSET}   ${KM0_BOOT_FILE}
	${PYTHON} ${GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT} -o ${ALL_BIN_OUTPUT_FILE} -f ${KM4_BOOT_OFFSET}   ${KM4_BOOT_FILE}
	${PYTHON} ${GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT} -o ${ALL_BIN_OUTPUT_FILE} -f ${OTA_OFFSET}        ${IMAGE_TARGET_FOLDER}/km0_km4_image2.bin
# ifneq (${wildcard ${ATE_FILE}},}
# 	${PYTHON} ${GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT} -o ${ALL_BIN_OUTPUT_FILE} -f ${ATE_OFFSET}        ${ATE_FILE}
# endif
# if (${wildcard ${ATE_FILE}},}
if [ -n "$ATE_FILE" ];then
	${PYTHON} ${GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT} -o ${ALL_BIN_OUTPUT_FILE} -f ${ATE_OFFSET}        ${ATE_FILE}
fi
	${AMEBAZ_TOOLDIR}/genfs.sh
	cp ${AMEBAZ_DIR}/prebuild/littlefs.bin ${IMAGE_TARGET_FOLDER}/littlefs.bin
	echo "========== Generate all.bin end =========="
}

gen_cm0_cm4_bin

if [ ! -f ${BIN_OUTPUT_FILE} ];
then
    echo "run external script failed"
else
    echo "run external script success"
fi
