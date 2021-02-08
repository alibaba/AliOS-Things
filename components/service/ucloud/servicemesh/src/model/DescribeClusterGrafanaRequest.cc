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

#include <alibabacloud/servicemesh/model/DescribeClusterGrafanaRequest.h>

using AlibabaCloud::Servicemesh::Model::DescribeClusterGrafanaRequest;

DescribeClusterGrafanaRequest::DescribeClusterGrafanaRequest() :
	RpcServiceRequest("servicemesh", "2020-01-11", "DescribeClusterGrafana")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeClusterGrafanaRequest::~DescribeClusterGrafanaRequest()
{}

std::string DescribeClusterGrafanaRequest::getK8sClusterId()const
{
	return k8sClusterId_;
}

void DescribeClusterGrafanaRequest::setK8sClusterId(const std::string& k8sClusterId)
{
	k8sClusterId_ = k8sClusterId;
	setParameter("K8sClusterId", k8sClusterId);
}

std::string DescribeClusterGrafanaRequest::getServiceMeshId()const
{
	return serviceMeshId_;
}

void DescribeClusterGrafanaRequest::setServiceMeshId(const std::string& serviceMeshId)
{
	serviceMeshId_ = serviceMeshId;
	setParameter("ServiceMeshId", serviceMeshId);
}

