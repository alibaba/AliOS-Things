NAME := rhino_test

$(NAME)_COMPONENTS  += rhino 

$(NAME)_SOURCES     += rhino_test.c

ifneq (,$(findstring linux, $(BUILD_STRING)))
$(NAME)_SOURCES     += arch/linux/port_test.c
endif

$(NAME)_CFLAGS      += -Wall -Werror

