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

#include "ModelError.h"

using namespace AlibabaCloud::OSS;

static const char * GetArgErrorMsg(const int code)
{
    static const char * msg[] =
    {
        "Argument is invalid, please check.",
        /*Common 1-2*/
        "The bucket name is invalid. A bucket name must  be comprised of lower-case characters, numbers or dash(-) with 3-63 characters long.",
        "The object key is invalid. An object name should be between 1-1023 bytes long and cannot begin with '/' or '\\'",
        /*CORS   3-10*/
        "One bucket not allow exceed ten item of CORSRules.",
        "CORSRule.AllowedOrigins should not be empty.",
        "CORSRule.AllowedOrigins allowes at most one asterisk wildcard.",
        "CORSRule.AllowedMethods should not be empty.",
        "CORSRule.AllowedMethods only supports GET/PUT/DELETE/POST/HEAD.",
        "CORSRule.AllowedHeaders allowes at most one asterisk wildcard.",
        "CORSRule.ExposedHeader dose not allowe asterisk wildcard.",
        "CORSRule.MaxAgeSeconds should not be less than 0 or greater than 999999999.",
        /*Logging -11*/
        "Invalid logging prefix.",
        /*storageCapacity -12*/
        "Storage capacity must greater than -1.",
        /*WebSiet -13*/
        "Index document must not be empty.",
        "Invalid index document, must be end with.html.",
        "Invalid error document, must be end with .html.",
        /*iostream request body -16*/
        "Request body is null.",
        "Request body is in fail state. Logical error on i/o operation.",
        "Request body is in bad state. Read/writing error on i/o operation.",
        /*MultipartUpload -19*/
        "PartList is empty.",
        "PartSize should not be less than 100*1024 or greater than 5*1024*1024*1024.",
        "PartNumber should not be less than 1 or greater than 10000.",
        /*Lifecycle Rules -22*/
        "One bucket not allow exceed one thousand item of LifecycleRules.",
        "LifecycleRule should not be null or empty.",
        "Only one expiration property should be specified.",
        "You have a rule for a prefix, and therefore you cannot create the rule for the whole bucket.",
        "Configure at least one of file and fragment lifecycle.",
        /*ResumableUpload -27*/
        "The path of file to upload is empty.",
        "Open upload file failed.",
        "The part size is less than 100KB.",
        "The thread num is less than 1.",
        "Checkpoint directory is not exist.",
        "Parse resumable upload record failed.",
        "Upload file has been modified since last upload.",
        "Upload record is invalid, it has been modified.",
        /*ResumableCopy -35*/
        "Parse resumable copy record failed.",
        "Source object has been modified since last copy.",
        "Copy record is invalid, it has been modified.",
        /*ResumableDownload -38*/
        "Invalid range of resumable download",
        "The path of file download to is empty",
        "Source object has been modified since last download.",
        "Parse resumable download record failed.",
        "invalid range values in download record record file.",
        "Range values has been modified since last download.",
        "Open temp file for download failed",
        /*GetObject -45*/
        "The range is invalid. The start should not be less than 0 or less then the end. The end could be -1 to get the rest of the data.",
        /*LiveChannel -46*/
        "The status param is invalid, it must be 'enabled' or 'disabled' ",
        "The channelName param is invalid, it shouldn't contain '/' and length < 1023",
        "The dest bucket name is invalid",
        "The live channel description is invalied, it should be shorter than 129",
        "The channel type is invalid, it shoudld only be HLS",
        "The live channel frag duration param is invalid, it should be [1,100]",
        "The live channel frag count param is invalid, it should be [1,100]",
        "The live channel play list param is invalid, it should end with '.m3u8' & length in [6,128]",
        "The snapshot param is invalid, please check.",
        "The time param is invalid, endTime should bigger than startTime and difference smaller than 24*60*60",
        "The Max Key param is invalid, it's default valus is 100, and smaller than 1000",
        /*SelectObject -57*/
        "The range is invalid. It cannot set lien range and split range at the same time.",
        "The line range is invalid. The start should not be less than 0 or less then the end. The end could be -1 to get the rest of the data.",
        "The split range is invalid. The start should not be less than 0 or less then the end. The end could be -1 to get the rest of the data.",
        "The select object expressiontype must is SQL.",
        "The select object content checksum failed.",
        "The request InputFormat/OutputFormat is invalid. It cannot set InputFormat or OutputFormat as nullptr.",
        "The request InputFormat/OutputFormat is invalid. It cannot set InputFormat and OutputFormat in difficent type.",
        /*CreateSelectObject -64*/
        "The request InputFormat is invalid. It cannot set InputFormat as nullptr.",
        /*Tagging -65*/
        "Object tags cannot be greater than 10.",
        "Object Tag key is invalid, it's length should be [1, 128].",
        "Object Tag value is invalid, it's length should be less than 256.",
        /*Resumable for wstring path -68*/
        "Only support wstring path in windows os.",
        "The type of filePath and checkpointDir should be the same, either string or wstring."
    };

    int index = code - ARG_ERROR_START;
    int msg_size = sizeof(msg)/sizeof(msg[0]);
    if (code < ARG_ERROR_START || index > msg_size) {
        index = 0;
    }

    return msg[index];
}


const char * AlibabaCloud::OSS::GetModelErrorMsg(const int code)
{
    if (code >= ARG_ERROR_START && code <= ARG_ERROR_END) {
        return GetArgErrorMsg(code);
    }

    return "Model error, but undefined.";
}
