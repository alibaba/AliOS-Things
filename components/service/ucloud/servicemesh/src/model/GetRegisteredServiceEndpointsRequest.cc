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

#include <alibabacloud/servicemesh/model/GetRegisteredServiceEndpointsRequest.h>

using AlibabaCloud::Servicemesh::Model::GetRegisteredServiceEndpointsRequest;

GetRegisteredServiceEndpointsRequest::GetRegisteredServiceEndpointsRequest() :
	RpcServiceRequest("servicemesh", "2020-01-11", "GetRegisteredServiceEndpoints")
{
	setMethod(HttpRequest::Method::Post);
}

GetRegisteredServiceEndpointsRequest::~GetRegisteredServiceEndpointsRequest()
{}

std::string GetRegisteredServiceEndpointsRequest::get_Namespace()const
{
	return _namespace_;
}

void GetRegisteredServiceEndpointsRequest::set_Namespace(const std::string& _namespace)
{
	_namespace_ = _namespace;
	setParameter("_Namespace", _namespace);
}

std::string GetRegisteredServiceEndpointsRequest::getName()const
{
	return name_;
}

void GetRegisteredServiceEndpointsRequest::setName(const std::string& name)
{
	name_ = name;
	setParameter("Name", name);
}

std::string GetRegisteredServiceEndpointsRequest::getServiceMeshId()const
{
	return serviceMeshId_;
}

void GetRegisteredServiceEndpointsRequest::setServiceMeshId(const std::string& serviceMeshId)
{
	serviceMeshId_ = serviceMeshId;
	setParameter("ServiceMeshId", serviceMeshId);
}

