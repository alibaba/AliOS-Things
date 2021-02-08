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

#include <alibabacloud/live/model/DescribeLiveLazyPullStreamConfigResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeLiveLazyPullStreamConfigResult::DescribeLiveLazyPullStreamConfigResult() :
	ServiceResult()
{}

DescribeLiveLazyPullStreamConfigResult::DescribeLiveLazyPullStreamConfigResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeLiveLazyPullStreamConfigResult::~DescribeLiveLazyPullStreamConfigResult()
{}

void DescribeLiveLazyPullStreamConfigResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allLiveLazyPullConfigListNode = value["LiveLazyPullConfigList"]["LiveLazyPullConfig"];
	for (auto valueLiveLazyPullConfigListLiveLazyPullConfig : allLiveLazyPullConfigListNode)
	{
		LiveLazyPullConfig liveLazyPullConfigListObject;
		if(!valueLiveLazyPullConfigListLiveLazyPullConfig["DomainName"].isNull())
			liveLazyPullConfigListObject.domainName = valueLiveLazyPullConfigListLiveLazyPullConfig["DomainName"].asString();
		if(!valueLiveLazyPullConfigListLiveLazyPullConfig["AppName"].isNull())
			liveLazyPullConfigListObject.appName = valueLiveLazyPullConfigListLiveLazyPullConfig["AppName"].asString();
		if(!valueLiveLazyPullConfigListLiveLazyPullConfig["PullDomainName"].isNull())
			liveLazyPullConfigListObject.pullDomainName = valueLiveLazyPullConfigListLiveLazyPullConfig["PullDomainName"].asString();
		if(!valueLiveLazyPullConfigListLiveLazyPullConfig["PullAppName"].isNull())
			liveLazyPullConfigListObject.pullAppName = valueLiveLazyPullConfigListLiveLazyPullConfig["PullAppName"].asString();
		if(!valueLiveLazyPullConfigListLiveLazyPullConfig["PullProtocol"].isNull())
			liveLazyPullConfigListObject.pullProtocol = valueLiveLazyPullConfigListLiveLazyPullConfig["PullProtocol"].asString();
		if(!valueLiveLazyPullConfigListLiveLazyPullConfig["PullAuthType"].isNull())
			liveLazyPullConfigListObject.pullAuthType = valueLiveLazyPullConfigListLiveLazyPullConfig["PullAuthType"].asString();
		if(!valueLiveLazyPullConfigListLiveLazyPullConfig["PullAuthKey"].isNull())
			liveLazyPullConfigListObject.pullAuthKey = valueLiveLazyPullConfigListLiveLazyPullConfig["PullAuthKey"].asString();
		if(!valueLiveLazyPullConfigListLiveLazyPullConfig["PullArgs"].isNull())
			liveLazyPullConfigListObject.pullArgs = valueLiveLazyPullConfigListLiveLazyPullConfig["PullArgs"].asString();
		liveLazyPullConfigList_.push_back(liveLazyPullConfigListObject);
	}

}

std::vector<DescribeLiveLazyPullStreamConfigResult::LiveLazyPullConfig> DescribeLiveLazyPullStreamConfigResult::getLiveLazyPullConfigList()const
{
	return liveLazyPullConfigList_;
}

