cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj-y := $(obj-y:.c=.o)

ifeq ($(MAKE_CRC32_TABLE),1)
ccflags-y += -DMAKE_CRC32_TABLE
endif
