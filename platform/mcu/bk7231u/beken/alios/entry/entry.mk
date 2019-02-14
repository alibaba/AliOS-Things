NAME := entry

$(NAME)_TYPE := kernel

$(NAME)_CFLAGS += -marm
                   
#$(NAME)_SOURCES	 := arch_main.c \
#                    boot_handlers.S \
#                    boot_vectors.S \
#                    ll.S \
#                    ../../driver/intc/intc.c 

$(NAME)_PREBUILT_LIBRARY := entry.a