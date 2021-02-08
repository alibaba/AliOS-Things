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

#include <alibabacloud/cloudphoto/model/InactivatePhotosResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudPhoto;
using namespace AlibabaCloud::CloudPhoto::Model;

InactivatePhotosResult::InactivatePhotosResult() :
	ServiceResult()
{}

InactivatePhotosResult::InactivatePhotosResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

InactivatePhotosResult::~InactivatePhotosResult()
{}

void InactivatePhotosResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allResultsNode = value["Results"]["Result"];
	for (auto valueResultsResult : allResultsNode)
	{
		Result resultsObject;
		if(!valueResultsResult["Id"].isNull())
			resultsObject.id = std::stol(valueResultsResult["Id"].asString());
		if(!valueResultsResult["IdStr"].isNull())
			resultsObject.idStr = valueResultsResult["IdStr"].asString();
		if(!valueResultsResult["Code"].isNull())
			resultsObject.code = valueResultsResult["Code"].asString();
		if(!valueResultsResult["Message"].isNull())
			resultsObject.message = valueResultsResult["Message"].asString();
		results_.push_back(resultsObject);
	}
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["Action"].isNull())
		action_ = value["Action"].asString();

}

std::string InactivatePhotosResult::getAction()const
{
	return action_;
}

std::string InactivatePhotosResult::getMessage()const
{
	return message_;
}

std::vector<InactivatePhotosResult::Result> InactivatePhotosResult::getResults()const
{
	return results_;
}

std::string InactivatePhotosResult::getCode()const
{
	return code_;
}

