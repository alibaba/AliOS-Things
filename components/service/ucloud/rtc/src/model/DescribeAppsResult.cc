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

#include <alibabacloud/rtc/model/DescribeAppsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Rtc;
using namespace AlibabaCloud::Rtc::Model;

DescribeAppsResult::DescribeAppsResult() :
	ServiceResult()
{}

DescribeAppsResult::DescribeAppsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeAppsResult::~DescribeAppsResult()
{}

void DescribeAppsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allAppListNode = value["AppList"]["App"];
	for (auto valueAppListApp : allAppListNode)
	{
		App appListObject;
		if(!valueAppListApp["Status"].isNull())
			appListObject.status = std::stoi(valueAppListApp["Status"].asString());
		if(!valueAppListApp["AppId"].isNull())
			appListObject.appId = valueAppListApp["AppId"].asString();
		if(!valueAppListApp["AppName"].isNull())
			appListObject.appName = valueAppListApp["AppName"].asString();
		if(!valueAppListApp["CreateTime"].isNull())
			appListObject.createTime = valueAppListApp["CreateTime"].asString();
		if(!valueAppListApp["AppType"].isNull())
			appListObject.appType = valueAppListApp["AppType"].asString();
		if(!valueAppListApp["BillType"].isNull())
			appListObject.billType = valueAppListApp["BillType"].asString();
		auto allServiceAreas = value["ServiceAreas"]["ServiceArea"];
		for (auto value : allServiceAreas)
			appListObject.serviceAreas.push_back(value.asString());
		appList_.push_back(appListObject);
	}
	if(!value["TotalNum"].isNull())
		totalNum_ = std::stoi(value["TotalNum"].asString());
	if(!value["TotalPage"].isNull())
		totalPage_ = std::stoi(value["TotalPage"].asString());

}

int DescribeAppsResult::getTotalNum()const
{
	return totalNum_;
}

int DescribeAppsResult::getTotalPage()const
{
	return totalPage_;
}

std::vector<DescribeAppsResult::App> DescribeAppsResult::getAppList()const
{
	return appList_;
}

