// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
// All rights reserved.

#ifndef __ESP_SONIC_H__
#define __ESP_SONIC_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* This specifies the range of voice pitches we try to match.
Note that if we go lower than 65, we could overflow in findPitchInRange */
#define SONIC_MIN_PITCH 65
#define SONIC_MAX_PITCH 400

/* For all of the following functions, numChannels is multiplied by numSamples
to determine the actual number of values read or returned. */

/**
 * @brief      Create a sonic stream
 *
 * @param      sample_rate  The sample rate of the stream
 * @param      channels     The number channel(s) of the stream (1 : mono,  2 : dual)
 *
 * @return     allocate the stream.  NULL only if we are out of memory and cannot  
 */
void* esp_sonic_create_stream(int sample_rate, int channels);

/**
 * @brief      Set the resample method
 *
 * @param      handle                               The handle of the sonic stream
 * @param      resample_linear_interpolate          1 for linear interpolation, faster but lower accuracy
 */
void esp_sonic_set_resample_mode(void* handle, int resample_linear_interpolate);

/**
 * @brief      Set the speed of the stream
 *
 * @param      handle        The handle of the sonic stream
 * @param      speed         The scaling factor of speed of the stream
 */
void esp_sonic_set_speed(void* handle, float speed);

/**
 * @brief      Set the pitch of the stream
 *
 * @param      handle        The handle of the sonic stream
 * @param      pitch         The scaling factor of pitch of the stream
 */
void esp_sonic_set_pitch(void* handle, float pitch);

/**
 * @brief      Set the rate of the stream
 *
 * @param      handle        The handle of the sonic stream
 * @param      rate          The rate of the stream
 */
void esp_sonic_set_rate(void* handle, float rate);

/**
 * @brief      Set the scaling factor of the stream
 *
 * @param      handle        The handle of the sonic stream
 * @param      volume        The scaling factor of volume of the stream
 */
void esp_sonic_set_volume(void* handle, float volume);

/**
 * @brief      Set chord pitch mode on or off.
 *
 * @param      handle            The handle of the sonic stream
 * @param      use_chord_pitch   Default is off.
 */
void esp_sonic_set_chord_pitch(void* handle, int use_chord_pitch);

/**
 * @brief      Set the "quality"
 *
 * @param      handle        The handle of the sonic stream
 * @param      quality       Default 0 is virtually as good as 1, but very much faster
 */
void esp_sonic_set_quality(void* handle, int quality);

/**
 * @brief      Force the sonic stream to generate output using whatever data it currently
 *             has.  No extra delay will be added to the output, but flushing in the middle
 *             of words could introduce distortion. 
 *
 * @param      handle         The handle of the sonic stream
 */
int esp_sonic_flush_stream(void* handle);

/**
 * @brief      Use this to write 16-bit data to be speed up or down into the stream
 *
 * @param      handle         The handle of the sonic stream
 * @param      samples        The buffer of output stream
 * @param      num_samples     The length of output stream
 *
 * @return     0 if memory realloc failed, otherwise 1  
 */
int esp_sonic_write_to_stream(void* handle, short* samples, int num_samples);

/**
 * @brief      Use this to read 16-bit data out of the stream.  Sometimes no data will
 *             be available, and zero is returned, which is not an error condition.
 *
 * @param      handle         The handle of the sonic stream
 * @param      samples        The buffer of input stream 
 * @param      max_samples    The maximum of the length of "samples"
 *
 * @return     allocate the stream.  NULL will be returned ,only if we are out of memory and cannot  
 */
int esp_sonic_read_from_stream(void* handle, short* samples, int max_samples);

/**
 * @brief      Destroy the sonic stream
 *
 * @param      handle         The handle of the sonic stream
 */
void esp_sonic_destroy_stream(void* handle);

#ifdef __cplusplus
}
#endif

#endif