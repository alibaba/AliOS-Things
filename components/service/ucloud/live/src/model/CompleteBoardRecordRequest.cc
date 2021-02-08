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

#include <alibabacloud/live/model/CompleteBoardRecordRequest.h>

using AlibabaCloud::Live::Model::CompleteBoardRecordRequest;

CompleteBoardRecordRequest::CompleteBoardRecordRequest() :
	RpcServiceRequest("live", "2016-11-01", "CompleteBoardRecord")
{
	setMethod(HttpRequest::Method::Post);
}

CompleteBoardRecordRequest::~CompleteBoardRecordRequest()
{}

std::string CompleteBoardRecordRequest::getEndTime()const
{
	return endTime_;
}

void CompleteBoardRecordRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", endTime);
}

long CompleteBoardRecordRequest::getOwnerId()const
{
	return ownerId_;
}

void CompleteBoardRecordRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string CompleteBoardRecordRequest::getRecordId()const
{
	return recordId_;
}

void CompleteBoardRecordRequest::setRecordId(const std::string& recordId)
{
	recordId_ = recordId;
	setParameter("RecordId", recordId);
}

std::string CompleteBoardRecordRequest::getAppId()const
{
	return appId_;
}

void CompleteBoardRecordRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

