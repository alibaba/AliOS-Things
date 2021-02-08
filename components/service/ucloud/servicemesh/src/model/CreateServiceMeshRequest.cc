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

#include <alibabacloud/servicemesh/model/CreateServiceMeshRequest.h>

using AlibabaCloud::Servicemesh::Model::CreateServiceMeshRequest;

CreateServiceMeshRequest::CreateServiceMeshRequest() :
	RpcServiceRequest("servicemesh", "2020-01-11", "CreateServiceMesh")
{
	setMethod(HttpRequest::Method::Post);
}

CreateServiceMeshRequest::~CreateServiceMeshRequest()
{}

std::string CreateServiceMeshRequest::getProxyRequestCPU()const
{
	return proxyRequestCPU_;
}

void CreateServiceMeshRequest::setProxyRequestCPU(const std::string& proxyRequestCPU)
{
	proxyRequestCPU_ = proxyRequestCPU;
	setBodyParameter("ProxyRequestCPU", proxyRequestCPU);
}

std::string CreateServiceMeshRequest::getOPALimitCPU()const
{
	return oPALimitCPU_;
}

void CreateServiceMeshRequest::setOPALimitCPU(const std::string& oPALimitCPU)
{
	oPALimitCPU_ = oPALimitCPU;
	setBodyParameter("OPALimitCPU", oPALimitCPU);
}

bool CreateServiceMeshRequest::getOpenAgentPolicy()const
{
	return openAgentPolicy_;
}

void CreateServiceMeshRequest::setOpenAgentPolicy(bool openAgentPolicy)
{
	openAgentPolicy_ = openAgentPolicy;
	setBodyParameter("OpenAgentPolicy", openAgentPolicy ? "true" : "false");
}

bool CreateServiceMeshRequest::getOpaEnabled()const
{
	return opaEnabled_;
}

void CreateServiceMeshRequest::setOpaEnabled(bool opaEnabled)
{
	opaEnabled_ = opaEnabled;
	setBodyParameter("OpaEnabled", opaEnabled ? "true" : "false");
}

std::string CreateServiceMeshRequest::getProxyLimitMemory()const
{
	return proxyLimitMemory_;
}

void CreateServiceMeshRequest::setProxyLimitMemory(const std::string& proxyLimitMemory)
{
	proxyLimitMemory_ = proxyLimitMemory;
	setBodyParameter("ProxyLimitMemory", proxyLimitMemory);
}

bool CreateServiceMeshRequest::getStrictMTLS()const
{
	return strictMTLS_;
}

void CreateServiceMeshRequest::setStrictMTLS(bool strictMTLS)
{
	strictMTLS_ = strictMTLS;
	setBodyParameter("StrictMTLS", strictMTLS ? "true" : "false");
}

std::string CreateServiceMeshRequest::getOPALogLevel()const
{
	return oPALogLevel_;
}

void CreateServiceMeshRequest::setOPALogLevel(const std::string& oPALogLevel)
{
	oPALogLevel_ = oPALogLevel;
	setBodyParameter("OPALogLevel", oPALogLevel);
}

std::string CreateServiceMeshRequest::getExcludeIPRanges()const
{
	return excludeIPRanges_;
}

void CreateServiceMeshRequest::setExcludeIPRanges(const std::string& excludeIPRanges)
{
	excludeIPRanges_ = excludeIPRanges;
	setBodyParameter("ExcludeIPRanges", excludeIPRanges);
}

std::string CreateServiceMeshRequest::getIstioVersion()const
{
	return istioVersion_;
}

void CreateServiceMeshRequest::setIstioVersion(const std::string& istioVersion)
{
	istioVersion_ = istioVersion;
	setBodyParameter("IstioVersion", istioVersion);
}

bool CreateServiceMeshRequest::getTracing()const
{
	return tracing_;
}

void CreateServiceMeshRequest::setTracing(bool tracing)
{
	tracing_ = tracing;
	setBodyParameter("Tracing", tracing ? "true" : "false");
}

