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

#include <alibabacloud/ivision/model/DescribeStreamPredictResultRequest.h>

using AlibabaCloud::Ivision::Model::DescribeStreamPredictResultRequest;

DescribeStreamPredictResultRequest::DescribeStreamPredictResultRequest() :
	RpcServiceRequest("ivision", "2019-03-08", "DescribeStreamPredictResult")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeStreamPredictResultRequest::~DescribeStreamPredictResultRequest()
{}

std::string DescribeStreamPredictResultRequest::getNextPageToken()const
{
	return nextPageToken_;
}

void DescribeStreamPredictResultRequest::setNextPageToken(const std::string& nextPageToken)
{
	nextPageToken_ = nextPageToken;
	setParameter("NextPageToken", nextPageToken);
}

std::string DescribeStreamPredictResultRequest::getStartTime()const
{
	return startTime_;
}

void DescribeStreamPredictResultRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

std::string DescribeStreamPredictResultRequest::getPredictId()const
{
	return predictId_;
}

void DescribeStreamPredictResultRequest::setPredictId(const std::string& predictId)
{
	predictId_ = predictId;
	setParameter("PredictId", predictId);
}

long DescribeStreamPredictResultRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeStreamPredictResultRequest::setPageSize(long pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::string DescribeStreamPredictResultRequest::getProbabilityThreshold()const
{
	return probabilityThreshold_;
}

void DescribeStreamPredictResultRequest::setProbabilityThreshold(const std::string& probabilityThreshold)
{
	probabilityThreshold_ = probabilityThreshold;
	setParameter("ProbabilityThreshold", probabilityThreshold);
}

std::string DescribeStreamPredictResultRequest::getShowLog()const
{
	return showLog_;
}

void DescribeStreamPredictResultRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

std::string DescribeStreamPredictResultRequest::getModelId()const
{
	return modelId_;
}

void DescribeStreamPredictResultRequest::setModelId(const std::string& modelId)
{
	modelId_ = modelId;
	setParameter("ModelId", modelId);
}

std::string DescribeStreamPredictResultRequest::getEndTime()const
{
	return endTime_;
}

void DescribeStreamPredictResultRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", endTime);
}

long DescribeStreamPredictResultRequest::getCurrentPage()const
{
	return currentPage_;
}

void DescribeStreamPredictResultRequest::setCurrentPage(long currentPage)
{
	currentPage_ = currentPage;
	setParameter("CurrentPage", std::to_string(currentPage));
}

long DescribeStreamPredictResultRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeStreamPredictResultRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

