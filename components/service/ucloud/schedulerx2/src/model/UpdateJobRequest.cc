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

#include <alibabacloud/schedulerx2/model/UpdateJobRequest.h>

using AlibabaCloud::Schedulerx2::Model::UpdateJobRequest;

UpdateJobRequest::UpdateJobRequest() :
	RpcServiceRequest("schedulerx2", "2019-04-30", "UpdateJob")
{
	setMethod(HttpRequest::Method::Post);
}

UpdateJobRequest::~UpdateJobRequest()
{}

std::string UpdateJobRequest::getNamespaceSource()const
{
	return namespaceSource_;
}

void UpdateJobRequest::setNamespaceSource(const std::string& namespaceSource)
{
	namespaceSource_ = namespaceSource;
	setBodyParameter("NamespaceSource", namespaceSource);
}

std::string UpdateJobRequest::getDescription()const
{
	return description_;
}

void UpdateJobRequest::setDescription(const std::string& description)
{
	description_ = description;
	setBodyParameter("Description", description);
}

int UpdateJobRequest::getAttemptInterval()const
{
	return attemptInterval_;
}

void UpdateJobRequest::setAttemptInterval(int attemptInterval)
{
	attemptInterval_ = attemptInterval;
	setBodyParameter("AttemptInterval", std::to_string(attemptInterval));
}

std::string UpdateJobRequest::getContent()const
{
	return content_;
}

void UpdateJobRequest::setContent(const std::string& content)
{
	content_ = content;
	setBodyParameter("Content", content);
}

long UpdateJobRequest::getTimeout()const
{
	return timeout_;
}

void UpdateJobRequest::setTimeout(long timeout)
{
	timeout_ = timeout;
	setBodyParameter("Timeout", std::to_string(timeout));
}

bool UpdateJobRequest::getTimeoutKillEnable()const
{
	return timeoutKillEnable_;
}

void UpdateJobRequest::setTimeoutKillEnable(bool timeoutKillEnable)
{
	timeoutKillEnable_ = timeoutKillEnable;
	setBodyParameter("TimeoutKillEnable", timeoutKillEnable ? "true" : "false");
}

long UpdateJobRequest::getJobId()const
{
	return jobId_;
}

void UpdateJobRequest::setJobId(long jobId)
{
	jobId_ = jobId;
	setBodyParameter("JobId", std::to_string(jobId));
}

int UpdateJobRequest::getPageSize()const
{
	return pageSize_;
}

void UpdateJobRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setBodyParameter("PageSize", std::to_string(pageSize));
}

int UpdateJobRequest::getConsumerSize()const
{
	return consumerSize_;
}

void UpdateJobRequest::setConsumerSize(int consumerSize)
{
	consumerSize_ = consumerSize;
	setBodyParameter("ConsumerSize", std::to_string(consumerSize));
}

std::string UpdateJobRequest::getJarUrl()const
{
	return jarUrl_;
}

void UpdateJobRequest::setJarUrl(const std::string& jarUrl)
{
	jarUrl_ = jarUrl;
	setBodyParameter("JarUrl", jarUrl);
}

std::string UpdateJobRequest::getCalendar()const
{
	return calendar_;
}

void UpdateJobRequest::setCalendar(const std::string& calendar)
{
	calendar_ = calendar;
	setBodyParameter("Calendar", calendar);
}

bool UpdateJobRequest::getFailEnable()const
{
	return failEnable_;
}

void UpdateJobRequest::setFailEnable(bool failEnable)
{
	failEnable_ = failEnable;
	setBodyParameter("FailEnable", failEnable ? "true" : "false");
}

std::string UpdateJobRequest::getSendChannel()const
{
	return sendChannel_;
}

void UpdateJobRequest::setSendChannel(const std::string& sendChannel)
{
	sendChannel_ = sendChannel;
	setBodyParameter("SendChannel", sendChannel);
}

int UpdateJobRequest::getDataOffset()const
{
	return dataOffset_;
}

void UpdateJobRequest::setDataOffset(int dataOffset)
{
	dataOffset_ = dataOffset;
	setBodyParameter("DataOffset", std::to_string(dataOffset));
}

std::string UpdateJobRequest::getGroupId()const
{
	return groupId_;
}

void UpdateJobRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setBodyParameter("GroupId", groupId);
}

int UpdateJobRequest::getTaskMaxAttempt()const
{
	return taskMaxAttempt_;
}

