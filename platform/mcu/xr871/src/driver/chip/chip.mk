NAME := chip

$(NAME)_TYPE := kernel
$(NAME)_SOURCES := \
                   codec/hal_ac101.c          \
                   codec/hal_codec.c          \
                   flashchip/flash_chip.c     \
                   flashchip/flash_default.c  \
                   hal_adc.c                  \
                   hal_board.c                \
                   hal_ccm.c                  \
                   hal_crypto.c               \
                   hal_csi.c                  \
                   hal_dma.c                  \
                   hal_dmic.c                 \
                   hal_efuse.c                \
                   hal_flash.c                \
                   hal_flashcache.c           \
                   hal_flashctrl.c            \
                   hal_global.c               \
                   hal_gpio.c                 \
                   hal_i2c.c                  \
                   hal_i2s.c                  \
                   hal_irrx.c                 \
                   hal_irtx.c                 \
                   hal_mbox.c                 \
                   hal_nvic.c                 \
                   hal_prcm.c                 \
                   hal_pwm.c                  \
                   hal_rtc.c                  \
                   hal_spi.c                  \
                   hal_spinlock.c             \
                   hal_timer.c                \
                   hal_uart.c                 \
                   hal_util.c                 \
                   hal_wakeup.c               \
                   hal_wdg.c                  \
                   hal_xip.c                  \
                   ir_nec.c                   \
                   sdmmc/core.c               \
                   sdmmc/hal_sdhost.c         \
                   sdmmc/sd.c                 \
                   system_chip.c

