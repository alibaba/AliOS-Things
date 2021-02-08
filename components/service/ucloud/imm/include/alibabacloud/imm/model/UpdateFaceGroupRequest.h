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

#ifndef ALIBABACLOUD_IMM_MODEL_UPDATEFACEGROUPREQUEST_H_
#define ALIBABACLOUD_IMM_MODEL_UPDATEFACEGROUPREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/imm/ImmExport.h>

namespace AlibabaCloud
{
	namespace Imm
	{
		namespace Model
		{
			class ALIBABACLOUD_IMM_EXPORT UpdateFaceGroupRequest : public RpcServiceRequest
			{

			public:
				UpdateFaceGroupRequest();
				~UpdateFaceGroupRequest();

				std::string getProject()const;
				void setProject(const std::string& project);
				std::string getExternalId()const;
				void setExternalId(const std::string& externalId);
				std::string getAccessKeyId()const;
				void setAccessKeyId(const std::string& accessKeyId);
				std::string getGroupId()const;
				void setGroupId(const std::string& groupId);
				std::string getRemarksB()const;
				void setRemarksB(const std::string& remarksB);
				std::string getRemarksA()const;
				void setRemarksA(const std::string& remarksA);
				std::string getGroupName()const;
				void setGroupName(const std::string& groupName);
				std::string getRemarksArrayA()const;
				void setRemarksArrayA(const std::string& remarksArrayA);
				std::string getRemarksArrayB()const;
				void setRemarksArrayB(const std::string& remarksArrayB);
				std::string getRemarksD()const;
				void setRemarksD(const std::string& remarksD);
				std::string getRemarksC()const;
				void setRemarksC(const std::string& remarksC);
				std::string getSetId()const;
				void setSetId(const std::string& setId);
				std::string getGroupCoverFaceId()const;
				void setGroupCoverFaceId(const std::string& groupCoverFaceId);

            private:
				std::string project_;
				std::string externalId_;
				std::string accessKeyId_;
				std::string groupId_;
				std::string remarksB_;
				std::string remarksA_;
				std::string groupName_;
				std::string remarksArrayA_;
				std::string remarksArrayB_;
				std::string remarksD_;
				std::string remarksC_;
				std::string setId_;
				std::string groupCoverFaceId_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IMM_MODEL_UPDATEFACEGROUPREQUEST_H_