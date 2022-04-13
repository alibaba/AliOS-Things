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

#include <alibabacloud/imageenhan/model/ImageBlindPicWatermarkRequest.h>

using AlibabaCloud::Imageenhan::Model::ImageBlindPicWatermarkRequest;

ImageBlindPicWatermarkRequest::ImageBlindPicWatermarkRequest() :
	RpcServiceRequest("imageenhan", "2019-09-30", "ImageBlindPicWatermark")
{
	setMethod(HttpRequest::Method::Post);
}

ImageBlindPicWatermarkRequest::~ImageBlindPicWatermarkRequest()
{}

std::string ImageBlindPicWatermarkRequest::getWatermarkImageURL()const
{
	return watermarkImageURL_;
}

void ImageBlindPicWatermarkRequest::setWatermarkImageURL(const std::string& watermarkImageURL)
{
	watermarkImageURL_ = watermarkImageURL;
	setBodyParameter("WatermarkImageURL", watermarkImageURL);
}

int ImageBlindPicWatermarkRequest::getQualityFactor()const
{
	return qualityFactor_;
}

void ImageBlindPicWatermarkRequest::setQualityFactor(int qualityFactor)
{
	qualityFactor_ = qualityFactor;
	setBodyParameter("QualityFactor", std::to_string(qualityFactor));
}

std::string ImageBlindPicWatermarkRequest::getFunctionType()const
{
	return functionType_;
}

void ImageBlindPicWatermarkRequest::setFunctionType(const std::string& functionType)
{
	functionType_ = functionType;
	setBodyParameter("FunctionType", functionType);
}

std::string ImageBlindPicWatermarkRequest::getLogoURL()const
{
	return logoURL_;
}

void ImageBlindPicWatermarkRequest::setLogoURL(const std::string& logoURL)
{
	logoURL_ = logoURL;
	setBodyParameter("LogoURL", logoURL);
}

std::string ImageBlindPicWatermarkRequest::getOutputFileType()const
{
	return outputFileType_;
}

void ImageBlindPicWatermarkRequest::setOutputFileType(const std::string& outputFileType)
{
	outputFileType_ = outputFileType;
	setBodyParameter("OutputFileType", outputFileType);
}

std::string ImageBlindPicWatermarkRequest::getOriginImageURL()const
{
	return originImageURL_;
}

void ImageBlindPicWatermarkRequest::setOriginImageURL(const std::string& originImageURL)
{
	originImageURL_ = originImageURL;
	setBodyParameter("OriginImageURL", originImageURL);
}

