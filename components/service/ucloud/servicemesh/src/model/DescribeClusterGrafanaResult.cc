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

#include <alibabacloud/servicemesh/model/DescribeClusterGrafanaResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Servicemesh;
using namespace AlibabaCloud::Servicemesh::Model;

DescribeClusterGrafanaResult::DescribeClusterGrafanaResult() :
	ServiceResult()
{}

DescribeClusterGrafanaResult::DescribeClusterGrafanaResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeClusterGrafanaResult::~DescribeClusterGrafanaResult()
{}

void DescribeClusterGrafanaResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allDashboardsNode = value["Dashboards"]["Dashboard"];
	for (auto valueDashboardsDashboard : allDashboardsNode)
	{
		Dashboard dashboardsObject;
		if(!valueDashboardsDashboard["Url"].isNull())
			dashboardsObject.url = valueDashboardsDashboard["Url"].asString();
		if(!valueDashboardsDashboard["Title"].isNull())
			dashboardsObject.title = valueDashboardsDashboard["Title"].asString();
		dashboards_.push_back(dashboardsObject);
	}

}

std::vector<DescribeClusterGrafanaResult::Dashboard> DescribeClusterGrafanaResult::getDashboards()const
{
	return dashboards_;
}

