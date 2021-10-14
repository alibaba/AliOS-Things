CHIP        ?= haas1000

DEBUG       ?= 1

FPGA        ?= 0

MBED        ?= 0

RTOS        ?= 1

LIBC_ROM    ?= 0

CPU         ?= m33

WATCHER_DOG ?= 1

export SUPPORT_IR ?= 0
ifeq ($(SUPPORT_IR),1)
KBUILD_CPPFLAGS += -DSUPPORT_IR
endif

ifeq ($(ALIOS_THINGS_RELEASE),1)
export NO_DEBUG_RELEASE ?= 0
else
export NO_DEBUG_RELEASE ?= 0
endif
ifeq ($(NO_DEBUG_RELEASE),1)
KBUILD_CPPFLAGS += -DNO_DEBUG_RELEASE
endif

KBUILD_CPPFLAGS += -DMIC_SAMPLERATE=48000 -DMIC_PERIOD_MS=48

export ARCH_ENHANCE ?= 1

export FLASH_UNIQUE_ID ?= 1

export FA_RX_GAIN_CTRL ?= 0

ifeq ($(CPU), m4)
ifeq ($(ARCH_ENHANCE), 1)
export UNALIGNED_ACCESS ?= 0
KBUILD_CPPFLAGS += -DCHIP_HAAS1000_ARCH_ENHANCE
else
export UNALIGNED_ACCESS ?= 1
KBUILD_CPPFLAGS += -DFORCE_DCACHE_DISABLE
endif
endif

#KERNEL      ?= RTX5
KERNEL      ?= RHINO

ifeq ($(KERNEL), RHINO)
export NO_STARTFILES ?= 1
endif

export USER_SECURE_BOOT	?= 0
# enable:1
# disable:0

export RF_USE_SP3T ?= 0

export NO_OVERLAY ?= 1
export TRACE_CRLF ?= 1
export ALI_BT_STACK ?= 1
ifeq ($(ALI_BT_STACK),1)
export BT_CLOSE ?= 1
else
export BT_CLOSE ?= 0
endif

export POWERON_SCAN ?= 0
ifeq ($(POWERON_SCAN),1)
KBUILD_CPPFLAGS += -D__POWERON_BT_SCAN__
endif

export ALIOS_SUPPORT ?= 1
ifeq ($(ALIOS_SUPPORT),1)
ifeq ($(GEN_LIB)_$(SDK),_)
KBUILD_CFLAGS += -DDEBUG_BY_UART
else
KBUILD_CFLAGS += -DALIOS_WIFI_SERVICE -DQUICK_SCAN_ENABLED -DDEBUG_BY_UART
endif
export ALIOS_APP_SUPPORT ?= 1
ifeq ($(ALIOS_APP_SUPPORT),1)
KBUILD_CPPFLAGS += -DALIOS_APP_SUPPORT
endif
endif

export APPLICATION_HEAP_SIZE ?= 0x70000

export SHORT_GI ?= 0
ifeq ($(SHORT_GI),1)
KBUILD_CPPFLAGS += -D__SHORT_GI__
endif

export GEN_SDK_LIB ?= 0
export GEN_SDK_LIB_NET ?= 1
ifeq ($(GEN_LIB)_$(SDK),_)
export NET_LWIP_ALIOS_LIB ?= 0
else
export NET_LWIP_ALIOS_LIB ?= 1
endif
export SERVICE_ONE_LIB ?= 0
export A7_INTO_LIB ?= 1
export GEN_SDK_LIB_WW ?= 1

WATCHER_DOG ?= 0

WIFI_RECOVERY ?= 1
ifeq ($(WIFI_RECOVERY),1)
KBUILD_CPPFLAGS += -D__WIFI_RECOVERY_SUPPORT__
endif

export A7_DSP_SPEED ?= 1000
export A7_UART ?= 4
#default transq, support 1(uart0), 3(uart2)
ifeq ($(A7_UART),1)
DEBUG_PORT	?= 2
DSP_USE_AUDMA ?= 1
else ifeq ($(A7_UART),3)
DEBUG_PORT	?= 1
DSP_USE_AUDMA ?= 1
else
DEBUG_PORT	?= 1
DSP_USE_AUDMA ?= 1
endif
KBUILD_CPPFLAGS += -DA7_UART=$(A7_UART)

