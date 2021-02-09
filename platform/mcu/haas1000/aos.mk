
NAME := mcu_haas1000

HOST_OPENOCD := haas1000
HOST_ARCH    := Cortex-M4

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := driver & sdk for platform/mcu haas1000

ifneq ($(AOS_2NDBOOT_SUPPORT),yes)

$(NAME)_COMPONENTS += arch_armv7m rhino network
$(NAME)_COMPONENTS += newlib_stub debug osal_posix

ifeq ($(HW_MODULE), 1)
$(NAME)_COMPONENTS += input
endif

$(NAME)_INCLUDES += ./drivers/platform/cmsis/inc
$(NAME)_INCLUDES += ./drivers/platform/hal
$(NAME)_INCLUDES += ./drivers/platform/hal/haas1000
$(NAME)_INCLUDES += ./drivers/rtos/rhino/cmsis
$(NAME)_INCLUDES += ./drivers/utils/boot_struct
$(NAME)_INCLUDES += ./drivers/utils/cqueue
$(NAME)_INCLUDES += ./drivers/utils/hexdump
$(NAME)_INCLUDES += ./drivers/utils/list
$(NAME)_INCLUDES += ./drivers/utils/crash_catcher
$(NAME)_INCLUDES += ./drivers/utils/crash_catcher/include
$(NAME)_INCLUDES += ./drivers/services/nv_section/log_section
$(NAME)_INCLUDES += ./drivers/utils/xyzmodem
$(NAME)_INCLUDES += ./drivers/utils/kfifo
$(NAME)_INCLUDES += ./drivers/services/multimedia/audio/process/resample/coef
$(NAME)_INCLUDES += ./drivers/services/multimedia/audio/process/resample/include
$(NAME)_INCLUDES += ./drivers/apps/alios/genie/include
$(NAME)_INCLUDES += ./drivers/apps/alios/genie/hal/include
$(NAME)_INCLUDES += ./drivers/services/wifi_app/alsa_adapter
$(NAME)_INCLUDES += ./drivers/apps/key
$(NAME)_INCLUDES += ./drivers/apps/main
$(NAME)_INCLUDES += ./drivers/apps/alios
$(NAME)_INCLUDES += ./drivers/apps/common
$(NAME)_INCLUDES += ./drivers/services/transq_msg
$(NAME)_INCLUDES += ./drivers/services/wifi_app
$(NAME)_INCLUDES += ./drivers/services/ai_voice/audio
$(NAME)_INCLUDES += ./drivers/platform/drivers/ana
$(NAME)_INCLUDES += ./drivers/platform/drivers/bt
$(NAME)_INCLUDES += ./drivers/services/wifi_app/sniffer
$(NAME)_INCLUDES += ./drivers/platform/drivers/ana/haas1000
$(NAME)_INCLUDES += ./drivers/net/net_os
$(NAME)_INCLUDES += ./drivers/net/ftp_client
$(NAME)_INCLUDES += ./drivers/utils
$(NAME)_INCLUDES += ./drivers/services/audioflinger
$(NAME)_INCLUDES += ./drivers/services/wifi_app/wifi_voice
$(NAME)_INCLUDES += ./drivers/services/wifi_app/audio_dec
$(NAME)_INCLUDES += ./drivers/services/a7_dsp
$(NAME)_INCLUDES += ./drivers/services/bt_app
$(NAME)_INCLUDES += ./drivers/services/sys_time
$(NAME)_INCLUDES += ../../../components/dm/ulog/include

