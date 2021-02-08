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

#include <alibabacloud/servicemesh/model/RemoveClusterFromServiceMeshRequest.h>

using AlibabaCloud::Servicemesh::Model::RemoveClusterFromServiceMeshRequest;

RemoveClusterFromServiceMeshRequest::RemoveClusterFromServiceMeshRequest() :
	RpcServiceRequest("servicemesh", "2020-01-11", "RemoveClusterFromServiceMesh")
{
	setMethod(HttpRequest::Method::Post);
}

RemoveClusterFromServiceMeshRequest::~RemoveClusterFromServiceMeshRequest()
{}

std::string RemoveClusterFromServiceMeshRequest::getClusterId()const
{
	return clusterId_;
}

void RemoveClusterFromServiceMeshRequest::setClusterId(const std::string& clusterId)
{
	clusterId_ = clusterId;
	setBodyParameter("ClusterId", clusterId);
}

std::string RemoveClusterFromServiceMeshRequest::getServiceMeshId()const
{
	return serviceMeshId_;
}

void RemoveClusterFromServiceMeshRequest::setServiceMeshId(const std::string& serviceMeshId)
{
	serviceMeshId_ = serviceMeshId;
	setBodyParameter("ServiceMeshId", serviceMeshId);
}

