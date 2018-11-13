NAME := cpp_demo

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=
$(NAME)_SOURCES := cpp_demo.cpp
$(NAME)_SOURCES += thread_test.cpp
$(NAME)_SOURCES += timer_test.cpp
$(NAME)_SOURCES += queue_test.cpp
$(NAME)_SOURCES += semaphore_test.cpp
$(NAME)_SOURCES += mutex_test.cpp
$(NAME)_SOURCES += workqueue_test.cpp
$(NAME)_SOURCES += inheritance.cpp

GLOBAL_DEFINES += AOS_NO_WIFI
GLOBAL_LDFLAGS += -lstdc++

$(NAME)_COMPONENTS += yloop cli kernel/rhino/cplusplus

ifeq ($(BENCHMARKS),1)
$(NAME)_COMPONENTS  += benchmarks
GLOBAL_DEFINES      += CONFIG_CMD_BENCHMARKS
endif

GLOBAL_INCLUDES += ./