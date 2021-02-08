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

#include <alibabacloud/servicemesh/model/GetVmMetaRequest.h>

using AlibabaCloud::Servicemesh::Model::GetVmMetaRequest;

GetVmMetaRequest::GetVmMetaRequest() :
	RpcServiceRequest("servicemesh", "2020-01-11", "GetVmMeta")
{
	setMethod(HttpRequest::Method::Get);
}

GetVmMetaRequest::~GetVmMetaRequest()
{}

std::string GetVmMetaRequest::getServiceAccount()const
{
	return serviceAccount_;
}

void GetVmMetaRequest::setServiceAccount(const std::string& serviceAccount)
{
	serviceAccount_ = serviceAccount;
	setParameter("ServiceAccount", serviceAccount);
}

std::string GetVmMetaRequest::getTrustDomain()const
{
	return trustDomain_;
}

void GetVmMetaRequest::setTrustDomain(const std::string& trustDomain)
{
	trustDomain_ = trustDomain;
	setParameter("TrustDomain", trustDomain);
}

std::string GetVmMetaRequest::get_Namespace()const
{
	return _namespace_;
}

void GetVmMetaRequest::set_Namespace(const std::string& _namespace)
{
	_namespace_ = _namespace;
	setParameter("_Namespace", _namespace);
}

std::string GetVmMetaRequest::getServiceMeshId()const
{
	return serviceMeshId_;
}

void GetVmMetaRequest::setServiceMeshId(const std::string& serviceMeshId)
{
	serviceMeshId_ = serviceMeshId;
	setParameter("ServiceMeshId", serviceMeshId);
}

