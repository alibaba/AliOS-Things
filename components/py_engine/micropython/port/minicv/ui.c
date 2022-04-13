#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "k_api.h"
#include "HaasLog.h"
#include "WrapperIHaasUI.h"

extern const mp_obj_type_t minicv_ui_type;
// this is the actual C-structure for our new object
typedef struct
{
    // base represents some basic information, like type
    mp_obj_base_t Base;
    // a member created by us
    char *ModuleName;
    UIFrameworkType_t  mType;
    void               *mInstance;
} mp_ui_obj_t;

void ui_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    LOG_D("entern %s;\n", __func__);
    mp_ui_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ModuleName(%s)", self->ModuleName);
}

STATIC mp_obj_t ui_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    LOG_D("entern  %s;\n", __func__);
    mp_ui_obj_t* driver_obj = m_new_obj(mp_ui_obj_t);
    if (!driver_obj) {
        mp_raise_OSError(ENOMEM);
    }

    driver_obj->Base.type = &minicv_ui_type;
    driver_obj->ModuleName = "minicv-UI";
    driver_obj->mType = UI_FRAMEWORK_NONE;
    driver_obj->mInstance = NULL;

    return MP_OBJ_FROM_PTR(driver_obj);
}

STATIC mp_obj_t obj_open(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_ui_obj_t* driver_obj = (mp_ui_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    if (driver_obj->mInstance != NULL)
    {
        LOG_E("Module has been opened, please clode first\n");
        return mp_const_none;
    }

    driver_obj->mType = (UIFrameworkType_t)mp_obj_get_int(args[1]);
    LOG_D("%s:mType = %d;\n", __func__, driver_obj->mType);
    instance = UICreateInstance(driver_obj->mType);
    driver_obj->mInstance = instance;
    if (instance == NULL)
    {
        LOG_E("MLCreateInstance failed\n");
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ui_obj_open, 2, obj_open);

STATIC mp_obj_t obj_close(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_ui_obj_t* driver_obj = (mp_ui_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    if (driver_obj->mInstance == NULL)
    {
        LOG_E("Module has not been opened, not need close\n");
        return mp_const_none;
    }

    UIDestoryInstance(driver_obj->mInstance);
    driver_obj->mType = UI_FRAMEWORK_NONE;
    driver_obj->mInstance = NULL;
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ui_obj_close, 1, obj_close);

STATIC mp_obj_t obj_getDisplayWidth(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_ui_obj_t* driver_obj = (mp_ui_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    if (driver_obj->mInstance == NULL)
    {
        LOG_E("Module has been closed, please open first\n");
        return mp_const_none;
    }

    ret = UIGetDisplayWidth(driver_obj->mInstance);

    LOG_D("%s:out ret = %d;\n", __func__, ret);
    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ui_obj_getDisplayWidth, 1, obj_getDisplayWidth);

STATIC mp_obj_t obj_getDisplayHeight(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_ui_obj_t* driver_obj = (mp_ui_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    if (driver_obj->mInstance == NULL)
    {
        LOG_E("Module has been closed, please open first\n");
        return mp_const_none;
    }

    ret = UIGetDisplayHeight(driver_obj->mInstance);

    LOG_D("%s:out ret = %d;\n", __func__, ret);
    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ui_obj_getDisplayHeight, 1, obj_getDisplayHeight);

STATIC mp_obj_t obj_getDisplayBacklight(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_ui_obj_t* driver_obj = (mp_ui_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    if (driver_obj->mInstance == NULL)
    {
        LOG_E("Module has been closed, please open first\n");
        return mp_const_none;
    }

    ret = UIGetDisplayBacklight(driver_obj->mInstance);

    LOG_D("%s:out ret = %d;\n", __func__, ret);
    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ui_obj_getDisplayBacklight, 1, obj_getDisplayBacklight);

STATIC mp_obj_t obj_setDisplayBacklight(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_ui_obj_t* driver_obj = (mp_ui_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    if (driver_obj->mInstance == NULL)
    {
        LOG_E("Module has been closed, please open first\n");
        return mp_const_none;
    }

    int32_t value = (int32_t)mp_obj_get_int(args[1]);
    ret = UISetDisplayBacklight(driver_obj->mInstance, value);

    LOG_D("%s:out ret = %d;\n", __func__, ret);
    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ui_obj_setDisplayBacklight, 2, obj_setDisplayBacklight);

STATIC mp_obj_t obj_getDisplayType(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_ui_obj_t* driver_obj = (mp_ui_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    if (driver_obj->mInstance == NULL)
    {
        LOG_E("Module has been closed, please open first\n");
        return mp_const_none;
    }

    ret = UIGetDisplayType(driver_obj->mInstance);

    LOG_D("%s:out ret = %d;\n", __func__, ret);
    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ui_obj_getDisplayType, 1, obj_getDisplayType);

STATIC mp_obj_t obj_getDisplayFreq(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_ui_obj_t* driver_obj = (mp_ui_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    if (driver_obj->mInstance == NULL)
    {
        LOG_E("Module has been closed, please open first\n");
        return mp_const_none;
    }

    ret = UIGetDisplayFreq(driver_obj->mInstance);

    LOG_D("%s:out ret = %d;\n", __func__, ret);
    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ui_obj_getDisplayFreq, 1, obj_getDisplayFreq);

STATIC mp_obj_t obj_setDisplayFreq(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_ui_obj_t* driver_obj = (mp_ui_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    if (driver_obj->mInstance == NULL)
    {
        LOG_E("Module has been closed, please open first\n");
        return mp_const_none;
    }

    int32_t value = (int32_t)mp_obj_get_int(args[1]);
    ret = UISetDisplayFreq(driver_obj->mInstance, value);

    LOG_D("%s:out ret = %d;\n", __func__, ret);
    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ui_obj_setDisplayFreq, 2, obj_setDisplayFreq);

STATIC mp_obj_t obj_drawPoint(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 4)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_ui_obj_t* driver_obj = (mp_ui_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    if (driver_obj->mInstance == NULL)
    {
        LOG_E("Module has been closed, please open first\n");
        return mp_const_none;
    }

    uint32_t x = (uint32_t)mp_obj_get_int(args[1]);
    uint32_t y = (uint32_t)mp_obj_get_int(args[2]);
    int32_t color = (int32_t)mp_obj_get_int(args[3]);
    Point_t pt;
    pt.x = x;
    pt.y = y;
    ret = UIDrawPoint(driver_obj->mInstance, &pt, color);

    LOG_D("%s:out ret = %d;\n", __func__, ret);
    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ui_obj_drawPoint, 4, obj_drawPoint);

STATIC mp_obj_t obj_drawLine(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 6)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_ui_obj_t* driver_obj = (mp_ui_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    if (driver_obj->mInstance == NULL)
    {
        LOG_E("Module has been closed, please open first\n");
        return mp_const_none;
    }

    uint32_t startx = (uint32_t)mp_obj_get_int(args[1]);
    uint32_t starty = (uint32_t)mp_obj_get_int(args[2]);
    uint32_t endx = (uint32_t)mp_obj_get_int(args[3]);
    uint32_t endy = (uint32_t)mp_obj_get_int(args[4]);
    int32_t color = (int32_t)mp_obj_get_int(args[5]);
    Point_t start_pt;
    Point_t end_pt;
    start_pt.x = startx;
    start_pt.y = starty;
    end_pt.x = endx;
    end_pt.y = endy;
    ret = UIDrawLine(driver_obj->mInstance, &start_pt, &end_pt, color);

    LOG_D("%s:out ret = %d;\n", __func__, ret);
    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ui_obj_drawLine, 6, obj_drawLine);

STATIC mp_obj_t obj_drawRectangle(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 6)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_ui_obj_t* driver_obj = (mp_ui_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    if (driver_obj->mInstance == NULL)
    {
        LOG_E("Module has been closed, please open first\n");
        return mp_const_none;
    }

    int32_t left = (int32_t)mp_obj_get_int(args[1]);
    int32_t top = (int32_t)mp_obj_get_int(args[2]);
    int32_t right = (int32_t)mp_obj_get_int(args[3]);
    int32_t bottom = (int32_t)mp_obj_get_int(args[4]);
    int32_t color = (int32_t)mp_obj_get_int(args[5]);
    ret = UIDrawRectangle(driver_obj->mInstance, left, top, right, bottom, color);

    LOG_D("%s:out ret = %d;\n", __func__, ret);
    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ui_obj_drawRectangle, 6, obj_drawRectangle);

STATIC mp_obj_t obj_drawCircle(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 5)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_ui_obj_t* driver_obj = (mp_ui_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    if (driver_obj->mInstance == NULL)
    {
        LOG_E("Module has been closed, please open first\n");
        return mp_const_none;
    }

    uint32_t x = (uint32_t)mp_obj_get_int(args[1]);
    uint32_t y = (uint32_t)mp_obj_get_int(args[2]);
    int32_t radius = (int32_t)mp_obj_get_int(args[3]);
    int32_t color = (int32_t)mp_obj_get_int(args[4]);
    Point_t pt;
    pt.x = x;
    pt.y = y;
    ret = UIDrawCircle(driver_obj->mInstance, &pt, radius, color);

    LOG_D("%s:out ret = %d;\n", __func__, ret);
    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ui_obj_drawCircle, 5, obj_drawCircle);

STATIC mp_obj_t obj_drawText(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 8)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_ui_obj_t* driver_obj = (mp_ui_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    if (driver_obj->mInstance == NULL)
    {
        LOG_E("Module has been closed, please open first\n");
        return mp_const_none;
    }

    char* text = (char*)mp_obj_str_get_str(args[1]);
    int32_t size = (int32_t)mp_obj_get_int(args[2]);
    int32_t color = (int32_t)mp_obj_get_int(args[3]);
    int32_t left = (int32_t)mp_obj_get_int(args[4]);
    int32_t top = (int32_t)mp_obj_get_int(args[5]);
    int32_t right = (int32_t)mp_obj_get_int(args[6]);
    int32_t bottom = (int32_t)mp_obj_get_int(args[7]);
    UIDrawText(driver_obj->mInstance, text, size, color, left, top, right, bottom);

    LOG_D("%s:out\n", __func__);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ui_obj_drawText, 8, obj_drawText);

STATIC mp_obj_t obj_drawPixels(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 6)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_ui_obj_t* driver_obj = (mp_ui_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    if (driver_obj->mInstance == NULL)
    {
        LOG_E("Module has been closed, please open first\n");
        return mp_const_none;
    }

    int32_t* pixels = (int32_t*)MP_OBJ_TO_PTR(args[1]);
    uint32_t x = (uint32_t)mp_obj_get_int(args[2]);
    uint32_t y = (uint32_t)mp_obj_get_int(args[3]);
    int32_t width = (int32_t)mp_obj_get_int(args[4]);
    int32_t height = (int32_t)mp_obj_get_int(args[5]);
    Point_t pt;
    pt.x = x;
    pt.y = y;
    ret = UIDrawPixels(driver_obj->mInstance, pixels, &pt, width, height);

    LOG_D("%s:out ret = %d;\n", __func__, ret);
    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ui_obj_drawPixels, 6, obj_drawPixels);

STATIC mp_obj_t obj_drawImage(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 6)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_ui_obj_t* driver_obj = (mp_ui_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    if (driver_obj->mInstance == NULL)
    {
        LOG_E("Module has been closed, please open first\n");
        return mp_const_none;
    }

    char* path = (char*)mp_obj_str_get_str(args[1]);
    uint32_t x = (uint32_t)mp_obj_get_int(args[2]);
    uint32_t y = (uint32_t)mp_obj_get_int(args[3]);
    int32_t width = (int32_t)mp_obj_get_int(args[4]);
    int32_t height = (int32_t)mp_obj_get_int(args[5]);
    Point_t pt;
    pt.x = x;
    pt.y = y;
    ret = UIDrawImage(driver_obj->mInstance, path, &pt, width, height);

    LOG_D("%s:out ret = %d;\n", __func__, ret);
    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ui_obj_drawImage, 6, obj_drawImage);

STATIC mp_obj_t obj_updateDisplay(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_ui_obj_t* driver_obj = (mp_ui_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    if (driver_obj->mInstance == NULL)
    {
        LOG_E("Module has been closed, please open first\n");
        return mp_const_none;
    }

    UIUpdateDisplay(driver_obj->mInstance);

    LOG_D("%s:out\n", __func__);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ui_obj_updateDisplay, 1, obj_updateDisplay);

STATIC const mp_rom_map_elem_t ui_locals_dict_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_UI)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_UI_FRAMEWORK_NONE), MP_ROM_INT(UI_FRAMEWORK_NONE)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_UI_FRAMEWORK_GREENUI), MP_ROM_INT(UI_FRAMEWORK_GREENUI)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_UI_FRAMEWORK_FB), MP_ROM_INT(UI_FRAMEWORK_FB)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_UI_FRAMEWORK_LVGL), MP_ROM_INT(UI_FRAMEWORK_LVGL)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_UI_FRAMEWORK_SDL), MP_ROM_INT(UI_FRAMEWORK_SDL)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_UI_FRAMEWORK_MAX), MP_ROM_INT(UI_FRAMEWORK_MAX)},
    {MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&ui_obj_open)},
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&ui_obj_close)},
    {MP_ROM_QSTR(MP_QSTR_getDisplayWidth), MP_ROM_PTR(&ui_obj_getDisplayWidth)},
    {MP_ROM_QSTR(MP_QSTR_getDisplayHeight), MP_ROM_PTR(&ui_obj_getDisplayHeight)},
    {MP_ROM_QSTR(MP_QSTR_getDisplayBacklight), MP_ROM_PTR(&ui_obj_getDisplayBacklight)},
    {MP_ROM_QSTR(MP_QSTR_setDisplayBacklight), MP_ROM_PTR(&ui_obj_setDisplayBacklight)},
    {MP_ROM_QSTR(MP_QSTR_getDisplayType), MP_ROM_PTR(&ui_obj_getDisplayType)},
    {MP_ROM_QSTR(MP_QSTR_getDisplayFreq), MP_ROM_PTR(&ui_obj_getDisplayFreq)},
    {MP_ROM_QSTR(MP_QSTR_setDisplayFreq), MP_ROM_PTR(&ui_obj_setDisplayFreq)},
    {MP_ROM_QSTR(MP_QSTR_drawPoint), MP_ROM_PTR(&ui_obj_drawPoint)},
    {MP_ROM_QSTR(MP_QSTR_drawLine), MP_ROM_PTR(&ui_obj_drawLine)},
    {MP_ROM_QSTR(MP_QSTR_drawRectangle), MP_ROM_PTR(&ui_obj_drawRectangle)},
    {MP_ROM_QSTR(MP_QSTR_drawCircle), MP_ROM_PTR(&ui_obj_drawCircle)},
    {MP_ROM_QSTR(MP_QSTR_drawText), MP_ROM_PTR(&ui_obj_drawText)},
    {MP_ROM_QSTR(MP_QSTR_drawPixels), MP_ROM_PTR(&ui_obj_drawPixels)},
    {MP_ROM_QSTR(MP_QSTR_drawImage), MP_ROM_PTR(&ui_obj_drawImage)},
    {MP_ROM_QSTR(MP_QSTR_updateDisplay), MP_ROM_PTR(&ui_obj_updateDisplay)},
};

STATIC MP_DEFINE_CONST_DICT(ui_locals_dict, ui_locals_dict_table);

const mp_obj_type_t minicv_ui_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_UI,
    .print = ui_obj_print,
    .make_new = ui_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&ui_locals_dict,
};
