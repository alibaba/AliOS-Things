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

#include <alibabacloud/iot/model/QuerySceneRuleRequest.h>

using AlibabaCloud::Iot::Model::QuerySceneRuleRequest;

QuerySceneRuleRequest::QuerySceneRuleRequest() :
	RpcServiceRequest("iot", "2018-01-20", "QuerySceneRule")
{
	setMethod(HttpRequest::Method::Post);
}

QuerySceneRuleRequest::~QuerySceneRuleRequest()
{}

std::string QuerySceneRuleRequest::getRuleName()const
{
	return ruleName_;
}

void QuerySceneRuleRequest::setRuleName(const std::string& ruleName)
{
	ruleName_ = ruleName;
	setParameter("RuleName", ruleName);
}

std::string QuerySceneRuleRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void QuerySceneRuleRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string QuerySceneRuleRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void QuerySceneRuleRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

int QuerySceneRuleRequest::getPageSize()const
{
	return pageSize_;
}

void QuerySceneRuleRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

int QuerySceneRuleRequest::getCurrentPage()const
{
	return currentPage_;
}

void QuerySceneRuleRequest::setCurrentPage(int currentPage)
{
	currentPage_ = currentPage;
	setParameter("CurrentPage", std::to_string(currentPage));
}

std::string QuerySceneRuleRequest::getApiProduct()const
{
	return apiProduct_;
}

void QuerySceneRuleRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string QuerySceneRuleRequest::getApiRevision()const
{
	return apiRevision_;
}

void QuerySceneRuleRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

