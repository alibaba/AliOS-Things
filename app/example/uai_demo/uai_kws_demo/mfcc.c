/*
 * Copyright (C) 2018 Arm Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Description: MFCC feature extraction to match with TensorFlow MFCC Op
 */

#include <string.h>
#include <stdlib.h>
#include "mfcc.h"
#include "float.h"

#define M_PI 3.14159265358979323846

static float *create_dct_matrix(int32_t input_length, int32_t coefficient_count) {
    int32_t k, n;
    float * M = malloc(sizeof(float)*input_length * coefficient_count);
    float normalizer;
    arm_sqrt_f32(2.0/(float)input_length, &normalizer);
    for (k = 0; k < coefficient_count; k++) {
        for (n = 0; n < input_length; n++) {
            M[k*input_length+n] = normalizer * cos( ((double)M_PI)/input_length * (n + 0.5) * k );
        }
    }
    return M;
}

static float **create_mel_fbank(void *mfccp) {
    int32_t bin, i;
    MFCC *mfcc = (MFCC *)mfccp;

    int32_t num_fft_bins = mfcc->frame_len_padded/2;
    float fft_bin_width = ((float)SAMP_FREQ) / mfcc->frame_len_padded;
    float mel_low_freq = MelScale(MEL_LOW_FREQ);
    float mel_high_freq = MelScale(MEL_HIGH_FREQ); 
    float mel_freq_delta = (mel_high_freq - mel_low_freq) / (NUM_FBANK_BINS+1);

    float *this_bin = malloc(sizeof(float)*num_fft_bins);
    if(this_bin == NULL) {
        return 0;
    }

    float ** mel_fbank = malloc(sizeof(float)*NUM_FBANK_BINS);
    if(mel_fbank == NULL) {
        free(this_bin);
        return 0;
    }

    for (bin = 0; bin < NUM_FBANK_BINS; bin++) {
        float left_mel = mel_low_freq + bin * mel_freq_delta;
        float center_mel = mel_low_freq + (bin + 1) * mel_freq_delta;
        float right_mel = mel_low_freq + (bin + 2) * mel_freq_delta;

        int32_t first_index = -1, last_index = -1;

        for (i = 0; i < num_fft_bins; i++) {

            float freq = (fft_bin_width * i);  /* center freq of this fft bin. */
            float mel = MelScale(freq);
            this_bin[i] = 0.0;

            if (mel > left_mel && mel < right_mel) {
                float weight;
                if (mel <= center_mel) {
                    weight = (mel - left_mel) / (center_mel - left_mel);
                } else {
                    weight = (right_mel-mel) / (right_mel-center_mel);
                }
                this_bin[i] = weight;
                if (first_index == -1)
                    first_index = i;
                last_index = i;
            }
        }

        mfcc->fbank_filter_first[bin] = first_index;
        mfcc->fbank_filter_last[bin] = last_index;
        mel_fbank[bin] = malloc(sizeof(float)*(last_index-first_index+1)); 

        int32_t j = 0;
        /* copy the part we care about */
        for (i = first_index; i <= last_index; i++) {
            mel_fbank[bin][j++] = this_bin[i];
        }
    }
    free(this_bin);
    return mel_fbank; 
}

