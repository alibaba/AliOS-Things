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

#ifndef ALIBABACLOUD_SERVICEMESH_MODEL_UPDATEMESHFEATUREREQUEST_H_
#define ALIBABACLOUD_SERVICEMESH_MODEL_UPDATEMESHFEATUREREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/servicemesh/ServicemeshExport.h>

namespace AlibabaCloud
{
	namespace Servicemesh
	{
		namespace Model
		{
			class ALIBABACLOUD_SERVICEMESH_EXPORT UpdateMeshFeatureRequest : public RpcServiceRequest
			{

			public:
				UpdateMeshFeatureRequest();
				~UpdateMeshFeatureRequest();

				std::string getProxyRequestCPU()const;
				void setProxyRequestCPU(const std::string& proxyRequestCPU);
				std::string getOPALimitCPU()const;
				void setOPALimitCPU(const std::string& oPALimitCPU);
				bool getOpenAgentPolicy()const;
				void setOpenAgentPolicy(bool openAgentPolicy);
				bool getOpaEnabled()const;
				void setOpaEnabled(bool opaEnabled);
				std::string getProxyLimitMemory()const;
				void setProxyLimitMemory(const std::string& proxyLimitMemory);
				std::string getCniExcludeNamespaces()const;
				void setCniExcludeNamespaces(const std::string& cniExcludeNamespaces);
				std::string getOPALogLevel()const;
				void setOPALogLevel(const std::string& oPALogLevel);
				bool getCustomizedZipkin()const;
				void setCustomizedZipkin(bool customizedZipkin);
				std::string getSidecarInjectorRequestCPU()const;
				void setSidecarInjectorRequestCPU(const std::string& sidecarInjectorRequestCPU);
				bool getCniEnabled()const;
				void setCniEnabled(bool cniEnabled);
				bool getTracing()const;
				void setTracing(bool tracing);
				std::string getIncludeIPRanges()const;
				void setIncludeIPRanges(const std::string& includeIPRanges);
				std::string getOPALimitMemory()const;
				void setOPALimitMemory(const std::string& oPALimitMemory);
				bool getCADisableSecretAutoGeneration()const;
				void setCADisableSecretAutoGeneration(bool cADisableSecretAutoGeneration);
				std::string getCAListenedNamespaces()const;
				void setCAListenedNamespaces(const std::string& cAListenedNamespaces);
				std::string getProxyLimitCPU()const;
				void setProxyLimitCPU(const std::string& proxyLimitCPU);
				std::string getProxyRequestMemory()const;
				void setProxyRequestMemory(const std::string& proxyRequestMemory);
				bool getTelemetry()const;
				void setTelemetry(bool telemetry);
				std::string getOPARequestCPU()const;
				void setOPARequestCPU(const std::string& oPARequestCPU);
				std::string getSidecarInjectorWebhookAsYaml()const;
				void setSidecarInjectorWebhookAsYaml(const std::string& sidecarInjectorWebhookAsYaml);
				std::string getOPARequestMemory()const;
				void setOPARequestMemory(const std::string& oPARequestMemory);
				bool getAutoInjectionPolicyEnabled()const;
				void setAutoInjectionPolicyEnabled(bool autoInjectionPolicyEnabled);
				std::string getSidecarInjectorLimitMemory()const;
				void setSidecarInjectorLimitMemory(const std::string& sidecarInjectorLimitMemory);
				bool getEnableAudit()const;
				void setEnableAudit(bool enableAudit);
				std::string getClusterDomain()const;
				void setClusterDomain(const std::string& clusterDomain);
				std::string getSidecarInjectorRequestMemory()const;
				void setSidecarInjectorRequestMemory(const std::string& sidecarInjectorRequestMemory);
				std::string getServiceMeshId()const;
				void setServiceMeshId(const std::string& serviceMeshId);
				bool getLocalityLoadBalancing()const;
				void setLocalityLoadBalancing(bool localityLoadBalancing);
				std::string getSidecarInjectorLimitCPU()const;
				void setSidecarInjectorLimitCPU(const std::string& sidecarInjectorLimitCPU);
				float getTraceSampling()const;
				void setTraceSampling(float traceSampling);
				bool getHttp10Enabled()const;
				void setHttp10Enabled(bool http10Enabled);
				std::string getAppNamespaces()const;
				void setAppNamespaces(const std::string& appNamespaces);
				bool getPilotPublicEip()const;
				void setPilotPublicEip(bool pilotPublicEip);
				std::string getAuditProject()const;
				void setAuditProject(const std::string& auditProject);
				std::string getOutboundTrafficPolicy()const;
				void setOutboundTrafficPolicy(const std::string& outboundTrafficPolicy);
				bool getEnableNamespacesByDefault()const;
				void setEnableNamespacesByDefault(bool enableNamespacesByDefault);

            private:
				std::string proxyRequestCPU_;
				std::string oPALimitCPU_;
				bool openAgentPolicy_;
				bool opaEnabled_;
				std::string proxyLimitMemory_;
				std::string cniExcludeNamespaces_;
				std::string oPALogLevel_;
				bool customizedZipkin_;
				std::string sidecarInjectorRequestCPU_;
				bool cniEnabled_;
				bool tracing_;
				std::string includeIPRanges_;
				std::string oPALimitMemory_;
				bool cADisableSecretAutoGeneration_;
				std::string cAListenedNamespaces_;
				std::string proxyLimitCPU_;
				std::string proxyRequestMemory_;
				bool telemetry_;
				std::string oPARequestCPU_;
				std::string sidecarInjectorWebhookAsYaml_;
				std::string oPARequestMemory_;
				bool autoInjectionPolicyEnabled_;
				std::string sidecarInjectorLimitMemory_;
				bool enableAudit_;
				std::string clusterDomain_;
				std::string sidecarInjectorRequestMemory_;
				std::string serviceMeshId_;
				bool localityLoadBalancing_;
				std::string sidecarInjectorLimitCPU_;
				float traceSampling_;
				bool http10Enabled_;
				std::string appNamespaces_;
				bool pilotPublicEip_;
				std::string auditProject_;
				std::string outboundTrafficPolicy_;
				bool enableNamespacesByDefault_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_SERVICEMESH_MODEL_UPDATEMESHFEATUREREQUEST_H_