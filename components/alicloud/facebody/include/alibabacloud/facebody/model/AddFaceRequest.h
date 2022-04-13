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

#ifndef ALIBABACLOUD_FACEBODY_MODEL_ADDFACEREQUEST_H_
#define ALIBABACLOUD_FACEBODY_MODEL_ADDFACEREQUEST_H_

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
			class ALIBABACLOUD_FACEBODY_EXPORT AddFaceRequest : public RpcServiceRequest
			{

			public:
				AddFaceRequest();
				~AddFaceRequest();

				std::string getEntityId()const;
				void setEntityId(const std::string& entityId);
				std::string getDbName()const;
				void setDbName(const std::string& dbName);
				std::string getImageUrl()const;
				void setImageUrl(const std::string& imageUrl);
				std::string getExtraData()const;
				void setExtraData(const std::string& extraData);

            private:
				std::string entityId_;
				std::string dbName_;
				std::string imageUrl_;
				std::string extraData_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_FACEBODY_MODEL_ADDFACEREQUEST_H_