void UpdateJobRequest::setTaskMaxAttempt(int taskMaxAttempt)
{
	taskMaxAttempt_ = taskMaxAttempt;
	setBodyParameter("TaskMaxAttempt", std::to_string(taskMaxAttempt));
}

int UpdateJobRequest::getMaxAttempt()const
{
	return maxAttempt_;
}

void UpdateJobRequest::setMaxAttempt(int maxAttempt)
{
	maxAttempt_ = maxAttempt;
	setBodyParameter("MaxAttempt", std::to_string(maxAttempt));
}

int UpdateJobRequest::getDispatcherSize()const
{
	return dispatcherSize_;
}

void UpdateJobRequest::setDispatcherSize(int dispatcherSize)
{
	dispatcherSize_ = dispatcherSize;
	setBodyParameter("DispatcherSize", std::to_string(dispatcherSize));
}

int UpdateJobRequest::getTaskAttemptInterval()const
{
	return taskAttemptInterval_;
}

void UpdateJobRequest::setTaskAttemptInterval(int taskAttemptInterval)
{
	taskAttemptInterval_ = taskAttemptInterval;
	setBodyParameter("TaskAttemptInterval", std::to_string(taskAttemptInterval));
}

std::string UpdateJobRequest::getExecuteMode()const
{
	return executeMode_;
}

void UpdateJobRequest::setExecuteMode(const std::string& executeMode)
{
	executeMode_ = executeMode;
	setBodyParameter("ExecuteMode", executeMode);
}

int UpdateJobRequest::getQueueSize()const
{
	return queueSize_;
}

void UpdateJobRequest::setQueueSize(int queueSize)
{
	queueSize_ = queueSize;
	setBodyParameter("QueueSize", std::to_string(queueSize));
}

std::string UpdateJobRequest::getTimeExpression()const
{
	return timeExpression_;
}

void UpdateJobRequest::setTimeExpression(const std::string& timeExpression)
{
	timeExpression_ = timeExpression;
	setBodyParameter("TimeExpression", timeExpression);
}

std::string UpdateJobRequest::getClassName()const
{
	return className_;
}

void UpdateJobRequest::setClassName(const std::string& className)
{
	className_ = className;
	setBodyParameter("ClassName", className);
}

bool UpdateJobRequest::getTimeoutEnable()const
{
	return timeoutEnable_;
}

void UpdateJobRequest::setTimeoutEnable(bool timeoutEnable)
{
	timeoutEnable_ = timeoutEnable;
	setBodyParameter("TimeoutEnable", timeoutEnable ? "true" : "false");
}

std::vector<UpdateJobRequest::ContactInfo> UpdateJobRequest::getContactInfo()const
{
	return contactInfo_;
}

void UpdateJobRequest::setContactInfo(const std::vector<ContactInfo>& contactInfo)
{
	contactInfo_ = contactInfo;
	for(int dep1 = 0; dep1!= contactInfo.size(); dep1++) {
		auto contactInfoObj = contactInfo.at(dep1);
		std::string contactInfoObjStr = "ContactInfo." + std::to_string(dep1 + 1);
		setParameter(contactInfoObjStr + ".UserPhone", contactInfoObj.userPhone);
		setParameter(contactInfoObjStr + ".UserName", contactInfoObj.userName);
	}
}

std::string UpdateJobRequest::getName()const
{
	return name_;
}

void UpdateJobRequest::setName(const std::string& name)
{
	name_ = name;
	setBodyParameter("Name", name);
}

std::string UpdateJobRequest::get_Namespace()const
{
	return _namespace_;
}

void UpdateJobRequest::set_Namespace(const std::string& _namespace)
{
	_namespace_ = _namespace;
	setBodyParameter("_Namespace", _namespace);
}

int UpdateJobRequest::getMaxConcurrency()const
{
	return maxConcurrency_;
}

void UpdateJobRequest::setMaxConcurrency(int maxConcurrency)
{
	maxConcurrency_ = maxConcurrency;
	setBodyParameter("MaxConcurrency", std::to_string(maxConcurrency));
}

int UpdateJobRequest::getTimeType()const
{
	return timeType_;
}

void UpdateJobRequest::setTimeType(int timeType)
{
	timeType_ = timeType;
	setBodyParameter("TimeType", std::to_string(timeType));
}

std::string UpdateJobRequest::getParameters()const
{
	return parameters_;
}

void UpdateJobRequest::setParameters(const std::string& parameters)
{
	parameters_ = parameters;
	setBodyParameter("Parameters", parameters);
}