export A7_SRAM ?= 2
# a7 sram mode:
# 0: none, use m33 share sync flags for transq, stack/ttb use PSRAM
# 1: solo, use m33 share sync flags for transq, stack/ttb use SRAM, solo 512K, for alg test
# 2: default, solo share sync flags for transq, stack/ttb use SRAM
ifeq ($(A7_SRAM),0)
export DSP_RAM_SIZE ?= 0
export DSP_RAM_SHARE_SIZE ?= 0x15000
KBUILD_CPPFLAGS += -DDSP_RAM_SHARE_SIZE=$(DSP_RAM_SHARE_SIZE)
else ifeq ($(A7_SRAM),1)
export DSP_RAM_SIZE ?= 0x80000
KBUILD_CPPFLAGS += -DDSP_SRAM_SOLO
else
export DSP_RAM_SIZE ?= 0x80000
endif

export A7_PROJECT ?= dummy

# 0: usb
# 1: uart0
# 2: uart1
export TRACE_BAUD_RATE ?= 1500000

export AQE_KWS := 1
AQE_KWS_NAME := HAAS

FLASH_CHIP	?= ALL
# GD25Q80C
# GD25Q32C
# ALL

export VCODEC_VOLT ?= 1.7V

export ANA_26M_X4_ENABLE ?= 1

export ANA_26M_X6_ENABLE ?= 0

XTAL_26M  ?=1

export PMU_IRQ_UNIFIED ?= 1

export MCU_HIGH_PERFORMANCE_MODE ?= 320

export FLASH_SUSPEND := 1
export FLASH_PAGE_MPM_ENABLE ?= 1
export FLASH_PROTECTION ?= 1
# lower 32KB
KBUILD_CPPFLAGS += -DFLASH_BLOCK_PROTECTION_128M_BIT=0x78

export CP_ENABLE ?= 1
ifeq ($(CP_ENABLE),1)
export MP3_DECODE_IN_CP ?= 0
export AAC_DECODE_IN_CP ?= 0
export OPUS_DECODE_IN_CP ?= 1
export OPUS_ENCODE_IN_CP ?= 1
export EQ_DRC_IN_CP ?= 1
export ALSA_RESAMPLE_IN_CP ?= 1
export VOIP_ALG_IN_CP ?= 0
ifeq ($(VOIP_ALG_IN_CP),1)
export VOIP_ALG_ALI_TEST_IN_CP ?= 0
endif
else
export MP3_DECODE_IN_CP ?= 0
export AAC_DECODE_IN_CP ?= 0
export OPUS_DECODE_IN_CP ?= 0
export OPUS_ENCODE_IN_CP ?= 0
export EQ_DRC_IN_CP ?= 0
export ALSA_RESAMPLE_IN_CP ?= 0
export VOIP_ALG_IN_CP ?= 0
ifeq ($(VOIP_ALG_IN_CP),1)
export VOIP_ALG_ALI_TEST_IN_CP ?= 0
endif
endif

export PLATFORM_THERMAL_REBOOT := 1
KBUILD_CPPFLAGS +=-D_MAX_TEMP_=125
ifeq ($(RF_USE_SP3T),1)
ifeq ($(PLATFORM_THERMAL_REBOOT),1)
KBUILD_CPPFLAGS += -DPLATFORM_THERMAL_REBOOT -D_PRINT_TEMP_INTERVAL_=10000
endif
endif
ifeq ($(MP3_DECODE_IN_CP),1)
KBUILD_CPPFLAGS += -DMP3_DECODE_IN_CP
endif
ifeq ($(AAC_DECODE_IN_CP),1)
KBUILD_CPPFLAGS += -DAAC_DECODE_IN_CP
endif
ifeq ($(OPUS_DECODE_IN_CP),1)
KBUILD_CPPFLAGS += -DOPUS_DECODE_IN_CP
endif
ifeq ($(OPUS_ENCODE_IN_CP),1)
KBUILD_CPPFLAGS += -DOPUS_ENCODE_IN_CP
endif
ifeq ($(EQ_DRC_IN_CP),1)
KBUILD_CPPFLAGS += -DEQ_DRC_IN_CP
endif
ifeq ($(ALSA_RESAMPLE_IN_CP),1)
KBUILD_CPPFLAGS += -DALSA_RESAMPLE_IN_CP
endif
ifeq ($(VOIP_ALG_IN_CP),1)
KBUILD_CPPFLAGS += -DVOIP_ALG_IN_CP
ifeq ($(VOIP_ALG_ALI_TEST_IN_CP), 1)
KBUILD_CPPFLAGS += -DVOIP_ALG_ALI_TEST_IN_CP
endif
endif

