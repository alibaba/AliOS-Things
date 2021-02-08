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

#include <alibabacloud/servicemesh/model/UpdateIstioInjectionConfigRequest.h>

using AlibabaCloud::Servicemesh::Model::UpdateIstioInjectionConfigRequest;

UpdateIstioInjectionConfigRequest::UpdateIstioInjectionConfigRequest() :
	RpcServiceRequest("servicemesh", "2020-01-11", "UpdateIstioInjectionConfig")
{
	setMethod(HttpRequest::Method::Post);
}

UpdateIstioInjectionConfigRequest::~UpdateIstioInjectionConfigRequest()
{}

bool UpdateIstioInjectionConfigRequest::getEnableIstioInjection()const
{
	return enableIstioInjection_;
}

void UpdateIstioInjectionConfigRequest::setEnableIstioInjection(bool enableIstioInjection)
{
	enableIstioInjection_ = enableIstioInjection;
	setBodyParameter("EnableIstioInjection", enableIstioInjection ? "true" : "false");
}

std::string UpdateIstioInjectionConfigRequest::get_Namespace()const
{
	return _namespace_;
}

void UpdateIstioInjectionConfigRequest::set_Namespace(const std::string& _namespace)
{
	_namespace_ = _namespace;
	setBodyParameter("_Namespace", _namespace);
}

std::string UpdateIstioInjectionConfigRequest::getServiceMeshId()const
{
	return serviceMeshId_;
}

void UpdateIstioInjectionConfigRequest::setServiceMeshId(const std::string& serviceMeshId)
{
	serviceMeshId_ = serviceMeshId;
	setBodyParameter("ServiceMeshId", serviceMeshId);
}

