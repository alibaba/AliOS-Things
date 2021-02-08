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

#include <alibabacloud/live/model/ApplyRecordTokenResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

ApplyRecordTokenResult::ApplyRecordTokenResult() :
	ServiceResult()
{}

ApplyRecordTokenResult::ApplyRecordTokenResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ApplyRecordTokenResult::~ApplyRecordTokenResult()
{}

void ApplyRecordTokenResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	if(!value["SecurityToken"].isNull())
		securityToken_ = value["SecurityToken"].asString();
	if(!value["AccessKeySecret"].isNull())
		accessKeySecret_ = value["AccessKeySecret"].asString();
	if(!value["AccessKeyId"].isNull())
		accessKeyId_ = value["AccessKeyId"].asString();
	if(!value["Expiration"].isNull())
		expiration_ = value["Expiration"].asString();

}

std::string ApplyRecordTokenResult::getSecurityToken()const
{
	return securityToken_;
}

std::string ApplyRecordTokenResult::getAccessKeyId()const
{
	return accessKeyId_;
}

std::string ApplyRecordTokenResult::getAccessKeySecret()const
{
	return accessKeySecret_;
}

std::string ApplyRecordTokenResult::getExpiration()const
{
	return expiration_;
}

