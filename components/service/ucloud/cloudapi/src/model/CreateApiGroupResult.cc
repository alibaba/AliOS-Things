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

#include <alibabacloud/cloudapi/model/CreateApiGroupResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudAPI;
using namespace AlibabaCloud::CloudAPI::Model;

CreateApiGroupResult::CreateApiGroupResult() :
	ServiceResult()
{}

CreateApiGroupResult::CreateApiGroupResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

CreateApiGroupResult::~CreateApiGroupResult()
{}

void CreateApiGroupResult::parse(const std::string &payload)
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
	if(!value["InstanceId"].isNull())
		instanceId_ = value["InstanceId"].asString();
	if(!value["InstanceType"].isNull())
		instanceType_ = value["InstanceType"].asString();
	if(!value["TagStatus"].isNull())
		tagStatus_ = value["TagStatus"].asString() == "true";

}

std::string CreateApiGroupResult::getGroupName()const
{
	return groupName_;
}

std::string CreateApiGroupResult::getSubDomain()const
{
	return subDomain_;
}

std::string CreateApiGroupResult::getDescription()const
{
	return description_;
}

std::string CreateApiGroupResult::getInstanceId()const
{
	return instanceId_;
}

bool CreateApiGroupResult::getTagStatus()const
{
	return tagStatus_;
}

std::string CreateApiGroupResult::getInstanceType()const
{
	return instanceType_;
}

std::string CreateApiGroupResult::getGroupId()const
{
	return groupId_;
}

