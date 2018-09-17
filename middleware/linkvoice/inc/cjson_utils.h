/*
 * cjson_utils.h
 *
 *  Created on: Jan 22, 2017
 *      Author: wj
 */

#ifndef CJSON_UTILS_H_
#define CJSON_UTILS_H_

#include <stdlib.h>
#include <stdarg.h>
#include "cJSON.h"

cJSON* get_service_data(cJSON* service_data, int num, ...);

#endif /* CJSON_UTILS_H_ */
