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

#include <alibabacloud/config/model/DescribeComplianceRequest.h>

using AlibabaCloud::Config::Model::DescribeComplianceRequest;

DescribeComplianceRequest::DescribeComplianceRequest() :
	RpcServiceRequest("config", "2019-01-08", "DescribeCompliance")
{
	setMethod(HttpRequest::Method::Get);
}

DescribeComplianceRequest::~DescribeComplianceRequest()
{}

std::string DescribeComplianceRequest::getConfigRuleId()const
{
	return configRuleId_;
}

void DescribeComplianceRequest::setConfigRuleId(const std::string& configRuleId)
{
	configRuleId_ = configRuleId;
	setParameter("ConfigRuleId", configRuleId);
}

std::string DescribeComplianceRequest::getResourceId()const
{
	return resourceId_;
}

void DescribeComplianceRequest::setResourceId(const std::string& resourceId)
{
	resourceId_ = resourceId;
	setParameter("ResourceId", resourceId);
}

bool DescribeComplianceRequest::getMultiAccount()const
{
	return multiAccount_;
}

void DescribeComplianceRequest::setMultiAccount(bool multiAccount)
{
	multiAccount_ = multiAccount;
	setParameter("MultiAccount", multiAccount ? "true" : "false");
}

std::string DescribeComplianceRequest::getResourceType()const
{
	return resourceType_;
}

void DescribeComplianceRequest::setResourceType(const std::string& resourceType)
{
	resourceType_ = resourceType;
	setParameter("ResourceType", resourceType);
}

std::string DescribeComplianceRequest::getComplianceType()const
{
	return complianceType_;
}

void DescribeComplianceRequest::setComplianceType(const std::string& complianceType)
{
	complianceType_ = complianceType;
	setParameter("ComplianceType", complianceType);
}

long DescribeComplianceRequest::getMemberId()const
{
	return memberId_;
}

void DescribeComplianceRequest::setMemberId(long memberId)
{
	memberId_ = memberId;
	setParameter("MemberId", std::to_string(memberId));
}

