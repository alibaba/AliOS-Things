#ifdef AMP_ADVANCED_ADDON_UI

#include "model_bind.h"

#define MODEL_BUFF_LEN 256

#define MODEL_STR_HEAD       "var model = require('model'); model.setData({"
#define MODEL_STR_DELIMITER  ":"
#define MODEL_STR_TAIL       "});"

static model_bind_proc_t g_model_bind_proc = {0};
vm_msg_type_e  g_msg_type = msg_invalid;

int model_data_bind_init(MODEL_DATA_SEND_TO_VM_F send)
{
    if (send == NULL) {
        amp_error(UI_MOD_STR, "data bind cb is null");
        return -1;
    }

    g_model_bind_proc.send   = send;
    g_model_bind_proc.flag   = true;

    return 0;
}

int model_data_bind_deinit(void) {
    memset(&g_model_bind_proc, 0, sizeof(g_model_bind_proc));
    return 0;
}

int model_data_send_to_vm(vm_msg_t *msg, int len)
{

    if (msg == NULL) {
        amp_error(UI_MOD_STR, "msg is null");
        return -1;
    }

    if (len == 0) {
        amp_error(UI_MOD_STR, "len is %d", len);
        return -1;
    }

    if (g_model_bind_proc.flag != true) {
        amp_error(UI_MOD_STR, "model bind proc is uninited");
        return -1;
    }

    if (g_model_bind_proc.send == NULL) {
        amp_error(UI_MOD_STR, "model bind send cb is null");
        return -1;
    }

    return g_model_bind_proc.send(msg, len);
}

JSValue native_model_setdata(JSContext *ctx, JSValueConst this_val,
                          int argc, JSValueConst *argv)
{
    vm_data_type_t data;
    vm_msg_t msg;
    int arr_idx = 0;
    int len = 0;
    int i, j;
    model_array_t *array;
    widget_desc_t* widget = NULL;
    page_desc_t *page;

    if (!JS_IsObject(argv[0]))
    {
        amp_error(UI_MOD_STR, "ctx is invalid, %d", ctx);
        goto done;
    }

    duk_enum(ctx, 0 ,0);

    page = &g_app.page[g_app.cur_page];

    if (page == NULL) {
        goto done;
    }

    while (duk_next(ctx, -1, 1))
    {

        if (!duk_is_string(ctx, -2)) {
            amp_error(UI_MOD_STR, "para name is not string");
            duk_pop_2(ctx);
            continue;
        }
        msg.type = msg_model_to_vm;
        memset(&data, 0, sizeof(data));

        if (duk_is_string(ctx, -1)) {
            data.val_str = duk_safe_to_string(ctx, -1);
            data.type = vm_data_str;
            amp_debug(UI_MOD_STR, "para name: %s, str para: %s", duk_safe_to_string(ctx, -2), duk_safe_to_string(ctx, -1));
        }
        else if (duk_is_boolean(ctx, -1)) {

            data.val_bool = duk_to_boolean(ctx, -1);
            data.type = vm_data_bool;
            amp_debug(UI_MOD_STR, "para name: %s, bool para: %d", duk_safe_to_string(ctx, -2), duk_to_boolean(ctx, -1));
        }

        else if (duk_is_number(ctx, -1)) {

            data.val_double = duk_to_number(ctx, -1);
            data.type = vm_data_double;

            amp_debug(UI_MOD_STR, "para name: %s, float para: %f", duk_safe_to_string(ctx, -2), duk_to_boolean(ctx, -1));
        }
        else if (duk_is_array(ctx, -1)) {
            arr_idx = duk_normalize_index(ctx, -1);
            len = duk_get_length(ctx, arr_idx);

            for (i = 0; i < page->total_num; i++) {
                if(page->widgets[i] == NULL) {
                    continue;
                }
                if (page->widgets[i]->array_parent != list_parent) {
                    continue;
                }

                if (page->widgets[i]->array == NULL) {
                    continue;
                }

                if (widget_str_equal(page->widgets[i]->array->name, duk_safe_to_string(ctx, -2))) {
                    array = page->widgets[i]->array;
                    widget = page->widgets[i];
                    break;
                }
            }
            if (i ==  page->total_num) {
                continue;
            }

            for(j = 0; j < array->ele_num; j++) {
                array->array_len = duk_get_length(ctx, arr_idx);
                array->element[j].data = aos_malloc(sizeof(vm_data_type_t) * array->array_len);
                if (array->element[j].data == NULL) {
                    amp_error(UI_MOD_STR, "array->element->data aos_malloc fail,array->array_len =  %d", array->array_len);

                    amp_error(UI_MOD_STR, "array->element->data aos_malloc fail,size =  %d", sizeof(vm_data_type_t) * array->array_len);
                    goto done;
                }
            }

            for (i = 0; i < len; i++) {
                duk_get_prop_index(ctx, arr_idx, i);
                if (!duk_is_object(ctx, -1)) {
                    amp_warn(UI_MOD_STR, "data is not number, index: %d", i);
                    duk_pop(ctx);
                }
                else {
                    amp_warn(UI_MOD_STR, "object, index: %d", i);
                }


                for (j = 0; j < array->ele_num; j++) {

                    duk_get_prop_string(ctx, -1, array->element[j].name);
                    amp_warn(UI_MOD_STR, "array->element[%d].data[%d].val_str: %s",j, i,  duk_safe_to_string(ctx, -1));

                    if (duk_is_string(ctx, -1))
                    {
                        array->element[j].data[i].val_str = duk_safe_to_string(ctx, -1);
                        array->element[j].data[i].type = vm_data_str;
                        amp_warn(UI_MOD_STR, "data is not number, array->element[%d].data[%d].val_str: %s",j, i, array->element[j].data[i].val_str);
                    }
                    else if (duk_is_number(ctx, -1))
                    {
                        array->element[j].data[i].val_double = duk_to_number(ctx, -1);
                        array->element[j].data[i].type = vm_data_double;
                    }

                    duk_pop(ctx);

                }

            }

            msg.type = msg_list_to_vm;
            //data.type = vm_data_pointer;
            msg.payload.widget = widget;

        }
        else {
            continue;
        }

        msg.payload.name = duk_safe_to_string(ctx, -2);
        memcpy(&msg.payload.data, &data, sizeof(data));

        model_data_send_to_vm(&msg, sizeof(msg));
        if (msg_invalid == g_msg_type) {
            msg.type = msg_on_load_ret;
            model_data_send_to_vm(&msg, sizeof(msg));
            g_msg_type = msg_on_load_ret;
        }

        duk_pop_2(ctx);
    }

    amp_debug(UI_MOD_STR, "native_model_setdata success");

done:
    return 1;
}