GLOBAL_INCLUDES += aos
GLOBAL_INCLUDES += aos/arch
GLOBAL_INCLUDES += drivers/platform/hal
GLOBAL_INCLUDES += drivers/platform/hal/haas1000
GLOBAL_INCLUDES += drivers/platform/cmsis/inc
GLOBAL_INCLUDES += drivers/rtos/cmsis
GLOBAL_INCLUDES += drivers/net
GLOBAL_INCLUDES += drivers/services/nvrecord
GLOBAL_INCLUDES += drivers/apps/alios/genie/include
GLOBAL_INCLUDES += drivers/apps/alios/genie/hal/include
GLOBAL_INCLUDES += drivers/apps/alios/genie/hal/bluetooth/include
GLOBAL_INCLUDES += drivers/utils/crc32
GLOBAL_INCLUDES += drivers/utils/hwtimer_list
GLOBAL_INCLUDES += drivers/platform/drivers/bt
GLOBAL_INCLUDES += ../../../components/network/lwip/include
GLOBAL_INCLUDES += ../../../components/network/lwip/include/lwip
GLOBAL_INCLUDES += ../../../component/websocket
GLOBAL_INCLUDES += drivers/services/multimedia/audio/codec/libopusenc/include
GLOBAL_INCLUDES += drivers/services/multimedia/audio/codec/libopusenc/src
GLOBAL_INCLUDES += drivers/services/multimedia/audio/codec/opus/include
GLOBAL_INCLUDES += drivers/apps/cmd
GLOBAL_INCLUDES += drivers/apps/main
GLOBAL_INCLUDES += drivers/platform/drivers/usb/usb_host/inc

$(NAME)_SOURCES += hal/adc.c \
                   hal/flash.c \
                   hal/gpio.c \
                   hal/i2c.c \
                   hal/pwm.c \
                   hal/sd.c \
                   hal/timer.c \
                   hal/uart.c \
                   hal/wdg.c \
                   hal/spi.c \
                   hal/wifi_port.c

$(NAME)_SOURCES += 	aos/aos.c \
					aos/hook_impl.c \
					aos/soc_init.c \
					aos/ota_port.c

$(NAME)_SOURCES += 	aos/dsp/dsp.S

$(NAME)_SOURCES += drivers/utils/boot_struct/boot_struct.c \
				drivers/utils/build_info/build_info.c

ifeq ($(A7_DSP_ENABLE), 1)
$(NAME)_SOURCES += hal/audio.c
endif

HW_MODULE ?= 0
ifeq ($(HW_MODULE), 1)
GLOBAL_DEFINES += CFG_HW_ALI_MODULE
endif

GLOBAL_DEFINES += SWD_ENABLE_AS_DEFAULT CLI_CONFIG_MAX_COMMANDS=256 CUSTOM_CLI_ENABLED SUPPORT_24G_SPECIFIED_SCAN

BATTERY_VER ?= 0
ifeq ($(BATTERY_VER), 1)
GLOBAL_DEFINES += CFG_BATTERY_VER
endif

ifeq ($(CONFIG_WORK_WITH_ETH),y)
$(NAME)_PREBUILT_LIBRARY := drivers/libhaas1000_nolwip.a

else ifeq ($(CONFIG_WORK_WITH_LTE),y)
$(NAME)_PREBUILT_LIBRARY := drivers/libhaas1000_nolwip.a

else ifeq ($(CONFIG_WORK_WITH_WIFI),y)
$(NAME)_PREBUILT_LIBRARY := drivers/libhaas1000.a

else
$(NAME)_PREBUILT_LIBRARY := drivers/libhaas1000.a
endif

CPU_FLAGS := -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -DCORTEX_DEBUG_V8M
#-mno-unaligned-access

ifeq ($(CONFIG_UAI_USE_CMSIS_NN), y)
CPU_FLAGS += -D__FPU_PRESENT=1
CMSIS_DSP_LIB := 1
endif

COM_FLAGS := -g -O2 -Wall -Werror -w

COM_MACROS := -DCHIP=haas1000 -DKERNEL=RHINO -DCHIP_HAAS1000_ARCH_ENHANCE -DCHIP_HAAS1000 -DMIC_SAMPLERATE=48000 -DMIC_PERIOD_MS=48

