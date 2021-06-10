/**
 * @file lvgl.h
 * Include all LittleV GL related headers
 */

#ifndef LV_H
#define LV_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

//#include "aui_version.h"

#include "../../../../littlevgl/src/lv_draw/lv_draw_img.h"
#include "../../../../littlevgl/src/lv_misc/lv_log.h"
#include "../../../../littlevgl/src/lv_misc/lv_task.h"
#include "../../../../littlevgl/src/lv_misc/lv_fs.h"
#include "../../../../littlevgl/src/lv_misc/lv_color.h"
#include "../../../../littlevgl/src/lv_misc/lv_math.h"

#include "../../../../littlevgl/src/lv_font/lv_font_fmt_txt.h"

#include "../../../../littlevgl/src/lv_hal/lv_hal.h"

#include "../../../../littlevgl/src/lv_core/lv_obj.h"
#include "../../../../littlevgl/src/lv_core/lv_group.h"
//#include "../../../../littlevgl/src/lv_core/lv_lang.h"
#include "../../../../littlevgl/src/lv_core/lv_style.h"
#include "../../../../littlevgl/src/lv_core/lv_refr.h"

#include "../../../../littlevgl/src/lv_themes/lv_theme.h"

#include "../../../../littlevgl/src/lv_objx/lv_btn.h"
#include "../../../../littlevgl/src/lv_objx/lv_imgbtn.h"
#include "../../../../littlevgl/src/lv_objx/lv_img.h"
#include "../../../../littlevgl/src/lv_objx/lv_label.h"
#include "../../../../littlevgl/src/lv_objx/lv_line.h"
#include "../../../../littlevgl/src/lv_objx/lv_page.h"
#include "../../../../littlevgl/src/lv_objx/lv_cont.h"
#include "../../../../littlevgl/src/lv_objx/lv_list.h"
#include "../../../../littlevgl/src/lv_objx/lv_chart.h"
#include "../../../../littlevgl/src/lv_objx/lv_table.h"
#include "../../../../littlevgl/src/lv_objx/lv_cb.h"
#include "../../../../littlevgl/src/lv_objx/lv_bar.h"
#include "../../../../littlevgl/src/lv_objx/lv_slider.h"
#include "../../../../littlevgl/src/lv_objx/lv_led.h"
#include "../../../../littlevgl/src/lv_objx/lv_btnm.h"
#include "../../../../littlevgl/src/lv_objx/lv_kb.h"
#include "../../../../littlevgl/src/lv_objx/lv_ddlist.h"
#include "../../../../littlevgl/src/lv_objx/lv_roller.h"
#include "../../../../littlevgl/src/lv_objx/lv_ta.h"
#include "../../../../littlevgl/src/lv_objx/lv_canvas.h"
#include "../../../../littlevgl/src/lv_objx/lv_win.h"
#include "../../../../littlevgl/src/lv_objx/lv_tabview.h"
#include "../../../../littlevgl/src/lv_objx/lv_tileview.h"
#include "../../../../littlevgl/src/lv_objx/lv_mbox.h"
#include "../../../../littlevgl/src/lv_objx/lv_gauge.h"
#include "../../../../littlevgl/src/lv_objx/lv_lmeter.h"
#include "../../../../littlevgl/src/lv_objx/lv_sw.h"
//#include "aui_objx/aui_kb.h"
#include "../../../../littlevgl/src/lv_objx/lv_arc.h"
#include "../../../../littlevgl/src/lv_objx/lv_preload.h"
#include "../../../../littlevgl/src/lv_objx/lv_calendar.h"
#include "../../../../littlevgl/src/lv_objx/lv_spinbox.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
}
#endif

#endif /*LV_H*/
