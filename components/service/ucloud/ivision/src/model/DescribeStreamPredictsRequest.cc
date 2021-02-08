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

#include <alibabacloud/ivision/model/DescribeStreamPredictsRequest.h>

using AlibabaCloud::Ivision::Model::DescribeStreamPredictsRequest;

DescribeStreamPredictsRequest::DescribeStreamPredictsRequest() :
	RpcServiceRequest("ivision", "2019-03-08", "DescribeStreamPredicts")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeStreamPredictsRequest::~DescribeStreamPredictsRequest()
{}

std::string DescribeStreamPredictsRequest::getNextPageToken()const
{
	return nextPageToken_;
}

void DescribeStreamPredictsRequest::setNextPageToken(const std::string& nextPageToken)
{
	nextPageToken_ = nextPageToken;
	setParameter("NextPageToken", nextPageToken);
}

std::string DescribeStreamPredictsRequest::getPredictIds()const
{
	return predictIds_;
}

void DescribeStreamPredictsRequest::setPredictIds(const std::string& predictIds)
{
	predictIds_ = predictIds;
	setParameter("PredictIds", predictIds);
}

long DescribeStreamPredictsRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeStreamPredictsRequest::setPageSize(long pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::string DescribeStreamPredictsRequest::getShowLog()const
{
	return showLog_;
}

void DescribeStreamPredictsRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

std::string DescribeStreamPredictsRequest::getModelId()const
{
	return modelId_;
}

void DescribeStreamPredictsRequest::setModelId(const std::string& modelId)
{
	modelId_ = modelId;
	setParameter("ModelId", modelId);
}

long DescribeStreamPredictsRequest::getCurrentPage()const
{
	return currentPage_;
}

void DescribeStreamPredictsRequest::setCurrentPage(long currentPage)
{
	currentPage_ = currentPage;
	setParameter("CurrentPage", std::to_string(currentPage));
}

long DescribeStreamPredictsRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeStreamPredictsRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

