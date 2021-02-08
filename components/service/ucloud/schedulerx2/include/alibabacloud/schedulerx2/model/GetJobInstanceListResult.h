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

#ifndef ALIBABACLOUD_SCHEDULERX2_MODEL_GETJOBINSTANCELISTRESULT_H_
#define ALIBABACLOUD_SCHEDULERX2_MODEL_GETJOBINSTANCELISTRESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/schedulerx2/Schedulerx2Export.h>

namespace AlibabaCloud
{
	namespace Schedulerx2
	{
		namespace Model
		{
			class ALIBABACLOUD_SCHEDULERX2_EXPORT GetJobInstanceListResult : public ServiceResult
			{
			public:
				struct Data
				{
					struct JobInstanceDetailsItem
					{
						int status;
						int triggerType;
						std::string progress;
						std::string endTime;
						long instanceId;
						std::string workAddr;
						std::string startTime;
						std::string dataTime;
						std::string result;
						int timeType;
						std::string executor;
						std::string scheduleTime;
						long jobId;
					};
					std::vector<JobInstanceDetailsItem> jobInstanceDetails;
				};


				GetJobInstanceListResult();
				explicit GetJobInstanceListResult(const std::string &payload);
				~GetJobInstanceListResult();
				std::string getMessage()const;
				Data getData()const;
				int getCode()const;
				bool getSuccess()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::string message_;
				Data data_;
				int code_;
				bool success_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_SCHEDULERX2_MODEL_GETJOBINSTANCELISTRESULT_H_