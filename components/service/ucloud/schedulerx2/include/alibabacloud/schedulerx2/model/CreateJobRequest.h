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

#ifndef ALIBABACLOUD_SCHEDULERX2_MODEL_CREATEJOBREQUEST_H_
#define ALIBABACLOUD_SCHEDULERX2_MODEL_CREATEJOBREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/schedulerx2/Schedulerx2Export.h>

namespace AlibabaCloud
{
	namespace Schedulerx2
	{
		namespace Model
		{
			class ALIBABACLOUD_SCHEDULERX2_EXPORT CreateJobRequest : public RpcServiceRequest
			{
			public:
				struct ContactInfo
				{
					std::string userPhone;
					std::string userName;
				};

			public:
				CreateJobRequest();
				~CreateJobRequest();

				std::string getNamespaceSource()const;
				void setNamespaceSource(const std::string& namespaceSource);
				std::string getDescription()const;
				void setDescription(const std::string& description);
				int getAttemptInterval()const;
				void setAttemptInterval(int attemptInterval);
				std::string getContent()const;
				void setContent(const std::string& content);
				long getTimeout()const;
				void setTimeout(long timeout);
				bool getTimeoutKillEnable()const;
				void setTimeoutKillEnable(bool timeoutKillEnable);
				int getPageSize()const;
				void setPageSize(int pageSize);
				int getConsumerSize()const;
				void setConsumerSize(int consumerSize);
				std::string getJarUrl()const;
				void setJarUrl(const std::string& jarUrl);
				std::string getCalendar()const;
				void setCalendar(const std::string& calendar);
				bool getFailEnable()const;
				void setFailEnable(bool failEnable);
				std::string getSendChannel()const;
				void setSendChannel(const std::string& sendChannel);
				int getDataOffset()const;
				void setDataOffset(int dataOffset);
				std::string getGroupId()const;
				void setGroupId(const std::string& groupId);
				int getTaskMaxAttempt()const;
				void setTaskMaxAttempt(int taskMaxAttempt);
				int getMaxAttempt()const;
				void setMaxAttempt(int maxAttempt);
				int getDispatcherSize()const;
				void setDispatcherSize(int dispatcherSize);
				std::string getJobType()const;
				void setJobType(const std::string& jobType);
				int getTaskAttemptInterval()const;
				void setTaskAttemptInterval(int taskAttemptInterval);
				std::string getExecuteMode()const;
				void setExecuteMode(const std::string& executeMode);
				int getQueueSize()const;
				void setQueueSize(int queueSize);
				std::string getTimeExpression()const;
				void setTimeExpression(const std::string& timeExpression);
				std::string getClassName()const;
				void setClassName(const std::string& className);
				bool getTimeoutEnable()const;
				void setTimeoutEnable(bool timeoutEnable);
				std::vector<ContactInfo> getContactInfo()const;
				void setContactInfo(const std::vector<ContactInfo>& contactInfo);
				std::string getName()const;
				void setName(const std::string& name);
				std::string get_Namespace()const;
				void set_Namespace(const std::string& _namespace);
				int getMaxConcurrency()const;
				void setMaxConcurrency(int maxConcurrency);
				int getTimeType()const;
				void setTimeType(int timeType);
				std::string getParameters()const;
				void setParameters(const std::string& parameters);

            private:
				std::string namespaceSource_;
				std::string description_;
				int attemptInterval_;
				std::string content_;
				long timeout_;
				bool timeoutKillEnable_;
				int pageSize_;
				int consumerSize_;
				std::string jarUrl_;
				std::string calendar_;
				bool failEnable_;
				std::string sendChannel_;
				int dataOffset_;
				std::string groupId_;
				int taskMaxAttempt_;
				int maxAttempt_;
				int dispatcherSize_;
				std::string jobType_;
				int taskAttemptInterval_;
				std::string executeMode_;
				int queueSize_;
				std::string timeExpression_;
				std::string className_;
				bool timeoutEnable_;
				std::vector<ContactInfo> contactInfo_;
				std::string name_;
				std::string _namespace_;
				int maxConcurrency_;
				int timeType_;
				std::string parameters_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_SCHEDULERX2_MODEL_CREATEJOBREQUEST_H_