export ALSA_AUDIO_PROCESS_MODE_CHECK ?=1
ifeq ($(ALSA_AUDIO_PROCESS_MODE_CHECK), 1)
KBUILD_CPPFLAGS += -DALSA_AUDIO_PROCESS_MODE_CHECK
endif

export AUDIO_PROCESS_CFG_DIFF_FOR_ODM ?= 1
ifeq ($(AUDIO_PROCESS_CFG_DIFF_FOR_ODM),1)
KBUILD_CFLAGS += -DAUDIO_PROCESS_CFG_DIFF_FOR_ODM
endif

export AUDIO_PROCESS_SUPPORT_BYPASS_EQ ?= 1
ifeq ($(AUDIO_PROCESS_SUPPORT_BYPASS_EQ),1)
KBUILD_CFLAGS += -DAUDIO_PROCESS_SUPPORT_BYPASS_EQ
endif

export SPEECH_LIB := 1

AUDIO_OUTPUT_MONO ?= 1

AUDIO_OUTPUT_DIFF ?= 0

HW_FIR_EQ_PROCESS ?= 0

SW_IIR_EQ_PROCESS ?= 1

HW_DAC_IIR_EQ_PROCESS ?= 0

HW_IIR_EQ_PROCESS ?= 0

HW_DC_FILTER_WITH_IIR ?= 0

AUDIO_DRC ?= 1

AUDIO_DRC2 ?= 1

PC_CMD_UART ?= 3

AUDIO_SECTION_ENABLE ?= 0

AUDIO_RESAMPLE ?= 1

RESAMPLE_ANY_SAMPLE_RATE ?= 1

export RESAMPLE_ANY_SAMPLE_RATE_ACCURATE ?= 1

OSC_26M_X4_AUD2BB ?= 1

AUDIO_OUTPUT_INVERT_ALL_CHANNEL ?=1

AUDIO_OUTPUT_VOLUME_DEFAULT ?= 10
# range:1~16

AUDIO_OUTPUT_SW_GAIN ?= 0

AUDIO_INPUT_CAPLESSMODE ?= 0

AUDIO_INPUT_LARGEGAIN ?= 0

AUDIO_CODEC_ASYNC_CLOSE ?= 0

AUDIO_SCO_BTPCM_CHANNEL ?= 1

export ADC_DC_FILTER ?= 1

SPEECH_TX_DC_FILTER ?= 1

SPEECH_TX_AEC2FLOAT ?= 1

SPEECH_TX_NS2FLOAT ?= 0

SPEECH_TX_2MIC_NS2 ?= 0

SPEECH_TX_COMPEXP ?= 1

SPEECH_TX_EQ ?= 0

SPEECH_TX_POST_GAIN ?= 0

SPEECH_RX_NS2FLOAT ?= 0

SPEECH_RX_EQ ?= 0

SPEECH_RX_POST_GAIN ?= 0

HSP_ENABLE ?= 0

HFP_1_6_ENABLE ?= 1

ifeq ($(HFP_1_6_ENABLE),1)
    MSBC_16K_SAMPLE_RATE ?= 1
else
    MSBC_16K_SAMPLE_RATE := 0
endif

SBC_FUNC_IN_ROM ?= 0

APP_LINEIN_A2DP_SOURCE ?= 0

APP_I2S_A2DP_SOURCE ?= 0

VOICE_PROMPT ?= 1

