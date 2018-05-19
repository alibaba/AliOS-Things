NAME := littlevGL

$(NAME)_TYPE 	    := framework

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),armcc)
GLOBAL_DEFINES      += __BSD_VISIBLE
endif

GLOBAL_INCLUDES     += .
GLOBAL_DEFINES      += AOS_LITTLEVGL

ifneq ($(IDE),keil)
$(NAME)_COMPONENTS  += framework.GUI.littlevGL.lvgl
endif
