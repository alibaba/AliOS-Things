NAME := lvgl

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
GLOBAL_DEFINES      += AOS_LVGL

ifneq ($(IDE),keil)
$(NAME)_COMPONENTS  += framework.GUI.littlevGL.lvgl.lv_core
$(NAME)_COMPONENTS  += framework.GUI.littlevGL.lvgl.lv_draw
$(NAME)_COMPONENTS  += framework.GUI.littlevGL.lvgl.lv_hal
$(NAME)_COMPONENTS  += framework.GUI.littlevGL.lvgl.lv_misc
$(NAME)_COMPONENTS  += framework.GUI.littlevGL.lvgl.lv_misc.lv_fonts
$(NAME)_COMPONENTS  += framework.GUI.littlevGL.lvgl.lv_objx
$(NAME)_COMPONENTS  += framework.GUI.littlevGL.lvgl.lv_themes
endif

