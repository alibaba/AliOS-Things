
# drivers/src/driver/chip
XR871_CHIP_PATH  := drivers/src/driver/chip
XR871_CHIP_FILES := \
                   $(XR871_CHIP_PATH)/codec/hal_ac101.c          \
                   $(XR871_CHIP_PATH)/codec/hal_ac102.c          \
                   $(XR871_CHIP_PATH)/codec/hal_codec.c          \
                   $(XR871_CHIP_PATH)/flashchip/flash_chip.c     \
                   $(XR871_CHIP_PATH)/flashchip/flash_default.c  \
                   $(XR871_CHIP_PATH)/flashchip/flash_EN25QHXXA.c  \
                   $(XR871_CHIP_PATH)/flashchip/flash_P25QXXH.c  \
                   $(XR871_CHIP_PATH)/flashchip/flash_XM25QHXXA.c  \
                   $(XR871_CHIP_PATH)/flashchip/flash_XT25FXXB.c  \
                   $(XR871_CHIP_PATH)/hal_adc.c                  \
                   $(XR871_CHIP_PATH)/hal_board.c                \
                   $(XR871_CHIP_PATH)/hal_ccm.c                  \
                   $(XR871_CHIP_PATH)/hal_crypto.c               \
                   $(XR871_CHIP_PATH)/hal_csi.c                  \
                   $(XR871_CHIP_PATH)/hal_dma.c                  \
                   $(XR871_CHIP_PATH)/hal_dmic.c                 \
                   $(XR871_CHIP_PATH)/hal_efuse.c                \
                   $(XR871_CHIP_PATH)/hal_flash.c                \
                   $(XR871_CHIP_PATH)/hal_flashcache.c           \
                   $(XR871_CHIP_PATH)/hal_flashctrl.c            \
                   $(XR871_CHIP_PATH)/hal_global.c               \
                   $(XR871_CHIP_PATH)/hal_gpio.c                 \
                   $(XR871_CHIP_PATH)/hal_i2c.c                  \
                   $(XR871_CHIP_PATH)/hal_i2s.c                  \
                   $(XR871_CHIP_PATH)/hal_irrx.c                 \
                   $(XR871_CHIP_PATH)/hal_irtx.c                 \
                   $(XR871_CHIP_PATH)/hal_mbox.c                 \
                   $(XR871_CHIP_PATH)/hal_nvic.c                 \
                   $(XR871_CHIP_PATH)/hal_prcm.c                 \
                   $(XR871_CHIP_PATH)/hal_pwm.c                  \
                   $(XR871_CHIP_PATH)/hal_rtc.c                  \
                   $(XR871_CHIP_PATH)/hal_spi.c                  \
                   $(XR871_CHIP_PATH)/hal_spinlock.c             \
                   $(XR871_CHIP_PATH)/hal_timer.c                \
                   $(XR871_CHIP_PATH)/hal_uart.c                 \
                   $(XR871_CHIP_PATH)/hal_util.c                 \
                   $(XR871_CHIP_PATH)/hal_wakeup.c               \
                   $(XR871_CHIP_PATH)/hal_wdg.c                  \
                   $(XR871_CHIP_PATH)/hal_xip.c                  \
                   $(XR871_CHIP_PATH)/hal_swd.c                  \
                   $(XR871_CHIP_PATH)/ir_nec.c                   \
                   $(XR871_CHIP_PATH)/sdmmc/core.c               \
                   $(XR871_CHIP_PATH)/sdmmc/hal_sdhost.c         \
                   $(XR871_CHIP_PATH)/sdmmc/sd.c                 \
                   $(XR871_CHIP_PATH)/system_chip.c

# drivers/src/image
XR871_IMAGE_PATH  := drivers/src/image
XR871_IMAGE_FILES := $(XR871_IMAGE_PATH)/fdcm.c \
                     $(XR871_IMAGE_PATH)/flash.c \
                     $(XR871_IMAGE_PATH)/image.c

# drivers/src/net/wlan
XR871_WLAN_PATH  := drivers/src/net/wlan
XR871_WLAN_FILES := $(XR871_WLAN_PATH)/ethernetif.c \
                    $(XR871_WLAN_PATH)/wlan.c \
                    $(XR871_WLAN_PATH)/wlan_ctrl.c

