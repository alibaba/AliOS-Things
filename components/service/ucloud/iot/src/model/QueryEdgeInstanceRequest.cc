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

#include <alibabacloud/iot/model/QueryEdgeInstanceRequest.h>

using AlibabaCloud::Iot::Model::QueryEdgeInstanceRequest;

QueryEdgeInstanceRequest::QueryEdgeInstanceRequest() :
	RpcServiceRequest("iot", "2018-01-20", "QueryEdgeInstance")
{
	setMethod(HttpRequest::Method::Post);
}

QueryEdgeInstanceRequest::~QueryEdgeInstanceRequest()
{}

std::string QueryEdgeInstanceRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void QueryEdgeInstanceRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string QueryEdgeInstanceRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void QueryEdgeInstanceRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

int QueryEdgeInstanceRequest::getPageSize()const
{
	return pageSize_;
}

void QueryEdgeInstanceRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

int QueryEdgeInstanceRequest::getCurrentPage()const
{
	return currentPage_;
}

void QueryEdgeInstanceRequest::setCurrentPage(int currentPage)
{
	currentPage_ = currentPage;
	setParameter("CurrentPage", std::to_string(currentPage));
}

std::string QueryEdgeInstanceRequest::getApiProduct()const
{
	return apiProduct_;
}

void QueryEdgeInstanceRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string QueryEdgeInstanceRequest::getName()const
{
	return name_;
}

void QueryEdgeInstanceRequest::setName(const std::string& name)
{
	name_ = name;
	setParameter("Name", name);
}

std::string QueryEdgeInstanceRequest::getApiRevision()const
{
	return apiRevision_;
}

void QueryEdgeInstanceRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

