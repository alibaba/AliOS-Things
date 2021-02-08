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


#include <set>
#include <sstream>
#include <alibabacloud/oss/model/ListPartsRequest.h>
#include <alibabacloud/oss/http/HttpType.h>
#include "../utils/Utils.h"
#include "ModelError.h"
#include <alibabacloud/oss/Const.h>


using namespace AlibabaCloud::OSS;
using std::stringstream;

ListPartsRequest::ListPartsRequest(const std::string &bucket,
     const std::string &key) :
    ListPartsRequest(bucket, key, std::string())
{
}

ListPartsRequest::ListPartsRequest(const std::string &bucket,
    const std::string &key, const std::string &uploadId) :
    OssObjectRequest(bucket, key),
    uploadId_(uploadId),
    maxPartsIsSet_(false),
    partNumberMarkerIsSet_(false),
    encodingTypeIsSet_(false)
{
}

void ListPartsRequest::setUploadId(const std::string &uploadId)
{
    uploadId_ = uploadId;
}

void ListPartsRequest::setEncodingType(const std::string &str)
{
    encodingType_ = str;
    encodingTypeIsSet_ = true;
}

void ListPartsRequest::setMaxParts(uint32_t maxParts)
{
    maxParts_ = maxParts > MaxReturnedKeys ? MaxReturnedKeys: maxParts;
    maxPartsIsSet_ = true;
}

void ListPartsRequest::setPartNumberMarker(uint32_t partNumberMarker)
{
    partNumberMarker_ = partNumberMarker;
    partNumberMarkerIsSet_ = true;
}

ParameterCollection ListPartsRequest::specialParameters() const
{
    ParameterCollection parameters;
    parameters["uploadId"] = uploadId_;

    if (maxPartsIsSet_) {
        parameters["max-parts"] = std::to_string(maxParts_);
    }

    if (partNumberMarkerIsSet_) {
        parameters["part-number-marker"] = std::to_string(partNumberMarker_);
    }

    if (encodingTypeIsSet_) {
        parameters["encoding-type"] = encodingType_;
    }

    return parameters;
}
