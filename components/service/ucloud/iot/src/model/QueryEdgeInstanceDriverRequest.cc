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

#include <alibabacloud/iot/model/QueryEdgeInstanceDriverRequest.h>

using AlibabaCloud::Iot::Model::QueryEdgeInstanceDriverRequest;

QueryEdgeInstanceDriverRequest::QueryEdgeInstanceDriverRequest() :
	RpcServiceRequest("iot", "2018-01-20", "QueryEdgeInstanceDriver")
{
	setMethod(HttpRequest::Method::Post);
}

QueryEdgeInstanceDriverRequest::~QueryEdgeInstanceDriverRequest()
{}

std::string QueryEdgeInstanceDriverRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void QueryEdgeInstanceDriverRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string QueryEdgeInstanceDriverRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void QueryEdgeInstanceDriverRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

int QueryEdgeInstanceDriverRequest::getPageSize()const
{
	return pageSize_;
}

void QueryEdgeInstanceDriverRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

int QueryEdgeInstanceDriverRequest::getCurrentPage()const
{
	return currentPage_;
}

void QueryEdgeInstanceDriverRequest::setCurrentPage(int currentPage)
{
	currentPage_ = currentPage;
	setParameter("CurrentPage", std::to_string(currentPage));
}

std::string QueryEdgeInstanceDriverRequest::getInstanceId()const
{
	return instanceId_;
}

void QueryEdgeInstanceDriverRequest::setInstanceId(const std::string& instanceId)
{
	instanceId_ = instanceId;
	setParameter("InstanceId", instanceId);
}

std::string QueryEdgeInstanceDriverRequest::getApiProduct()const
{
	return apiProduct_;
}

void QueryEdgeInstanceDriverRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string QueryEdgeInstanceDriverRequest::getApiRevision()const
{
	return apiRevision_;
}

void QueryEdgeInstanceDriverRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

