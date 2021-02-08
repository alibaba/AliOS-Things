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

#include <alibabacloud/servicemesh/model/DescribeServiceMeshDetailResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Servicemesh;
using namespace AlibabaCloud::Servicemesh::Model;

DescribeServiceMeshDetailResult::DescribeServiceMeshDetailResult() :
	ServiceResult()
{}

DescribeServiceMeshDetailResult::DescribeServiceMeshDetailResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeServiceMeshDetailResult::~DescribeServiceMeshDetailResult()
{}

void DescribeServiceMeshDetailResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto serviceMeshNode = value["ServiceMesh"];
	auto endpointsNode = serviceMeshNode["Endpoints"];
	if(!endpointsNode["IntranetApiServerEndpoint"].isNull())
		serviceMesh_.endpoints.intranetApiServerEndpoint = endpointsNode["IntranetApiServerEndpoint"].asString();
	if(!endpointsNode["IntranetPilotEndpoint"].isNull())
		serviceMesh_.endpoints.intranetPilotEndpoint = endpointsNode["IntranetPilotEndpoint"].asString();
	if(!endpointsNode["PublicApiServerEndpoint"].isNull())
		serviceMesh_.endpoints.publicApiServerEndpoint = endpointsNode["PublicApiServerEndpoint"].asString();
	if(!endpointsNode["PublicPilotEndpoint"].isNull())
		serviceMesh_.endpoints.publicPilotEndpoint = endpointsNode["PublicPilotEndpoint"].asString();
	auto serviceMeshInfoNode = serviceMeshNode["ServiceMeshInfo"];
	if(!serviceMeshInfoNode["CreationTime"].isNull())
		serviceMesh_.serviceMeshInfo.creationTime = serviceMeshInfoNode["CreationTime"].asString();
	if(!serviceMeshInfoNode["ErrorMessage"].isNull())
		serviceMesh_.serviceMeshInfo.errorMessage = serviceMeshInfoNode["ErrorMessage"].asString();
	if(!serviceMeshInfoNode["Name"].isNull())
		serviceMesh_.serviceMeshInfo.name = serviceMeshInfoNode["Name"].asString();
	if(!serviceMeshInfoNode["RegionId"].isNull())
		serviceMesh_.serviceMeshInfo.regionId = serviceMeshInfoNode["RegionId"].asString();
	if(!serviceMeshInfoNode["ServiceMeshId"].isNull())
		serviceMesh_.serviceMeshInfo.serviceMeshId = serviceMeshInfoNode["ServiceMeshId"].asString();
	if(!serviceMeshInfoNode["State"].isNull())
		serviceMesh_.serviceMeshInfo.state = serviceMeshInfoNode["State"].asString();
	if(!serviceMeshInfoNode["UpdateTime"].isNull())
		serviceMesh_.serviceMeshInfo.updateTime = serviceMeshInfoNode["UpdateTime"].asString();
	if(!serviceMeshInfoNode["Version"].isNull())
		serviceMesh_.serviceMeshInfo.version = serviceMeshInfoNode["Version"].asString();
	auto specNode = serviceMeshNode["Spec"];
	auto loadBalancerNode = specNode["LoadBalancer"];
	if(!loadBalancerNode["ApiServerLoadbalancerId"].isNull())
		serviceMesh_.spec.loadBalancer.apiServerLoadbalancerId = loadBalancerNode["ApiServerLoadbalancerId"].asString();
	if(!loadBalancerNode["ApiServerPublicEip"].isNull())
		serviceMesh_.spec.loadBalancer.apiServerPublicEip = loadBalancerNode["ApiServerPublicEip"].asString() == "true";
	if(!loadBalancerNode["PilotPublicEip"].isNull())
		serviceMesh_.spec.loadBalancer.pilotPublicEip = loadBalancerNode["PilotPublicEip"].asString() == "true";
	if(!loadBalancerNode["PilotPublicLoadbalancerId"].isNull())
		serviceMesh_.spec.loadBalancer.pilotPublicLoadbalancerId = loadBalancerNode["PilotPublicLoadbalancerId"].asString();
	auto meshConfigNode = specNode["MeshConfig"];
	if(!meshConfigNode["EnableLocalityLB"].isNull())
		serviceMesh_.spec.meshConfig.enableLocalityLB = meshConfigNode["EnableLocalityLB"].asString() == "true";
	if(!meshConfigNode["Telemetry"].isNull())
		serviceMesh_.spec.meshConfig.telemetry = meshConfigNode["Telemetry"].asString() == "true";
	if(!meshConfigNode["Tracing"].isNull())
		serviceMesh_.spec.meshConfig.tracing = meshConfigNode["Tracing"].asString() == "true";
	if(!meshConfigNode["CustomizedZipkin"].isNull())
		serviceMesh_.spec.meshConfig.customizedZipkin = meshConfigNode["CustomizedZipkin"].asString() == "true";
	if(!meshConfigNode["OutboundTrafficPolicy"].isNull())
		serviceMesh_.spec.meshConfig.outboundTrafficPolicy = meshConfigNode["OutboundTrafficPolicy"].asString();
	if(!meshConfigNode["IncludeIPRanges"].isNull())
		serviceMesh_.spec.meshConfig.includeIPRanges = meshConfigNode["IncludeIPRanges"].asString();
	auto pilotNode = meshConfigNode["Pilot"];
	if(!pilotNode["TraceSampling"].isNull())
		serviceMesh_.spec.meshConfig.pilot.traceSampling = std::stof(pilotNode["TraceSampling"].asString());
	if(!pilotNode["Http10Enabled"].isNull())
		serviceMesh_.spec.meshConfig.pilot.http10Enabled = pilotNode["Http10Enabled"].asString() == "true";
	auto oPANode = meshConfigNode["OPA"];
	if(!oPANode["Enabled"].isNull())
		serviceMesh_.spec.meshConfig.oPA.enabled = oPANode["Enabled"].asString() == "true";
	if(!oPANode["LogLevel"].isNull())
		serviceMesh_.spec.meshConfig.oPA.logLevel = oPANode["LogLevel"].asString();
	if(!oPANode["RequestCPU"].isNull())
		serviceMesh_.spec.meshConfig.oPA.requestCPU = oPANode["RequestCPU"].asString();
	if(!oPANode["RequestMemory"].isNull())
		serviceMesh_.spec.meshConfig.oPA.requestMemory = oPANode["RequestMemory"].asString();
	if(!oPANode["LimitCPU"].isNull())
		serviceMesh_.spec.meshConfig.oPA.limitCPU = oPANode["LimitCPU"].asString();
	if(!oPANode["LimitMemory"].isNull())
		serviceMesh_.spec.meshConfig.oPA.limitMemory = oPANode["LimitMemory"].asString();
	auto auditNode = meshConfigNode["Audit"];
	if(!auditNode["Enabled"].isNull())
		serviceMesh_.spec.meshConfig.audit.enabled = auditNode["Enabled"].asString() == "true";
	if(!auditNode["Project"].isNull())
		serviceMesh_.spec.meshConfig.audit.project = auditNode["Project"].asString();
	auto proxyNode = meshConfigNode["Proxy"];
	if(!proxyNode["ClusterDomain"].isNull())
		serviceMesh_.spec.meshConfig.proxy.clusterDomain = proxyNode["ClusterDomain"].asString();
	if(!proxyNode["RequestCPU"].isNull())
		serviceMesh_.spec.meshConfig.proxy.requestCPU = proxyNode["RequestCPU"].asString();
	if(!proxyNode["RequestMemory"].isNull())
		serviceMesh_.spec.meshConfig.proxy.requestMemory = proxyNode["RequestMemory"].asString();
	if(!proxyNode["LimitCPU"].isNull())
		serviceMesh_.spec.meshConfig.proxy.limitCPU = proxyNode["LimitCPU"].asString();
	if(!proxyNode["LimitMemory"].isNull())
		serviceMesh_.spec.meshConfig.proxy.limitMemory = proxyNode["LimitMemory"].asString();
	auto sidecarInjectorNode = meshConfigNode["SidecarInjector"];
	if(!sidecarInjectorNode["EnableNamespacesByDefault"].isNull())
		serviceMesh_.spec.meshConfig.sidecarInjector.enableNamespacesByDefault = sidecarInjectorNode["EnableNamespacesByDefault"].asString() == "true";
	if(!sidecarInjectorNode["AutoInjectionPolicyEnabled"].isNull())
		serviceMesh_.spec.meshConfig.sidecarInjector.autoInjectionPolicyEnabled = sidecarInjectorNode["AutoInjectionPolicyEnabled"].asString() == "true";
	if(!sidecarInjectorNode["RequestCPU"].isNull())
		serviceMesh_.spec.meshConfig.sidecarInjector.requestCPU = sidecarInjectorNode["RequestCPU"].asString();
	if(!sidecarInjectorNode["RequestMemory"].isNull())
		serviceMesh_.spec.meshConfig.sidecarInjector.requestMemory = sidecarInjectorNode["RequestMemory"].asString();
	if(!sidecarInjectorNode["LimitCPU"].isNull())
		serviceMesh_.spec.meshConfig.sidecarInjector.limitCPU = sidecarInjectorNode["LimitCPU"].asString();
	if(!sidecarInjectorNode["LimitMemory"].isNull())
		serviceMesh_.spec.meshConfig.sidecarInjector.limitMemory = sidecarInjectorNode["LimitMemory"].asString();
	if(!sidecarInjectorNode["SidecarInjectorWebhookAsYaml"].isNull())
		serviceMesh_.spec.meshConfig.sidecarInjector.sidecarInjectorWebhookAsYaml = sidecarInjectorNode["SidecarInjectorWebhookAsYaml"].asString();
	auto initCNIConfigurationNode = sidecarInjectorNode["InitCNIConfiguration"];
	if(!initCNIConfigurationNode["Enabled"].isNull())
		serviceMesh_.spec.meshConfig.sidecarInjector.initCNIConfiguration.enabled = initCNIConfigurationNode["Enabled"].asString() == "true";
	if(!initCNIConfigurationNode["ExcludeNamespaces"].isNull())
		serviceMesh_.spec.meshConfig.sidecarInjector.initCNIConfiguration.excludeNamespaces = initCNIConfigurationNode["ExcludeNamespaces"].asString();
	auto networkNode = specNode["Network"];
	if(!networkNode["SecurityGroupId"].isNull())
		serviceMesh_.spec.network.securityGroupId = networkNode["SecurityGroupId"].asString();
	if(!networkNode["VpcId"].isNull())
		serviceMesh_.spec.network.vpcId = networkNode["VpcId"].asString();
		auto allVSwitches = networkNode["VSwitches"]["VSwitch"];
		for (auto value : allVSwitches)
			serviceMesh_.spec.network.vSwitches.push_back(value.asString());
		auto allClusters = serviceMeshNode["Clusters"]["Cluster"];
		for (auto value : allClusters)
			serviceMesh_.clusters.push_back(value.asString());

}

DescribeServiceMeshDetailResult::ServiceMesh DescribeServiceMeshDetailResult::getServiceMesh()const
{
	return serviceMesh_;
}

