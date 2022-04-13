#!/bin/bash
# redirect to tmp_build.info
log_path="tmp_build.info"
log_str=""
#log_str=" 2>&1 | tee ${log_path}"

echo "***************************** get option $1 $2 $3 ***************************"

COM_OPT="make T=haas1000_normalization PSRAMUHS_ENABLE=1 PSRAM_ENABLE=1 ALIOS_THINGS_RELEASE=1 CONTRA_EN=1 CMSIS_AOS=1 -j8"

# build cmd define
#make_a7="make T=haas1000_a7 -j8 A7_ALI_LIB=1 KERNEL=RHINO $1 $2 $3"
make_a7="make T=haas1000_a7 -j8 $1 $2 $3"
make_contra_mm_debug="$COM_OPT RF_USE_SP3T=0 RHINO_MM_DEBUG=1 $1 $2 $3"
make_contra_sp3t="$COM_OPT RF_USE_SP3T=0 $1 $2 $3"
make_contra_psram_link="$COM_OPT OTA_CODE_OFFSET=0x2A000 scripts/link $1 $2 $3"

# #del tmp_build.info
function del_file()
{
   if [ -f $log_path ]; then
      rm -rf $log_path
   fi
}

function build_result()
{
   if [ $? -ne 0 ];then
      echo -e "\033[41mbuild err: $@ \033[0m"
      del_file
      exit 1
   fi
   del_file
}

CRTDIR=$(pwd)

if [ -d "../../../../build/compiler/" ];then
   export PATH=$CRTDIR/../../../../build/compiler/gcc-arm-alios-eabi/Linux64/bin:$PATH
fi
which arm-alios-eabi-gcc
sleep 1

# clear out/
rm -rf out

# make_a7
#build_cmd=$make_a7$log_str
$build_cmd
#grep -l "error:" $log_path
#build_result ${make_a7}

# compress dsp bin
#tools/lzma e out/haas1000_a7/haas1000_a7.bin out/haas1000_a7/haas1000_a7.bin.lzma
#mv out/haas1000_a7/haas1000_a7.bin.lzma out/haas1000_a7/haas1000_a7.bin

if [[ "$1" == "GEN_LIB=1" || "$1" == "SDK=1" ]]; then
   # make_contra_mm_debug
   build_cmd=$make_contra_mm_debug$log_str
   $build_cmd
   #grep -l "error:" $log_path
   build_result ${make_contra_mm_debug}

   # make_contra_psram_link
   build_cmd=$make_contra_psram_link$log_str
   ${build_cmd}
   #grep -l "error:" $log_path
   #build_result ${make_contra_psram_link}
   mv out/haas1000_normalization/_haas1000_alios.lds out/haas1000_normalization/_haas1000_alios_sec_b.lds
else
   # make_contra_psram_link
   build_cmd=$make_contra_sp3t$log_str
   ${build_cmd}
fi

if [ "$1" == "GEN_LIB=1" ]; then
   # release script
   tools/relsw_haas1000_normalization haas1000_normalization WORKING
elif [ "$1" == "SDK=1" ]; then
   result=$(echo $make_contra_sp3t | grep "CMSIS_AOS=1")
   if [ "$result" != "" ];then
      mv ./out/haas1000_normalization/rtos/rhino/cmsis/cmsis_aos.o ./out/haas1000_normalization/rtos/rhino/cmsis/cmsis_os.o
   fi

   result=$(echo $make_contra_sp3t | grep "A7_DSP_ENABLE=1")
   if [ "$result" != "" ];then
      ar -M <tools/haas1000_normalization_witha7.mri
   else
      ar -M <tools/haas1000_normalization.mri
   fi
   rm -rf tmp;mkdir tmp;mv libtmp.a tmp/;cp tools/haas1000_libs_*.mri tmp/
   cd tmp/;ar x libtmp.a;rm libtmp.a
   ar -M <haas1000_libs_wifi.mri
   ar -M <haas1000_libs_bt.mri
   ar -M <haas1000_libs_common.mri
   ar -M <haas1000_libs_audio.mri
   ar -M <haas1000_libs_dma.mri
   ar -M <haas1000_libs_usb.mri
   ar -M <haas1000_libs_wpa.mri
   ar -M <haas1000_libs_cmsis.mri
   ar -M <haas1000_libs_others.mri
   mv lib*.a ../;cd ../;rm -rf tmp
   cp out/haas1000_normalization/_haas1000_alios_sec_b.lds ./
   #mv out/haas1000_a7/haas1000_a7.bin ../aos/dsp/dsp.bin.lzma
   #tar cfvz out/haas1000_a7/haas1000_a7.elf.tar.gz out/haas1000_a7/haas1000_a7.elf
   #tar cfvz out/haas1000_a7/haas1000_a7.lst.tar.gz out/haas1000_a7/haas1000_a7.lst
   #tar cfvz out/haas1000_a7/haas1000_a7.map.tar.gz out/haas1000_a7/haas1000_a7.map
   #cp out/haas1000_a7/haas1000_a7.*.tar.gz ../aos/dsp/
fi
