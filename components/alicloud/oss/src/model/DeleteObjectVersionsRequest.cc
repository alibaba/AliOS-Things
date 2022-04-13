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


#include <alibabacloud/oss/model/DeleteObjectVersionsRequest.h>
#include <sstream>
#include "utils/Utils.h"

using namespace AlibabaCloud::OSS;

DeleteObjectVersionsRequest::DeleteObjectVersionsRequest(const std::string &bucket) :
    OssBucketRequest(bucket),
    quiet_(false),
    encodingType_(),
    requestPayer_(RequestPayer::NotSet)
{
    setFlags(Flags() | REQUEST_FLAG_CONTENTMD5);
}

bool DeleteObjectVersionsRequest::Quiet() const
{
    return quiet_;
}

const std::string &DeleteObjectVersionsRequest::EncodingType() const
{
    return encodingType_;
}

void DeleteObjectVersionsRequest::addObject(const ObjectIdentifier& object)
{
    objects_.push_back(object);
}

void DeleteObjectVersionsRequest::setObjects(const ObjectIdentifierList& objects)
{
    objects_ = objects;
}

const ObjectIdentifierList& DeleteObjectVersionsRequest::Objects() const
{
    return objects_;
}

void DeleteObjectVersionsRequest::clearObjects()
{
    objects_.clear();
}

void DeleteObjectVersionsRequest::setQuiet(bool quiet)
{
    quiet_ = quiet;
}

void DeleteObjectVersionsRequest::setEncodingType(const std::string &value)
{
    encodingType_ = value;
}

void DeleteObjectVersionsRequest::setRequestPayer(RequestPayer value)
{
    requestPayer_ = value; 
}

std::string DeleteObjectVersionsRequest::payload() const
{
    std::stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    ss << "<Delete>" << std::endl;
    ss << "  <Quiet>" << (quiet_ ? "true":"false") << "</Quiet>" << std::endl;
    for (auto const& object : objects_) {
    ss << "  <Object>" << std::endl << ""; 
    ss << "    <Key>" << XmlEscape(object.Key()) << "</Key>" << std::endl;
    if (!object.VersionId().empty()) {
    ss << "    <VersionId>" << object.VersionId() << "</VersionId>" << std::endl;
    }
    ss << "  </Object>" << std::endl;
    }
    ss << "</Delete>" << std::endl;
    return ss.str();
}

ParameterCollection DeleteObjectVersionsRequest::specialParameters() const
{
    ParameterCollection parameters;
    parameters["delete"] = "";
    if (!encodingType_.empty()) {
        parameters["encoding-type"] = encodingType_;
    }
    return parameters;
}

HeaderCollection DeleteObjectVersionsRequest::specialHeaders() const
{
    HeaderCollection headers;
    if (requestPayer_ == RequestPayer::Requester) {
        headers["x-oss-request-payer"] = ToLower(ToRequestPayerName(RequestPayer::Requester));
    }
    return headers;
}
