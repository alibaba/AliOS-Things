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

#include <alibabacloud/live/model/DescribeHlsLiveStreamRealTimeBpsDataResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeHlsLiveStreamRealTimeBpsDataResult::DescribeHlsLiveStreamRealTimeBpsDataResult() :
	ServiceResult()
{}

DescribeHlsLiveStreamRealTimeBpsDataResult::DescribeHlsLiveStreamRealTimeBpsDataResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeHlsLiveStreamRealTimeBpsDataResult::~DescribeHlsLiveStreamRealTimeBpsDataResult()
{}

void DescribeHlsLiveStreamRealTimeBpsDataResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allUsageDataNode = value["UsageData"]["UsageDataPerDomain"];
	for (auto valueUsageDataUsageDataPerDomain : allUsageDataNode)
	{
		UsageDataPerDomain usageDataObject;
		if(!valueUsageDataUsageDataPerDomain["DomainName"].isNull())
			usageDataObject.domainName = valueUsageDataUsageDataPerDomain["DomainName"].asString();
		auto allStreamInfosNode = allUsageDataNode["StreamInfos"]["StreamInfo"];
		for (auto allUsageDataNodeStreamInfosStreamInfo : allStreamInfosNode)
		{
			UsageDataPerDomain::StreamInfo streamInfosObject;
			if(!allUsageDataNodeStreamInfosStreamInfo["StreamName"].isNull())
				streamInfosObject.streamName = allUsageDataNodeStreamInfosStreamInfo["StreamName"].asString();
			auto allInfosNode = allStreamInfosNode["Infos"]["Info"];
			for (auto allStreamInfosNodeInfosInfo : allInfosNode)
			{
				UsageDataPerDomain::StreamInfo::Info infosObject;
				if(!allStreamInfosNodeInfosInfo["DownFlow"].isNull())
					infosObject.downFlow = std::stof(allStreamInfosNodeInfosInfo["DownFlow"].asString());
				if(!allStreamInfosNodeInfosInfo["Rate"].isNull())
					infosObject.rate = allStreamInfosNodeInfosInfo["Rate"].asString();
				if(!allStreamInfosNodeInfosInfo["Online"].isNull())
					infosObject.online = std::stof(allStreamInfosNodeInfosInfo["Online"].asString());
				streamInfosObject.infos.push_back(infosObject);
			}
			usageDataObject.streamInfos.push_back(streamInfosObject);
		}
		usageData_.push_back(usageDataObject);
	}
	if(!value["Time"].isNull())
		time_ = value["Time"].asString();

}

std::vector<DescribeHlsLiveStreamRealTimeBpsDataResult::UsageDataPerDomain> DescribeHlsLiveStreamRealTimeBpsDataResult::getUsageData()const
{
	return usageData_;
}

std::string DescribeHlsLiveStreamRealTimeBpsDataResult::getTime()const
{
	return time_;
}

