EXTRA_POST_BUILD_TARGETS += gen_mklitlefs_data
GEN_MK_DATA_FILE:= haas1000_genfs.sh

gen_mklitlefs_data:
	echo "make littlefs"
	cd ./$($(HOST_MCU_FAMILY)_LOCATION)release/auto_build_tool/ && ./$(GEN_MK_DATA_FILE)
	echo "make littlefs done"