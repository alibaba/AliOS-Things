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

#include <alibabacloud/alimt/model/TranslateCertificateRequest.h>

using AlibabaCloud::Alimt::Model::TranslateCertificateRequest;

TranslateCertificateRequest::TranslateCertificateRequest() :
	RpcServiceRequest("alimt", "2018-10-12", "TranslateCertificate")
{
	setMethod(HttpRequest::Method::Post);
}

TranslateCertificateRequest::~TranslateCertificateRequest()
{}

std::string TranslateCertificateRequest::getSourceLanguage()const
{
	return sourceLanguage_;
}

void TranslateCertificateRequest::setSourceLanguage(const std::string& sourceLanguage)
{
	sourceLanguage_ = sourceLanguage;
	setBodyParameter("SourceLanguage", sourceLanguage);
}

std::string TranslateCertificateRequest::getCertificateType()const
{
	return certificateType_;
}

void TranslateCertificateRequest::setCertificateType(const std::string& certificateType)
{
	certificateType_ = certificateType;
	setBodyParameter("CertificateType", certificateType);
}

std::string TranslateCertificateRequest::getResultType()const
{
	return resultType_;
}

void TranslateCertificateRequest::setResultType(const std::string& resultType)
{
	resultType_ = resultType;
	setBodyParameter("ResultType", resultType);
}

std::string TranslateCertificateRequest::getImageUrl()const
{
	return imageUrl_;
}

void TranslateCertificateRequest::setImageUrl(const std::string& imageUrl)
{
	imageUrl_ = imageUrl;
	setBodyParameter("ImageUrl", imageUrl);
}

std::string TranslateCertificateRequest::getTargetLanguage()const
{
	return targetLanguage_;
}

void TranslateCertificateRequest::setTargetLanguage(const std::string& targetLanguage)
{
	targetLanguage_ = targetLanguage;
	setBodyParameter("TargetLanguage", targetLanguage);
}

