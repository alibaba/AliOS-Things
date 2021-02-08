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

#include <alibabacloud/iot/model/QueryDetailSceneRuleLogResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

QueryDetailSceneRuleLogResult::QueryDetailSceneRuleLogResult() :
	ServiceResult()
{}

QueryDetailSceneRuleLogResult::QueryDetailSceneRuleLogResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

QueryDetailSceneRuleLogResult::~QueryDetailSceneRuleLogResult()
{}

void QueryDetailSceneRuleLogResult::parse(const std::string &payload)
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
		if(!dataNodeLogListLogInfo["Code"].isNull())
			logInfoObject.code = dataNodeLogListLogInfo["Code"].asString();
		if(!dataNodeLogListLogInfo["Message"].isNull())
			logInfoObject.message = dataNodeLogListLogInfo["Message"].asString();
		if(!dataNodeLogListLogInfo["PkDn"].isNull())
			logInfoObject.pkDn = dataNodeLogListLogInfo["PkDn"].asString();
		data_.logList.push_back(logInfoObject);
	}
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();

}

QueryDetailSceneRuleLogResult::Data QueryDetailSceneRuleLogResult::getData()const
{
	return data_;
}

std::string QueryDetailSceneRuleLogResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string QueryDetailSceneRuleLogResult::getCode()const
{
	return code_;
}

bool QueryDetailSceneRuleLogResult::getSuccess()const
{
	return success_;
}

