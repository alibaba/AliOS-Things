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

#include <alibabacloud/multimediaai/model/GetTaskResultResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Multimediaai;
using namespace AlibabaCloud::Multimediaai::Model;

GetTaskResultResult::GetTaskResultResult() :
	ServiceResult()
{}

GetTaskResultResult::GetTaskResultResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetTaskResultResult::~GetTaskResultResult()
{}

void GetTaskResultResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto resultNode = value["Result"];
	if(!resultNode["ApplicationId"].isNull())
		result_.applicationId = resultNode["ApplicationId"].asString();
	if(!resultNode["VideoName"].isNull())
		result_.videoName = resultNode["VideoName"].asString();
	if(!resultNode["ProcessResultUrl"].isNull())
		result_.processResultUrl = resultNode["ProcessResultUrl"].asString();
	if(!resultNode["AnalysisUseTime"].isNull())
		result_.analysisUseTime = std::stol(resultNode["AnalysisUseTime"].asString());
	if(!resultNode["VideoUrl"].isNull())
		result_.videoUrl = resultNode["VideoUrl"].asString();
	if(!resultNode["ErrorName"].isNull())
		result_.errorName = resultNode["ErrorName"].asString();
	if(!resultNode["ErrorReason"].isNull())
		result_.errorReason = resultNode["ErrorReason"].asString();
	if(!resultNode["ErrorCode"].isNull())
		result_.errorCode = resultNode["ErrorCode"].asString();
	if(!resultNode["ErrorMessage"].isNull())
		result_.errorMessage = resultNode["ErrorMessage"].asString();
	if(!value["Status"].isNull())
		status_ = std::stoi(value["Status"].asString());

}

int GetTaskResultResult::getStatus()const
{
	return status_;
}

GetTaskResultResult::Result GetTaskResultResult::getResult()const
{
	return result_;
}

