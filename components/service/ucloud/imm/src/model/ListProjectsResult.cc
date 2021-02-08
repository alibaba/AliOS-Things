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

#include <alibabacloud/imm/model/ListProjectsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imm;
using namespace AlibabaCloud::Imm::Model;

ListProjectsResult::ListProjectsResult() :
	ServiceResult()
{}

ListProjectsResult::ListProjectsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ListProjectsResult::~ListProjectsResult()
{}

void ListProjectsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allProjectsNode = value["Projects"]["ProjectsItem"];
	for (auto valueProjectsProjectsItem : allProjectsNode)
	{
		ProjectsItem projectsObject;
		if(!valueProjectsProjectsItem["Project"].isNull())
			projectsObject.project = valueProjectsProjectsItem["Project"].asString();
		if(!valueProjectsProjectsItem["Endpoint"].isNull())
			projectsObject.endpoint = valueProjectsProjectsItem["Endpoint"].asString();
		if(!valueProjectsProjectsItem["ServiceRole"].isNull())
			projectsObject.serviceRole = valueProjectsProjectsItem["ServiceRole"].asString();
		if(!valueProjectsProjectsItem["CreateTime"].isNull())
			projectsObject.createTime = valueProjectsProjectsItem["CreateTime"].asString();
		if(!valueProjectsProjectsItem["ModifyTime"].isNull())
			projectsObject.modifyTime = valueProjectsProjectsItem["ModifyTime"].asString();
		if(!valueProjectsProjectsItem["CU"].isNull())
			projectsObject.cU = std::stoi(valueProjectsProjectsItem["CU"].asString());
		if(!valueProjectsProjectsItem["Type"].isNull())
			projectsObject.type = valueProjectsProjectsItem["Type"].asString();
		if(!valueProjectsProjectsItem["BillingType"].isNull())
			projectsObject.billingType = valueProjectsProjectsItem["BillingType"].asString();
		if(!valueProjectsProjectsItem["RegionId"].isNull())
			projectsObject.regionId = valueProjectsProjectsItem["RegionId"].asString();
		projects_.push_back(projectsObject);
	}
	if(!value["NextMarker"].isNull())
		nextMarker_ = value["NextMarker"].asString();

}

std::vector<ListProjectsResult::ProjectsItem> ListProjectsResult::getProjects()const
{
	return projects_;
}

std::string ListProjectsResult::getNextMarker()const
{
	return nextMarker_;
}

