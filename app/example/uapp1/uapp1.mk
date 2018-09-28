NAME := uapp1

$(NAME)_TYPE := app
$(NAME)_MBINS_TYPE := app

ifeq ($(ENABLE_USPACE),1)
$(NAME)_SOURCES := system/entry.c \
	               system/app_mm.c \
	               src/app.c

$(NAME)_SOURCES += test/test.c
$(NAME)_SOURCES += test/mutex_test.c
$(NAME)_SOURCES += test/semphore_test.c
$(NAME)_SOURCES += test/time_test.c
$(NAME)_SOURCES += test/buf_queue_test.c

$(NAME)_INCLUDES := test

GLOBAL_INCLUDES += system \

ifeq ($(MBINS),app)
GLOBAL_DEFINES += AOS_APP_MM
endif

else
$(NAME)_SOURCES := src/app.c
endif

GLOBAL_INCLUDES += .


GLOBAL_DEFINES += AOS_NO_WIFI

$(NAME)_COMPONENTS := yloop cli

