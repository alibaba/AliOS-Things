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

#ifndef ALIBABACLOUD_CLOUDPHOTO_MODEL_LISTEVENTSREQUEST_H_
#define ALIBABACLOUD_CLOUDPHOTO_MODEL_LISTEVENTSREQUEST_H_

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
			class ALIBABACLOUD_CLOUDPHOTO_EXPORT ListEventsRequest : public RpcServiceRequest
			{

			public:
				ListEventsRequest();
				~ListEventsRequest();

				std::string getCursor()const;
				void setCursor(const std::string& cursor);
				std::string getStoreName()const;
				void setStoreName(const std::string& storeName);
				int getSize()const;
				void setSize(int size);
				std::string getState()const;
				void setState(const std::string& state);
				std::string getDirection()const;
				void setDirection(const std::string& direction);

            private:
				std::string cursor_;
				std::string storeName_;
				int size_;
				std::string state_;
				std::string direction_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_CLOUDPHOTO_MODEL_LISTEVENTSREQUEST_H_