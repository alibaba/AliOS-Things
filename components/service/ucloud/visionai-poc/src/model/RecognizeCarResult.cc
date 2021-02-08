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

#include <alibabacloud/visionai-poc/model/RecognizeCarResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Visionai_poc;
using namespace AlibabaCloud::Visionai_poc::Model;

RecognizeCarResult::RecognizeCarResult() :
	ServiceResult()
{}

RecognizeCarResult::RecognizeCarResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

RecognizeCarResult::~RecognizeCarResult()
{}

void RecognizeCarResult::parse(const std::string &payload)
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
	if(!dataNode["External"].isNull())
		response_.data.external = dataNode["External"].asString();
	if(!dataNode["ModelName"].isNull())
		response_.data.modelName = dataNode["ModelName"].asString();
	if(!dataNode["Confidence"].isNull())
		response_.data.confidence = dataNode["Confidence"].asString();
	if(!dataNode["Prop"].isNull())
		response_.data.prop = dataNode["Prop"].asString();
	if(!dataNode["Name"].isNull())
		response_.data.name = dataNode["Name"].asString();
	if(!dataNode["PicUrl"].isNull())
		response_.data.picUrl = dataNode["PicUrl"].asString();
	if(!dataNode["SptId"].isNull())
		response_.data.sptId = dataNode["SptId"].asString();
	if(!dataNode["Roid"].isNull())
		response_.data.roid = dataNode["Roid"].asString();
	if(!dataNode["Tips"].isNull())
		response_.data.tips = dataNode["Tips"].asString();
	if(!dataNode["Description"].isNull())
		response_.data.description = dataNode["Description"].asString();
	if(!dataNode["PredictRes"].isNull())
		response_.data.predictRes = dataNode["PredictRes"].asString();
	if(!value["Code"].isNull())
		code_ = std::stoi(value["Code"].asString());
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";

}

RecognizeCarResult::Response RecognizeCarResult::getResponse()const
{
	return response_;
}

std::string RecognizeCarResult::getMessage()const
{
	return message_;
}

int RecognizeCarResult::getCode()const
{
	return code_;
}

bool RecognizeCarResult::getSuccess()const
{
	return success_;
}

