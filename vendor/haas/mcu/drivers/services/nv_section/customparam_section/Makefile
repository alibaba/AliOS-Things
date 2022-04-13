
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

ccflags-y := \
	-I$(obj)/../../nv_section/customparam_section \
	-I$(obj)/../../nv_section/include \
	-I$(obj)/../platform/cmsis/inc	\
	-Ithirdparty/bluetooth/bt_host/bt_common/include/common	\
	-Ithirdparty/bluetooth/bt_host/bt_common/include/net	\
	-Ithirdparty/bluetooth/bt_host/bt_common/include/misc \
	-Ithirdparty/bluetooth/bt_host/bt_common/include/zephyr	\
	-Ithirdparty/bluetooth/bt_host/bt_common/port/include \
	-Ithirdparty/bluetooth/bt_host/bt_common/tinycrypt/include \
	-Ithirdparty/bluetooth/bt_host/bt_common/tinycrypt/include/tinycrypt \
	-Iutils/crc32 \
