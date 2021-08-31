
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

ccflags-y := \
	-I$(obj)/../../mbed/api/ \
	-I$(obj)/../../mbed/common \
	-I$(obj)/../../mbed/targets/hal/TARGET_BEST/TARGET_BEST100X/TARGET_MBED_BEST1000/ \
	-I$(obj)/../../cmsis/inc/ \
	-I$(obj)/../../mbed/targets/hal/TARGET_BEST/TARGET_BEST100X/ \
	-I$(obj)/../../mbed/hal/ \
	-I$(obj)/../../fs/fat/ \
	-I$(obj)/../../fs/sd/ \
	-I$(obj)/../../fs/fat/ChaN \
	-I$(obj)/../../rtos/rtos/ \
	-I$(obj)/../../iabt/inc/

ccflags-y += -DTARGET_LPC1768 -DWORDS_STACK_SIZE=1024 -DOS_TIMERSTKSZ=1024 -D__CORTEX_M4
