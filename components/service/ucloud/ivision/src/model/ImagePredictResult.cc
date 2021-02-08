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

#include <alibabacloud/ivision/model/ImagePredictResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Ivision;
using namespace AlibabaCloud::Ivision::Model;

ImagePredictResult::ImagePredictResult() :
	ServiceResult()
{}

ImagePredictResult::ImagePredictResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ImagePredictResult::~ImagePredictResult()
{}

void ImagePredictResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto imagePredictNode = value["ImagePredict"];
	if(!imagePredictNode["PredictId"].isNull())
		imagePredict_.predictId = imagePredictNode["PredictId"].asString();
	if(!imagePredictNode["ModelId"].isNull())
		imagePredict_.modelId = imagePredictNode["ModelId"].asString();
	if(!imagePredictNode["DataUrl"].isNull())
		imagePredict_.dataUrl = imagePredictNode["DataUrl"].asString();
	if(!imagePredictNode["PredictTime"].isNull())
		imagePredict_.predictTime = imagePredictNode["PredictTime"].asString();
	if(!imagePredictNode["Status"].isNull())
		imagePredict_.status = imagePredictNode["Status"].asString();
	if(!imagePredictNode["Code"].isNull())
		imagePredict_.code = imagePredictNode["Code"].asString();
	if(!imagePredictNode["Message"].isNull())
		imagePredict_.message = imagePredictNode["Message"].asString();
	if(!imagePredictNode["PredictResult"].isNull())
		imagePredict_.predictResult = imagePredictNode["PredictResult"].asString();

}

ImagePredictResult::ImagePredict ImagePredictResult::getImagePredict()const
{
	return imagePredict_;
}

