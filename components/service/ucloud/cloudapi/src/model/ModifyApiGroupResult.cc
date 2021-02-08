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

#include <alibabacloud/cloudapi/model/ModifyApiGroupResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudAPI;
using namespace AlibabaCloud::CloudAPI::Model;

ModifyApiGroupResult::ModifyApiGroupResult() :
	ServiceResult()
{}

ModifyApiGroupResult::ModifyApiGroupResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ModifyApiGroupResult::~ModifyApiGroupResult()
{}

void ModifyApiGroupResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	if(!value["GroupId"].isNull())
		groupId_ = value["GroupId"].asString();
	if(!value["GroupName"].isNull())
		groupName_ = value["GroupName"].asString();
	if(!value["SubDomain"].isNull())
		subDomain_ = value["SubDomain"].asString();
	if(!value["Description"].isNull())
		description_ = value["Description"].asString();

}

std::string ModifyApiGroupResult::getGroupName()const
{
	return groupName_;
}

std::string ModifyApiGroupResult::getSubDomain()const
{
	return subDomain_;
}

std::string ModifyApiGroupResult::getDescription()const
{
	return description_;
}

std::string ModifyApiGroupResult::getGroupId()const
{
	return groupId_;
}

