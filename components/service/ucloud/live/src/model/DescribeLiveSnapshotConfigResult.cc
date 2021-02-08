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

#include <alibabacloud/live/model/DescribeLiveSnapshotConfigResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeLiveSnapshotConfigResult::DescribeLiveSnapshotConfigResult() :
	ServiceResult()
{}

DescribeLiveSnapshotConfigResult::DescribeLiveSnapshotConfigResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeLiveSnapshotConfigResult::~DescribeLiveSnapshotConfigResult()
{}

void DescribeLiveSnapshotConfigResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allLiveStreamSnapshotConfigListNode = value["LiveStreamSnapshotConfigList"]["LiveStreamSnapshotConfig"];
	for (auto valueLiveStreamSnapshotConfigListLiveStreamSnapshotConfig : allLiveStreamSnapshotConfigListNode)
	{
		LiveStreamSnapshotConfig liveStreamSnapshotConfigListObject;
		if(!valueLiveStreamSnapshotConfigListLiveStreamSnapshotConfig["DomainName"].isNull())
			liveStreamSnapshotConfigListObject.domainName = valueLiveStreamSnapshotConfigListLiveStreamSnapshotConfig["DomainName"].asString();
		if(!valueLiveStreamSnapshotConfigListLiveStreamSnapshotConfig["AppName"].isNull())
			liveStreamSnapshotConfigListObject.appName = valueLiveStreamSnapshotConfigListLiveStreamSnapshotConfig["AppName"].asString();
		if(!valueLiveStreamSnapshotConfigListLiveStreamSnapshotConfig["TimeInterval"].isNull())
			liveStreamSnapshotConfigListObject.timeInterval = std::stoi(valueLiveStreamSnapshotConfigListLiveStreamSnapshotConfig["TimeInterval"].asString());
		if(!valueLiveStreamSnapshotConfigListLiveStreamSnapshotConfig["OssEndpoint"].isNull())
			liveStreamSnapshotConfigListObject.ossEndpoint = valueLiveStreamSnapshotConfigListLiveStreamSnapshotConfig["OssEndpoint"].asString();
		if(!valueLiveStreamSnapshotConfigListLiveStreamSnapshotConfig["OssBucket"].isNull())
			liveStreamSnapshotConfigListObject.ossBucket = valueLiveStreamSnapshotConfigListLiveStreamSnapshotConfig["OssBucket"].asString();
		if(!valueLiveStreamSnapshotConfigListLiveStreamSnapshotConfig["OverwriteOssObject"].isNull())
			liveStreamSnapshotConfigListObject.overwriteOssObject = valueLiveStreamSnapshotConfigListLiveStreamSnapshotConfig["OverwriteOssObject"].asString();
		if(!valueLiveStreamSnapshotConfigListLiveStreamSnapshotConfig["SequenceOssObject"].isNull())
			liveStreamSnapshotConfigListObject.sequenceOssObject = valueLiveStreamSnapshotConfigListLiveStreamSnapshotConfig["SequenceOssObject"].asString();
		if(!valueLiveStreamSnapshotConfigListLiveStreamSnapshotConfig["CreateTime"].isNull())
			liveStreamSnapshotConfigListObject.createTime = valueLiveStreamSnapshotConfigListLiveStreamSnapshotConfig["CreateTime"].asString();
		if(!valueLiveStreamSnapshotConfigListLiveStreamSnapshotConfig["Callback"].isNull())
			liveStreamSnapshotConfigListObject.callback = valueLiveStreamSnapshotConfigListLiveStreamSnapshotConfig["Callback"].asString();
		liveStreamSnapshotConfigList_.push_back(liveStreamSnapshotConfigListObject);
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

std::string DescribeLiveSnapshotConfigResult::getOrder()const
{
	return order_;
}

int DescribeLiveSnapshotConfigResult::getTotalNum()const
{
	return totalNum_;
}

int DescribeLiveSnapshotConfigResult::getPageNum()const
{
	return pageNum_;
}

int DescribeLiveSnapshotConfigResult::getPageSize()const
{
	return pageSize_;
}

int DescribeLiveSnapshotConfigResult::getTotalPage()const
{
	return totalPage_;
}

std::vector<DescribeLiveSnapshotConfigResult::LiveStreamSnapshotConfig> DescribeLiveSnapshotConfigResult::getLiveStreamSnapshotConfigList()const
{
	return liveStreamSnapshotConfigList_;
}

