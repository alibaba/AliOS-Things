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

#include <alibabacloud/iot/model/QuerySummarySceneRuleLogResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

QuerySummarySceneRuleLogResult::QuerySummarySceneRuleLogResult() :
	ServiceResult()
{}

QuerySummarySceneRuleLogResult::QuerySummarySceneRuleLogResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

QuerySummarySceneRuleLogResult::~QuerySummarySceneRuleLogResult()
{}

void QuerySummarySceneRuleLogResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["Total"].isNull())
		data_.total = std::stoi(dataNode["Total"].asString());
	if(!dataNode["PageSize"].isNull())
		data_.pageSize = std::stoi(dataNode["PageSize"].asString());
	if(!dataNode["CurrentPage"].isNull())
		data_.currentPage = std::stoi(dataNode["CurrentPage"].asString());
	auto allLogListNode = dataNode["LogList"]["LogInfo"];
	for (auto dataNodeLogListLogInfo : allLogListNode)
	{
		Data::LogInfo logInfoObject;
		if(!dataNodeLogListLogInfo["LogTime"].isNull())
			logInfoObject.logTime = std::stoi(dataNodeLogListLogInfo["LogTime"].asString());
		if(!dataNodeLogListLogInfo["Result"].isNull())
			logInfoObject.result = dataNodeLogListLogInfo["Result"].asString();
		if(!dataNodeLogListLogInfo["TraceId"].isNull())
			logInfoObject.traceId = dataNodeLogListLogInfo["TraceId"].asString();
		data_.logList.push_back(logInfoObject);
	}
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();

}

QuerySummarySceneRuleLogResult::Data QuerySummarySceneRuleLogResult::getData()const
{
	return data_;
}

std::string QuerySummarySceneRuleLogResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string QuerySummarySceneRuleLogResult::getCode()const
{
	return code_;
}

bool QuerySummarySceneRuleLogResult::getSuccess()const
{
	return success_;
}

