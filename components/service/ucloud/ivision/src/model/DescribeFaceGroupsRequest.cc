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

#include <alibabacloud/ivision/model/DescribeFaceGroupsRequest.h>

using AlibabaCloud::Ivision::Model::DescribeFaceGroupsRequest;

DescribeFaceGroupsRequest::DescribeFaceGroupsRequest() :
	RpcServiceRequest("ivision", "2019-03-08", "DescribeFaceGroups")
{
	setMethod(HttpRequest::Method::Get);
}

DescribeFaceGroupsRequest::~DescribeFaceGroupsRequest()
{}

std::string DescribeFaceGroupsRequest::getNextPageToken()const
{
	return nextPageToken_;
}

void DescribeFaceGroupsRequest::setNextPageToken(const std::string& nextPageToken)
{
	nextPageToken_ = nextPageToken;
	setParameter("NextPageToken", nextPageToken);
}

long DescribeFaceGroupsRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeFaceGroupsRequest::setPageSize(long pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::string DescribeFaceGroupsRequest::getShowLog()const
{
	return showLog_;
}

void DescribeFaceGroupsRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

long DescribeFaceGroupsRequest::getCurrentPage()const
{
	return currentPage_;
}

void DescribeFaceGroupsRequest::setCurrentPage(long currentPage)
{
	currentPage_ = currentPage;
	setParameter("CurrentPage", std::to_string(currentPage));
}

long DescribeFaceGroupsRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeFaceGroupsRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

