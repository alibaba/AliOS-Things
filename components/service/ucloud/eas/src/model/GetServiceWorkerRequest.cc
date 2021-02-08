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

#include <alibabacloud/eas/model/GetServiceWorkerRequest.h>

using AlibabaCloud::Eas::Model::GetServiceWorkerRequest;

GetServiceWorkerRequest::GetServiceWorkerRequest() :
	RoaServiceRequest("eas", "2018-05-22")
{
	setResourcePath("/api/services/[region]/[service_name]/worker");
	setMethod(HttpRequest::Method::Get);
}

GetServiceWorkerRequest::~GetServiceWorkerRequest()
{}

std::string GetServiceWorkerRequest::getService_name()const
{
	return service_name_;
}

void GetServiceWorkerRequest::setService_name(const std::string& service_name)
{
	service_name_ = service_name;
	setParameter("Service_name", service_name);
}

std::string GetServiceWorkerRequest::getRegion()const
{
	return region_;
}

void GetServiceWorkerRequest::setRegion(const std::string& region)
{
	region_ = region;
	setParameter("Region", region);
}

