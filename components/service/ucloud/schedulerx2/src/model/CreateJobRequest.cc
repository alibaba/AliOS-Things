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

#include <alibabacloud/schedulerx2/model/CreateJobRequest.h>

using AlibabaCloud::Schedulerx2::Model::CreateJobRequest;

CreateJobRequest::CreateJobRequest() :
	RpcServiceRequest("schedulerx2", "2019-04-30", "CreateJob")
{
	setMethod(HttpRequest::Method::Post);
}

CreateJobRequest::~CreateJobRequest()
{}

std::string CreateJobRequest::getNamespaceSource()const
{
	return namespaceSource_;
}

void CreateJobRequest::setNamespaceSource(const std::string& namespaceSource)
{
	namespaceSource_ = namespaceSource;
	setBodyParameter("NamespaceSource", namespaceSource);
}

std::string CreateJobRequest::getDescription()const
{
	return description_;
}

void CreateJobRequest::setDescription(const std::string& description)
{
	description_ = description;
	setBodyParameter("Description", description);
}

int CreateJobRequest::getAttemptInterval()const
{
	return attemptInterval_;
}

void CreateJobRequest::setAttemptInterval(int attemptInterval)
{
	attemptInterval_ = attemptInterval;
	setBodyParameter("AttemptInterval", std::to_string(attemptInterval));
}

std::string CreateJobRequest::getContent()const
{
	return content_;
}

void CreateJobRequest::setContent(const std::string& content)
{
	content_ = content;
	setBodyParameter("Content", content);
}

long CreateJobRequest::getTimeout()const
{
	return timeout_;
}

void CreateJobRequest::setTimeout(long timeout)
{
	timeout_ = timeout;
	setBodyParameter("Timeout", std::to_string(timeout));
}

bool CreateJobRequest::getTimeoutKillEnable()const
{
	return timeoutKillEnable_;
}

void CreateJobRequest::setTimeoutKillEnable(bool timeoutKillEnable)
{
	timeoutKillEnable_ = timeoutKillEnable;
	setBodyParameter("TimeoutKillEnable", timeoutKillEnable ? "true" : "false");
}

int CreateJobRequest::getPageSize()const
{
	return pageSize_;
}

void CreateJobRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setBodyParameter("PageSize", std::to_string(pageSize));
}

int CreateJobRequest::getConsumerSize()const
{
	return consumerSize_;
}

void CreateJobRequest::setConsumerSize(int consumerSize)
{
	consumerSize_ = consumerSize;
	setBodyParameter("ConsumerSize", std::to_string(consumerSize));
}

std::string CreateJobRequest::getJarUrl()const
{
	return jarUrl_;
}

void CreateJobRequest::setJarUrl(const std::string& jarUrl)
{
	jarUrl_ = jarUrl;
	setBodyParameter("JarUrl", jarUrl);
}

std::string CreateJobRequest::getCalendar()const
{
	return calendar_;
}

void CreateJobRequest::setCalendar(const std::string& calendar)
{
	calendar_ = calendar;
	setBodyParameter("Calendar", calendar);
}

bool CreateJobRequest::getFailEnable()const
{
	return failEnable_;
}

void CreateJobRequest::setFailEnable(bool failEnable)
{
	failEnable_ = failEnable;
	setBodyParameter("FailEnable", failEnable ? "true" : "false");
}

std::string CreateJobRequest::getSendChannel()const
{
	return sendChannel_;
}

void CreateJobRequest::setSendChannel(const std::string& sendChannel)
{
	sendChannel_ = sendChannel;
	setBodyParameter("SendChannel", sendChannel);
}

int CreateJobRequest::getDataOffset()const
{
	return dataOffset_;
}

void CreateJobRequest::setDataOffset(int dataOffset)
{
	dataOffset_ = dataOffset;
	setBodyParameter("DataOffset", std::to_string(dataOffset));
}

std::string CreateJobRequest::getGroupId()const
{
	return groupId_;
}

void CreateJobRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setBodyParameter("GroupId", groupId);
}

int CreateJobRequest::getTaskMaxAttempt()const
{
	return taskMaxAttempt_;
}

void CreateJobRequest::setTaskMaxAttempt(int taskMaxAttempt)
{
	taskMaxAttempt_ = taskMaxAttempt;
	setBodyParameter("TaskMaxAttempt", std::to_string(taskMaxAttempt));
}

