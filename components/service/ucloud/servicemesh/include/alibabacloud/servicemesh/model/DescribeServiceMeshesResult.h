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

#ifndef ALIBABACLOUD_SERVICEMESH_MODEL_DESCRIBESERVICEMESHESRESULT_H_
#define ALIBABACLOUD_SERVICEMESH_MODEL_DESCRIBESERVICEMESHESRESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/servicemesh/ServicemeshExport.h>

namespace AlibabaCloud
{
	namespace Servicemesh
	{
		namespace Model
		{
			class ALIBABACLOUD_SERVICEMESH_EXPORT DescribeServiceMeshesResult : public ServiceResult
			{
			public:
				struct ServiceMesh
				{
					struct Endpoints
					{
						std::string publicPilotEndpoint;
						std::string reverseTunnelEndpoint;
						std::string intranetApiServerEndpoint;
						std::string intranetPilotEndpoint;
						std::string publicApiServerEndpoint;
					};
					struct ServiceMeshInfo
					{
						std::string serviceMeshId;
						std::string version;
						std::string state;
						std::string creationTime;
						std::string updateTime;
						std::string regionId;
						std::string errorMessage;
						std::string name;
					};
					struct Spec
					{
						struct LoadBalancer
						{
							std::string apiServerLoadbalancerId;
							bool pilotPublicEip;
							std::string pilotPublicLoadbalancerId;
							bool apiServerPublicEip;
						};
						struct MeshConfig
						{
							struct Pilot
							{
								float traceSampling;
								bool http10Enabled;
							};
							struct SidecarInjector
							{
								struct InitCNIConfiguration
								{
									bool enabled;
									std::string excludeNamespaces;
								};
								bool autoInjectionPolicyEnabled;
								bool enableNamespacesByDefault;
								InitCNIConfiguration initCNIConfiguration;
							};
							Pilot pilot;
							SidecarInjector sidecarInjector;
							bool mtls;
							bool strictMtls;
							bool tracing;
							std::string outboundTrafficPolicy;
							bool telemetry;
						};
						struct Network
						{
							std::string vpcId;
							std::string securityGroupId;
							std::vector<std::string> vSwitches;
						};
						MeshConfig meshConfig;
						Network network;
						LoadBalancer loadBalancer;
					};
					Endpoints endpoints;
					std::vector<std::string> clusters;
					ServiceMeshInfo serviceMeshInfo;
					Spec spec;
				};


				DescribeServiceMeshesResult();
				explicit DescribeServiceMeshesResult(const std::string &payload);
				~DescribeServiceMeshesResult();
				std::vector<ServiceMesh> getServiceMeshes()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::vector<ServiceMesh> serviceMeshes_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_SERVICEMESH_MODEL_DESCRIBESERVICEMESHESRESULT_H_