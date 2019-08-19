NAME := cplusplus

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := cplusplus for alios things

$(NAME)_SOURCES := cpp_mem.cpp
$(NAME)_SOURCES += cpp_init.c
$(NAME)_SOURCES += thread.cpp
$(NAME)_SOURCES += semaphore.cpp
$(NAME)_SOURCES += mutex.cpp
$(NAME)_SOURCES += queue.cpp
$(NAME)_SOURCES += timer.cpp
$(NAME)_SOURCES += workQueue.cpp

$(NAME)_INCLUDES += ./include

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall -Werror
endif

GLOBAL_DEFINES  += AOS_COMP_CPLUSPLUS
CPLUSPLUS_FLAGS :=1

GLOBAL_INCLUDES += .
