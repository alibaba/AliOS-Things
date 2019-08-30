#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "be_module.h"
#include "jse_common.h"

#include <k_api.h>
#include "lvgl.h"

static be_jse_symbol_t *gui_set_size(void)
{
    be_jse_symbol_t *arg0 = NULL;
    be_jse_symbol_t *arg1 = NULL;
    be_jse_symbol_t *arg2 = NULL;

    be_jse_handle_function(0, &arg0, &arg1, &arg2, NULL);
    if (!arg0 || !symbol_is_int(arg0) || !arg1 || !symbol_is_int(arg1) ||
        !arg2 || !symbol_is_int(arg2)) {
        symbol_unlock(arg0);
        symbol_unlock(arg1);
        symbol_unlock(arg2);
        return new_int_symbol(-1);
    }

    lv_obj_t *obj = (lv_obj_t *)get_symbol_value_int(arg0);
    int w         = get_symbol_value_int(arg1);
    int h         = get_symbol_value_int(arg2);
    // jse_debug("obj:%p, w:%d, h:%d\r\n", obj, w, h);
    lv_obj_set_size(obj, w, h);

    symbol_unlock(arg0);
    symbol_unlock(arg1);
    symbol_unlock(arg2);
    return new_int_symbol(1);
}

static be_jse_symbol_t *gui_set_width(void)
{
    be_jse_symbol_t *arg0 = NULL;
    be_jse_symbol_t *arg1 = NULL;

    be_jse_handle_function(0, &arg0, &arg1, NULL, NULL);
    if (!arg0 || !symbol_is_int(arg0) || !arg1 || !symbol_is_int(arg1)) {
        symbol_unlock(arg0);
        symbol_unlock(arg1);
        return new_int_symbol(-1);
    }

    lv_obj_t *obj = (lv_obj_t *)get_symbol_value_int(arg0);
    int width     = get_symbol_value_int(arg1);
    // jse_debug("obj:%p, width:%d\r\n", obj, width);
    lv_obj_set_width(obj, width);

    symbol_unlock(arg0);
    symbol_unlock(arg1);
    return new_int_symbol(1);
}

