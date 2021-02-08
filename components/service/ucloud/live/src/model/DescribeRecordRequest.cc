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

#include <alibabacloud/live/model/DescribeRecordRequest.h>

using AlibabaCloud::Live::Model::DescribeRecordRequest;

DescribeRecordRequest::DescribeRecordRequest() :
	RpcServiceRequest("live", "2016-11-01", "DescribeRecord")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeRecordRequest::~DescribeRecordRequest()
{}

long DescribeRecordRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeRecordRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string DescribeRecordRequest::getRecordId()const
{
	return recordId_;
}

void DescribeRecordRequest::setRecordId(const std::string& recordId)
{
	recordId_ = recordId;
	setParameter("RecordId", recordId);
}

std::string DescribeRecordRequest::getAppId()const
{
	return appId_;
}

void DescribeRecordRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

