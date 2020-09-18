/**
 * Copyright (C) 2018 Alibaba.inc, All rights reserved.
 *
 * @file AGAudioOutput.h
 * @brief audio interface
 * @author lifen.lf@alibaba-inc.com
 * @data 2018/04/07
 * @version 1.0
 */

#ifndef AG_AUDIO_OUTPUT_H
#define AG_AUDIO_OUTPUT_H

#include <stdint.h>

typedef enum
{
    AG_SAMPLE_FORMAT_INVALID= -1,
    AG_SAMPLE_FORMAT_UNSPECIFIED = 0,
    AG_SAMPLE_FORMAT_PCM_S16,
    AG_SAMPLE_FORMAT_PCM_S32
} AGSampleFormat;

typedef enum
{
    AG_TRACK_TYPE_INVALID= -1,
    AG_TRACK_TYPE_UNSPECIFIED = 0,
    AG_TRACK_TYPE_URI,
    AG_TRACK_TYPE_TTS,
    AG_TRACK_TYPE_TTS_NO_MUTE,
    AG_TRACK_TYPE_PROMPT,
    AG_TRACK_TYPE_PROMPT_NO_MUTE,
    AG_TRACK_TYPE_BLUETOOTH,
    AG_TRACK_TYPE_CMIMS,
    AG_TRACK_TYPE_VOIP,
} AGTrackType;

typedef enum {
    AG_TRACK_EVENT_UNDERRUN,
    AG_TRACK_EVENT_UNDERRUN_EXIT
} AGTrackEvent;

typedef void (*audio_track_event_callback_t)(AGTrackEvent event, void* user, void *info);

//Extra audio parameters for audio output/input settings.
typedef struct _AudioParameters
{
    int period_time;
    int buffer_time;
    AGTrackType track_type;
} AudioParameters;

class AGAudioTrack
{
public:
    virtual ~AGAudioTrack(){}
    /**
     * @brief Get preferred playback sample rate
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t getPreferredSampleRate();
    /**
     * @brief Get preferred playback sample format
     * @return AGSampleFormat
     */
    virtual AGSampleFormat getPreferredSampleFormat();
    /**
     * @brief Used to inform geniesdk underrun occured not requied
     * @param[in] cbf Callback for audio track event
     * @param[in] user Parameter which would be got again from callback
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t setCallback(audio_track_event_callback_t cbf, void* user);
    /**
     * @brief Create audio track
     * @param[in] sampleRate Audio pcm sample rate
     * @param[in] channelCount Audio pcm channel count
     * @param[in] format Audio pcm format
     * @param[in] trackType Audio track type
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t open(int32_t sampleRate, int32_t channelCount, int32_t format, int32_t trackType);
    /**
     * @brief Start audio track
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t start();
    /**
     * @brief Pause track, geniesdk will call start to make it active again
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t pause();
    /**
     * @brief Flush track when track in pause or stopped state
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t flush();
    /**
     * @brief Stop audio track, geniesdk will call start to make it active again
     *         stop will cause more to make it active to pause
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t stop();
    /**
     * @brief Delete audio track
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t close();
    /**
     * @brief Write pcm data to audio track
     * @param[in] buffer Pcm data buffer
     * @param[in] size Pcm data length
     * @return int32_t Actual write size
     */
    virtual int32_t write(uint8_t* buffer, int32_t size);
    //Extra parameters set for pcm device when open.
    virtual int32_t setAudioParameters(AudioParameters *params);
};


class AGAudioSystem
{
public:
    virtual ~AGAudioSystem(){}
    /**
     * @brief Get audio system mute status
     * @param[out] pfgMute Mute status
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t getMute(bool* pfgMute);
    /**
     * @brief Set audio system mute status
     * @param[in] fgMute Mute status to set
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t setMute(bool fgMute);
    /**
     * @brief Get audio system volume
     * @param[out] pi8Volume Actual volume
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t getVolume(int8_t* pi8Volume);
    /**
     * @brief Set audio system volume
     * @param[in] i8Volume Volume to set
     * @return int32_t 0 : success; other: fail
     */
    virtual int32_t setVolume(int8_t i8Volume);
};
#endif

