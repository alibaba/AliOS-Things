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

#include <alibabacloud/servicemesh/model/DescribeServiceMeshesResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Servicemesh;
using namespace AlibabaCloud::Servicemesh::Model;

DescribeServiceMeshesResult::DescribeServiceMeshesResult() :
	ServiceResult()
{}

DescribeServiceMeshesResult::DescribeServiceMeshesResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeServiceMeshesResult::~DescribeServiceMeshesResult()
{}

void DescribeServiceMeshesResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allServiceMeshesNode = value["ServiceMeshes"]["ServiceMesh"];
	for (auto valueServiceMeshesServiceMesh : allServiceMeshesNode)
	{
		ServiceMesh serviceMeshesObject;
		auto endpointsNode = value["Endpoints"];
		if(!endpointsNode["IntranetApiServerEndpoint"].isNull())
			serviceMeshesObject.endpoints.intranetApiServerEndpoint = endpointsNode["IntranetApiServerEndpoint"].asString();
		if(!endpointsNode["IntranetPilotEndpoint"].isNull())
			serviceMeshesObject.endpoints.intranetPilotEndpoint = endpointsNode["IntranetPilotEndpoint"].asString();
		if(!endpointsNode["PublicApiServerEndpoint"].isNull())
			serviceMeshesObject.endpoints.publicApiServerEndpoint = endpointsNode["PublicApiServerEndpoint"].asString();
		if(!endpointsNode["PublicPilotEndpoint"].isNull())
			serviceMeshesObject.endpoints.publicPilotEndpoint = endpointsNode["PublicPilotEndpoint"].asString();
		if(!endpointsNode["ReverseTunnelEndpoint"].isNull())
			serviceMeshesObject.endpoints.reverseTunnelEndpoint = endpointsNode["ReverseTunnelEndpoint"].asString();
		auto serviceMeshInfoNode = value["ServiceMeshInfo"];
		if(!serviceMeshInfoNode["CreationTime"].isNull())
			serviceMeshesObject.serviceMeshInfo.creationTime = serviceMeshInfoNode["CreationTime"].asString();
		if(!serviceMeshInfoNode["ErrorMessage"].isNull())
			serviceMeshesObject.serviceMeshInfo.errorMessage = serviceMeshInfoNode["ErrorMessage"].asString();
		if(!serviceMeshInfoNode["Name"].isNull())
			serviceMeshesObject.serviceMeshInfo.name = serviceMeshInfoNode["Name"].asString();
		if(!serviceMeshInfoNode["RegionId"].isNull())
			serviceMeshesObject.serviceMeshInfo.regionId = serviceMeshInfoNode["RegionId"].asString();
		if(!serviceMeshInfoNode["ServiceMeshId"].isNull())
			serviceMeshesObject.serviceMeshInfo.serviceMeshId = serviceMeshInfoNode["ServiceMeshId"].asString();
		if(!serviceMeshInfoNode["State"].isNull())
			serviceMeshesObject.serviceMeshInfo.state = serviceMeshInfoNode["State"].asString();
		if(!serviceMeshInfoNode["UpdateTime"].isNull())
			serviceMeshesObject.serviceMeshInfo.updateTime = serviceMeshInfoNode["UpdateTime"].asString();
		if(!serviceMeshInfoNode["Version"].isNull())
			serviceMeshesObject.serviceMeshInfo.version = serviceMeshInfoNode["Version"].asString();
		auto specNode = value["Spec"];
		auto loadBalancerNode = specNode["LoadBalancer"];
		if(!loadBalancerNode["ApiServerLoadbalancerId"].isNull())
			serviceMeshesObject.spec.loadBalancer.apiServerLoadbalancerId = loadBalancerNode["ApiServerLoadbalancerId"].asString();
		if(!loadBalancerNode["ApiServerPublicEip"].isNull())
			serviceMeshesObject.spec.loadBalancer.apiServerPublicEip = loadBalancerNode["ApiServerPublicEip"].asString() == "true";
		if(!loadBalancerNode["PilotPublicEip"].isNull())
			serviceMeshesObject.spec.loadBalancer.pilotPublicEip = loadBalancerNode["PilotPublicEip"].asString() == "true";
		if(!loadBalancerNode["PilotPublicLoadbalancerId"].isNull())
			serviceMeshesObject.spec.loadBalancer.pilotPublicLoadbalancerId = loadBalancerNode["PilotPublicLoadbalancerId"].asString();
		auto meshConfigNode = specNode["MeshConfig"];
		if(!meshConfigNode["Mtls"].isNull())
			serviceMeshesObject.spec.meshConfig.mtls = meshConfigNode["Mtls"].asString() == "true";
		if(!meshConfigNode["OutboundTrafficPolicy"].isNull())
			serviceMeshesObject.spec.meshConfig.outboundTrafficPolicy = meshConfigNode["OutboundTrafficPolicy"].asString();
		if(!meshConfigNode["StrictMtls"].isNull())
			serviceMeshesObject.spec.meshConfig.strictMtls = meshConfigNode["StrictMtls"].asString() == "true";
		if(!meshConfigNode["Tracing"].isNull())
			serviceMeshesObject.spec.meshConfig.tracing = meshConfigNode["Tracing"].asString() == "true";
		if(!meshConfigNode["Telemetry"].isNull())
			serviceMeshesObject.spec.meshConfig.telemetry = meshConfigNode["Telemetry"].asString() == "true";
		auto pilotNode = meshConfigNode["Pilot"];
		if(!pilotNode["TraceSampling"].isNull())
			serviceMeshesObject.spec.meshConfig.pilot.traceSampling = std::stof(pilotNode["TraceSampling"].asString());
		if(!pilotNode["Http10Enabled"].isNull())
			serviceMeshesObject.spec.meshConfig.pilot.http10Enabled = pilotNode["Http10Enabled"].asString() == "true";
		auto sidecarInjectorNode = meshConfigNode["SidecarInjector"];
		if(!sidecarInjectorNode["EnableNamespacesByDefault"].isNull())
			serviceMeshesObject.spec.meshConfig.sidecarInjector.enableNamespacesByDefault = sidecarInjectorNode["EnableNamespacesByDefault"].asString() == "true";
		if(!sidecarInjectorNode["AutoInjectionPolicyEnabled"].isNull())
			serviceMeshesObject.spec.meshConfig.sidecarInjector.autoInjectionPolicyEnabled = sidecarInjectorNode["AutoInjectionPolicyEnabled"].asString() == "true";
		auto initCNIConfigurationNode = sidecarInjectorNode["InitCNIConfiguration"];
		if(!initCNIConfigurationNode["Enabled"].isNull())
			serviceMeshesObject.spec.meshConfig.sidecarInjector.initCNIConfiguration.enabled = initCNIConfigurationNode["Enabled"].asString() == "true";
		if(!initCNIConfigurationNode["ExcludeNamespaces"].isNull())
			serviceMeshesObject.spec.meshConfig.sidecarInjector.initCNIConfiguration.excludeNamespaces = initCNIConfigurationNode["ExcludeNamespaces"].asString();
		auto networkNode = specNode["Network"];
		if(!networkNode["SecurityGroupId"].isNull())
			serviceMeshesObject.spec.network.securityGroupId = networkNode["SecurityGroupId"].asString();
		if(!networkNode["VpcId"].isNull())
			serviceMeshesObject.spec.network.vpcId = networkNode["VpcId"].asString();
			auto allVSwitches = networkNode["VSwitches"]["VSwitch"];
			for (auto value : allVSwitches)
				serviceMeshesObject.spec.network.vSwitches.push_back(value.asString());
		auto allClusters = value["Clusters"]["Cluster"];
		for (auto value : allClusters)
			serviceMeshesObject.clusters.push_back(value.asString());
		serviceMeshes_.push_back(serviceMeshesObject);
	}

}

std::vector<DescribeServiceMeshesResult::ServiceMesh> DescribeServiceMeshesResult::getServiceMeshes()const
{
	return serviceMeshes_;
}

