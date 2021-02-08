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

#include <alibabacloud/iot/model/QueryEdgeDriverVersionRequest.h>

using AlibabaCloud::Iot::Model::QueryEdgeDriverVersionRequest;

QueryEdgeDriverVersionRequest::QueryEdgeDriverVersionRequest() :
	RpcServiceRequest("iot", "2018-01-20", "QueryEdgeDriverVersion")
{
	setMethod(HttpRequest::Method::Post);
}

QueryEdgeDriverVersionRequest::~QueryEdgeDriverVersionRequest()
{}

int QueryEdgeDriverVersionRequest::getVersionState()const
{
	return versionState_;
}

void QueryEdgeDriverVersionRequest::setVersionState(int versionState)
{
	versionState_ = versionState;
	setParameter("VersionState", std::to_string(versionState));
}

std::string QueryEdgeDriverVersionRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void QueryEdgeDriverVersionRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string QueryEdgeDriverVersionRequest::getDriverId()const
{
	return driverId_;
}

void QueryEdgeDriverVersionRequest::setDriverId(const std::string& driverId)
{
	driverId_ = driverId;
	setParameter("DriverId", driverId);
}

std::string QueryEdgeDriverVersionRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void QueryEdgeDriverVersionRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

int QueryEdgeDriverVersionRequest::getPageSize()const
{
	return pageSize_;
}

void QueryEdgeDriverVersionRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::string QueryEdgeDriverVersionRequest::getDriverVersion()const
{
	return driverVersion_;
}

void QueryEdgeDriverVersionRequest::setDriverVersion(const std::string& driverVersion)
{
	driverVersion_ = driverVersion;
	setParameter("DriverVersion", driverVersion);
}

int QueryEdgeDriverVersionRequest::getCurrentPage()const
{
	return currentPage_;
}

void QueryEdgeDriverVersionRequest::setCurrentPage(int currentPage)
{
	currentPage_ = currentPage;
	setParameter("CurrentPage", std::to_string(currentPage));
}

std::string QueryEdgeDriverVersionRequest::getApiProduct()const
{
	return apiProduct_;
}

void QueryEdgeDriverVersionRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string QueryEdgeDriverVersionRequest::getApiRevision()const
{
	return apiRevision_;
}

void QueryEdgeDriverVersionRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

