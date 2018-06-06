#!/bin/bash
out_path_file=$1

srecmap -c platform/mcu/rda8955/build/8955_map_cfg -m flsh_spi32m -b platform/mcu/rda8955/build/targetgen $1.srec  $1

python platform/mcu/rda8955/build/dual_boot_merge.py --bl="platform/mcu/rda8955/build/blfota_8955_modem_test_debug_flash.lod" \
	--lod="$1flash.lod" \
	--output="$1_final.lod"