#### Google related feature ####
# BISTO is a GVA service on Bluetooth audio device
# BISTO is an isolated service relative to GFPS
export BISTO_ENABLE ?= 0

# GSOUND_HOTWORD is a hotword library running on Bluetooth audio device
# GSOUND_HOTWORD is a subset of BISTO
export GSOUND_HOTWORD_ENABLE ?= 0

# GFPS is google fastpair service
# GFPS is an isolated service relative to BISTO
export GFPS_ENABLE ?= 0
#### Google related feature ####

export AMA_VOICE ?= 0

BLE ?= 1

export BES_BLE_MESH_ENABLE ?= 0

BT_ONE_BRING_TWO ?= 1

DSD_SUPPORT ?= 0

A2DP_LHDC_ON ?= 0

A2DP_EQ_24BIT ?= 1

A2DP_AAC_ON ?= 1

export TX_RX_PCM_MASK ?= 0

A2DP_SCALABLE_ON ?= 0

FACTORY_MODE ?= 1

ENGINEER_MODE ?= 1

ULTRA_LOW_POWER	?= 0

DAC_CLASSG_ENABLE ?= 0

NO_SLEEP ?= 0

ENHANCED_STACK ?= 1

CALIB_SLOW_TIMER ?= 1

export JTAG_ENABLE ?= 1

export A7_DSP_ENABLE ?= 0
export DSP_IMAGE_COMP ?= 1
ifeq ($(A7_DSP_ENABLE),1)
export DSP_AUDIO_TASK ?= 1
else
export DSP_AUDIO_TASK ?= 0
endif

export SPEECH_CODEC ?= 1
export WIFI_FW_RAMRUN ?= 0

ifeq ($(WIFI_FW_RAMRUN),1)
KBUILD_CPPFLAGS += -D__WIFI_FW_RAMRUN__
endif

export FILT_ADC_CAP_CALI ?= 1
ifeq ($(FILT_ADC_CAP_CALI),1)
KBUILD_CPPFLAGS += -D__FILT_ADC_CAP_CALI__
endif

export HIGH_THRUPUT ?= 0
ifeq ($(HIGH_THRUPUT),1)
KBUILD_CPPFLAGS += -D__HIGH_THRUPUT__
#KBUILD_CPPFLAGS += -DMCU_HIGH_PERFORMANCE_MODE_320
endif
KBUILD_CFLAGS += -DPOWER_SAVE_TEST
export POWER_MODE   ?= DIG_DCDC

export PSRAM_INIT_CHECK ?= 1

export A7_DSP_HEARTBEAT_CHECK ?= 1

# total ram size is 0x280000(2.5MB)
export TOTAL_RAM_SIZE := 0x280000
#export LARGE_RAM ?= 1
# ram size for mcu
export MCU_SRAM_EXT ?= 1
export PSRAMUHS_ENABLE ?= 0
ifeq ($(PSRAMUHS_ENABLE),1)

ifneq ($(DSP_RAM_SHARE_SIZE),)
export RAM_SIZE ?= 0x280000
export DSP_RAM_SIZE ?= 0
else
export RAM_SIZE ?= 0x200000
export DSP_RAM_SIZE ?= 0x80000
endif

ifeq ($(PSRAM_ENABLE), 1)
export PSRAMUHS_SIZE ?= 0x800000
export DSP_PSRAMUHS_SIZE ?= 0x800000
export MCU_PSRAMUHS_SIZE ?= 0x000000
export PSRAMUHS_SPEED ?= 1000
else
export PSRAMUHS_SIZE ?= 0x1000000
export DSP_PSRAMUHS_SIZE ?= 0x800000
export MCU_PSRAMUHS_SIZE ?= 0x800000
export PSRAMUHS_DUAL_8BIT ?= 1
export PSRAMUHS_SPEED ?= 1000
endif
else
ifeq ($(A7_DSP_ENABLE),1)
export RAM_SIZE ?= 0x200000
export DSP_RAM_SIZE ?= 0x80000
else
export LARGE_RAM ?= 1
export RAM_SIZE ?= $(TOTAL_RAM_SIZE)
export PSRAMUHS_SIZE ?= 0
endif
endif

