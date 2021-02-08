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

#include <alibabacloud/iot/model/QuerySummarySceneRuleLogRequest.h>

using AlibabaCloud::Iot::Model::QuerySummarySceneRuleLogRequest;

QuerySummarySceneRuleLogRequest::QuerySummarySceneRuleLogRequest() :
	RpcServiceRequest("iot", "2018-01-20", "QuerySummarySceneRuleLog")
{
	setMethod(HttpRequest::Method::Post);
}

QuerySummarySceneRuleLogRequest::~QuerySummarySceneRuleLogRequest()
{}

int QuerySummarySceneRuleLogRequest::getStartTime()const
{
	return startTime_;
}

void QuerySummarySceneRuleLogRequest::setStartTime(int startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", std::to_string(startTime));
}

std::string QuerySummarySceneRuleLogRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void QuerySummarySceneRuleLogRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string QuerySummarySceneRuleLogRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void QuerySummarySceneRuleLogRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

int QuerySummarySceneRuleLogRequest::getPageSize()const
{
	return pageSize_;
}

void QuerySummarySceneRuleLogRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

int QuerySummarySceneRuleLogRequest::getEndTime()const
{
	return endTime_;
}

void QuerySummarySceneRuleLogRequest::setEndTime(int endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", std::to_string(endTime));
}

int QuerySummarySceneRuleLogRequest::getCurrentPage()const
{
	return currentPage_;
}

void QuerySummarySceneRuleLogRequest::setCurrentPage(int currentPage)
{
	currentPage_ = currentPage;
	setParameter("CurrentPage", std::to_string(currentPage));
}

std::string QuerySummarySceneRuleLogRequest::getApiProduct()const
{
	return apiProduct_;
}

void QuerySummarySceneRuleLogRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string QuerySummarySceneRuleLogRequest::getApiRevision()const
{
	return apiRevision_;
}

void QuerySummarySceneRuleLogRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string QuerySummarySceneRuleLogRequest::getRuleId()const
{
	return ruleId_;
}

void QuerySummarySceneRuleLogRequest::setRuleId(const std::string& ruleId)
{
	ruleId_ = ruleId;
	setParameter("RuleId", ruleId);
}

std::string QuerySummarySceneRuleLogRequest::getStatus()const
{
	return status_;
}

void QuerySummarySceneRuleLogRequest::setStatus(const std::string& status)
{
	status_ = status;
	setParameter("Status", status);
}

