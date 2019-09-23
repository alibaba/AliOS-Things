#!/bin/sh
<<<<<<< HEAD
<<<<<<< HEAD
export PATH=${PATH}:$5
ota_idx=$1
=======
=======
export PATH=${PATH}:$5
>>>>>>> 6177c74fe (code synchronization at 2018-06-06@13:59)
ota_offset=$1
>>>>>>> 61271e937 (make the flash address of image2_all_ota1.bin can be modified.)
#dir=/home/cwhaiyi/pcshare/rualxw/AliOS-Things/platform/mcu/rtl8710bn
platform_dir=$2/platform/mcu/rtl8710bn
if [ ! -d "${platform_dir}/Debug/Exe" ]; then
  mkdir -p ${platform_dir}/Debug/Exe
fi
BIN_DIR=${platform_dir}/Debug/Exe
app=`echo $3 | tr '/' '.'`
outputplatform=$app@$4
outputdir=$2/out/${outputplatform}/binary
if [ "${ota_offset}" = "0x0800B000" ]; then
	outputname=${outputplatform}.2ndboot
else
	outputname=${outputplatform}
fi
OS=`uname -s`
PICK=${platform_dir}/tools/pick
PAD=${platform_dir}/tools/padding
CHKSUM=${platform_dir}/tools/checksum
OTA=${platform_dir}/tools/ota
echo ${platform_dir}
echo ""

echo -n "install dependent software packages ..."

if [ "$OS" = "Darwin" ]; then
	 if [ "`which gawk`" = "" ];then
        	sudo easy_install gawk > /dev/null
   	 fi
else #Some Linux version
	if [ "`which apt-get`" != "" ]; then
		if [ "`which gawk`" = "" ];then
			sudo sudo apt-get -y install gawk > /dev/null
		fi
	 fi
fi
find ${BIN_DIR}/ -name "*.axf" | xargs rm -rf
find ${BIN_DIR}/ -name "*.map" | xargs rm -rf
rm -f ${outputdir}/${outputname}.bin
cp ${outputdir}/${outputname}.elf ${BIN_DIR}/${outputname}.axf
arm-none-eabi-nm ${BIN_DIR}/${outputname}.axf | sort > ${BIN_DIR}/${outputname}.nmap
arm-none-eabi-objcopy -j .ram_image2.entry -j .ram_image2.data -j .ram_image2.text -j .ram_image2.bss -j .ram_image2.skb.bss -j .ram_heap.data -Obinary ${BIN_DIR}/${outputname}.axf ${BIN_DIR}/ram_2.r.bin
arm-none-eabi-objcopy -j .xip_image2.text -Obinary ${BIN_DIR}/${outputname}.axf ${BIN_DIR}/xip_image2.bin
arm-none-eabi-objcopy -j .ram_rdp.text -Obinary ${BIN_DIR}/${outputname}.axf ${BIN_DIR}/rdp.bin
if [ ! -f "${BIN_DIR}/bin/boot_all.bin" ]; then
	cp ${platform_dir}/bin/boot_all.bin ${BIN_DIR}/boot_all.bin
fi
chmod 777 ${BIN_DIR}/boot_all.bin
chmod +rx ${PICK} ${CHKSUM} ${PAD} ${OTA}
${PICK} 0x`grep __ram_image2_text_start__ ${BIN_DIR}/${outputname}.nmap | gawk '{print $1}'` 0x`grep __ram_image2_text_end__ ${BIN_DIR}/${outputname}.nmap | gawk '{print $1}'` ${BIN_DIR}/ram_2.r.bin ${BIN_DIR}/ram_2.bin raw
${PICK} 0x`grep __ram_image2_text_start__ ${BIN_DIR}/${outputname}.nmap | gawk '{print $1}'` 0x`grep __ram_image2_text_end__ ${BIN_DIR}/${outputname}.nmap | gawk '{print $1}'` ${BIN_DIR}/ram_2.bin ${BIN_DIR}/ram_2.p.bin
${PICK} 0x`grep __xip_image2_start__ ${BIN_DIR}/${outputname}.nmap | gawk '{print $1}'` 0x`grep __xip_image2_start__ ${BIN_DIR}/${outputname}.nmap | gawk '{print $1}'` ${BIN_DIR}/xip_image2.bin ${BIN_DIR}/xip_image2.p.bin
IMAGE2_OTA1=image2_2ndboot.bin
IMAGE2_OTA2=image2_app.bin
OTA_ALL=ota_all.bin
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
if [ "${ota_idx}" = "1" ]; then
=======
if [ "${ota_idx}" == "1" ]; then
>>>>>>> 4109dccdc (supporting linux platform to generate bin file)
=======
if [ "${ota_idx}" = "1" ]; then
>>>>>>> d09a5fc6f (fix the incompatibility of scripting language)
=======
=======

