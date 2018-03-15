//
//  opus_codec.h
//

#ifndef opus_codec_h
#define opus_codec_h

#include "opus_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FRAME_SAMPLES (320)
#define MAX_DATA_BYTES (320 * sizeof(short))

int opus_codec_init(int sample_rate, int channels, int VBR, int bit_rate,
		int complexity, int signal);

int opus_codec_destroy();

/*
 * samples: data, char 
 * sample_data_len : the number of samples in bytes
 * buffer: output , uchar
 * buffer_capacity: the number of uchar. for example equal to 2*samples_len
 */
int opus_codec_encode(const char *sample_data, int sample_data_len,
		unsigned char *buffer, int buffer_capacity, int *buffer_len);

#ifdef __cplusplus
}
#endif

#endif /* opus_codec_h */