ifeq ($(CONFIG_WORK_WITH_ETH),y)
MACROS := -DFLASH_SIZE=0x1000000 -DCRASH_DUMP_SECTION_SIZE=0 \
		-DAUD_SECTION_SIZE=0 -DUSERDATA_SECTION_SIZE=0x1000 -DFACTORY_SECTION_SIZE=0x1000 \
		-DNV_REC_DEV_VER=2 -DREVISION_INFO=x -DCHIP_HAS_UART=3 -DRESAMPLE_ANY_SAMPLE_RATE -DCHIP_HAS_TRANSQ -DRTOS=1 \
		-DCQ_FUNC_ATTR= -DDEBUG=1 -DREL_TRACE_ENABLE -DLWIP_ETHERNETIF=0 -DLWIP_SUPPORT=0 -DPERSIST_DATA_SECTION_SIZE=0x1000
else
MACROS := -DFLASH_SIZE=0x1000000 -DCRASH_DUMP_SECTION_SIZE=0 \
		-DAUD_SECTION_SIZE=0 -DUSERDATA_SECTION_SIZE=0x1000 -DFACTORY_SECTION_SIZE=0x1000 \
		-DNV_REC_DEV_VER=2 -DREVISION_INFO=x -DCHIP_HAS_UART=3 -DRESAMPLE_ANY_SAMPLE_RATE -DCHIP_HAS_TRANSQ -DRTOS=1 \
		-DCQ_FUNC_ATTR= -DDEBUG=1 -DREL_TRACE_ENABLE -DLWIP_ETHERNETIF=1 -DLWIP_SUPPORT=0 -DPERSIST_DATA_SECTION_SIZE=0x1000
endif

ifeq ($(SECURE_OTA_BOOT), yes)
MACROS += -DSECURE_BOOT -DHAAS_OTA_ENABLED=1 -DHAAS_OTA_BIN_VER=\"HAAS_AOS_3.1\"
endif

CWARN_FLAGS := \
		-Wno-int-conversion -Wno-unused-variable -Wno-unused-parameter -Wno-type-limits \
		-Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable -Wno-unused-value \
		-Wno-strict-aliasing -Wno-pointer-sign -Wno-discarded-qualifiers -Wno-overflow \
		-Wno-comment -Wno-maybe-uninitialized -Wno-float-conversion -Wno-incompatible-pointer-types \
		-Wno-implicit-function-declaration

$(NAME)_CFLAGS += $(CPU_FLAGS) $(COM_FLAGS) $(CWARN_FLAGS) $(COM_MACROS) $(MACROS) -std=gnu11 -fno-common -fmessage-length=0 -fno-exceptions -ffunction-sections -fdata-sections -fomit-frame-pointer

$(NAME)_CXXFLAGS += $(CPU_FLAGS) $(COM_MACROS) $(MACROS)

GLOBAL_CFLAGS += $(CPU_FLAGS) $(COM_FLAGS) $(CWARN_FLAGS) $(COM_MACROS) -DCHIP_HAS_TRANSQ -DCHIP_HAS_UART=3 -DRTOS=1 -DDISABLE_CLI_MULTI_CONSOLE=0 \
		-std=gnu11 -fno-common -fmessage-length=0 -fno-exceptions -ffunction-sections -fdata-sections -fomit-frame-pointer

GLOBAL_CXXFLAGS += $(CPU_FLAGS) $(COM_FLAGS) $(COM_MACROS) -D_GLIBCXX_HAS_GTHREADS \
		-std=gnu++11 -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions -fpermissive

GLOBAL_ASMFLAGS += $(CPU_FLAGS) $(COM_FLAGS) $(COM_MACROS) -c -x assembler-with-cpp

GLOBAL_LDFLAGS += $(CPU_FLAGS) -u _printf_float -lstdc++ -lsupc++ -lm -lc -lgcc -lnosys -nostartfiles --specs=nano.specs
GLOBAL_LDFLAGS += -Wl,--wrap=snprintf -Wl,--wrap=printf

GLOBAL_CFLAGS += -DSNTP_PERIODIC_SYNC_INTERVAL_MS=3600000

else

GLOBAL_INCLUDES +=  hal/2ndboot
GLOBAL_INCLUDES +=  drivers/platform/hal \
					drivers/platform/cmsis/inc

