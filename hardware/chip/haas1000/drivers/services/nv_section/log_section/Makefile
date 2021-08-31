
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

ccflags-y := \
	-Iutils/crc32 \
	-Iutils/heap \
	-Iplatform/drivers/ana \
	-Iservices/norflash_api 

ifeq ($(TRACE_DUMP2FLASH),1)
ccflags-y += -DTRACE_DUMP2FLASH
endif

ifeq ($(TRACE_CRLF),1)
ccflags-y += -DTRACE_CRLF
endif

