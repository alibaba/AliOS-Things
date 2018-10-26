NAME := uapp2

$(NAME)_TYPE := app
$(NAME)_MBINS_TYPE := app

ifeq ($(ENABLE_USPACE),1)
$(NAME)_SOURCES := system/entry.c \
                   system/app_mm.c \
                   src/app.c

GLOBAL_INCLUDES += system

ifeq ($(MBINS),app)
GLOBAL_DEFINES += AOS_APP_MM
endif

else
$(NAME)_SOURCES := src/app.c
endif

GLOBAL_INCLUDES += .


GLOBAL_DEFINES += AOS_NO_WIFI

$(NAME)_COMPONENTS := yloop cli

