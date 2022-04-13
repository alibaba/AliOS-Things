/**
 * Copyright (C) 2018 Alibaba.inc, All rights reserved.
 *
 * @brief defines  for EOU detection.
 * @author lifen.lf@alibaba-inc.com
 * @data 2019/01/16
 * @version 1.0
 */

#ifndef __AG_EOU_DETECTION_DEF_H__
#define __AG_EOU_DETECTION_DEF_H__
enum VadStatus {
    VAD_SILENCE = 0,
    VAD_VOICE,
    VAD_STOP
};

enum AGASRStopPolicy {
    AGASR_STOP_POLICY_REFER_ASR = 1,
    AGASR_STOP_POLICY_REFER_ENERGY = 2,
    AGASR_STOP_POLICY_REFER_ENERGY_RADICAL
};

struct AGVadParam
{
public:
    AGVadParam():
        mVadPauseTime(700),
        mASRStopPolicy(AGASR_STOP_POLICY_REFER_ENERGY),
        mEnergyThreshold(0.9),
        mMinCVCnt(10),
        mContextCnt(2),
        mEnergyJudgeTime(1950)
        {}
    int32_t mVadPauseTime;
    int32_t mASRStopPolicy;
    float mEnergyThreshold;
    int32_t mMinCVCnt;
    int32_t mContextCnt;
    int32_t mEnergyJudgeTime;
};

struct AGSpeechRecognizerConfig
{
public:
    AGSpeechRecognizerConfig(){}
    AGSpeechRecognizerConfig(int32_t vadStartTime,
        int32_t pauseTime,
        int32_t vadSilenceTimeout1,
        int32_t vadSilenceTimeout2,
        int32_t maxRecordTime)
        :mVadStartTime(vadStartTime),
        mPauseTime(pauseTime),
        mVadSilenceTimeout1(vadSilenceTimeout1),
        mVadSilenceTimeout2(vadSilenceTimeout2),
        mMaxRecordTime(maxRecordTime){}
    int32_t getAddiPauseTime() const {
        return mPauseTime-mVadParam.mVadPauseTime;
    }
    int32_t mVadStartTime;
    int32_t mPauseTime;
    int32_t mVadSilenceTimeout1;
    int32_t mVadSilenceTimeout2;
    int32_t mMaxRecordTime;
    AGVadParam mVadParam;
};

enum AGAIInfo
{
    AGASR_INFO_ONESHOT = 80101,
    AGASR_INFO_VAD_TIME_OUT1 = 80201,
    AGASR_INFO_VAD_TIME_OUT2 = 80202,
    AGASR_INFO_VAD_TIME_OUT3 = 80203,
    AGASR_INFO_VAD_STOP
};

enum AGAIVadStopReason
{
    AGAIVSR_NONE = 0,
    AGAIVSR_VAD_STOP = 1,
    AGAIVSR_VAD_VOICE_TIME_OUT = 2,
    AGAIVSR_VAD_SILENCE_TIME_OUT = 3,
    AGAIVSR_VAD_SILENCE_AND_ASR_VALID = 4,
    AGAIVSR_VAD_ONE_SHOT_BY_ASR_WHEN_SILENCE = 5,
    AGAIVSR_VAD_ONE_SHOT_BY_ENERGY = 6,
    AGAIVSR_VAD_ONE_SHOT_BY_ASR = 7,
    AGAIVSR_VAD_STOP_BY_CLOUD_EOU = 8
};

typedef struct AGAIExtInfo
{
    int32_t version;
    int32_t i4SpeechLen;
    int32_t i4RemindFinishTime;
    AGAIVadStopReason sReason;
    AGVadParam vadParam;
    const char* pszAudioId;
} AGAIExtInfo_T;

enum AGAIError
{
    AGAI_ERROR_NONE = 0,
    AGAI_ERROR_UNKNOWN = -1,
    AGAI_ERROR_NETWORK = -2,
    AGAI_ERROR_NETWORK_TIMEOUT = -3,
    AGAI_LOCALASR_HIT = -4
};

#endif

