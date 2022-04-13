/*
 * Copyright 2009-2017 Alibaba Cloud All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include <alibabacloud/oss/client/Error.h>

namespace AlibabaCloud
{
namespace OSS
{
	const char * GetModelErrorMsg(const int code);

    /*Error For Argument Check*/
    const int ARG_ERROR_BASE   = ERROR_CLIENT_BASE + 1000;
    const int ARG_ERROR_START  = ARG_ERROR_BASE;
    const int ARG_ERROR_END    = ARG_ERROR_START +  999;

    /*Default*/
    const int ARG_ERROR_DEFAULT     = ARG_ERROR_BASE + 0;
    /*Common*/
    const int ARG_ERROR_BUCKET_NAME = ARG_ERROR_BASE + 1;
    const int ARG_ERROR_OBJECT_NAME = ARG_ERROR_BASE + 2;
    /*CORS*/
    const int ARG_ERROR_CORS_RULE_LIMIT                    = ARG_ERROR_BASE + 3;
    const int ARG_ERROR_CORS_ALLOWEDORIGINS_EMPTY          = ARG_ERROR_BASE + 4;
    const int ARG_ERROR_CORS_ALLOWEDORIGINS_ASTERISK_COUNT = ARG_ERROR_BASE + 5;
    const int ARG_ERROR_CORS_ALLOWEDMETHODS_EMPTY          = ARG_ERROR_BASE + 6;
    const int ARG_ERROR_CORS_ALLOWEDMETHODS_VALUE          = ARG_ERROR_BASE + 7;
    const int ARG_ERROR_CORS_ALLOWEDHEADERS_ASTERISK_COUNT = ARG_ERROR_BASE + 8;
    const int ARG_ERROR_CORS_EXPOSEHEADERS_ASTERISK_COUNT  = ARG_ERROR_BASE + 9;
    const int ARG_ERROR_CORS_MAXAGESECONDS_RANGE           = ARG_ERROR_BASE + 10;
    /*Logging*/
    const int ARG_ERROR_LOGGING_TARGETPREFIX_INVALID  = ARG_ERROR_BASE + 11;
    /*StorageCapacity*/
    const int ARG_ERROR_STORAGECAPACITY_INVALID = ARG_ERROR_BASE + 12;
    /*WebSiet*/
    const int ARG_ERROR_WEBSITE_INDEX_DOCCUMENT_EMPTY        = ARG_ERROR_BASE + 13;
    const int ARG_ERROR_WEBSITE_INDEX_DOCCUMENT_NAME_INVALID = ARG_ERROR_BASE + 14;
    const int ARG_ERROR_WEBSITE_ERROR_DOCCUMENT_NAME_INVALID = ARG_ERROR_BASE + 15;
    /*iostream request body*/
    const int ARG_ERROR_REQUEST_BODY_NULLPTR    = ARG_ERROR_BASE + 16;
    const int ARG_ERROR_REQUEST_BODY_FAIL_STATE = ARG_ERROR_BASE + 17;
    const int ARG_ERROR_REQUEST_BODY_BAD_STATE  = ARG_ERROR_BASE + 18;
    /*MultipartUpload*/
    const int ARG_ERROR_MULTIPARTUPLOAD_PARTLIST_EMPTY   = ARG_ERROR_BASE + 19;
    const int ARG_ERROR_MULTIPARTUPLOAD_PARTSIZE_RANGE   = ARG_ERROR_BASE + 20;
    const int ARG_ERROR_MULTIPARTUPLOAD_PARTNUMBER_RANGE = ARG_ERROR_BASE + 21;
    /*Lifecycle Rules*/
    const int ARG_ERROR_LIFECYCLE_RULE_LIMIT       = ARG_ERROR_BASE + 22;
    const int ARG_ERROR_LIFECYCLE_RULE_EMPTY       = ARG_ERROR_BASE + 23;
    const int ARG_ERROR_LIFECYCLE_RULE_EXPIRATION  = ARG_ERROR_BASE + 24;
    const int ARG_ERROR_LIFECYCLE_RULE_ONLY_ONE_FOR_BUCKET = ARG_ERROR_BASE + 25;
    const int ARG_ERROR_LIFECYCLE_RULE_CONFIG_EMPTY = ARG_ERROR_BASE + 26;
    
    /*Resumable Upload*/
    const int ARG_ERROR_UPLOAD_FILE_PATH_EMPTY = ARG_ERROR_BASE + 27;
    const int ARG_ERROR_OPEN_UPLOAD_FILE = ARG_ERROR_BASE + 28;
    const int ARG_ERROR_CHECK_PART_SIZE_LOWER = ARG_ERROR_BASE + 29;
    const int ARG_ERROR_CHECK_THREAD_NUM_LOWER = ARG_ERROR_BASE + 30;
    const int ARG_ERROR_CHECK_POINT_DIR_NONEXIST = ARG_ERROR_BASE + 31;
    const int ARG_ERROR_PARSE_UPLOAD_RECORD_FILE = ARG_ERROR_BASE + 32;
    const int ARG_ERROR_UPLOAD_FILE_MODIFIED = ARG_ERROR_BASE + 33;
    const int ARG_ERROR_UPLOAD_RECORD_INVALID = ARG_ERROR_BASE + 34;
    
    /*Resumable Copy*/
    const int ARG_ERROR_PARSE_COPY_RECORD_FILE = ARG_ERROR_BASE + 35;
    const int ARG_ERROR_COPY_SRC_OBJECT_MODIFIED = ARG_ERROR_BASE + 36;
    const int ARG_ERROR_COPY_RECORD_INVALID = ARG_ERROR_BASE + 37;
    
    /*Resumable Download*/
    const int ARG_ERROR_INVALID_RANGE = ARG_ERROR_BASE + 38;
    const int ARG_ERROR_DOWNLOAD_FILE_PATH_EMPTY = ARG_ERROR_BASE + 39;
    const int ARG_ERROR_DOWNLOAD_OBJECT_MODIFIED = ARG_ERROR_BASE + 40;
    const int ARG_ERROR_PARSE_DOWNLOAD_RECORD_FILE = ARG_ERROR_BASE + 41;
    const int ARG_ERROR_INVALID_RANGE_IN_DWONLOAD_RECORD = ARG_ERROR_BASE + 42;
    const int ARG_ERROR_RANGE_HAS_BEEN_RESET = ARG_ERROR_BASE + 43;
    const int ARG_ERROR_OPEN_DOWNLOAD_TEMP_FILE = ARG_ERROR_BASE + 44;

    /*GetObject*/
    const int ARG_ERROR_OBJECT_RANGE_INVALID = ARG_ERROR_BASE + 45;
    /*LiveChannel*/
    const int ARG_ERROR_LIVECHANNEL_BAD_STATUS_PARAM = ARG_ERROR_BASE + 46;
    const int ARG_ERROR_LIVECHANNEL_BAD_CHANNELNAME_PARAM = ARG_ERROR_BASE + 47;
    const int ARG_ERROR_LIVECHANNEL_BAD_DEST_BUCKET_PARAM = ARG_ERROR_BASE + 48;
    const int ARG_ERROR_LIVECHANNEL_BAD_DESCRIPTION_PARAM = ARG_ERROR_BASE + 49;
    const int ARG_ERROR_LIVECHANNEL_BAD_CHANNEL_TYPE_PARAM = ARG_ERROR_BASE + 50;
    const int ARG_ERROR_LIVECHANNEL_BAD_FRAGDURATION_PARAM = ARG_ERROR_BASE + 51;
    const int ARG_ERROR_LIVECHANNEL_BAD_FRAGCOUNT_PARAM = ARG_ERROR_BASE + 52;
    const int ARG_ERROR_LIVECHANNEL_BAD_PALYLIST_PARAM = ARG_ERROR_BASE + 53;
    const int ARG_ERROR_LIVECHANNEL_BAD_SNAPSHOT_PARAM = ARG_ERROR_BASE + 54;
    const int ARG_ERROR_LIVECHANNEL_BAD_TIME_PARAM = ARG_ERROR_BASE + 55;
    const int ARG_ERROR_LIVECHANNEL_BAD_MAXKEY_PARAM = ARG_ERROR_BASE + 56;

    /*SelectObject*/
    const int ARG_ERROR_SELECT_OBJECT_RANGE_INVALID = ARG_ERROR_BASE + 57;
    const int ARG_ERROR_SELECT_OBJECT_LINE_RANGE_INVALID = ARG_ERROR_BASE + 58;
    const int ARG_ERROR_SELECT_OBJECT_SPLIT_RANGE_INVALID = ARG_ERROR_BASE + 59;
    const int ARG_ERROR_SELECT_OBJECT_NOT_SQL_EXPRESSION = ARG_ERROR_BASE + 60;
    const int ARG_ERROR_SELECT_OBJECT_CHECK_SUM_FAILED = ARG_ERROR_BASE + 61;
    const int ARG_ERROR_SELECT_OBJECT_NULL_POINT = ARG_ERROR_BASE + 62;
    const int ARG_ERROR_SELECT_OBJECT_PROCESS_NOT_SAME = ARG_ERROR_BASE + 63;

    /*CreateSelectObjectMeta*/
    const int ARG_ERROR_CREATE_SELECT_OBJECT_META_NULL_POINT = ARG_ERROR_BASE + 64;

    /*Tagging*/
    const int ARG_ERROR_TAGGING_TAGS_LIMIT = ARG_ERROR_BASE + 65;
    const int ARG_ERROR_TAGGING_TAG_KEY_LIMIT = ARG_ERROR_BASE + 66;
    const int ARG_ERROR_TAGGING_TAG_VALUE_LIMIT = ARG_ERROR_BASE + 67;

    /*Resumable for wstring path*/
    const int ARG_ERROR_PATH_NOT_SUPPORT_WSTRING_TYPE = ARG_ERROR_BASE + 68;
    const int ARG_ERROR_PATH_NOT_SAME_TYPE = ARG_ERROR_BASE + 69;
}
}

