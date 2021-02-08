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

#include <alibabacloud/imm/model/VideoAnalyseFeedbackRequest.h>

using AlibabaCloud::Imm::Model::VideoAnalyseFeedbackRequest;

VideoAnalyseFeedbackRequest::VideoAnalyseFeedbackRequest() :
	RpcServiceRequest("imm", "2017-09-06", "VideoAnalyseFeedback")
{
	setMethod(HttpRequest::Method::Post);
}

VideoAnalyseFeedbackRequest::~VideoAnalyseFeedbackRequest()
{}

std::string VideoAnalyseFeedbackRequest::getNote()const
{
	return note_;
}

void VideoAnalyseFeedbackRequest::setNote(const std::string& note)
{
	note_ = note;
	setParameter("Note", note);
}

std::string VideoAnalyseFeedbackRequest::getProject()const
{
	return project_;
}

void VideoAnalyseFeedbackRequest::setProject(const std::string& project)
{
	project_ = project;
	setParameter("Project", project);
}

std::string VideoAnalyseFeedbackRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void VideoAnalyseFeedbackRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string VideoAnalyseFeedbackRequest::getTaskId()const
{
	return taskId_;
}

void VideoAnalyseFeedbackRequest::setTaskId(const std::string& taskId)
{
	taskId_ = taskId;
	setParameter("TaskId", taskId);
}

std::string VideoAnalyseFeedbackRequest::getFrames()const
{
	return frames_;
}

void VideoAnalyseFeedbackRequest::setFrames(const std::string& frames)
{
	frames_ = frames;
	setParameter("Frames", frames);
}

std::string VideoAnalyseFeedbackRequest::getSuggestion()const
{
	return suggestion_;
}

void VideoAnalyseFeedbackRequest::setSuggestion(const std::string& suggestion)
{
	suggestion_ = suggestion;
	setParameter("Suggestion", suggestion);
}

std::string VideoAnalyseFeedbackRequest::getUri()const
{
	return uri_;
}

void VideoAnalyseFeedbackRequest::setUri(const std::string& uri)
{
	uri_ = uri;
	setParameter("Uri", uri);
}

std::string VideoAnalyseFeedbackRequest::getScenes()const
{
	return scenes_;
}

void VideoAnalyseFeedbackRequest::setScenes(const std::string& scenes)
{
	scenes_ = scenes;
	setParameter("Scenes", scenes);
}

