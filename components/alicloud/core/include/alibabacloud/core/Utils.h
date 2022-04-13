/*
 * Copyright 1999-2019 Alibaba Cloud All rights reserved.
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

#ifndef CORE_SRC_UTILS_H_
#define CORE_SRC_UTILS_H_

#include <alibabacloud/core/HttpMessage.h>
#include <alibabacloud/core/HttpRequest.h>
//#include <json/json.h>
#include <map>
#include <string>

namespace AlibabaCloud {
std::string ComputeContentMD5(const char *data, size_t size);
std::string GenerateUuid();
std::string HttpMethodToString(HttpRequest::Method method);
void StringReplace(std::string &src, const std::string &s1,
                   const std::string &s2);
std::string UrlEncode(const std::string &src);
std::string UrlDecode(const std::string &src);
std::string
canonicalizedQuery(const std::map<std::string, std::string> &params);
std::string canonicalizedHeaders(const HttpMessage::HeaderCollection &headers);
std::string GetEnv(const std::string env);
std::string MapToJson(const std::map<std::string, std::string> &maps);
std::map<std::string, std::string> JsonToMap(const std::string &json);
} // namespace AlibabaCloud
#endif // CORE_SRC_UTILS_H_
