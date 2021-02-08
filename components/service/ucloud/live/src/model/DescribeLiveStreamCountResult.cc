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

#include <alibabacloud/live/model/DescribeLiveStreamCountResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeLiveStreamCountResult::DescribeLiveStreamCountResult() :
	ServiceResult()
{}

DescribeLiveStreamCountResult::DescribeLiveStreamCountResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeLiveStreamCountResult::~DescribeLiveStreamCountResult()
{}

void DescribeLiveStreamCountResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allStreamCountInfosNode = value["StreamCountInfos"]["StreamCountInfo"];
	for (auto valueStreamCountInfosStreamCountInfo : allStreamCountInfosNode)
	{
		StreamCountInfo streamCountInfosObject;
		if(!valueStreamCountInfosStreamCountInfo["Count"].isNull())
			streamCountInfosObject.count = std::stol(valueStreamCountInfosStreamCountInfo["Count"].asString());
		if(!valueStreamCountInfosStreamCountInfo["Limit"].isNull())
			streamCountInfosObject.limit = std::stol(valueStreamCountInfosStreamCountInfo["Limit"].asString());
		if(!valueStreamCountInfosStreamCountInfo["Type"].isNull())
			streamCountInfosObject.type = valueStreamCountInfosStreamCountInfo["Type"].asString();
		auto allStreamCountDetailsNode = allStreamCountInfosNode["StreamCountDetails"]["StreamCountDetail"];
		for (auto allStreamCountInfosNodeStreamCountDetailsStreamCountDetail : allStreamCountDetailsNode)
		{
			StreamCountInfo::StreamCountDetail streamCountDetailsObject;
			if(!allStreamCountInfosNodeStreamCountDetailsStreamCountDetail["Format"].isNull())
				streamCountDetailsObject.format = allStreamCountInfosNodeStreamCountDetailsStreamCountDetail["Format"].asString();
			if(!allStreamCountInfosNodeStreamCountDetailsStreamCountDetail["VideoDataRate"].isNull())
				streamCountDetailsObject.videoDataRate = std::stol(allStreamCountInfosNodeStreamCountDetailsStreamCountDetail["VideoDataRate"].asString());
			if(!allStreamCountInfosNodeStreamCountDetailsStreamCountDetail["Count"].isNull())
				streamCountDetailsObject.count = std::stol(allStreamCountInfosNodeStreamCountDetailsStreamCountDetail["Count"].asString());
			streamCountInfosObject.streamCountDetails.push_back(streamCountDetailsObject);
		}
		streamCountInfos_.push_back(streamCountInfosObject);
	}

}

std::vector<DescribeLiveStreamCountResult::StreamCountInfo> DescribeLiveStreamCountResult::getStreamCountInfos()const
{
	return streamCountInfos_;
}

