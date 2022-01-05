/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2019 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "esp_log.h"
#include "esp_wn_iface.h"
#include "sdkconfig.h"

extern const esp_wn_iface_t esp_sr_wakenet3_quantized;
extern const esp_wn_iface_t esp_sr_wakenet4_quantized;
extern const esp_wn_iface_t esp_sr_wakenet5_quantized;
extern const esp_wn_iface_t esp_sr_wakenet5_float;
extern const esp_wn_iface_t esp_sr_wakenet6_quantized;

void get_wakenet_iface(esp_wn_iface_t **wakenet_iface)
{
#if CONFIG_SR_MODEL_WN3_QUANT
    *wakenet_iface = (esp_wn_iface_t *)&esp_sr_wakenet3_quantized;
#elif CONFIG_SR_MODEL_WN4_QUANT
    *wakenet_iface = (esp_wn_iface_t *)&esp_sr_wakenet4_quantized;
#elif CONFIG_SR_MODEL_WN5_FLOAT
    *wakenet_iface = (esp_wn_iface_t *)&esp_sr_wakenet5_float;
#elif CONFIG_SR_MODEL_WN5_QUANT
    *wakenet_iface = (esp_wn_iface_t *)&esp_sr_wakenet5_quantized;
#elif CONFIG_SR_MODEL_WN6_QUANT
    *wakenet_iface = (esp_wn_iface_t *)&esp_sr_wakenet6_quantized;
#else
    ESP_LOGE("RECORDER_ENG_HELPER", "No valid neural network model selected");
#endif
}

void get_wakenet_coeff(model_coeff_getter_t **model_coeff)
{
#if CONFIG_SR_WN3_HILEXIN
#include "hilexin_wn3.h"
    *model_coeff = (model_coeff_getter_t *)&get_coeff_hilexin_wn3;

#elif CONFIG_SR_WN4_HILEXIN
#include "hilexin_wn4.h"
    *model_coeff = (model_coeff_getter_t *)&get_coeff_hilexin_wn4;

#elif CONFIG_SR_WN5_HILEXIN & CONFIG_SR_MODEL_WN5_FLOAT
#include "hilexin_wn5_float.h"
    *model_coeff = (model_coeff_getter_t *)&get_coeff_hilexin_wn5_float;

#elif CONFIG_SR_WN5_HILEXIN & CONFIG_SR_MODEL_WN5_QUANT
#include "hilexin_wn5.h"
    *model_coeff = (model_coeff_getter_t *)&get_coeff_hilexin_wn5;

#elif CONFIG_SR_WN5X2_HILEXIN & CONFIG_SR_MODEL_WN5_QUANT
#include "hilexin_wn5X2.h"
    *model_coeff = (model_coeff_getter_t *)&get_coeff_hilexin_wn5X2;

#elif CONFIG_SR_WN5X3_HILEXIN & CONFIG_SR_MODEL_WN5_QUANT
#include "hilexin_wn5X3.h"
    *model_coeff = (model_coeff_getter_t *)&get_coeff_hilexin_wn5X3;

#elif CONFIG_SR_WN5_NIHAOXIAOZHI & CONFIG_SR_MODEL_WN5_QUANT
#include "nihaoxiaozhi_wn5.h"
    *model_coeff = (model_coeff_getter_t *)&get_coeff_nihaoxiaozhi_wn5;

#elif CONFIG_SR_WN5X2_NIHAOXIAOZHI & CONFIG_SR_MODEL_WN5_QUANT
#include "nihaoxiaozhi_wn5X2.h"
    *model_coeff = (model_coeff_getter_t *)&get_coeff_nihaoxiaozhi_wn5X2;

#elif CONFIG_SR_WN5X3_NIHAOXIAOZHI & CONFIG_SR_MODEL_WN5_QUANT
#include "nihaoxiaozhi_wn5X3.h"
    *model_coeff = (model_coeff_getter_t *)&get_coeff_nihaoxiaozhi_wn5X3;

#elif CONFIG_SR_WN5X3_NIHAOXIAOXIN & CONFIG_SR_MODEL_WN5_QUANT
#include "nihaoxiaoxin_wn5X3.h"
    *model_coeff = (model_coeff_getter_t *)&get_coeff_nihaoxiaoxin_wn5X3;

#elif CONFIG_SR_WN5X3_HIJESON & CONFIG_SR_MODEL_WN5_QUANT
#include "hijeson_wn5X3.h"
    *model_coeff = (model_coeff_getter_t *)&get_coeff_hijeson_wn5X3;

#elif CONFIG_SR_WN6_NIHAOXIAOXIN
#include "nihaoxiaoxin_wn6.h"
    *model_coeff = (model_coeff_getter_t *)&get_coeff_nihaoxiaoxin_wn6;

#elif CONFIG_SR_WN5_CUSTOMIZED_WORD
#include "customized_word_wn5.h"
    *model_coeff = (model_coeff_getter_t *)&get_coeff_customized_word_wn5;

#elif CONFIG_SR_WN6_CUSTOMIZED_WORD
#include "customized_word_wn6.h"
    *model_coeff = (model_coeff_getter_t *)&get_coeff_customized_word_wn6;

#else
    ESP_LOGE("RECORDER_ENG_HELPER", "No valid wake word selected");
#endif

}
