NAME := emwinapp

$(NAME)_SOURCES := emwinapp.c

$(NAME)_SOURCES += app/GUIDEMO.c
$(NAME)_SOURCES += app/GUIDEMO_BarGraph.c
$(NAME)_SOURCES += app/GUIDEMO_ColorBar.c
$(NAME)_SOURCES += app/GUIDEMO_Conf.c
$(NAME)_SOURCES += app/GUIDEMO_Graph.c
$(NAME)_SOURCES += app/GUIDEMO_Intro.c
$(NAME)_SOURCES += app/GUIDEMO_Resource.c
$(NAME)_SOURCES += app/GUIDEMO_Start.c
$(NAME)_SOURCES += app/GUIDEMO_TransparentDialog.c

$(NAME)_SOURCES += core/GUI_init.c

$(NAME)_SOURCES += STemWin/Config/GUIConf.c
$(NAME)_SOURCES += STemWin/Config/LCDConf_FlexColor_Template.c
$(NAME)_SOURCES += STemWin/OS/GUI_X_rhino.c

GLOBAL_DEFINES += AOS_NO_WIFI

$(NAME)_COMPONENTS := yloop cli

ifeq ($(BENCHMARKS),1)
$(NAME)_COMPONENTS  += benchmarks
GLOBAL_DEFINES      += CONFIG_CMD_BENCHMARKS
endif

$(NAME)_PREBUILT_LIBRARY := STemWin/Lib/STemWin540_CM4_OS_GCC_ot.a

$(NAME)_INCLUDES    += core/include/
$(NAME)_INCLUDES    += STemWin/Config/
$(NAME)_INCLUDES    += STemWin/inc/

GLOBAL_CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=hard
GLOBAL_LDFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=hard
