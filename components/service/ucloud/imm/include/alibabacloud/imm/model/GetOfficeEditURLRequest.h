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

#ifndef ALIBABACLOUD_IMM_MODEL_GETOFFICEEDITURLREQUEST_H_
#define ALIBABACLOUD_IMM_MODEL_GETOFFICEEDITURLREQUEST_H_

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
			class ALIBABACLOUD_IMM_EXPORT GetOfficeEditURLRequest : public RpcServiceRequest
			{

			public:
				GetOfficeEditURLRequest();
				~GetOfficeEditURLRequest();

				std::string getSrcType()const;
				void setSrcType(const std::string& srcType);
				std::string getProject()const;
				void setProject(const std::string& project);
				std::string getUserID()const;
				void setUserID(const std::string& userID);
				std::string getAccessKeyId()const;
				void setAccessKeyId(const std::string& accessKeyId);
				std::string getNotifyEndpoint()const;
				void setNotifyEndpoint(const std::string& notifyEndpoint);
				std::string getFileID()const;
				void setFileID(const std::string& fileID);
				std::string getNotifyTopicName()const;
				void setNotifyTopicName(const std::string& notifyTopicName);
				std::string getFileName()const;
				void setFileName(const std::string& fileName);
				std::string getSrcUri()const;
				void setSrcUri(const std::string& srcUri);
				std::string getTgtUri()const;
				void setTgtUri(const std::string& tgtUri);
				std::string getUserName()const;
				void setUserName(const std::string& userName);

            private:
				std::string srcType_;
				std::string project_;
				std::string userID_;
				std::string accessKeyId_;
				std::string notifyEndpoint_;
				std::string fileID_;
				std::string notifyTopicName_;
				std::string fileName_;
				std::string srcUri_;
				std::string tgtUri_;
				std::string userName_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IMM_MODEL_GETOFFICEEDITURLREQUEST_H_