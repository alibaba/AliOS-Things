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

#include <alibabacloud/servicemesh/model/GetServiceMeshSlbResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Servicemesh;
using namespace AlibabaCloud::Servicemesh::Model;

GetServiceMeshSlbResult::GetServiceMeshSlbResult() :
	ServiceResult()
{}

GetServiceMeshSlbResult::GetServiceMeshSlbResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetServiceMeshSlbResult::~GetServiceMeshSlbResult()
{}

void GetServiceMeshSlbResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allDataNode = value["Data"]["SLBInfo"];
	for (auto valueDataSLBInfo : allDataNode)
	{
		SLBInfo dataObject;
		if(!valueDataSLBInfo["LoadBalancerId"].isNull())
			dataObject.loadBalancerId = valueDataSLBInfo["LoadBalancerId"].asString();
		if(!valueDataSLBInfo["Status"].isNull())
			dataObject.status = valueDataSLBInfo["Status"].asString();
		if(!valueDataSLBInfo["ServerHealthStatus"].isNull())
			dataObject.serverHealthStatus = valueDataSLBInfo["ServerHealthStatus"].asString();
		data_.push_back(dataObject);
	}

}

std::vector<GetServiceMeshSlbResult::SLBInfo> GetServiceMeshSlbResult::getData()const
{
	return data_;
}

