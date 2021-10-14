#!/bin/bash
# redirect to tmp_build.info
log_path="tmp_build.info"
log_str=""
A7_PROJECT_NAME=kws
#log_str=" 2>&1 | tee ${log_path}"

echo "***************************** get option $1 $2 $3 ***************************"

COM_OPT="make T=haas1000_normalization PSRAM_ENABLE=1 ALIOS_THINGS_RELEASE=1 CONTRA_EN=1 MARIE_EN=1 USB_SLEEP_TEST=1 SUPPORT_IR=1 -j8 "
COM_OPT_A7=" A7_PROJECT=$A7_PROJECT_NAME A7_DSP_ENABLE=1 PSRAMUHS_ENABLE=1 "

# build cmd define
#make_a7="make T=haas1000_a7 -j8 A7_ALI_LIB=1 KERNEL=RHINO $1 $2 $3"
if [[ "$1" == "SDK=1" ]]; then
make_a7="make T=haas1000_a7 A7_PROJECT=$A7_PROJECT_NAME A7_ALI_LIB=1 -j8 $1 $2 $3 "
else
make_a7="make T=haas1000_a7 A7_PROJECT=$A7_PROJECT_NAME -j8 $1 $2 $3 "
fi
make_contra_mm_debug="$COM_OPT RF_USE_SP3T=0 RHINO_MM_DEBUG=1 $1 $2 $3 "
make_contra_sp3t="$COM_OPT RF_USE_SP3T=0 $1 $2 $3 "
if [[ "$1" == "SDK=1" ]]; then
make_contra_psram_link="$COM_OPT OTA_CODE_OFFSET=0x2A000 scripts/link $1 $2 $3 "
else
make_contra_psram_link="$COM_OPT OTA_CODE_OFFSET=0 scripts/link $1 $2 $3 "
fi

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
if [ -d "$HOME/.aliot/arm-ali-aoseabi" ];then
   export PATH=$HOME/.aliot/arm-ali-aoseabi/main/bin:$PATH
fi

if [ -d "../shenmulite/build/compiler" ];then
   export PATH=$CRTDIR/../shenmulite/build/compiler/gcc-arm-none-eabi/Linux64/bin:$PATH
fi
arm-ali-aoseabi-gcc -v

# clear out/
rm -rf out

if [[ "$1" == "GEN_LIB=1" || "$1" == "SDK=1" ]]; then
   if [[ "$1" == "GEN_LIB=1" || "$2" != "A7_DSP_ENABLE=1" ]]; then
      echo "build m33 alone"
      sleep 2

      # make_contra_mm_debug
      build_cmd=$make_contra_mm_debug$log_str
      $build_cmd
      #grep -l "error:" $log_path
      build_result ${make_contra_mm_debug}

      if [ "$1" == "SDK=1" ]; then
         ar -M <tools/haas1000_normalization.mri
      fi

      # make_contra_psram_link
      build_cmd=$make_contra_psram_link$log_str
      ${build_cmd}
      #grep -l "error:" $log_path
      #build_result ${make_contra_psram_link}
      mv out/haas1000_normalization/_haas1000_alios.lds out/haas1000_normalization/_haas1000_alios_sec_b.lds
   fi

   if [[ "$1" == "GEN_LIB=1" || "$2" == "A7_DSP_ENABLE=1" ]]; then
      echo "build a7 dsp"
      sleep 2

      # make_a7
      build_cmd=$make_a7$log_str
      $build_cmd
      #grep -l "error:" $log_path
      build_result ${make_a7}

      if [ "$1" == "GEN_LIB=1" ]; then
         # update a7 lib
         echo "update a7 alg lib"
         cp services/a7_dsp/prj/$A7_PROJECT_NAME/$A7_PROJECT_NAME/lib/lib_a7_$A7_PROJECT_NAME.a services/a7_dsp/prj/$A7_PROJECT_NAME/lib/
      fi

      # compress dsp bin
      tools/lzma e out/haas1000_a7/haas1000_a7.bin out/haas1000_a7/haas1000_a7.bin.lzma
      mv out/haas1000_a7/haas1000_a7.bin.lzma out/haas1000_a7/haas1000_a7.bin

      echo "build m33 with dsp"
      sleep 2

      # build A7 special lib
      # make_contra_mm_debug
      build_cmd=$make_contra_mm_debug$COM_OPT_A7$log_str
      $build_cmd
      #grep -l "error:" $log_path
      build_result ${make_contra_mm_debug}

      if [ "$1" == "SDK=1" ]; then
         ar -M <tools/haas1000_normalization_witha7.mri
      fi

      # make_contra_psram_link
      build_cmd=$make_contra_psram_link$COM_OPT_A7$log_str
      ${build_cmd}
      #grep -l "error:" $log_path
      #build_result ${make_contra_psram_link}
      mv out/haas1000_normalization/_haas1000_alios.lds out/haas1000_normalization/_haas1000_alios_sec_b_a7.lds

      if [ "$1" == "SDK=1" ]; then
         cp out/haas1000_a7/haas1000_a7.bin ../aos/dsp/dsp.bin.lzma
         # make sure rebuild dsp
         find ../../../../solutions/ -name dsp.o|xargs rm
         #tar cfvz out/haas1000_a7/haas1000_a7.elf.tar.gz out/haas1000_a7/haas1000_a7.elf
         #tar cfvz out/haas1000_a7/haas1000_a7.lst.tar.gz out/haas1000_a7/haas1000_a7.lst
         #tar cfvz out/haas1000_a7/haas1000_a7.map.tar.gz out/haas1000_a7/haas1000_a7.map
         #cp out/haas1000_a7/haas1000_a7.*.tar.gz ../aos/dsp/
      fi
   fi

   if [ "$1" == "GEN_LIB=1" ]; then
      # release script
      tools/relsw_haas1000_normalization haas1000_normalization WORKING
   elif [ "$1" == "SDK=1" ]; then
      if [ "$2" == "A7_DSP_ENABLE=1" ]; then
         cp out/haas1000_normalization/_haas1000_alios_sec_b_a7.lds ../_haas1000_alios_sec_b_a7.lds
         mv libhaas1000_a7.a libmcuhaas1000_a7.a
      else
         cp out/haas1000_normalization/_haas1000_alios_sec_b.lds ../
         mv libhaas1000.a libmcuhaas1000.a
      fi
   fi
else
   # make_a7
   build_cmd=$make_a7$log_str
   $build_cmd
   #grep -l "error:" $log_path
   build_result ${make_a7}

   # compress dsp bin
   tools/lzma e out/haas1000_a7/haas1000_a7.bin out/haas1000_a7/haas1000_a7.bin.lzma
   mv out/haas1000_a7/haas1000_a7.bin.lzma out/haas1000_a7/haas1000_a7.bin

   # make_contra_psram_link
   build_cmd=$make_contra_sp3t$log_str$COM_OPT_A7
   ${build_cmd}
fi