$(NAME)_SOURCES +=  hal/2ndboot/wdg.c \
					hal/2ndboot/sys.c \
					hal/2ndboot/uart.c \
					hal/2ndboot/flash.c \
					hal/2ndboot/startup.c \
					hal/2ndboot/boot_ringbuffer.c

CPU_FLAGS := -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
#-mno-unaligned-access

COM_FLAGS := -g -O2 -Wall -Werror -w

COM_MACROS := -DCHIP=haas1000 -DKERNEL=RHINO -DCHIP_HAAS1000_ARCH_ENHANCE -DCHIP_HAAS1000 -DMIC_SAMPLERATE=48000 -DMIC_PERIOD_MS=48

MACROS := -DFLASH_SIZE=0x1000000 -DCRASH_DUMP_SECTION_SIZE=0 \
		-DAUD_SECTION_SIZE=0 -DUSERDATA_SECTION_SIZE=0x1000 -DFACTORY_SECTION_SIZE=0x1000 \
		-DNV_REC_DEV_VER=2 -DREVISION_INFO=x -DCHIP_HAS_UART=3 -DRESAMPLE_ANY_SAMPLE_RATE -DCHIP_HAS_TRANSQ -DRTOS=1 \
		-DCQ_FUNC_ATTR= -DDEBUG=1 -DREL_TRACE_ENABLE -DLWIP_ETHERNETIF=1 -DLWIP_SUPPORT=0 -DPERSIST_DATA_SECTION_SIZE=0x1000

ifeq ($(SECURE_OTA_BOOT), yes)
MACROS += -DSECURE_BOOT -DHAAS_OTA_ENABLED=1 -DHAAS_OTA_BIN_VER=\"HAAS_AOS_3.1\"
endif

CWARN_FLAGS := \
		-Wno-int-conversion -Wno-unused-variable -Wno-unused-parameter -Wno-type-limits \
		-Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable -Wno-unused-value \
		-Wno-strict-aliasing -Wno-pointer-sign -Wno-discarded-qualifiers -Wno-overflow \
		-Wno-comment -Wno-maybe-uninitialized -Wno-float-conversion -Wno-incompatible-pointer-types \
		-Wno-implicit-function-declaration

$(NAME)_CFLAGS += $(CPU_FLAGS) $(COM_FLAGS) $(CWARN_FLAGS) $(COM_MACROS) $(MACROS) -std=gnu11 -fno-common -fmessage-length=0 -fno-exceptions -ffunction-sections -fdata-sections -fomit-frame-pointer

$(NAME)_CXXFLAGS += $(CPU_FLAGS) $(COM_MACROS) $(MACROS)

GLOBAL_CFLAGS += $(CPU_FLAGS) $(COM_FLAGS) $(CWARN_FLAGS) $(COM_MACROS) -DCHIP_HAS_TRANSQ -DCHIP_HAS_UART=3 -DRTOS=1 -DDISABLE_CLI_MULTI_CONSOLE=0 \
		-std=gnu11 -fno-common -fmessage-length=0 -fno-exceptions -ffunction-sections -fdata-sections -fomit-frame-pointer

GLOBAL_CXXFLAGS += $(CPU_FLAGS) $(COM_FLAGS) $(COM_MACROS) -D_GLIBCXX_HAS_GTHREADS \
		-std=gnu++11 -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions -fpermissive

GLOBAL_ASMFLAGS += $(CPU_FLAGS) $(COM_FLAGS) $(COM_MACROS) -c -x assembler-with-cpp

GLOBAL_LDFLAGS += $(CPU_FLAGS) -u _printf_float -lstdc++ -lsupc++ -lm -lc -lgcc -lnosys -nostartfiles --specs=nano.specs

GLOBAL_LDFLAGS += -Wl,--wrap=snprintf -Wl,--wrap=printf

GLOBAL_CFLAGS += -DSNTP_PERIODIC_SYNC_INTERVAL_MS=3600000

endif
