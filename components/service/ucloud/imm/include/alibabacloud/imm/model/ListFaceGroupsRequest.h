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

#ifndef ALIBABACLOUD_IMM_MODEL_LISTFACEGROUPSREQUEST_H_
#define ALIBABACLOUD_IMM_MODEL_LISTFACEGROUPSREQUEST_H_

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
			class ALIBABACLOUD_IMM_EXPORT ListFaceGroupsRequest : public RpcServiceRequest
			{

			public:
				ListFaceGroupsRequest();
				~ListFaceGroupsRequest();

				std::string getProject()const;
				void setProject(const std::string& project);
				std::string getRemarksBQuery()const;
				void setRemarksBQuery(const std::string& remarksBQuery);
				std::string getExternalId()const;
				void setExternalId(const std::string& externalId);
				std::string getAccessKeyId()const;
				void setAccessKeyId(const std::string& accessKeyId);
				int getLimit()const;
				void setLimit(int limit);
				std::string getRemarksArrayBQuery()const;
				void setRemarksArrayBQuery(const std::string& remarksArrayBQuery);
				std::string getOrder()const;
				void setOrder(const std::string& order);
				std::string getRemarksAQuery()const;
				void setRemarksAQuery(const std::string& remarksAQuery);
				std::string getOrderBy()const;
				void setOrderBy(const std::string& orderBy);
				std::string getRemarksDQuery()const;
				void setRemarksDQuery(const std::string& remarksDQuery);
				std::string getRemarksArrayAQuery()const;
				void setRemarksArrayAQuery(const std::string& remarksArrayAQuery);
				std::string getMarker()const;
				void setMarker(const std::string& marker);
				std::string getSetId()const;
				void setSetId(const std::string& setId);
				std::string getRemarksCQuery()const;
				void setRemarksCQuery(const std::string& remarksCQuery);

            private:
				std::string project_;
				std::string remarksBQuery_;
				std::string externalId_;
				std::string accessKeyId_;
				int limit_;
				std::string remarksArrayBQuery_;
				std::string order_;
				std::string remarksAQuery_;
				std::string orderBy_;
				std::string remarksDQuery_;
				std::string remarksArrayAQuery_;
				std::string marker_;
				std::string setId_;
				std::string remarksCQuery_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IMM_MODEL_LISTFACEGROUPSREQUEST_H_