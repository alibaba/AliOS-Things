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

#include <alibabacloud/iot/model/QueryEdgeDriverRequest.h>

using AlibabaCloud::Iot::Model::QueryEdgeDriverRequest;

QueryEdgeDriverRequest::QueryEdgeDriverRequest() :
	RpcServiceRequest("iot", "2018-01-20", "QueryEdgeDriver")
{
	setMethod(HttpRequest::Method::Post);
}

QueryEdgeDriverRequest::~QueryEdgeDriverRequest()
{}

std::string QueryEdgeDriverRequest::getDriverName()const
{
	return driverName_;
}

void QueryEdgeDriverRequest::setDriverName(const std::string& driverName)
{
	driverName_ = driverName;
	setParameter("DriverName", driverName);
}

int QueryEdgeDriverRequest::getType()const
{
	return type_;
}

void QueryEdgeDriverRequest::setType(int type)
{
	type_ = type;
	setParameter("Type", std::to_string(type));
}

std::string QueryEdgeDriverRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void QueryEdgeDriverRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string QueryEdgeDriverRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void QueryEdgeDriverRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

int QueryEdgeDriverRequest::getPageSize()const
{
	return pageSize_;
}

void QueryEdgeDriverRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

int QueryEdgeDriverRequest::getCurrentPage()const
{
	return currentPage_;
}

void QueryEdgeDriverRequest::setCurrentPage(int currentPage)
{
	currentPage_ = currentPage;
	setParameter("CurrentPage", std::to_string(currentPage));
}

std::string QueryEdgeDriverRequest::getApiProduct()const
{
	return apiProduct_;
}

void QueryEdgeDriverRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string QueryEdgeDriverRequest::getApiRevision()const
{
	return apiRevision_;
}

void QueryEdgeDriverRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

