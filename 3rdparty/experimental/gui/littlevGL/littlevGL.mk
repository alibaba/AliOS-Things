NAME := littlevGL

$(NAME)_MBINS_TYPE 	    := framework
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=

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
$(NAME)_COMPONENTS  += 3rdparty.experimental.gui.littlevGL.lvgl
endif
