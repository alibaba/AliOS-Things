#ifndef _WIDGET_H
#define _WIDGET_H

//#include <unistd.h>
//#include "amp_system.h"
#include "lvgl.h"
#include "render_public.h"
#include "render_style.h"
#ifdef CONFIG_ENGINE_DUKTAPE
#include "be_inl.h"
#endif
#include "amp_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UI_MOD_STR "AMP_UI"

#define UI_MEM_FREE_CLEAR(addr)       \
    if ((addr) != NULL) {            \
        render_free((addr));         \
        addr = NULL;                 \
    }


typedef enum {
    render_state_task_crt = 0,
    render_state_task_entry,
    render_state_invalid,
}render_state_e;

typedef enum {
    msg_unused = 0,

    msg_vm_to_view,
    msg_view_to_vm,
    msg_vm_to_model,
    msg_model_to_vm,
    msg_list_to_vm,
    msg_list_to_view,

    msg_on_load_trig,
    msg_on_load_ret,

    msg_invalid,
}vm_msg_type_e;

typedef enum {
    vm_unused = 0,
    vm_created,
    vm_to_model,
    vm_to_view,
    vm_invalid,
}vm_data_state_e;


typedef enum {
    vm_data_none = 0,
    vm_data_uint,
    vm_data_int,
    vm_data_double,
    vm_data_bool,
    vm_data_str,
    vm_data_pointer,
    vm_data_type_max,
}vm_data_type_e;

typedef struct _vm_data_type_t {
    vm_data_type_e  type;
    union {
        int           val_none;
        void*         val_pointer;
        uint32_t      val_uint;
        int32_t       val_int;
        double        val_double;
        bool          val_bool;
        char         *val_str;
    };
} vm_data_type_t;

typedef struct _model_array_element_t {
    bool  flag;
    char *name;
    char *full_name;
    char *props;
    vm_data_type_t *data;
}model_array_element_t;

typedef struct _model_array_t {
    char *name;
    int   ele_num;
    int   array_len;
    model_array_element_t *element;
}model_array_t;

typedef enum {
    widget_type_page = 1,
    widget_type_button,
    widget_type_text,
    widget_type_icon,
    widget_type_slider,
    widget_type_bar,
    widget_type_preload,
    widget_type_switch,
    widget_type_roller,
    widget_type_line,
    widget_type_image,
    widget_type_container,
    widget_type_checkbox,
    widget_type_text_area,
    widget_type_qrcode,
	widget_type_list,
    widget_type_list_item,
    widget_type_max
}widget_type_e;

typedef struct _widget_type_map_t {
    widget_type_e   type;
    char            *name;
} widget_type_map_t;


typedef enum {
    widget_clock_hour_angle = 1,
    widget_clock_second_angle,
    widget_clock_minute_angle,
    widget_data_invalid,
}widget_data_e;

typedef struct _slist_class_t {
    char                  *class_name;
    struct _slist_class_t *next;
}slist_class_t;

typedef enum {
    list_parent = 1,
    list_child,
    list_child_rdy,
    list_invalid,
}widget_list_flag_e;

typedef struct _widget_desc_t {
    char                  *id;
    slist_class_t         *class;
    int                    class_num;
    widget_type_e          type;
    int                    state;
    bool                   disabled;
    bool                   timer_flag;
    bool                   sub_flag;
    char                  *data;
    void                  *attribute;
    void                  *custom_style;
    widget_common_style_t *common_style;

#if 0
    widget_style_t        *style;
    uint32_t               style_len;
    int                    cur_style;
    int                    cur_val;
#else
    amp_widget_style_t     widget_style;
#endif

    void                  *object;
    void                  *widget;
    uint32_t               child_num;
    uint32_t               disp_num;
    struct _widget_desc_t *parent;
    list                   child;
    list                   child_display;

    list_node_t            node;
    list_node_t            node_display;

    widget_list_flag_e     array_parent;
    widget_list_flag_e     array_child;
    model_array_t         *array;
    int                    index;
}widget_desc_t;

typedef int (*WIDGST_CREATE_F)(widget_desc_t*);
typedef int (*WIDGST_DELETE_F)(widget_desc_t*);

typedef int (*WIDGST_ATTR_PARSE_F)(widget_desc_t*, char*, char*);

typedef int (*WIDGST_STYLE_INIT_F)(widget_desc_t*);

typedef int (*WIDGST_STYLE_PARSE_F)(widget_style_handle_t *, uint32_t, char*, uint32_t);

typedef int (*WIDGST_CUSTOM_STYLE_PARSE_F)(widget_desc_t *, uint32_t, char*, uint32_t);

typedef int (*WIDGST_UPDATE_F)(widget_desc_t*);
typedef int (*WIDGST_REFRESH_F)(widget_desc_t*);
typedef int (*WIDGST_PAINT_F)(widget_desc_t*);
typedef int (*WIDGST_ERASE_F)(widget_desc_t*);


