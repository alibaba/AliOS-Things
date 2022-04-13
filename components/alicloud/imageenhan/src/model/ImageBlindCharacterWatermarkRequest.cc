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

#include <alibabacloud/imageenhan/model/ImageBlindCharacterWatermarkRequest.h>

using AlibabaCloud::Imageenhan::Model::ImageBlindCharacterWatermarkRequest;

ImageBlindCharacterWatermarkRequest::ImageBlindCharacterWatermarkRequest() :
	RpcServiceRequest("imageenhan", "2019-09-30", "ImageBlindCharacterWatermark")
{
	setMethod(HttpRequest::Method::Post);
}

ImageBlindCharacterWatermarkRequest::~ImageBlindCharacterWatermarkRequest()
{}

std::string ImageBlindCharacterWatermarkRequest::getWatermarkImageURL()const
{
	return watermarkImageURL_;
}

void ImageBlindCharacterWatermarkRequest::setWatermarkImageURL(const std::string& watermarkImageURL)
{
	watermarkImageURL_ = watermarkImageURL;
	setBodyParameter("WatermarkImageURL", watermarkImageURL);
}

int ImageBlindCharacterWatermarkRequest::getQualityFactor()const
{
	return qualityFactor_;
}

void ImageBlindCharacterWatermarkRequest::setQualityFactor(int qualityFactor)
{
	qualityFactor_ = qualityFactor;
	setBodyParameter("QualityFactor", std::to_string(qualityFactor));
}

std::string ImageBlindCharacterWatermarkRequest::getFunctionType()const
{
	return functionType_;
}

void ImageBlindCharacterWatermarkRequest::setFunctionType(const std::string& functionType)
{
	functionType_ = functionType;
	setBodyParameter("FunctionType", functionType);
}

std::string ImageBlindCharacterWatermarkRequest::getOutputFileType()const
{
	return outputFileType_;
}

void ImageBlindCharacterWatermarkRequest::setOutputFileType(const std::string& outputFileType)
{
	outputFileType_ = outputFileType;
	setBodyParameter("OutputFileType", outputFileType);
}

std::string ImageBlindCharacterWatermarkRequest::getOriginImageURL()const
{
	return originImageURL_;
}

void ImageBlindCharacterWatermarkRequest::setOriginImageURL(const std::string& originImageURL)
{
	originImageURL_ = originImageURL;
	setBodyParameter("OriginImageURL", originImageURL);
}

std::string ImageBlindCharacterWatermarkRequest::getText()const
{
	return text_;
}

void ImageBlindCharacterWatermarkRequest::setText(const std::string& text)
{
	text_ = text;
	setBodyParameter("Text", text);
}

