NAME := cfg

$(NAME)_CFLAGS +=
$(NAME)_INCLUDES += ../$(SDKDIR)/components/third_party/cJSON
$(NAME)_INCLUDES += ../$(SDKDIR)/components/sys

$(NAME)_SOURCES :=
$(NAME)_SOURCES += sa_cfg.c
$(NAME)_SOURCES += mac_cfg.c
$(NAME)_SOURCES += user_cfg.c
