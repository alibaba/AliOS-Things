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

#include <alibabacloud/visionai-poc/model/RecognizeEntityResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Visionai_poc;
using namespace AlibabaCloud::Visionai_poc::Model;

RecognizeEntityResult::RecognizeEntityResult() :
	ServiceResult()
{}

RecognizeEntityResult::RecognizeEntityResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

RecognizeEntityResult::~RecognizeEntityResult()
{}

void RecognizeEntityResult::parse(const std::string &payload)
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
	auto dataNode = responseNode["Data"];
	if(!dataNode["CallSubModelApi"].isNull())
		response_.data.callSubModelApi = std::stoi(dataNode["CallSubModelApi"].asString());
	if(!dataNode["RequestId"].isNull())
		response_.data.requestId = dataNode["RequestId"].asString();
	if(!dataNode["UseSubModelResult"].isNull())
		response_.data.useSubModelResult = std::stoi(dataNode["UseSubModelResult"].asString());
	auto allResultNode = dataNode["Result"]["Node"];
	for (auto dataNodeResultNode : allResultNode)
	{
		Response::Data::Node nodeObject;
		if(!dataNodeResultNode["Score"].isNull())
			nodeObject.score = dataNodeResultNode["Score"].asString();
		if(!dataNodeResultNode["Tag"].isNull())
			nodeObject.tag = dataNodeResultNode["Tag"].asString();
		response_.data.result.push_back(nodeObject);
	}
	if(!value["Code"].isNull())
		code_ = std::stoi(value["Code"].asString());
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";

}

RecognizeEntityResult::Response RecognizeEntityResult::getResponse()const
{
	return response_;
}

std::string RecognizeEntityResult::getMessage()const
{
	return message_;
}

int RecognizeEntityResult::getCode()const
{
	return code_;
}

bool RecognizeEntityResult::getSuccess()const
{
	return success_;
}

