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

#include <alibabacloud/nlp-automl/model/GetPredictResultRequest.h>

using AlibabaCloud::Nlp_automl::Model::GetPredictResultRequest;

GetPredictResultRequest::GetPredictResultRequest() :
	RpcServiceRequest("nlp-automl", "2019-11-11", "GetPredictResult")
{
	setMethod(HttpRequest::Method::Post);
}

GetPredictResultRequest::~GetPredictResultRequest()
{}

int GetPredictResultRequest::getTopK()const
{
	return topK_;
}

void GetPredictResultRequest::setTopK(int topK)
{
	topK_ = topK;
	setBodyParameter("TopK", std::to_string(topK));
}

int GetPredictResultRequest::getModelId()const
{
	return modelId_;
}

void GetPredictResultRequest::setModelId(int modelId)
{
	modelId_ = modelId;
	setBodyParameter("ModelId", std::to_string(modelId));
}

std::string GetPredictResultRequest::getDetailTag()const
{
	return detailTag_;
}

void GetPredictResultRequest::setDetailTag(const std::string& detailTag)
{
	detailTag_ = detailTag;
	setBodyParameter("DetailTag", detailTag);
}

std::string GetPredictResultRequest::getContent()const
{
	return content_;
}

void GetPredictResultRequest::setContent(const std::string& content)
{
	content_ = content;
	setBodyParameter("Content", content);
}

std::string GetPredictResultRequest::getModelVersion()const
{
	return modelVersion_;
}

void GetPredictResultRequest::setModelVersion(const std::string& modelVersion)
{
	modelVersion_ = modelVersion;
	setBodyParameter("ModelVersion", modelVersion);
}