export PSRAM_ENABLE ?= 0
ifeq ($(PSRAM_ENABLE),1)
export PSRAM_SPEED ?= 200
export PSRAM_SIZE ?= 0x800000
else
export PSRAM_SIZE ?= 0
endif

ifeq ($(CP_ENABLE), 1)
# total cp sram use 512KB = 0x80000
export RAMCP_SIZE ?= 0x00057000
export RAMCPX_SIZE ?= 0x00029000
export RAM_SIZE := $(RAM_SIZE)-$(RAMCP_SIZE)-$(RAMCPX_SIZE)
export RAMCP_BASE ?= 0x20000000+$(RAM_SIZE)
export RAMCPX_BASE ?= 0x00200000+$(RAM_SIZE)+$(RAMCP_SIZE)
endif

USE_THIRDPARTY ?= 0
export FLASH_SIZE ?= 0x1000000

ifeq ($(DSD_SUPPORT),1)
export BTUSB_AUDIO_MODE     ?= 1
export AUDIO_INPUT_MONO     ?= 1
export USB_ISO              ?= 1
export USB_AUDIO_DYN_CFG    ?= 1
export DELAY_STREAM_OPEN    ?= 0
export KEEP_SAME_LATENCY    ?= 1
export HW_FIR_DSD_PROCESS   ?= 1
ifeq ($(HW_FIR_DSD_PROCESS),1)
ifeq ($(CHIP),best2300)
export HW_FIR_DSD_BUF_MID_ADDR  ?= 0x200A0000
export DATA_BUF_START           ?= 0x20040000
endif
endif
export USB_AUDIO_UAC2 ?= 1
export USB_HIGH_SPEED ?= 0
KBUILD_CPPFLAGS += \
    -DHW_FIR_DSD_BUF_MID_ADDR=$(HW_FIR_DSD_BUF_MID_ADDR) \
    -DDATA_BUF_START=$(DATA_BUF_START)
endif

ifeq ($(CURRENT_TEST),1)
export VCODEC_VOLT ?= 1.6V
endif

export BT_XTAL_SYNC ?= 1

export BTADDR_FOR_DEBUG ?= 1

AUTO_TEST ?= 0

#export DUMP_NORMAL_LOG ?= 1

export DSP_LIB ?= 1

SUPPORT_BATTERY_REPORT ?= 1

SUPPORT_HF_INDICATORS ?= 0

SUPPORT_SIRI ?= 1

export USE_BT_ADAPTER ?= 0

export PERSIST_DATA_SECTION_SIZE ?= 0x1000

export LITTLEFS_ENABLE ?= 0

export FILE_SYSTEM_SECTION_SIZE ?= 0x4B8000

export LLD_PRINT_ENABLE ?= 1

#export OTA_CODE_OFFSET ?= 0x38000

export FLASH_SECURITY_REGISTER ?= 1

#lwip support
export NET_LWIP_SUPPORT ?= 1
ifeq ($(NET_LWIP_SUPPORT),1)
KBUILD_CFLAGS += -DLWIP_ETHERNETIF=1
ifeq ($(NET_LWIP_ALIOS_LIB),1)
KBUILD_CFLAGS += -DLWIP_SUPPORT=0
else
KBUILD_CFLAGS += -DLWIP_SUPPORT=1
endif
export NET_LIBCURL_SUPPORT ?= 0
export NET_LIBRWS_SUPPORT ?= 0

ifeq ($(ALIOS_THINGS_RELEASE),1)
export NET_MBEDTLS_SUPPORT ?= 0
else
export NET_MBEDTLS_SUPPORT ?= 1
endif

ifeq ($(NET_MBEDTLS_SUPPORT),1)
export NET_MBEDTLS_AES_ALT ?= 1
ifeq ($(NET_MBEDTLS_AES_ALT),1)
KBUILD_CFLAGS += -DEN_MBEDTLS_AES_ALT
endif
export ENTROPY_HARDWARE_ALT ?= 1
ifeq ($(ENTROPY_HARDWARE_ALT),1)
KBUILD_CFLAGS += -DEN_HARDWARE_ENTROPY_ALT
endif
endif
export NET_MQTT_SUPPORT ?= 0
export NET_NGHTTP2_SUPPORT ?= 0
export NET_COAP_SUPPORT ?= 0
export NET_CJSON_SUPPORT ?= 0
export NET_FTP_CLIENT_SUPPORT ?= 1
endif

