$(NAME)_INCLUDES += src/STM32L071KB/runapp                    \
$(NAME)_DEFINES += STM32L071xx

$(NAME)_SOURCES += src/STM32L071KB/runapp/main.c                                  \
                    src/STM32L071KB/startup_stm32l071xx_gcc.s                      \
                    src/STM32L071KB/runapp/stm32l0xx_hal_msp.c                     \
                    src/STM32L071KB/runapp/system_stm32l0xx.c                      \
                    src/STM32L071KB/runapp/stm32l0xx_it.c                          \

GLOBAL_LDFLAGS += -T platform/mcu/stm32l0xx/stm32l071kb/STM32L071KBUx_FLASH.ld