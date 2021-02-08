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

#include <alibabacloud/imm/model/GetOfficeEditURLRequest.h>

using AlibabaCloud::Imm::Model::GetOfficeEditURLRequest;

GetOfficeEditURLRequest::GetOfficeEditURLRequest() :
	RpcServiceRequest("imm", "2017-09-06", "GetOfficeEditURL")
{
	setMethod(HttpRequest::Method::Post);
}

GetOfficeEditURLRequest::~GetOfficeEditURLRequest()
{}

std::string GetOfficeEditURLRequest::getSrcType()const
{
	return srcType_;
}

void GetOfficeEditURLRequest::setSrcType(const std::string& srcType)
{
	srcType_ = srcType;
	setParameter("SrcType", srcType);
}

std::string GetOfficeEditURLRequest::getProject()const
{
	return project_;
}

void GetOfficeEditURLRequest::setProject(const std::string& project)
{
	project_ = project;
	setParameter("Project", project);
}

std::string GetOfficeEditURLRequest::getUserID()const
{
	return userID_;
}

void GetOfficeEditURLRequest::setUserID(const std::string& userID)
{
	userID_ = userID;
	setParameter("UserID", userID);
}

std::string GetOfficeEditURLRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void GetOfficeEditURLRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string GetOfficeEditURLRequest::getNotifyEndpoint()const
{
	return notifyEndpoint_;
}

void GetOfficeEditURLRequest::setNotifyEndpoint(const std::string& notifyEndpoint)
{
	notifyEndpoint_ = notifyEndpoint;
	setParameter("NotifyEndpoint", notifyEndpoint);
}

std::string GetOfficeEditURLRequest::getFileID()const
{
	return fileID_;
}

void GetOfficeEditURLRequest::setFileID(const std::string& fileID)
{
	fileID_ = fileID;
	setParameter("FileID", fileID);
}

std::string GetOfficeEditURLRequest::getNotifyTopicName()const
{
	return notifyTopicName_;
}

void GetOfficeEditURLRequest::setNotifyTopicName(const std::string& notifyTopicName)
{
	notifyTopicName_ = notifyTopicName;
	setParameter("NotifyTopicName", notifyTopicName);
}

std::string GetOfficeEditURLRequest::getFileName()const
{
	return fileName_;
}

void GetOfficeEditURLRequest::setFileName(const std::string& fileName)
{
	fileName_ = fileName;
	setParameter("FileName", fileName);
}

std::string GetOfficeEditURLRequest::getSrcUri()const
{
	return srcUri_;
}

void GetOfficeEditURLRequest::setSrcUri(const std::string& srcUri)
{
	srcUri_ = srcUri;
	setParameter("SrcUri", srcUri);
}

std::string GetOfficeEditURLRequest::getTgtUri()const
{
	return tgtUri_;
}

void GetOfficeEditURLRequest::setTgtUri(const std::string& tgtUri)
{
	tgtUri_ = tgtUri;
	setParameter("TgtUri", tgtUri);
}

std::string GetOfficeEditURLRequest::getUserName()const
{
	return userName_;
}

void GetOfficeEditURLRequest::setUserName(const std::string& userName)
{
	userName_ = userName;
	setParameter("UserName", userName);
}

