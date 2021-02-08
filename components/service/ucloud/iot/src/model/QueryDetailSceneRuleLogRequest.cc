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

#include <alibabacloud/iot/model/QueryDetailSceneRuleLogRequest.h>

using AlibabaCloud::Iot::Model::QueryDetailSceneRuleLogRequest;

QueryDetailSceneRuleLogRequest::QueryDetailSceneRuleLogRequest() :
	RpcServiceRequest("iot", "2018-01-20", "QueryDetailSceneRuleLog")
{
	setMethod(HttpRequest::Method::Post);
}

QueryDetailSceneRuleLogRequest::~QueryDetailSceneRuleLogRequest()
{}

std::string QueryDetailSceneRuleLogRequest::getTraceId()const
{
	return traceId_;
}

void QueryDetailSceneRuleLogRequest::setTraceId(const std::string& traceId)
{
	traceId_ = traceId;
	setParameter("TraceId", traceId);
}

int QueryDetailSceneRuleLogRequest::getStartTime()const
{
	return startTime_;
}

void QueryDetailSceneRuleLogRequest::setStartTime(int startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", std::to_string(startTime));
}

std::string QueryDetailSceneRuleLogRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void QueryDetailSceneRuleLogRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string QueryDetailSceneRuleLogRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void QueryDetailSceneRuleLogRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

int QueryDetailSceneRuleLogRequest::getPageSize()const
{
	return pageSize_;
}

void QueryDetailSceneRuleLogRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

int QueryDetailSceneRuleLogRequest::getEndTime()const
{
	return endTime_;
}

void QueryDetailSceneRuleLogRequest::setEndTime(int endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", std::to_string(endTime));
}

int QueryDetailSceneRuleLogRequest::getCurrentPage()const
{
	return currentPage_;
}

void QueryDetailSceneRuleLogRequest::setCurrentPage(int currentPage)
{
	currentPage_ = currentPage;
	setParameter("CurrentPage", std::to_string(currentPage));
}

std::string QueryDetailSceneRuleLogRequest::getApiProduct()const
{
	return apiProduct_;
}

void QueryDetailSceneRuleLogRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string QueryDetailSceneRuleLogRequest::getApiRevision()const
{
	return apiRevision_;
}

void QueryDetailSceneRuleLogRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string QueryDetailSceneRuleLogRequest::getRuleId()const
{
	return ruleId_;
}

void QueryDetailSceneRuleLogRequest::setRuleId(const std::string& ruleId)
{
	ruleId_ = ruleId;
	setParameter("RuleId", ruleId);
}

