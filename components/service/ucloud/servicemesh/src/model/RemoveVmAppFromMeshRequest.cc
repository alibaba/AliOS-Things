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

#include <alibabacloud/servicemesh/model/RemoveVmAppFromMeshRequest.h>

using AlibabaCloud::Servicemesh::Model::RemoveVmAppFromMeshRequest;

RemoveVmAppFromMeshRequest::RemoveVmAppFromMeshRequest() :
	RpcServiceRequest("servicemesh", "2020-01-11", "RemoveVmAppFromMesh")
{
	setMethod(HttpRequest::Method::Post);
}

RemoveVmAppFromMeshRequest::~RemoveVmAppFromMeshRequest()
{}

std::string RemoveVmAppFromMeshRequest::get_Namespace()const
{
	return _namespace_;
}

void RemoveVmAppFromMeshRequest::set_Namespace(const std::string& _namespace)
{
	_namespace_ = _namespace;
	setParameter("_Namespace", _namespace);
}

std::string RemoveVmAppFromMeshRequest::getServiceMeshId()const
{
	return serviceMeshId_;
}

void RemoveVmAppFromMeshRequest::setServiceMeshId(const std::string& serviceMeshId)
{
	serviceMeshId_ = serviceMeshId;
	setParameter("ServiceMeshId", serviceMeshId);
}

std::string RemoveVmAppFromMeshRequest::getServiceName()const
{
	return serviceName_;
}

void RemoveVmAppFromMeshRequest::setServiceName(const std::string& serviceName)
{
	serviceName_ = serviceName;
	setParameter("ServiceName", serviceName);
}

