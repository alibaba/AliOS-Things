#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"

#include "base/modules/c/include/WrapperIHaasML.h"
#include "ulog/ulog.h"

#include "ucloud_ai_common.h"

#define LOG_TAG "ML"

extern const mp_obj_type_t minicv_ml_type;
// this is the actual C-structure for our new object
typedef struct
{
    // base represents some basic information, like type
    mp_obj_base_t Base;
    // a member created by us
    char *ModuleName;
    MLEngineType_t mType;
    void *mInstance;
    char *mNetName;
} mp_ml_obj_t;

static mp_obj_t callback = mp_const_none;

void ml_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    LOGD(LOG_TAG, "entern %s;\n", __func__);
    mp_ml_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ModuleName(%s)", self->ModuleName);
}

STATIC mp_obj_t ml_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s;\n", __func__);
    mp_ml_obj_t *driver_obj = m_new_obj(mp_ml_obj_t);
    if (!driver_obj)
    {
        mp_raise_OSError(MP_EINVAL);
    }

    driver_obj->Base.type = &minicv_ml_type;
    driver_obj->ModuleName = "minicv-ML";
    driver_obj->mType = ML_ENGINE_NONE;
    driver_obj->mInstance = NULL;

    return MP_OBJ_FROM_PTR(driver_obj);
}