lv_style_t *_symbol_style_parser(be_jse_symbol_t *arg)
{
    /* body */
    be_jse_symbol_t *body_radius_symbol     = NULL;
    be_jse_symbol_t *body_main_color_symbol = NULL;
    be_jse_symbol_t *body_grad_color_symbol = NULL;
    be_jse_symbol_t *body_opa_symbol        = NULL;
    /* body.border */
    be_jse_symbol_t *body_border_color_symbol = NULL;
    be_jse_symbol_t *body_border_width_symbol = NULL;
    be_jse_symbol_t *body_border_opa_symbol   = NULL;
    /* body.shadow */
    be_jse_symbol_t *body_shadow_color_symbol = NULL;
    be_jse_symbol_t *body_shadow_width_symbol = NULL;
    be_jse_symbol_t *body_shadow_type_symbol  = NULL;

    /* TODO: may cause memery overflow */
    lv_style_t *style = (lv_style_t *)jse_calloc(1, sizeof(lv_style_t));

    char color[8] = {0}, R = 0, G = 0, B = 0;
    int i = 0;

    /* set to default style */
    lv_style_copy(style, &lv_style_pretty_color);

    /* body.radius */
    body_radius_symbol = unlock_symbol_value(lookup_named_child_symbol(
        get_symbol_node_id(arg), "bodyRadius", false));
    if (body_radius_symbol != NULL) {
        if (!symbol_is_int(body_radius_symbol)) {
            goto err;
        }
        style->body.radius = get_symbol_value_int(body_radius_symbol);
    }

    /* body.main_color */
    body_main_color_symbol = unlock_symbol_value(lookup_named_child_symbol(
        get_symbol_node_id(arg), "bodyMainColor", false));
    if (body_main_color_symbol != NULL) {
        if (!symbol_is_string(body_main_color_symbol)) {
            jse_error("body.main_color wrong type\r\n");
            goto err;
        }
        if (symbol_str_len(body_main_color_symbol) != 7) {
            jse_error("body.main_color wrong size\r\n");
            goto err;
        }

        symbol_to_str(body_main_color_symbol, color, 7);
        if (color[0] != '#') {
            jse_error("body.main_color wrong format\r\n");
            goto err;
        }
        R = hex2num(color[1]) * 16 + hex2num(color[2]);
        G = hex2num(color[3]) * 16 + hex2num(color[4]);
        B = hex2num(color[5]) * 16 + hex2num(color[6]);
        // jse_debug("shadow.color:%s, R:%02X, G:%02X, B:%02X\r\n", color, R, G,
        // B);

        style->body.main_color = LV_COLOR_MAKE(R, G, B);
    }

    /* body.grad_color */
    body_grad_color_symbol = unlock_symbol_value(lookup_named_child_symbol(
        get_symbol_node_id(arg), "bodyGradColor", false));
    if (body_grad_color_symbol != NULL) {
        if (!symbol_is_string(body_grad_color_symbol)) {
            jse_error("body.grad_color wrong type\r\n");
            goto err;
        }
        if (symbol_str_len(body_grad_color_symbol) != 7) {
            jse_error("body.grad_color wrong size\r\n");
            goto err;
        }

        symbol_to_str(body_grad_color_symbol, color, 7);
        if (color[0] != '#') {
            jse_error("body.grad_color wrong format\r\n");
            goto err;
        }
        R = hex2num(color[1]) * 16 + hex2num(color[2]);
        G = hex2num(color[3]) * 16 + hex2num(color[4]);
        B = hex2num(color[5]) * 16 + hex2num(color[6]);
        // jse_debug("shadow.color:%s, R:%02X, G:%02X, B:%02X\r\n", color, R, G,
        //           B);

        style->body.grad_color = LV_COLOR_MAKE(R, G, B);
    }

    /* body.opa */
    body_opa_symbol = unlock_symbol_value(
        lookup_named_child_symbol(get_symbol_node_id(arg), "bodyOpa", false));
    if (body_opa_symbol != NULL) {
        if (!symbol_is_int(body_opa_symbol)) {
            goto err;
        }
        style->body.opa = get_symbol_value_int(body_opa_symbol);
    }

    /* body.border.color */
    body_border_color_symbol = unlock_symbol_value(lookup_named_child_symbol(
        get_symbol_node_id(arg), "bodyBorderColor", false));
    if (body_border_color_symbol != NULL) {
        if (!symbol_is_string(body_border_color_symbol)) {
            jse_error("body.border.color wrong type\r\n");
            goto err;
        }
        if (symbol_str_len(body_border_color_symbol) != 7) {
            jse_error("body.border.color wrong size\r\n");
            goto err;
        }

        symbol_to_str(body_border_color_symbol, color, 7);
        if (color[0] != '#') {
            jse_error("body.border.color wrong format\r\n");
            goto err;
        }
        R = hex2num(color[1]) * 16 + hex2num(color[2]);
        G = hex2num(color[3]) * 16 + hex2num(color[4]);
        B = hex2num(color[5]) * 16 + hex2num(color[6]);
        // jse_debug("shadow.color:%s, R:%02X, G:%02X, B:%02X\r\n", color, R, G,
        // B);

        style->body.border.color = LV_COLOR_MAKE(R, G, B);
    }

    /* body.border.width */
    body_border_width_symbol = unlock_symbol_value(lookup_named_child_symbol(
        get_symbol_node_id(arg), "bodyBorderWidth", false));
    if (body_border_width_symbol != NULL) {
        if (!symbol_is_int(body_border_width_symbol)) {
            goto err;
        }
        style->body.border.width =
            get_symbol_value_int(body_border_width_symbol);
    }

    /* body.border.opa */
    body_border_opa_symbol = unlock_symbol_value(lookup_named_child_symbol(
        get_symbol_node_id(arg), "bodyBorderOpa", false));
    if (body_border_opa_symbol != NULL) {
        if (!symbol_is_int(body_border_opa_symbol)) {
            goto err;
        }
        style->body.border.opa = get_symbol_value_int(body_border_opa_symbol);
    }

    /* body.shadow.color */
    body_shadow_color_symbol = unlock_symbol_value(lookup_named_child_symbol(
        get_symbol_node_id(arg), "bodyShadowColor", false));
    if (body_shadow_color_symbol != NULL) {
        if (!symbol_is_string(body_shadow_color_symbol)) {
            jse_error("body.shadow.color wrong type\r\n");
            goto err;
        }
        if (symbol_str_len(body_shadow_color_symbol) != 7) {
            jse_error("body.shadow.color wrong size\r\n");
            goto err;
        }

        symbol_to_str(body_shadow_color_symbol, color, 7);
        if (color[0] != '#') {
            jse_error("body.shadow.color wrong format\r\n");
            goto err;
        }

        R = hex2num(color[1]) * 16 + hex2num(color[2]);
        G = hex2num(color[3]) * 16 + hex2num(color[4]);
        B = hex2num(color[5]) * 16 + hex2num(color[6]);
        // jse_debug("shadow.color:%s, R:%02X, G:%02X, B:%02X\r\n", color, R, G,
        // B);
        style->body.shadow.color = LV_COLOR_MAKE(R, G, B);
    }

    /* body.shadow.width */
    body_shadow_width_symbol = unlock_symbol_value(lookup_named_child_symbol(
        get_symbol_node_id(arg), "bodyShadowWidth", false));
    if (body_shadow_width_symbol != NULL) {
        if (!symbol_is_int(body_shadow_width_symbol)) {
            goto err;
        }
        style->body.shadow.width =
            get_symbol_value_int(body_shadow_width_symbol);
    }

    /* body.shadow.type */
    body_shadow_type_symbol = unlock_symbol_value(lookup_named_child_symbol(
        get_symbol_node_id(arg), "bodyShadowType", false));
    if (body_shadow_type_symbol != NULL) {
        if (!symbol_is_int(body_shadow_type_symbol)) {
            goto err;
        }
        style->body.shadow.type = get_symbol_value_int(body_shadow_type_symbol);
    }

    /* body */
    symbol_unlock(body_radius_symbol);
    symbol_unlock(body_main_color_symbol);
    symbol_unlock(body_grad_color_symbol);
    symbol_unlock(body_opa_symbol);
    /* body.border */
    symbol_unlock(body_border_color_symbol);
    symbol_unlock(body_border_width_symbol);
    symbol_unlock(body_border_opa_symbol);
    /* body.shadow */
    symbol_unlock(body_shadow_color_symbol);
    symbol_unlock(body_shadow_width_symbol);
    symbol_unlock(body_shadow_type_symbol);
    return style;
err:
    /* body */
    symbol_unlock(body_radius_symbol);
    symbol_unlock(body_main_color_symbol);
    symbol_unlock(body_grad_color_symbol);
    symbol_unlock(body_opa_symbol);
    /* body.border */
    symbol_unlock(body_border_color_symbol);
    symbol_unlock(body_border_width_symbol);
    symbol_unlock(body_border_opa_symbol);
    /* body.shadow */
    symbol_unlock(body_shadow_color_symbol);
    symbol_unlock(body_shadow_width_symbol);
    symbol_unlock(body_shadow_type_symbol);
    jse_free(style);
    return NULL;
}

