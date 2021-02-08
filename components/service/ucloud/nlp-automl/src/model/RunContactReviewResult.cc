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

#include <alibabacloud/nlp-automl/model/RunContactReviewResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Nlp_automl;
using namespace AlibabaCloud::Nlp_automl::Model;

RunContactReviewResult::RunContactReviewResult() :
	ServiceResult()
{}

RunContactReviewResult::RunContactReviewResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

RunContactReviewResult::~RunContactReviewResult()
{}

void RunContactReviewResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto contactContentNode = value["ContactContent"];
	auto allReviewResultsNode = contactContentNode["ReviewResults"]["ReviewResult"];
	for (auto contactContentNodeReviewResultsReviewResult : allReviewResultsNode)
	{
		ContactContent::ReviewResult reviewResultObject;
		if(!contactContentNodeReviewResultsReviewResult["RiskLevel"].isNull())
			reviewResultObject.riskLevel = contactContentNodeReviewResultsReviewResult["RiskLevel"].asString();
		if(!contactContentNodeReviewResultsReviewResult["Reason"].isNull())
			reviewResultObject.reason = contactContentNodeReviewResultsReviewResult["Reason"].asString();
		if(!contactContentNodeReviewResultsReviewResult["Type"].isNull())
			reviewResultObject.type = contactContentNodeReviewResultsReviewResult["Type"].asString();
		if(!contactContentNodeReviewResultsReviewResult["ModificationSuggestion"].isNull())
			reviewResultObject.modificationSuggestion = contactContentNodeReviewResultsReviewResult["ModificationSuggestion"].asString();
		auto allValue = value["Value"]["Value"];
		for (auto value : allValue)
			reviewResultObject.value.push_back(value.asString());
		auto allEndPosition = value["EndPosition"]["EndPosition"];
		for (auto value : allEndPosition)
			reviewResultObject.endPosition.push_back(value.asString());
		auto allStartPosition = value["StartPosition"]["StartPosition"];
		for (auto value : allStartPosition)
			reviewResultObject.startPosition.push_back(value.asString());
		contactContent_.reviewResults.push_back(reviewResultObject);
	}
	auto allStructureResultsNode = contactContentNode["StructureResults"]["StructureResult"];
	for (auto contactContentNodeStructureResultsStructureResult : allStructureResultsNode)
	{
		ContactContent::StructureResult structureResultObject;
		if(!contactContentNodeStructureResultsStructureResult["Name"].isNull())
			structureResultObject.name = contactContentNodeStructureResultsStructureResult["Name"].asString();
		if(!contactContentNodeStructureResultsStructureResult["Type"].isNull())
			structureResultObject.type = contactContentNodeStructureResultsStructureResult["Type"].asString();
		auto allValue1 = value["Value"]["Value"];
		for (auto value : allValue1)
			structureResultObject.value1.push_back(value.asString());
		auto allEndPosition2 = value["EndPosition"]["EndPosition"];
		for (auto value : allEndPosition2)
			structureResultObject.endPosition2.push_back(value.asString());
		auto allStartPosition3 = value["StartPosition"]["StartPosition"];
		for (auto value : allStartPosition3)
			structureResultObject.startPosition3.push_back(value.asString());
		contactContent_.structureResults.push_back(structureResultObject);
	}
	if(!value["RawContractContent"].isNull())
		rawContractContent_ = value["RawContractContent"].asString();

}

std::string RunContactReviewResult::getRawContractContent()const
{
	return rawContractContent_;
}

RunContactReviewResult::ContactContent RunContactReviewResult::getContactContent()const
{
	return contactContent_;
}

