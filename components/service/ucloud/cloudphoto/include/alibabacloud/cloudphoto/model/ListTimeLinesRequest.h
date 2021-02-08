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

#ifndef ALIBABACLOUD_CLOUDPHOTO_MODEL_LISTTIMELINESREQUEST_H_
#define ALIBABACLOUD_CLOUDPHOTO_MODEL_LISTTIMELINESREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/cloudphoto/CloudPhotoExport.h>

namespace AlibabaCloud
{
	namespace CloudPhoto
	{
		namespace Model
		{
			class ALIBABACLOUD_CLOUDPHOTO_EXPORT ListTimeLinesRequest : public RpcServiceRequest
			{

			public:
				ListTimeLinesRequest();
				~ListTimeLinesRequest();

				long getCursor()const;
				void setCursor(long cursor);
				std::string getLibraryId()const;
				void setLibraryId(const std::string& libraryId);
				std::string getStoreName()const;
				void setStoreName(const std::string& storeName);
				std::string getFilterBy()const;
				void setFilterBy(const std::string& filterBy);
				int getPhotoSize()const;
				void setPhotoSize(int photoSize);
				int getTimeLineCount()const;
				void setTimeLineCount(int timeLineCount);
				std::string getTimeLineUnit()const;
				void setTimeLineUnit(const std::string& timeLineUnit);
				std::string getDirection()const;
				void setDirection(const std::string& direction);
				std::string getOrder()const;
				void setOrder(const std::string& order);

            private:
				long cursor_;
				std::string libraryId_;
				std::string storeName_;
				std::string filterBy_;
				int photoSize_;
				int timeLineCount_;
				std::string timeLineUnit_;
				std::string direction_;
				std::string order_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_CLOUDPHOTO_MODEL_LISTTIMELINESREQUEST_H_