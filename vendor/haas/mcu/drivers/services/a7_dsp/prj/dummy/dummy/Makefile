
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

LIB_NAME := lib_a7_dummy
ifneq ($(SDK), 1)

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.s))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

src_obj := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)
$(LIB_NAME)-y := $(src_obj)
endif

obj-y := $(LIB_NAME).a

ccflags-y += \
             -Iservices/transq_msg \
             -Iservices/sys_time/ \
             -Iservices/a7_dsp/common/ \
             -Iservices/a7_dsp/prj/genie/ \
             -Iutils/hwtimer_list \
             -Iutils/ \
             -Inet \
             -Inet/net_os \
             -Iapps/common