# drivers/src/ota
XR871_OTA_PATH  := drivers/src/ota
XR871_OTA_FILES := $(XR871_OTA_PATH)/ota.c \
                   $(XR871_OTA_PATH)/ota_file.c \
                   $(XR871_OTA_PATH)/ota_http.c

# drivers/src/pm 
XR871_PM_PATH  := drivers/src/pm
XR871_PM_FILES := $(XR871_PM_PATH)/pm.c \
                  $(XR871_PM_PATH)/port.c

# drivers/src/efpg
XR871_EFPG_PATH  := drivers/src/efpg
XR871_EFPG_FILES := $(XR871_EFPG_PATH)/efpg_efuse.c \
                    $(XR871_EFPG_PATH)/efpg.c

# drivers/src/sys 
XR871_SYS_PATH  := drivers/src/sys
XR871_SYS_FILES := $(XR871_SYS_PATH)/ducc/ducc.c \
                   $(XR871_SYS_PATH)/ducc/ducc_app.c \
                   $(XR871_SYS_PATH)/ducc/ducc_hw_mbox.c \
                   $(XR871_SYS_PATH)/ducc/ducc_mbox.c \
                   $(XR871_SYS_PATH)/mbuf/mbuf_1.c

# drivers/src/net/lwip
XR871_LWIP_PATH  := drivers/src/net/lwip
XR871_LWIP_FILES := $(XR871_LWIP_PATH)/memcpy.c \
                    $(XR871_LWIP_PATH)/checksum.c

# drivers/src/console
XR871_CONSOLE_PATH  := drivers/src/console
XR871_CONSOLE_FILES := $(XR871_CONSOLE_PATH)/console.c

# drivers/src/project
XR871_PROJECT_PATH  := drivers/project
XR871_PROJECT_FILES := $(XR871_PROJECT_PATH)/common/framework/sys_ctrl/container.c \
                       $(XR871_PROJECT_PATH)/common/framework/sys_ctrl/event_queue.c \
                       $(XR871_PROJECT_PATH)/common/framework/sys_ctrl/observer.c \
                       $(XR871_PROJECT_PATH)/common/framework/sys_ctrl/publisher.c \
                       $(XR871_PROJECT_PATH)/common/framework/sys_ctrl/sys_ctrl.c \
                       $(XR871_PROJECT_PATH)/common/framework/sys_ctrl/looper.c \
                       $(XR871_PROJECT_PATH)/common/framework/audio_ctrl.c \
                       $(XR871_PROJECT_PATH)/common/framework/fs_ctrl.c \
                       $(XR871_PROJECT_PATH)/common/framework/net_ctrl.c \
                       $(XR871_PROJECT_PATH)/common/framework/net_sys.c \
                       $(XR871_PROJECT_PATH)/common/framework/sc_assistant_port.c \
                       $(XR871_PROJECT_PATH)/common/framework/platform_init.c \
                       $(XR871_PROJECT_PATH)/common/framework/sysinfo.c \
                       $(XR871_PROJECT_PATH)/common/cmd/cmd_wlan.c \
                       $(XR871_PROJECT_PATH)/common/cmd/cmd_util.c \
                       $(XR871_PROJECT_PATH)/common/cmd/cmd_upgrade.c \
                       $(XR871_PROJECT_PATH)/common/cmd/cmd_mem.c \
                       $(XR871_PROJECT_PATH)/common/board/board.c \
                       $(XR871_PROJECT_PATH)/common/board/board_common.c \
                       $(XR871_PROJECT_PATH)/common/board/xr871_evb_main/board_config.c \
                       $(XR871_PROJECT_PATH)/common/startup/gcc/exception.c \
                       $(XR871_PROJECT_PATH)/common/startup/gcc/retarget.c \
                       $(XR871_PROJECT_PATH)/common/startup/gcc/retarget_stdout.c
                       #$(XR871_PROJECT_PATH)/common/startup/gcc/startup.S

# drivers/src/audio
XR871_AUDIO_PATH  := drivers/src/audio
XR871_AUDIO_FILES := $(XR871_AUDIO_PATH)/manager/audio_manager.c \
                     $(XR871_AUDIO_PATH)/pcm/audio_pcm.c

