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

#include <alibabacloud/servicemesh/model/DescribeGuestClusterAccessLogDashboardsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Servicemesh;
using namespace AlibabaCloud::Servicemesh::Model;

DescribeGuestClusterAccessLogDashboardsResult::DescribeGuestClusterAccessLogDashboardsResult() :
	ServiceResult()
{}

DescribeGuestClusterAccessLogDashboardsResult::DescribeGuestClusterAccessLogDashboardsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeGuestClusterAccessLogDashboardsResult::~DescribeGuestClusterAccessLogDashboardsResult()
{}

void DescribeGuestClusterAccessLogDashboardsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allDashboardsNode = value["Dashboards"]["DashboardsItem"];
	for (auto valueDashboardsDashboardsItem : allDashboardsNode)
	{
		DashboardsItem dashboardsObject;
		if(!valueDashboardsDashboardsItem["Title"].isNull())
			dashboardsObject.title = valueDashboardsDashboardsItem["Title"].asString();
		if(!valueDashboardsDashboardsItem["Url"].isNull())
			dashboardsObject.url = valueDashboardsDashboardsItem["Url"].asString();
		dashboards_.push_back(dashboardsObject);
	}
	if(!value["K8sClusterId"].isNull())
		k8sClusterId_ = value["K8sClusterId"].asString();

}

std::vector<DescribeGuestClusterAccessLogDashboardsResult::DashboardsItem> DescribeGuestClusterAccessLogDashboardsResult::getDashboards()const
{
	return dashboards_;
}

std::string DescribeGuestClusterAccessLogDashboardsResult::getK8sClusterId()const
{
	return k8sClusterId_;
}