int CreateJobRequest::getMaxAttempt()const
{
	return maxAttempt_;
}

void CreateJobRequest::setMaxAttempt(int maxAttempt)
{
	maxAttempt_ = maxAttempt;
	setBodyParameter("MaxAttempt", std::to_string(maxAttempt));
}

int CreateJobRequest::getDispatcherSize()const
{
	return dispatcherSize_;
}

void CreateJobRequest::setDispatcherSize(int dispatcherSize)
{
	dispatcherSize_ = dispatcherSize;
	setBodyParameter("DispatcherSize", std::to_string(dispatcherSize));
}

std::string CreateJobRequest::getJobType()const
{
	return jobType_;
}

void CreateJobRequest::setJobType(const std::string& jobType)
{
	jobType_ = jobType;
	setBodyParameter("JobType", jobType);
}

int CreateJobRequest::getTaskAttemptInterval()const
{
	return taskAttemptInterval_;
}

void CreateJobRequest::setTaskAttemptInterval(int taskAttemptInterval)
{
	taskAttemptInterval_ = taskAttemptInterval;
	setBodyParameter("TaskAttemptInterval", std::to_string(taskAttemptInterval));
}

std::string CreateJobRequest::getExecuteMode()const
{
	return executeMode_;
}

void CreateJobRequest::setExecuteMode(const std::string& executeMode)
{
	executeMode_ = executeMode;
	setBodyParameter("ExecuteMode", executeMode);
}

int CreateJobRequest::getQueueSize()const
{
	return queueSize_;
}

void CreateJobRequest::setQueueSize(int queueSize)
{
	queueSize_ = queueSize;
	setBodyParameter("QueueSize", std::to_string(queueSize));
}

std::string CreateJobRequest::getTimeExpression()const
{
	return timeExpression_;
}

void CreateJobRequest::setTimeExpression(const std::string& timeExpression)
{
	timeExpression_ = timeExpression;
	setBodyParameter("TimeExpression", timeExpression);
}

std::string CreateJobRequest::getClassName()const
{
	return className_;
}

void CreateJobRequest::setClassName(const std::string& className)
{
	className_ = className;
	setBodyParameter("ClassName", className);
}

bool CreateJobRequest::getTimeoutEnable()const
{
	return timeoutEnable_;
}

void CreateJobRequest::setTimeoutEnable(bool timeoutEnable)
{
	timeoutEnable_ = timeoutEnable;
	setBodyParameter("TimeoutEnable", timeoutEnable ? "true" : "false");
}

std::vector<CreateJobRequest::ContactInfo> CreateJobRequest::getContactInfo()const
{
	return contactInfo_;
}

void CreateJobRequest::setContactInfo(const std::vector<ContactInfo>& contactInfo)
{
	contactInfo_ = contactInfo;
	for(int dep1 = 0; dep1!= contactInfo.size(); dep1++) {
		auto contactInfoObj = contactInfo.at(dep1);
		std::string contactInfoObjStr = "ContactInfo." + std::to_string(dep1 + 1);
		setParameter(contactInfoObjStr + ".UserPhone", contactInfoObj.userPhone);
		setParameter(contactInfoObjStr + ".UserName", contactInfoObj.userName);
	}
}

std::string CreateJobRequest::getName()const
{
	return name_;
}

void CreateJobRequest::setName(const std::string& name)
{
	name_ = name;
	setBodyParameter("Name", name);
}

std::string CreateJobRequest::get_Namespace()const
{
	return _namespace_;
}

void CreateJobRequest::set_Namespace(const std::string& _namespace)
{
	_namespace_ = _namespace;
	setBodyParameter("_Namespace", _namespace);
}

int CreateJobRequest::getMaxConcurrency()const
{
	return maxConcurrency_;
}

void CreateJobRequest::setMaxConcurrency(int maxConcurrency)
{
	maxConcurrency_ = maxConcurrency;
	setBodyParameter("MaxConcurrency", std::to_string(maxConcurrency));
}

int CreateJobRequest::getTimeType()const
{
	return timeType_;
}

void CreateJobRequest::setTimeType(int timeType)
{
	timeType_ = timeType;
	setBodyParameter("TimeType", std::to_string(timeType));
}

std::string CreateJobRequest::getParameters()const
{
	return parameters_;
}

void CreateJobRequest::setParameters(const std::string& parameters)
{
	parameters_ = parameters;
	setBodyParameter("Parameters", parameters);
}

