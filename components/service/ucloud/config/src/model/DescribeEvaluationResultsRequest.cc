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

#include <alibabacloud/config/model/DescribeEvaluationResultsRequest.h>

using AlibabaCloud::Config::Model::DescribeEvaluationResultsRequest;

DescribeEvaluationResultsRequest::DescribeEvaluationResultsRequest() :
	RpcServiceRequest("config", "2019-01-08", "DescribeEvaluationResults")
{
	setMethod(HttpRequest::Method::Get);
}

DescribeEvaluationResultsRequest::~DescribeEvaluationResultsRequest()
{}

std::string DescribeEvaluationResultsRequest::getConfigRuleId()const
{
	return configRuleId_;
}

void DescribeEvaluationResultsRequest::setConfigRuleId(const std::string& configRuleId)
{
	configRuleId_ = configRuleId;
	setParameter("ConfigRuleId", configRuleId);
}

bool DescribeEvaluationResultsRequest::getMultiAccount()const
{
	return multiAccount_;
}

void DescribeEvaluationResultsRequest::setMultiAccount(bool multiAccount)
{
	multiAccount_ = multiAccount;
	setParameter("MultiAccount", multiAccount ? "true" : "false");
}

int DescribeEvaluationResultsRequest::getPageNumber()const
{
	return pageNumber_;
}

void DescribeEvaluationResultsRequest::setPageNumber(int pageNumber)
{
	pageNumber_ = pageNumber;
	setParameter("PageNumber", std::to_string(pageNumber));
}

int DescribeEvaluationResultsRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeEvaluationResultsRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::string DescribeEvaluationResultsRequest::getComplianceType()const
{
	return complianceType_;
}

void DescribeEvaluationResultsRequest::setComplianceType(const std::string& complianceType)
{
	complianceType_ = complianceType;
	setParameter("ComplianceType", complianceType);
}

std::string DescribeEvaluationResultsRequest::getResourceId()const
{
	return resourceId_;
}

void DescribeEvaluationResultsRequest::setResourceId(const std::string& resourceId)
{
	resourceId_ = resourceId;
	setParameter("ResourceId", resourceId);
}

std::string DescribeEvaluationResultsRequest::getResourceType()const
{
	return resourceType_;
}

void DescribeEvaluationResultsRequest::setResourceType(const std::string& resourceType)
{
	resourceType_ = resourceType;
	setParameter("ResourceType", resourceType);
}

long DescribeEvaluationResultsRequest::getMemberId()const
{
	return memberId_;
}

void DescribeEvaluationResultsRequest::setMemberId(long memberId)
{
	memberId_ = memberId;
	setParameter("MemberId", std::to_string(memberId));
}

