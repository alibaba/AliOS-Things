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

#include <alibabacloud/live/model/DescribeLiveStreamRecordIndexFilesResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeLiveStreamRecordIndexFilesResult::DescribeLiveStreamRecordIndexFilesResult() :
	ServiceResult()
{}

DescribeLiveStreamRecordIndexFilesResult::DescribeLiveStreamRecordIndexFilesResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeLiveStreamRecordIndexFilesResult::~DescribeLiveStreamRecordIndexFilesResult()
{}

void DescribeLiveStreamRecordIndexFilesResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allRecordIndexInfoListNode = value["RecordIndexInfoList"]["RecordIndexInfo"];
	for (auto valueRecordIndexInfoListRecordIndexInfo : allRecordIndexInfoListNode)
	{
		RecordIndexInfo recordIndexInfoListObject;
		if(!valueRecordIndexInfoListRecordIndexInfo["RecordId"].isNull())
			recordIndexInfoListObject.recordId = valueRecordIndexInfoListRecordIndexInfo["RecordId"].asString();
		if(!valueRecordIndexInfoListRecordIndexInfo["RecordUrl"].isNull())
			recordIndexInfoListObject.recordUrl = valueRecordIndexInfoListRecordIndexInfo["RecordUrl"].asString();
		if(!valueRecordIndexInfoListRecordIndexInfo["DomainName"].isNull())
			recordIndexInfoListObject.domainName = valueRecordIndexInfoListRecordIndexInfo["DomainName"].asString();
		if(!valueRecordIndexInfoListRecordIndexInfo["AppName"].isNull())
			recordIndexInfoListObject.appName = valueRecordIndexInfoListRecordIndexInfo["AppName"].asString();
		if(!valueRecordIndexInfoListRecordIndexInfo["StreamName"].isNull())
			recordIndexInfoListObject.streamName = valueRecordIndexInfoListRecordIndexInfo["StreamName"].asString();
		if(!valueRecordIndexInfoListRecordIndexInfo["OssBucket"].isNull())
			recordIndexInfoListObject.ossBucket = valueRecordIndexInfoListRecordIndexInfo["OssBucket"].asString();
		if(!valueRecordIndexInfoListRecordIndexInfo["OssEndpoint"].isNull())
			recordIndexInfoListObject.ossEndpoint = valueRecordIndexInfoListRecordIndexInfo["OssEndpoint"].asString();
		if(!valueRecordIndexInfoListRecordIndexInfo["OssObject"].isNull())
			recordIndexInfoListObject.ossObject = valueRecordIndexInfoListRecordIndexInfo["OssObject"].asString();
		if(!valueRecordIndexInfoListRecordIndexInfo["StartTime"].isNull())
			recordIndexInfoListObject.startTime = valueRecordIndexInfoListRecordIndexInfo["StartTime"].asString();
		if(!valueRecordIndexInfoListRecordIndexInfo["EndTime"].isNull())
			recordIndexInfoListObject.endTime = valueRecordIndexInfoListRecordIndexInfo["EndTime"].asString();
		if(!valueRecordIndexInfoListRecordIndexInfo["Duration"].isNull())
			recordIndexInfoListObject.duration = std::stof(valueRecordIndexInfoListRecordIndexInfo["Duration"].asString());
		if(!valueRecordIndexInfoListRecordIndexInfo["Height"].isNull())
			recordIndexInfoListObject.height = std::stoi(valueRecordIndexInfoListRecordIndexInfo["Height"].asString());
		if(!valueRecordIndexInfoListRecordIndexInfo["Width"].isNull())
			recordIndexInfoListObject.width = std::stoi(valueRecordIndexInfoListRecordIndexInfo["Width"].asString());
		if(!valueRecordIndexInfoListRecordIndexInfo["CreateTime"].isNull())
			recordIndexInfoListObject.createTime = valueRecordIndexInfoListRecordIndexInfo["CreateTime"].asString();
		recordIndexInfoList_.push_back(recordIndexInfoListObject);
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

std::string DescribeLiveStreamRecordIndexFilesResult::getOrder()const
{
	return order_;
}

int DescribeLiveStreamRecordIndexFilesResult::getTotalNum()const
{
	return totalNum_;
}

int DescribeLiveStreamRecordIndexFilesResult::getPageNum()const
{
	return pageNum_;
}

int DescribeLiveStreamRecordIndexFilesResult::getPageSize()const
{
	return pageSize_;
}

int DescribeLiveStreamRecordIndexFilesResult::getTotalPage()const
{
	return totalPage_;
}

std::vector<DescribeLiveStreamRecordIndexFilesResult::RecordIndexInfo> DescribeLiveStreamRecordIndexFilesResult::getRecordIndexInfoList()const
{
	return recordIndexInfoList_;
}

