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

#include <alibabacloud/servicemesh/model/UpdateMeshFeatureRequest.h>

using AlibabaCloud::Servicemesh::Model::UpdateMeshFeatureRequest;

UpdateMeshFeatureRequest::UpdateMeshFeatureRequest() :
	RpcServiceRequest("servicemesh", "2020-01-11", "UpdateMeshFeature")
{
	setMethod(HttpRequest::Method::Post);
}

UpdateMeshFeatureRequest::~UpdateMeshFeatureRequest()
{}

std::string UpdateMeshFeatureRequest::getProxyRequestCPU()const
{
	return proxyRequestCPU_;
}

void UpdateMeshFeatureRequest::setProxyRequestCPU(const std::string& proxyRequestCPU)
{
	proxyRequestCPU_ = proxyRequestCPU;
	setBodyParameter("ProxyRequestCPU", proxyRequestCPU);
}

std::string UpdateMeshFeatureRequest::getOPALimitCPU()const
{
	return oPALimitCPU_;
}

void UpdateMeshFeatureRequest::setOPALimitCPU(const std::string& oPALimitCPU)
{
	oPALimitCPU_ = oPALimitCPU;
	setBodyParameter("OPALimitCPU", oPALimitCPU);
}

bool UpdateMeshFeatureRequest::getOpenAgentPolicy()const
{
	return openAgentPolicy_;
}

void UpdateMeshFeatureRequest::setOpenAgentPolicy(bool openAgentPolicy)
{
	openAgentPolicy_ = openAgentPolicy;
	setBodyParameter("OpenAgentPolicy", openAgentPolicy ? "true" : "false");
}

bool UpdateMeshFeatureRequest::getOpaEnabled()const
{
	return opaEnabled_;
}

void UpdateMeshFeatureRequest::setOpaEnabled(bool opaEnabled)
{
	opaEnabled_ = opaEnabled;
	setBodyParameter("OpaEnabled", opaEnabled ? "true" : "false");
}

std::string UpdateMeshFeatureRequest::getProxyLimitMemory()const
{
	return proxyLimitMemory_;
}

void UpdateMeshFeatureRequest::setProxyLimitMemory(const std::string& proxyLimitMemory)
{
	proxyLimitMemory_ = proxyLimitMemory;
	setBodyParameter("ProxyLimitMemory", proxyLimitMemory);
}

std::string UpdateMeshFeatureRequest::getCniExcludeNamespaces()const
{
	return cniExcludeNamespaces_;
}

void UpdateMeshFeatureRequest::setCniExcludeNamespaces(const std::string& cniExcludeNamespaces)
{
	cniExcludeNamespaces_ = cniExcludeNamespaces;
	setBodyParameter("CniExcludeNamespaces", cniExcludeNamespaces);
}

std::string UpdateMeshFeatureRequest::getOPALogLevel()const
{
	return oPALogLevel_;
}

void UpdateMeshFeatureRequest::setOPALogLevel(const std::string& oPALogLevel)
{
	oPALogLevel_ = oPALogLevel;
	setBodyParameter("OPALogLevel", oPALogLevel);
}

bool UpdateMeshFeatureRequest::getCustomizedZipkin()const
{
	return customizedZipkin_;
}

void UpdateMeshFeatureRequest::setCustomizedZipkin(bool customizedZipkin)
{
	customizedZipkin_ = customizedZipkin;
	setBodyParameter("CustomizedZipkin", customizedZipkin ? "true" : "false");
}

std::string UpdateMeshFeatureRequest::getSidecarInjectorRequestCPU()const
{
	return sidecarInjectorRequestCPU_;
}

void UpdateMeshFeatureRequest::setSidecarInjectorRequestCPU(const std::string& sidecarInjectorRequestCPU)
{
	sidecarInjectorRequestCPU_ = sidecarInjectorRequestCPU;
	setBodyParameter("SidecarInjectorRequestCPU", sidecarInjectorRequestCPU);
}

bool UpdateMeshFeatureRequest::getCniEnabled()const
{
	return cniEnabled_;
}

void UpdateMeshFeatureRequest::setCniEnabled(bool cniEnabled)
{
	cniEnabled_ = cniEnabled;
	setBodyParameter("CniEnabled", cniEnabled ? "true" : "false");
}

bool UpdateMeshFeatureRequest::getTracing()const
{
	return tracing_;
}

void UpdateMeshFeatureRequest::setTracing(bool tracing)
{
	tracing_ = tracing;
	setBodyParameter("Tracing", tracing ? "true" : "false");
}

std::string UpdateMeshFeatureRequest::getIncludeIPRanges()const
{
	return includeIPRanges_;
}

