/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : CdxEnumCommon.h
 * Description : helper for some public enum type
 * History :
 *
 */

#ifndef CDX_ENUM_COMMON_H
#define CDX_ENUM_COMMON_H

enum SPLIT_PUBLIC_ENUM_RANGE {
    MEDIA_EVENT_VALID_RANGE_MIN = 0,
    MEDIA_EVENT_VALID_RANGE_MAX = 255,

    PARSER_NOTIFY_VALID_RANGE_MIN = 256,
    PARSER_NOTIFY_VALID_RANGE_MAX = 500,

    DEMUX_NOTIFY_VALID_RANGE_MIN = 512,
    DEMUX_NOTIFY_VALID_RANGE_MAX = 1000,

    PLAYBACK_NOTIFY_VALID_RANGE_MIN = 1024,
    PLAYBACK_NOTIFY_VALID_RANGE_MAX = 1500,

    SUBTITLE_CALLBACK_VALID_RANGE_MIN = 1600,
    SUBTITLE_CALLBACK_VALID_RANGE_MAX = 2000,

    STREAM_EVENT_VALID_RANGE_MIN = 4096,
    STREAM_EVENT_VALID_RANGE_MAX = 4500,
};

#define CHECK_MEDIA_EVENT_MAX_VALID(x) \
    typedef char MEDIA_EVENT_MAX_VALID[(int)MEDIA_EVENT_VALID_RANGE_MAX - (int)x];

#define CHECK_PARSER_NOTIFY_MAX_VALID(x) \
    typedef char PARSER_NOTIFY_MAX_VALID[(int)PARSER_NOTIFY_VALID_RANGE_MAX - (int)x];

#define CHECK_DEMUX_NOTIFY_MAX_VALID(x) \
    typedef char DEMUX_NOTIFY_MAX_VALID[(int)DEMUX_NOTIFY_VALID_RANGE_MAX - (int)x];

#define CHECK_PLAYBACK_NOTIFY_MAX_VALID(x) \
    typedef char PLAYBACK_NOTIFY_MAX_VALID[(int)PLAYBACK_NOTIFY_VALID_RANGE_MAX - (int)x];

#define CHECK_SUBTITLE_CALLBACK_MAX_VALID(x) \
    typedef char SUBTITLE_CALLBACK_MAX_VALID[(int)SUBTITLE_CALLBACK_VALID_RANGE_MAX - (int)x];

#define CHECK_STREAM_EVENT_MAX_VALID(x) \
    typedef char STREAM_EVENT_MAX_VALID[(int)STREAM_EVENT_VALID_RANGE_MAX - (int)x];

#endif
