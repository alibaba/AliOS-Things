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

#include <alibabacloud/nlp-automl/model/CreateAsyncPredictRequest.h>

using AlibabaCloud::Nlp_automl::Model::CreateAsyncPredictRequest;

CreateAsyncPredictRequest::CreateAsyncPredictRequest() :
	RpcServiceRequest("nlp-automl", "2019-11-11", "CreateAsyncPredict")
{
	setMethod(HttpRequest::Method::Post);
}

CreateAsyncPredictRequest::~CreateAsyncPredictRequest()
{}

int CreateAsyncPredictRequest::getTopK()const
{
	return topK_;
}

void CreateAsyncPredictRequest::setTopK(int topK)
{
	topK_ = topK;
	setBodyParameter("TopK", std::to_string(topK));
}

std::string CreateAsyncPredictRequest::getFileType()const
{
	return fileType_;
}

void CreateAsyncPredictRequest::setFileType(const std::string& fileType)
{
	fileType_ = fileType;
	setBodyParameter("FileType", fileType);
}

std::string CreateAsyncPredictRequest::getDetailTag()const
{
	return detailTag_;
}

void CreateAsyncPredictRequest::setDetailTag(const std::string& detailTag)
{
	detailTag_ = detailTag;
	setBodyParameter("DetailTag", detailTag);
}

std::string CreateAsyncPredictRequest::getFetchContent()const
{
	return fetchContent_;
}

void CreateAsyncPredictRequest::setFetchContent(const std::string& fetchContent)
{
	fetchContent_ = fetchContent;
	setBodyParameter("FetchContent", fetchContent);
}

std::string CreateAsyncPredictRequest::getContent()const
{
	return content_;
}

void CreateAsyncPredictRequest::setContent(const std::string& content)
{
	content_ = content;
	setBodyParameter("Content", content);
}

std::string CreateAsyncPredictRequest::getFileContent()const
{
	return fileContent_;
}

void CreateAsyncPredictRequest::setFileContent(const std::string& fileContent)
{
	fileContent_ = fileContent;
	setBodyParameter("FileContent", fileContent);
}

int CreateAsyncPredictRequest::getModelId()const
{
	return modelId_;
}

void CreateAsyncPredictRequest::setModelId(int modelId)
{
	modelId_ = modelId;
	setBodyParameter("ModelId", std::to_string(modelId));
}

std::string CreateAsyncPredictRequest::getFileUrl()const
{
	return fileUrl_;
}

void CreateAsyncPredictRequest::setFileUrl(const std::string& fileUrl)
{
	fileUrl_ = fileUrl;
	setBodyParameter("FileUrl", fileUrl);
}

std::string CreateAsyncPredictRequest::getModelVersion()const
{
	return modelVersion_;
}

void CreateAsyncPredictRequest::setModelVersion(const std::string& modelVersion)
{
	modelVersion_ = modelVersion;
	setBodyParameter("ModelVersion", modelVersion);
}