void UpdateMeshFeatureRequest::setIncludeIPRanges(const std::string& includeIPRanges)
{
	includeIPRanges_ = includeIPRanges;
	setBodyParameter("IncludeIPRanges", includeIPRanges);
}

std::string UpdateMeshFeatureRequest::getOPALimitMemory()const
{
	return oPALimitMemory_;
}

void UpdateMeshFeatureRequest::setOPALimitMemory(const std::string& oPALimitMemory)
{
	oPALimitMemory_ = oPALimitMemory;
	setBodyParameter("OPALimitMemory", oPALimitMemory);
}

bool UpdateMeshFeatureRequest::getCADisableSecretAutoGeneration()const
{
	return cADisableSecretAutoGeneration_;
}

void UpdateMeshFeatureRequest::setCADisableSecretAutoGeneration(bool cADisableSecretAutoGeneration)
{
	cADisableSecretAutoGeneration_ = cADisableSecretAutoGeneration;
	setBodyParameter("CADisableSecretAutoGeneration", cADisableSecretAutoGeneration ? "true" : "false");
}

std::string UpdateMeshFeatureRequest::getCAListenedNamespaces()const
{
	return cAListenedNamespaces_;
}

void UpdateMeshFeatureRequest::setCAListenedNamespaces(const std::string& cAListenedNamespaces)
{
	cAListenedNamespaces_ = cAListenedNamespaces;
	setBodyParameter("CAListenedNamespaces", cAListenedNamespaces);
}

std::string UpdateMeshFeatureRequest::getProxyLimitCPU()const
{
	return proxyLimitCPU_;
}

void UpdateMeshFeatureRequest::setProxyLimitCPU(const std::string& proxyLimitCPU)
{
	proxyLimitCPU_ = proxyLimitCPU;
	setBodyParameter("ProxyLimitCPU", proxyLimitCPU);
}

std::string UpdateMeshFeatureRequest::getProxyRequestMemory()const
{
	return proxyRequestMemory_;
}

void UpdateMeshFeatureRequest::setProxyRequestMemory(const std::string& proxyRequestMemory)
{
	proxyRequestMemory_ = proxyRequestMemory;
	setBodyParameter("ProxyRequestMemory", proxyRequestMemory);
}

bool UpdateMeshFeatureRequest::getTelemetry()const
{
	return telemetry_;
}

void UpdateMeshFeatureRequest::setTelemetry(bool telemetry)
{
	telemetry_ = telemetry;
	setBodyParameter("Telemetry", telemetry ? "true" : "false");
}

std::string UpdateMeshFeatureRequest::getOPARequestCPU()const
{
	return oPARequestCPU_;
}

void UpdateMeshFeatureRequest::setOPARequestCPU(const std::string& oPARequestCPU)
{
	oPARequestCPU_ = oPARequestCPU;
	setBodyParameter("OPARequestCPU", oPARequestCPU);
}

std::string UpdateMeshFeatureRequest::getSidecarInjectorWebhookAsYaml()const
{
	return sidecarInjectorWebhookAsYaml_;
}

void UpdateMeshFeatureRequest::setSidecarInjectorWebhookAsYaml(const std::string& sidecarInjectorWebhookAsYaml)
{
	sidecarInjectorWebhookAsYaml_ = sidecarInjectorWebhookAsYaml;
	setBodyParameter("SidecarInjectorWebhookAsYaml", sidecarInjectorWebhookAsYaml);
}

std::string UpdateMeshFeatureRequest::getOPARequestMemory()const
{
	return oPARequestMemory_;
}

void UpdateMeshFeatureRequest::setOPARequestMemory(const std::string& oPARequestMemory)
{
	oPARequestMemory_ = oPARequestMemory;
	setBodyParameter("OPARequestMemory", oPARequestMemory);
}

bool UpdateMeshFeatureRequest::getAutoInjectionPolicyEnabled()const
{
	return autoInjectionPolicyEnabled_;
}

void UpdateMeshFeatureRequest::setAutoInjectionPolicyEnabled(bool autoInjectionPolicyEnabled)
{
	autoInjectionPolicyEnabled_ = autoInjectionPolicyEnabled;
	setBodyParameter("AutoInjectionPolicyEnabled", autoInjectionPolicyEnabled ? "true" : "false");
}

std::string UpdateMeshFeatureRequest::getSidecarInjectorLimitMemory()const
{
	return sidecarInjectorLimitMemory_;
}

void UpdateMeshFeatureRequest::setSidecarInjectorLimitMemory(const std::string& sidecarInjectorLimitMemory)
{
	sidecarInjectorLimitMemory_ = sidecarInjectorLimitMemory;
	setBodyParameter("SidecarInjectorLimitMemory", sidecarInjectorLimitMemory);
}

bool UpdateMeshFeatureRequest::getEnableAudit()const
{
	return enableAudit_;
}