ifeq ($(NET_FTP_CLIENT_SUPPORT),1)
export LITTLEFS_ENABLE ?= 1
KBUILD_CPPFLAGS +=  -Inet/ftp_client
endif

export WIFI_APP_SUPPORT ?= 1
export WIFI_IPERF_SUPPORT ?= 1

ifeq ($(RF_USE_SP3T),1)
export USE_EXT_SPDT ?= 0
else
export USE_EXT_SPDT ?=1
endif

ifeq ($(WIFI_APP_SUPPORT),1)
KBUILD_CFLAGS += -D__WIFI_APP_SUPPORT__
endif

ifeq ($(WIFI_IPERF_SUPPORT),1)
KBUILD_CFLAGS += -DWIFI_IPERF_SUPPORT
CFLAGS_IMAGE += -u _printf_float -u _scanf_float
endif

export AP_MODE_SUPPORT ?= 1
ifeq ($(AP_MODE_SUPPORT),1)
KBUILD_CFLAGS += -D__AP_MODE__
endif

ifneq ($(BT_CLOSE)_$(ALI_BT_STACK),1_0)
KBUILD_CFLAGS += -D__WIFI_BT_COEX__
KBUILD_CPPFLAGS += -D__WIFI_BT_COEX__
endif

ifeq ($(A7_DSP_ENABLE),1)
export DSP_ENABLE := 1
KBUILD_CFLAGS += -D__A7_DSP_ENABLE__ -D__A7_DSP_TRANSQ_AF__
endif

export UTILS_INTERSYSHCI ?= 1
export UTILS_BES_KV ?= 1

export ALSA_RESAMPLE_USER_HOOK_EN ?= 1

export DIGMIC_EN ?= 0
ifeq ($(DIGMIC_EN),1)
KBUILD_CFLAGS += -D__DIGMIC_USED__
endif

init-y      :=
ifneq ($(SERVICE_ONE_LIB),1)
core-y      := platform/ services/ thirdparty/ net/ apps/ utils/cqueue/ utils/list/ utils/hexdump/ utils/kfifo/ services/multimedia/ utils/secure_boot/mincrypt/
else
core-y      := platform/ services/ thirdparty/ net/ apps/ utils/cqueue/ utils/list/ utils/hexdump/ utils/kfifo/ utils/secure_boot/mincrypt/
endif

ifeq ($(LLD_PRINT_ENABLE),1)
core-y		+= utils/lld_print/
endif

ifeq ($(A7_DSP_ENABLE),1)
core-y		+= services/a7_dsp/prj/$(A7_PROJECT)/
endif

KBUILD_CPPFLAGS +=  -Iplatform/cmsis/inc \
                    -Iservices/audioflinger \
                    -Iplatform/hal \
                    -Iservices/fs/ \
                    -Iservices/fs/sd \
                    -Iservices/fs/fat \
                    -Iservices/fs/fat/ChaN
#KBUILD_CPPFLAGS += -Iplatform/cmsis/inc -Iplatform/hal -Iplatform/drivers/usb/usb_dev/inc -Iplatform/drivers/wifi
KBUILD_CPPFLAGS += -Iplatform/cmsis/inc -Iservices/audioflinger -Iplatform/hal -Iservices/fs/ -Iservices/fs/sd -Iservices/fs/fat  -Iservices/fs/fat/ChaN -Inet
KBUILD_CPPFLAGS += -Imbed/api -Imbed/common -Imbed/targets/hal/TARGET_BEST/TARGET_BEST100X/TARGET_MBED_BEST1000 -Iplatform/cmsis/inc \
				   -Imbed/targets/hal/TARGET_BEST/TARGET_BEST100X -Imbed/hal -Iplatform/drivers/wifi

