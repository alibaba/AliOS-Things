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

#ifndef ALIBABACLOUD_FACEBODY_MODEL_FACEMAKEUPREQUEST_H_
#define ALIBABACLOUD_FACEBODY_MODEL_FACEMAKEUPREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/facebody/FacebodyExport.h>

namespace AlibabaCloud
{
	namespace Facebody
	{
		namespace Model
		{
			class ALIBABACLOUD_FACEBODY_EXPORT FaceMakeupRequest : public RpcServiceRequest
			{

			public:
				FaceMakeupRequest();
				~FaceMakeupRequest();

				float getStrength()const;
				void setStrength(float strength);
				std::string getMakeupType()const;
				void setMakeupType(const std::string& makeupType);
				std::string getResourceType()const;
				void setResourceType(const std::string& resourceType);
				std::string getImageURL()const;
				void setImageURL(const std::string& imageURL);

            private:
				float strength_;
				std::string makeupType_;
				std::string resourceType_;
				std::string imageURL_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_FACEBODY_MODEL_FACEMAKEUPREQUEST_H_