std::string CreateServiceMeshRequest::getIncludeIPRanges()const
{
	return includeIPRanges_;
}

void CreateServiceMeshRequest::setIncludeIPRanges(const std::string& includeIPRanges)
{
	includeIPRanges_ = includeIPRanges;
	setBodyParameter("IncludeIPRanges", includeIPRanges);
}

std::string CreateServiceMeshRequest::getExcludeInboundPorts()const
{
	return excludeInboundPorts_;
}

void CreateServiceMeshRequest::setExcludeInboundPorts(const std::string& excludeInboundPorts)
{
	excludeInboundPorts_ = excludeInboundPorts;
	setBodyParameter("ExcludeInboundPorts", excludeInboundPorts);
}

std::string CreateServiceMeshRequest::getOPALimitMemory()const
{
	return oPALimitMemory_;
}

void CreateServiceMeshRequest::setOPALimitMemory(const std::string& oPALimitMemory)
{
	oPALimitMemory_ = oPALimitMemory;
	setBodyParameter("OPALimitMemory", oPALimitMemory);
}

std::string CreateServiceMeshRequest::getVSwitches()const
{
	return vSwitches_;
}

void CreateServiceMeshRequest::setVSwitches(const std::string& vSwitches)
{
	vSwitches_ = vSwitches;
	setBodyParameter("VSwitches", vSwitches);
}

bool CreateServiceMeshRequest::getCADisableSecretAutoGeneration()const
{
	return cADisableSecretAutoGeneration_;
}

void CreateServiceMeshRequest::setCADisableSecretAutoGeneration(bool cADisableSecretAutoGeneration)
{
	cADisableSecretAutoGeneration_ = cADisableSecretAutoGeneration;
	setBodyParameter("CADisableSecretAutoGeneration", cADisableSecretAutoGeneration ? "true" : "false");
}

std::string CreateServiceMeshRequest::getCAListenedNamespaces()const
{
	return cAListenedNamespaces_;
}

void CreateServiceMeshRequest::setCAListenedNamespaces(const std::string& cAListenedNamespaces)
{
	cAListenedNamespaces_ = cAListenedNamespaces;
	setBodyParameter("CAListenedNamespaces", cAListenedNamespaces);
}

std::string CreateServiceMeshRequest::getProxyLimitCPU()const
{
	return proxyLimitCPU_;
}

void CreateServiceMeshRequest::setProxyLimitCPU(const std::string& proxyLimitCPU)
{
	proxyLimitCPU_ = proxyLimitCPU;
	setBodyParameter("ProxyLimitCPU", proxyLimitCPU);
}

std::string CreateServiceMeshRequest::getProxyRequestMemory()const
{
	return proxyRequestMemory_;
}

void CreateServiceMeshRequest::setProxyRequestMemory(const std::string& proxyRequestMemory)
{
	proxyRequestMemory_ = proxyRequestMemory;
	setBodyParameter("ProxyRequestMemory", proxyRequestMemory);
}

std::string CreateServiceMeshRequest::getName()const
{
	return name_;
}

void CreateServiceMeshRequest::setName(const std::string& name)
{
	name_ = name;
	setBodyParameter("Name", name);
}

bool CreateServiceMeshRequest::getTelemetry()const
{
	return telemetry_;
}

void CreateServiceMeshRequest::setTelemetry(bool telemetry)
{
	telemetry_ = telemetry;
	setBodyParameter("Telemetry", telemetry ? "true" : "false");
}

std::string CreateServiceMeshRequest::getOPARequestCPU()const
{
	return oPARequestCPU_;
}

void CreateServiceMeshRequest::setOPARequestCPU(const std::string& oPARequestCPU)
{
	oPARequestCPU_ = oPARequestCPU;
	setBodyParameter("OPARequestCPU", oPARequestCPU);
}

std::string CreateServiceMeshRequest::getOPARequestMemory()const
{
	return oPARequestMemory_;
}

void CreateServiceMeshRequest::setOPARequestMemory(const std::string& oPARequestMemory)
{
	oPARequestMemory_ = oPARequestMemory;
	setBodyParameter("OPARequestMemory", oPARequestMemory);
}

