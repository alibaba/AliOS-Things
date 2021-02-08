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

#include <alibabacloud/oss/auth/Credentials.h>

using namespace AlibabaCloud::OSS;

Credentials::Credentials(const std::string &accessKeyId,
        const std::string &accessKeySecret,
        const std::string &sessionToken) :
    accessKeyId_(accessKeyId),
    accessKeySecret_(accessKeySecret),
    sessionToken_(sessionToken)
{
}

Credentials::~Credentials()
{
}

const std::string& Credentials::AccessKeyId () const
{
    return accessKeyId_;
}

const std::string& Credentials::AccessKeySecret () const
{
    return accessKeySecret_;
}

const std::string& Credentials::SessionToken () const
{
    return sessionToken_;
}

void Credentials::setAccessKeyId(const std::string &accessKeyId)
{
    accessKeyId_ = accessKeyId;
}

void Credentials::setAccessKeySecret(const std::string &accessKeySecret)
{
    accessKeySecret_ = accessKeySecret;
}

void Credentials::setSessionToken (const std::string &sessionToken)
{
    sessionToken_ = sessionToken;
}

