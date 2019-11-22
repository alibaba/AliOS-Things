NAME := board_mt7682s

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := configuration for board mt7682s
MODULE             := 1062
HOST_ARCH          := Cortex-M4
HOST_MCU_FAMILY    := mcu_mtk7682
SUPPORT_MBINS      := no
HOST_MCU_NAME      := MTK7682
ENABLE_VFP         := 1

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) newlib_stub kernel_init netmgr lwip

GLOBAL_DEFINES += CLI_CONFIG_STACK_SIZE=4096

$(NAME)_SOURCES += config/k_config.c
$(NAME)_SOURCES += config/partition_conf.c
$(NAME)_SOURCES += startup/board.c
$(NAME)_SOURCES += startup/startup.c

# Drivers - sys
$(NAME)_SOURCES += drivers/sys/src/system_mt7682.c
$(NAME)_SOURCES += drivers/sys/src/port_systick.c
$(NAME)_SOURCES += drivers/sys/src/nvdm_config.c
$(NAME)_SOURCES += drivers/sys/src/mem_layout_info.c
$(NAME)_SOURCES += drivers/sys/src/sys_init.c
$(NAME)_SOURCES += drivers/sys/src/regions_init.c
$(NAME)_SOURCES += drivers/sys/src/spi_io.c
$(NAME)_SOURCES += drivers/sys/src/lsdspi_flash.c

GLOBAL_INCLUDES += drivers/sys/inc/

# Drivers - hw
$(NAME)_SOURCES += drivers/hal_hardware_port.c

# Drivers - ethernetif
$(NAME)_SOURCES += drivers/arch/wifi_lwip_helper.c
$(NAME)_SOURCES += drivers/arch/dhcps.c
$(NAME)_SOURCES += drivers/sys/src/network_default_config.c
$(NAME)_SOURCES += drivers/sys/src/wifi_nvdm_config.c
$(NAME)_SOURCES += drivers/ethernetif.c

# Drivers - OS api
$(NAME)_SOURCES += drivers/os_api/src/os_task.c
$(NAME)_SOURCES += drivers/os_api/src/os_mutex.c
$(NAME)_SOURCES += drivers/os_api/src/os_queue.c
$(NAME)_SOURCES += drivers/os_api/src/os_sem.c
$(NAME)_SOURCES += drivers/os_api/src/os_timer.c
$(NAME)_SOURCES += drivers/os_api/src/os_event.c
$(NAME)_SOURCES += drivers/os_api/src/os_mem.c
$(NAME)_SOURCES += drivers/os_api/freertos_aos/freertos_to_aos.c
GLOBAL_INCLUDES += drivers/os_api/inc
GLOBAL_INCLUDES += drivers/os_api/freertos_aos


$(NAME)_SOURCES += startup/startup_mt7682_gcc.s

GLOBAL_LDFLAGS  += -T platform/board/mt7682s/mt7682_flash.ld

GLOBAL_INCLUDES += .    \
                   config/   \
                   drivers/  \
                   startup/  \

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_mt7682s
CONFIG_SYSINFO_DEVICE_NAME := mt7682s

GLOBAL_CFLAGS += -DSYSINFO_OS_VERSION=\"$(CONFIG_SYSINFO_OS_VERSION)\"
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