KBUILD_CPPFLAGS += \
    -D__APP_KEY_FN_STYLE_A__ \
    -DINTERSYS_DEBUG
#    -D__EARPHONE_STAY_BOTH_SCAN__

#    -D__PC_CMD_UART__

#KBUILD_CPPFLAGS += -D__3M_PACK__

#-D_AUTO_SWITCH_POWER_MODE__
#-D__APP_KEY_FN_STYLE_A__
#-D__APP_KEY_FN_STYLE_B__
#-D__EARPHONE_STAY_BOTH_SCAN__
#-D__POWERKEY_CTRL_ONOFF_ONLY__
#-DAUDIO_LINEIN

ifeq ($(CURRENT_TEST),1)
INTSRAM_RUN ?= 1
endif
ifeq ($(INTSRAM_RUN),1)
LDS_FILE := best1000_intsram.lds
else
LDS_FILE := best1000.lds
endif

ifeq ($(BISTO_ENABLE),1)
LDS_FILE := best2000_bisto.lds
else
ifeq ($(ROM_BUILD),1)
LDS_FILE	:= rom.lds
ROM_SIZE	:= 0x80000
else
LDS_FILE	:= best1000.lds
endif
endif

ifeq ($(DSP_ENABLE), 1)
LDS_FILE	:= best1000.lds
endif

ifeq ($(HIGH_THRUPUT), 1)
LDS_FILE        :=best1000_sram.lds
endif

ifeq ($(PSRAM_ENABLE),1)
export UNALIGNED_ACCESS := 1
KBUILD_CPPFLAGS += -DCHIP_HAS_PSRAM -DPSRAM_ENABLE
ifeq ($(MCU_SRAM_EXT),1)
KBUILD_CPPFLAGS += -D__SRAM_EXT_PSRAM__
KBUILD_CPPFLAGS += -DPSRAM_SIZE=$(PSRAM_SIZE)
LDS_FILE        :=$(CHIP).lds
endif
endif

ifeq ($(PSRAMUHS_ENABLE),1)
KBUILD_CPPFLAGS += -DCHIP_HAS_PSRAMUHS -DPSRAMUHS_ENABLE
ifeq ($(MCU_SRAM_EXT),1)
KBUILD_CPPFLAGS += -D__SRAM_EXT_PSRAMUHS__
KBUILD_CPPFLAGS += -DMCU_PSRAMUHS_SIZE=$(MCU_PSRAMUHS_SIZE)
LDS_FILE        :=$(CHIP).lds
endif
KBUILD_CPPFLAGS += -DDSP_PSRAMUHS_SIZE=$(DSP_PSRAMUHS_SIZE)
endif

ifeq ($(DSP_IMAGE_COMP), 1)
KBUILD_CPPFLAGS += -DDSP_IMAGE_COMP
endif

ifeq ($(ALIOS_THINGS_RELEASE),1)
#KBUILD_CPPFLAGS += -D__GOT_INFO_AVAILABLE__
KBUILD_CPPFLAGS += -DALIOS_THINGS_RELEASE
#LINK_CFLAGS += -nostdlib -nostartfiles
LDS_FILE        :=$(CHIP)_alios.lds
endif

ifeq ($(NO_STARTFILES),1)
KBUILD_CPPFLAGS += -D__NO_STARTFILES__
endif

ifeq ($(INTERCONNECTION), 1)
OTA_BASIC ?= 1
endif

export ALLOW_WARNING ?= 1

KBUILD_CPPFLAGS +=

KBUILD_CFLAGS +=

LIB_LDFLAGS += -lstdc++ -lsupc++

CORE_DUMP ?= 1

#CFLAGS_IMAGE += -u _printf_float -u _scanf_float

#LDFLAGS_IMAGE += --wrap main
ifeq ($(BT_CLOSE),1)
KBUILD_CPPFLAGS += -D__BT_CLOSE__
endif

ifeq ($(WIFI_APP_TEST),1)
KBUILD_CPPFLAGS += -D__WIFI_APP_TEST__
endif

ifeq ($(RTOS),1)
KBUILD_CPPFLAGS += -DRTOS
endif

