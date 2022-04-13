cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj-y := $(obj-y:.c=.o)

ccflags-y := -Iplatform/hal -Iplatform/cmsis

ifneq ($(HWTIMER_NUM),)
ccflags-y += -DHWTIMER_NUM=$(HWTIMER_NUM)
endif

