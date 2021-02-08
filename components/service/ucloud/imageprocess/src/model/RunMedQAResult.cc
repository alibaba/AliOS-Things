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

#include <alibabacloud/imageprocess/model/RunMedQAResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imageprocess;
using namespace AlibabaCloud::Imageprocess::Model;

RunMedQAResult::RunMedQAResult() :
	ServiceResult()
{}

RunMedQAResult::RunMedQAResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

RunMedQAResult::~RunMedQAResult()
{}

void RunMedQAResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["Answer"].isNull())
		data_.answer = dataNode["Answer"].asString();
		auto allSimilarQuestion = dataNode["SimilarQuestion"]["SimilarQuestion"];
		for (auto value : allSimilarQuestion)
			data_.similarQuestion.push_back(value.asString());

}

RunMedQAResult::Data RunMedQAResult::getData()const
{
	return data_;
}

