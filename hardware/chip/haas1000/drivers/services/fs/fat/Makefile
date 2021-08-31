
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ChaN/*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))
obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ChaN/*.cpp))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

ccflags-y := \
	-Iservices/fs/fat/ \
	-Iservices/fs/sd/ \
	-Iservices/fs/fat/ChaN

