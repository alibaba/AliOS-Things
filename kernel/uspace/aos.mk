NAME := uspace

$(NAME)_MBINS_TYPE := share
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := syscall uprocess utask utimer IPC

ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall -Werror
endif

ifeq ($(ENABLE_USPACE),1)
ifeq ($(MBINS),kernel)
$(NAME)_SOURCES := u_task.c
$(NAME)_SOURCES += u_proc_msg.c
$(NAME)_SOURCES += syscall/ksyscall/hal_adc_ksyscall.c
$(NAME)_SOURCES += syscall/ksyscall/hal_dac_ksyscall.c
$(NAME)_SOURCES += syscall/ksyscall/hal_flash_ksyscall.c
$(NAME)_SOURCES += syscall/ksyscall/hal_gpio_ksyscall.c
$(NAME)_SOURCES += syscall/ksyscall/hal_i2c_ksyscall.c
$(NAME)_SOURCES += syscall/ksyscall/hal_nand_ksyscall.c
$(NAME)_SOURCES += syscall/ksyscall/hal_nor_ksyscall.c
$(NAME)_SOURCES += syscall/ksyscall/hal_rtc_ksyscall.c
$(NAME)_SOURCES += syscall/ksyscall/hal_sd_ksyscall.c
$(NAME)_SOURCES += syscall/ksyscall/hal_spi_ksyscall.c
$(NAME)_SOURCES += syscall/ksyscall/hal_uart_ksyscall.c
$(NAME)_SOURCES += syscall/ksyscall/cli_ksyscall.c
$(NAME)_SOURCES += syscall/ksyscall/lwip_ksyscall.c
$(NAME)_SOURCES += syscall/ksyscall/rhino_ksyscall.c
$(NAME)_SOURCES += syscall/ksyscall/syscall_tbl.c

$(NAME)_INCLUDES   += syscall/ksyscall
else ifeq ($(MBINS),app)
$(NAME)_SOURCES := u_timer.c
$(NAME)_SOURCES += u_res_task.c
$(NAME)_SOURCES += u_mm.c
$(NAME)_SOURCES += syscall/usyscall/hal_adc_usyscall.c
$(NAME)_SOURCES += syscall/usyscall/hal_dac_usyscall.c
$(NAME)_SOURCES += syscall/usyscall/hal_flash_usyscall.c
$(NAME)_SOURCES += syscall/usyscall/hal_gpio_usyscall.c
$(NAME)_SOURCES += syscall/usyscall/hal_i2c_usyscall.c
$(NAME)_SOURCES += syscall/usyscall/hal_nand_usyscall.c
$(NAME)_SOURCES += syscall/usyscall/hal_nor_usyscall.c
$(NAME)_SOURCES += syscall/usyscall/hal_rtc_usyscall.c
$(NAME)_SOURCES += syscall/usyscall/hal_sd_usyscall.c
$(NAME)_SOURCES += syscall/usyscall/hal_spi_usyscall.c
$(NAME)_SOURCES += syscall/usyscall/hal_uart_usyscall.c
$(NAME)_SOURCES += syscall/usyscall/cli_usyscall.c
$(NAME)_SOURCES += syscall/usyscall/lwip_usyscall.c
$(NAME)_SOURCES += syscall/usyscall/rhino_usyscall.c

GLOBAL_DEFINES += AOS_USPACE

AOS_COMP_VFS := 0

endif
GLOBAL_INCLUDES    += syscall/include
GLOBAL_INCLUDES    += ./include

ifeq ($(AOS_COMP_USER_SPACE),y)
GLOBAL_DEFINES     += RHINO_CONFIG_USER_SPACE=1
else
GLOBAL_DEFINES     += RHINO_CONFIG_USER_SPACE=0
endif

endif

