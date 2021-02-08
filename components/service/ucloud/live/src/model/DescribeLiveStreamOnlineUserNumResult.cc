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

#include <alibabacloud/live/model/DescribeLiveStreamOnlineUserNumResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeLiveStreamOnlineUserNumResult::DescribeLiveStreamOnlineUserNumResult() :
	ServiceResult()
{}

DescribeLiveStreamOnlineUserNumResult::DescribeLiveStreamOnlineUserNumResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeLiveStreamOnlineUserNumResult::~DescribeLiveStreamOnlineUserNumResult()
{}

void DescribeLiveStreamOnlineUserNumResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allOnlineUserInfoNode = value["OnlineUserInfo"]["LiveStreamOnlineUserNumInfo"];
	for (auto valueOnlineUserInfoLiveStreamOnlineUserNumInfo : allOnlineUserInfoNode)
	{
		LiveStreamOnlineUserNumInfo onlineUserInfoObject;
		if(!valueOnlineUserInfoLiveStreamOnlineUserNumInfo["StreamUrl"].isNull())
			onlineUserInfoObject.streamUrl = valueOnlineUserInfoLiveStreamOnlineUserNumInfo["StreamUrl"].asString();
		if(!valueOnlineUserInfoLiveStreamOnlineUserNumInfo["UserNumber"].isNull())
			onlineUserInfoObject.userNumber = std::stol(valueOnlineUserInfoLiveStreamOnlineUserNumInfo["UserNumber"].asString());
		if(!valueOnlineUserInfoLiveStreamOnlineUserNumInfo["Time"].isNull())
			onlineUserInfoObject.time = valueOnlineUserInfoLiveStreamOnlineUserNumInfo["Time"].asString();
		onlineUserInfo_.push_back(onlineUserInfoObject);
	}
	if(!value["TotalUserNumber"].isNull())
		totalUserNumber_ = std::stol(value["TotalUserNumber"].asString());

}

std::vector<DescribeLiveStreamOnlineUserNumResult::LiveStreamOnlineUserNumInfo> DescribeLiveStreamOnlineUserNumResult::getOnlineUserInfo()const
{
	return onlineUserInfo_;
}

long DescribeLiveStreamOnlineUserNumResult::getTotalUserNumber()const
{
	return totalUserNumber_;
}

