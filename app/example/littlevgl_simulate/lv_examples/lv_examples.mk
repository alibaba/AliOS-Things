NAME := lv_examples

$(NAME)_TYPE 	    := framework

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),armcc)
GLOBAL_DEFINES      += __BSD_VISIBLE
else ifeq ($(COMPILER),rvct)
GLOBAL_DEFINES      += __BSD_VISIBLE
endif

GLOBAL_INCLUDES     += include
GLOBAL_DEFINES      += AOS_LV_EXAMPLES

ifneq ($(IDE),keil)
$(NAME)_COMPONENTS  += example.littlevgl_simulate.lv_examples.lv_apps
$(NAME)_COMPONENTS  += example.littlevgl_simulate.lv_examples.lv_tests
$(NAME)_COMPONENTS  += example.littlevgl_simulate.lv_examples.lv_tutorial
endif