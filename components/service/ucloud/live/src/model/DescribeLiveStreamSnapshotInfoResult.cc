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

#include <alibabacloud/live/model/DescribeLiveStreamSnapshotInfoResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeLiveStreamSnapshotInfoResult::DescribeLiveStreamSnapshotInfoResult() :
	ServiceResult()
{}

DescribeLiveStreamSnapshotInfoResult::DescribeLiveStreamSnapshotInfoResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeLiveStreamSnapshotInfoResult::~DescribeLiveStreamSnapshotInfoResult()
{}

void DescribeLiveStreamSnapshotInfoResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allLiveStreamSnapshotInfoListNode = value["LiveStreamSnapshotInfoList"]["LiveStreamSnapshotInfo"];
	for (auto valueLiveStreamSnapshotInfoListLiveStreamSnapshotInfo : allLiveStreamSnapshotInfoListNode)
	{
		LiveStreamSnapshotInfo liveStreamSnapshotInfoListObject;
		if(!valueLiveStreamSnapshotInfoListLiveStreamSnapshotInfo["OssEndpoint"].isNull())
			liveStreamSnapshotInfoListObject.ossEndpoint = valueLiveStreamSnapshotInfoListLiveStreamSnapshotInfo["OssEndpoint"].asString();
		if(!valueLiveStreamSnapshotInfoListLiveStreamSnapshotInfo["OssBucket"].isNull())
			liveStreamSnapshotInfoListObject.ossBucket = valueLiveStreamSnapshotInfoListLiveStreamSnapshotInfo["OssBucket"].asString();
		if(!valueLiveStreamSnapshotInfoListLiveStreamSnapshotInfo["OssObject"].isNull())
			liveStreamSnapshotInfoListObject.ossObject = valueLiveStreamSnapshotInfoListLiveStreamSnapshotInfo["OssObject"].asString();
		if(!valueLiveStreamSnapshotInfoListLiveStreamSnapshotInfo["CreateTime"].isNull())
			liveStreamSnapshotInfoListObject.createTime = valueLiveStreamSnapshotInfoListLiveStreamSnapshotInfo["CreateTime"].asString();
		liveStreamSnapshotInfoList_.push_back(liveStreamSnapshotInfoListObject);
	}
	if(!value["NextStartTime"].isNull())
		nextStartTime_ = value["NextStartTime"].asString();

}

std::vector<DescribeLiveStreamSnapshotInfoResult::LiveStreamSnapshotInfo> DescribeLiveStreamSnapshotInfoResult::getLiveStreamSnapshotInfoList()const
{
	return liveStreamSnapshotInfoList_;
}

std::string DescribeLiveStreamSnapshotInfoResult::getNextStartTime()const
{
	return nextStartTime_;
}

