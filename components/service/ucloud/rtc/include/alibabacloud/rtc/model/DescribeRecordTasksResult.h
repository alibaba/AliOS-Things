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

#ifndef ALIBABACLOUD_RTC_MODEL_DESCRIBERECORDTASKSRESULT_H_
#define ALIBABACLOUD_RTC_MODEL_DESCRIBERECORDTASKSRESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/rtc/RtcExport.h>

namespace AlibabaCloud
{
	namespace Rtc
	{
		namespace Model
		{
			class ALIBABACLOUD_RTC_EXPORT DescribeRecordTasksResult : public ServiceResult
			{
			public:
				struct RecordTask
				{
					struct UserPanesItem
					{
						std::string userId;
						int paneId;
						std::string source;
					};
					int status;
					std::vector<RecordTask::UserPanesItem> userPanes;
					std::string taskId;
					std::string appId;
					std::vector<std::string> subSpecUsers;
					std::string createTime;
					std::string channelId;
					std::string templateId;
				};


				DescribeRecordTasksResult();
				explicit DescribeRecordTasksResult(const std::string &payload);
				~DescribeRecordTasksResult();
				std::vector<RecordTask> getRecordTasks()const;
				long getTotalNum()const;
				long getTotalPage()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::vector<RecordTask> recordTasks_;
				long totalNum_;
				long totalPage_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_RTC_MODEL_DESCRIBERECORDTASKSRESULT_H_