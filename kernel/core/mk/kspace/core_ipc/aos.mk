NAME := core_ipc

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := inter process communication


$(NAME)_SOURCES += ipc.c
$(NAME)_SOURCES += ipc_str.c
ifneq ($(HOST_MCU_FAMILY),mcu_raspberry-pi)
$(NAME)_SOURCES += ipc_uds.c
endif

$(NAME)_SOURCES += ipc_perf.c
$(NAME)_SOURCES += ipc_queue.c
$(NAME)_SOURCES += ipc_stream.c
$(NAME)_SOURCES += ipc_fifo.c
$(NAME)_SOURCES += ipc_msg.c

$(NAME)_INCLUDES += ./include
