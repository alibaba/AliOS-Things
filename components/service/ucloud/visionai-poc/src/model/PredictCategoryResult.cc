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

#include <alibabacloud/visionai-poc/model/PredictCategoryResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Visionai_poc;
using namespace AlibabaCloud::Visionai_poc::Model;

PredictCategoryResult::PredictCategoryResult() :
	ServiceResult()
{}

PredictCategoryResult::PredictCategoryResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

PredictCategoryResult::~PredictCategoryResult()
{}

void PredictCategoryResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto responseNode = value["Response"];
	if(!responseNode["Success"].isNull())
		response_.success = responseNode["Success"].asString() == "true";
	if(!responseNode["Url"].isNull())
		response_.url = responseNode["Url"].asString();
	if(!responseNode["RequestId"].isNull())
		response_.requestId = responseNode["RequestId"].asString();
	if(!responseNode["ErrorCode"].isNull())
		response_.errorCode = responseNode["ErrorCode"].asString();
	if(!responseNode["ErrorMessage"].isNull())
		response_.errorMessage = responseNode["ErrorMessage"].asString();
	auto allDataNode = responseNode["Data"]["Node"];
	for (auto responseNodeDataNode : allDataNode)
	{
		Response::Node nodeObject;
		if(!responseNodeDataNode["CateId"].isNull())
			nodeObject.cateId = responseNodeDataNode["CateId"].asString();
		auto categoryNode = value["Category"];
		if(!categoryNode["CateLevelFOURName"].isNull())
			nodeObject.category.cateLevelFOURName = categoryNode["CateLevelFOURName"].asString();
		if(!categoryNode["CateLevelTwoId"].isNull())
			nodeObject.category.cateLevelTwoId = std::stoi(categoryNode["CateLevelTwoId"].asString());
		if(!categoryNode["CateLevelOneId"].isNull())
			nodeObject.category.cateLevelOneId = std::stoi(categoryNode["CateLevelOneId"].asString());
		if(!categoryNode["CateLevelOneName"].isNull())
			nodeObject.category.cateLevelOneName = categoryNode["CateLevelOneName"].asString();
		if(!categoryNode["CateLevelFourId"].isNull())
			nodeObject.category.cateLevelFourId = std::stoi(categoryNode["CateLevelFourId"].asString());
		if(!categoryNode["CateLevel"].isNull())
			nodeObject.category.cateLevel = std::stoi(categoryNode["CateLevel"].asString());
		if(!categoryNode["CateLevelThreeId"].isNull())
			nodeObject.category.cateLevelThreeId = std::stoi(categoryNode["CateLevelThreeId"].asString());
		if(!categoryNode["CateLevelFiveId"].isNull())
			nodeObject.category.cateLevelFiveId = std::stoi(categoryNode["CateLevelFiveId"].asString());
		if(!categoryNode["CateLevelFiveName"].isNull())
			nodeObject.category.cateLevelFiveName = categoryNode["CateLevelFiveName"].asString();
		if(!categoryNode["CateName"].isNull())
			nodeObject.category.cateName = categoryNode["CateName"].asString();
		if(!categoryNode["CateLevelTwoName"].isNull())
			nodeObject.category.cateLevelTwoName = categoryNode["CateLevelTwoName"].asString();
		if(!categoryNode["Score"].isNull())
			nodeObject.category.score = std::stof(categoryNode["Score"].asString());
		if(!categoryNode["CateId"].isNull())
			nodeObject.category.cateId = std::stoi(categoryNode["CateId"].asString());
		if(!categoryNode["CateLevelThreeName"].isNull())
			nodeObject.category.cateLevelThreeName = categoryNode["CateLevelThreeName"].asString();
		response_.data.push_back(nodeObject);
	}
	if(!value["Code"].isNull())
		code_ = std::stoi(value["Code"].asString());
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";

}

PredictCategoryResult::Response PredictCategoryResult::getResponse()const
{
	return response_;
}

std::string PredictCategoryResult::getMessage()const
{
	return message_;
}

int PredictCategoryResult::getCode()const
{
	return code_;
}

bool PredictCategoryResult::getSuccess()const
{
	return success_;
}

