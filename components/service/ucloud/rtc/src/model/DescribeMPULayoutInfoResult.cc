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

#include <alibabacloud/rtc/model/DescribeMPULayoutInfoResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Rtc;
using namespace AlibabaCloud::Rtc::Model;

DescribeMPULayoutInfoResult::DescribeMPULayoutInfoResult() :
	ServiceResult()
{}

DescribeMPULayoutInfoResult::DescribeMPULayoutInfoResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeMPULayoutInfoResult::~DescribeMPULayoutInfoResult()
{}

void DescribeMPULayoutInfoResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto layoutNode = value["Layout"];
	if(!layoutNode["LayoutId"].isNull())
		layout_.layoutId = std::stol(layoutNode["LayoutId"].asString());
	if(!layoutNode["Name"].isNull())
		layout_.name = layoutNode["Name"].asString();
	if(!layoutNode["AudioMixCount"].isNull())
		layout_.audioMixCount = std::stoi(layoutNode["AudioMixCount"].asString());
	auto allPanesNode = layoutNode["Panes"]["PanesItem"];
	for (auto layoutNodePanesPanesItem : allPanesNode)
	{
		Layout::PanesItem panesItemObject;
		if(!layoutNodePanesPanesItem["PaneId"].isNull())
			panesItemObject.paneId = std::stoi(layoutNodePanesPanesItem["PaneId"].asString());
		if(!layoutNodePanesPanesItem["MajorPane"].isNull())
			panesItemObject.majorPane = std::stoi(layoutNodePanesPanesItem["MajorPane"].asString());
		if(!layoutNodePanesPanesItem["X"].isNull())
			panesItemObject.x = std::stof(layoutNodePanesPanesItem["X"].asString());
		if(!layoutNodePanesPanesItem["Y"].isNull())
			panesItemObject.y = std::stof(layoutNodePanesPanesItem["Y"].asString());
		if(!layoutNodePanesPanesItem["Width"].isNull())
			panesItemObject.width = std::stof(layoutNodePanesPanesItem["Width"].asString());
		if(!layoutNodePanesPanesItem["Height"].isNull())
			panesItemObject.height = std::stof(layoutNodePanesPanesItem["Height"].asString());
		if(!layoutNodePanesPanesItem["ZOrder"].isNull())
			panesItemObject.zOrder = std::stoi(layoutNodePanesPanesItem["ZOrder"].asString());
		layout_.panes.push_back(panesItemObject);
	}

}

DescribeMPULayoutInfoResult::Layout DescribeMPULayoutInfoResult::getLayout()const
{
	return layout_;
}