be_jse_symbol_t *gui_set_style(void)
{
    be_jse_symbol_t *arg0 = NULL;
    be_jse_symbol_t *arg1 = NULL;
    lv_style_t *style     = NULL;

    be_jse_handle_function(0, &arg0, &arg1, NULL, NULL);
    if (!arg0 || !symbol_is_int(arg0) || !arg1 || !symbol_is_object(arg1)) {
        symbol_unlock(arg0);
        symbol_unlock(arg1);
        return new_int_symbol(-1);
    }

    lv_obj_t *obj = (lv_obj_t *)get_symbol_value_int(arg0);

    if ((style = _symbol_style_parser(arg1)) == NULL) {
        symbol_unlock(arg0);
        symbol_unlock(arg1);
        return new_int_symbol(-1);
    }

    lv_obj_set_style(obj, style);

    symbol_unlock(arg0);
    symbol_unlock(arg1);
    return new_int_symbol(1);
}

static be_jse_symbol_t *gui_align(void)
{
    be_jse_symbol_t *arg0 = NULL;
    be_jse_symbol_t *arg1 = NULL;

    be_jse_handle_function(0, &arg0, &arg1, NULL, NULL);
    if (!arg0 || !symbol_is_int(arg0) || !arg1 || !symbol_is_object(arg1)) {
        symbol_unlock(arg0);
        symbol_unlock(arg1);
        return new_int_symbol(-1);
    }

    lv_obj_t *obj                = (lv_obj_t *)get_symbol_value_int(arg0);
    be_jse_symbol_t *base_symbol = unlock_symbol_value(
        lookup_named_child_symbol(get_symbol_node_id(arg1), "base", false));
    be_jse_symbol_t *align_symbol = unlock_symbol_value(
        lookup_named_child_symbol(get_symbol_node_id(arg1), "align", false));
    be_jse_symbol_t *x_symbol = unlock_symbol_value(
        lookup_named_child_symbol(get_symbol_node_id(arg1), "x", false));
    be_jse_symbol_t *y_symbol = unlock_symbol_value(
        lookup_named_child_symbol(get_symbol_node_id(arg1), "y", false));

    if (align_symbol == NULL || x_symbol == NULL || y_symbol == NULL) {
        symbol_unlock(align_symbol);
        symbol_unlock(x_symbol);
        symbol_unlock(y_symbol);
        symbol_unlock(arg0);
        symbol_unlock(arg1);

        jse_error("align or x or y is null");
        return new_int_symbol(-1);
    }

    lv_obj_t *base = (lv_obj_t *)get_symbol_value_int(base_symbol);
    int align      = get_symbol_value_int(align_symbol);
    int x          = get_symbol_value_int(x_symbol);
    int y          = get_symbol_value_int(y_symbol);
    // jse_debug("obj:%p, base:%p, x:%d, y:%d\r\n", obj, base, x, y);

    lv_obj_align(obj, base, align, x, y);

    symbol_unlock(arg0);
    symbol_unlock(arg1);
    return new_int_symbol(1);
}

be_jse_symbol_t *gui_obj_handle_cb(be_jse_vm_ctx_t *execInfo,
                                   be_jse_symbol_t *var, const char *name)
{
    if (0 == strcmp(name, "setSize")) {
        return gui_set_size();
    }
    if (0 == strcmp(name, "setWidth")) {
        return gui_set_width();
    }
    if (0 == strcmp(name, "setStyle")) {
        return gui_set_style();
    }
    if (0 == strcmp(name, "align")) {
        return gui_align();
    }

    if (0 == strcmp(name, "ALIGN_CENTER")) {
        lexer_token_cmp(execInfo->lex, BE_TOKEN_ID);
        return new_int_symbol(LV_ALIGN_CENTER);
    }
    if (0 == strcmp(name, "RADIUS_CIRCLE")) {
        lexer_token_cmp(execInfo->lex, BE_TOKEN_ID);
        return new_int_symbol(LV_RADIUS_CIRCLE);
    }
    return NULL;
}