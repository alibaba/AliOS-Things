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

#include <alibabacloud/servicemesh/model/DeleteServiceMeshRequest.h>

using AlibabaCloud::Servicemesh::Model::DeleteServiceMeshRequest;

DeleteServiceMeshRequest::DeleteServiceMeshRequest() :
	RpcServiceRequest("servicemesh", "2020-01-11", "DeleteServiceMesh")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteServiceMeshRequest::~DeleteServiceMeshRequest()
{}

std::string DeleteServiceMeshRequest::getServiceMeshId()const
{
	return serviceMeshId_;
}

void DeleteServiceMeshRequest::setServiceMeshId(const std::string& serviceMeshId)
{
	serviceMeshId_ = serviceMeshId;
	setBodyParameter("ServiceMeshId", serviceMeshId);
}

bool DeleteServiceMeshRequest::getForce()const
{
	return force_;
}

void DeleteServiceMeshRequest::setForce(bool force)
{
	force_ = force;
	setBodyParameter("Force", force ? "true" : "false");
}

