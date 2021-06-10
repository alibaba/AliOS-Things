#ifndef _WIDGET_STYLE_H
#define _WIDGET_STYLE_H

#include "render_public.h"

#ifdef __cplusplus
extern "C" {
#endif
#define STYLE_MAX_CLASS_SELECTOR_ONCE    32

typedef enum {
    //css_parse_start,
    css_parse_selector_start,
    css_parse_selector_id,

    css_parse_selector_class_flag,
    css_parse_selector_class,

    css_parse_property,
    css_parse_value,
    css_parse_finish,
    css_parse_invalid,
}css_parse_state_e;

typedef enum {
    style_width,
    style_height,
    style_top,
    style_left,
    style_align,

    style_main_color,
    style_grad_color,
    style_radius,
    style_opacity,
    style_border_color,
    style_border_width,
    style_border_part,
    style_border_opacity,

    style_shadow_color,
    style_shadow_width,
    style_shadow_type,


    style_padding_ver,
    style_padding_hor,
//    style_padding_left,
//    style_padding_right,
    style_padding_inner,

    style_text_size,
    style_text_color,
    style_text_sel_color,
    style_text_font,
    style_text_letter_space,
    style_text_line_space,
    style_text_opacity,
    style_text_align,

    style_image_color,
    style_image_intense,
    style_image_opacity,


    style_line_color,
    style_line_width,
    style_line_opacity,
    style_line_type,

    style_position,
    style_z_index,

    style_max,
}widget_style_type_e;


typedef enum {
    style_data_none = 0,
    style_data_uint,
    style_data_int,
    style_data_float,
    style_data_bool,
    style_data_str,
    style_data_color,

    style_data_type_max,
}widget_data_type_e;


typedef enum {
    selector_none = 0,
    selector_id,
    selector_class,
    selector_max,
}widget_style_selector_e;


typedef enum {
    position_static,
    position_fix,
    position_absolute,
    position_invalid,
}widget_position_e;

typedef struct _widget_style_t {
    widget_style_selector_e selector;
    widget_data_type_e  type;
    union {
        int           val_none;
        uint32_t      val_uint;
        int32_t       val_int;
        float         val_float;
        bool          val_bool;
        char         *str;
        widget_color  val_color;
    };
} widget_style_t;


typedef struct _widget_user_style_t {
    widget_style_t *sheet;
    uint32_t        length;
} widget_user_style_t;

typedef struct _widget_basic_style_t {
    uint16_t   width;
    uint16_t   height;
    uint16_t   top;
    uint16_t   left;
    uint16_t   align;
    uint16_t   text_align;

    int        z_index;
    widget_position_e position;
}widget_basic_style_t;

typedef struct _widget_common_style_t {
    widget_basic_style_t basic_style;
    lv_style_t           style;
}widget_common_style_t;

typedef struct _amp_widget_style_t {
    widget_style_t        *style;
    bool                   flag;
    uint32_t               style_len;
    int                    cur_style;
    int                    cur_val;
    char                  *class;
}amp_widget_style_t;

typedef struct _widget_style_class_t {
    amp_widget_style_t     widget_style;
    list_node_t            node;
}widget_style_class_t;

typedef struct _widget_style_handle_t {
    amp_widget_style_t   *handle[STYLE_MAX_CLASS_SELECTOR_ONCE];
    uint32_t              num;
}widget_style_handle_t;


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*_WIDGET_STYLE_H*/

