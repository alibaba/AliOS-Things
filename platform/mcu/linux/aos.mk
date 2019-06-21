HOST_OPENOCD := linux

NAME := mcu_linuximpl

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := driver & sdk for platform/mcu linuximpl

no_with_lwip ?= 1

$(NAME)_COMPONENTS := arch_linux netmgr kernel_init

ifeq ($(openssl),1)
GLOBAL_LDFLAGS += -lssl -lcrypto
endif

ifeq ($(gcov),1)
GLOBAL_DEFINES += GCOV_ENABLE
GLOBAL_CFLAGS  += -fprofile-arcs -ftest-coverage
GLOBAL_LDFLAGS += --coverage
endif

ifeq ($(gprof),1)
GLOBAL_CFLAGS  += -pg
GLOBAL_LDFLAGS += -pg
endif

ifeq ($(valgrind), 1)
GLOBAL_CFLAGS += $(shell [ -f /usr/include/valgrind/valgrind.h ] && echo -DHAVE_VALGRIND_VALGRIND_H)
GLOBAL_CFLAGS += $(shell [ -f /usr/include/valgrind.h ] && echo -DHAVE_VALGRIND_H)
endif

$(NAME)_CFLAGS += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS += -Wno-unused-value -Wno-strict-aliasing

$(NAME)_INCLUDES += .
GLOBAL_INCLUDES  += include include/aos csp/lwip/include
GLOBAL_LDFLAGS   += -lpthread -lm -lrt
GLOBAL_DEFINES   += CONFIG_AOS_RHINO_MMREGION
GLOBAL_DEFINES   += CONFIG_YSH_CMD_DUMPSYS
GLOBAL_CFLAGS    += -Wall -Wno-missing-field-initializers -Wno-strict-aliasing -Wno-address -Wno-unused-result
GLOBAL_DEFINES   += CSP_LINUXHOST
GLOBAL_DEFINES   += CONFIG_AOS_FATFS_SUPPORT
GLOBAL_DEFINES   += CONFIG_AOS_FATFS_SUPPORT_MMC
GLOBAL_DEFINES   += CONFIG_AOS_UOTA_BREAKPOINT
GLOBAL_DEFINES   += DEBUG_CONFIG_ERRDUMP=0

$(NAME)_SOURCES :=

ifneq ($(osal),posix)
$(NAME)_SOURCES += soc/soc_impl.c
$(NAME)_SOURCES += soc/hook_impl.c
else
$(NAME)_DEFINES += CONFIG_OSAL_POSIX
endif

$(NAME)_SOURCES += soc/uart.c

# mcu
$(NAME)_SOURCES += main/arg_options.c
$(NAME)_SOURCES += main/main.c
$(NAME)_SOURCES += main/hw.c
$(NAME)_SOURCES += main/wifi_port.c
$(NAME)_SOURCES += main/nand.c
$(NAME)_SOURCES += main/vfs_trap.c
$(NAME)_SOURCES += main/ota.c

ifneq (,$(filter fatfs,$(COMPONENTS)))
$(NAME)_SOURCES += main/sdmmc.c
endif

ifeq ($(linux80211),1)
$(NAME)_SOURCES += csp/wifi/common.c
$(NAME)_SOURCES += csp/wifi/linux.c
$(NAME)_SOURCES += csp/wifi/osdep.c
ifeq ($(HOST_ARCH), rockchiplinux)
$(NAME)_SOURCES += csp/wifi/mesh-rk1108.c
else
$(NAME)_SOURCES += csp/wifi/mesh.c
endif
$(NAME)_SOURCES += csp/wifi/radiotap/radiotap.c
$(NAME)_DEFINES += LINUX_MESH_80211
$(NAME)_CFLAGS  += -Wno-unused-but-set-variable
endif

ifeq ($(AOS_COMP_LWIP),y)
$(NAME)_SOURCES += csp/lwip/netif/delif.c   \
                   csp/lwip/netif/fifo.c    \
                   csp/lwip/netif/tapif.c   \
                   csp/lwip/netif/tcpdump.c \
                   csp/lwip/netif/tunif.c

$(NAME)_SOURCES += csp/lwip/lwip_linuxhost.c
endif
