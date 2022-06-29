/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */
#ifndef HAAS_UI_DEF_H
#define HAAS_UI_DEF_H

/**@enum UIFrameworkType_t
* @brief UI Framework Type \n
*/
typedef enum {
    UI_FRAMEWORK_NONE        = -1,
    UI_FRAMEWORK_GREENUI     = 0,
    UI_FRAMEWORK_FB          = 1,
    UI_FRAMEWORK_LVGL        = 2,
    UI_FRAMEWORK_SDL         = 3,
    UI_FRAMEWORK_MAX         = 30,
} UIFrameworkType_t;
#endif
