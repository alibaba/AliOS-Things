NAME := board_imx6sl_svc

$(NAME)_MBINS_TYPE   := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=

MODULE               := imx6sl_svc
HOST_ARCH            := Cortex-A9
HOST_MCU_FAMILY      := imx6sl_svc
SUPPORT_BINS         := no
SUPPORT_MBINS        := yes
ENABLE_USPACE        := 1

$(NAME)_SOURCES := src/board.c \
                   src/main.c \
                   src/uspace_init.c

GLOBAL_INCLUDES += include

GLOBAL_DEFINES += STDIO_UART=0 CONFIG_NO_TCPIP  CHIP_MX6SL  BOARD_EVK  BOARD_REV_A

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_IMX6SL
CONFIG_SYSINFO_DEVICE_NAME := IMX6SL

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

ifeq ($(MBINS),)
GLOBAL_LDS_FILES += board/imx6sl_svc/imx6sl_svc.ld.S
else ifeq ($(MBINS),kernel)
GLOBAL_LDS_FILES +=  board/imx6sl_svc/imx6sl_svc_kernel.ld.S
else ifeq ($(MBINS), app)
ifneq ($(MBINS_APP),)
GLOBAL_LDS_FILES +=  board/imx6sl_svc/$(MBINS_APP).ld.S
else
GLOBAL_LDS_FILES +=  board/imx6sl_svc/imx6sl_svc_app.ld.S
endif
endif

SUBGDBINIT_STRING := "b main"
