NAME := osal

$(NAME)_MBINS_TYPE := share
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := os interface adaptation layer

GLOBAL_INCLUDES += ./aos

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall -Werror
endif

ifeq ($(HOST_ARCH),ARM968E-S)
$(NAME)_CFLAGS += -marm
endif

osal            ?= rhino
$(NAME)_SOURCES += aos/common.c

ifeq ($(osal),freertos)
GLOBAL_DEFINES  += OSAL_FREERTOS
$(NAME)_SOURCES += aos/ext/freertos.c
endif

ifeq ($(osal),posix)
GLOBAL_DEFINES  += OSAL_POSIX
$(NAME)_SOURCES += aos/ext/posix.c
endif

ifeq ($(osal),rhino_sxr)
GLOBAL_DEFINES  += OSAL_SXR
$(NAME)_SOURCES += aos/ext/sxr.c
endif

ifeq ($(osal),rhino_sxr_mutios)
GLOBAL_DEFINES  += OSAL_SXR_MUTIOS
$(NAME)_SOURCES += aos/ext/rhino_rda8955.c
endif

ifeq ($(osal),rhino)
GLOBAL_DEFINES     += OSAL_RHINO AOS_HAL
$(NAME)_COMPONENTS += rhino debug

ifeq ($(MBINS),app)
$(NAME)_SOURCES += aos/rhino_uspace.c
else
$(NAME)_SOURCES += aos/rhino.c
endif

ifeq ($(AOS_KV), 1)
$(NAME)_SOURCES += aos/kv.c
endif

ifeq ($(AOS_COMP_MBMASTER),1)
$(NAME)_SOURCES += aos/mbmaster.c
endif

ifeq ($(AOS_CLI),1)
$(NAME)_SOURCES += aos/cli.c
endif

ifeq ($(AOS_VFS),1)
$(NAME)_SOURCES += aos/vfs.c
endif

endif

