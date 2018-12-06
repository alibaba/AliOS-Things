NAME := board_rda8955

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 0.0.1
$(NAME)_SUMMARY    := configuration for board rda8955
SUPPORT_MBINS      := no
MODULE             := 1062
HOST_ARCH          := MIPS-I
HOST_MCU_FAMILY    := rda8955
HOST_MCU_NAME      := rda8955

GLOBAL_INCLUDES += .

$(NAME)_SOURCES := board.c

#rhino_sxr_mutios
osal := rhino_sxr

no_with_lwip := 1

use_private_lwip := 1

GLOBAL_CFLAGS += -include fpconfig.h

GLOBAL_DEFINES += STDIO_UART=0 CONFIG_NO_TCPIP BOARD_RDA8955 CONFIG_GPIO_AS_PINRESET FLOAT_ABI_HARD   

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_RDA8955
CONFIG_SYSINFO_DEVICE_NAME := rda8955

#rda8955 pro CFLAGS
GLOBAL_CFLAGS += -std=gnu99 -Wno-pointer-sign -Os -g -fno-inline-small-functions -fno-inline-functions -fno-align-functions -fno-align-jumps -fno-align-loops -fno-align-labels -msoft-float -Wundef -Wunused-function -Wuninitialized -minterlink-mips16 -fno-strict-aliasing -ffunction-sections -fdata-sections -fno-common -fno-builtin-iswspace -ffixed-t3 -ffixed-t4 -ffixed-t5 -ffixed-t6 -ffixed-t7 -ffixed-s2 -ffixed-s3 -ffixed-s4 -ffixed-s5 -ffixed-s6 -ffixed-s7 -ffixed-fp -G0 -Wall -march=xcpu -mtune=xcpu -Wa,-march=xcpu,-mtune=xcpu -EL -mexplicit-relocs -fweb -frename-registers -mmemcpy -mmips-tfile -nostartfiles -nostdlib -nostdinc -nodefaultlibs -c -pipe -fwide-exec-charset=UTF-16LE -fshort-wchar  -mips16  -DEL -DAUD_MUTE_FOR_BT_WORKAROUND -DBCPU_CACHE_MODE_WRITE_BACK=0 -DCHIP_ASIC_ID=9 -DCHIP_CAP_ON_APB=0 -DCHIP_CHEST_LEN=5 -DCHIP_CIPHER_A53_SUPPORTED=1 -DCHIP_DIE_8955 -DCHIP_EDGE_SUPPORTED=0 -DCHIP_GPIO_NUM_CHARGER_DETECT=0 -DCHIP_HAS_14_BIT_RF_PATH=1 -DCHIP_HAS_AHB_MONITOR=0 -DCHIP_HAS_ANA_RF=0 -DCHIP_HAS_ASYNC_BCPU=0 -DCHIP_HAS_AUTOSIMCLK=1 -DCHIP_HAS_BCPU_ROM=1 -DCHIP_HAS_DBG_UART_SW_FLOW_CONTROL=1 -DCHIP_HAS_GOUDA=1 -DCHIP_HAS_I2S_DI_1=1 -DCHIP_HAS_PROXY_MEMORY=0 -DCHIP_HAS_PXTS_LEVELS=1 -DCHIP_HAS_SDIO=1 -DCHIP_HAS_SDMMC=1 -DCHIP_HAS_SIM_T0T1=0 -DCHIP_HAS_STEREO_DAC=1 -DCHIP_HAS_SYS_IFC2 -DCHIP_HAS_SYS_IFC_SIZE=1 -DCHIP_HAS_TMCU=0 -DCHIP_HAS_USB=1 -DCHIP_IFC_RF_TX_CHANNEL=1 -DCHIP_ISPI_CS_QTY=3 -DCHIP_ITLV_8PSK_SUPPORTED=0 -DCHIP_KEY_COL_NB=8 -DCHIP_KEY_ROW_NB=8 -DCHIP_MAX_SPEED=312000000 -DCHIP_NUMBER_OF_PATCHES=16 -DCHIP_SPI2_CS_QTY=1 -DCHIP_SPI_CS_QTY=3 -DCHIP_SPI_HAS_DIO=1 -DCHIP_SPI_HAS_INTERNAL_FORCE=1 -DCHIP_SPI_HAS_PATTERN_WHILE=1 -DCHIP_STD_UART_QTY=2 -DCHIP_SYS_CRITICAL_SECTION_REG=0xA1A01014 -DCHIP_TCU_CLK=13000000 -DCHIP_VAMOSI_SUPPORTED=1 -DCHIP_VITAC_8PSK_SUPPORTED=0 -DCHIP_WITH_VOC=1 -DCHIP_WITH_VOC_ROM=0 -DCHIP_XCOR_8PSK_SUPPORTED=0 -DCHIP_XCOR_DELAY8=0 -DCHIP_XCOR_HAS_SLIDING_WINDOW=1 -DCSW_EXTENDED_API=0 -DCSW_EXTENDED_API_AUDIO_VIDEO=0 -DCSW_EXTENDED_API_CAMERA=0 -DCSW_EXTENDED_API_LCD=1 -DCSW_EXTENDED_API_MRS=0 -DCSW_EXTENDED_API_PMD_USB=0 -DCT_PRODUCT=test -DDISABLE_SECOND_SIM -DFAST_CLOCK_156M=156000000 -DGPRS_MEASURE_MODE_HT100=0 -DHAL_USB_DRIVER_DONT_USE_ROMED_CODE -DNUMBER_OF_SIM=2 -DONLY_AT_SUPPORT -DPAL_WINDOWS_LOCK_MCP_ON_RX -DPAL_WINDOW_EXTENSION_FOR_AHB_LATENCY -DSPC_IF_VER=5 -DSPI_REG_DEBUG -DSXR_CSP_JB_CTX_HEAP=0 -DSXR_CSP_JB_STCK_HEAP=0 -DSXR_CSP_TK_STCK_HEAP=0 -DVMIC_POWER_ON_WORKAROUND -DXCPU_CACHE_MODE_WRITE_BACK=1 -D__JADE__ -D__SXS_NO_FLASH_HANDLER__ -D__SXS_STR_TO_ID__ -D__TARGET__ -D__XCPU__ -D__NEW_GCC__ -DUSE_GCC_4=1 -DUSE_BINUTILS_2_19=1 -D__REDUCED_REGS__ -DBUILD_DATE=20180518 -D_T_MMI= 


