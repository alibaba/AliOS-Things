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


#include <sstream>
#include <alibabacloud/oss/model/ListMultipartUploadsRequest.h>
#include "../utils/Utils.h"
#include "ModelError.h"
#include <alibabacloud/oss/Const.h>

using namespace AlibabaCloud::OSS;
using std::stringstream;

ListMultipartUploadsRequest::ListMultipartUploadsRequest(const std::string &bucket):
    OssBucketRequest(bucket),
    delimiterIsSet_(false),
    keyMarkerIsSet_(false),
    prefixIsSet_(false),
    uploadIdMarkerIsSet_(false),
    encodingTypeIsSet_(false),
    maxUploadsIsSet_(false),
    requestPayer_(RequestPayer::NotSet)
{
}

void ListMultipartUploadsRequest::setDelimiter(const std::string &delimiter)
{
    delimiter_ = delimiter;
    delimiterIsSet_ = true;
}

void ListMultipartUploadsRequest::setMaxUploads(uint32_t maxUploads)
{
    maxUploads_ = maxUploads > MaxUploads ? MaxUploads : maxUploads;
    maxUploadsIsSet_ = true;
}

void ListMultipartUploadsRequest::setKeyMarker(const std::string &keyMarker)
{
    keyMarker_ = keyMarker;
    keyMarkerIsSet_ = true;
}

void ListMultipartUploadsRequest::setPrefix(const std::string &prefix)
{
    prefix_ = prefix;
    prefixIsSet_ = true;
}

void ListMultipartUploadsRequest::setUploadIdMarker(const std::string &uploadIdMarker)
{
    uploadIdMarker_ = uploadIdMarker;
    uploadIdMarkerIsSet_ = true;
}

void ListMultipartUploadsRequest::setEncodingType(const std::string &encodingType)
{
    encodingType_ = encodingType;
    encodingTypeIsSet_ = true;
}

void ListMultipartUploadsRequest::setRequestPayer(RequestPayer value)
{
    requestPayer_ = value;
}

ParameterCollection ListMultipartUploadsRequest::specialParameters() const
{
    ParameterCollection parameters;
    parameters["uploads"] = "";
    if(delimiterIsSet_){
        parameters["delimiter"] = delimiter_;
    }

    if (maxUploadsIsSet_) {
        parameters["max-uploads"] = std::to_string(maxUploads_);
    }

    if(keyMarkerIsSet_){
        parameters["key-marker"] = keyMarker_;
        if (uploadIdMarkerIsSet_) {
            parameters["upload-id-marker"] = uploadIdMarker_;
        }
    }

    if(prefixIsSet_){
        parameters["prefix"] = prefix_;
    }

    if(encodingTypeIsSet_){
        parameters["encoding-type"] = encodingType_;
    }
    return parameters;
}

HeaderCollection ListMultipartUploadsRequest::specialHeaders() const
{
    HeaderCollection headers;
    if (requestPayer_ == RequestPayer::Requester) {
        headers["x-oss-request-payer"] = ToLower(ToRequestPayerName(RequestPayer::Requester));
    }
    return headers;
}
