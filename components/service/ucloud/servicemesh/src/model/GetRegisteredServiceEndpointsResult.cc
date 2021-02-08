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

#include <alibabacloud/servicemesh/model/GetRegisteredServiceEndpointsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Servicemesh;
using namespace AlibabaCloud::Servicemesh::Model;

GetRegisteredServiceEndpointsResult::GetRegisteredServiceEndpointsResult() :
	ServiceResult()
{}

GetRegisteredServiceEndpointsResult::GetRegisteredServiceEndpointsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetRegisteredServiceEndpointsResult::~GetRegisteredServiceEndpointsResult()
{}

void GetRegisteredServiceEndpointsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allServiceEndpointsNode = value["ServiceEndpoints"]["ServiceEndpoint"];
	for (auto valueServiceEndpointsServiceEndpoint : allServiceEndpointsNode)
	{
		ServiceEndpoint serviceEndpointsObject;
		if(!valueServiceEndpointsServiceEndpoint["Address"].isNull())
			serviceEndpointsObject.address = valueServiceEndpointsServiceEndpoint["Address"].asString();
		if(!valueServiceEndpointsServiceEndpoint["ClusterId"].isNull())
			serviceEndpointsObject.clusterId = valueServiceEndpointsServiceEndpoint["ClusterId"].asString();
		serviceEndpoints_.push_back(serviceEndpointsObject);
	}

}

std::vector<GetRegisteredServiceEndpointsResult::ServiceEndpoint> GetRegisteredServiceEndpointsResult::getServiceEndpoints()const
{
	return serviceEndpoints_;
}

