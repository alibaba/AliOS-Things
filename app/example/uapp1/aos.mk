NAME := uapp1

$(NAME)_TYPE := app

$(NAME)_MBINS_TYPE := app

$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY :=

ifeq ($(ENABLE_USPACE),1)

ifeq ($(APP_FORMAT),bin)
$(NAME)_SOURCES := src/entry_bin.c
else ifeq ($(APP_FORMAT),elf)
$(NAME)_SOURCES := src/entry_elf.c
else
$(error "Unknown app format")
endif

$(NAME)_SOURCES += src/app.c

ifeq ($(RHINO_CONFIG_UTCPIP),y)
$(NAME)_SOURCES += net/lwip/core/def.c \
                   net/lwip/core/ip4_addr.c \
                   net/lwip/core/ip6_addr.c

$(NAME)_SOURCES += test/lwip/http_download.c \
                   test/lwip/parse_host.c
endif

$(NAME)_SOURCES += test/aos_uspace_test.c \
                   test/cut.c

$(NAME)_INCLUDES += test include

endif

