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

#include <alibabacloud/servicemesh/model/AddVmAppToMeshRequest.h>

using AlibabaCloud::Servicemesh::Model::AddVmAppToMeshRequest;

AddVmAppToMeshRequest::AddVmAppToMeshRequest() :
	RpcServiceRequest("servicemesh", "2020-01-11", "AddVmAppToMesh")
{
	setMethod(HttpRequest::Method::Post);
}

AddVmAppToMeshRequest::~AddVmAppToMeshRequest()
{}

std::string AddVmAppToMeshRequest::getServiceAccount()const
{
	return serviceAccount_;
}

void AddVmAppToMeshRequest::setServiceAccount(const std::string& serviceAccount)
{
	serviceAccount_ = serviceAccount;
	setParameter("ServiceAccount", serviceAccount);
}

std::string AddVmAppToMeshRequest::getAnnotations()const
{
	return annotations_;
}

void AddVmAppToMeshRequest::setAnnotations(const std::string& annotations)
{
	annotations_ = annotations;
	setParameter("Annotations", annotations);
}

std::string AddVmAppToMeshRequest::getPorts()const
{
	return ports_;
}

void AddVmAppToMeshRequest::setPorts(const std::string& ports)
{
	ports_ = ports;
	setParameter("Ports", ports);
}

std::string AddVmAppToMeshRequest::getServiceMeshId()const
{
	return serviceMeshId_;
}

void AddVmAppToMeshRequest::setServiceMeshId(const std::string& serviceMeshId)
{
	serviceMeshId_ = serviceMeshId;
	setParameter("ServiceMeshId", serviceMeshId);
}

std::string AddVmAppToMeshRequest::getServiceName()const
{
	return serviceName_;
}

void AddVmAppToMeshRequest::setServiceName(const std::string& serviceName)
{
	serviceName_ = serviceName;
	setParameter("ServiceName", serviceName);
}

bool AddVmAppToMeshRequest::getUseWorkload()const
{
	return useWorkload_;
}

void AddVmAppToMeshRequest::setUseWorkload(bool useWorkload)
{
	useWorkload_ = useWorkload;
	setParameter("UseWorkload", useWorkload ? "true" : "false");
}

std::string AddVmAppToMeshRequest::getIps()const
{
	return ips_;
}

void AddVmAppToMeshRequest::setIps(const std::string& ips)
{
	ips_ = ips;
	setParameter("Ips", ips);
}

std::string AddVmAppToMeshRequest::getLabels()const
{
	return labels_;
}

void AddVmAppToMeshRequest::setLabels(const std::string& labels)
{
	labels_ = labels;
	setParameter("Labels", labels);
}

std::string AddVmAppToMeshRequest::get_Namespace()const
{
	return _namespace_;
}

void AddVmAppToMeshRequest::set_Namespace(const std::string& _namespace)
{
	_namespace_ = _namespace;
	setParameter("_Namespace", _namespace);
}

bool AddVmAppToMeshRequest::getForce()const
{
	return force_;
}

void AddVmAppToMeshRequest::setForce(bool force)
{
	force_ = force;
	setParameter("Force", force ? "true" : "false");
}