int model_eval_str(char *name, vm_data_type_t* data)
{
    JSContext *ctx = NULL;
    char         buffer[MODEL_BUFF_LEN];

    if (name == NULL) {
        amp_error(UI_MOD_STR, "name is null");
        return -1;
    }

    if (data == NULL) {
        amp_error(UI_MOD_STR, "name is null");
        return -1;
    }

    ctx = js_get_context();

    switch(data->type) {
        case vm_data_uint:
            memset(buffer, 0, sizeof(buffer));
            snprintf(buffer, MODEL_BUFF_LEN - 1, "%s%s%s%u%s", MODEL_STR_HEAD, name, MODEL_STR_DELIMITER, data->val_uint, MODEL_STR_TAIL);

            duk_eval_string(ctx, buffer);
            amp_debug(UI_MOD_STR, "name: %s, code: %f", name, buffer);
            break;
        case vm_data_int:
            memset(buffer, 0, sizeof(buffer));
            snprintf(buffer, MODEL_BUFF_LEN - 1, "%s%s%s%d%s", MODEL_STR_HEAD, name, MODEL_STR_DELIMITER, data->val_int, MODEL_STR_TAIL);

            duk_eval_string(ctx, buffer);
            amp_debug(UI_MOD_STR, "name: %s, code: %f", name, buffer);
            break;

        case vm_data_double:
            memset(buffer, 0, sizeof(buffer));
            snprintf(buffer, MODEL_BUFF_LEN - 1, "%s%s%s%f%s", MODEL_STR_HEAD, name, MODEL_STR_DELIMITER, data->val_double, MODEL_STR_TAIL);

            duk_eval_string(ctx, buffer);
            amp_debug(UI_MOD_STR, "name: %s, code: %f", name, buffer);
            break;

        case vm_data_str:
            memset(buffer, 0, sizeof(buffer));
            snprintf(buffer, MODEL_BUFF_LEN - 1, "%s%s%s%s%s", MODEL_STR_HEAD, name, MODEL_STR_DELIMITER, data->val_str, MODEL_STR_TAIL);

            duk_eval_string(ctx, buffer);
            amp_debug(UI_MOD_STR, "name: %s, code: %f", name, buffer);
            break;

        case vm_data_bool:
            memset(buffer, 0, sizeof(buffer));
            snprintf(buffer, MODEL_BUFF_LEN - 1, "%s%s%s%s%s", MODEL_STR_HEAD, name, MODEL_STR_DELIMITER, data->val_bool == 0 ? "false" : "true", MODEL_STR_TAIL);

            duk_eval_string(ctx, buffer);
            amp_debug(UI_MOD_STR, "name: %s, code: %f", name, buffer);
            break;

        default :
            break;
    }

    return 0;

}


#endif