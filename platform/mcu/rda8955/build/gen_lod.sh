srecmap -c platform/mcu/rda8955/build/8955_map_cfg -m flsh_spi32m -b platform/mcu/rda8955/build/targetgen out/helloworld_nocli@rda8955/binary/helloworld_nocli@rda8955.srec  out/helloworld_nocli@rda8955/binary/helloworld_nocli@rda8955

python platform/mcu/rda8955/build/dual_boot_merge.py --bl="platform/mcu/rda8955/build/blfota_8955_modem_test_debug_flash.lod" \
	--lod="out/helloworld_nocli@rda8955/binary/helloworld_nocli@rda8955flash.lod" \
	--output="out/helloworld_nocli@rda8955/binary/helloworld_nocli@rda8955_final.lod"