void UpdateMeshFeatureRequest::setEnableAudit(bool enableAudit)
{
	enableAudit_ = enableAudit;
	setBodyParameter("EnableAudit", enableAudit ? "true" : "false");
}

std::string UpdateMeshFeatureRequest::getClusterDomain()const
{
	return clusterDomain_;
}

void UpdateMeshFeatureRequest::setClusterDomain(const std::string& clusterDomain)
{
	clusterDomain_ = clusterDomain;
	setBodyParameter("ClusterDomain", clusterDomain);
}

std::string UpdateMeshFeatureRequest::getSidecarInjectorRequestMemory()const
{
	return sidecarInjectorRequestMemory_;
}

void UpdateMeshFeatureRequest::setSidecarInjectorRequestMemory(const std::string& sidecarInjectorRequestMemory)
{
	sidecarInjectorRequestMemory_ = sidecarInjectorRequestMemory;
	setBodyParameter("SidecarInjectorRequestMemory", sidecarInjectorRequestMemory);
}

std::string UpdateMeshFeatureRequest::getServiceMeshId()const
{
	return serviceMeshId_;
}

void UpdateMeshFeatureRequest::setServiceMeshId(const std::string& serviceMeshId)
{
	serviceMeshId_ = serviceMeshId;
	setBodyParameter("ServiceMeshId", serviceMeshId);
}

bool UpdateMeshFeatureRequest::getLocalityLoadBalancing()const
{
	return localityLoadBalancing_;
}

void UpdateMeshFeatureRequest::setLocalityLoadBalancing(bool localityLoadBalancing)
{
	localityLoadBalancing_ = localityLoadBalancing;
	setBodyParameter("LocalityLoadBalancing", localityLoadBalancing ? "true" : "false");
}

std::string UpdateMeshFeatureRequest::getSidecarInjectorLimitCPU()const
{
	return sidecarInjectorLimitCPU_;
}

void UpdateMeshFeatureRequest::setSidecarInjectorLimitCPU(const std::string& sidecarInjectorLimitCPU)
{
	sidecarInjectorLimitCPU_ = sidecarInjectorLimitCPU;
	setBodyParameter("SidecarInjectorLimitCPU", sidecarInjectorLimitCPU);
}

float UpdateMeshFeatureRequest::getTraceSampling()const
{
	return traceSampling_;
}

void UpdateMeshFeatureRequest::setTraceSampling(float traceSampling)
{
	traceSampling_ = traceSampling;
	setBodyParameter("TraceSampling", std::to_string(traceSampling));
}

bool UpdateMeshFeatureRequest::getHttp10Enabled()const
{
	return http10Enabled_;
}

void UpdateMeshFeatureRequest::setHttp10Enabled(bool http10Enabled)
{
	http10Enabled_ = http10Enabled;
	setBodyParameter("Http10Enabled", http10Enabled ? "true" : "false");
}

std::string UpdateMeshFeatureRequest::getAppNamespaces()const
{
	return appNamespaces_;
}

void UpdateMeshFeatureRequest::setAppNamespaces(const std::string& appNamespaces)
{
	appNamespaces_ = appNamespaces;
	setBodyParameter("AppNamespaces", appNamespaces);
}

bool UpdateMeshFeatureRequest::getPilotPublicEip()const
{
	return pilotPublicEip_;
}

void UpdateMeshFeatureRequest::setPilotPublicEip(bool pilotPublicEip)
{
	pilotPublicEip_ = pilotPublicEip;
	setBodyParameter("PilotPublicEip", pilotPublicEip ? "true" : "false");
}

std::string UpdateMeshFeatureRequest::getAuditProject()const
{
	return auditProject_;
}

void UpdateMeshFeatureRequest::setAuditProject(const std::string& auditProject)
{
	auditProject_ = auditProject;
	setBodyParameter("AuditProject", auditProject);
}

std::string UpdateMeshFeatureRequest::getOutboundTrafficPolicy()const
{
	return outboundTrafficPolicy_;
}

void UpdateMeshFeatureRequest::setOutboundTrafficPolicy(const std::string& outboundTrafficPolicy)
{
	outboundTrafficPolicy_ = outboundTrafficPolicy;
	setBodyParameter("OutboundTrafficPolicy", outboundTrafficPolicy);
}

bool UpdateMeshFeatureRequest::getEnableNamespacesByDefault()const
{
	return enableNamespacesByDefault_;
}

void UpdateMeshFeatureRequest::setEnableNamespacesByDefault(bool enableNamespacesByDefault)
{
	enableNamespacesByDefault_ = enableNamespacesByDefault;
	setBodyParameter("EnableNamespacesByDefault", enableNamespacesByDefault ? "true" : "false");
}

