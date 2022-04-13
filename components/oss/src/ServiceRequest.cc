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

#include <alibabacloud/oss/ServiceRequest.h>
#include <sstream>

using namespace AlibabaCloud::OSS;

ServiceRequest::ServiceRequest() :
    flags_(0),
    path_("/"),
    responseStreamFactory_([] { return std::make_shared<std::stringstream>(); })
{
    transferProgress_.Handler = nullptr;
    transferProgress_.UserData = nullptr;
}

std::string ServiceRequest::Path() const
{
    return path_;
}

int ServiceRequest::Flags() const 
{ 
    return flags_; 
}

const IOStreamFactory& ServiceRequest::ResponseStreamFactory() const 
{ 
    return responseStreamFactory_; 
}

void ServiceRequest::setResponseStreamFactory(const IOStreamFactory& factory) 
{
    responseStreamFactory_ = factory; 
}

const AlibabaCloud::OSS::TransferProgress & ServiceRequest::TransferProgress() const 
{
    return transferProgress_; 
}

void ServiceRequest::setTransferProgress(const AlibabaCloud::OSS::TransferProgress &arg) 
{ 
    transferProgress_ = arg; 
}

void ServiceRequest::setPath(const std::string & path)
{
    path_ = path;
}

void ServiceRequest::setFlags(int flags)
{
    flags_ = flags;
}
