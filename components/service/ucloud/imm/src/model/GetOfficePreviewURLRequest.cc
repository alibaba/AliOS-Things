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

#include <alibabacloud/imm/model/GetOfficePreviewURLRequest.h>

using AlibabaCloud::Imm::Model::GetOfficePreviewURLRequest;

GetOfficePreviewURLRequest::GetOfficePreviewURLRequest() :
	RpcServiceRequest("imm", "2017-09-06", "GetOfficePreviewURL")
{
	setMethod(HttpRequest::Method::Post);
}

GetOfficePreviewURLRequest::~GetOfficePreviewURLRequest()
{}

std::string GetOfficePreviewURLRequest::getSrcType()const
{
	return srcType_;
}

void GetOfficePreviewURLRequest::setSrcType(const std::string& srcType)
{
	srcType_ = srcType;
	setParameter("SrcType", srcType);
}

std::string GetOfficePreviewURLRequest::getProject()const
{
	return project_;
}

void GetOfficePreviewURLRequest::setProject(const std::string& project)
{
	project_ = project;
	setParameter("Project", project);
}

int GetOfficePreviewURLRequest::getWatermarkVertical()const
{
	return watermarkVertical_;
}

void GetOfficePreviewURLRequest::setWatermarkVertical(int watermarkVertical)
{
	watermarkVertical_ = watermarkVertical;
	setParameter("WatermarkVertical", std::to_string(watermarkVertical));
}

std::string GetOfficePreviewURLRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void GetOfficePreviewURLRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

int GetOfficePreviewURLRequest::getWatermarkType()const
{
	return watermarkType_;
}

void GetOfficePreviewURLRequest::setWatermarkType(int watermarkType)
{
	watermarkType_ = watermarkType;
	setParameter("WatermarkType", std::to_string(watermarkType));
}

float GetOfficePreviewURLRequest::getWatermarkRotate()const
{
	return watermarkRotate_;
}

void GetOfficePreviewURLRequest::setWatermarkRotate(float watermarkRotate)
{
	watermarkRotate_ = watermarkRotate;
	setParameter("WatermarkRotate", std::to_string(watermarkRotate));
}

std::string GetOfficePreviewURLRequest::getWatermarkValue()const
{
	return watermarkValue_;
}

void GetOfficePreviewURLRequest::setWatermarkValue(const std::string& watermarkValue)
{
	watermarkValue_ = watermarkValue;
	setParameter("WatermarkValue", watermarkValue);
}

std::string GetOfficePreviewURLRequest::getWatermarkFont()const
{
	return watermarkFont_;
}

void GetOfficePreviewURLRequest::setWatermarkFont(const std::string& watermarkFont)
{
	watermarkFont_ = watermarkFont;
	setParameter("WatermarkFont", watermarkFont);
}

int GetOfficePreviewURLRequest::getWatermarkHorizontal()const
{
	return watermarkHorizontal_;
}

void GetOfficePreviewURLRequest::setWatermarkHorizontal(int watermarkHorizontal)
{
	watermarkHorizontal_ = watermarkHorizontal;
	setParameter("WatermarkHorizontal", std::to_string(watermarkHorizontal));
}

std::string GetOfficePreviewURLRequest::getSrcUri()const
{
	return srcUri_;
}

void GetOfficePreviewURLRequest::setSrcUri(const std::string& srcUri)
{
	srcUri_ = srcUri;
	setParameter("SrcUri", srcUri);
}

std::string GetOfficePreviewURLRequest::getWatermarkFillStyle()const
{
	return watermarkFillStyle_;
}

void GetOfficePreviewURLRequest::setWatermarkFillStyle(const std::string& watermarkFillStyle)
{
	watermarkFillStyle_ = watermarkFillStyle;
	setParameter("WatermarkFillStyle", watermarkFillStyle);
}

