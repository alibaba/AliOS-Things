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

#include <alibabacloud/live/model/DescribeMixStreamListResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeMixStreamListResult::DescribeMixStreamListResult() :
	ServiceResult()
{}

DescribeMixStreamListResult::DescribeMixStreamListResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeMixStreamListResult::~DescribeMixStreamListResult()
{}

void DescribeMixStreamListResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allMixStreamListNode = value["MixStreamList"]["MixStreamInfo"];
	for (auto valueMixStreamListMixStreamInfo : allMixStreamListNode)
	{
		MixStreamInfo mixStreamListObject;
		if(!valueMixStreamListMixStreamInfo["MixstreamId"].isNull())
			mixStreamListObject.mixstreamId = valueMixStreamListMixStreamInfo["MixstreamId"].asString();
		if(!valueMixStreamListMixStreamInfo["DomainName"].isNull())
			mixStreamListObject.domainName = valueMixStreamListMixStreamInfo["DomainName"].asString();
		if(!valueMixStreamListMixStreamInfo["AppName"].isNull())
			mixStreamListObject.appName = valueMixStreamListMixStreamInfo["AppName"].asString();
		if(!valueMixStreamListMixStreamInfo["StreamName"].isNull())
			mixStreamListObject.streamName = valueMixStreamListMixStreamInfo["StreamName"].asString();
		if(!valueMixStreamListMixStreamInfo["LayoutId"].isNull())
			mixStreamListObject.layoutId = valueMixStreamListMixStreamInfo["LayoutId"].asString();
		if(!valueMixStreamListMixStreamInfo["InputStreamNumber"].isNull())
			mixStreamListObject.inputStreamNumber = std::stoi(valueMixStreamListMixStreamInfo["InputStreamNumber"].asString());
		if(!valueMixStreamListMixStreamInfo["MixStreamTemplate"].isNull())
			mixStreamListObject.mixStreamTemplate = valueMixStreamListMixStreamInfo["MixStreamTemplate"].asString();
		if(!valueMixStreamListMixStreamInfo["GmtCreate"].isNull())
			mixStreamListObject.gmtCreate = valueMixStreamListMixStreamInfo["GmtCreate"].asString();
		if(!valueMixStreamListMixStreamInfo["GmtModified"].isNull())
			mixStreamListObject.gmtModified = valueMixStreamListMixStreamInfo["GmtModified"].asString();
		mixStreamList_.push_back(mixStreamListObject);
	}
	if(!value["Total"].isNull())
		total_ = std::stoi(value["Total"].asString());

}

int DescribeMixStreamListResult::getTotal()const
{
	return total_;
}

std::vector<DescribeMixStreamListResult::MixStreamInfo> DescribeMixStreamListResult::getMixStreamList()const
{
	return mixStreamList_;
}

