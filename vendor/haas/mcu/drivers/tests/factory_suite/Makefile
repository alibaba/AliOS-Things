cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

ifeq ($(RTOS),1)
ccflags-y += -DRTOS
endif

ccflags-y +=  \
    -Iapps/audioplayers \
    -Iapps/battery \
    -Iapps/common \
    -Iapps/main \
    -Iapps/factory \
    -Iapps/pwl \
    -Iapps/key \
    -Iutils/boot_struct \
    -Iutils/list \
    -Iservices/bt_app \
    -Iservices/nvrecord \
    $(BT_PROFILES_INCLUDES) \
    -Iservices/resources \
    -Iservices/overlay \
    -Iplatform/drivers/bt \
    -Iplatform/drivers/ana

