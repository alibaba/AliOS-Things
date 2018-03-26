NAME := starterkitgui

$(NAME)_SOURCES := starterkitgui.c
$(NAME)_SOURCES += st7789.c

$(NAME)_SOURCES += app/GUIDEMO.c
$(NAME)_SOURCES += app/GUIDEMO_BarGraph.c
$(NAME)_SOURCES += app/GUIDEMO_ColorBar.c
$(NAME)_SOURCES += app/GUIDEMO_Conf.c
$(NAME)_SOURCES += app/GUIDEMO_Graph.c
$(NAME)_SOURCES += app/GUIDEMO_Intro.c
$(NAME)_SOURCES += app/GUIDEMO_Resource.c
$(NAME)_SOURCES += app/GUIDEMO_Start.c
$(NAME)_SOURCES += app/GUIDEMO_Uncalssified.c
$(NAME)_SOURCES += app/GUIDEMO_TransparentDialog.c

$(NAME)_SOURCES += core/GUI_init.c

$(NAME)_SOURCES += STemWin/Config/GUIConf.c
$(NAME)_SOURCES += STemWin/Config/LCDConf_FlexColor_Template.c
$(NAME)_SOURCES += STemWin/OS/GUI_X_rhino.c


$(NAME)_COMPONENTS := yloop cli

ifeq ($(BENCHMARKS),1)
$(NAME)_COMPONENTS  += benchmarks
GLOBAL_DEFINES      += CONFIG_CMD_BENCHMARKS
endif

ifeq ($(COMPILER),armcc)
 $(NAME)_PREBUILT_LIBRARY := STemWin/Lib/STemWin540_CM4_OS_Keil_ot.lib
else ifeq ($(COMPILER),iar)

else
 $(NAME)_PREBUILT_LIBRARY := STemWin/Lib/STemWin540_CM4_OS_GCC_ot.a
endif

$(NAME)_INCLUDES    += core/include/
$(NAME)_INCLUDES    += STemWin/Config/
$(NAME)_INCLUDES    += STemWin/inc/
$(NAME)_INCLUDES    += .
