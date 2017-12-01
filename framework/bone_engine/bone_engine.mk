NAME := bone_engine


$(NAME)_SOURCES += jse/be_jse_executor.c \
                   jse/be_jse_lex.c \
                   jse/be_jse_utils.c \
                   jse/be_jse_symbol.c \
                   src/bone_engine.c \
                   modules/be_bi_module.c \
                   modules/be_iot_module.c \
                   modules/aos_alink_impl.c \
                   modules/module_alink.c \
                   modules/module_process.c \
                   modules/module_gpio.c \
                   modules/module_timers.c \
                   modules/module_uart.c

GLOBAL_INCLUDES += include


# 根据board类型区别编译

$(warning BUILD_TYPE=$(BUILD_TYPE) )
$(warning $(NAME)_BUILD_TYPE=$(bone_engine_BUILD_TYPE) )
$(warning HOST_ARCH=$(HOST_ARCH) )
$(warning CONFIG_SYSINFO_PRODUCT_MODEL=$(CONFIG_SYSINFO_PRODUCT_MODEL) )

GLOBAL_INCLUDES += boards/$(CONFIG_SYSINFO_PRODUCT_MODEL)

