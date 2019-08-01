NAME := entry

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    :=

$(NAME)_CFLAGS += -marm
                   
$(NAME)_SOURCES	 := boot_handlers.S \
                    boot_vectors.S \
                    ll.S 