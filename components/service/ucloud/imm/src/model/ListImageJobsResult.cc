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

#include <alibabacloud/imm/model/ListImageJobsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imm;
using namespace AlibabaCloud::Imm::Model;

ListImageJobsResult::ListImageJobsResult() :
	ServiceResult()
{}

ListImageJobsResult::ListImageJobsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ListImageJobsResult::~ListImageJobsResult()
{}

void ListImageJobsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allJobsNode = value["Jobs"]["JobsItem"];
	for (auto valueJobsJobsItem : allJobsNode)
	{
		JobsItem jobsObject;
		if(!valueJobsJobsItem["Status"].isNull())
			jobsObject.status = valueJobsJobsItem["Status"].asString();
		if(!valueJobsJobsItem["JobId"].isNull())
			jobsObject.jobId = valueJobsJobsItem["JobId"].asString();
		if(!valueJobsJobsItem["JobType"].isNull())
			jobsObject.jobType = valueJobsJobsItem["JobType"].asString();
		if(!valueJobsJobsItem["Parameters"].isNull())
			jobsObject.parameters = valueJobsJobsItem["Parameters"].asString();
		if(!valueJobsJobsItem["Result"].isNull())
			jobsObject.result = valueJobsJobsItem["Result"].asString();
		if(!valueJobsJobsItem["StartTime"].isNull())
			jobsObject.startTime = valueJobsJobsItem["StartTime"].asString();
		if(!valueJobsJobsItem["EndTime"].isNull())
			jobsObject.endTime = valueJobsJobsItem["EndTime"].asString();
		if(!valueJobsJobsItem["ErrorMessage"].isNull())
			jobsObject.errorMessage = valueJobsJobsItem["ErrorMessage"].asString();
		if(!valueJobsJobsItem["NotifyEndpoint"].isNull())
			jobsObject.notifyEndpoint = valueJobsJobsItem["NotifyEndpoint"].asString();
		if(!valueJobsJobsItem["NotifyTopicName"].isNull())
			jobsObject.notifyTopicName = valueJobsJobsItem["NotifyTopicName"].asString();
		if(!valueJobsJobsItem["Progress"].isNull())
			jobsObject.progress = std::stoi(valueJobsJobsItem["Progress"].asString());
		jobs_.push_back(jobsObject);
	}
	if(!value["NextMarker"].isNull())
		nextMarker_ = value["NextMarker"].asString();

}

std::vector<ListImageJobsResult::JobsItem> ListImageJobsResult::getJobs()const
{
	return jobs_;
}

std::string ListImageJobsResult::getNextMarker()const
{
	return nextMarker_;
}