static void mfcc_compute(void *mfccp, const int16_t * audio_data, q7_t* mfcc_out) {
    int32_t i, j, bin;
    MFCC *mfcc = (MFCC *)mfccp;

    /* TensorFlow way of normalizing .wav data to (-1,1) */
    for (i = 0; i < mfcc->frame_len; i++) {
        mfcc->frame[i] = (float)audio_data[i]/(1<<15); 
    }
    /* Fill up remaining with zeros */
    memset(&mfcc->frame[mfcc->frame_len], 0, sizeof(float) * (mfcc->frame_len_padded-mfcc->frame_len));

    for (i = 0; i < mfcc->frame_len; i++) {
        mfcc->frame[i] *= mfcc->window_func[i];
    }

    /* Compute FFT */
    arm_rfft_fast_f32(mfcc->rfft, mfcc->frame, mfcc->buffer, 0);

    /* Convert to power spectrum */
    /* frame is stored as [real0, realN/2-1, real1, im1, real2, im2, ...] */
    int32_t half_dim = mfcc->frame_len_padded/2;
    float first_energy = mfcc->buffer[0] * mfcc->buffer[0];
    float last_energy =  mfcc->buffer[1] * mfcc->buffer[1];  /* handle this special case */
    for (i = 1; i < half_dim; i++) {
        float real = mfcc->buffer[i*2];
        float im = mfcc->buffer[i*2 + 1];
        mfcc->buffer[i] = real*real + im*im;
    }
    mfcc->buffer[0] = first_energy;
    mfcc->buffer[half_dim] = last_energy;  

    float sqrt_data;
    /* Apply mel filterbanks */
    for (bin = 0; bin < NUM_FBANK_BINS; bin++) {
        j = 0;
        float mel_energy = 0;
        int32_t first_index = mfcc->fbank_filter_first[bin];
        int32_t last_index = mfcc->fbank_filter_last[bin];
        for (i = first_index; i <= last_index; i++) {
            arm_sqrt_f32(mfcc->buffer[i],&sqrt_data);
            mel_energy += (sqrt_data) * mfcc->mel_fbank[bin][j++];
        }
        mfcc->mel_energies[bin] = mel_energy;

        /* avoid log of zero */
        if (mel_energy == 0.0)
            mfcc->mel_energies[bin] = FLT_MIN;
    }

    /* Take log */
    for (bin = 0; bin < NUM_FBANK_BINS; bin++)
        mfcc->mel_energies[bin] = logf(mfcc->mel_energies[bin]);

    /* Take DCT. Uses matrix mul. */
    for (i = 0; i < mfcc->num_mfcc_features; i++) {
        float sum = 0.0;
        for (j = 0; j < NUM_FBANK_BINS; j++) {
        sum += mfcc->dct_matrix[i*NUM_FBANK_BINS+j] * mfcc->mel_energies[j];
        }

        /* Input is Qx.mfcc_dec_bits (from quantization step) */
        sum *= (0x1<<mfcc->mfcc_dec_bits);
        sum = round(sum); 
        if(sum >= 127)
        mfcc_out[i] = 127;
        else if(sum <= -128)
        mfcc_out[i] = -128;
        else
        mfcc_out[i] = sum; 
    }
}

void mfcc_init(MFCC *mfcc, int num_mfcc_features, int frame_len, int mfcc_dec_bits)
{
    mfcc->num_mfcc_features = num_mfcc_features;
    mfcc->frame_len = frame_len;
    mfcc->mfcc_dec_bits = mfcc_dec_bits;
    
    /* Round-up to nearest power of 2. */
    mfcc->frame_len_padded = pow(2,ceil((log(frame_len)/log(2))));

    mfcc->frame = malloc(sizeof(float) * mfcc->frame_len_padded);
    mfcc->buffer = malloc(sizeof(float) * mfcc->frame_len_padded);
    mfcc->mel_energies = malloc(sizeof(float) * NUM_FBANK_BINS);

    /* create window function */
    mfcc->window_func = malloc(sizeof(float) *frame_len);
    for (int i = 0; i < frame_len; i++)
        mfcc->window_func[i] = 0.5 - 0.5*cos(M_2PI * ((float)i) / (frame_len));

    /* create mel filterbank */
    mfcc->fbank_filter_first = malloc(sizeof(int32_t) * NUM_FBANK_BINS);
    mfcc->fbank_filter_last = malloc(sizeof(int32_t) * NUM_FBANK_BINS);
    mfcc->mel_fbank = create_mel_fbank(mfcc);

    /* create DCT matrix */
    mfcc->dct_matrix = create_dct_matrix(NUM_FBANK_BINS, num_mfcc_features);

    /* initialize FFT */
    mfcc->rfft = malloc(sizeof(arm_rfft_fast_instance_f32));
    arm_rfft_fast_init_f32(mfcc->rfft, mfcc->frame_len_padded);

    mfcc->create_dct_matrix = create_dct_matrix;
    mfcc->create_mel_fbank  = create_mel_fbank;
    mfcc->mfcc_compute      = mfcc_compute;
}

void mfcc_uninit(MFCC *mfcc) {
    free(mfcc->frame);
    free(mfcc->buffer);
    free(mfcc->mel_energies);
    free(mfcc->window_func);
    free(mfcc->fbank_filter_first);
    free(mfcc->fbank_filter_last);
    free(mfcc->dct_matrix);
    free(mfcc->rfft);
    for(int i = 0; i < NUM_FBANK_BINS; i++)
        free(mfcc->mel_fbank[i]);
    free(mfcc->mel_fbank);
}
