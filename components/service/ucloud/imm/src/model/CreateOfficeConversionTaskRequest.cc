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

#include <alibabacloud/imm/model/CreateOfficeConversionTaskRequest.h>

using AlibabaCloud::Imm::Model::CreateOfficeConversionTaskRequest;

CreateOfficeConversionTaskRequest::CreateOfficeConversionTaskRequest() :
	RpcServiceRequest("imm", "2017-09-06", "CreateOfficeConversionTask")
{
	setMethod(HttpRequest::Method::Post);
}

CreateOfficeConversionTaskRequest::~CreateOfficeConversionTaskRequest()
{}

std::string CreateOfficeConversionTaskRequest::getSrcType()const
{
	return srcType_;
}

void CreateOfficeConversionTaskRequest::setSrcType(const std::string& srcType)
{
	srcType_ = srcType;
	setParameter("SrcType", srcType);
}

std::string CreateOfficeConversionTaskRequest::getProject()const
{
	return project_;
}

void CreateOfficeConversionTaskRequest::setProject(const std::string& project)
{
	project_ = project;
	setParameter("Project", project);
}

std::string CreateOfficeConversionTaskRequest::getIdempotentToken()const
{
	return idempotentToken_;
}

void CreateOfficeConversionTaskRequest::setIdempotentToken(const std::string& idempotentToken)
{
	idempotentToken_ = idempotentToken;
	setParameter("IdempotentToken", idempotentToken);
}

bool CreateOfficeConversionTaskRequest::getPdfVector()const
{
	return pdfVector_;
}

void CreateOfficeConversionTaskRequest::setPdfVector(bool pdfVector)
{
	pdfVector_ = pdfVector;
	setParameter("PdfVector", pdfVector ? "true" : "false");
}

std::string CreateOfficeConversionTaskRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void CreateOfficeConversionTaskRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string CreateOfficeConversionTaskRequest::getPassword()const
{
	return password_;
}

void CreateOfficeConversionTaskRequest::setPassword(const std::string& password)
{
	password_ = password;
	setParameter("Password", password);
}

long CreateOfficeConversionTaskRequest::getStartPage()const
{
	return startPage_;
}

void CreateOfficeConversionTaskRequest::setStartPage(long startPage)
{
	startPage_ = startPage;
	setParameter("StartPage", std::to_string(startPage));
}

std::string CreateOfficeConversionTaskRequest::getNotifyEndpoint()const
{
	return notifyEndpoint_;
}

void CreateOfficeConversionTaskRequest::setNotifyEndpoint(const std::string& notifyEndpoint)
{
	notifyEndpoint_ = notifyEndpoint;
	setParameter("NotifyEndpoint", notifyEndpoint);
}

bool CreateOfficeConversionTaskRequest::getFitToPagesWide()const
{
	return fitToPagesWide_;
}

void CreateOfficeConversionTaskRequest::setFitToPagesWide(bool fitToPagesWide)
{
	fitToPagesWide_ = fitToPagesWide;
	setParameter("FitToPagesWide", fitToPagesWide ? "true" : "false");
}

std::string CreateOfficeConversionTaskRequest::getTgtFilePrefix()const
{
	return tgtFilePrefix_;
}

void CreateOfficeConversionTaskRequest::setTgtFilePrefix(const std::string& tgtFilePrefix)
{
	tgtFilePrefix_ = tgtFilePrefix;
	setParameter("TgtFilePrefix", tgtFilePrefix);
}

std::string CreateOfficeConversionTaskRequest::getNotifyTopicName()const
{
	return notifyTopicName_;
}

void CreateOfficeConversionTaskRequest::setNotifyTopicName(const std::string& notifyTopicName)
{
	notifyTopicName_ = notifyTopicName;
	setParameter("NotifyTopicName", notifyTopicName);
}

std::string CreateOfficeConversionTaskRequest::getModelId()const
{
	return modelId_;
}

void CreateOfficeConversionTaskRequest::setModelId(const std::string& modelId)
{
	modelId_ = modelId;
	setParameter("ModelId", modelId);
}

int CreateOfficeConversionTaskRequest::getDisplayDpi()const
{
	return displayDpi_;
}

