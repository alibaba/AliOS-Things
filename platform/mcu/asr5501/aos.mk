NAME := mcu_asr5501

HOST_OPENOCD := asr5501

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := driver & sdk for platform/mcu asr5501

LWIP := 1

ifeq ($(LWIP),1)
$(NAME)_COMPONENTS += lwip
no_with_lwip       := 0
GLOBAL_DEFINES     += WITH_LWIP
endif

$(NAME)_COMPONENTS += arch_armv7m

$(NAME)_CFLAGS += -DLEGA_CM4 -DALIOS_SUPPORT -DWIFI_DEVICE -D_SPI_FLASH_ENABLE_ -DPS_CLOSE_APLL -DDCDC_PFMMODE_CLOSE -D_SPI_FLASH_120MHz_ -DCFG_MIMO_UF
$(NAME)_CFLAGS += -DCFG_BATX=1 -DCFG_BARX=1  -DCFG_REORD_BUF=4  -DCFG_SPC=4  -DCFG_TXDESC0=4 -DCFG_TXDESC1=4 -DCFG_TXDESC2=4 -DCFG_TXDESC3=4 -DCFG_TXDESC4=4 -DCFG_CMON -DCFG_MDM_VER_V21 -DCFG_SOFTAP_SUPPORT -DCFG_SNIFFER_SUPPORT -DCFG_DBG=2 -D__FPU_PRESENT=1 -DDX_CC_TEE -DHASH_SHA_512_SUPPORTED -DCC_HW_VERSION=0xF0 -DDLLI_MAX_BUFF_SIZE=0x10000 -DSSI_CONFIG_TRNG_MODE=0
#$(NAME)_CFLAGS += -DXO_FREQ_BIAS_CONFIG

#default a0v2 config
ifeq ($(buildsoc),a0v1)
$(NAME)_CFLAGS += -DLEGA_A0V1
else
$(NAME)_CFLAGS += -DLEGA_A0V2
endif

$(NAME)_CFLAGS += -Wall -Wno-error -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS += -Wno-unused-value -Wno-strict-aliasing

BINS ?=

GLOBAL_CFLAGS   += -mcpu=cortex-m4 -mthumb -mthumb-interwork -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -w -std=gnu99 -ffunction-sections -fdata-sections
GLOBAL_ASMFLAGS += -mcpu=cortex-m4 -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16

GLOBAL_DEFINES += CONFIG_AOS_KV_MULTIPTN_MODE
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN=6
GLOBAL_DEFINES += CONFIG_AOS_KV_SECOND_PTN=7
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN_SIZE=4096
GLOBAL_DEFINES += KV_CONFIG_TOTAL_SIZE=8192
GLOBAL_DEFINES += ALI_ALGO_AES_ROM_TABLES

GLOBAL_LDFLAGS += -mcpu=cortex-m4 -mthumb -Wl,-gc-sections

GLOBAL_INCLUDES += drivers/driver/inc \
            drivers/platform/CMSIS/Include \
            drivers/common \
            drivers/platform/misc \
            drivers/platform/system/include \
            hal/inc \

#soc
$(NAME)_SOURCES += drivers/platform/system/systick_delay.c \
            drivers/platform/system/system_cm4.c \
            drivers/platform/misc/misc.c \
            drivers/common/lega_mac_addr.c

#hal
$(NAME)_SOURCES += hal/src/hw.c \
            hal/src/gpio.c \
            hal/src/uart.c \
            hal/src/flash.c \
            hal/src/wdg.c   \
            hal/src/wifi_port.c \
            hal/src/pwrmgmt_hal/board_cpu_pwr.c \
            hal/src/pwrmgmt_hal/board_cpu_pwr_rtc.c \
            hal/src/pwrmgmt_hal/board_cpu_pwr_systick.c

#driver
$(NAME)_SOURCES += drivers/driver/src/lega_flash_alg.c \
            drivers/driver/src/lega_efuse.c \
            drivers/driver/src/lega_board.c \
            drivers/driver/src/lega_flash.c \
            drivers/driver/src/lega_gpio.c \
            drivers/driver/src/lega_uart.c \
            drivers/driver/src/lega_wdg.c \

$(NAME)_PREBUILT_LIBRARY := drivers/libs/$(HOST_ARCH)/libasr_wifi.a

