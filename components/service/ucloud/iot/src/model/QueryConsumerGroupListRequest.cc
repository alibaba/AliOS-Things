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

#include <alibabacloud/iot/model/QueryConsumerGroupListRequest.h>

using AlibabaCloud::Iot::Model::QueryConsumerGroupListRequest;

QueryConsumerGroupListRequest::QueryConsumerGroupListRequest() :
	RpcServiceRequest("iot", "2018-01-20", "QueryConsumerGroupList")
{
	setMethod(HttpRequest::Method::Post);
}

QueryConsumerGroupListRequest::~QueryConsumerGroupListRequest()
{}

std::string QueryConsumerGroupListRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void QueryConsumerGroupListRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

int QueryConsumerGroupListRequest::getPageSize()const
{
	return pageSize_;
}

void QueryConsumerGroupListRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

bool QueryConsumerGroupListRequest::getFuzzy()const
{
	return fuzzy_;
}

void QueryConsumerGroupListRequest::setFuzzy(bool fuzzy)
{
	fuzzy_ = fuzzy;
	setParameter("Fuzzy", fuzzy ? "true" : "false");
}

int QueryConsumerGroupListRequest::getCurrentPage()const
{
	return currentPage_;
}

void QueryConsumerGroupListRequest::setCurrentPage(int currentPage)
{
	currentPage_ = currentPage;
	setParameter("CurrentPage", std::to_string(currentPage));
}

std::string QueryConsumerGroupListRequest::getGroupName()const
{
	return groupName_;
}

void QueryConsumerGroupListRequest::setGroupName(const std::string& groupName)
{
	groupName_ = groupName;
	setParameter("GroupName", groupName);
}

std::string QueryConsumerGroupListRequest::getApiProduct()const
{
	return apiProduct_;
}

void QueryConsumerGroupListRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string QueryConsumerGroupListRequest::getApiRevision()const
{
	return apiRevision_;
}

void QueryConsumerGroupListRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

