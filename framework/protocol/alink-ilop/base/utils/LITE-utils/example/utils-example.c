/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


#include "lite-utils.h"

int main(void)
{
    LITE_openlog("utils");
    LITE_set_loglevel(LOG_DEBUG_LEVEL);

    demo_string_utils();
    demo_json_token();
    demo_json_parser();
    demo_json_array_token();
    demo_json_array_parse();

    LITE_closelog();
    return 0;
}
