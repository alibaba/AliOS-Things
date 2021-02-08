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

#include <alibabacloud/live/model/DescribeLiveSnapshotDetectPornConfigResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeLiveSnapshotDetectPornConfigResult::DescribeLiveSnapshotDetectPornConfigResult() :
	ServiceResult()
{}

DescribeLiveSnapshotDetectPornConfigResult::DescribeLiveSnapshotDetectPornConfigResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeLiveSnapshotDetectPornConfigResult::~DescribeLiveSnapshotDetectPornConfigResult()
{}

void DescribeLiveSnapshotDetectPornConfigResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allLiveSnapshotDetectPornConfigListNode = value["LiveSnapshotDetectPornConfigList"]["LiveSnapshotDetectPornConfig"];
	for (auto valueLiveSnapshotDetectPornConfigListLiveSnapshotDetectPornConfig : allLiveSnapshotDetectPornConfigListNode)
	{
		LiveSnapshotDetectPornConfig liveSnapshotDetectPornConfigListObject;
		if(!valueLiveSnapshotDetectPornConfigListLiveSnapshotDetectPornConfig["DomainName"].isNull())
			liveSnapshotDetectPornConfigListObject.domainName = valueLiveSnapshotDetectPornConfigListLiveSnapshotDetectPornConfig["DomainName"].asString();
		if(!valueLiveSnapshotDetectPornConfigListLiveSnapshotDetectPornConfig["AppName"].isNull())
			liveSnapshotDetectPornConfigListObject.appName = valueLiveSnapshotDetectPornConfigListLiveSnapshotDetectPornConfig["AppName"].asString();
		if(!valueLiveSnapshotDetectPornConfigListLiveSnapshotDetectPornConfig["OssEndpoint"].isNull())
			liveSnapshotDetectPornConfigListObject.ossEndpoint = valueLiveSnapshotDetectPornConfigListLiveSnapshotDetectPornConfig["OssEndpoint"].asString();
		if(!valueLiveSnapshotDetectPornConfigListLiveSnapshotDetectPornConfig["OssBucket"].isNull())
			liveSnapshotDetectPornConfigListObject.ossBucket = valueLiveSnapshotDetectPornConfigListLiveSnapshotDetectPornConfig["OssBucket"].asString();
		if(!valueLiveSnapshotDetectPornConfigListLiveSnapshotDetectPornConfig["OssObject"].isNull())
			liveSnapshotDetectPornConfigListObject.ossObject = valueLiveSnapshotDetectPornConfigListLiveSnapshotDetectPornConfig["OssObject"].asString();
		if(!valueLiveSnapshotDetectPornConfigListLiveSnapshotDetectPornConfig["Interval"].isNull())
			liveSnapshotDetectPornConfigListObject.interval = std::stoi(valueLiveSnapshotDetectPornConfigListLiveSnapshotDetectPornConfig["Interval"].asString());
		auto allScenes = value["Scenes"]["scene"];
		for (auto value : allScenes)
			liveSnapshotDetectPornConfigListObject.scenes.push_back(value.asString());
		liveSnapshotDetectPornConfigList_.push_back(liveSnapshotDetectPornConfigListObject);
	}
	if(!value["PageNum"].isNull())
		pageNum_ = std::stoi(value["PageNum"].asString());
	if(!value["PageSize"].isNull())
		pageSize_ = std::stoi(value["PageSize"].asString());
	if(!value["Order"].isNull())
		order_ = value["Order"].asString();
	if(!value["TotalNum"].isNull())
		totalNum_ = std::stoi(value["TotalNum"].asString());
	if(!value["TotalPage"].isNull())
		totalPage_ = std::stoi(value["TotalPage"].asString());

}

std::string DescribeLiveSnapshotDetectPornConfigResult::getOrder()const
{
	return order_;
}

int DescribeLiveSnapshotDetectPornConfigResult::getTotalNum()const
{
	return totalNum_;
}

int DescribeLiveSnapshotDetectPornConfigResult::getPageNum()const
{
	return pageNum_;
}

int DescribeLiveSnapshotDetectPornConfigResult::getPageSize()const
{
	return pageSize_;
}

int DescribeLiveSnapshotDetectPornConfigResult::getTotalPage()const
{
	return totalPage_;
}

std::vector<DescribeLiveSnapshotDetectPornConfigResult::LiveSnapshotDetectPornConfig> DescribeLiveSnapshotDetectPornConfigResult::getLiveSnapshotDetectPornConfigList()const
{
	return liveSnapshotDetectPornConfigList_;
}

