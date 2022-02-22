#
# Component Makefile
#

CFLAGS += -DLV_LVGL_H_INCLUDE_SIMPLE

COMPONENT_SRCDIRS := lv_demos           \
    lv_demos/src/lv_demo_benchmark      \
    lv_demos/src/lv_demo_keypad_encoder \
    lv_demos/src/demo_stress            \
    lv_demos/src/lv_demo_widgets        \
    lv_demos/src/lv_ex_style            \
    lv_demos/src/lv_ex_widgets          \
    lv_demos/assets

COMPONENT_ADD_INCLUDEDIRS := $(COMPONENT_SRCDIRS) .
