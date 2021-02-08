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

#include <alibabacloud/iot/model/ListRuleRequest.h>

using AlibabaCloud::Iot::Model::ListRuleRequest;

ListRuleRequest::ListRuleRequest() :
	RpcServiceRequest("iot", "2018-01-20", "ListRule")
{
	setMethod(HttpRequest::Method::Post);
}

ListRuleRequest::~ListRuleRequest()
{}

std::string ListRuleRequest::getSearchName()const
{
	return searchName_;
}

void ListRuleRequest::setSearchName(const std::string& searchName)
{
	searchName_ = searchName;
	setParameter("SearchName", searchName);
}

std::string ListRuleRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void ListRuleRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string ListRuleRequest::getResourceGroupId()const
{
	return resourceGroupId_;
}

void ListRuleRequest::setResourceGroupId(const std::string& resourceGroupId)
{
	resourceGroupId_ = resourceGroupId;
	setParameter("ResourceGroupId", resourceGroupId);
}

std::string ListRuleRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void ListRuleRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

int ListRuleRequest::getPageSize()const
{
	return pageSize_;
}

void ListRuleRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

int ListRuleRequest::getCurrentPage()const
{
	return currentPage_;
}

void ListRuleRequest::setCurrentPage(int currentPage)
{
	currentPage_ = currentPage;
	setParameter("CurrentPage", std::to_string(currentPage));
}

std::string ListRuleRequest::getApiProduct()const
{
	return apiProduct_;
}

void ListRuleRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string ListRuleRequest::getApiRevision()const
{
	return apiRevision_;
}

void ListRuleRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

