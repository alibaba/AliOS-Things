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

#include <alibabacloud/live/model/DescribeRecordsRequest.h>

using AlibabaCloud::Live::Model::DescribeRecordsRequest;

DescribeRecordsRequest::DescribeRecordsRequest() :
	RpcServiceRequest("live", "2016-11-01", "DescribeRecords")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeRecordsRequest::~DescribeRecordsRequest()
{}

std::string DescribeRecordsRequest::getRecordState()const
{
	return recordState_;
}

void DescribeRecordsRequest::setRecordState(const std::string& recordState)
{
	recordState_ = recordState;
	setParameter("RecordState", recordState);
}

int DescribeRecordsRequest::getPageNum()const
{
	return pageNum_;
}

void DescribeRecordsRequest::setPageNum(int pageNum)
{
	pageNum_ = pageNum;
	setParameter("PageNum", std::to_string(pageNum));
}

int DescribeRecordsRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeRecordsRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

long DescribeRecordsRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeRecordsRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string DescribeRecordsRequest::getAppId()const
{
	return appId_;
}

void DescribeRecordsRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

