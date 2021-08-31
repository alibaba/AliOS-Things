
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

ccflags-y := \
	-Iplatform/cmsis/inc/ \

ifeq ($(ROM_UTILS_ON),1)
ccflags-y += -DCQ_FUNC_ATTR=WEAK
else
ccflags-y += -DCQ_FUNC_ATTR=
endif

