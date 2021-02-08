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

#include <alibabacloud/ivision/model/ImagePredictRequest.h>

using AlibabaCloud::Ivision::Model::ImagePredictRequest;

ImagePredictRequest::ImagePredictRequest() :
	RpcServiceRequest("ivision", "2019-03-08", "ImagePredict")
{
	setMethod(HttpRequest::Method::Get);
}

ImagePredictRequest::~ImagePredictRequest()
{}

std::string ImagePredictRequest::getDataUrl()const
{
	return dataUrl_;
}

void ImagePredictRequest::setDataUrl(const std::string& dataUrl)
{
	dataUrl_ = dataUrl;
	setParameter("DataUrl", dataUrl);
}

std::string ImagePredictRequest::getShowLog()const
{
	return showLog_;
}

void ImagePredictRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

std::string ImagePredictRequest::getModelId()const
{
	return modelId_;
}

void ImagePredictRequest::setModelId(const std::string& modelId)
{
	modelId_ = modelId;
	setParameter("ModelId", modelId);
}

long ImagePredictRequest::getOwnerId()const
{
	return ownerId_;
}

void ImagePredictRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

