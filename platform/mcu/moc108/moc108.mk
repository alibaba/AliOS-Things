NAME := moc108

HOST_OPENOCD := moc108

ifeq ($(CONFIG_SOFTAP),1)
GLOBAL_CFLAGS += -DCONFIG_SOFTAP
endif

$(NAME)_TYPE := kernel

$(NAME)_COMPONENTS := platform/arch/arm/armv5
$(NAME)_COMPONENTS += libc rhino yloop rhino.fs.kv alicrypto digest_algorithm
$(NAME)_COMPONENTS += network.lwip
$(NAME)_COMPONENTS += platform/mcu/moc108/aos/framework_runtime
$(NAME)_COMPONENTS += platform/mcu/moc108/aos/app_runtime
$(NAME)_COMPONENTS += prov
$(NAME)_COMPONENTS += hal

GLOBAL_DEFINES += CONFIG_MX108
GLOBAL_DEFINES += CONFIG_AOS_KV_MULTIPTN_MODE
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN=6
GLOBAL_DEFINES += CONFIG_AOS_KV_SECOND_PTN=7
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN_SIZE=4096
GLOBAL_DEFINES += CONFIG_AOS_KV_BUFFER_SIZE=8192
GLOBAL_DEFINES += CONFIG_AOS_CLI_BOARD
GLOBAL_DEFINES += CONFIG_AOS_UOTA_BREAKPOINT

GLOBAL_CFLAGS += -mcpu=arm968e-s \
                 -march=armv5te \
                 -mthumb -mthumb-interwork \
                 -mlittle-endian \
				 -w

$(NAME)_CFLAGS  += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS  += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS  += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS  += -Wno-unused-value -Wno-strict-aliasing

GLOBAL_INCLUDES += include/lwip-2.0.2 \
                   include/common \
                   include/app \
                   include/func \
                   include/os \
                   include/driver \
                   include/ip \
                   include

GLOBAL_LDFLAGS += -mcpu=arm968e-s \
                 -march=armv5te \
                 -mthumb -mthumb-interwork\
                 -mlittle-endian \
                 --specs=nosys.specs \
                 -nostartfiles \
                 $(CLIB_LDFLAGS_NANO_FLOAT)

BINS ?=
ifeq ($(BINS),)
GLOBAL_LDS_FILES += platform/mcu/moc108/linkinfo/mx108.ld.S
else ifeq ($(BINS),app)
GLOBAL_LDS_FILES += platform/mcu/moc108/linkinfo/mx108_app.ld.S
else ifeq ($(BINS),framework)
GLOBAL_LDS_FILES += platform/mcu/moc108/linkinfo/mx108_framework.ld.S
else ifeq ($(BINS),kernel)
GLOBAL_LDS_FILES += platform/mcu/moc108/linkinfo/mx108_kernel.ld.S
endif

$(NAME)_INCLUDES += aos
$(NAME)_SOURCES := aos/aos_main.c
$(NAME)_SOURCES += aos/soc_impl.c \
                   aos/trace_impl.c

#ifneq (,$(filter network.umesh,$(COMPONENTS)))
$(NAME)_SOURCES +=  hal/mesh_wifi_hal.c
#endif

$(NAME)_PREBUILT_LIBRARY := lib/gcc/moc108.a
