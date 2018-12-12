NAME := board_frdmkl28z

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := configuration for board frdmkl28z

MODULE          := 28
HOST_ARCH       := Cortex-M0
HOST_MCU_FAMILY := mcu_mkl28z7impl

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_MKL28Z7
CONFIG_SYSINFO_DEVICE_NAME   := MKL28Z7
GLOBAL_CFLAGS                += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS                += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
GLOBAL_CFLAGS                += -DSYSINFO_ARCH=\"$(HOST_ARCH)\"
GLOBAL_CFLAGS                += -DSYSINFO_MCU=\"$(HOST_MCU_FAMILY)\"
GLOBAL_CFLAGS                += -DCONFIG_NO_TCPIP
GLOBAL_CFLAGS                += -DFRDM_KL28Z -DFREEDOM

GLOBAL_INCLUDES += .

$(NAME)_SOURCES :=
$(NAME)_SOURCES += ./board.c
$(NAME)_SOURCES += ./clock_config.c
$(NAME)_SOURCES += ./pin_mux.c

TEST_COMPONENTS += certificate_test
GLOBAL_CFLAGS   += -DTEST_CONFIG_KV_ENABLED=0
GLOBAL_CFLAGS   += -DTEST_CONFIG_YLOOP_ENABLED=1
