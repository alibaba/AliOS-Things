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

#include <alibabacloud/rtc/model/DescribeUserInfoInChannelResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Rtc;
using namespace AlibabaCloud::Rtc::Model;

DescribeUserInfoInChannelResult::DescribeUserInfoInChannelResult() :
	ServiceResult()
{}

DescribeUserInfoInChannelResult::DescribeUserInfoInChannelResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeUserInfoInChannelResult::~DescribeUserInfoInChannelResult()
{}

void DescribeUserInfoInChannelResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allPropertyNode = value["Property"]["PropertyItem"];
	for (auto valuePropertyPropertyItem : allPropertyNode)
	{
		PropertyItem propertyObject;
		if(!valuePropertyPropertyItem["Session"].isNull())
			propertyObject.session = valuePropertyPropertyItem["Session"].asString();
		if(!valuePropertyPropertyItem["Join"].isNull())
			propertyObject.join = std::stoi(valuePropertyPropertyItem["Join"].asString());
		if(!valuePropertyPropertyItem["Role"].isNull())
			propertyObject.role = std::stoi(valuePropertyPropertyItem["Role"].asString());
		property_.push_back(propertyObject);
	}
	if(!value["Timestamp"].isNull())
		timestamp_ = std::stoi(value["Timestamp"].asString());
	if(!value["IsChannelExist"].isNull())
		isChannelExist_ = value["IsChannelExist"].asString() == "true";
	if(!value["IsInChannel"].isNull())
		isInChannel_ = value["IsInChannel"].asString() == "true";

}

bool DescribeUserInfoInChannelResult::getIsInChannel()const
{
	return isInChannel_;
}

std::vector<DescribeUserInfoInChannelResult::PropertyItem> DescribeUserInfoInChannelResult::getProperty()const
{
	return property_;
}

int DescribeUserInfoInChannelResult::getTimestamp()const
{
	return timestamp_;
}

bool DescribeUserInfoInChannelResult::getIsChannelExist()const
{
	return isChannelExist_;
}

