NAME := lv_tutorial

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
GLOBAL_DEFINES      += AOS_LV_TUTORIAL

ifneq ($(IDE),keil)
$(NAME)_COMPONENTS  += example.littlevgl_simulate.lv_examples.lv_tutorial.0_porting
$(NAME)_COMPONENTS  += example.littlevgl_simulate.lv_examples.lv_tutorial.1_hello_world
$(NAME)_COMPONENTS  += example.littlevgl_simulate.lv_examples.lv_tutorial.2_objects
$(NAME)_COMPONENTS  += example.littlevgl_simulate.lv_examples.lv_tutorial.3_styles
$(NAME)_COMPONENTS  += example.littlevgl_simulate.lv_examples.lv_tutorial.4_themes
$(NAME)_COMPONENTS  += example.littlevgl_simulate.lv_examples.lv_tutorial.5_antialiasing
$(NAME)_COMPONENTS  += example.littlevgl_simulate.lv_examples.lv_tutorial.6_images
$(NAME)_COMPONENTS  += example.littlevgl_simulate.lv_examples.lv_tutorial.7_fonts
$(NAME)_COMPONENTS  += example.littlevgl_simulate.lv_examples.lv_tutorial.8_animations
$(NAME)_COMPONENTS  += example.littlevgl_simulate.lv_examples.lv_tutorial.9_responsive
$(NAME)_COMPONENTS  += example.littlevgl_simulate.lv_examples.lv_tutorial.10_keyboard
endif