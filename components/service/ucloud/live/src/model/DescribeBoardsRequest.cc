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

#include <alibabacloud/live/model/DescribeBoardsRequest.h>

using AlibabaCloud::Live::Model::DescribeBoardsRequest;

DescribeBoardsRequest::DescribeBoardsRequest() :
	RpcServiceRequest("live", "2016-11-01", "DescribeBoards")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeBoardsRequest::~DescribeBoardsRequest()
{}

int DescribeBoardsRequest::getPageNum()const
{
	return pageNum_;
}

void DescribeBoardsRequest::setPageNum(int pageNum)
{
	pageNum_ = pageNum;
	setParameter("PageNum", std::to_string(pageNum));
}

int DescribeBoardsRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeBoardsRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

long DescribeBoardsRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeBoardsRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string DescribeBoardsRequest::getAppId()const
{
	return appId_;
}

void DescribeBoardsRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

