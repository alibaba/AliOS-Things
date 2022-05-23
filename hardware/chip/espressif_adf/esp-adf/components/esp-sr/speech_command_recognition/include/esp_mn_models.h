#pragma once
#include "esp_mn_iface.h"

//Contains declarations of all available speech recognion models. Pair this up with the right coefficients and you have a model that can recognize
//a specific phrase or word.
extern const esp_mn_iface_t esp_sr_multinet1_quantized_cn;
extern const esp_mn_iface_t esp_sr_multinet1_quantized_en;
extern const esp_mn_iface_t esp_sr_multinet2_quantized_cn;
/*
 Configure network to use based on what's selected in menuconfig.
*/
// #if CONFIG_SINGLE_RECOGNITION
// #define MULTINET_MODEL esp_sr_multinet1_quantized
// #else
// #error No valid neural network model selected.
// #endif


/*
 Configure wake word to use based on what's selected in menuconfig.
*/
#if CONFIG_SR_CHINESE && CONFIG_SINGLE_RECOGNITION
#include "multinet1_ch.h"
#define MULTINET_MODEL esp_sr_multinet1_quantized_cn
#define MULTINET_COEFF get_coeff_multinet1_ch
#elif CONFIG_SR_ENGLISH && CONFIG_SINGLE_RECOGNITION
#include "multinet1_en.h"
#define MULTINET_MODEL esp_sr_multinet1_quantized_en
#define MULTINET_COEFF get_coeff_multinet1_en
#elif CONFIG_SR_CHINESE && CONFIG_CONTINUOUS_RECOGNITION
#include "multinet1_ch.h"
#define MULTINET_MODEL esp_sr_multinet2_quantized_cn
#define MULTINET_COEFF get_coeff_multinet1_ch
#else
#error No valid wake word selected.
#endif

/* example

static const esp_mn_iface_t *multinet = &MULTINET_MODEL;

//Initialize MultiNet model data
model_iface_data_t *model_data = multinet->create(&MULTINET_COEFF);
add_speech_commands(multinet, model_data);

//Set parameters of buffer
int audio_chunksize=model->get_samp_chunksize(model_data);
int frequency = model->get_samp_rate(model_data);
int16_t *buffer=malloc(audio_chunksize*sizeof(int16_t));

//Detect
int r=model->detect(model_data, buffer);
if (r>0) {
    printf("Detection triggered output %d.\n",  r);
}

//Destroy model
model->destroy(model_data)

*/
