NAME := cplusplus

$(NAME)_MBINS_TYPE := share
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := cplusplus for alios things

$(NAME)_SOURCES += cpp_init.c
$(NAME)_SOURCES += thread.cpp
$(NAME)_SOURCES += semaphore.cpp
$(NAME)_SOURCES += mutex.cpp
$(NAME)_SOURCES += queue.cpp
$(NAME)_SOURCES += timer.cpp
$(NAME)_SOURCES += workQueue.cpp

$(NAME)_INCLUDES += ./include

ifeq ($(MBINS), app)
$(NAME)_SOURCES += cpp_mem_uspace.cpp
else
$(NAME)_SOURCES += cpp_mem.cpp
endif

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall
endif

GLOBAL_DEFINES  += AOS_COMP_CPLUSPLUS
CPLUSPLUS_FLAGS :=1

GLOBAL_INCLUDES += .