STATIC mp_obj_t obj_open(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void *instance = NULL;
    if (n_args < 2)
    {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(args[0]);
    mp_ml_obj_t *driver_obj = (mp_ml_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOGE(LOG_TAG, "driver_obj is NULL\n");
        return mp_const_none;
    }

    if (driver_obj->mInstance != NULL)
    {
        LOGE(LOG_TAG, "Module has been opened, please clode first\n");
        return mp_const_none;
    }

    driver_obj->mType = (MLEngineType_t)mp_obj_get_int(args[1]);
    LOGD(LOG_TAG, "%s:mType = %d;\n", __func__, driver_obj->mType);
    instance = MLCreateInstance(driver_obj->mType);
    driver_obj->mInstance = instance;
    if (instance == NULL)
    {
        LOGE(LOG_TAG, "MLCreateInstance failed\n");
        return mp_const_none;
    }
    LOGD(LOG_TAG, "%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ml_obj_open, 2, obj_open);

STATIC mp_obj_t obj_close(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void *instance = NULL;
    if (n_args < 1)
    {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(args[0]);
    mp_ml_obj_t *driver_obj = (mp_ml_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOGE(LOG_TAG, "driver_obj is NULL\n");
        return mp_const_none;
    }

    if (driver_obj->mInstance == NULL)
    {
        LOGE(LOG_TAG, "Module has not been opened, not need close\n");
        return mp_const_none;
    }

    MLDestoryInstance(driver_obj->mInstance);
    driver_obj->mType = ML_ENGINE_NONE;
    driver_obj->mInstance = NULL;
    LOGD(LOG_TAG, "%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ml_obj_close, 1, obj_close);

STATIC mp_obj_t obj_config(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void *instance = NULL;
    if (n_args < 5)
    {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(args[0]);
    mp_ml_obj_t *driver_obj = (mp_ml_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOGE(LOG_TAG, "driver_obj is NULL\n");
        return mp_const_none;
    }

    if (driver_obj->mInstance == NULL)
    {
        LOGE(LOG_TAG, "Module has been closed, please open first\n");
        return mp_const_none;
    }

    char *key = (mp_obj_get_type(args[1]) == &mp_type_NoneType) ? NULL : (char *)mp_obj_str_get_str(args[1]);
    char *secret = (mp_obj_get_type(args[2]) == &mp_type_NoneType) ? NULL : (char *)mp_obj_str_get_str(args[2]);
    char *endpoint = (mp_obj_get_type(args[3]) == &mp_type_NoneType) ? NULL : (char *)mp_obj_str_get_str(args[3]);
    char *bucket = (mp_obj_get_type(args[4]) == &mp_type_NoneType) ? NULL : (char *)mp_obj_str_get_str(args[4]);

    LOGD(LOG_TAG, "key = %s;\n", key);
    LOGD(LOG_TAG, "secret = %s;\n", secret);
    LOGD(LOG_TAG, "endpoint = %s;\n", endpoint);
    LOGD(LOG_TAG, "bucket = %s;\n", bucket);
    ret = MLConfig(driver_obj->mInstance, key, secret, endpoint, bucket);
    if (ret != 0)
    {
        LOGE(LOG_TAG, "MLConfig failed\n");
        return mp_const_none;
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ml_obj_config, 5, obj_config);

STATIC mp_obj_t obj_setInputData(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void *instance = NULL;
    if (n_args < 2)
    {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }

    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(args[0]);
    mp_ml_obj_t *driver_obj = (mp_ml_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOGE(LOG_TAG, "driver_obj is NULL\n");
        return mp_const_none;
    }

    if (driver_obj->mInstance == NULL)
    {
        LOGE(LOG_TAG, "Module has been closed, please open first\n");
        return mp_const_none;
    }

    char *mFileName = NULL;
    char *mFileName2 = NULL;

    if (n_args == 2)
    {
        mFileName = (char *)mp_obj_str_get_str(args[1]);
    }
    else if (n_args > 2)
    {
        mFileName = (char *)mp_obj_str_get_str(args[1]);
        mFileName2 = (mp_obj_get_type(args[2]) == &mp_type_NoneType) ? NULL : (char *)mp_obj_str_get_str(args[2]);
    }

    ret = MLSetInputData(driver_obj->mInstance, mFileName, mFileName2);
    if (ret != 0)
    {
        LOGE(LOG_TAG, "MLSetInputData failed mFileName = %s, mFileName2: %s;\n", mFileName, mFileName2);
        return mp_const_none;
    }
    LOGD(LOG_TAG, "%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ml_obj_setInputData, 2, 3, obj_setInputData);

STATIC mp_obj_t obj_loadNet(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void *instance = NULL;
    if (n_args < 2)
    {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(args[0]);
    mp_ml_obj_t *driver_obj = (mp_ml_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOGE(LOG_TAG, "driver_obj is NULL\n");
        return mp_const_none;
    }

    if (driver_obj->mInstance == NULL)
    {
        LOGE(LOG_TAG, "Module has been closed, please open first\n");
        return mp_const_none;
    }

    driver_obj->mNetName = (char *)mp_obj_str_get_str(args[1]);
    LOGD(LOG_TAG, "mNetName = %s;\n", driver_obj->mNetName);
    ret = MLLoadNet(driver_obj->mInstance, driver_obj->mNetName);
    if (ret != 0)
    {
        LOGE(LOG_TAG, "MLLoadNet failed mNetName = %s;\n", driver_obj->mNetName);
        return mp_const_none;
    }
    LOGD(LOG_TAG, "%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ml_obj_loadNet, 2, obj_loadNet);

STATIC mp_obj_t obj_predict(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void *instance = NULL;
    if (n_args < 1)
    {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(args[0]);
    mp_ml_obj_t *driver_obj = (mp_ml_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOGE(LOG_TAG, "driver_obj is NULL\n");
        return mp_const_none;
    }

    if (driver_obj->mInstance == NULL)
    {
        LOGE(LOG_TAG, "Module has been closed, please open first\n");
        return mp_const_none;
    }
    MP_THREAD_GIL_EXIT();
    ret = MLPredict(driver_obj->mInstance);
    MP_THREAD_GIL_ENTER();
    if (ret != 0)
    {
        LOGE(LOG_TAG, "MLPredict failed\n");
        return mp_const_none;
    }
    LOGD(LOG_TAG, "%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ml_obj_predict, 1, obj_predict);

STATIC mp_obj_t obj_getPredictResponses(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void *instance = NULL;
    mp_obj_t ret_list = mp_obj_new_list(0, NULL);

    if (n_args < 2)
    {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(args[0]);
    mp_ml_obj_t *driver_obj = (mp_ml_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOGE(LOG_TAG, "driver_obj is NULL\n");
        return mp_const_none;
    }

    if (driver_obj->mInstance == NULL)
    {
        LOGE(LOG_TAG, "Module has been closed, please open first\n");
        return mp_const_none;
    }

    if (args[1] != mp_const_none)
    {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_WRITE);
        memset(bufinfo.buf, 0, bufinfo.len);
        LOGD(LOG_TAG, "bufinfo.buf = %p;bufinfo.len = %d;\n", bufinfo.buf, bufinfo.len);
        ret = MLGetPredictResponses(driver_obj->mInstance, &bufinfo.buf, &bufinfo.len);
        if (ret != 0)
        {
            LOGE(LOG_TAG, "MLGetPredictResponses failed\n");
        }
        LOGD(LOG_TAG, "bufinfo.buf = %s;bufinfo.len = %d;\n", bufinfo.buf, bufinfo.len);
        LOGD(LOG_TAG, "%s:out ret = %d;\n", __func__, ret);
        return mp_const_none;
    }
    else
    {
        if (driver_obj->mType == ML_ENGINE_CLOUD)
        {
            int i = 0, ret_len = 0;
            ucloud_ai_result_t result[16];

            memset(result, 0, sizeof(ucloud_ai_result_t) * 16);

            if (!strcmp(driver_obj->mNetName, "FacebodyComparing"))
            {
                ret_len = MLGetPredictResponses(driver_obj->mInstance, (char *)&result[0], sizeof(ucloud_ai_result_t));
                if (ret_len <= 0)
                {
                    LOGE(LOG_TAG, "MLGetPredictResponses failed\n");
                    return mp_const_none;
                }

                mp_obj_t dict = mp_obj_new_dict(5);

                i = 0;
                LOGD(LOG_TAG, "%.1f\n", result[i].facebody.face.confidence);
                LOGD(LOG_TAG, "x: %.1f\n", result[i].facebody.face.location.x);
                LOGD(LOG_TAG, "y: %.1f\n", result[i].facebody.face.location.y);
                LOGD(LOG_TAG, "w: %.1f\n", result[i].facebody.face.location.w);
                LOGD(LOG_TAG, "h: %.1f\n", result[i].facebody.face.location.h);
                mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(qstr_from_str("confidence")),
                                  mp_obj_new_float(result[i].facebody.face.confidence));
                mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(qstr_from_str("x")),
                                  mp_obj_new_float(result[i].facebody.face.location.x));
                mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(qstr_from_str("y")),
                                  mp_obj_new_float(result[i].facebody.face.location.y));
                mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(qstr_from_str("w")),
                                  mp_obj_new_float(result[i].facebody.face.location.w));
                mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(qstr_from_str("h")),
                                  mp_obj_new_float(result[i].facebody.face.location.h));

                return dict;
            }
            else if (!strcmp(driver_obj->mNetName, "DetectFruits"))
            {
                ret_len = MLGetPredictResponses(driver_obj->mInstance, (char *)&result[0], sizeof(ucloud_ai_result_t) * 16);
                if (ret_len <= 0)
                {
                    LOGE(LOG_TAG, "MLGetPredictResponses failed\n");
                    return mp_const_none;
                }

                mp_obj_t dict = mp_obj_new_dict(6);
                mp_obj_t ret_list = mp_obj_new_list(0, NULL);

                for (i = 0; i < ret_len; i++)
                {
                    LOGD(LOG_TAG, "name: %s\n", result[i].imagerecog.fruits.name);
                    LOGD(LOG_TAG, "score: %.1f\n", result[i].imagerecog.fruits.score);
                    LOGD(LOG_TAG, "x: %.1f\n", result[i].imagerecog.fruits.box.x);
                    LOGD(LOG_TAG, "y: %.1f\n", result[i].imagerecog.fruits.box.y);
                    LOGD(LOG_TAG, "w: %.1f\n", result[i].imagerecog.fruits.box.w);
                    LOGD(LOG_TAG, "h: %.1f\n", result[i].imagerecog.fruits.box.h);
                    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(qstr_from_str("name")),
                                      mp_obj_new_strn(result[i].imagerecog.fruits.name));
                    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(qstr_from_str("score")),
                                      mp_obj_new_float(result[i].imagerecog.fruits.score));
                    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(qstr_from_str("x")),
                                      mp_obj_new_float(result[i].imagerecog.fruits.box.x));
                    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(qstr_from_str("y")),
                                      mp_obj_new_float(result[i].facebody.face.location.y));
                    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(qstr_from_str("w")),
                                      mp_obj_new_float(result[i].facebody.face.location.w));
                    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(qstr_from_str("h")),
                                      mp_obj_new_float(result[i].facebody.face.location.h));
                    mp_obj_list_append(ret_list, dict);
                }
                return ret_list;
            }
            else if (!strcmp(driver_obj->mNetName, "DetectPedestrian"))
            {
                ret_len = MLGetPredictResponses(driver_obj->mInstance, (char *)&result[0], sizeof(ucloud_ai_result_t) * 16);
                if (ret_len <= 0)
                {
                    LOGE(LOG_TAG, "MLGetPredictResponses failed\n");
                    return mp_const_none;
                }

                mp_obj_t dict = mp_obj_new_dict(6);
                mp_obj_t ret_list = mp_obj_new_list(0, NULL);

                for (i = 0; i < ret_len; i++)
                {
                    LOGD(LOG_TAG, "type: %s\n", result[i].facebody.pedestrian.type);
                    LOGD(LOG_TAG, "score: %.1f\n", result[i].facebody.pedestrian.score);
                    LOGD(LOG_TAG, "x: %.1f\n", result[i].facebody.pedestrian.box.x);
                    LOGD(LOG_TAG, "y: %.1f\n", result[i].facebody.pedestrian.box.y);
                    LOGD(LOG_TAG, "w: %.1f\n", result[i].facebody.pedestrian.box.w);
                    LOGD(LOG_TAG, "h: %.1f\n", result[i].facebody.pedestrian.box.h);
                    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(qstr_from_str("type")),
                                      mp_obj_new_strn(result[i].facebody.pedestrian.type));
                    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(qstr_from_str("score")),
                                      mp_obj_new_float(result[i].facebody.pedestrian.score));
                    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(qstr_from_str("x")),
                                      mp_obj_new_float(result[i].facebody.pedestrian.box.x));
                    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(qstr_from_str("y")),
                                      mp_obj_new_float(result[i].facebody.pedestrian.box.y));
                    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(qstr_from_str("w")),
                                      mp_obj_new_float(result[i].facebody.pedestrian.box.w));
                    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(qstr_from_str("h")),
                                      mp_obj_new_float(result[i].facebody.pedestrian.box.h));
                    mp_obj_list_append(ret_list, dict);
                }
                return ret_list;
            } else if (!strcmp(driver_obj->mNetName, "RecognizeLicensePlate")) {
                ret_len =
                    MLGetPredictResponses(driver_obj->mInstance, (char *)&result[0], sizeof(ucloud_ai_result_t) * 16);
                if (ret_len <= 0) {
                    LOGE(LOG_TAG, "MLGetPredictResponses failed\n");
                    return mp_const_none;
                }

                mp_obj_t dict = mp_obj_new_dict(8);
                mp_obj_t ret_list = mp_obj_new_list(0, NULL);

                for (i = 0; i < ret_len; i++) {
                    LOGD(LOG_TAG, "plateNumber: %s\n", result[i].ocr.licensePlate.plateNumber);
                    LOGD(LOG_TAG, "confidence: %.1f\n", result[i].ocr.licensePlate.confidence);
                    LOGD(LOG_TAG, "plateType: %s\n", result[i].ocr.licensePlate.plateType);
                    LOGD(LOG_TAG, "plateTypeConfidence: %.1f\n", result[i].ocr.licensePlate.plateTypeConfidence);
                    LOGD(LOG_TAG, "x: %d\n", result[i].ocr.licensePlate.roi.x);
                    LOGD(LOG_TAG, "y: %d\n", result[i].ocr.licensePlate.roi.y);
                    LOGD(LOG_TAG, "w: %d\n", result[i].ocr.licensePlate.roi.w);
                    LOGD(LOG_TAG, "h: %.1f\n", result[i].ocr.licensePlate.roi.h);
                    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(qstr_from_str("plateNumber")),
                                      mp_obj_new_strn(result[i].ocr.licensePlate.plateNumber));
                    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(qstr_from_str("confidence")),
                                      mp_obj_new_float(result[i].ocr.licensePlate.confidence));
                    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(qstr_from_str("plateType")),
                                      mp_obj_new_strn(result[i].ocr.licensePlate.plateType));
                    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(qstr_from_str("plateTypeConfidence")),
                                      mp_obj_new_float(result[i].ocr.licensePlate.plateTypeConfidence));
                    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(qstr_from_str("x")),
                                      mp_obj_new_int(result[i].ocr.licensePlate.roi.x));
                    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(qstr_from_str("y")),
                                      mp_obj_new_int(result[i].ocr.licensePlate.roi.y));
                    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(qstr_from_str("w")),
                                      mp_obj_new_int(result[i].ocr.licensePlate.roi.w));
                    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(qstr_from_str("h")),
                                      mp_obj_new_int(result[i].ocr.licensePlate.roi.h));
                    mp_obj_list_append(ret_list, dict);
                }
                return ret_list;
            }
        }
        else
        {
            return mp_const_none;
        }
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ml_obj_getPredictResponses, 2, obj_getPredictResponses);

