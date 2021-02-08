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

#include <alibabacloud/servicemesh/model/DescribeClusterPrometheusRequest.h>

using AlibabaCloud::Servicemesh::Model::DescribeClusterPrometheusRequest;

DescribeClusterPrometheusRequest::DescribeClusterPrometheusRequest() :
	RpcServiceRequest("servicemesh", "2020-01-11", "DescribeClusterPrometheus")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeClusterPrometheusRequest::~DescribeClusterPrometheusRequest()
{}

std::string DescribeClusterPrometheusRequest::getK8sClusterId()const
{
	return k8sClusterId_;
}

void DescribeClusterPrometheusRequest::setK8sClusterId(const std::string& k8sClusterId)
{
	k8sClusterId_ = k8sClusterId;
	setParameter("K8sClusterId", k8sClusterId);
}

std::string DescribeClusterPrometheusRequest::getK8sClusterRegionId()const
{
	return k8sClusterRegionId_;
}

void DescribeClusterPrometheusRequest::setK8sClusterRegionId(const std::string& k8sClusterRegionId)
{
	k8sClusterRegionId_ = k8sClusterRegionId;
	setParameter("K8sClusterRegionId", k8sClusterRegionId);
}

std::string DescribeClusterPrometheusRequest::getServiceMeshId()const
{
	return serviceMeshId_;
}

void DescribeClusterPrometheusRequest::setServiceMeshId(const std::string& serviceMeshId)
{
	serviceMeshId_ = serviceMeshId;
	setParameter("ServiceMeshId", serviceMeshId);
}

