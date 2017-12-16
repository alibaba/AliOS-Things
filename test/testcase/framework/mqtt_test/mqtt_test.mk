NAME := mqtt_test

$(NAME)_COMPONENTS  += connectivity.mqtt

$(NAME)_SOURCES     += mqtt_test.c

$(NAME)_CFLAGS      += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS      += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS      += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS      += -Wno-unused-value -Wno-strict-aliasing