void CreateOfficeConversionTaskRequest::setDisplayDpi(int displayDpi)
{
	displayDpi_ = displayDpi;
	setParameter("DisplayDpi", std::to_string(displayDpi));
}

long CreateOfficeConversionTaskRequest::getMaxSheetRow()const
{
	return maxSheetRow_;
}

void CreateOfficeConversionTaskRequest::setMaxSheetRow(long maxSheetRow)
{
	maxSheetRow_ = maxSheetRow;
	setParameter("MaxSheetRow", std::to_string(maxSheetRow));
}

long CreateOfficeConversionTaskRequest::getMaxSheetCount()const
{
	return maxSheetCount_;
}

void CreateOfficeConversionTaskRequest::setMaxSheetCount(long maxSheetCount)
{
	maxSheetCount_ = maxSheetCount;
	setParameter("MaxSheetCount", std::to_string(maxSheetCount));
}

long CreateOfficeConversionTaskRequest::getEndPage()const
{
	return endPage_;
}

void CreateOfficeConversionTaskRequest::setEndPage(long endPage)
{
	endPage_ = endPage;
	setParameter("EndPage", std::to_string(endPage));
}

std::string CreateOfficeConversionTaskRequest::getTgtFileSuffix()const
{
	return tgtFileSuffix_;
}

void CreateOfficeConversionTaskRequest::setTgtFileSuffix(const std::string& tgtFileSuffix)
{
	tgtFileSuffix_ = tgtFileSuffix;
	setParameter("TgtFileSuffix", tgtFileSuffix);
}

bool CreateOfficeConversionTaskRequest::getSheetOnePage()const
{
	return sheetOnePage_;
}

void CreateOfficeConversionTaskRequest::setSheetOnePage(bool sheetOnePage)
{
	sheetOnePage_ = sheetOnePage;
	setParameter("SheetOnePage", sheetOnePage ? "true" : "false");
}

long CreateOfficeConversionTaskRequest::getMaxSheetCol()const
{
	return maxSheetCol_;
}

void CreateOfficeConversionTaskRequest::setMaxSheetCol(long maxSheetCol)
{
	maxSheetCol_ = maxSheetCol;
	setParameter("MaxSheetCol", std::to_string(maxSheetCol));
}

std::string CreateOfficeConversionTaskRequest::getTgtType()const
{
	return tgtType_;
}

void CreateOfficeConversionTaskRequest::setTgtType(const std::string& tgtType)
{
	tgtType_ = tgtType;
	setParameter("TgtType", tgtType);
}

bool CreateOfficeConversionTaskRequest::getHidecomments()const
{
	return hidecomments_;
}

void CreateOfficeConversionTaskRequest::setHidecomments(bool hidecomments)
{
	hidecomments_ = hidecomments;
	setParameter("Hidecomments", hidecomments ? "true" : "false");
}

bool CreateOfficeConversionTaskRequest::getFitToPagesTall()const
{
	return fitToPagesTall_;
}

void CreateOfficeConversionTaskRequest::setFitToPagesTall(bool fitToPagesTall)
{
	fitToPagesTall_ = fitToPagesTall;
	setParameter("FitToPagesTall", fitToPagesTall ? "true" : "false");
}

std::string CreateOfficeConversionTaskRequest::getSrcUri()const
{
	return srcUri_;
}

void CreateOfficeConversionTaskRequest::setSrcUri(const std::string& srcUri)
{
	srcUri_ = srcUri;
	setParameter("SrcUri", srcUri);
}

std::string CreateOfficeConversionTaskRequest::getTgtFilePages()const
{
	return tgtFilePages_;
}

void CreateOfficeConversionTaskRequest::setTgtFilePages(const std::string& tgtFilePages)
{
	tgtFilePages_ = tgtFilePages;
	setParameter("TgtFilePages", tgtFilePages);
}

std::string CreateOfficeConversionTaskRequest::getTgtUri()const
{
	return tgtUri_;
}

void CreateOfficeConversionTaskRequest::setTgtUri(const std::string& tgtUri)
{
	tgtUri_ = tgtUri;
	setParameter("TgtUri", tgtUri);
}

