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

#ifndef ALIBABACLOUD_ALIYUNCVC_MODEL_LISTCONFERENCEDEVICESRESULT_H_
#define ALIBABACLOUD_ALIYUNCVC_MODEL_LISTCONFERENCEDEVICESRESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/aliyuncvc/AliyuncvcExport.h>

namespace AlibabaCloud
{
	namespace Aliyuncvc
	{
		namespace Model
		{
			class ALIBABACLOUD_ALIYUNCVC_EXPORT ListConferenceDevicesResult : public ServiceResult
			{
			public:
				struct ConferencesDatas
				{
					struct Data
					{
						std::string deviceModel;
						std::string status;
						std::string castScreenCode;
						std::string createTime;
						std::string manufacturer;
						std::string pictureUrl;
						std::string sN;
						std::string conferenceName;
						std::string activationCode;
						std::string conferenceCode;
					};
					std::vector<Data> conferences;
					int pageSize;
					int pageNumber;
					int total;
				};


				ListConferenceDevicesResult();
				explicit ListConferenceDevicesResult(const std::string &payload);
				~ListConferenceDevicesResult();
				ConferencesDatas getConferencesDatas()const;
				std::string getMessage()const;
				int getErrorCode()const;
				bool getSuccess()const;

			protected:
				void parse(const std::string &payload);
			private:
				ConferencesDatas conferencesDatas_;
				std::string message_;
				int errorCode_;
				bool success_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_ALIYUNCVC_MODEL_LISTCONFERENCEDEVICESRESULT_H_