bool CreateServiceMeshRequest::getEnableAudit()const
{
	return enableAudit_;
}

void CreateServiceMeshRequest::setEnableAudit(bool enableAudit)
{
	enableAudit_ = enableAudit;
	setBodyParameter("EnableAudit", enableAudit ? "true" : "false");
}

std::string CreateServiceMeshRequest::getClusterDomain()const
{
	return clusterDomain_;
}

void CreateServiceMeshRequest::setClusterDomain(const std::string& clusterDomain)
{
	clusterDomain_ = clusterDomain;
	setBodyParameter("ClusterDomain", clusterDomain);
}

std::string CreateServiceMeshRequest::getRegionId()const
{
	return regionId_;
}

void CreateServiceMeshRequest::setRegionId(const std::string& regionId)
{
	regionId_ = regionId;
	setBodyParameter("RegionId", regionId);
}

bool CreateServiceMeshRequest::getLocalityLoadBalancing()const
{
	return localityLoadBalancing_;
}

void CreateServiceMeshRequest::setLocalityLoadBalancing(bool localityLoadBalancing)
{
	localityLoadBalancing_ = localityLoadBalancing;
	setBodyParameter("LocalityLoadBalancing", localityLoadBalancing ? "true" : "false");
}

bool CreateServiceMeshRequest::getApiServerPublicEip()const
{
	return apiServerPublicEip_;
}

void CreateServiceMeshRequest::setApiServerPublicEip(bool apiServerPublicEip)
{
	apiServerPublicEip_ = apiServerPublicEip;
	setBodyParameter("ApiServerPublicEip", apiServerPublicEip ? "true" : "false");
}

float CreateServiceMeshRequest::getTraceSampling()const
{
	return traceSampling_;
}

void CreateServiceMeshRequest::setTraceSampling(float traceSampling)
{
	traceSampling_ = traceSampling;
	setBodyParameter("TraceSampling", std::to_string(traceSampling));
}

std::string CreateServiceMeshRequest::getAppNamespaces()const
{
	return appNamespaces_;
}

void CreateServiceMeshRequest::setAppNamespaces(const std::string& appNamespaces)
{
	appNamespaces_ = appNamespaces;
	setBodyParameter("AppNamespaces", appNamespaces);
}

bool CreateServiceMeshRequest::getPilotPublicEip()const
{
	return pilotPublicEip_;
}

void CreateServiceMeshRequest::setPilotPublicEip(bool pilotPublicEip)
{
	pilotPublicEip_ = pilotPublicEip;
	setBodyParameter("PilotPublicEip", pilotPublicEip ? "true" : "false");
}

std::string CreateServiceMeshRequest::getAuditProject()const
{
	return auditProject_;
}

void CreateServiceMeshRequest::setAuditProject(const std::string& auditProject)
{
	auditProject_ = auditProject;
	setBodyParameter("AuditProject", auditProject);
}

std::string CreateServiceMeshRequest::getOutboundTrafficPolicy()const
{
	return outboundTrafficPolicy_;
}

void CreateServiceMeshRequest::setOutboundTrafficPolicy(const std::string& outboundTrafficPolicy)
{
	outboundTrafficPolicy_ = outboundTrafficPolicy;
	setBodyParameter("OutboundTrafficPolicy", outboundTrafficPolicy);
}

std::string CreateServiceMeshRequest::getVpcId()const
{
	return vpcId_;
}

void CreateServiceMeshRequest::setVpcId(const std::string& vpcId)
{
	vpcId_ = vpcId;
	setBodyParameter("VpcId", vpcId);
}

std::string CreateServiceMeshRequest::getExcludeOutboundPorts()const
{
	return excludeOutboundPorts_;
}

void CreateServiceMeshRequest::setExcludeOutboundPorts(const std::string& excludeOutboundPorts)
{
	excludeOutboundPorts_ = excludeOutboundPorts;
	setBodyParameter("ExcludeOutboundPorts", excludeOutboundPorts);
}