ifeq ($(XTAL_26M),1)
KBUILD_CPPFLAGS += -D__USE_26M_XTAL__
endif

export DUAL_BAND ?=1
ifeq ($(DUAL_BAND),1)
KBUILD_CPPFLAGS += -D__DUAL_BAND__
KBUILD_CFLAGS += -D__DUAL_BAND__
endif

export SPEECH_CODEC_FRAME_MS    ?= 16

export SPEECH_SCO_FRAME_MS      ?= 15

export SPEECH_CODEC_CAPTURE_CHANNEL_NUM ?= 1
export PWM_ENABLE ?= 1

export OPUS_CODEC ?= 1
ifeq ($(OPUS_CODEC),1)
KBUILD_CFLAGS += -DOPUS_CODEC_SUPPORT
endif

KBUILD_CPPFLAGS += -DSPEECH_CODEC_CAPTURE_CHANNEL_NUM=$(SPEECH_CODEC_CAPTURE_CHANNEL_NUM)
KBUILD_CPPFLAGS += -DSPEECH_CODEC_FRAME_MS=$(SPEECH_CODEC_FRAME_MS)
KBUILD_CPPFLAGS += -DSPEECH_SCO_FRAME_MS=$(SPEECH_SCO_FRAME_MS)
KBUILD_CPPFLAGS += -DUSE_SD_FILESYS

ifeq ($(XIAOQIANG),1)
KBUILD_CPPFLAGS += -DCFG_HW_ALI_XIAOQIANG
endif

export CONTRA_EN ?= 0
ifeq ($(CONTRA_EN),1)
KBUILD_CPPFLAGS += -DCFG_HW_ALI_CONTRA
endif

export MARIE_EN ?= 0
ifeq ($(MARIE_EN),1)
KBUILD_CPPFLAGS += -DCFG_HW_ALI_SUPER_MARIE
endif

export HW_MODULE ?= 0
ifeq ($(HW_MODULE),1)
PWM_ENABLE := 0
KBUILD_CPPFLAGS += -DCFG_HW_ALI_MODULE
endif

export BATTERY_VER ?= 0
ifeq ($(BATTERY_VER), 1)
export NO_PWRKEY := 0
KBUILD_CPPFLAGS += -DCFG_BATTERY_VER
endif

ifeq ($(PWM_ENABLE),1)
KBUILD_CPPFLAGS += -DCFG_PWM_ENABLE
endif

ifeq ($(RHINO_MM_DEBUG),1)
KBUILD_CPPFLAGS += -DRHINO_CONFIG_MM_DEBUG=0 -DDEBUG_CONFIG_AOS_TRACE=1
endif

SH_IOT_USED_SPI0_DMA ?= 1
export PERIPH_PLL_FREQ ?= 320000000
ifeq ($(SH_IOT_USED_SPI0_DMA),1)
KBUILD_CPPFLAGS += -DUSED_SPI0_DMA
endif

# aos3.3 yaml configs
KBUILD_CPPFLAGS += -DRHINO_CONFIG_MM_TLF=1 \
                   -DRHINO_CONFIG_MM_MINISIZEBIT=6 \
                   -DRHINO_CONFIG_MM_MAXMSIZEBIT=28 \
                   -DRHINO_CONFIG_MM_QUICK=0 \
                   -DRHINO_CONFIG_MM_REGION_MUTEX=0 \
                   -DRHINO_CONFIG_MM_BLK=0 \
                   -DRHINO_CONFIG_MM_BLK_SIZE=32 \
                   -DRHINO_CONFIG_MM_TLF_BLK_SIZE=1024 \
                   -DRHINO_CONFIG_MM_TRACE_LVL=0

KBUILD_CPPFLAGS += -DA7_LASTWORD_REGION_LEN=0x4000

C_ONLY_FLAGS    := -std=gnu11
C++_ONLY_FLAGS  := -std=gnu++11 -fno-rtti -fno-exceptions -fno-enforce-eh-specs  -fno-use-cxa-atexit -fno-threadsafe-statics

LDFLAGS_IMAGE += --wrap malloc --wrap calloc --wrap free --wrap realloc --wrap snprintf
