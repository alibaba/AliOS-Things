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


#include <alibabacloud/oss/model/DeleteObjectsRequest.h>
#include <sstream>
#include "../utils/Utils.h"

using namespace AlibabaCloud::OSS;

DeleteObjectsRequest::DeleteObjectsRequest(const std::string &bucket) :
    OssBucketRequest(bucket),
    quiet_(false),
    encodingType_(),
    requestPayer_(RequestPayer::NotSet)
{
    setFlags(Flags() | REQUEST_FLAG_CONTENTMD5);
}

bool DeleteObjectsRequest::Quiet() const
{
    return quiet_;
}

const std::string &DeleteObjectsRequest::EncodingType() const
{
    return encodingType_;
}

const std::list<std::string> &DeleteObjectsRequest::KeyList() const
{
    return keyList_;
}

void DeleteObjectsRequest::setQuiet(bool quiet)
{
    quiet_ = quiet;
}

void DeleteObjectsRequest::setEncodingType(const std::string &value)
{
    encodingType_ = value;
}

void DeleteObjectsRequest::addKey(const std::string &key)
{
    keyList_.push_back(key);
}

void DeleteObjectsRequest::setKeyList(const DeletedKeyList &keyList)
{
    keyList_ = keyList;
}

void DeleteObjectsRequest::clearKeyList()
{
    keyList_.clear();
}

void DeleteObjectsRequest::setRequestPayer(RequestPayer value)
{
    requestPayer_ = value; 
}

std::string DeleteObjectsRequest::payload() const
{
    std::stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    ss << "<Delete>" << std::endl;
    ss << "  <Quiet>" << (quiet_ ? "true":"false") << "</Quiet>" << std::endl;
    for (auto const &key : keyList_) {
    ss << "  <Object>" << std::endl << ""; 
    ss << "    <Key>" << XmlEscape(key) << "</Key>" << std::endl;
    ss << "  </Object>" << std::endl;
    }
    ss << "</Delete>" << std::endl;
    return ss.str();
}

ParameterCollection DeleteObjectsRequest::specialParameters() const
{
    ParameterCollection parameters;
    parameters["delete"] = "";
    if (!encodingType_.empty()) {
        parameters["encoding-type"] = encodingType_;
    }
    return parameters;
}

HeaderCollection DeleteObjectsRequest::specialHeaders() const
{
    HeaderCollection headers;
    if (requestPayer_ == RequestPayer::Requester) {
        headers["x-oss-request-payer"] = ToLower(ToRequestPayerName(RequestPayer::Requester));
    }
    return headers;
}