STATIC mp_obj_t obj_unLoadNet(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void *instance = NULL;
    if (n_args < 1)
    {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(args[0]);
    mp_ml_obj_t *driver_obj = (mp_ml_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOGE(LOG_TAG, "driver_obj is NULL\n");
        return mp_const_none;
    }

    if (driver_obj->mInstance == NULL)
    {
        LOGE(LOG_TAG, "Module has been closed, please open first\n");
        return mp_const_none;
    }

    ret = MLUnLoadNet(driver_obj->mInstance);
    if (ret != 0)
    {
        LOGE(LOG_TAG, "MLUnLoadNet failed\n");
        return mp_const_none;
    }
    LOGD(LOG_TAG, "%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ml_obj_unLoadNet, 1, obj_unLoadNet);

STATIC const mp_rom_map_elem_t ml_locals_dict_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ML)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_ML_ENGINE_NONE), MP_ROM_INT(ML_ENGINE_NONE)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_ML_ENGINE_MNN), MP_ROM_INT(ML_ENGINE_MNN)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_ML_ENGINE_CLOUD), MP_ROM_INT(ML_ENGINE_CLOUD)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_ML_ENGINE_ODLA), MP_ROM_INT(ML_ENGINE_ODLA)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_ML_ENGINE_MAX), MP_ROM_INT(ML_ENGINE_MAX)},
    {MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&ml_obj_open)},
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&ml_obj_close)},
    {MP_ROM_QSTR(MP_QSTR_config), MP_ROM_PTR(&ml_obj_config)},
    {MP_ROM_QSTR(MP_QSTR_setInputData), MP_ROM_PTR(&ml_obj_setInputData)},
    {MP_ROM_QSTR(MP_QSTR_loadNet), MP_ROM_PTR(&ml_obj_loadNet)},
    {MP_ROM_QSTR(MP_QSTR_predict), MP_ROM_PTR(&ml_obj_predict)},
    {MP_ROM_QSTR(MP_QSTR_getPredictResponses), MP_ROM_PTR(&ml_obj_getPredictResponses)},
    {MP_ROM_QSTR(MP_QSTR_unLoadNet), MP_ROM_PTR(&ml_obj_unLoadNet)},
};

STATIC MP_DEFINE_CONST_DICT(ml_locals_dict, ml_locals_dict_table);

const mp_obj_type_t minicv_ml_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_ML,
    .print = ml_obj_print,
    .make_new = ml_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&ml_locals_dict,
};
