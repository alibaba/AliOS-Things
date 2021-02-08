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

#ifndef ALIBABACLOUD_LIVE_MODEL_DESCRIBERECORDRESULT_H_
#define ALIBABACLOUD_LIVE_MODEL_DESCRIBERECORDRESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/live/LiveExport.h>

namespace AlibabaCloud
{
	namespace Live
	{
		namespace Model
		{
			class ALIBABACLOUD_LIVE_EXPORT DescribeRecordResult : public ServiceResult
			{
			public:


				DescribeRecordResult();
				explicit DescribeRecordResult(const std::string &payload);
				~DescribeRecordResult();
				int getBoardId()const;
				std::string getAppId()const;
				long getRecordStartTime()const;
				long getEndTime()const;
				std::string getOssEndpoint()const;
				int getState()const;
				std::string getOssPath()const;
				long getStartTime()const;
				std::string getOssBucket()const;
				std::string getRecordId()const;

			protected:
				void parse(const std::string &payload);
			private:
				int boardId_;
				std::string appId_;
				long recordStartTime_;
				long endTime_;
				std::string ossEndpoint_;
				int state_;
				std::string ossPath_;
				long startTime_;
				std::string ossBucket_;
				std::string recordId_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_LIVE_MODEL_DESCRIBERECORDRESULT_H_