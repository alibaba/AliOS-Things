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

#ifndef ALIBABACLOUD_IMM_MODEL_GETVIDEOTASKRESULT_H_
#define ALIBABACLOUD_IMM_MODEL_GETVIDEOTASKRESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/imm/ImmExport.h>

namespace AlibabaCloud
{
	namespace Imm
	{
		namespace Model
		{
			class ALIBABACLOUD_IMM_EXPORT GetVideoTaskResult : public ServiceResult
			{
			public:


				GetVideoTaskResult();
				explicit GetVideoTaskResult(const std::string &payload);
				~GetVideoTaskResult();
				std::string getStatus()const;
				std::string getNotifyEndpoint()const;
				int getProgress()const;
				std::string getTaskId()const;
				std::string getParameters()const;
				std::string getEndTime()const;
				std::string getTaskType()const;
				std::string getStartTime()const;
				std::string getNotifyTopicName()const;
				std::string getErrorMessage()const;
				std::string getResult()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::string status_;
				std::string notifyEndpoint_;
				int progress_;
				std::string taskId_;
				std::string parameters_;
				std::string endTime_;
				std::string taskType_;
				std::string startTime_;
				std::string notifyTopicName_;
				std::string errorMessage_;
				std::string result_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IMM_MODEL_GETVIDEOTASKRESULT_H_