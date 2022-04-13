EXTRA_POST_BUILD_TARGETS += gen_2ndboot_images

MODULE_LIB_PATH = out/$(APP_FULL)@$(PLATFORM)/libraries/
DST_LIB_PATH = ./$($(HOST_MCU_FAMILY)_LOCATION)/drivers/tests/ota_boot_secboot/lib/

gen_2ndboot_images:
	echo "gen 2ndboot image"
	cp $(MODULE_LIB_PATH)/mcu_haas1000.a $(DST_LIB_PATH)/mcu_haas1000.a
	cp $(MODULE_LIB_PATH)/board_haas100.a $(DST_LIB_PATH)/board_haas100.a
	cp $(MODULE_LIB_PATH)/ota_2ndboot.a $(DST_LIB_PATH)/ota_2ndboot.a
	cp $(MODULE_LIB_PATH)/ota_updater.a $(DST_LIB_PATH)/ota_updater.a
	cd ./$($(HOST_MCU_FAMILY)_LOCATION)/drivers && . ./build2boot.sh


