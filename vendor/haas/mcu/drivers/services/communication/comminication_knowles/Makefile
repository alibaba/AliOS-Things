cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.s))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))

obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)


subdir-ccflags-y += \
	-Iapps/common \
	-Iplatform/drivers/usb/usb_dev/inc \
	-Iplatform/drivers/ana \
	-Iutils/crc32 \
	-Iservices/bt_app \
	-Iservices/communication/comminication_knowles \
	-Iservices/audio_dump/include \
	-Iutils/cqueue \
	$(BT_IF_INCLUDES) \
	-Ithirdparty/userapi/knowles_uart_app \
	-Ithirdparty/knowles_uart_lib

