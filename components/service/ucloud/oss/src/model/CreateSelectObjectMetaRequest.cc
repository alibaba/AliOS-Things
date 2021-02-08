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

#include <alibabacloud/oss/model/CreateSelectObjectMetaRequest.h>
#include <sstream>
#include "ModelError.h"
#include "../utils/Utils.h"

using namespace AlibabaCloud::OSS;

CreateSelectObjectMetaRequest::CreateSelectObjectMetaRequest(const std::string& bucket, const std::string& key) :
    OssObjectRequest(bucket, key), 
    inputFormat_(nullptr), 
    overWriteIfExists_(false)
{
    setFlags(Flags() | REQUEST_FLAG_CONTENTMD5);
}

void CreateSelectObjectMetaRequest::setOverWriteIfExists(bool overWriteIfExists)
{
    overWriteIfExists_ = overWriteIfExists;
}

void CreateSelectObjectMetaRequest::setInputFormat(InputFormat& inputFormat)
{
    inputFormat_ = &inputFormat;
}

std::string CreateSelectObjectMetaRequest::payload() const
{
    std::stringstream ss;
    if (inputFormat_->Type() == "csv") {
        ss << "<CsvMetaRequest>" << std::endl;
        ss << inputFormat_->toXML(0) << std::endl;
        ss << "<OverwriteIfExists>" << (overWriteIfExists_ ? "true" : "false") << "</OverwriteIfExists>" << std::endl;
        ss << "</CsvMetaRequest>" << std::endl;
    }
    else {
        ss << "<JsonMetaRequest>" << std::endl;
        ss << inputFormat_->toXML(0) << std::endl;
        ss << "<OverwriteIfExists>" << (overWriteIfExists_ ? "true" : "false") << "</OverwriteIfExists>" << std::endl;
        ss << "</JsonMetaRequest>" << std::endl;
    }
    return ss.str();
}

int CreateSelectObjectMetaRequest::validate() const
{
    int ret = OssObjectRequest::validate();
    if (ret != 0) {
        return ret;
    }
    if (inputFormat_ == nullptr) {
        return ARG_ERROR_CREATE_SELECT_OBJECT_META_NULL_POINT;
    }
    return 0;
}

ParameterCollection CreateSelectObjectMetaRequest::specialParameters() const
{
    ParameterCollection parameters;
    if (inputFormat_) {
        auto type = inputFormat_->Type();
        type.append("/meta");
        parameters["x-oss-process"] = type;
    }
    return parameters;
}
