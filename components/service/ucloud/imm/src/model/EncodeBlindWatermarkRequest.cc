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

#include <alibabacloud/imm/model/EncodeBlindWatermarkRequest.h>

using AlibabaCloud::Imm::Model::EncodeBlindWatermarkRequest;

EncodeBlindWatermarkRequest::EncodeBlindWatermarkRequest() :
	RpcServiceRequest("imm", "2017-09-06", "EncodeBlindWatermark")
{
	setMethod(HttpRequest::Method::Post);
}

EncodeBlindWatermarkRequest::~EncodeBlindWatermarkRequest()
{}

std::string EncodeBlindWatermarkRequest::getImageQuality()const
{
	return imageQuality_;
}

void EncodeBlindWatermarkRequest::setImageQuality(const std::string& imageQuality)
{
	imageQuality_ = imageQuality;
	setParameter("ImageQuality", imageQuality);
}

std::string EncodeBlindWatermarkRequest::getWatermarkUri()const
{
	return watermarkUri_;
}

void EncodeBlindWatermarkRequest::setWatermarkUri(const std::string& watermarkUri)
{
	watermarkUri_ = watermarkUri;
	setParameter("WatermarkUri", watermarkUri);
}

std::string EncodeBlindWatermarkRequest::getProject()const
{
	return project_;
}

void EncodeBlindWatermarkRequest::setProject(const std::string& project)
{
	project_ = project;
	setParameter("Project", project);
}

std::string EncodeBlindWatermarkRequest::getContent()const
{
	return content_;
}

void EncodeBlindWatermarkRequest::setContent(const std::string& content)
{
	content_ = content;
	setParameter("Content", content);
}

std::string EncodeBlindWatermarkRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void EncodeBlindWatermarkRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string EncodeBlindWatermarkRequest::getWatermarkType()const
{
	return watermarkType_;
}

void EncodeBlindWatermarkRequest::setWatermarkType(const std::string& watermarkType)
{
	watermarkType_ = watermarkType;
	setParameter("WatermarkType", watermarkType);
}

std::string EncodeBlindWatermarkRequest::getTargetUri()const
{
	return targetUri_;
}

void EncodeBlindWatermarkRequest::setTargetUri(const std::string& targetUri)
{
	targetUri_ = targetUri;
	setParameter("TargetUri", targetUri);
}

std::string EncodeBlindWatermarkRequest::getModel()const
{
	return model_;
}

void EncodeBlindWatermarkRequest::setModel(const std::string& model)
{
	model_ = model;
	setParameter("Model", model);
}

std::string EncodeBlindWatermarkRequest::getTargetImageType()const
{
	return targetImageType_;
}

void EncodeBlindWatermarkRequest::setTargetImageType(const std::string& targetImageType)
{
	targetImageType_ = targetImageType;
	setParameter("TargetImageType", targetImageType);
}

std::string EncodeBlindWatermarkRequest::getImageUri()const
{
	return imageUri_;
}

void EncodeBlindWatermarkRequest::setImageUri(const std::string& imageUri)
{
	imageUri_ = imageUri;
	setParameter("ImageUri", imageUri);
}

