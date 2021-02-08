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


#include <alibabacloud/oss/model/ObjectCallbackBuilder.h>
#include <sstream>
#include "../utils/Utils.h"

using namespace AlibabaCloud::OSS;

ObjectCallbackBuilder::ObjectCallbackBuilder(const std::string &url, const std::string &body):
    ObjectCallbackBuilder(url, body, "", Type::URL)
{
}

ObjectCallbackBuilder::ObjectCallbackBuilder(const std::string &url, const std::string &body,
    const std::string &host, Type type):
    callbackUrl_(url),
    callbackHost_(host),
    callbackBody_(body),
    callbackBodyType_(type)
{
}

std::string ObjectCallbackBuilder::build()
{
    if (callbackUrl_.empty() || callbackBody_.empty())
    {
        return "";
    }

    std::stringstream ss;
    ss << "{";
    ss << "\"callbackUrl\":\"" << callbackUrl_ << "\"";
    if (!callbackHost_.empty())
    {
        ss << ",\"callbackHost\":\"" << callbackHost_ << "\"";
    }
    ss << ",\"callbackBody\":\"" << callbackBody_ << "\"";

    if (callbackBodyType_ == Type::JSON)
    {
        ss << ",\"callbackBodyType\":\"" << "application/json" << "\"";
    }

    ss << "}";

    return Base64Encode(ss.str());
}


bool ObjectCallbackVariableBuilder::addCallbackVariable(const std::string &key, const std::string &value)
{
    if (!!key.compare(0, 2 , "x:", 2))
    {
        return false;
    }
    callbackVariable_[key] = value;
    return true;
}

std::string ObjectCallbackVariableBuilder::build()
{
    if (callbackVariable_.size() == 0)
    {
        return "";
    }

    std::stringstream ss;
    ss << "{";
    int i = 0;
    for (auto const& var : callbackVariable_) {
        if (i > 0) {
            ss << ",";
        }
        ss << "\"" << var.first << "\":\"" << var.second << "\"";
        i++;
    }
    ss << "}";
    return Base64Encode(ss.str());
}