GLOBAL_ASMFLAGS +=  -std=gnu99 -Wno-pointer-sign -Os -g -fno-inline-small-functions -fno-inline-functions -fno-align-functions -fno-align-jumps -fno-align-loops -fno-align-labels -msoft-float -Wundef -Wunused-function -Wuninitialized -minterlink-mips16 -fno-strict-aliasing -ffunction-sections -fdata-sections -fno-common -fno-builtin-iswspace -ffixed-t3 -ffixed-t4 -ffixed-t5 -ffixed-t6 -ffixed-t7 -ffixed-s2 -ffixed-s3 -ffixed-s4 -ffixed-s5 -ffixed-s6 -ffixed-s7 -ffixed-fp -G0 -Wall -march=xcpu -mtune=xcpu -Wa,-march=xcpu,-mtune=xcpu -EL -mexplicit-relocs -fweb -frename-registers -mmemcpy -mmips-tfile -nostartfiles -nostdlib -nostdinc -nodefaultlibs -c -pipe -fwide-exec-charset=UTF-16LE -fshort-wchar  -mips16  -DEL -DAUD_MUTE_FOR_BT_WORKAROUND -DBCPU_CACHE_MODE_WRITE_BACK=0 -DCHIP_ASIC_ID=9 -DCHIP_CAP_ON_APB=0 -DCHIP_CHEST_LEN=5 -DCHIP_CIPHER_A53_SUPPORTED=1 -DCHIP_DIE_8955 -DCHIP_EDGE_SUPPORTED=0 -DCHIP_GPIO_NUM_CHARGER_DETECT=0 -DCHIP_HAS_14_BIT_RF_PATH=1 -DCHIP_HAS_AHB_MONITOR=0 -DCHIP_HAS_ANA_RF=0 -DCHIP_HAS_ASYNC_BCPU=0 -DCHIP_HAS_AUTOSIMCLK=1 -DCHIP_HAS_BCPU_ROM=1 -DCHIP_HAS_DBG_UART_SW_FLOW_CONTROL=1 -DCHIP_HAS_GOUDA=1 -DCHIP_HAS_I2S_DI_1=1 -DCHIP_HAS_PROXY_MEMORY=0 -DCHIP_HAS_PXTS_LEVELS=1 -DCHIP_HAS_SDIO=1 -DCHIP_HAS_SDMMC=1 -DCHIP_HAS_SIM_T0T1=0 -DCHIP_HAS_STEREO_DAC=1 -DCHIP_HAS_SYS_IFC2 -DCHIP_HAS_SYS_IFC_SIZE=1 -DCHIP_HAS_TMCU=0 -DCHIP_HAS_USB=1 -DCHIP_IFC_RF_TX_CHANNEL=1 -DCHIP_ISPI_CS_QTY=3 -DCHIP_ITLV_8PSK_SUPPORTED=0 -DCHIP_KEY_COL_NB=8 -DCHIP_KEY_ROW_NB=8 -DCHIP_MAX_SPEED=312000000 -DCHIP_NUMBER_OF_PATCHES=16 -DCHIP_SPI2_CS_QTY=1 -DCHIP_SPI_CS_QTY=3 -DCHIP_SPI_HAS_DIO=1 -DCHIP_SPI_HAS_INTERNAL_FORCE=1 -DCHIP_SPI_HAS_PATTERN_WHILE=1 -DCHIP_STD_UART_QTY=2 -DCHIP_SYS_CRITICAL_SECTION_REG=0xA1A01014 -DCHIP_TCU_CLK=13000000 -DCHIP_VAMOSI_SUPPORTED=1 -DCHIP_VITAC_8PSK_SUPPORTED=0 -DCHIP_WITH_VOC=1 -DCHIP_WITH_VOC_ROM=0 -DCHIP_XCOR_8PSK_SUPPORTED=0 -DCHIP_XCOR_DELAY8=0 -DCHIP_XCOR_HAS_SLIDING_WINDOW=1 -DCSW_EXTENDED_API=0 -DCSW_EXTENDED_API_AUDIO_VIDEO=0 -DCSW_EXTENDED_API_CAMERA=0 -DCSW_EXTENDED_API_LCD=1 -DCSW_EXTENDED_API_MRS=0 -DCSW_EXTENDED_API_PMD_USB=0 -DCT_PRODUCT=test -DDISABLE_SECOND_SIM -DFAST_CLOCK_156M=156000000 -DGPRS_MEASURE_MODE_HT100=0 -DHAL_USB_DRIVER_DONT_USE_ROMED_CODE -DNUMBER_OF_SIM=2 -DONLY_AT_SUPPORT -DPAL_WINDOWS_LOCK_MCP_ON_RX -DPAL_WINDOW_EXTENSION_FOR_AHB_LATENCY -DSPC_IF_VER=5 -DSPI_REG_DEBUG -DSXR_CSP_JB_CTX_HEAP=0 -DSXR_CSP_JB_STCK_HEAP=0 -DSXR_CSP_TK_STCK_HEAP=0 -DVMIC_POWER_ON_WORKAROUND -DXCPU_CACHE_MODE_WRITE_BACK=1 -D__JADE__ -D__SXS_NO_FLASH_HANDLER__ -D__SXS_STR_TO_ID__ -D__TARGET__ -D__XCPU__ -D__NEW_GCC__ -DUSE_GCC_4=1 -DUSE_BINUTILS_2_19=1 -D__REDUCED_REGS__ -DBUILD_DATE=20180518 -D_T_MMI=

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"


# Extra build target in mico_standard_targets.mk, include bootloader, and copy output file to eclipse debug file (copy_output_for_eclipse)
EXTRA_TARGET_MAKEFILES +=  $(MAKEFILES_PATH)/aos_standard_targets.mk
#EXTRA_TARGET_MAKEFILES +=  $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/gen_crc_bin.mk

# Define default component testcase set
ifeq (, $(findstring yts, $(BUILD_STRING)))
GLOBAL_DEFINES += RHINO_CONFIG_WORKQUEUE=1
TEST_COMPONENTS += basic api wifi_hal rhino osal kv yloop alicrypto cjson digest_algorithm
else
GLOBAL_DEFINES += RHINO_CONFIG_WORKQUEUE=0
endif
