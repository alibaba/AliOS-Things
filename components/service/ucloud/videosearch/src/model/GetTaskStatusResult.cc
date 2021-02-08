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

#include <alibabacloud/videosearch/model/GetTaskStatusResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Videosearch;
using namespace AlibabaCloud::Videosearch::Model;

GetTaskStatusResult::GetTaskStatusResult() :
	ServiceResult()
{}

GetTaskStatusResult::GetTaskStatusResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetTaskStatusResult::~GetTaskStatusResult()
{}

void GetTaskStatusResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto taskInfoNode = value["TaskInfo"];
	if(!taskInfoNode["AnalysisUseTime"].isNull())
		taskInfo_.analysisUseTime = std::stol(taskInfoNode["AnalysisUseTime"].asString());
	if(!taskInfoNode["Duration"].isNull())
		taskInfo_.duration = std::stof(taskInfoNode["Duration"].asString());
	if(!taskInfoNode["ProcessResultOss"].isNull())
		taskInfo_.processResultOss = taskInfoNode["ProcessResultOss"].asString();
	if(!taskInfoNode["Resolution"].isNull())
		taskInfo_.resolution = taskInfoNode["Resolution"].asString();
	if(!taskInfoNode["Status"].isNull())
		taskInfo_.status = std::stoi(taskInfoNode["Status"].asString());
	if(!taskInfoNode["SubmitTime"].isNull())
		taskInfo_.submitTime = std::stol(taskInfoNode["SubmitTime"].asString());
	if(!taskInfoNode["FinishTime"].isNull())
		taskInfo_.finishTime = std::stol(taskInfoNode["FinishTime"].asString());
	if(!taskInfoNode["TaskId"].isNull())
		taskInfo_.taskId = std::stol(taskInfoNode["TaskId"].asString());
	if(!taskInfoNode["ErrorInfo"].isNull())
		taskInfo_.errorInfo = taskInfoNode["ErrorInfo"].asString();
	if(!taskInfoNode["StorageInfo"].isNull())
		taskInfo_.storageInfo = std::stoi(taskInfoNode["StorageInfo"].asString());
	if(!taskInfoNode["Description"].isNull())
		taskInfo_.description = taskInfoNode["Description"].asString();
	if(!taskInfoNode["VideoId"].isNull())
		taskInfo_.videoId = taskInfoNode["VideoId"].asString();
	if(!taskInfoNode["VideoTags"].isNull())
		taskInfo_.videoTags = taskInfoNode["VideoTags"].asString();
	if(!taskInfoNode["VideoUrl"].isNull())
		taskInfo_.videoUrl = taskInfoNode["VideoUrl"].asString();
	if(!taskInfoNode["QueryTags"].isNull())
		taskInfo_.queryTags = taskInfoNode["QueryTags"].asString();
	if(!value["Data"].isNull())
		data_ = std::stoi(value["Data"].asString());

}

GetTaskStatusResult::TaskInfo GetTaskStatusResult::getTaskInfo()const
{
	return taskInfo_;
}

int GetTaskStatusResult::getData()const
{
	return data_;
}

