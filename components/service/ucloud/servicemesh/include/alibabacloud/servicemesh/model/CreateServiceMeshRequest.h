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

#ifndef ALIBABACLOUD_SERVICEMESH_MODEL_CREATESERVICEMESHREQUEST_H_
#define ALIBABACLOUD_SERVICEMESH_MODEL_CREATESERVICEMESHREQUEST_H_

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
			class ALIBABACLOUD_SERVICEMESH_EXPORT CreateServiceMeshRequest : public RpcServiceRequest
			{

			public:
				CreateServiceMeshRequest();
				~CreateServiceMeshRequest();

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
				bool getStrictMTLS()const;
				void setStrictMTLS(bool strictMTLS);
				std::string getOPALogLevel()const;
				void setOPALogLevel(const std::string& oPALogLevel);
				std::string getExcludeIPRanges()const;
				void setExcludeIPRanges(const std::string& excludeIPRanges);
				std::string getIstioVersion()const;
				void setIstioVersion(const std::string& istioVersion);
				bool getTracing()const;
				void setTracing(bool tracing);
				std::string getIncludeIPRanges()const;
				void setIncludeIPRanges(const std::string& includeIPRanges);
				std::string getExcludeInboundPorts()const;
				void setExcludeInboundPorts(const std::string& excludeInboundPorts);
				std::string getOPALimitMemory()const;
				void setOPALimitMemory(const std::string& oPALimitMemory);
				std::string getVSwitches()const;
				void setVSwitches(const std::string& vSwitches);
				bool getCADisableSecretAutoGeneration()const;
				void setCADisableSecretAutoGeneration(bool cADisableSecretAutoGeneration);
				std::string getCAListenedNamespaces()const;
				void setCAListenedNamespaces(const std::string& cAListenedNamespaces);
				std::string getProxyLimitCPU()const;
				void setProxyLimitCPU(const std::string& proxyLimitCPU);
				std::string getProxyRequestMemory()const;
				void setProxyRequestMemory(const std::string& proxyRequestMemory);
				std::string getName()const;
				void setName(const std::string& name);
				bool getTelemetry()const;
				void setTelemetry(bool telemetry);
				std::string getOPARequestCPU()const;
				void setOPARequestCPU(const std::string& oPARequestCPU);
				std::string getOPARequestMemory()const;
				void setOPARequestMemory(const std::string& oPARequestMemory);
				bool getEnableAudit()const;
				void setEnableAudit(bool enableAudit);
				std::string getClusterDomain()const;
				void setClusterDomain(const std::string& clusterDomain);
				std::string getRegionId()const;
				void setRegionId(const std::string& regionId);
				bool getLocalityLoadBalancing()const;
				void setLocalityLoadBalancing(bool localityLoadBalancing);
				bool getApiServerPublicEip()const;
				void setApiServerPublicEip(bool apiServerPublicEip);
				float getTraceSampling()const;
				void setTraceSampling(float traceSampling);
				std::string getAppNamespaces()const;
				void setAppNamespaces(const std::string& appNamespaces);
				bool getPilotPublicEip()const;
				void setPilotPublicEip(bool pilotPublicEip);
				std::string getAuditProject()const;
				void setAuditProject(const std::string& auditProject);
				std::string getOutboundTrafficPolicy()const;
				void setOutboundTrafficPolicy(const std::string& outboundTrafficPolicy);
				std::string getVpcId()const;
				void setVpcId(const std::string& vpcId);
				std::string getExcludeOutboundPorts()const;
				void setExcludeOutboundPorts(const std::string& excludeOutboundPorts);

            private:
				std::string proxyRequestCPU_;
				std::string oPALimitCPU_;
				bool openAgentPolicy_;
				bool opaEnabled_;
				std::string proxyLimitMemory_;
				bool strictMTLS_;
				std::string oPALogLevel_;
				std::string excludeIPRanges_;
				std::string istioVersion_;
				bool tracing_;
				std::string includeIPRanges_;
				std::string excludeInboundPorts_;
				std::string oPALimitMemory_;
				std::string vSwitches_;
				bool cADisableSecretAutoGeneration_;
				std::string cAListenedNamespaces_;
				std::string proxyLimitCPU_;
				std::string proxyRequestMemory_;
				std::string name_;
				bool telemetry_;
				std::string oPARequestCPU_;
				std::string oPARequestMemory_;
				bool enableAudit_;
				std::string clusterDomain_;
				std::string regionId_;
				bool localityLoadBalancing_;
				bool apiServerPublicEip_;
				float traceSampling_;
				std::string appNamespaces_;
				bool pilotPublicEip_;
				std::string auditProject_;
				std::string outboundTrafficPolicy_;
				std::string vpcId_;
				std::string excludeOutboundPorts_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_SERVICEMESH_MODEL_CREATESERVICEMESHREQUEST_H_