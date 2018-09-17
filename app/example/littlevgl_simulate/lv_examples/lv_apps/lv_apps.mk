NAME := lv_apps

$(NAME)_TYPE 	    := framework

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),armcc)
GLOBAL_DEFINES      += __BSD_VISIBLE
endif

GLOBAL_INCLUDES     += include
GLOBAL_DEFINES      += AOS_LV_APPS

ifneq ($(IDE),keil)
$(NAME)_COMPONENTS  += example.littlevgl_simulate.lv_examples.lv_apps.benchmark
$(NAME)_COMPONENTS  += example.littlevgl_simulate.lv_examples.lv_apps.demo
$(NAME)_COMPONENTS  += example.littlevgl_simulate.lv_examples.lv_apps.sysmon
$(NAME)_COMPONENTS  += example.littlevgl_simulate.lv_examples.lv_apps.terminal
$(NAME)_COMPONENTS  += example.littlevgl_simulate.lv_examples.lv_apps.tpcal
endif