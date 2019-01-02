NAME := cplusplus

$(NAME)_TYPE := kernel
$(NAME)_SOURCES     := cpp_mem.cpp
$(NAME)_SOURCES     += cpp_init.c
$(NAME)_SOURCES     += Thread.cpp
$(NAME)_SOURCES     += Semaphore.cpp
$(NAME)_SOURCES     += Mutex.cpp
$(NAME)_SOURCES     += Queue.cpp
$(NAME)_SOURCES     += Timer.cpp
$(NAME)_SOURCES     += WorkQueue.cpp
$(NAME)_INCLUDES    += ./include

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

GLOBAL_DEFINES      += AOS_CPLUSPLUS

GLOBAL_INCLUDES     += .