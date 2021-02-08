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

#ifndef ALIBABACLOUD_IVISION_MODEL_UNREGISTERFACEREQUEST_H_
#define ALIBABACLOUD_IVISION_MODEL_UNREGISTERFACEREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/ivision/IvisionExport.h>

namespace AlibabaCloud
{
	namespace Ivision
	{
		namespace Model
		{
			class ALIBABACLOUD_IVISION_EXPORT UnregisterFaceRequest : public RpcServiceRequest
			{

			public:
				UnregisterFaceRequest();
				~UnregisterFaceRequest();

				std::string getShowLog()const;
				void setShowLog(const std::string& showLog);
				std::string getGroupId()const;
				void setGroupId(const std::string& groupId);
				long getOwnerId()const;
				void setOwnerId(long ownerId);
				std::string getFaceToken()const;
				void setFaceToken(const std::string& faceToken);

            private:
				std::string showLog_;
				std::string groupId_;
				long ownerId_;
				std::string faceToken_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IVISION_MODEL_UNREGISTERFACEREQUEST_H_