cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

ifeq ($(USE_KNOWLES),1)
obj-y += comminication_knowles/
endif

ccflags-y += \
	-Iapps/common \
	-Iplatform/drivers/usb/usb_dev/inc \
	-Iplatform/drivers/ana \
	$(BT_IF_INCLUDES) \
	-Iservices/communication/comminication_knowles \
	-Ithirdparty/knowles_uart_lib \
	-Iutils/crc32

