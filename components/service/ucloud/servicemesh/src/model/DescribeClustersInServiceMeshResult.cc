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

#include <alibabacloud/servicemesh/model/DescribeClustersInServiceMeshResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Servicemesh;
using namespace AlibabaCloud::Servicemesh::Model;

DescribeClustersInServiceMeshResult::DescribeClustersInServiceMeshResult() :
	ServiceResult()
{}

DescribeClustersInServiceMeshResult::DescribeClustersInServiceMeshResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeClustersInServiceMeshResult::~DescribeClustersInServiceMeshResult()
{}

void DescribeClustersInServiceMeshResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allClustersNode = value["Clusters"]["Cluster"];
	for (auto valueClustersCluster : allClustersNode)
	{
		Cluster clustersObject;
		if(!valueClustersCluster["ClusterId"].isNull())
			clustersObject.clusterId = valueClustersCluster["ClusterId"].asString();
		if(!valueClustersCluster["ClusterType"].isNull())
			clustersObject.clusterType = valueClustersCluster["ClusterType"].asString();
		if(!valueClustersCluster["CreationTime"].isNull())
			clustersObject.creationTime = valueClustersCluster["CreationTime"].asString();
		if(!valueClustersCluster["ErrorMessage"].isNull())
			clustersObject.errorMessage = valueClustersCluster["ErrorMessage"].asString();
		if(!valueClustersCluster["Name"].isNull())
			clustersObject.name = valueClustersCluster["Name"].asString();
		if(!valueClustersCluster["RegionId"].isNull())
			clustersObject.regionId = valueClustersCluster["RegionId"].asString();
		if(!valueClustersCluster["State"].isNull())
			clustersObject.state = valueClustersCluster["State"].asString();
		if(!valueClustersCluster["UpdateTime"].isNull())
			clustersObject.updateTime = valueClustersCluster["UpdateTime"].asString();
		if(!valueClustersCluster["Version"].isNull())
			clustersObject.version = valueClustersCluster["Version"].asString();
		if(!valueClustersCluster["VpcId"].isNull())
			clustersObject.vpcId = valueClustersCluster["VpcId"].asString();
		if(!valueClustersCluster["SgId"].isNull())
			clustersObject.sgId = valueClustersCluster["SgId"].asString();
		if(!valueClustersCluster["ClusterDomain"].isNull())
			clustersObject.clusterDomain = valueClustersCluster["ClusterDomain"].asString();
		clusters_.push_back(clustersObject);
	}

}

std::vector<DescribeClustersInServiceMeshResult::Cluster> DescribeClustersInServiceMeshResult::getClusters()const
{
	return clusters_;
}

