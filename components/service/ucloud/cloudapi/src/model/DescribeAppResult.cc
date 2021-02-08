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

#include <alibabacloud/cloudapi/model/DescribeAppResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudAPI;
using namespace AlibabaCloud::CloudAPI::Model;

DescribeAppResult::DescribeAppResult() :
	ServiceResult()
{}

DescribeAppResult::DescribeAppResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeAppResult::~DescribeAppResult()
{}

void DescribeAppResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	if(!value["AppId"].isNull())
		appId_ = std::stol(value["AppId"].asString());
	if(!value["AppName"].isNull())
		appName_ = value["AppName"].asString();
	if(!value["Description"].isNull())
		description_ = value["Description"].asString();
	if(!value["CreatedTime"].isNull())
		createdTime_ = value["CreatedTime"].asString();
	if(!value["ModifiedTime"].isNull())
		modifiedTime_ = value["ModifiedTime"].asString();

}

std::string DescribeAppResult::getDescription()const
{
	return description_;
}

std::string DescribeAppResult::getCreatedTime()const
{
	return createdTime_;
}

long DescribeAppResult::getAppId()const
{
	return appId_;
}

std::string DescribeAppResult::getModifiedTime()const
{
	return modifiedTime_;
}

std::string DescribeAppResult::getAppName()const
{
	return appName_;
}

