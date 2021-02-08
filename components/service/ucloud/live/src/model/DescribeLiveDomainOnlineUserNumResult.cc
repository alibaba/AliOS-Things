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

#include <alibabacloud/live/model/DescribeLiveDomainOnlineUserNumResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeLiveDomainOnlineUserNumResult::DescribeLiveDomainOnlineUserNumResult() :
	ServiceResult()
{}

DescribeLiveDomainOnlineUserNumResult::DescribeLiveDomainOnlineUserNumResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeLiveDomainOnlineUserNumResult::~DescribeLiveDomainOnlineUserNumResult()
{}

void DescribeLiveDomainOnlineUserNumResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allOnlineUserInfoNode = value["OnlineUserInfo"]["LiveStreamOnlineUserNumInfo"];
	for (auto valueOnlineUserInfoLiveStreamOnlineUserNumInfo : allOnlineUserInfoNode)
	{
		LiveStreamOnlineUserNumInfo onlineUserInfoObject;
		if(!valueOnlineUserInfoLiveStreamOnlineUserNumInfo["StreamName"].isNull())
			onlineUserInfoObject.streamName = valueOnlineUserInfoLiveStreamOnlineUserNumInfo["StreamName"].asString();
		auto allInfosNode = allOnlineUserInfoNode["Infos"]["Info"];
		for (auto allOnlineUserInfoNodeInfosInfo : allInfosNode)
		{
			LiveStreamOnlineUserNumInfo::Info infosObject;
			if(!allOnlineUserInfoNodeInfosInfo["TranscodeTemplate"].isNull())
				infosObject.transcodeTemplate = allOnlineUserInfoNodeInfosInfo["TranscodeTemplate"].asString();
			if(!allOnlineUserInfoNodeInfosInfo["UserNumber"].isNull())
				infosObject.userNumber = std::stol(allOnlineUserInfoNodeInfosInfo["UserNumber"].asString());
			onlineUserInfoObject.infos.push_back(infosObject);
		}
		onlineUserInfo_.push_back(onlineUserInfoObject);
	}
	if(!value["StreamCount"].isNull())
		streamCount_ = std::stoi(value["StreamCount"].asString());
	if(!value["UserCount"].isNull())
		userCount_ = std::stoi(value["UserCount"].asString());

}

int DescribeLiveDomainOnlineUserNumResult::getStreamCount()const
{
	return streamCount_;
}

std::vector<DescribeLiveDomainOnlineUserNumResult::LiveStreamOnlineUserNumInfo> DescribeLiveDomainOnlineUserNumResult::getOnlineUserInfo()const
{
	return onlineUserInfo_;
}

int DescribeLiveDomainOnlineUserNumResult::getUserCount()const
{
	return userCount_;
}