<<<<<<< HEAD
#2boot bin
>>>>>>> ea3b1f788 (BugID:17115756:[OTA] support mk3080 2nd bootloader)
=======
#2ndboot bin
>>>>>>> c9aad2eb5 (BugID:20262438: OTA refactor for 2.2.1)
if [ "${ota_offset}" = "0x0800B000" ]; then
>>>>>>> 61271e937 (make the flash address of image2_all_ota1.bin can be modified.)
	cat ${BIN_DIR}/xip_image2.p.bin > ${BIN_DIR}/${IMAGE2_OTA1}
	chmod 777 ${BIN_DIR}/${IMAGE2_OTA1}
	cat ${BIN_DIR}/ram_2.p.bin >> ${BIN_DIR}/${IMAGE2_OTA1}
	${CHKSUM} ${BIN_DIR}/${IMAGE2_OTA1} || true
	rm ${BIN_DIR}/xip_image2.p.bin ${BIN_DIR}/ram_2.p.bin
<<<<<<< HEAD
<<<<<<< HEAD
elif [ "${ota_idx}" = "2" ]; then
=======
=======
	cp ${platform_dir}/bin/boot_all.bin ${outputdir}/boot_all.bin
	cp ${BIN_DIR}/${IMAGE2_OTA1} ${outputdir}/${IMAGE2_OTA1}
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> 355f80e8c (Modified script file content format.)
elif [ "${ota_idx}" == "2" ]; then
>>>>>>> 4109dccdc (supporting linux platform to generate bin file)
=======
elif [ "${ota_idx}" = "2" ]; then
>>>>>>> d09a5fc6f (fix the incompatibility of scripting language)
=======
=======

>>>>>>> ea3b1f788 (BugID:17115756:[OTA] support mk3080 2nd bootloader)
else
>>>>>>> 61271e937 (make the flash address of image2_all_ota1.bin can be modified.)
	cat ${BIN_DIR}/xip_image2.p.bin > ${BIN_DIR}/${IMAGE2_OTA2}
	chmod 777 ${BIN_DIR}/${IMAGE2_OTA2}
	cat ${BIN_DIR}/ram_2.p.bin >> ${BIN_DIR}/${IMAGE2_OTA2}
	${CHKSUM} ${BIN_DIR}/${IMAGE2_OTA2} || true
	rm ${BIN_DIR}/xip_image2.p.bin ${BIN_DIR}/ram_2.p.bin
	cp ${BIN_DIR}/${IMAGE2_OTA2} ${outputdir}/${IMAGE2_OTA2}
fi
<<<<<<< HEAD
<<<<<<< HEAD
#	rm -f ${BIN_DIR}/ram_2.bin ${BIN_DIR}/ram_2.p.bin ${BIN_DIR}/ram_2.r.bin ${BIN_DIR}/xip_image2.bin ${BIN_DIR}/xip_image2.p.bin
=======
#	rm -f ${BIN_DIR}/ram_2.bin ${BIN_DIR}/ram_2.p.bin ${BIN_DIR}/ram_2.r.bin ${BIN_DIR}/xip_image2.bin ${BIN_DIR}/xip_image2.p.bin
>>>>>>> 4109dccdc (supporting linux platform to generate bin file)
=======
#rm -f ${BIN_DIR}/ram_2.bin ${BIN_DIR}/ram_2.p.bin ${BIN_DIR}/ram_2.r.bin ${BIN_DIR}/xip_image2.bin ${BIN_DIR}/xip_image2.p.bin
>>>>>>> 355f80e8c (Modified script file content format.)
