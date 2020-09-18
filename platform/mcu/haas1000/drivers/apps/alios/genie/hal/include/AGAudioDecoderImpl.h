#ifndef AG_AUDIO_DECODERIMPL_H
#define AG_AUDIO_DECODERIMPL_H

#include "AGAudioDecoder.h"


class AGAudioDecoderImpl: public AGAudioDecoder
{
public:
    AGAudioDecoderImpl();
    int32_t open(AGAudioDecoderFormat format);
    int32_t decoder(uint8_t *data_in, int input_size, uint8_t *data_out, int *output_size, int *output_sample, int *sample_rate);
    int32_t close();

private:
    AGAudioDecoderFormat mAudioFormat;
    void* mAudioHandle;
};

#endif  // AG_AUDIO_DECODERIMPL_H