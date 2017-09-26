GLOBAL_INCLUDES  += os
$(NAME)_INCLUDES += os/product/ os/platform/

$(NAME)_SOURCES  += os/os_misc.c os/os_thread.c
$(NAME)_SOURCES  += os/product/product.c

ifneq ($(findstring linuxhost, $(BUILD_STRING)), linuxhost)
$(NAME)_SOURCES  += os/platform/aos_awss.c os/platform/aos_hardware.c os/platform/aos_os.c os/platform/aos_network.c os/platform/aos_ssl.c
else
$(NAME)_SOURCES  += os/platform/aos_awss.c os/platform/linux_hardware.c os/platform/aos_os.c os/platform/linux_ota.c os/platform/aos_network.c
$(NAME)_SOURCES  += os/platform/aos_ssl.c
endif
