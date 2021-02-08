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

#include <alibabacloud/rtc/model/DescribeChannelUsersResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Rtc;
using namespace AlibabaCloud::Rtc::Model;

DescribeChannelUsersResult::DescribeChannelUsersResult() :
	ServiceResult()
{}

DescribeChannelUsersResult::DescribeChannelUsersResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeChannelUsersResult::~DescribeChannelUsersResult()
{}

void DescribeChannelUsersResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allUserList = value["UserList"]["User"];
	for (const auto &item : allUserList)
		userList_.push_back(item.asString());
	auto allInteractiveUserList = value["InteractiveUserList"]["InteractiveUser"];
	for (const auto &item : allInteractiveUserList)
		interactiveUserList_.push_back(item.asString());
	auto allLiveUserList = value["LiveUserList"]["LiveUser"];
	for (const auto &item : allLiveUserList)
		liveUserList_.push_back(item.asString());
	if(!value["Timestamp"].isNull())
		timestamp_ = std::stoi(value["Timestamp"].asString());
	if(!value["IsChannelExist"].isNull())
		isChannelExist_ = value["IsChannelExist"].asString() == "true";
	if(!value["ChannelProfile"].isNull())
		channelProfile_ = std::stoi(value["ChannelProfile"].asString());
	if(!value["CommTotalNum"].isNull())
		commTotalNum_ = std::stoi(value["CommTotalNum"].asString());
	if(!value["InteractiveUserNum"].isNull())
		interactiveUserNum_ = std::stoi(value["InteractiveUserNum"].asString());
	if(!value["LiveUserNum"].isNull())
		liveUserNum_ = std::stoi(value["LiveUserNum"].asString());

}

std::vector<std::string> DescribeChannelUsersResult::getInteractiveUserList()const
{
	return interactiveUserList_;
}

int DescribeChannelUsersResult::getLiveUserNum()const
{
	return liveUserNum_;
}

int DescribeChannelUsersResult::getChannelProfile()const
{
	return channelProfile_;
}

int DescribeChannelUsersResult::getInteractiveUserNum()const
{
	return interactiveUserNum_;
}

std::vector<std::string> DescribeChannelUsersResult::getUserList()const
{
	return userList_;
}

std::vector<std::string> DescribeChannelUsersResult::getLiveUserList()const
{
	return liveUserList_;
}

int DescribeChannelUsersResult::getTimestamp()const
{
	return timestamp_;
}

bool DescribeChannelUsersResult::getIsChannelExist()const
{
	return isChannelExist_;
}

int DescribeChannelUsersResult::getCommTotalNum()const
{
	return commTotalNum_;
}

