## Zephyr driver for LV bindings project

This driver allows Micropython with LVGL as an external C module to run as a Zephyr OS application; thus, all boards, display, and input drivers can be used from Zephyr's codebase.

### lv_micropython
This driver is integrated for Micropython's Zephyr port build in the [lv_micropython project](https://github.com/lvgl/lv_micropython). Using the lv_micopython Zephyr port with LVGL requires:
* specifying the display and input drivers in the your-board.prj and in the lvgl_driver.h
* editing LV_TICK_CUSTOM_INCLUDE in lv_conf.h as stated below.

### Driver architecture
This driver consist of three files:

#### lvgl.c
This file is taken from the Zephyr codebase and is adjusted for this use case. Display driver initialization is being done here.
#### modlvzephyr.c
This file provides a binding to the lv_init function, which needs to be called before using LVGL from Micropython.
#### lvgl_driver.h
In this file, the display driver and the input driver need to be defined. In addition, the display resolution has to be defined here.

### Building the driver
LV bindings project needs to be included as stated in the main README.
To integrate the Zephyr driver, the following lines have to be inserted in Micropython's Zephyr port:
* Include path of Micropython's build systems have to be added to the Makefile:
```
INC += -I$(ZEPHYR_BASE)
INC += -I$(TOP)/lib/lv_bindings
INC += -I$(TOP)/lib/lv_bindings/driver/zephyr
INC += -I$(ZEPHYR_BASE)/include
INC += -I$(ZEPHYR_BASE)/lib/gui/lvgl/
INC += -I$(TOP)/lib/lv_bindings/lvgl
```

* Driver C files from Zephyr codebase need to be add for the build to the Makefile.
```
SRC_C +=  $(ZEPHYR_BASE)/lib/gui/lvgl/lvgl_display_16bit.c \
    $(ZEPHYR_BASE)/lib/gui/lvgl/lvgl_display_24bit.c \
    $(ZEPHYR_BASE)/lib/gui/lvgl/lvgl_display_32bit.c \
    $(ZEPHYR_BASE)/lib/gui/lvgl/lvgl_display.c \
    $(ZEPHYR_BASE)/lib/gui/lvgl/lvgl_display_mono.c \
    build/lvgl/lv_mpy.c
```
* ```lvgl_driver.h``` has to be adjusted to specific board and display.
* Display driver and input device driver need to be enabled in some project config, e.g., in your board .prj file. The following example is enabling the ELCDIF display driver and FT5336 input driver:
```
CONFIG_DISPLAY_MCUX_ELCDIF=y
CONFIG_DISPLAY=y
CONFIG_MCUX_ELCDIF_PANEL_RK043FN02H=y
CONFIG_KSCAN=y
CONFIG_KSCAN_FT5336=y
```
* Link Zephyr kernel with MPY app, adding following line to Zepjyr's port CMakelists.txt:
```
target_link_libraries(libmicropython INTERFACE kernel)
```
* edit lv_conf.h:
```
#define LV_TICK_CUSTOM_INCLUDE      "kernel.h"
#define LV_TICK_CUSTOM_SYS_TIME_EXPR    (k_uptime_get_32())

typedef void *lv_disp_drv_user_data_t;
typedef void *lv_indev_drv_user_data_t;

```

#### Usage
LVGL and driver initialization is following:
```
import lvgl as lv
import ZDD
ZDD.init()
```
