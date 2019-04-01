NAME := cfg

$(NAME)_CFLAGS +=
$(NAME)_INCLUDES += ../../../../components/third_party/cJSON
$(NAME)_INCLUDES += ../../../../components/sys

$(NAME)_SOURCES :=
$(NAME)_SOURCES += sa_cfg.c
$(NAME)_SOURCES += mac_cfg.c
$(NAME)_SOURCES += user_cfg.c