typedef int (*WIDGST_SET_ATTR_F)(widget_desc_t*,char *, vm_data_type_t *);
typedef int (*WIDGST_SET_TEXT_F)(widget_desc_t*);
typedef int (*WIDGST_SET_TEXT_AREA_F)(widget_desc_t*);

typedef int (*WIDGST_SET_QRCODE_F)(widget_desc_t*);

typedef int (*WIDGST_PARA_COPY_F)(widget_desc_t *dest, widget_desc_t *src);

typedef struct _widget_style_type_map_t {
    int                   type;
    char                 *name;
    WIDGST_STYLE_PARSE_F  parse;
} widget_style_type_map_t;


typedef struct _widget_proc_t {
    bool                  flag;
    widget_type_e         type;

    WIDGST_CREATE_F       create;
    WIDGST_DELETE_F       delete;
    WIDGST_ATTR_PARSE_F   attr_parse;

    WIDGST_STYLE_INIT_F   style_init;
    WIDGST_CUSTOM_STYLE_PARSE_F  style_parse;
    WIDGST_PAINT_F        paint;
    WIDGST_ERASE_F        erase;

    WIDGST_UPDATE_F       update;
    WIDGST_REFRESH_F      refresh;

    WIDGST_SET_ATTR_F   set_attr;
    WIDGST_SET_TEXT_F   set_text;
    WIDGST_SET_TEXT_AREA_F   set_text_area;

    WIDGST_SET_QRCODE_F   set_qrcode;

    WIDGST_PARA_COPY_F  copy;
}widget_proc_t;


typedef enum {
    page_state_free = 0,
    page_state_init,
    page_state_loading,
    page_state_loaded,
    page_state_erase,
    page_state_delete,
}page_stete_e;


typedef struct {
    /* options object */
    int object;
    /* ref of globalData */
    int data;
    /* ref of onShow() */
    int on_show;
    /* ref of onUpdate() */
    int on_update;
    /* ref of onExit() */
    int on_exit;
}page_options_t;


typedef struct _page_desc_t {
    page_stete_e    state;
    int             index;
    char            name[32];
    char            path[64];
    char            css_file[64];
    char            xml_file[64];
    char            js_file[64];
    uint32_t        total_num;
    uint32_t        used_num;
    widget_desc_t  *root_widget;
    widget_desc_t **widgets;
    struct _page_desc_t *next_page;
    css_parse_state_e parse_state;
    list              style_class;
    widget_style_handle_t widget_handle;
    page_options_t  options;
}page_desc_t;

typedef struct _amp_app_desc_t {
    uint32_t        state;
    int             cur_page;
    int             next_page;
    int             num;
    char           *name;
    page_desc_t    *page;
}amp_app_desc_t;


extern bool g_page_active;
extern widget_desc_t **g_widget;
extern uint32_t g_widget_num;
extern uint32_t g_widget_total;
extern widget_desc_t *g_page_root;

widget_desc_t *amp_ui_widget_get(page_desc_t* page, char *id);
int amp_ui_widget_type_get(char *str);

widget_desc_t *amp_ui_widget_creat(page_desc_t* page, widget_type_e type);

widget_proc_t *amp_ui_widget_proc_get(widget_type_e type);

int amp_ui_widget_proc_reg(widget_type_e type, widget_proc_t *proc);

void clock_demo_entry();

int clock_get_seconds_angle();

int clock_get_minutes_angle();

int clock_get_hours_angle();


void amp_bar_proc_init(void);

void amp_button_proc_init(void);

void amp_image_proc_init(void);

void amp_line_proc_init(void);

void amp_preload_proc_init(void);

void amp_roller_proc_init(void);

void amp_slider_proc_init(void);

void amp_switch_proc_init(void);

void amp_text_proc_init(void);

void amp_text_area_proc_init(void);

void amp_qrcode_proc_init(void);

void amp_cont_proc_init(void);

int amp_ui_xml_file_load(page_desc_t* page);

int amp_ui_css_file_load(page_desc_t* page);

int amp_ui_style_refresh(widget_desc_t *widget);

int amp_ui_style_update(page_desc_t* page, widget_desc_t *widget);

int amp_ui_layout(widget_desc_t *root);

int amp_ui_paint(widget_desc_t *root);

int amp_ui_widget_erase(widget_desc_t *widget);

int amp_ui_widget_delete(widget_desc_t *widget);

int render_init(void);

char *amp_bind_para_parse(char *data);

int amp_data_bind(widget_desc_t *widget, char *props, char* value);
int amp_ui_widget_style_init(widget_desc_t* widget);
int amp_ui_widget_style_copy(widget_desc_t* widget,widget_desc_t* src);


extern void page_entry(void *para);
extern void page_exit(void *para);
extern void page_update(void *para);


extern uint64_t g_time_render[10];
extern amp_app_desc_t g_app;


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*_WIDGET_H*/

